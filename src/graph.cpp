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

    if (!this->nodes.at(from).count(to))
    {
        if (this->nodes.at(from).size() == 0)
        {
            this->nodes.at(from).reserve(50);
        }
        this->nodes.at(from).insert(to); }
}

void Graph::remove_edge(sigint from, sigint to)
{
    if (!this->has_vertex(from) || !this->has_vertex(to)) return;

    this->nodes.at(from).erase(to);
}

void Graph::add_vertex(sigint num)
{
    this->nodes.insert({num, std::unordered_set<sigint>()});
}

void Graph::one_forward(sigint from, std::vector<sigint>& edges) const
{
    for (auto edge : this->nodes.at(from))
    {
        edges.push_back(edge);
    }
}
