#pragma once

#include <vector>
#include <cstddef>
#include <cstring>

#include "settings.h"

class Graph;

struct Vertex
{
public:
    Vertex()
    {

    }
    Vertex(sigint id) : id(id)
#ifndef USE_THREADS
                        ,visited(0)
#endif
#ifdef USE_UNION_FIND
            ,parent(id), rank(0)
#endif
    {
#ifdef USE_THREADS
        std::memset(this->visited, 0, sizeof(this->visited));
#endif
    }

    sigint id;
    std::vector<Vertex*> edges_out;
#ifdef USE_THREADS
    size_t visited[THREAD_POOL_THREAD_COUNT + 1];
#else
    size_t visited;
#endif

#ifdef USE_UNION_FIND
    sigint get_parent(Graph& graph);
    void join(Graph& graph, Vertex& vertex);

    sigint parent;
    size_t rank;
#endif
};
