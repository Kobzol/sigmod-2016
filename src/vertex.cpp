#include "vertex.h"
#include "graph.h"

#ifdef USE_UNION_FIND
sigint Vertex::get_parent(Graph &graph)
{
    if (this->parent != this->id)
    {
        this->parent = graph.nodes[this->parent].get_parent(graph);
    }

    return this->parent;
}

void Vertex::join(Graph& graph, Vertex& vertex)
{
    sigint thisParentId = this->get_parent(graph);
    sigint otherParentId = vertex.get_parent(graph);

    if (thisParentId == otherParentId) return;

    Vertex& thisParent = graph.nodes[thisParentId];
    Vertex& otherParent = graph.nodes[otherParentId];

    if (thisParent.rank < otherParent.rank)
    {
        thisParent.parent = otherParentId;
    }
    else if (thisParent.rank > otherParent.rank)
    {
        otherParent.parent = thisParentId;
    }
    else
    {
        otherParent.parent = thisParentId;
        thisParent.rank++;
    }
}
#endif
