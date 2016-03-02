#pragma once

#include <queue>

#include "settings.h"
#include "graph.h"

extern Graph graph;

class GraphEvaluator
{
public:
    static int64_t query(sigint from, sigint to)
    {
        std::unordered_map<sigint, uint64_t> distances;

        if (!graph.has_vertex(from) || !graph.has_vertex(to)) return -1;
        if (from == to) return 0;

#ifdef USE_UNION_FIND
        if (graph.nodes[from].get_parent(graph) != graph.nodes[to].get_parent(graph))
        {
            return -1;
        }
#endif

        distances.insert({from, 0});

        std::queue<sigint> q;
        q.push(from);

        while (!q.empty())
        {
            sigint current = q.front();
            q.pop();
            uint64_t distance = distances[current];

            for (sigint neighbor : graph.nodes[current].edges_out)
            {
                if (neighbor == to)
                {
                    return distance + 1;
                }

                if (!distances.count(neighbor))
                {
                    q.push(neighbor);
                    distances.insert({neighbor, distance + 1});
                }
            }
        }

        return -1;
    }
};