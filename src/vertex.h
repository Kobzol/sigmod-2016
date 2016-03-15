#pragma once

#include <vector>
#include <cstddef>
#include <cstring>
#include <unordered_map>

#include "settings.h"

class Graph;
struct Vertex;

struct Landmark
{
public:
    Landmark(size_t id, int32_t distance = 0, int32_t vertexId = 0) : id(id), distance(distance), vertexId(vertexId)
    {

    }

    size_t id;
    int32_t distance;
    int32_t vertexId;
};

struct Vertex
{
public:
    Vertex(sigint id = 0) : id(id), visited(0)
    {

    }

    bool operator<(const Vertex& other)
    {
        return this->edges_out.size() > other.edges_out.size();
    }

    sigint id;
    size_t visited;
    std::vector<Vertex*> edges_out;
    std::vector<Vertex*> edges_in;
    std::vector<Landmark> landmarks_out;
    std::vector<Landmark> landmarks_in;
};

struct DistanceInfo
{
public:
    DistanceInfo(Vertex* vertex, int distance) : vertex(vertex), distance(distance)
    {

    }

    Vertex* vertex;
    int distance;
};
