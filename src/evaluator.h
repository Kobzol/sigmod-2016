#pragma once

#include <queue>

#include "settings.h"
#include "graph.h"

extern Graph graph;

#ifdef COLLECT_STATS
extern size_t UNION_HITS;
extern size_t BFS_QUEUE_MAX_SIZE;
#endif

struct DistanceInfo
{
public:
    DistanceInfo(sigint vertexId, sigint distance) : vertexId(vertexId), distance(distance)
    {

    }

    sigint vertexId;
    sigint distance;
};

class GraphEvaluator
{
public:
    static int64_t query(sigint from, sigint to, size_t query_id, size_t thread_id)
    {
        if (!graph.has_vertex(from) || !graph.has_vertex(to)) return -1;
        if (from == to) return 0;

#ifdef USE_UNION_FIND
        if (graph.nodes[from].get_parent(graph) != graph.nodes[to].get_parent(graph))
        {
#ifdef COLLECT_STATS
            UNION_HITS++;
#endif
            return -1;
        }
#endif

        std::queue<DistanceInfo> q;
        q.push(DistanceInfo(from, 0));

        while (!q.empty())
        {
            DistanceInfo current = q.front();
            q.pop();

            for (Edge& edge : graph.nodes.at(current.vertexId).edges_out)
            {
                if (edge.from < query_id && edge.to > query_id)
                {
                    if (edge.neighbor->id == to)
                    {
                        return current.distance + 1;
                    }

                    if (edge.neighbor->visited[thread_id] < query_id && edge.neighbor->edges_out.size() > 0)
                    {
                        q.push(DistanceInfo(edge.neighbor->id, current.distance + 1));
                        edge.neighbor->visited[thread_id] = query_id;
                    }
                }
            }
        }

        return -1;
    }
};