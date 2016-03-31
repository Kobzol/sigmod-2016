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

#ifdef USE_INDEX
    void build_index();
    void index_bfs(Vertex& vertex, bool forward);
    void index_bfs_resume(Vertex& source, Vertex& vertex, bool forward, bool sort);
    bool is_connected(Vertex& from, Vertex& to);
    bool is_connected_prefix(Vertex& from, Vertex& to, int prefix);
#endif

    inline bool has_vertex(sigint id) const
    {
        return this->nodes.count(id) != 0;
    }

    std::unordered_map<sigint, Vertex> nodes;

private:
    Graph(const Graph& other) = delete;
    Graph& operator=(const Graph& other) = delete;

    void add_vertex(sigint id);

#ifdef USE_INDEX
    std::vector<Vertex*> vertices;
    sigint bfs_id = 1;
#endif
};
