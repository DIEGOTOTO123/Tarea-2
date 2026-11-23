# makefile -- Tarea 2, INF-221 (grid pathfinding)
# Autor: Diego Araneda Campusano (rol: 202473538-1)

CXX      := g++
CXXFLAGS := -O2 -std=c++17 -Wall -Wextra
TARGET   := grid_pathfinding
SRCS     := grid_pathfinding.cpp algorithms/brute_force.cpp \
            algorithms/dynamic_programming.cpp algorithms/greedy.cpp

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $(SRCS)

run: $(TARGET)
	./$(TARGET) data/grid_input data/grid_output data/measurements

grids:
	python3 scripts/grid_generator.py --seed 2026

plots:
	python3 scripts/plot_generator.py

clean:
	rm -f $(TARGET)

.PHONY: all run grids plots clean
