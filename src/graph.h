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
    void add_edge_index(sigint from, sigint to);
    void remove_edge(sigint from, sigint to);

    void rebuild();
    void sort();

    int32_t get_distance(sigint from, sigint to);

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
    void label_bfs_uni(Vertex& vertex, bool forward);
    void label_bfs_resume(size_t sourceIndex, Vertex& source, Vertex& node, int32_t distance, bool forward);
    int32_t prefixalQuery(Vertex& src, Vertex& dest, size_t maxIndex);
    int32_t query(Vertex& src, Vertex& dest);

    std::vector<int32_t> paths;

    size_t visit_id;
};
