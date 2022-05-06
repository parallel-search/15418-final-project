#include "heap.h"

#include <iostream>
#include <limits>
#include <stdexcept>
#include <unordered_map>
#include <vector>

struct link {
    int node;
    double cost;
    link() {
        link(0, 0);
    }
    link(int node, double cost) {
        this->node = node;
        this->cost = cost;
    }
};

struct path_link {
    int node;
    double cost;
    int length;
    path_link(int node=0, double cost=0, int length=1) {
        this->node = node;
        this->cost = cost;
        this->length = length;
    }
};

std::vector<int> astar(
    std::vector<int> start,
    bool (*is_goal)(int),
    std::vector<link> (*get_next)(int),
    double (*heuristic)(int)
);
