#pragma once

#include <vector>
#include <cstddef>

#include "settings.h"

class Graph;

struct Vertex
{
public:
    Vertex()
    {

    }
    Vertex(sigint id) : id(id)
#ifdef USE_UNION_FIND
            , parent(id), rank(0)
#endif
    {
        this->edges_out.reserve(50);
    }

    sigint id;
    std::vector<sigint> edges_out;

#ifdef USE_UNION_FIND
    sigint get_parent(Graph& graph);
    void join(Graph& graph, Vertex& vertex);

    sigint parent;
    size_t rank;
#endif
};
