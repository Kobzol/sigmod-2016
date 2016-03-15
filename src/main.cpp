#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>
#include <unordered_set>
#include <map>
#include <stack>

#include "graph.h"
#include "thread_pool.h"

Graph graph;
size_t batch_id = 0;

#ifdef USE_THREADS
ThreadPool threadPool;
#endif

int main()
{
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

    graph.sort();
    graph.rebuild();

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
