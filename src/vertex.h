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
    Landmark(int distance = 0) : distance(distance)
    {

    }

    int distance;
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
    std::unordered_map<sigint, Landmark> landmarks_out;
    std::unordered_map<sigint, Landmark> landmarks_in;
};

struct DistanceInfo
{
public:
    DistanceInfo(Vertex* vertex, size_t distance) : vertex(vertex), distance(distance)
    {

    }

    Vertex* vertex;
    size_t distance;
};
