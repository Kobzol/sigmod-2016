#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>
#include <unordered_set>
#include <map>
#include <stack>
#include <omp.h>

#include "graph.h"
#include "thread_pool.h"

Graph graph;
size_t batch_id = 0;

#ifdef USE_THREADS
ThreadPool threadPool;
#endif

int main()
{
    omp_set_num_threads(THREAD_POOL_THREAD_COUNT);

    std::ios::sync_with_stdio(true);

#ifdef REDIRECT_TEST_FILE_TO_INPUT
    std::ifstream soubor("test/test-data.txt", std::ios::in);
    std::istream& vstup = soubor;
#else
    std::istream& vstup = std::cin;
#endif

    std::string line;
    while (std::getline(vstup, line))
    {
        if (line[0] == 'S')
        {
            break;
        }
        else
        {
            std::stringstream ss(line);
            sigint from, to;
            ss >> from >> to;
            graph.add_edge(from, to);
        }
    }

    std::vector<Job> query_list;
    query_list.reserve(10000);

    size_t job_id = 1;
    size_t query_id = 0;

    auto timer = std::chrono::system_clock::now();
    graph.sort();
    graph.rebuild();
    auto end = std::chrono::system_clock::now();

    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - timer).count() << std::endl;
    return 0;

    bool graphClean = true;

    std::cout << "R" << std::endl;  // TIMER STARTS

    while (std::getline(vstup, line))
    {
        if (line[0] == 'F')
        {
            continue;
        }

        job_id++;

        std::stringstream ss(line);
        char action;
        sigint from, to;
        ss >> action >> from >> to;

        switch (action)
        {
            case 'A':
            {
                graphClean = false;
                graph.add_edge(from, to);
            }
                break;
            case 'D':
            {
                graphClean = false;
                graph.remove_edge(from, to);
            }
                break;
            case 'Q':
            {
                if (!graphClean)
                {
                    graph.rebuild();
                    graphClean = true;
                }

                if (query_id == 16)
                {
                    std::cout << "Info for query from " << from << " to " << to << std::endl;
                    for (Landmark& l : graph.nodes.at(from).landmarks_out)
                    {
                        std::cout << l.vertexId << ": " << l.distance << std::endl;
                    }

                    std::cout << std::endl;

                    for (Landmark& l : graph.nodes.at(to).landmarks_in)
                    {
                        std::cout << l.vertexId << ": " << l.distance << std::endl;
                    }
                }
                query_id++;
                std::cout << graph.get_distance(from, to) << std::endl;
            }
                break;
            default:
                break;
        }
    }

#ifdef REDIRECT_TEST_FILE_TO_INPUT
    soubor.close();
#endif

    return 0;
}
