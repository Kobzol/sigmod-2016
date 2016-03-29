#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>
#include <unordered_set>
#include <map>
#include <stack>
#include <signal.h>
#include <execinfo.h>
#include <unistd.h>
#include <omp.h>

#include "graph.h"
#include "thread_pool.h"

Graph graph;
size_t batch_id = 0;
size_t job_id = 0;

#ifdef COLLECT_STATS
size_t UNION_HITS = 0;
size_t QUERY_COUNT = 0;
std::map<int64_t, size_t> QUERY_RESULTS;
size_t BFS_QUEUE_MAX_SIZE = 0;
#endif

#ifdef USE_THREADS
//ThreadPool threadPool;
#endif

int main()
{
    std::ios::sync_with_stdio(false);
    omp_set_num_threads(THREAD_POOL_THREAD_COUNT);

    GraphEvaluator::init();

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

    std::cout << "R" << std::endl;  // TIMER STARTS

    while (std::getline(vstup, line))
    {
        if (line[0] == 'F')
        {
            size_t count = query_list.size();

#pragma omp parallel for schedule(dynamic, 1)
            for (size_t i = 0; i < count; i++)
            {
                Job& job = query_list[i];
                query_list[i].result = GraphEvaluator::query(job.from, job.to, job.id, (size_t) omp_get_thread_num());
            }

            for (size_t i = 0; i < count; i++)
            {
                std::cout << query_list[i].result << std::endl;
            }

            query_list.clear();

            continue;
        }

        job_id += 2;

        std::stringstream ss(line);
        char action;
        sigint from, to;
        ss >> action >> from >> to;

        switch (action)
        {
            case 'A':
                graph.add_edge_stamp(from, to, job_id);
                break;
            case 'D':
                graph.remove_edge_stamp(from, to, job_id);
                break;
            case 'Q':
            {
                query_list.emplace_back(JobType::Query, from, to, job_id);
            }
                break;
            default:
                break;
        }
    }

#ifdef COLLECT_STATS
    std::cout << "BFS queue max size: " << BFS_QUEUE_MAX_SIZE << std::endl;
    std::cout << "Pocet dotazu: " << QUERY_COUNT << std::endl;
    std::cout << "Pocet union hitu: " << UNION_HITS << std::endl;

    std::cout << "Histogram vysledku dotazu: " << std::endl;
    for (auto& pair : QUERY_RESULTS)
    {
        std::cout << "Vysledek " << pair.first << " byl nalezen " << pair.second << "x" << std::endl;
    }
#endif

    // threads are detached, so they die with the main thread
    //threadPool.terminate();
    //jobQueue.quit();
    //jobQueue.bufferEmpty.notify_all();
    //threadPool.join();

#ifdef REDIRECT_TEST_FILE_TO_INPUT
    soubor.close();
#endif

    return 0;
}
