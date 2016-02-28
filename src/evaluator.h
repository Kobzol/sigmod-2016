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

        distances.clear();
        distances.insert({from, 0});

        std::queue<sigint> q;
        q.push(from);

        std::vector<sigint> edges;

        while (!q.empty())
        {
            sigint current = q.front();
            q.pop();

            edges.clear();
            graph.one_forward(current, edges);
            for (sigint neighbor : edges)
            {
                if (neighbor == to)
                {
                    return distances[current] + 1;
                }

                if (!distances.count(neighbor))
                {
                    q.push(neighbor);
                    distances.insert({neighbor, distances[current] + 1});
                }
            }
        }

        return -1;
    }
};