
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

/*
struct link {
    slider_state_t node;
    double cost;
    link() {
        slider_state_t node;
        node.zero_idx = DIM_X * DIM_Y;
        link(node, 0);
    }
    link(slider_state_t node, double cost) {
        this->node = node;
        this->cost = cost;
    }
};
*/