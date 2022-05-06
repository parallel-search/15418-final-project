#include <stdio.h>

#include <cuda.h>
#include <cuda_runtime.h>
#include <driver_functions.h>

#include "CycleTimer.h"
#include "parallel_hash.cuh"
#include "astar_utils.cuh"

__device__ inline bool is_goal(slider_state_t node) {
    for (unsigned char i = 0; i < DIM_X * DIM_Y; ++i) {
        if (node.board[i] != i) return false;
    }

    return true;
}

/*
__device__ inline link* get_next(slider_state_t state) {
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
}
*/

__device__ double heuristic(slider_state_t node) {
    return 0;
}


__global__ void parallel_astar_kernel(
    slider_state_t start,
    int num_queues,
    HashTable *closed_set, 
    Node *S, 
    int *best_cost,
    int *all_queue_empty,
    int *min_goal_reached
) {
    int thread_idx = blockIdx.x * blockDim.x + threadIdx.x;

    // Priority queue based on cost plus heuristic
    heap* pq = new_heap();

    // Initialize data structures
    Node begin = Node(start, 0, heuristic(start));
    Node init_node_list[] = {begin};
    if (thread_idx == 0) {
        insert_deduplicate(closed_set, init_node_list, 1, pq);
    }

    // If all queues are empty, then failed to find goal state
    atomicAnd(all_queue_empty, is_empty_heap(pq));
    __syncthreads();

    while (!(*all_queue_empty)) {
        if (!is_empty_heap(pq)) {
            element q = peak_heap(pq)[0];
            pop_heap(pq);
            slider_state_t node = q.value;
            if (is_goal(node)) {
                printf("reached a goal state!\n");
                int fnode = q.priority;
                atomicMin(best_cost, fnode);
            } else {
                int cntr = 0;

                for (unsigned char move = UP; move <= RIGHT; ++move) {
                    unsigned short cost = query(closed_set, node).g + 1;

                    slider_state_t next_state = node;
                    unsigned char new_zero;
                    switch (move) {
                        case DOWN:
                            if (node.zero_idx < DIM_X) continue;
                            new_zero = node.zero_idx - DIM_X;
                            break;
                        case UP:
                            if (node.zero_idx >= DIM_X * DIM_Y - DIM_X) continue;
                            new_zero = node.zero_idx + DIM_X;
                            break;
                        case RIGHT:
                            if (node.zero_idx % DIM_X == 0) continue;
                            new_zero = node.zero_idx - 1;
                            break;
                        case LEFT:
                            if (node.zero_idx % DIM_X == DIM_X - 1) continue;
                            new_zero = node.zero_idx + 1;
                            break;
                    }
                    next_state.board[node.zero_idx] = next_state.board[new_zero];
                    next_state.board[new_zero] = 0;
                    next_state.zero_idx = new_zero;

                    Node new_node = Node(next_state, move, node, cost, cost+heuristic(next_state));
                    S[4 * thread_idx + cntr] = new_node;
                    cntr++;
                }
            }
        }

        // return best path if goal is found and there is no
        // element in any of the pq's that are less than current
        // path to goal.
        *min_goal_reached = true;
        __syncthreads();
        if (*best_cost != INT32_MAX) {
            for (int i = 0; i < 4; i++) {
                Node n = S[i * num_queues + thread_idx];
                // unpopulated entry
                if (n.id.zero_idx == DIM_X * DIM_Y) continue;

                atomicAnd(min_goal_reached, n.f >= *best_cost);
            }

            if (!is_empty_heap(pq) && peak_heap(pq)[0].priority < *best_cost) {
                int val = peak_heap(pq)[0].priority >= *best_cost;
                atomicAnd(min_goal_reached, val);
            }

            __syncthreads();
            if (*min_goal_reached) {
                return;
            }
        }

        // deduplication section
        // run in parallel
        for (size_t i = 0; i < 4; i++) {
            Node n = S[i * num_queues + thread_idx];
            if (query_cost_check(closed_set, n)) {
                S[i * num_queues + thread_idx].id.zero_idx = DIM_X * DIM_Y;
            }
        }
        __syncthreads();

        // insert the remaining nodes in parallel in closed array
        // and priority queues.
        for (int i = 0; i < 4; i++) {
            Node n = S[i * num_queues + thread_idx];
            if (n.id.zero_idx == DIM_X * DIM_Y) continue;

            push_heap(pq, n.id, n.f);

            int z = 0;
            int ind0 = hash_fn1(n.id, closed_set->size);
            int ind1 = hash_fn2(n.id, closed_set->size);

            if (closed_set->table[ind0].id == n.id || closed_set->table[ind0].id.zero_idx == DIM_X * DIM_Y) {
                z = 0;
            } else if (closed_set->table[ind1].id == n.id || closed_set->table[ind1].id.zero_idx == DIM_X * DIM_Y) {
                z = 1;
            }

            bool to_keep = query_cost_check(closed_set, n);
            if (z == 0 && to_keep) {
                Node old = atomicExch(closed_set->table[ind0], n);
                n = old;
            } else if (z == 1 && to_keep) {
                Node old = atomicExch(closed_set->table[ind1], n);
                n = old;    
            }
        }

        *all_queue_empty = true;
        __syncthreads();
        atomicAnd(all_queue_empty, is_empty_heap(pq));
        __syncthreads();
    }
}

void cuda_astar(slider_state_t start, int num_queues, int hash_table_size) {
    // Compute number of blocks and threads per block
    // We use one thread per priority queue
    const int threadsPerBlock = 256;
    const int blocks = (num_queues + threadsPerBlock - 1) / threadsPerBlock;

    // HashTable* closed_set = create_hash_table(hash_table_size);
    Node S[4 * num_queues];
    int best_cost = INT32_MAX;

    HashTable* device_closed_set;
    Node* device_table;
    Node* device_S;
    int* device_best_cost;
    int* device_all_queue_empty;
    int* device_min_goal_reached;

    cudaMalloc((void **) &device_closed_set, sizeof(HashTable));
    cudaMalloc((void **) &device_table, sizeof(Node) * hash_table_size);
    cudaMalloc((void **) &device_S, sizeof(Node) * 4 * num_queues);
    cudaMalloc((void **) &device_best_cost, sizeof(int));
    cudaMalloc((void **) &device_all_queue_empty, sizeof(int));
    cudaMalloc((void **) &device_min_goal_reached, sizeof(int));

    // cudaMemcpy(device_closed_set, closed_set, sizeof(HashTable), cudaMemcpyHostToDevice);
    device_closed_set = create_hash_table(hash_table_size);
    // cudaMemcpy(device_table, closed_set->table, sizeof(Node) * hash_table_size, cudaMemcpyHostToDevice);
    device_table = device_closed_set->table;
    cudaMemcpy(device_S, S, sizeof(Node) * 4 * num_queues, cudaMemcpyHostToDevice);
    cudaMemcpy(device_best_cost, &best_cost, sizeof(int), cudaMemcpyHostToDevice);
    *device_all_queue_empty = true;
    *device_min_goal_reached = true;

    double kernelStartTime = CycleTimer::currentSeconds();
    parallel_astar_kernel<<<blocks, threadsPerBlock>>>(start, num_queues, device_closed_set, device_S, device_best_cost, device_all_queue_empty, device_min_goal_reached);
    cudaDeviceSynchronize();

    double kernelEndTime = CycleTimer::currentSeconds();

    // determine the actions using goal_state here
    uarray* backtrack_path = new_uarray(num_queues);
    slider_state_t curr;
    for (int i = 0; i < DIM_X * DIM_Y; i++) {
        curr.board[i] = i;
    }
    curr.zero_idx = 0;
    
    HashTable* closed_set;
    cudaMemcpy(closed_set, device_closed_set, sizeof(HashTable), cudaMemcpyDeviceToHost);
    cudaMemcpy(closed_set->table, device_closed_set->table, sizeof(Node) * hash_table_size, cudaMemcpyDeviceToHost);
    cudaMemcpy(&best_cost, device_best_cost, sizeof(int), cudaMemcpyDeviceToHost);
    while (curr != start) {
        Node curr_node = query(closed_set, curr);
        push_uarray(backtrack_path, curr_node.prev_action);
        curr = curr_node.prev_id;
    }
    reverse_uarray(backtrack_path);

    printf("Total cost: %d\n", best_cost);
    printf("Actions to goal: ");
    for (int i = 0; i < size_uarray(backtrack_path); i++) {
        printf("%d ", backtrack_path->data[i]);
    }
    printf("\n");

    double kernelOverallDuration = kernelEndTime - kernelStartTime;

    printf("Kernel Duration: %f\n", kernelOverallDuration);
}

void printCudaInfo() {
    // For fun, just print out some stats on the machine

    int deviceCount = 0;
    cudaError_t err = cudaGetDeviceCount(&deviceCount);

    printf("---------------------------------------------------------\n");
    printf("Found %d CUDA devices\n", deviceCount);

    for (int i = 0; i < deviceCount; i++) {
        cudaDeviceProp deviceProps;
        cudaGetDeviceProperties(&deviceProps, i);
        printf("Device %d: %s\n", i, deviceProps.name);
        printf("   SMs:        %d\n", deviceProps.multiProcessorCount);
        printf("   Global mem: %.0f MB\n", static_cast<float>(deviceProps.totalGlobalMem) / (1024 * 1024));
        printf("   CUDA Cap:   %d.%d\n", deviceProps.major, deviceProps.minor);
    }
    printf("---------------------------------------------------------\n");
}