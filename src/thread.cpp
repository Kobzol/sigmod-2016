#include <cmath>
#include "thread.h"
#include "thread_pool.h"

void Thread::thread_fn()
{
    while (!this->terminated)
    {
        std::unique_lock<std::mutex> lock(threadPool.jobLock);

        while (threadPool.jobs == nullptr || this->batch > 0)
        {
            threadPool.jobCV.wait(lock);
        }

        int size = (int) threadPool.jobs->size();

        lock.unlock();

        /*if (batch_id % (TIMESTAMP_NORMALIZE_RATE) == 0)
        {
            size_t maxTimestampEnd = (size_t) -1;
            size_t part = graph.vertices.size();
            size_t start = (this->id - 1) * part;
            size_t end = std::min(graph.vertices.size(), part + start);

            for (size_t v = start; v < end; v++)
            {
                std::vector<Edge>& edges = graph.vertices.at(v)->edges_out;
                for (size_t i = 0; i < edges.size(); i++)
                {
                    if (edges.at(i).to != maxTimestampEnd)
                    {
                        edges.erase(edges.begin() + i);
                        i--;
                    }
                }
                edges = graph.vertices.at(v)->edges_in;
                for (size_t i = 0; i < edges.size(); i++)
                {
                    if (edges.at(i).to != maxTimestampEnd)
                    {
                        edges.erase(edges.begin() + i);
                        i--;
                    }
                }
            }
        }*/

        for (int i = 0; i < size; i++)
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
    }
}
