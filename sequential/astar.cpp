#include "astar.h"

std::vector<int> astar(
    std::vector<int> start,
    bool (*is_goal)(int),
    std::vector<link> (*get_next)(int),
    double (*heuristic)(int)
) {
    // Priority queue based on cost plus heuristic
    heap* open_set = new_heap();

    // Hashmap which stores minimum costs from sources and parent
    std::unordered_map<int, path_link> visited;

    // Initialize data structures
    for (int elem: start) {
        push_heap(open_set, elem, heuristic(elem));
        visited[elem] = path_link();
    }

    // If open_set is empty, then failed to find goal state
    while (!is_empty_heap(open_set)) {
        element* q = peak_heap(open_set);
        pop_heap(open_set);
        int node = q->value;
        if (visited[node].cost < q->priority - heuristic(node)) continue;
        if (is_goal(node)) {
            std::vector<int> path(visited[node].length);
            int on = node;
            for (int i = visited[node].length - 1; i >= 0; --i) {
                path[i] = on;
                on = visited[on].node;
            }
            return path;
        }
        for (link neighbor : get_next(node)) {
            double cost = visited[node].cost + neighbor.cost;
            if (visited.find(neighbor.node) == visited.end() || visited[neighbor.node].cost > cost) {
                visited[neighbor.node] = path_link(node, cost, visited[node].length + 1);
                push_heap(open_set, neighbor.node, cost + heuristic(neighbor.node));
            }
        }
    }

    return {};
}
