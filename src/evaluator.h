#pragma once

#include "settings.h"
#include "graph.h"
#include <omp.h>
#include <cmath>

extern Graph graph;

#ifdef COLLECT_STATS
extern size_t UNION_HITS;
extern size_t BFS_QUEUE_MAX_SIZE;
#endif

struct DistanceInfo
{
public:
    DistanceInfo(Vertex* vertex, sigint distance) : vertex(vertex), distance(distance)
    {

    }

    Vertex* vertex;
    sigint distance;
};

class GraphEvaluator
{
public:
    static int64_t query(sigint from, sigint to, size_t query_id, size_t thread_id)
    {
        if (from == to) return 0;
        if (!graph.has_vertex(from) || !graph.has_vertex(to)) return -1;

#ifdef USE_UNION_FIND
        if (graph.nodes[from].get_parent(graph) != graph.nodes[to].get_parent(graph))
        {
            return -1;
        }
#endif
        std::vector<DistanceInfo> bag;
        bag.reserve(200);
        bag.push_back(DistanceInfo(&graph.nodes.at(from), 0));
        bool found = false;
        int64_t resultDistance = 0;

        size_t threadCount = THREAD_POOL_THREAD_COUNT;
        std::vector<std::vector<DistanceInfo>> bags(threadCount, std::vector<DistanceInfo>());

        while (!bag.empty())
        {
            const size_t bagSize = bag.size();
            size_t part = std::ceil(bagSize / (double) threadCount);
#pragma omp parallel
            {
                int threadId = omp_get_thread_num();
                int start = threadId * part;
                int end = std::min(bagSize, start + part);

                for(int i = start; i < end; i++)
                {
                    DistanceInfo current = bag[i];
                    int64_t currentDistance = current.distance + 1;

                    for (Vertex *neighbor : current.vertex->edges_out)
                    {
                        if (neighbor->id == to)
                        {
                            resultDistance = currentDistance;
                            found = true;
                        }
                        if (neighbor->visited < query_id && neighbor->edges_out.size() > 0)
                        {
                            bags[threadId].push_back(DistanceInfo(neighbor, currentDistance));
                            neighbor->visited = query_id;
                        }
                    }
                }

#pragma omp barrier
#pragma omp single
                {
                    bag.clear();
                }

#pragma omp critical
                {
                    bag.insert(bag.end(), bags[threadId].begin(), bags[threadId].end());
                }
                bags[threadId].clear();
            }

            if (found) {
                return resultDistance;
            }
        }

        return -1;
    }
};