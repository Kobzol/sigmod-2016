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
ThreadPool threadPool;
JobQueue jobQueue(JOB_QUEUE_SIZE);

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
    while(std::getline(vstup, line))
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

    while(std::getline(vstup, line))
    {
        if (line[0] == 'F')
        {
            if (jobQueue.get_size() > 0 || jobQueue.jobs_in_work > 0)
            {
                std::unique_lock<std::mutex> lock(jobQueue.jobMutex);
                jobQueue.batchEnded = true;
                jobQueue.conditionVariable.wait(lock);
            }

            std::map<size_t, int64_t>& results = jobQueue.get_results();
            std::stringstream ss;
            for (auto& item : results)
            {
                ss << item.second << std::endl;
            }

            std::cout << ss.str();

            jobQueue.clear_results();

            continue;
        }

        std::stringstream ss(line);
        char action;
        sigint from, to;
        ss >> action >> from >> to;

        switch (action)
        {
            case 'A':
                //graph.add_edge(from, to);
                jobQueue.add_job(JobType::AddEdge, from, to);
                break;
            case 'D':
                //graph.remove_edge(from, to);
                jobQueue.add_job(JobType::RemoveEdge, from, to);
                break;
            case 'Q':
                jobQueue.add_job(JobType::Query, from, to);
                break;
            default:
                break;
        }
    }

    threadPool.stop();

#ifdef REDIRECT_TEST_FILE_TO_INPUT
    soubor.close();
#endif

    return 0;
}
