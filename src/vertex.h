#pragma once

#include <vector>
#include <cstddef>
#include <cstring>

#include "settings.h"

class Graph;
struct Vertex;

struct Edge
{
public:
    Edge(Vertex* neighbor=nullptr, unsigned int from=0, unsigned int to=(unsigned int) -1)
            : neighbor(neighbor), from(from), to(to)
    {

    }

    Vertex* neighbor;
    unsigned int from;
    unsigned int to;
};


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

    std::vector<Edge> edges_out;
    std::vector<Edge> edges_in;
    sigint id;
#ifdef USE_THREADS
    sigint visited[THREAD_POOL_THREAD_COUNT];
#else
    sigint visited;
#endif

#ifdef USE_UNION_FIND
    sigint get_parent(Graph& graph);
    void join(Graph& graph, Vertex& vertex);

    sigint parent;
    sigint rank;
#endif
};
