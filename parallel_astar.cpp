#include "hash_with_replacement/parallel_hash.h"
#include <assert.h>

#include <iostream>
#include <limits>
#include <stdexcept>
#include <unordered_map>
#include <vector>
#include <algorithm>

void print_heap(heap<int>* pq, int id) {
    printf("pq %d: ", id);
    for (int i = 0; i < pq->num_elements; ++i) {
        printf("(%d, %f), ", pq->data[i].value, pq->data[i].priority);
    }
    printf("\n");
}

void print_node_vector(std::vector<Node> S) {
    for (Node n : S) {
         printf("(%d, %d, %d, %d) ", n.id, n.prev_id, n.g, n.f);
    }
    printf("\n");
}

template <typename T>
struct link {
    T node;
    double cost;
    link() {
        link(0, 0);
    }
    link(T node, double cost) {
        this->node = node;
        this->cost = cost;
    }
};

template <typename T>
struct path_link {
    T node;
    double cost;
    int length;
    path_link() {
        path_link(0, 0, 1);
    }
    path_link(T node, double cost, int length) {
        this->node = node;
        this->cost = cost;
        this->length = length;
    }
};

template <typename T>
std::vector<T> astar(
    T start,
    bool (*is_goal)(T),
    std::vector<link<T>> (*get_next)(T),
    double (*heuristic)(T)
) {
    // Priority queue based on cost plus heuristic
    heap<T>* open_set = new_heap<T>();

    // Hashmap which stores minimum costs from sources and parent
    std::unordered_map<T, path_link<T>> visited;

    // Initialize data structures
    push_heap(open_set, start, heuristic(start));
    visited[start] = path_link<T>();

    // If open_set is empty, then failed to find goal state
    while (!is_empty_heap(open_set)) {
        element<T> q = peak_heap(open_set)[0];
        pop_heap(open_set);
        T node = q.value;
        if (visited[node].cost < q.priority - heuristic(node)) continue;
        if (is_goal(node)) {
            std::vector<T> path(visited[node].length);
            T on = node;
            for (int i = visited[node].length - 1; i >= 0; --i) {
                path[i] = on;
                on = visited[on].node;
            }
            return path;
        }
        for (link<T> neighbor : get_next(node)) {
            double cost = visited[node].cost + neighbor.cost;
            if (visited.find(neighbor.node) == visited.end() || visited[neighbor.node].cost > cost) {
                visited[neighbor.node] = path_link<T>(node, cost, visited[node].length + 1);
                push_heap(open_set, neighbor.node, cost + heuristic(neighbor.node));
            }
        }
    }

    return {};
}

std::vector<int> parallel_astar(
    int start,
    bool (*is_goal)(int),
    std::vector<link<int>> (*get_next)(int),
    double (*heuristic)(int), // f is not heuristic -- it is heuristic + g
    int num_queues
) {
    if (num_queues < 1) {
        throw "We need at least one priority queue";
    }
    // Priority queue based on cost plus heuristic
    std::vector<heap<int>*> open_set (num_queues);
    for (int i = 0; i < num_queues; i++) {
        open_set[i] = new_heap<int>();
    }

    // Hashmap for explored vertices (hashmap size should be dependent on number of nodes)
    HashTable *closed_set = create_hash_table(20);

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

        printf("S: ");
        print_node_vector(S);

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
        bool query_mask[S.size()];
        int num_left = S.size();
        // run in parallel
        for (size_t i = 0; i < S.size(); i++) {
            query_mask[i] = query_cost_check(closed_set, S[i]);
            if (!query_mask[i]) num_left--;
        }

        Node *T = (Node *) malloc(num_left * sizeof(Node));
        int j = 0;
        for (size_t i = 0; i < S.size(); i++) {
            if (query_mask[i]) {
                T[j] = S[i];
                j++;
            }
        }

        printf("T: ");
        for (int i = 0; i < num_left; i++) {
            printf("(%d, %d, %d, %d) ", T[i].id, T[i].prev_id, T[i].g, T[i].f);
        }
        printf("\n");

        // insert the remaining nodes in parallel in closed array
        // and priority queues.
        insert_deduplicate(closed_set, T, num_left, open_set);

        bool all_empty = true;
        // we can parallelize this
        for (int i = 0; i < num_queues; i++) {
            all_empty = all_empty && is_empty_heap(open_set[i]);
        }

        printf("Closed set\n");
        for (int i = 0; i < closed_set->size; i++) {
            Node n = closed_set->table[i];
            printf("id: %d\t g: %d\t f: %d\n", n.id, n.g, n.f);
        }
        printf("\n");

        printf("-----NEW ITERATION-----\n");
    }

    return {};
}

bool is_goal(int node) {
    return node == 10;
}

bool is_goal9(int node) {
    return node == 9;
}

bool is_goal6(int node) {
    return node == 6;
}

std::vector<link<int>> get_next(int node) {
    std::vector<link<int>> neighbors;
    if (node > 0) {
        neighbors.push_back(link<int>(node-1, node));
    }
    neighbors.push_back(link<int>(node + 1, node+1));
    return neighbors;
}

std::vector<link<int>> get_next_cycle(int node) {
    std::vector<link<int>> neighbors;
    if (node > 0) {
        neighbors.push_back(link<int>(node-1, node));
    } else {
        // large cost for skipping from 0 to 10
        neighbors.push_back(link<int>(10, 1000));
    }
    neighbors.push_back(link<int>(node + 1, node+1));
    return neighbors;
}

std::vector<link<int>> get_next_cycle2(int node) {
    std::vector<link<int>> neighbors;
    if (node > 0) {
        neighbors.push_back(link<int>(node-1, node));
    } else {
        // small cost for skipping from 0 to 10
        neighbors.push_back(link<int>(10, 37));
    }
    neighbors.push_back(link<int>(node + 1, node+1));
    return neighbors;
}

// return trajectory changes via node 8
std::vector<link<int>> get_next_cycle3(int node) {
    std::vector<link<int>> neighbors;
    if (node > 0) {
        neighbors.push_back(link<int>(node-1, node));
    } else {
        // small cost for skipping from 0 to 10
        neighbors.push_back(link<int>(10, 37));
    }

    if (node == 1) neighbors.push_back(link<int>(8, 33));

    neighbors.push_back(link<int>(node + 1, node+1));
    return neighbors;
}

// stolen from 16-311 HW 6 : pretty direct
std::vector<link<int>> get_next_complex(int node) {
    std::vector<link<int>> neighbors;
    if (node == 0) {
        neighbors.push_back(link<int>(1, 10));
        neighbors.push_back(link<int>(3, 15));
        neighbors.push_back(link<int>(4, 20));
    } else if (node == 1) {
        neighbors.push_back(link<int>(2, 15));
        neighbors.push_back(link<int>(3, 5));
    } else if (node == 2) {
        neighbors.push_back(link<int>(3, 5));
        neighbors.push_back(link<int>(6, 5));
    } else if (node == 3) {
        neighbors.push_back(link<int>(6, 10));
    } else if (node == 4) {
        neighbors.push_back(link<int>(3, 20));
        neighbors.push_back(link<int>(5, 5));
        neighbors.push_back(link<int>(8, 5));
    } else if (node == 5) {
        neighbors.push_back(link<int>(3, 10));
        neighbors.push_back(link<int>(6, 20));
        neighbors.push_back(link<int>(7, 20));
        neighbors.push_back(link<int>(8, 15));
        neighbors.push_back(link<int>(9, 5));
    } else if (node == 6) {
        neighbors.push_back(link<int>(7, 10));
    } else if (node == 7) {
        neighbors.push_back(link<int>(9, 5));
    } else if (node == 8) {
        neighbors.push_back(link<int>(9, 10));
    }
    return neighbors;
}

// also from 16-311: lots of backtracking
std::vector<link<int>> get_next_complex2(int node) {
    std::vector<link<int>> neighbors;
    if (node == 0) {
        neighbors.push_back(link<int>(1, 2));
        neighbors.push_back(link<int>(3, 6));
        neighbors.push_back(link<int>(2, 4));
    } else if (node == 1) {
        neighbors.push_back(link<int>(4, 2));
    } else if (node == 2) {
        neighbors.push_back(link<int>(5, 1));
    } else if (node == 3) {
        neighbors.push_back(link<int>(6, 11));
    } else if (node == 4) {
        neighbors.push_back(link<int>(3, 1));
        neighbors.push_back(link<int>(6, 6));
    } else if (node == 5) {
        neighbors.push_back(link<int>(6, 6));
    }
    return neighbors;
}

double heuristic(int node) {
    return 10 - node;
}

double heuristic9(int node) {
    if (node > 9) {
        return node-9;
    }
    return 9 - node;
}

// stolen from 16-311 HW 6
double heuristic_complex(int node) {
    if (node == 0) return 1000;
    else if (node == 1) return 25;
    else if (node == 2) return 15;
    else if (node == 3) return 20;
    else if (node == 4) return 10;
    else if (node == 5) return 5;
    else if (node == 6) return 10;
    else if (node == 7) return 5;
    else if (node == 8) return 5;
    else if (node == 9) return 0;
    else {
        throw "You shouldn't be here";
    }
}

// stolen from 16-311 HW 6
double heuristic_complex2(int node) {
    if (node == 0) return 1000;
    else if (node == 1) return 1;
    else if (node == 2) return 4;
    else if (node == 3) return 9;
    else if (node == 4) return 3;
    else if (node == 5) return 1;
    else if (node == 6) return 0;
    else {
        throw "You shouldn't be here";
    }
}

void test_seq() {
    int start = 0;

    std::vector<int> ans = astar(start, is_goal, get_next, heuristic);
    for (auto elem : ans) {
        printf("%d ", elem);
    }
    printf("\n");
}

void test_par() {
    int start = 0;
    int num_queues = 5;

    // line graph
    // cost = 55, path = 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10
    // std::vector<int> ans = parallel_astar(start, is_goal, get_next, heuristic, num_queues);


    // cycle graphs
    // cost = 55, path = 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10
    // std::vector<int> ans = parallel_astar(start, is_goal, get_next_cycle, heuristic, num_queues);
    
    // cost = 37, path = 0, 10
    // std::vector<int> ans = parallel_astar(start, is_goal, get_next_cycle2, heuristic, num_queues);
    
    // cost = 45, path = 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
    // std::vector<int> ans = parallel_astar(start, is_goal9, get_next_cycle2, heuristic9, num_queues);
    
    // cost = 43, path = 0, 1, 8, 9
    // std::vector<int> ans = parallel_astar(start, is_goal9, get_next_cycle3, heuristic9, num_queues);

    // more complex graphs
    // std::vector<int> ans = parallel_astar(start, is_goal9, get_next_complex, heuristic_complex, num_queues);
    std::vector<int> ans = parallel_astar(start, is_goal6, get_next_complex2, heuristic_complex2, num_queues);

    printf("returned trajectory: ");
    for (auto elem : ans) {
        printf("%d ", elem);
    }
    printf("\n");
}

int main() {
    test_par();
    return 0;
}
