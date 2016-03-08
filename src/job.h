#pragma once

#include <stddef.h>
#include <atomic>
#include "settings.h"

#define JOB_NOT_DONE (-2)

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
            id(id), type(type), from(from), to(to), result(JOB_NOT_DONE), available(ATOMIC_FLAG_INIT)
    {

    }
    Job(Job&& other)
    {
        this->id = other.id;
        this->type = other.type;
        this->from = other.from;
        this->to = other.to;
        this->result = other.result;
        this->available.clear();
    }

    size_t id;
    JobType type;
    sigint from;
    sigint to;
    int64_t result;
    std::atomic_flag available;
};
