#include "parallel_hash.h"
#include "heap.h"

#include <iostream>
#include <limits>
#include <stdexcept>
#include <unordered_map>
#include <vector>

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
    std::vector<T> start,
    bool (*is_goal)(T),
    std::vector<link<T>> (*get_next)(T),
    double (*heuristic)(T)
) {
    // Priority queue based on cost plus heuristic
    heap<T>* open_set = new_heap<T>();

    // Hashmap which stores minimum costs from sources and parent
    std::unordered_map<T, path_link<T>> visited;

    // Initialize data structures
    for (T elem: start) {
        push_heap(open_set, elem, heuristic(elem));
        visited[elem] = path_link<T>();
    }

    // If open_set is empty, then failed to find goal state
    while (!is_empty_heap(open_set)) {
        element<T>* q = peak_heap(open_set);
        pop_heap(open_set);
        T node = q->value;
        if (visited[node].cost < q->priority - heuristic(node)) continue;
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
    int (*f)(int), // f is not heuristic -- it is heuristic + g
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
    push_heap(open_set[0], start, f(start));
    Node begin = {start, 0, f(start)};
    Node init_node_list[] = {begin};
    insert_deduplicate(hash_table, init_node_list, 1);

    int m = -1;

    // If all queues are empty, then failed to find goal state
    bool all_empty = false;
    // we can parallelize this
    for (int i = 0; i < num_queues; i++) {
        all_empty = all_empty || is_empty_heap(open_set[i]);
    }

    while (!all_empty) {
        std::vector<Node> S;

        // for loop in parallel
        for (int i = 0; i < num_queues; i++) {
            if (is_empty_heap(open_set)) continue;

            element<T>* q = peak_heap(open_set[i]);
            pop_heap(open_set[i]);
            int node = q->value;
            if (is_goal(node)) {
                if (m == -1 || f(node) < f(m)) {
                    m = node;
                }
                continue;
            }

            for (link<T> neighbor : get_next(node)) {
                // assert query(closed_set, node) != -1;
                int cost = query(closed_set, node) + neighbor.cost;
                Node new_node = {neighbor.node, cost, f(neighbor.node)};
                S.push_back(new_node);
            }
        }


        // return best path if goal is found and there is no
        // element in any of the pq's that are less than current
        // path to goal.
        if (m != -1) {
            bool all_less = true;
            for (int i = 0; i < num_queues; i++) {
                if (is_empty_heap(open_set[i])) continue;

                int min_cost = peak_heap(open_set[i]).priority;
                if (f(m) < min_cost) {
                    all_less = false;
                    break;
                }
            }

            if (all_less) {
                // return path to goal
                // do this by adding a prev node on Node struct.
            }
        }

        // deduplication section
        bool query_mask[S.size()];
        int num_left = S.size();
        // run in parallel
        for (int i = 0; i < S.size(); i++) {
            query_mask[i] = query_cost_check(open_set, S[i]);
            if (!query_mask[i]) num_left--;
        }

        Node T[num_left];
        int j = 0;
        for (int i = 0; i < S.size(); i++) {
            if (query_mask[i]) {
                T[j] = S[i];
                j++;
            }
        }


        // insert the remaining nodes in parallel in closed array
        // and priority queues.
        insert_deduplicate(closed_set, T, num_left, &open_set);

        bool all_empty = false;
        // we can parallelize this
        for (int i = 0; i < num_queues; i++) {
            all_empty = all_empty || is_empty_heap(open_set[i]);
        }
    }

    return {};
}