#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <map>

#include "settings.h"
#include "vertex.h"


class Graph
{
public:
    Graph();
    ~Graph();

    void add_edge(sigint from, sigint to);
    void remove_edge(sigint from, sigint to);
    void rebuild();
    int64_t get_distance(sigint from, sigint to);

    inline bool has_vertex(sigint id)
    {
        return this->nodes.count(id) != 0;
    }

    std::map<sigint, Vertex> nodes;
    std::vector<Vertex*> vertices;

private:
    Graph(const Graph& other) = delete;
    Graph& operator=(const Graph& other) = delete;

    void add_vertex(sigint id);
    void label_bfs(Vertex& vertex);
    void label_bfs_in(Vertex& vertex);
    int64_t query(Vertex& src, Vertex& dest);

    size_t visit_id;
};
