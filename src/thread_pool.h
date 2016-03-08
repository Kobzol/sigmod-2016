#pragma once

#include <thread>
#include <iostream>
#include <condition_variable>

#include "graph.h"
#include "job.h"
#include "vector.h"
#include "settings.h"
#include "thread.h"

extern Graph graph;

class ThreadPool
{
public:
    ThreadPool()
    {
        for (size_t i = 0; i < THREAD_POOL_THREAD_COUNT; i++)
        {
            this->threads[i].id = i + 1;
        }
    }

    void reset_jobs()
    {
        this->jobs = nullptr;
        for (size_t i = 0; i < THREAD_POOL_THREAD_COUNT; i++)
        {
            this->threads[i].batch--;
        }
    }

    void terminate()
    {
        for (size_t i = 0; i < THREAD_POOL_THREAD_COUNT; i++)
        {
            this->threads[i].terminate();
        }
    }
    void join()
    {
        for (size_t i = 0; i < THREAD_POOL_THREAD_COUNT; i++)
        {
            this->threads[i].join();
        }
    }

    std::mutex jobLock;
    std::condition_variable jobCV;
    std::vector<Job>* jobs = nullptr;
    Thread threads[THREAD_POOL_THREAD_COUNT];
};
