#include "util.h"
#include <stdexcept>

void assert_throw(bool condition, const char* file, int line)
{
    if (!condition)
    {
        std::stringstream ss;
        ss << "Assert failed at " << file << ": " << line;
        throw std::runtime_error(ss.str());
    }
}
