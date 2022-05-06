#include "slider.h"

void print_slider(slider_state_t state) {
    for (int r = 0; r < DIM_Y; ++r) {
        for (int c = 0; c < DIM_X; ++c) {
            printf("%d ", state.board[r*DIM_X + c]);
        }
        printf("\n");
    }
    printf("\n");
}
