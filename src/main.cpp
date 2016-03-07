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

#ifdef COLLECT_STATS
size_t UNION_HITS = 0;
size_t QUERY_COUNT = 0;
std::map<int64_t, size_t> QUERY_RESULTS;
size_t BFS_QUEUE_MAX_SIZE = 0;
#endif

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

    size_t job_id = 0;

    std::vector<Job> query_list;
    query_list.reserve(10000);

    std::cout << "R" << std::endl;  // TIMER STARTS

    while (std::getline(vstup, line))
    {
        if (line[0] == 'F')
        {
            std::unique_lock<std::mutex> lock(threadPool.jobLock);
            threadPool.jobs = &query_list;
            threadPool.jobCV.notify_all();

            lock.unlock();

            std::stringstream ss;

            for (size_t i = 0; i < THREAD_POOL_THREAD_COUNT; i++)
            {
                while (threadPool.threads[i].jobsCompleted != 1)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }

                for (size_t j = 0; j < threadPool.threads[i].results.size(); j++)
                {
                    ss << threadPool.threads[i].results.at(j) << std::endl;
                }
            }

            std::cout << ss.rdbuf();

            threadPool.jobs = nullptr;

            for (size_t i = 0; i < THREAD_POOL_THREAD_COUNT; i++)
            {
                threadPool.threads[i].reset();
            }

            query_list.clear();
            batch_id++;

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
