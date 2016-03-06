#pragma once

#include <mutex>
#include <stddef.h>
#include <assert.h>
#include <atomic>
#include <map>
#include <condition_variable>

#include "settings.h"
#include "vector.h"

enum JobType
{
    Query,
    AddEdge,
    RemoveEdge,
    Invalid
};

class Job
{
public:
    Job() : type(JobType::Invalid)
    {

    }
    Job(JobType type, sigint from = 0, sigint to = 0, size_t id=0) :
            id(id), type(type), from(from), to(to)
    {

    }

    size_t id;
    JobType type;
    sigint from;
    sigint to;
};
/*
class JobQueue
{
public:
    JobQueue(size_t size)
            : jobs_in_work(0), size(0), start(0), end(0), jobs(Vector<Job>(size)), barrier(&BarrierMemory[0])
    {
        this->jobs.size = size;
        this->reset();
    }

    size_t get_size()
    {
        std::lock_guard<std::mutex> guard(this->lock);
        return this->size;
    }

    void add_job(JobType type, sigint from, sigint to)
    {
        std::unique_lock<std::mutex> guard(this->lock);

#ifdef THREAD_USE_JOB_SYNC
        while (this->size >= this->jobs.capacity)
        {
            this->bufferFull.wait(guard);
        }
#endif

        Barrier* barrier = this->barrier;
        Barrier* modifyBarrier = barrier;
        bool allocated = false;

        if (type != JobType::Query || this->barrier->modification)
        {
            Barrier* newBarrier = &BarrierMemory[BarrierIndex++];
            assert(BarrierIndex < BARRIER_BUFFER_COUNT);
            newBarrier->modification = type != JobType::Query;
            newBarrier->waiting = 1;
            modifyBarrier = newBarrier;
            this->barrier = modifyBarrier;
        }
        else if (!this->barrier->modification)
        {
            barrier->push();
        }

        this->jobs[this->end] = Job(type, from, to, barrier, modifyBarrier, allocated);
        this->end = (this->end + 1) % this->jobs.size;

        this->size++;

#ifdef THREAD_USE_JOB_SYNC
        this->bufferEmpty.notify_one();
#endif

        assert(this->size <= this->jobs.capacity);
    }

    Job pop_job()
    {
        std::unique_lock<std::mutex> guard(this->lock);

#ifdef THREAD_USE_JOB_SYNC
        this->bufferEmpty.wait(guard, [this] { return this->is_job_available(); });
#else
        if (!this->is_job_available())
        {
            return Job(JobType::Invalid);
        }
#endif

        Job job = this->jobs[this->start];

        this->start = (this->start + 1) % this->jobs.size;
        this->size--;
        this->jobs_in_work++;

#ifdef THREAD_USE_JOB_SYNC
        if (this->size == this->jobs.capacity - 1)
        {
            this->bufferFull.notify_one();
        }
#endif

        return job;
    }

    bool is_job_available()
    {
        if (this->size == 0) return false;
        if (this->jobs[this->start].type == JobType::Query && !this->jobs[this->start].barrier->modification) return true;
        return this->jobs[this->start].barrier->is_ready();
    }

    void add_result(Job& job, int64_t result)
    {
        std::lock_guard<std::mutex> guard(this->lock);

        if (job.type == JobType::Query)
        {
            this->results.insert({job.id, result});
        }

        job.modifyBarrier->pop();

        this->jobs_in_work--;

#ifdef THREAD_USE_JOB_SYNC
        this->bufferEmpty.notify_one();
#endif

        if (this->batchEnded && this->size == 0 && this->jobs_in_work == 0)
        {
            std::unique_lock<std::mutex> lock(this->jobMutex);
            this->batchEndCV.notify_one();
            this->reset();
        }
    }

    void reset()
    {
        this->batchEnded = false;
        BarrierMemory[0].modification = false;
        BarrierMemory[0].waiting = 0;
        BarrierIndex = 1;
        this->barrier = &BarrierMemory[0];
    }

    inline std::map<size_t, int64_t>& get_results()
    {
        return this->results;
    }
    inline void clear_results()
    {
        this->results.clear();
    }
    void quit()
    {
        this->quit_value = true;
    }

    size_t jobs_in_work;
    std::mutex jobMutex;
    std::condition_variable batchEndCV;
    bool batchEnded = false;

    std::condition_variable bufferEmpty;
private:
    size_t size;
    size_t start;
    size_t end;
    std::mutex lock;
    Vector<Job> jobs;
    Barrier* barrier;
    bool quit_value = false;

    std::map<size_t, int64_t> results;
    std::condition_variable bufferFull;
};
 */
