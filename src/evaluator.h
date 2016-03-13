#pragma once

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
        std::vector<DistanceInfo> bag, neighborBag;
        bag.push_back(DistanceInfo(from, 0));
        bool found = false;
        int64_t resultDistance = 0;

        while (!bag.empty())
        {
            unsigned long int bagSize = bag.size();
#pragma omp parallel for private(neighborBag)
            for(unsigned long int i = 0; i < bagSize; i++)
            {
                DistanceInfo current = bag[i];
                int64_t currentDistance = current.distance + 1;

                for (Vertex *neighbor : graph.nodes[current.vertexId].edges_out)
                {

                    if (neighbor->id == to)
                    {
                        resultDistance = currentDistance;
                        found = true;
                    }
#ifdef USE_THREADS
                    if (neighbor->visited[thread_id] < query_id && neighbor->edges_out.size() > 0)
#else
                    if (neighbor->visited < query_id && neighbor->edges_out.size() > 0)
#endif
                    {
                        neighborBag.push_back(DistanceInfo(neighbor->id, currentDistance));
#ifdef COLLECT_STATS
                        BFS_QUEUE_MAX_SIZE = std::max(BFS_QUEUE_MAX_SIZE, bagSize);
#endif
#ifdef USE_THREADS
                        neighbor->visited[thread_id] = query_id;
#else
                        neighbor->visited = query_id;
#endif
                    }
                }

#pragma omp critical
                {
                    bag.insert(bag.end(), neighborBag.begin(), neighborBag.end());
                    neighborBag.clear();
                }

            }

            if (found) {
                return resultDistance;
            }
            bag.erase(bag.begin(), bag.begin() + bagSize);
        }

        return -1;
    }
};