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
#include "util.h"

Graph graph;
size_t batch_id = 0;
size_t job_id = 0;

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

#ifdef USE_INDEX
    graph.build_index();
#endif

    std::cout << "R" << std::endl;  // TIMER STARTS

    while (std::getline(vstup, line))
    {
        if (line[0] == 'F')
        {
            size_t count = query_list.size();

#if THREAD_SCHEDULE == THREAD_SCHEDULE_STATIC
            size_t part = (count / THREAD_POOL_THREAD_COUNT) + 1;

#pragma omp parallel
            {
                size_t thread_id = (size_t) omp_get_thread_num();
                size_t start = part * thread_id;
                size_t end = std::min(count, start + part);

                for (size_t i = start; i < end; i++)
                {
                    Job& job = query_list[i];
                    query_list[i].result = GraphEvaluator::query(job.from, job.to, job.id, thread_id);
                }
            }
#elif THREAD_SCHEDULE == THREAD_SCHEDULE_DYNAMIC
#pragma omp parallel
            {
                size_t thread_id = (size_t) omp_get_thread_num();

                for (size_t i = 0; i < count; i++)
                {
                    Job& job = query_list[i];
                    if (!job.available.test_and_set())
                    {
#ifdef USE_INDEX
                        if (graph.is_connected(graph.nodes.at(job.from), graph.nodes.at(job.to)))
                        {
                            query_list[i].result = GraphEvaluator::query(job.from, job.to, job.id, thread_id);
                        }
                        else query_list[i].result = -1;
#else
                        query_list[i].result = GraphEvaluator::query(job.from, job.to, job.id, thread_id);
#endif
                    }
                }
            }
#endif

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
