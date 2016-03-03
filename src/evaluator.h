#pragma once

#include <queue>

#include "settings.h"
#include "graph.h"

extern Graph graph;

struct DistanceInfo
{
public:
    DistanceInfo(sigint vertexId, size_t distance) : vertexId(vertexId), distance(distance)
    {

    }

    sigint vertexId;
    size_t distance;
};

class GraphEvaluator
{
public:
    static int64_t query(sigint from, sigint to, size_t query_id)
    {
        if (!graph.has_vertex(from) || !graph.has_vertex(to)) return -1;
        if (from == to) return 0;

#ifdef USE_UNION_FIND
        if (graph.nodes[from].get_parent(graph) != graph.nodes[to].get_parent(graph))
        {
            return -1;
        }
#endif

        std::queue<DistanceInfo> q;
        q.push(DistanceInfo(from, 0));

        while (!q.empty())
        {
            DistanceInfo current = q.front();
            q.pop();
            uint64_t distance = current.distance;

            for (sigint neighbor : graph.nodes[current.vertexId].edges_out)
            {
                if (neighbor == to)
                {
                    return distance + 1;
                }

                Vertex& vertex = graph.nodes[neighbor];
                if (vertex.visited < query_id)
                {
                    q.push(DistanceInfo(neighbor, distance + 1));
                    vertex.visited = query_id;
                }
            }
        }

        return -1;
    }
};