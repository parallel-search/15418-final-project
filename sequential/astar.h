#include "heap.h"

#include<iostream>
#include<limits>
#include<stdexcept>
#include<unordered_map>
#include<vector>

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
