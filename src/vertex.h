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
    Vertex(sigint id) : id(id), visited(0)
#ifdef USE_UNION_FIND
            , parent(id), rank(0)
#endif
    {

    }

    sigint id;
    std::vector<Vertex*> edges_out;
    size_t visited;

#ifdef USE_UNION_FIND
    sigint get_parent(Graph& graph);
    void join(Graph& graph, Vertex& vertex);

    sigint parent;
    size_t rank;
#endif
};
