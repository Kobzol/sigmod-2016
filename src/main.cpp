#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>
#include <unordered_set>
#include <map>
#include <stack>

#include "graph.h"
#include "thread_pool.h"
#include "job.h"

Graph graph;

#ifdef COLLECT_STATS
size_t UNION_HITS = 0;
size_t QUERY_COUNT = 0;
std::map<int64_t, size_t> QUERY_RESULTS;
size_t BFS_QUEUE_MAX_SIZE = 0;
#endif

#ifdef USE_THREADS
ThreadPool threadPool;
JobQueue jobQueue(JOB_QUEUE_SIZE);
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

    std::cout << "R" << std::endl;  // TIMER STARTS
    size_t query_id = 1;

    while (std::getline(vstup, line))
    {
        if (line[0] == 'F')
        {
#ifdef USE_THREADS
            std::unique_lock<std::mutex> lock(jobQueue.jobMutex);
            jobQueue.batchEnded = true;

            while (jobQueue.get_size() > 0 || jobQueue.jobs_in_work > 0)
            {
                jobQueue.batchEndCV.wait(lock);
            }

            std::map<size_t, int64_t>& results = jobQueue.get_results();
            std::stringstream ss;
            for (auto& item : results)
            {
                ss << item.second << std::endl;
            }

            std::cout << ss.str();

            jobQueue.clear_results();
#endif
            continue;
        }

        std::stringstream ss(line);
        char action;
        sigint from, to;
        ss >> action >> from >> to;

        switch (action)
        {
            case 'A':
#ifdef USE_THREADS
                jobQueue.add_job(JobType::AddEdge, from, to);
#else
                graph.add_edge(from, to);
#endif
                break;
            case 'D':
#ifdef USE_THREADS
                jobQueue.add_job(JobType::RemoveEdge, from, to);
#else
                graph.remove_edge(from, to);
#endif
                break;
            case 'Q':
            {
#ifdef USE_THREADS
                jobQueue.add_job(JobType::Query, from, to);
#else
                int64_t result = GraphEvaluator::query(from, to, query_id++, 0);

#ifdef COLLECT_STATS
                QUERY_COUNT++;
                if (QUERY_RESULTS.count(result))
                {
                    QUERY_RESULTS[result]++;
                }
                else QUERY_RESULTS.insert({result, 1});
#else
                std::cout << result << std::endl;
#endif
#endif
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
