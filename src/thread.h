#pragma once

#include <thread>
#include <mutex>
#include <vector>

class ThreadPool;
extern ThreadPool threadPool;
extern size_t batch_id;

class Thread
{
public:
    Thread() : id(0), terminated(false), handle(std::thread(&Thread::thread_fn, this))
    {
        this->handle.detach();
    }

    void thread_fn();
    void reset()
    {
        this->jobsCompleted = 0;
        this->results.clear();
    }

    void terminate()
    {
        this->terminated = true;
    }
    void join()
    {
        this->handle.join();
    }

    size_t id;
    size_t jobsCompleted = 0;
    std::vector<int64_t> results;

private:
    bool terminated;
    std::thread handle;
};
