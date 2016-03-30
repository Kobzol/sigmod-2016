TARGET=sigmod
CPP_FLAGS=-std=c++11 -march=native -flto -Wl,--no-as-needed -fprefetch-loop-arrays -Ofast -pthread -fopenmp -Wall -Wextra
#-flto -Wl,--no-as-needed
#-fno-rtti -fprefetch-loop-arrays
CXX_COMPILER=g++

all: release

release: src/*.cpp
	$(CXX_COMPILER) $(CPP_FLAGS) $^ -o $(TARGET)

debug: src/*.cpp
	$(CXX_COMPILER) $(CPP_FLAGS) -g -Og $^ -o $(TARGET)

clean:
	rm -f $(TARGET)
