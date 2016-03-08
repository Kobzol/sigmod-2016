#include <cmath>
#include "thread.h"
#include "thread_pool.h"

void Thread::thread_fn()
{
    while (!this->terminated)
    {
        std::unique_lock<std::mutex> lock(threadPool.jobLock);

        while (threadPool.jobs == nullptr || threadPool.jobs->size() == 0 || this->batch > 0)
        {
            threadPool.jobCV.wait(lock);
        }

        lock.unlock();

        int size = (int) threadPool.jobs->size();

#ifdef LOAD_BALANCER_TWO_SIDE
        int start = 0;
        int end = size;
        int increment = 1;

        if (this->id % 2 == 0)
        {
            start = size - 1;
            end = -1;
            increment = -1;
        }

        for (int i = start; i != end; i += increment)
#else
        for (int i = 0; i < size; i++)
#endif
        {
            Job& job = threadPool.jobs->at((size_t) i);
            if (!job.available.test_and_set())
            {
                job.result = GraphEvaluator::query(job.from, job.to, job.id, this->id);
            }
        }

        if (this->batch == 0)
        {
            this->batch++;
        }

        if (batch_id % (TIMESTAMP_NORMALIZE_RATE) == 0)
        {
            size_t part = graph.nodes.size();
            size_t start = (this->id - 1) * part;
            size_t end = std::min(graph.nodes.size(), part + start);

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
    }
}
