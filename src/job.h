#pragma once

#include <stddef.h>
#include "settings.h"

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
