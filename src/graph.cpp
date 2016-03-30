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

    for (Edge& edge : src.edges_out)
    {
        if (edge.vertex == &dest) return;
    }

    src.edges_out.emplace_back(&dest);
    dest.edges_in.emplace_back(&src);
}

void Graph::add_edge_index(sigint from, sigint to)
{
    if (from == to) return;
    if (!this->has_vertex(from)) this->add_vertex(from);
    if (!this->has_vertex(to)) this->add_vertex(to);

    Vertex& src = this->nodes.at(from);
    Vertex& dest = this->nodes.at(to);

    src.edges_out.emplace_back(&dest);
    dest.edges_in.emplace_back(&src);

    size_t srcSize = src.landmarks_in.size();
    size_t destSize = dest.landmarks_out.size();

    for (size_t i = 0; i < srcSize; i++)
    {
        Landmark& landmark = src.landmarks_in[i];
        Vertex& vertex = this->nodes.at(landmark.vertexId);
        this->label_bfs_resume(landmark.id, vertex, dest, landmark.distance + 1, true);
    }

    for (size_t i = 0; i < destSize; i++)
    {
        Landmark& landmark = dest.landmarks_out[i];
        Vertex& vertex = this->nodes.at(landmark.vertexId);
        this->label_bfs_resume(landmark.id, vertex, src, landmark.distance + 1, false);
    }
}
void Graph::label_bfs_resume(size_t sourceIndex, Vertex& source, Vertex& node, int32_t distance, bool forward)
{
    this->visit_id++;

    std::queue<DistanceInfo> q;
    q.push(DistanceInfo(distance, nullptr));

    for (Landmark& landmark : (forward ? source.landmarks_out : source.landmarks_in))
    {
        if (landmark.active)
        {
            this->paths[landmark.vertexId] = landmark.distance;
        }
    }

    while (!q.empty())
    {
        DistanceInfo di = q.front();
        q.pop();
        Vertex* target = di.edge ? di.edge->vertex : &node;

        int32_t minimumDistance = DISTANCE_NOT_FOUND;
        for (Landmark &landmark : (forward ? target->landmarks_in : target->landmarks_out))
        {
            if (landmark.active)
            {
                minimumDistance = std::min(minimumDistance, paths[landmark.vertexId] + landmark.distance);
                if (minimumDistance == 1) break;
            }
        }

        if (minimumDistance <= di.distance)
        {
            continue;
        }

        /*if (forward)
        {
            if (this->prefixalQuery(source, *target, sourceIndex) <= di.distance)
            {
                continue;
            }
        }
        else
        {
            if (this->prefixalQuery(*target, source, sourceIndex) <= di.distance)
            {
                continue;
            }
        }*/

        if (di.edge)
        {
            di.edge->affectedVertices.push_back(&source);
        }

        std::vector<Landmark>& landmarks = forward ? target->landmarks_in : target->landmarks_out;

        size_t j = 0;
        for (; j < landmarks.size(); j++)
        {
            if (landmarks.at(j).id > source.id)
            {
                break;
            }
        }

        landmarks.insert(landmarks.begin() + j, Landmark(source.id, di.distance, source.id));

        for (Edge& edge : forward ? target->edges_out : target->edges_in)
        {
            if (edge.vertex->visited < this->visit_id)
            {
                edge.vertex->visited.store(this->visit_id);
                q.emplace(di.distance + 1, &edge);
            }
        }
    }

    for (Landmark& landmark : (forward ? source.landmarks_out : source.landmarks_in))
    {
        this->paths[landmark.vertexId] = DISTANCE_NOT_FOUND;
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
    if (!this->has_vertex(from) || !this->has_vertex(to)) return;

    Vertex& dest = this->nodes.at(to);
    Vertex& src = this->nodes.at(from);

    size_t outIndex = 0, inIndex = 0;

    for (Edge& edge : src.edges_out)
    {
        if (edge.vertex == &dest)
        {
            break;
        }
        else outIndex++;
    }

    if (outIndex == src.edges_out.size()) return;

    for (Edge& edge : dest.edges_in)
    {
        if (edge.vertex == &src)
        {
            break;
        }
        else inIndex++;
    }

    // invalidate vertices
    std::vector<Vertex*> affectedVertices = src.edges_out.at(outIndex).affectedVertices;
    for (Vertex* vertex : affectedVertices)
    {
        this->invalidateIndices(*vertex, true);
    }

    std::vector<Vertex*> affectedVerticesReverse = dest.edges_in.at(inIndex).affectedVertices;
    for (Vertex* vertex : affectedVerticesReverse)
    {
        this->invalidateIndices(*vertex, false);
    }

    // delete edges
    src.edges_out.erase(src.edges_out.begin() + outIndex);
    dest.edges_in.erase(dest.edges_in.begin() + inIndex);

    // re-run BFS
    for (Vertex* vertex : affectedVertices)
    {
        this->label_bfs_uni(*vertex, true);
    }
    for (Vertex* vertex : affectedVerticesReverse)
    {
        this->label_bfs_uni(*vertex, false);
    }
}

void Graph::invalidateIndices(Vertex &source, bool forward)
{
    this->visit_id++;

    std::queue<Vertex*> queue;
    queue.push(&source);

    sigint affectedId = source.id;

    while (!queue.empty())
    {
        Vertex* vertex = queue.front();
        queue.pop();

        std::vector<Landmark>& landmarks = forward ? vertex->landmarks_in : vertex->landmarks_out;
        for (size_t i = 0; i < landmarks.size(); i++)
        {
            if (landmarks.at(i).vertexId == affectedId)
            {
                landmarks.erase(landmarks.begin() + i);
                i--;
            }
        }

        for (Edge& edge : (forward ? vertex->edges_out : vertex->edges_in))
        {
            if (edge.vertex->visited < this->visit_id)
            {
                edge.vertex->visited.store(this->visit_id);
                queue.emplace(edge.vertex);
            }
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
    size_t bfs_id = vertex.id;//forward ? this->visit_id : this->visit_id - 1;

    for (Landmark& landmark : (forward ? vertex.landmarks_out : vertex.landmarks_in))
    {
        if (landmark.active)
        {
            this->paths[landmark.vertexId] = landmark.distance;
        }
    }

    vertex.visited = this->visit_id;

    std::vector<DistanceInfo> bag;
    std::vector<DistanceInfo> bag2;
    auto* currentBag = &bag;
    auto* nextBag = &bag2;
    nextBag->push_back(DistanceInfo(0, nullptr));

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
                Vertex* target = di.edge ? di.edge->vertex : &vertex;

                int32_t minimumDistance = DISTANCE_NOT_FOUND;
                std::vector<Landmark>& landmarks = forward ? target->landmarks_in : target->landmarks_out;
                int j = 0;
                int index = -1;

                for (Landmark &landmark : landmarks)
                {
                    if (landmark.active)
                    {
                        minimumDistance = std::min(minimumDistance, paths[landmark.vertexId] + landmark.distance);
                    }

                    if (landmark.id > bfs_id)
                    {
                        index = j;
                    }

                    j++;
                }

                if (minimumDistance <= di.distance)
                {
                    continue;
                }

                if (di.edge)
                {
                    di.edge->affectedVertices.push_back(&vertex);   // save affected vertex
                }

                if (index == -1)
                {
                    index = (int) landmarks.size();
                }

                landmarks.insert(landmarks.begin() + index, Landmark(bfs_id, di.distance, vertex.id));

                /*if (forward)
                {
                    target->landmarks_in.emplace_back(bfs_id, di.distance, vertex.id);
                }
                else target->landmarks_out.emplace_back(bfs_id, di.distance, vertex.id);*/

                for (Edge& edge : (forward ? target->edges_out : target->edges_in))
                {
                    size_t previous = edge.vertex->visited.exchange(this->visit_id);
                    if (previous < this->visit_id)
                    {
                        bags[threadId].emplace_back(di.distance + 1, &edge);
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

        if (srcLandmark.id == destLandmark.id && srcLandmark.active && destLandmark.active)
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
