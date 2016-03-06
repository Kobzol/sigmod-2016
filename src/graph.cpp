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

    src.edges_out.emplace_back(&dest, 0, (size_t) -1);
#ifdef USE_UNION_FIND
    src.join(*this, dest);
#endif
}

void Graph::add_vertex(sigint num)
{
    this->nodes.emplace(num, num);
}

void Graph::add_edge_stamp(sigint from, sigint to, size_t job_id)
{
    if (from == to) return;
    if (!this->has_vertex(from)) this->add_vertex(from);
    if (!this->has_vertex(to)) this->add_vertex(to);

    Vertex& dest = this->nodes.at(to);
    Vertex& src = this->nodes.at(from);

    src.edges_out.emplace_back(&dest, job_id, (size_t) -1);
#ifdef USE_UNION_FIND
    src.join(*this, dest);
#endif
}

void Graph::remove_edge_stamp(sigint from, sigint to, size_t job_id)
{
    if (!this->has_vertex(from)) return;

    std::vector<Edge>& edges = this->nodes.at(from).edges_out;
    Vertex* address = &this->nodes.at(to);

    for (Edge& edge : edges)
    {
        if (edge.neighbor == address)
        {
            edge.to = job_id;
        }
    }
}
