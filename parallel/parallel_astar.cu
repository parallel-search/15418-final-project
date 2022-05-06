#include <stdio.h>

#include <cuda.h>
#include <cuda_runtime.h>
#include <driver_functions.h>

#include "CycleTimer.h"

#define NUM_THREADS 256

bool is_goal(int node) {
    return node == 10;
}

std::vector<link<int>> get_next(int node) {
    std::vector<link<int>> neighbors;
    if (node > 0) {
        neighbors.push_back(link<int>(node-1, node));
    }
    neighbors.push_back(link<int>(node + 1, node+1));
    return neighbors;
}

double heuristic(int node) {
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

    int thread_index = blockIdx.x * blockDim.x + threadIdx.x;

    // Priority queue based on cost plus heuristic
    __shared__ heap<int>* open_set[num_queues];
    for (int i = 0; i < num_queues; i++) {
        open_set[i] = new_heap<int>();
    }

    // Hashmap for explored vertices (hashmap size should be dependent on number of nodes)
    __shared__ HashTable *closed_set = create_hash_table(hash_table_size);

    // Initialize data structures
    Node begin = Node(start, 0, heuristic(start));
    Node init_node_list[] = {begin};
    insert_deduplicate(closed_set, init_node_list, 1, open_set);

    int m = -1;
    int fm = -1;

    // If all queues are empty, then failed to find goal state
    bool all_empty = true;
    // we can parallelize this
    for (int i = 0; i < num_queues; i++) {
        all_empty = all_empty && is_empty_heap(open_set[i]);
    }

    while (!all_empty) {
        std::vector<Node> S;

        // for loop in parallel
        for (int i = 0; i < num_queues; i++) {
            // only evaluate this for loop if thread corresponding to queue index
            if (i != thread_index) continue;

            print_heap(open_set[i], i);

            if (is_empty_heap(open_set[i])) continue;

            element<int> q = peak_heap(open_set[i])[0];
            pop_heap(open_set[i]);
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
                continue;
            }

            for (link<int> neighbor : get_next(node)) {
                assert(query(closed_set, node).g != -1);
                int cost = query(closed_set, node).g + neighbor.cost;
                Node new_node = Node(neighbor.node, node, cost, cost+heuristic(neighbor.node));
                S.push_back(new_node);
            }
        }

        // return best path if goal is found and there is no
        // element in any of the pq's that are less than current
        // path to goal.
        if (m != -1) {
            printf("f(m): %d\n", fm);
            bool all_less = true;

            for (Node n : S) {
                if (n.f < fm) {
                    all_less = false;
                    break;
                }
            }

            for (int i = 0; i < num_queues; i++) {
                if (is_empty_heap(open_set[i])) continue;

                int min_cost = peak_heap(open_set[i])[0].priority;
                if (min_cost < fm) {
                    all_less = false;
                    break;
                }
            }

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
        __shared__ bool query_mask[S.size()];
        __shared__ int num_left = S.size();
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
    const int threadsPerBlock = NUM_THREADS;
    const int blocks = (num_queues + threadsPerBlock - 1) / threadsPerBlock;

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