#include <stdio.h>
#include <stdlib.h>

void cuda_astar(slider_state_t start, int num_queues, int hash_table_size)
void printCudaInfo();

int main() {
    printCudaInfo();

    slider_state_t start;
    start.board = {3, 0, 2, 1};
    start.zero_idx = 2;

    int num_queues = 1;
    int hash_table_size = 2000;

    cuda_astar(start, num_queues, hash_table_size);

    return 0;
}