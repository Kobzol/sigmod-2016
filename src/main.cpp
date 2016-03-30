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
#include "util.h"

Graph graph;
size_t batch_id = 0;

#ifdef USE_THREADS
ThreadPool threadPool;
#endif

int main()
{
    omp_set_num_threads(THREAD_POOL_THREAD_COUNT);

    std::ios::sync_with_stdio(false);

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

    Timer timer;
    graph.sort();
    graph.rebuild();
    timer.print("Index build");

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
                //timer.start("Add (" + std::to_string(from) + ", " + std::to_string(to) + ")");
                graph.add_edge_index(from, to);
                //timer.print("Add");
            }
                break;
            case 'D':
            {
                //timer.start("Remove (" + std::to_string(from) + ", " + std::to_string(to) + ")");
                graph.remove_edge(from, to);
                //timer.print("Remove");
            }
                break;
            case 'Q':
            {
                query_id++;
                //timer.start("Query (" + std::to_string(from) + ", " + std::to_string(to) + ")");
                std::cout << graph.get_distance(from, to) << std::endl;
                //timer.print("Query");
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
