#pragma once

#include <vector>
#include <cstddef>
#include <cstring>
#include <unordered_map>
#include <atomic>

#include "settings.h"

class Graph;
struct Vertex;

struct Landmark
{
public:
    Landmark(size_t id, int32_t distance = 0, sigint vertexId = 0) : id(id), distance(distance), vertexId(vertexId)
    {

    }

    size_t id;
    int32_t distance;
    sigint vertexId;
};

struct Vertex
{
public:
    Vertex(sigint id = 0) : id(id)
    {
        this->visited.store(0);
    }
    Vertex(const Vertex& other)
    {
        this->id = other.id;
        this->visited.store(0);
        this->edges_out = std::move(other.edges_out);
        this->edges_in = std::move(other.edges_in);
        this->landmarks_out = std::move(other.landmarks_out);
        this->landmarks_in = std::move(other.landmarks_in);
    }

    bool operator<(const Vertex& other)
    {
        return this->edges_out.size() > other.edges_out.size();
    }

    sigint id;
    std::atomic<size_t> visited;
    std::vector<Vertex*> edges_out;
    std::vector<Vertex*> edges_in;
    std::vector<Landmark> landmarks_out;
    std::vector<Landmark> landmarks_in;
};

struct DistanceInfo
{
public:
    DistanceInfo(Vertex* vertex, int32_t distance) : vertex(vertex), distance(distance)
    {

    }

    Vertex* vertex;
    int32_t distance;
};
