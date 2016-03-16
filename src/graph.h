#pragma once

#include <vector>
#include <unordered_map>
#include <queue>

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
    void sort();
    int get_distance(sigint from, sigint to);

    inline bool has_vertex(sigint id)
    {
        return this->nodes.count(id) != 0;
    }

    std::unordered_map<sigint, Vertex> nodes;
    std::vector<Vertex*> vertices;

private:
    Graph(const Graph& other) = delete;
    Graph& operator=(const Graph& other) = delete;

    void add_vertex(sigint id);
    void label_bfs(Vertex& vertex);
    void label_bfs_in(Vertex& vertex);
    int query(Vertex& src, Vertex& dest);

    std::vector<int32_t> paths;
    std::queue<DistanceInfo> queue;

    size_t visit_id;
};
