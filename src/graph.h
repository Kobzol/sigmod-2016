#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "settings.h"
#include "vertex.h"


class Graph
{
public:
    Graph();
    ~Graph();

    void add_edge(sigint from, sigint to);
    void remove_edge(sigint from, sigint to);

    inline bool has_vertex(sigint id) const
    {
        return this->nodes.count(id) != 0;
    }

    std::unordered_map<sigint, Vertex> nodes;
    size_t non_unique = 0;

private:
    Graph(const Graph& other) = delete;
    Graph& operator=(const Graph& other) = delete;

    void add_vertex(sigint id);
};
