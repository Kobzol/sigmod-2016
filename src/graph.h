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

    void add_edge_stamp(sigint from, sigint to, size_t job_id);
    void remove_edge_stamp(sigint from, sigint to, size_t job_id);

    inline bool has_vertex(sigint id) const
    {
        return this->nodes.count(id) != 0;
    }

    std::unordered_map<sigint, Vertex> nodes;
    std::vector<Vertex*> vertices;
    size_t non_unique = 0;

private:
    Graph(const Graph& other) = delete;
    Graph& operator=(const Graph& other) = delete;

    void add_vertex(sigint id);
};
