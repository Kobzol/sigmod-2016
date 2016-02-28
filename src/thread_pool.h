#pragma once

#include <thread>

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
    Thread() : terminated(false), handle(std::thread(&Thread::thread_fn, this))
    {

    }

    void thread_fn()
    {
        while (!this->terminated)
        {
            Job job = jobQueue.pop_job();

            if (job.type != JobType::Invalid)
            {
                this->compute_job(job);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    void compute_job(Job& job)
    {
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

        jobQueue.add_result(job, result);
    }

    void terminate()
    {
        this->terminated = true;
    }
    void join()
    {
        this->handle.join();
    }

private:
    bool terminated;
    std::thread handle;
};

class ThreadPool
{
public:
    ThreadPool()
    {

    }

    void stop()
    {
        for (int i = 0; i < THREAD_POOL_THREAD_COUNT; i++)
        {
            this->threads[i].terminate();
        }

        for (int i = 0; i < THREAD_POOL_THREAD_COUNT; i++)
        {
            this->threads[i].join();
        }
    }

private:
    Thread threads[THREAD_POOL_THREAD_COUNT];
};