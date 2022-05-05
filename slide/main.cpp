#include "slider.h"

#include <stdexcept>

int main(int argc, char** argv) {
    if (argc != 2)
        throw std::invalid_argument(
            "expected: input_file"
        );

    unsigned char on = 0;
    slider_state_t init_state; 

    char cur[3], cur_num;
    FILE *fp = fopen(argv[1], "r");
    while(fscanf(fp, "%s ", cur) > 0) {
        cur_num = atoi(cur);
        init_state.board[on] = cur_num;
        if (cur_num == 0) init_state.zero_idx = on;
        ++on;
    }
}
