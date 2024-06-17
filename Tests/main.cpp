#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include <atomic>

class ThreadPool {
public:
    // Initialize the thread pool with a given number of threads
    static void initialize(size_t num_threads = std::thread::hardware_concurrency());

    // Shut down the thread pool
    static void shutdown();

    // Enqueue task for execution by the thread pool
    template <typename Func, typename... Args>
    static void enqueue(Func&& func, Args&&... args);

    // Wait until all tasks have been completed
    static void waitForCompletion();

private:
    // Vector to store worker threads
    static std::vector<std::thread> threads_;

    // Queue of tasks
    static std::queue<std::function<void()>> tasks_;

    // Mutex to synchronize access to shared data
    static std::mutex queue_mutex_;

    // Condition variable to signal changes in the state of the tasks queue
    static std::condition_variable cv_;

    // Condition variable to signal when all tasks are completed
    static std::condition_variable completion_cv_;

    // Atomic counter to track the number of active tasks
    static std::atomic<size_t> active_tasks_;

    // Flag to indicate whether the thread pool should stop or not
    static bool stop_;
};

template <typename Func, typename... Args>
void ThreadPool::enqueue(Func&& func, Args&&... args) {
    {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        tasks_.emplace([func = std::forward<Func>(func), ... args = std::forward<Args>(args)]() mutable {
            func(args...);
            active_tasks_--;
            completion_cv_.notify_one();
        });
        active_tasks_++;
    }
    cv_.notify_one();
}

// Define static members
std::vector<std::thread> ThreadPool::threads_;
std::queue<std::function<void()>> ThreadPool::tasks_;
std::mutex ThreadPool::queue_mutex_;
std::condition_variable ThreadPool::cv_;
std::condition_variable ThreadPool::completion_cv_;
std::atomic<size_t> ThreadPool::active_tasks_{0};
bool ThreadPool::stop_ = false;

void ThreadPool::initialize(size_t num_threads) {
    // Prevent re-initialization if already initialized
    if (!threads_.empty()) return;

    stop_ = false;
    // Creating worker threads
    for (size_t i = 0; i < num_threads; ++i) {
        threads_.emplace_back([] {
            while (true) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(queue_mutex_);
                    cv_.wait(lock, [] {
                        return !tasks_.empty() || stop_;
                    });

                    if (stop_ && tasks_.empty()) {
                        return;
                    }

                    task = std::move(tasks_.front());
                    tasks_.pop();
                }

                task();
            }
        });
    }
}

void ThreadPool::shutdown() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        stop_ = true;
    }

    cv_.notify_all();

    for (auto& thread : threads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    threads_.clear();
}

void ThreadPool::waitForCompletion() {
    std::unique_lock<std::mutex> lock(queue_mutex_);
    completion_cv_.wait(lock, [] {
        return active_tasks_ == 0;
    });
}
#include <iostream>

void printTask(int id) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::cout << "Task " << id << " is printing message " << std::endl;
    
}

int main() {
    // Initialize thread pool with 4 threads
    ThreadPool::initialize(4);

    // Enqueue 4 print tasks
    for (int i = 0; i < 4; ++i) {
        ThreadPool::enqueue(printTask, i);
    }

    // Wait for all tasks to complete
    ThreadPool::waitForCompletion();

    // Shut down the thread pool
    ThreadPool::shutdown();

    std::cout << "All tasks completed and thread pool shut down." << std::endl;
    return 0;
}
