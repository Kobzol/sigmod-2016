TARGET=sigmod
CPP_FLAGS=-std=c++11 -O2 -march=native -Wall -Wextra -pthread

all: release

release: src/*.cpp
	g++ $(CPP_FLAGS) $^ -o $(TARGET)

debug: src/*.cpp
	g++ $(CPP_FLAGS) -g -fno-omit-frame-pointer -O0 $^ -o $(TARGET)

clean:
	rm -f $(TARGET)

