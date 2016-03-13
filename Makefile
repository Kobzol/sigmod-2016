TARGET=sigmod
CPP_FLAGS=-std=c++11 -march=native -pthread -O2 -Wall -Wextra -pedantic -fopenmp
CXX_COMPILER=clang-omp++

all: release

release: src/*.cpp
	$(CXX_COMPILER) $(CPP_FLAGS) $^ -o $(TARGET)

debug: src/*.cpp
	$(CXX_COMPILER) $(CPP_FLAGS) -g -O0 $^ -o $(TARGET)

clean:
	rm -f $(TARGET)

