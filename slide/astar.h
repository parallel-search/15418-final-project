#include "heap.h"
#include "uarray.h"

#include <unordered_map>

const unsigned char DONE = 0;
const unsigned char UP = 1;
const unsigned char DOWN = 2;
const unsigned char LEFT = 3;
const unsigned char RIGHT = 4;

typedef struct link {
    unsigned char move;
    unsigned short cost;
    link(unsigned char move=DONE, unsigned short cost=0) {
        this->move = move;
        this->cost = cost;
    }
} link_t;

uarray* astar(slider_state_t init_state);
