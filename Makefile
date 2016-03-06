TARGET=sigmod
CPP_FLAGS=-std=c++11 -march=native -pthread -O2 -Wall -Wextra
CXX_COMPILER=g++

all: release

release: src/*.cpp
	$(CXX_COMPILER) $(CPP_FLAGS) $^ -o $(TARGET)

debug: src/*.cpp
	$(CXX_COMPILER) $(CPP_FLAGS) -g -Og $^ -o $(TARGET)

clean:
	rm -f $(TARGET)

