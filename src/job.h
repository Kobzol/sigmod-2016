#pragma once

#include <mutex>
#include <stddef.h>
#include <assert.h>
#include <atomic>
#include <map>
#include <condition_variable>

#include "settings.h"
#include "vector.h"

class Barrier
{
public:
    Barrier(bool modification = false, size_t waiting = 0)
            : modification(modification), waiting(waiting)
    {

    }

    inline bool is_ready() { return this->waiting == 0; }
    inline void push() { this->waiting++; }
    inline void pop() { this->waiting--; }

    bool modification;
    std::atomic<size_t> waiting;
};

Barrier BarrierMemory[BARRIER_BUFFER_COUNT];
size_t BarrierIndex = 0;

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
    static size_t JOB_ID;

    Job() : type(JobType::Invalid)
    {

    }
    Job(JobType type, sigint from = 0, sigint to = 0, Barrier* barrier = nullptr, Barrier* modifyBarrier = nullptr,
        bool allocated = false) :
            id(Job::JOB_ID++), type(type), from(from), to(to), barrier(barrier), modifyBarrier(modifyBarrier),
            allocated(allocated)
    {

    }

    size_t id;
    JobType type;
    sigint from;
    sigint to;
    Barrier* barrier;
    Barrier* modifyBarrier;
    bool allocated;
};

size_t Job::JOB_ID = 0;

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
        std::lock_guard<std::mutex> guard(this->lock);

        Barrier* barrier = this->barrier;
        Barrier* modifyBarrier = barrier;
        bool allocated = false;

        if (type != JobType::Query || this->barrier->modification)
        {
            Barrier* newBarrier = &BarrierMemory[BarrierIndex++];
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
        assert(this->size <= this->jobs.capacity);
    }

    Job pop_job() {
        std::lock_guard<std::mutex> guard(this->lock);

        if (this->size < 1) return Job(JobType::Invalid);

        Job& job = this->jobs[this->start];
        if ((job.type == JobType::Query && !job.barrier->modification) || job.barrier->is_ready())
        {
            size_t position = this->start;

            this->start = (this->start + 1) % this->jobs.size;
            this->size--;
            this->jobs_in_work++;

            return this->jobs[position];
        }
        else return Job(JobType::Invalid);
    }

    void add_result(Job& job, int64_t result)
    {
        std::lock_guard<std::mutex> guard(this->lock);

        if (job.type == JobType::Query)
        {
            this->results[job.id] = result;
        }

        job.modifyBarrier->pop();

        this->jobs_in_work--;

        if (this->batchEnded && this->size == 0 && this->jobs_in_work == 0)
        {
            std::unique_lock<std::mutex> lock(this->jobMutex);
            this->conditionVariable.notify_one();
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

    size_t jobs_in_work;
    std::mutex jobMutex;
    std::condition_variable conditionVariable;
    bool batchEnded = false;

private:
    size_t size;
    size_t start;
    size_t end;
    std::mutex lock;
    Vector<Job> jobs;
    Barrier* barrier;

    std::map<size_t, int64_t> results;
};