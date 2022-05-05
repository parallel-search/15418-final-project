#include <stdio.h>
#include <stdlib.h>

#include "slider.cuh"

void cuda_astar(slider_state_t start, int num_queues, int hash_table_size);
void printCudaInfo();

int main() {
    printCudaInfo();

    unsigned char board[] = {3, 0, 2, 1};
    slider_state_t start = slider_state(board, 2);

    int num_queues = 1;
    int hash_table_size = 2000;

    cuda_astar(start, num_queues, hash_table_size);

    return 0;
}