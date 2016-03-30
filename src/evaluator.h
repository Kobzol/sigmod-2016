#pragma once

#include <queue>

#include "settings.h"
#include "graph.h"
#include "cQueue.h"

extern Graph graph;


#define DIR_FORWARD (0)
#define DIR_FORWARD_NEXT (1)
#define DIR_INVERSE (2)
#define DIR_INVERSE_NEXT (3)
#define QUEUE_FORWARD (0)
#define QUEUE_INVERSE (1)
#define BFS_NOT_FOUND (-1)
#define BFS_FOUND_DIRECT (1)
#define BFS_FOUND_BIDIR (2)

static cQueue<Vertex*>* QueuesOut[THREAD_POOL_THREAD_COUNT];
static cQueue<Vertex*>* QueuesIn[THREAD_POOL_THREAD_COUNT];

static int advanceBFS(int* nodeCounter, int direction, cQueue<Vertex*>& queue, sigint to, size_t query_id,
                   size_t thread_id)
{
    size_t count = nodeCounter[direction];
    sigint visited_id = direction == DIR_FORWARD ? query_id : query_id + 1;
    sigint visited_other_id = direction == DIR_FORWARD ? query_id + 1 : query_id;

    for (size_t i = 0; i < count; i++)
    {
        Vertex* vertex = *queue.Get();

        std::vector<Edge>& edges = direction == DIR_FORWARD ? vertex->edges_out : vertex->edges_in;

        for (Edge& edge : edges)
        {
            if (edge.from < query_id && edge.to > query_id)
            {
                if (edge.neighbor->id == to)
                {
                    return BFS_FOUND_DIRECT;
                }

                sigint visited = edge.neighbor->visited[thread_id];
                if (visited == visited_other_id)
                {
                    return BFS_FOUND_BIDIR;
                }
                if (visited < query_id)
                {
                    queue.Put(edge.neighbor);
                    edge.neighbor->visited[thread_id] = visited_id;
                    nodeCounter[direction + 1]++;
                }
            }
        }
    }

    return BFS_NOT_FOUND;
}

class GraphEvaluator
{
public:
    static void init()
    {
        const size_t queueSize = 1000000 * 10;

        for (int i = 0; i < THREAD_POOL_THREAD_COUNT; i++)
        {
            QueuesOut[i] = new cQueue<Vertex*>(queueSize);
            QueuesIn[i] = new cQueue<Vertex*>(queueSize);
        }
    }

    static int64_t query(sigint from, sigint to, size_t query_id, size_t thread_id)
    {
        if (from == to) return 0;
        if (!graph.has_vertex(from) || !graph.has_vertex(to)) return -1;

#ifdef USE_UNION_FIND
        if (graph.nodes.at(from).get_parent(graph) != graph.nodes.at(to).get_parent(graph))
        {
            return -1;
        }
#endif

        QueuesIn[thread_id]->Reset();
        QueuesOut[thread_id]->Reset();

        int nodeCounter[4] = { 1, 0, 1, 0 };
        /*std::queue<Vertex*> queues[2];
        queues[QUEUE_FORWARD].push(&graph.nodes.at(from));
        queues[QUEUE_INVERSE].push(&graph.nodes.at(to));*/
        QueuesOut[thread_id]->Put(&graph.nodes.at(from));
        QueuesIn[thread_id]->Put(&graph.nodes.at(to));

        size_t pathLength = 1;

        while (true)
        {
            int distance;

            if (nodeCounter[DIR_FORWARD] < nodeCounter[DIR_INVERSE])
            {
                distance = advanceBFS(nodeCounter, DIR_FORWARD, *QueuesOut[thread_id], to, query_id, thread_id);
                if (distance == BFS_FOUND_DIRECT) return pathLength;
                if (distance == BFS_FOUND_BIDIR) return pathLength * 2 - 1;
                if (nodeCounter[DIR_FORWARD_NEXT] < 1) return -1;

                distance = advanceBFS(nodeCounter, DIR_INVERSE, *QueuesIn[thread_id], from, query_id, thread_id);
                if (distance == BFS_FOUND_DIRECT) return pathLength;
                if (distance == BFS_FOUND_BIDIR) return pathLength * 2;
                if (nodeCounter[DIR_INVERSE_NEXT] < 1) return -1;
            }
            else
            {
                distance = advanceBFS(nodeCounter, DIR_INVERSE, *QueuesIn[thread_id], from, query_id, thread_id);
                if (distance == BFS_FOUND_DIRECT) return pathLength;
                if (distance == BFS_FOUND_BIDIR) return pathLength * 2 - 1;
                if (nodeCounter[DIR_INVERSE_NEXT] < 1) return -1;

                distance = advanceBFS(nodeCounter, DIR_FORWARD, *QueuesOut[thread_id], to, query_id, thread_id);
                if (distance == BFS_FOUND_DIRECT) return pathLength;
                if (distance == BFS_FOUND_BIDIR) return pathLength * 2;
                if (nodeCounter[DIR_FORWARD_NEXT] < 1) return -1;
            }

            pathLength++;
            nodeCounter[DIR_FORWARD] = nodeCounter[DIR_FORWARD_NEXT];
            nodeCounter[DIR_FORWARD_NEXT] = 0;
            nodeCounter[DIR_INVERSE] = nodeCounter[DIR_INVERSE_NEXT];
            nodeCounter[DIR_INVERSE_NEXT] = 0;
        }
    }
};