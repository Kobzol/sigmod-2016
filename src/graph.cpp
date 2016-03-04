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

    Vertex& dest = this->nodes.at(to);

#ifdef CHECK_UNIQUE_EDGES
    for (Vertex* edge : this->nodes.at(from).edges_out)
    {
        if (edge == &dest)
        {
            this->non_unique++;
            return;
        }
    }
#endif

    Vertex& src = this->nodes.at(from);

    src.edges_out.push_back(&dest);
#ifdef USE_UNION_FIND
    src.join(*this, dest);
#endif
}

void Graph::remove_edge(sigint from, sigint to)
{
    if (!this->has_vertex(from)) return;

    std::vector<Vertex*>& edges = this->nodes.at(from).edges_out;
    Vertex* address = &this->nodes.at(to);

#ifdef CHECK_UNIQUE_EDGES   // je kontrola uz pri vkladani, neni potreba prochazet vsechny
    size_t size = edges.size();

    for (size_t i = 0; i < size; i++)
    {
        if (edges[i] == address)
        {
            edges.erase(edges.begin() + i);
            return;
        }
    }
#else
    for (size_t i = 0; i < edges.size(); i++)
    {
        if (edges[i] == address)
        {
            edges.erase(edges.begin() + i);
            i--;
        }
    }
#endif
}

void Graph::add_vertex(sigint num)
{
    this->nodes.emplace(num, num);
}
