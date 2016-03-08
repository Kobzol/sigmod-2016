#include <queue>
#include <iostream>
#include <algorithm>
#include "graph.h"

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

int64_t Graph::get_distance(sigint from, sigint to)
{
    if (from == to) return 0;
    if (!this->has_vertex(from) || !this->has_vertex(to)) return -1;

    return this->query(this->nodes.at(from), this->nodes.at(to));
}

void Graph::rebuild()
{
    std::sort(this->vertices.begin(), this->vertices.end(), [](Vertex* lhs, Vertex* rhs)
    {
       return lhs->edges_out.size() > rhs->edges_out.size();
    });

    for (auto& kv : this->nodes)    // optimize: vector, sort by degree, two levels of landmarks
    {
        Vertex& vertex = kv.second;
        vertex.landmarks_in.clear();
        vertex.landmarks_out.clear();
    }

    for (size_t i = 0; i < this->vertices.size(); i++)
    {
        this->label_bfs(*this->vertices[i]);
        this->label_bfs_in(*this->vertices[i]);
    }
}

void Graph::label_bfs(Vertex& vertex)
{
    this->visit_id++;

    std::queue<DistanceInfo> q;
    q.push(DistanceInfo(&vertex, 0));

    vertex.visited = this->visit_id;

    while (!q.empty())
    {
        DistanceInfo di = q.front();
        q.pop();

        size_t distance = (size_t) this->query(vertex, *di.vertex);

        if (distance <= di.distance)
        {
            continue;
        }

        di.vertex->landmarks_in.emplace(vertex.id, di.distance);

        for (Vertex* edge : di.vertex->edges_out)
        {
            if (edge->visited < this->visit_id)
            {
                edge->visited = this->visit_id;
                q.emplace(edge, di.distance + 1);
            }
        }
    }
}
void Graph::label_bfs_in(Vertex& vertex)
{
    this->visit_id++;

    std::queue<DistanceInfo> q;
    q.push(DistanceInfo(&vertex, 0));

    vertex.visited = this->visit_id;

    while (!q.empty())
    {
        DistanceInfo di = q.front();
        q.pop();

        size_t distance = (size_t) this->query(*di.vertex, vertex);

        if (distance <= di.distance)
        {
            continue;
        }

        di.vertex->landmarks_out.emplace(vertex.id, di.distance);

        for (Vertex* edge : di.vertex->edges_in)
        {
            if (edge->visited < this->visit_id)
            {
                edge->visited = this->visit_id;
                q.emplace(edge, di.distance + 1);
            }
        }
    }
}

int64_t Graph::query(Vertex& src, Vertex& dest)
{
    int minimumDistance = INT32_MAX;

    for (auto& kv : src.landmarks_out)
    {
        if (dest.landmarks_in.count(kv.first))
        {
            minimumDistance = std::min(minimumDistance, kv.second.distance + dest.landmarks_in[kv.first].distance);
        }
    }

    if (minimumDistance == INT32_MAX) return -1;
    else return minimumDistance;
}
