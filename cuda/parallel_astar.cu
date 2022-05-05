#include <stdio.h>

#include <cuda.h>
#include <cuda_runtime.h>
#include <driver_functions.h>

#include "CycleTimer.h"
#include "parallel_hash.cuh"

__device__ inline bool is_goal(int node) {
    return node == 10;
}

__device__ inline link* get_next(int node) {
    int length = 1;
    if (node > 0) {
        length = 2;
    }
    link neighbors[length];
    neighbors[0] = link(node+1, node+1);
    if (node > 0) {
        neighbors[1] = link(node-1, node);
    }
    return neighbors;
}

__device__ inline double heuristic(int node) {
    return 10 - node;
}


___global___ std::vector<int> parallel_astar_kernel(
    int start,
    int num_queues,
    int hash_table_size
) {
    if (num_queues < 1) {
        throw "We need at least one priority queue";
    }

    int thread_idx = blockIdx.x * blockDim.x + threadIdx.x;

    // Priority queue based on cost plus heuristic
    heap* pq = new_heap();

    // Hashmap for explored vertices (hashmap size should be dependent on number of nodes)
    __shared__ HashTable *closed_set = create_hash_table(hash_table_size);

    // Array S for the neighbors of the nodes on the frontier (to be deduplicated)
    __shared__ Node S[4 * num_queues];

    // Array T for the elements of S post deduplication
    __shared__ Node T[4 * num_queues];

    // Initialize data structures
    Node begin = Node(start, 0, heuristic(start));
    Node init_node_list[] = {begin};
    if (thread_idx == 0) {
        insert_deduplicate(closed_set, init_node_list, 1, pq);
    }

    // for storing the best nodes that reach the final state
    __shared__ int m = -1;
    __shared__ int fm = -1;

    // If all queues are empty, then failed to find goal state
    __shared__ bool all_empty = true;
    all_empty = all_empty && is_empty_heap(pq); // TODO: should this be ATOMIC (?)
    __syncthreads();

    while (!all_empty) {
        if (!is_empty_heap(open_set[i])) {
            element q = peak_heap(pq)[0];
            pop_heap(pq);
            int node = q.value;
            if (is_goal(node)) {
                printf("reached a goal state!\n");
                // ATOMIC REGION (?)
                int fnode = q.priority;
                if (m == -1 || fnode < fm) {
                    m = node;
                    fm = fnode;
                }
                // END ATOMIC REGION (?)
            } else {
                int cntr = 0;
                for (link neighbor : get_next(node)) {
                    assert(query(closed_set, node).g != -1);
                    
                    int cost = query(closed_set, node).g + neighbor.cost;
                    Node new_node = Node(neighbor.node, node, cost, cost+heuristic(neighbor.node));
                    
                    S[4 * thread_idx + cntr] = new_node;
                    cntr++;
                }
            }
        }

        // return best path if goal is found and there is no
        // element in any of the pq's that are less than current
        // path to goal.
        if (m != -1) {
            printf("f(m): %d\n", fm);

            __shared__ bool all_less = true;
            for (int i = 0; i < 4; i++) {
                Node n = S[i * num_queues + thread_idx];
                // unpopulated entry
                if (n.id.zero_idx == DIM_X * DIM_Y) continue;

                if (n.f < fm) {
                    all_less = false;
                    break;
                }
            }

            if (!is_empty_heap(pq) && peak_heap(pq)[0].priority < fm) {
                all_less = false;
            }

            // TODO: NEED TO PUSH ACTIONS NOT STATES - AUGMENT THE NODE STRUCT?
            if (all_less) {
                // return path to goal
                std::vector<int> backtrack_path;
                int curr = m;
                while (curr != start) {
                    backtrack_path.push_back(curr);
                    curr = query(closed_set, curr).prev_id;
                }
                backtrack_path.push_back(start);

                std::reverse(backtrack_path.begin(), backtrack_path.end());
                return backtrack_path;
            }
        }

        // deduplication section
        __shared__ bool query_mask[4 * num_queues];
        __shared__ int num_left = 4 * num_queues;
        // run in parallel
        for (size_t i = 0; i < S.size(); i++) {
            query_mask[i] = query_cost_check(closed_set, S[i]);
            if (!query_mask[i]) num_left--;
        }

        __shared__ Node *T = (Node *) malloc(num_left * sizeof(Node));
        int j = 0;
        for (size_t i = 0; i < S.size(); i++) {
            if (query_mask[i]) {
                T[j] = S[i];
                j++;
            }
        }

        // insert the remaining nodes in parallel in closed array
        // and priority queues.
        insert_deduplicate(closed_set, T, num_left, open_set);

        bool all_empty = true;
        // we can parallelize this
        for (int i = 0; i < num_queues; i++) {
            all_empty = all_empty && is_empty_heap(open_set[i]);
        }
    }

    return {};
}

void cuda_astar(int start, int num_queues, int hash_table_size) {
    // Compute number of blocks and threads per block
    // We use one thread per priority queue
    const int threadsPerBlock = 256;
    const int blocks = (num_queues + threadsPerBlock - 1) / threadsPerBlock;

    // heap<int>** device_open_set;
    // HashTable* device_closed_set;
    // Node* device_table;

    // cudaMalloc((void **) &device_open_set, sizeof(heap<int>*) * num_queues);
    // cudaMalloc((void **) &device_closed_set, sizeof(HashTable));
    // cudaMalloc((void **) &device_table, sizeof(Node) * hash_table_size);

    double kernelStartTime = CycleTimer::currentSeconds();
    parallel_astar_kernel<<<blocks, threadsPerBlock>>>(start, num_queues, hash_table_size);
    cudaDeviceSynchronize();

    double kernelEndTime = CycleTimer::currentSeconds();

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