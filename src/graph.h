#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "settings.h"

struct Vertex
{
public:
    Vertex()
    {
        this->edges_out.reserve(50);
        this->edges_in.reserve(50);
    }

    std::vector<sigint> edges_out;
    std::vector<sigint> edges_in;
};

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

private:
    Graph(const Graph& other) = delete;
    Graph& operator=(const Graph& other) = delete;

    void add_vertex(sigint id);
};
