TARGET=sigmod
CPP_FLAGS=-std=c++11 -march=native -O2 -pthread -fopenmp -Wall -Wextra
#-flto -Wl,--no-as-needed
#-fno-rtti -fprefetch-loop-arrays
CXX_COMPILER=g++

all: release

release: src/*.cpp
	$(CXX_COMPILER) $(CPP_FLAGS) $^ -o $(TARGET)

debug: src/*.cpp
	$(CXX_COMPILER) $(CPP_FLAGS) -g -O0 $^ -o $(TARGET)

clean:
	rm -f $(TARGET)

