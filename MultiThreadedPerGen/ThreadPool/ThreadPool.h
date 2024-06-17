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
    ~ThreadPool();
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
            {
                std::lock_guard<std::mutex> lock(queue_mutex_); // Acquire lock briefly to update active_tasks_
                active_tasks_--;
            }
            completion_cv_.notify_one(); // Notify completion after releasing queue_mutex_
        });
        active_tasks_++;
    }
    cv_.notify_one(); // Notify new task after releasing queue_mutex_
}