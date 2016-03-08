#pragma once

#include <thread>
#include <mutex>
#include <vector>
#include <atomic>

class ThreadPool;
extern ThreadPool threadPool;
extern size_t batch_id;

class Thread
{
public:
    Thread() : id(0), terminated(false), handle(std::thread(&Thread::thread_fn, this))
    {
        this->batch.store(0);
        this->handle.detach();
    }

    void thread_fn();

    void terminate()
    {
        this->terminated = true;
    }
    void join()
    {
        this->handle.join();
    }

    size_t id;
    std::atomic<int> batch;

private:
    bool terminated;
    std::thread handle;
};
