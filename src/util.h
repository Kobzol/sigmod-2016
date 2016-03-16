#pragma once

#include <sstream>
#include <string>
#include <stdexcept>

void assert_throw(bool condition, const char* file, int line);

#define ASSERT(x) (assert_throw(x, __FILE__, __LINE__))
