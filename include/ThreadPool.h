#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>

class ThreadPool {
private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    std::mutex queue_mutex;
    std::condition_variable condition;

    bool stop;

public:
    ThreadPool(size_t number_of_threads)
        : stop(false) {

        for (size_t i = 0; i < number_of_threads; i++) {

            workers.emplace_back([this]() {

                while (true) {
                    std::function<void()> task;

                    {
                        std::unique_lock<std::mutex> lock(queue_mutex);

                        condition.wait(
                            lock,
                            [this]() {
                                return stop || !tasks.empty();
                            }
                        );

                        if (stop && tasks.empty()) {
                            return;
                        }

                        task = std::move(tasks.front());
                        tasks.pop();
                    }

                    task();
                }
            });
        }
    }

    void addTask(std::function<void()> task) {
        {
            std::lock_guard<std::mutex> lock(queue_mutex);

            tasks.push(std::move(task));
        }

        condition.notify_one();
    }

    ~ThreadPool() {
        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            stop = true;
        }

        condition.notify_all();

        for (std::thread& worker : workers) {
            worker.join();
        }
    }
};

#endif