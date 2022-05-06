#include "astar.h"

void print_heap(heap* pq) {
    printf("pq: ");
    for (unsigned int i = 0; i < pq->num_elements; ++i) {
        printf("%d, ", pq->data[i].value);
    }
    printf("\n");
}

void test1() {
    heap* a = new_heap(2);
    int ints[] = {0,1,2,3,4,5,6,7,8,9};
    print_heap(a);
    pop_heap(a);
    print_heap(a);
    printf("empty: %d\n", is_empty_heap(a));
    push_heap(a, ints[3], ints[3]);
    print_heap(a);
    printf("size: %d\n", size_heap(a));
    push_heap(a, ints[2], ints[2]);
    print_heap(a);
    printf("empty: %d\n", is_empty_heap(a));
    printf("size: %d\n", size_heap(a));
    push_heap(a, ints[4], ints[4]);
    print_heap(a);
    printf("size: %d\n", size_heap(a));
    pop_heap(a);
    print_heap(a);
    printf("size: %d\n", size_heap(a));
    push_heap(a, ints[9], ints[9]);
    print_heap(a);
    printf("size: %d\n", size_heap(a));
    push_heap(a, ints[0], ints[0]);
    print_heap(a);
    printf("size: %d\n", size_heap(a));
    push_heap(a, ints[1], ints[1]);
    print_heap(a);
    printf("size: %d\n", size_heap(a));
    pop_heap(a);
    push_heap(a, ints[2], ints[2]);
    print_heap(a);
    push_heap(a, ints[5], ints[5]);
    print_heap(a);
    pop_heap(a);
    print_heap(a);
    push_heap(a, ints[6], ints[6]);
    print_heap(a);
    pop_heap(a);
    print_heap(a);
    pop_heap(a);
    print_heap(a);
    pop_heap(a);
    print_heap(a);
    pop_heap(a);
    print_heap(a);
    pop_heap(a);
    print_heap(a);
}

bool is_goal(int node) {
    return node == 10;
}

std::vector<link> get_next(int node) {
    std::vector<link> neighbors;
    neighbors.push_back(link(node + 1, node));
    return neighbors;
}

double heuristic(int node) {
    return 10 - node;
}

/*
void test2() {
    std::vector<int> start = {0, 1};

    astar(start, is_goal, get_next, heuristic);
}
*/

int main() {
    test1();
    return 0;
}
