#include "graph.h"
#include <algorithm>
#include <queue>

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
    dest.edges_in.emplace_back(&src, 0, (size_t) -1);
#ifdef USE_UNION_FIND
    src.join(*this, dest);
#endif
}

void Graph::add_vertex(sigint num)
{
    this->nodes.emplace(num, num);
#ifdef USE_INDEX
    this->vertices.push_back(&this->nodes.at(num));
#endif
}

void Graph::add_edge_stamp(sigint from, sigint to, size_t job_id)
{
    if (from == to) return;
    if (!this->has_vertex(from)) this->add_vertex(from);
    if (!this->has_vertex(to)) this->add_vertex(to);

    Vertex& dest = this->nodes.at(to);
    Vertex& src = this->nodes.at(from);

    src.edges_out.emplace_back(&dest, job_id, (size_t) -1);
    dest.edges_in.emplace_back(&src, job_id, (size_t) -1);

#ifdef USE_INDEX
    if (src.bfs_id == -1) src.bfs_id = this->bfs_id++;
    if (dest.bfs_id == -1) dest.bfs_id = this->bfs_id++;

    size_t srcSize = src.landmarks_in.size();
    size_t destSize = dest.landmarks_out.size();

    for (size_t i = 0; i < srcSize; i++)
    {
        Landmark& landmark = src.landmarks_in[i];
        Vertex& vertex = *landmark.vertex;
        bool sort = true;

        if (vertex.bfs_id == -1)
        {
            vertex.bfs_id = this->bfs_id++;
            sort = false;
        }

        this->index_bfs_resume(vertex, dest, true, sort);
    }

    for (size_t i = 0; i < destSize; i++)
    {
        Landmark& landmark = dest.landmarks_out[i];
        Vertex& vertex = *landmark.vertex;
        bool sort = true;

        if (vertex.bfs_id == -1)
        {
            vertex.bfs_id = this->bfs_id++;
            sort = false;
        }

        this->index_bfs_resume(vertex, src, false, sort);
    }
#endif
#ifdef USE_UNION_FIND
    src.join(*this, dest);
#endif
}

void Graph::remove_edge_stamp(sigint from, sigint to, size_t job_id)
{
    if (!this->has_vertex(from) || !this->has_vertex(to)) return;

    std::vector<Edge>& edges = this->nodes.at(from).edges_out;
    Vertex* address = &this->nodes.at(to);

    for (Edge& edge : edges)
    {
        if (edge.neighbor == address)
        {
            edge.to = job_id;
        }
    }

    std::vector<Edge>& edgesInverse = this->nodes.at(to).edges_in;
    Vertex* addressInverse = &this->nodes.at(from);

    for (Edge& edge : edgesInverse)
    {
        if (edge.neighbor == addressInverse)
        {
            edge.to = job_id;
        }
    }
}

#ifdef USE_INDEX
void Graph::build_index()
{
    std::sort(this->vertices.begin(), this->vertices.end(), [](Vertex* lhs, Vertex* rhs)
    {
       return lhs->edges_out.size() > rhs->edges_out.size();
    });

    for (Vertex* vertex : this->vertices)
    {
        vertex->landmarks_out.clear();
        vertex->landmarks_in.clear();
    }

    for (Vertex* vertex : this->vertices)
    {
        vertex->bfs_id = this->bfs_id++;
        this->index_bfs(*vertex, true);
        this->index_bfs(*vertex, false);
    }
}

void Graph::index_bfs(Vertex& vertex, bool forward)
{
    this->bfs_id++;

    vertex.visited[THREAD_POOL_THREAD_COUNT] = this->bfs_id;

    std::queue<Vertex*> queue;
    queue.push(&vertex);

    while (!queue.empty())
    {
        Vertex *current = queue.front();
        queue.pop();

        if (forward)
        {
            if (this->is_connected(vertex, *current))
            {
                continue;
            }
        }
        else
        {
            if (this->is_connected(*current, vertex))
            {
                continue;
            }
        }

        std::vector<Landmark>& landmarks = forward ? current->landmarks_in : current->landmarks_out;
        landmarks.emplace_back(vertex.bfs_id, &vertex);
        std::vector<Edge>& edges = forward ? current->edges_out : current->edges_in;

        for (Edge& edge : edges)
        {
            if (edge.neighbor->visited[THREAD_POOL_THREAD_COUNT] < this->bfs_id)
            {
                edge.neighbor->visited[THREAD_POOL_THREAD_COUNT] = this->bfs_id;
                queue.push(edge.neighbor);
            }
        }
    }
}

void Graph::index_bfs_resume(Vertex &source, Vertex &vertex, bool forward, bool sort)
{
    this->bfs_id++;

    vertex.visited[THREAD_POOL_THREAD_COUNT] = this->bfs_id;

    std::queue<Vertex*> queue;
    queue.push(&vertex);

    while (!queue.empty())
    {
        Vertex *current = queue.front();
        queue.pop();

        if (forward)
        {
            if (this->is_connected_prefix(source, *current, source.bfs_id))
            {
                continue;
            }
        }
        else
        {
            if (this->is_connected_prefix(*current, source, source.bfs_id))
            {
                continue;
            }
        }

        std::vector<Landmark>& landmarks = forward ? current->landmarks_in : current->landmarks_out;

        if (sort)
        {
            size_t size = landmarks.size();
            size_t i = 0;

            for (; i < size; i++)
            {
                if (landmarks.at(i).id > source.bfs_id)
                {
                    break;
                }
            }

            landmarks.insert(landmarks.begin() + i, std::move(Landmark(source.bfs_id, &source)));
        }
        else landmarks.emplace_back(source.bfs_id, &source);

        std::vector<Edge>& edges = forward ? current->edges_out : current->edges_in;

        for (Edge& edge : edges)
        {
            if (edge.neighbor->visited[THREAD_POOL_THREAD_COUNT] < this->bfs_id)
            {
                edge.neighbor->visited[THREAD_POOL_THREAD_COUNT] = this->bfs_id;
                queue.push(edge.neighbor);
            }
        }
    }
}

bool Graph::is_connected(Vertex& from, Vertex& to)
{
    size_t outSize = from.landmarks_out.size();
    size_t inSize = to.landmarks_in.size();

    for (size_t i = 0, j = 0; i < outSize && j < inSize;)
    {
        Landmark& srcLandmark = from.landmarks_out[i];
        Landmark& destLandmark = to.landmarks_in[j];

        if (srcLandmark.id == destLandmark.id)
        {
            return true;
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

    return false;
}

bool Graph::is_connected_prefix(Vertex &from, Vertex &to, int prefix)
{
    size_t outSize = from.landmarks_out.size();
    size_t inSize = to.landmarks_in.size();

    for (size_t i = 0, j = 0; i < outSize && j < inSize;)
    {
        Landmark& srcLandmark = from.landmarks_out[i];
        Landmark& destLandmark = to.landmarks_in[j];

        if (srcLandmark.id > prefix || destLandmark.id > prefix) return false;

        if (srcLandmark.id == destLandmark.id)
        {
            return true;
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

    return false;
}


#endif
