#include "graph.h"

#include <iostream>
#include <algorithm>
#include <omp.h>

#include "util.h"

Graph::Graph()
{
    this->visit_id = 0;
}

Graph::~Graph()
{

}

void Graph::add_edge(sigint from, sigint to)
{
    if (from == to) return;
    if (!this->has_vertex(from)) this->add_vertex(from);
    if (!this->has_vertex(to)) this->add_vertex(to);

    Vertex& dest = this->nodes.at(to);
    Vertex& src = this->nodes.at(from);

    src.edges_out.push_back(&dest);
    dest.edges_in.push_back(&src);
}
void Graph::add_edge_index(sigint from, sigint to)
{
    if (from == to) return;
    if (!this->has_vertex(from)) this->add_vertex(from);
    if (!this->has_vertex(to)) this->add_vertex(to);

    Vertex& src = this->nodes.at(from);
    Vertex& dest = this->nodes.at(to);

    src.edges_out.push_back(&dest);
    dest.edges_in.push_back(&src);

    size_t srcSize = src.landmarks_in.size();
    size_t destSize = dest.landmarks_out.size();

    for (size_t i = 0; i < srcSize; i++)
    {
        Landmark& landmark = src.landmarks_in[i];
        Vertex& vertex = this->nodes.at(landmark.vertexId);
        this->label_bfs_resume(landmark.id, vertex, dest, this->query(vertex, src) + 1, true);
    }

    for (size_t i = 0; i < destSize; i++)
    {
        Landmark& landmark = dest.landmarks_out[i];
        Vertex& vertex = this->nodes.at(landmark.vertexId);
        this->label_bfs_resume(landmark.id, vertex, src, this->query(dest, vertex) + 1, false);
    }
}

void Graph::label_bfs_resume(size_t sourceIndex, Vertex& source, Vertex& node, int32_t distance, bool forward)
{
    this->visit_id++;

    std::queue<DistanceInfo> q;
    q.push(DistanceInfo(&node, distance));

    while (!q.empty())
    {
        DistanceInfo di = q.front();
        q.pop();

        if (forward)
        {
            if (this->prefixalQuery(source, *di.vertex, sourceIndex) <= di.distance)
            {
                continue;
            }
        }
        else
        {
            if (this->prefixalQuery(*di.vertex, source, sourceIndex) <= di.distance)
            {
                continue;
            }
        }

        if (forward)
        {
            di.vertex->landmarks_in.emplace_back(sourceIndex, di.distance, source.id);
        }
        else di.vertex->landmarks_out.emplace_back(sourceIndex, di.distance, source.id);

        for (Vertex* edge : forward ? di.vertex->edges_out : di.vertex->edges_in)
        {
            if (edge->visited < this->visit_id)
            {
                edge->visited.store(this->visit_id);
                q.emplace(edge, di.distance + 1);
            }
        }
    }
}

int32_t Graph::prefixalQuery(Vertex& src, Vertex& dest, size_t maxIndex)
{
    int32_t minimumDistance = DISTANCE_NOT_FOUND;

    size_t outSize = src.landmarks_out.size();
    size_t inSize = dest.landmarks_in.size();

    for (size_t i = 0, j = 0; i < outSize && j < inSize;)
    {
        Landmark& srcLandmark = src.landmarks_out[i];
        Landmark& destLandmark = dest.landmarks_in[j];

        if (srcLandmark.id > maxIndex || destLandmark.id > maxIndex)
        {
            return DISTANCE_NOT_FOUND;
        }

        if (srcLandmark.id == destLandmark.id)
        {
            minimumDistance = std::min(minimumDistance,
                                       srcLandmark.distance + destLandmark.distance);

            if (minimumDistance == 1)
            {
                return 1;
            }

            i++, j++;
        }
        else if (srcLandmark.id < destLandmark.id)
        {
            i++;
        }
        else
        {
            j++;
        }
    }

    return minimumDistance;
}

void Graph::remove_edge(sigint from, sigint to)
{
    if (from == to) return;
    if (!this->has_vertex(from)) return;

    Vertex& dest = this->nodes.at(to);
    Vertex& src = this->nodes.at(from);

    for (size_t i = 0; i < src.edges_out.size(); i++)
    {
        if (src.edges_out[i] == &dest)
        {
            src.edges_out.erase(src.edges_out.begin() + i);
            i--;
        }
    }

    for (size_t i = 0; i < dest.edges_in.size(); i++)
    {
        if (dest.edges_in[i] == &src)
        {
            dest.edges_in.erase(dest.edges_in.begin() + i);
            i--;
        }
    }
}

void Graph::add_vertex(sigint num)
{
    this->nodes.emplace(num, num);
    this->vertices.push_back(&this->nodes.at(num));
}

int32_t Graph::get_distance(sigint from, sigint to)
{
    if (from == to) return 0;
    if (!this->has_vertex(from) || !this->has_vertex(to)) return -1;

    int32_t distance = this->query(this->nodes.at(from), this->nodes.at(to));
    return distance == DISTANCE_NOT_FOUND ? -1 : distance;
}

void Graph::sort()
{
    std::sort(this->vertices.begin(), this->vertices.end(), [](Vertex* lhs, Vertex* rhs)
    {
        return lhs->edges_out.size() > rhs->edges_out.size();
    });
}

void Graph::rebuild()
{
    for (size_t i = 0; i < this->vertices.size(); i++)
    {
        this->vertices[i]->landmarks_out.clear();
        this->vertices[i]->landmarks_in.clear();
    }

    this->paths.resize(this->vertices.size(), DISTANCE_NOT_FOUND);

    for (size_t i = 0; i < this->vertices.size(); i++)
    {
        this->label_bfs_uni(*this->vertices[i], true);
        this->label_bfs_uni(*this->vertices[i], false);
    }
}

void Graph::label_bfs_uni(Vertex& vertex, bool forward)
{
    this->visit_id++;
    size_t bfs_id = forward ? this->visit_id : this->visit_id - 1;

    for (Landmark& landmark : (forward ? vertex.landmarks_out : vertex.landmarks_in))
    {
        this->paths[landmark.vertexId] = landmark.distance;
    }

    vertex.visited = this->visit_id;

    std::vector<DistanceInfo> bag;
    std::vector<DistanceInfo> bag2;
    auto* currentBag = &bag;
    auto* nextBag = &bag2;
    nextBag->push_back(DistanceInfo(&vertex, 0));

    size_t threadCount = THREAD_POOL_THREAD_COUNT;
    std::vector<std::vector<DistanceInfo>> bags(threadCount, std::vector<DistanceInfo>());

    while (!nextBag->empty())
    {
        currentBag->clear();
        std::swap(currentBag, nextBag);
        const size_t bagSize = currentBag->size();
        size_t part = (size_t) std::ceil(bagSize / (double) threadCount);

#pragma omp parallel
        {
            int threadId = omp_get_thread_num();
            size_t start = threadId * part;
            size_t end = std::min(bagSize, start + part);

            for(size_t i = start; i < end; i++)
            {
                DistanceInfo& di = (*currentBag)[i];

                int32_t minimumDistance = DISTANCE_NOT_FOUND;
                for (Landmark &landmark : (forward ? di.vertex->landmarks_in : di.vertex->landmarks_out))
                {
                    minimumDistance = std::min(minimumDistance, paths[landmark.vertexId] + landmark.distance);
                    if (minimumDistance == 1) break;
                }

                if (minimumDistance <= di.distance)
                {
                    continue;
                }

                if (forward)
                {
                    di.vertex->landmarks_in.emplace_back(bfs_id, di.distance, vertex.id);
                }
                else di.vertex->landmarks_out.emplace_back(bfs_id, di.distance, vertex.id);

                for (Vertex *edge : (forward ? di.vertex->edges_out : di.vertex->edges_in))
                {
                    size_t previous = edge->visited.exchange(this->visit_id);
                    if (previous < this->visit_id)
                    {
                        bags[threadId].emplace_back(edge, di.distance + 1);
                    }
                }
            }

#pragma omp critical
            {
                nextBag->insert(nextBag->end(), bags[threadId].begin(), bags[threadId].end());
            }
            bags[threadId].clear();
        }
    }

    for (Landmark& landmark : (forward ? vertex.landmarks_out : vertex.landmarks_in))
    {
        this->paths[landmark.vertexId] = DISTANCE_NOT_FOUND;
    }
}

int32_t Graph::query(Vertex& src, Vertex& dest)
{
    int32_t minimumDistance = DISTANCE_NOT_FOUND;

    size_t outSize = src.landmarks_out.size();
    size_t inSize = dest.landmarks_in.size();

    for (size_t i = 0, j = 0; i < outSize && j < inSize;)
    {
        Landmark& srcLandmark = src.landmarks_out[i];
        Landmark& destLandmark = dest.landmarks_in[j];

        if (srcLandmark.id == destLandmark.id)
        {
            minimumDistance = std::min(minimumDistance,
                                       srcLandmark.distance + destLandmark.distance);

            if (minimumDistance == 1)
            {
                return 1;
            }

            i++, j++;
        }
        else if (srcLandmark.id < destLandmark.id)
        {
            i++;
        }
        else
        {
            j++;
        }
    }

    return minimumDistance;
}
