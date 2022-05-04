#include <cstdlib>
#include <astar.h>
#include <vector>

#define DIM 4

typedef struct slider_state {
    unsigned char board[DIM*DIM];
    unsigned char free_r, free_c;
} slider_state_t;

struct link_slide {
    slider_state_t node;
    double cost;
    link_slide() {
        unsigned char board[DIM*DIM];
        for (int i = 0; i < DIM*DIM; i++) {
            board[i] = i;
        }
        slider_state_t node = {board, 0, 0};
        link(node, 0);
    }
    link(slider_state_t node, double cost) {
        this->node = node;
        this->cost = cost;
    }
};

bool is_goal(slider_state_t node) {
    if (node.free_r != 0 || node.free_c != 0) return false;

    for (int i = 1; i < DIM*DIM; i++) {
        if (node.board[i] != i) return false;
    }

    return true;
}

std::vector<link_slide> get_next(slider_state_t node) {
    std::vector<link> neighbors;
    for 
}

double heuristic(int node) {}
