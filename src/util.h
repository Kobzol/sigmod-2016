#pragma once

#include <chrono>
#include <iostream>
#include <sstream>
#include <string>

class Timer
{
public:
    Timer()
    {
        this->start();
    }

    long get_millis()
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - this->timer).count();
    }

    void start(std::string note="")
    {
        this->timer = std::chrono::steady_clock::now();

        if (note != "")
        {
            std::cout << "Starting: " << note << std::endl;
        }
    }
    void print(std::string note="")
    {
        std::cout << note << ": " << this->get_millis() << std::endl;
    }

private:
    std::chrono::time_point<std::chrono::steady_clock> timer;
};


void assert_throw(bool condition, const char* file, int line);

#define ASSERT(x) (assert_throw(x, __FILE__, __LINE__))
