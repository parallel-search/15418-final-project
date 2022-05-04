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
        printf("%d, ", pq->data[i].value);
    }
    printf("\n");
}

void print_node_vector(std::vector<Node> S) {
    printf("S: ");
    for (Node n : S) {
        printf("%d ", n.id);
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
    double (*f)(int), // f is not heuristic -- it is heuristic + g
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
    Node begin = Node(start, 0, f(start));
    Node init_node_list[] = {begin};
    insert_deduplicate(closed_set, init_node_list, 1, open_set);

    int m = -1;

    // If all queues are empty, then failed to find goal state
    bool all_empty = true;
    // we can parallelize this
    for (int i = 0; i < num_queues; i++) {
        all_empty = all_empty && is_empty_heap(open_set[i]);
    }

    // int cntr = 0;
    while (!all_empty) {
        std::vector<Node> S;

        // for loop in parallel
        for (int i = 0; i < num_queues; i++) {
            print_heap(open_set[i], i);

            if (is_empty_heap(open_set[i])) continue;

            element<int> q = peak_heap(open_set[i])[0];
            printf("q: %d\n", q.value);
            pop_heap(open_set[i]);
            int node = q.value;
            if (is_goal(node)) {
                printf("reached a goal state!\n");
                if (m == -1 || f(node) < f(m)) {
                    m = node;
                }
                continue;
            }

            for (link<int> neighbor : get_next(node)) {
                assert(query(closed_set, node).g != -1);
                int cost = query(closed_set, node).g + neighbor.cost;
                Node new_node = Node(neighbor.node, node, cost, f(neighbor.node));
                S.push_back(new_node);
            }
        }

        print_node_vector(S);

        // return best path if goal is found and there is no
        // element in any of the pq's that are less than current
        // path to goal.
        if (m != -1) {
            bool all_less = true;
            for (int i = 0; i < num_queues; i++) {
                if (is_empty_heap(open_set[i])) continue;

                int min_cost = peak_heap(open_set[i])->priority;
                if (f(m) < min_cost) {
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
        for (int i = 0; i < S.size(); i++) {
            query_mask[i] = query_cost_check(closed_set, S[i]);
            if (!query_mask[i]) num_left--;
        }

        Node *T = (Node *) malloc(num_left * sizeof(Node));
        int j = 0;
        for (int i = 0; i < S.size(); i++) {
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

        // cntr++;
        // if (cntr > 5) throw "end early";
        printf("-----NEW ITERATION-----\n");
    }

    return {};
}

bool is_goal(int node) {
    return node == 10;
}

std::vector<link<int>> get_next(int node) {
    std::vector<link<int>> neighbors;
    neighbors.push_back(link<int>(node + 1, node));
    return neighbors;
}

double heuristic(int node) {
    return 10 - node;
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
    std::vector<int> ans = parallel_astar(start, is_goal, get_next, heuristic, num_queues);
    for (auto elem : ans) {
        printf("%d ", elem);
    }
    printf("\n");
}

int main() {
    test_par();
    return 0;
}
