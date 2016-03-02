#include "graph.h"

Graph::Graph()
{
    this->nodes.reserve(10000);
}

Graph::~Graph()
{

}

void Graph::add_edge(sigint from, sigint to)
{
    if (from == to) return;
    if (!this->has_vertex(from)) this->add_vertex(from);
    if (!this->has_vertex(to)) this->add_vertex(to);

    for (sigint edge : this->nodes.at(from).edges_out)
    {
        if (edge == to) return;
    }

    this->nodes.at(from).edges_out.push_back(to);
#ifdef USE_UNION_FIND
    this->nodes.at(from).join(*this, this->nodes.at(to));
#endif
}

void Graph::remove_edge(sigint from, sigint to)
{
    if (!this->has_vertex(from)) return;

    std::vector<sigint>& edges = this->nodes.at(from).edges_out;
    size_t size = edges.size();

    for (size_t i = 0; i < size; i++)
    {
        if (edges[i] == to)
        {
            edges.erase(edges.begin() + i);
            return;
        }
    }
}

void Graph::add_vertex(sigint num)
{
    this->nodes.emplace(num, num);
}
