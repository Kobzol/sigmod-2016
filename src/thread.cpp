#include <cmath>
#include "thread.h"
#include "thread_pool.h"

void Thread::thread_fn()
{
    while (!this->terminated)
    {
        std::unique_lock<std::mutex> lock(threadPool.jobLock);

        while (threadPool.jobs == nullptr || threadPool.jobs->size() == 0 || this->jobsCompleted > 0)
        {
            threadPool.jobCV.wait(lock);
        }

        lock.unlock();

        size_t part = (size_t) std::ceil(threadPool.jobs->size() / (double)(THREAD_POOL_THREAD_COUNT));
        size_t start = (this->id - 1) * part;
        size_t end = std::min(threadPool.jobs->size(), part + start);

        for (size_t i = start; i < end; i++)
        {
            Job& job = threadPool.jobs->at(i);
            this->results.push_back(GraphEvaluator::query(job.from, job.to, job.id, this->id));
        }

        if (batch_id % (TIMESTAMP_NORMALIZE_RATE) == 0)
        {
            part = graph.nodes.size();
            start = (this->id - 1) * part;
            end = std::min(graph.nodes.size(), part + start);

            for (size_t v = start; v < end; v++)
            {
                std::vector<Edge>& edges = graph.nodes.at(v).edges_out;
                for (size_t i = 0; i < edges.size(); i++)
                {
                    if (edges.at(i).to != (size_t) -1)
                    {
                        edges.erase(edges.begin() + i);
                        i--;
                    }
                }
            }
        }

        this->jobsCompleted++;
    }
}
