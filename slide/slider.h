#define DIM_X 4
#define DIM_Y 4

typedef struct slider_state {
    unsigned char board[DIM_X * DIM_Y];
    unsigned char zero_idx;

    bool operator==(const slider_state& state) const {
        return board == state.board;
    }
} slider_state_t;

typedef struct slider_hash {
    unsigned long operator() (const slider_state& state) const {
        unsigned long hash = 0;
        unsigned long base = 1;
        for (unsigned char i = 0; i < DIM_X * DIM_Y; ++i) {
            hash += state.board[i] * base;
            base *= DIM_X * DIM_Y;
        }
        return hash;
    }
} slider_hash_t;
