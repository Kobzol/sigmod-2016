#pragma once

#include <thread>
#include <iostream>

#include "graph.h"
#include "job.h"
#include "vector.h"
#include "settings.h"
#include "evaluator.h"

extern JobQueue jobQueue;
extern Graph graph;

class Thread
{
public:
    Thread() : id(0), terminated(false), handle(std::thread(&Thread::thread_fn, this))
    {
        this->handle.detach();
    }

    void thread_fn()
    {
        while (!this->terminated)
        {
            //std::cerr << "Thread " << this->id << " waiting for job" << std::endl;
            Job job = jobQueue.pop_job();
            //std::cerr << "Thread " << this->id << " got job" << std::endl;

#ifndef THREAD_USE_JOB_SYNC
            if (job.type == JobType::Invalid)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                continue;
            }
#endif

            int64_t result = 0;
            if (job.type == JobType::AddEdge)
            {
                graph.add_edge(job.from, job.to);
            }
            else if (job.type == JobType::RemoveEdge)
            {
                graph.remove_edge(job.from, job.to);
            }
            else
            {
                result = GraphEvaluator::query(job.from, job.to);
            }

            //std::cerr << "Thread " << this->id << " giving job" << std::endl;
            jobQueue.add_result(job, result);
            //std::cerr << "Thread " << this->id << " gave job" << std::endl;
        }
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

private:
    bool terminated;
    std::thread handle;
};

class ThreadPool
{
public:
    ThreadPool()
    {
        for (int i = 0; i < THREAD_POOL_THREAD_COUNT; i++)
        {
            this->threads[i].id = i + 1;
        }
    }

    void terminate()
    {
        for (int i = 0; i < THREAD_POOL_THREAD_COUNT; i++)
        {
            this->threads[i].terminate();
        }
    }
    void join()
    {
        for (int i = 0; i < THREAD_POOL_THREAD_COUNT; i++)
        {
            this->threads[i].join();
        }
    }

private:
    Thread threads[THREAD_POOL_THREAD_COUNT];
};