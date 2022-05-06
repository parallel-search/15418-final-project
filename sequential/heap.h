#include <cstdlib>

struct element {
    double priority;
    int value;
};

struct heap {
    unsigned int size, num_elements;
    element* data;
};

heap* new_heap(unsigned int size=16);

void push_heap(heap* pq, int value, double priority);

void pop_heap(heap* pq);

element* peak_heap(heap* pq);

int size_heap(heap* pq);

bool is_empty_heap(heap* pq);

void free_heap(heap* pq);
