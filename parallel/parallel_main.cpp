#include "parallel_astar.h"

bool is_goal(int node) {
    return node == 10;
}

bool is_goal9(int node) {
    return node == 9;
}

bool is_goal6(int node) {
    return node == 6;
}

std::vector<link<int>> get_next(int node) {
    std::vector<link<int>> neighbors;
    if (node > 0) {
        neighbors.push_back(link<int>(node-1, node));
    }
    neighbors.push_back(link<int>(node + 1, node+1));
    return neighbors;
}

std::vector<link<int>> get_next_cycle(int node) {
    std::vector<link<int>> neighbors;
    if (node > 0) {
        neighbors.push_back(link<int>(node-1, node));
    } else {
        // large cost for skipping from 0 to 10
        neighbors.push_back(link<int>(10, 1000));
    }
    neighbors.push_back(link<int>(node + 1, node+1));
    return neighbors;
}

std::vector<link<int>> get_next_cycle2(int node) {
    std::vector<link<int>> neighbors;
    if (node > 0) {
        neighbors.push_back(link<int>(node-1, node));
    } else {
        // small cost for skipping from 0 to 10
        neighbors.push_back(link<int>(10, 37));
    }
    neighbors.push_back(link<int>(node + 1, node+1));
    return neighbors;
}

// return trajectory changes via node 8
std::vector<link<int>> get_next_cycle3(int node) {
    std::vector<link<int>> neighbors;
    if (node > 0) {
        neighbors.push_back(link<int>(node-1, node));
    } else {
        // small cost for skipping from 0 to 10
        neighbors.push_back(link<int>(10, 37));
    }

    if (node == 1) neighbors.push_back(link<int>(8, 33));

    neighbors.push_back(link<int>(node + 1, node+1));
    return neighbors;
}

// stolen from 16-311 HW 6 : pretty direct
std::vector<link<int>> get_next_complex(int node) {
    std::vector<link<int>> neighbors;
    if (node == 0) {
        neighbors.push_back(link<int>(1, 10));
        neighbors.push_back(link<int>(3, 15));
        neighbors.push_back(link<int>(4, 20));
    } else if (node == 1) {
        neighbors.push_back(link<int>(2, 15));
        neighbors.push_back(link<int>(3, 5));
    } else if (node == 2) {
        neighbors.push_back(link<int>(3, 5));
        neighbors.push_back(link<int>(6, 5));
    } else if (node == 3) {
        neighbors.push_back(link<int>(6, 10));
    } else if (node == 4) {
        neighbors.push_back(link<int>(3, 20));
        neighbors.push_back(link<int>(5, 5));
        neighbors.push_back(link<int>(8, 5));
    } else if (node == 5) {
        neighbors.push_back(link<int>(3, 10));
        neighbors.push_back(link<int>(6, 20));
        neighbors.push_back(link<int>(7, 20));
        neighbors.push_back(link<int>(8, 15));
        neighbors.push_back(link<int>(9, 5));
    } else if (node == 6) {
        neighbors.push_back(link<int>(7, 10));
    } else if (node == 7) {
        neighbors.push_back(link<int>(9, 5));
    } else if (node == 8) {
        neighbors.push_back(link<int>(9, 10));
    }
    return neighbors;
}

// also from 16-311: lots of backtracking
std::vector<link<int>> get_next_complex2(int node) {
    std::vector<link<int>> neighbors;
    if (node == 0) {
        neighbors.push_back(link<int>(1, 2));
        neighbors.push_back(link<int>(3, 6));
        neighbors.push_back(link<int>(2, 4));
    } else if (node == 1) {
        neighbors.push_back(link<int>(4, 2));
    } else if (node == 2) {
        neighbors.push_back(link<int>(5, 1));
    } else if (node == 3) {
        neighbors.push_back(link<int>(6, 11));
    } else if (node == 4) {
        neighbors.push_back(link<int>(3, 1));
        neighbors.push_back(link<int>(6, 6));
    } else if (node == 5) {
        neighbors.push_back(link<int>(6, 6));
    }
    return neighbors;
}

double heuristic(int node) {
    return 10 - node;
}

double heuristic9(int node) {
    if (node > 9) {
        return node-9;
    }
    return 9 - node;
}

// stolen from 16-311 HW 6
double heuristic_complex(int node) {
    if (node == 0) return 1000;
    else if (node == 1) return 25;
    else if (node == 2) return 15;
    else if (node == 3) return 20;
    else if (node == 4) return 10;
    else if (node == 5) return 5;
    else if (node == 6) return 10;
    else if (node == 7) return 5;
    else if (node == 8) return 5;
    else if (node == 9) return 0;
    else {
        throw "You shouldn't be here";
    }
}

// stolen from 16-311 HW 6
double heuristic_complex2(int node) {
    if (node == 0) return 1000;
    else if (node == 1) return 1;
    else if (node == 2) return 4;
    else if (node == 3) return 9;
    else if (node == 4) return 3;
    else if (node == 5) return 1;
    else if (node == 6) return 0;
    else {
        throw "You shouldn't be here";
    }
}

void test_seq() {
    int start = 0;

    std::vector<int> ans = astar(start, is_goal, get_next, heuristic);
    for (auto elem : ans) {
        printf("%d ", elem);
    }
    printf("\n");
}

void test_par() {
    int start = 0;
    int num_queues = 5;

    // line graph
    // cost = 55, path = 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10
    // std::vector<int> ans = parallel_astar(start, is_goal, get_next, heuristic, num_queues);


    // cycle graphs
    // cost = 55, path = 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10
    // std::vector<int> ans = parallel_astar(start, is_goal, get_next_cycle, heuristic, num_queues);
    
    // cost = 37, path = 0, 10
    // std::vector<int> ans = parallel_astar(start, is_goal, get_next_cycle2, heuristic, num_queues);
    
    // cost = 45, path = 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
    // std::vector<int> ans = parallel_astar(start, is_goal9, get_next_cycle2, heuristic9, num_queues);
    
    // cost = 43, path = 0, 1, 8, 9
    // std::vector<int> ans = parallel_astar(start, is_goal9, get_next_cycle3, heuristic9, num_queues);

    // more complex graphs
    // std::vector<int> ans = parallel_astar(start, is_goal9, get_next_complex, heuristic_complex, num_queues);
    std::vector<int> ans = parallel_astar(start, is_goal6, get_next_complex2, heuristic_complex2, num_queues);

    printf("returned trajectory: ");
    for (auto elem : ans) {
        printf("%d ", elem);
    }
    printf("\n");
}

int main() {
    test_par();
    return 0;
}
