#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "settings.h"

class Graph
{
public:
    Graph();
    ~Graph();

    void add_edge(sigint from, sigint to);
    void remove_edge(sigint from, sigint to);

    void one_forward(sigint from, std::vector<sigint>& edges) const;
    void one_backward(sigint from, std::vector<sigint>& edges) const = delete;

    inline bool has_vertex(sigint id) const
    {
        return this->nodes.count(id) != 0;
    }

    std::unordered_map<sigint, std::unordered_set<sigint>> nodes;

private:
    Graph(const Graph& other) = delete;
    Graph& operator=(const Graph& other) = delete;

    void add_vertex(sigint id);
};
