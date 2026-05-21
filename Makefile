CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2 -Iinclude
TARGET   := bin/pricer
SRCS     := src/main.cpp src/Option.cpp src/MonteCarlo.cpp
OBJS     := $(SRCS:src/%.cpp=build/%.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS) | bin
  $(CXX) $(CXXFLAGS) -o $@ $^

build/%.o: src/%.cpp | build
	$(CXX) $(CXXFLAGS) -c $< -o $@

bin:
  mkdir -p bin

build:
  mkdir -p build

clean:
	rm -rf build bin
