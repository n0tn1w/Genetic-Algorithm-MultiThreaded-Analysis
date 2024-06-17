#include "ThreadPool.h"

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

ThreadPool::~ThreadPool() {
    shutdown();
}

