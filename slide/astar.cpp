#include "astar.h"

uarray* astar(slider_state_t init_state) {
    // Priority queue based on cost plus heuristic
    heap* open_set = new_heap();

    // Hashmap which stores minimum costs from sources and parent
    std::unordered_map<slider_state_t, link_t, slider_hash> visited;

    // Initialize data structures
    unsigned short init_heuristic = 0;
    for (unsigned char r = 0; r < DIM_Y; ++r) {
        for (unsigned char c = 0; c < DIM_X; ++c) {
            unsigned char v = init_state.board[r * DIM_Y + c];
            unsigned char v_r = v / DIM_X;
            unsigned char v_c = v % DIM_X;
            unsigned char big_r = v_r > r ? v_r : r;
            unsigned char big_c = v_c > c ? v_c : c;
            unsigned char small_r = v_r > r ? r : v_r;
            unsigned char small_c = v_c > c ? c : v_c;
            init_heuristic += big_r - small_r + big_c - small_c;
        }
    }
    push_heap(open_set, init_state, init_heuristic);
    visited[init_state] = link();

    // If open_set is empty, then failed to find goal state
    while (!is_empty_heap(open_set)) {
        element* q = peak_heap(open_set);
        slider_state_t state = q->value;

        pop_heap(open_set);

        bool goal_state = true;
        for (unsigned char i = 0; i < DIM_X * DIM_Y; ++i) {
            if (state.board[i] != i) {
                goal_state = false;
                break;
            }
        }
        if (goal_state) {
            uarray* path = new_uarray();
            slider_state_t on = state;
            while (visited[on].move != DONE) {
                unsigned char move = visited[on].move;
                push_uarray(path, visited[on].move);

                unsigned char new_zero;
                switch(move) {
                    case UP:
                        new_zero = on.zero_idx - DIM_X;
                        break;
                    case DOWN:
                        new_zero = on.zero_idx + DIM_X;
                        break;
                    case LEFT:
                        new_zero = on.zero_idx - 1;
                        break;
                    case RIGHT:
                        new_zero = on.zero_idx + 1;
                        break;
                }
                on.board[on.zero_idx] = on.board[new_zero];
                on.board[new_zero] = 0;
                on.zero_idx = new_zero;
            }
            reverse_uarray(path);
            return path;
        }
        for (unsigned char move = UP; move <= RIGHT; ++move) {
            slider_state_t next_state = state;
            unsigned char new_zero;
            switch (move) {
                case DOWN:
                    if (state.zero_idx < DIM_X) continue;
                    new_zero = state.zero_idx - DIM_X;
                    break;
                case UP:
                    if (state.zero_idx >= DIM_X * DIM_Y - DIM_X) continue;
                    new_zero = state.zero_idx + DIM_X;
                    break;
                case RIGHT:
                    if (state.zero_idx % DIM_X == 0) continue;
                    new_zero = state.zero_idx - 1;
                    break;
                case LEFT:
                    if (state.zero_idx % DIM_X == DIM_X - 1) continue;
                    new_zero = state.zero_idx + 1;
                    break;
            }
            next_state.board[state.zero_idx] = next_state.board[new_zero];
            next_state.board[new_zero] = 0;
            next_state.zero_idx = new_zero;

            unsigned short cost = visited[state].cost + 1;
            if (visited.find(next_state) == visited.end() || visited[next_state].cost > cost) {
                visited[next_state] = link(move, cost);
                
                unsigned short heuristic = 0;
                for (unsigned char r = 0; r < DIM_Y; ++r) {
                    for (unsigned char c = 0; c < DIM_X; ++c) {
                        unsigned char v = state.board[r * DIM_Y + c];
                        unsigned char v_r = v / DIM_X;
                        unsigned char v_c = v % DIM_X;
                        unsigned char big_r = v_r > r ? v_r : r;
                        unsigned char big_c = v_c > c ? v_c : c;
                        unsigned char small_r = v_r > r ? r : v_r;
                        unsigned char small_c = v_c > c ? c : v_c;
                        heuristic += big_r - small_r + big_c - small_c;
                    }
                }
                push_heap(open_set, next_state, cost + heuristic);
            }
        }
    }

    return new uarray();
}
