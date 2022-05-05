#define DIM_X 2 
#define DIM_Y 2

#include <stdio.h>

typedef struct slider_state {
    unsigned char board[DIM_X * DIM_Y];
    unsigned char zero_idx;

    slider_state() {
        for (int i = 0; i < DIM_X * DIM_Y; i++) {
            this->board[i] = i; // goal board
        }
        this->zero_idx = DIM_X * DIM_Y; // invalid by default
    }

    slider_state(unsigned char board[DIM_X * DIM_Y], unsigned char zero_idx) {
        for (int i = 0; i < DIM_X * DIM_Y; i++) {
            this->board[i] = board[i];
        }
        this->zero_idx = zero_idx;
    }

    bool operator==(const slider_state& state) const {
        for (unsigned char i = 0; i < DIM_X * DIM_Y; ++i) {
            if (board[i] != state.board[i]) return false;
        }
        return true;
    }
    
    bool operator!=(const slider_state& state) const {
        for (unsigned char i = 0; i < DIM_X * DIM_Y; ++i) {
            if (board[i] != state.board[i]) return true;
        }
        return false;
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

void print_slider(slider_state_t state) {
    for (int r = 0; r < DIM_Y; ++r) {
        for (int c = 0; c < DIM_X; ++c) {
            printf("%d ", state.board[r*DIM_X + c]);
        }
        printf("\n");
    }
    printf("\n");
}

