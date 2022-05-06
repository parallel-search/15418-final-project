#include "slider.h"

#include <cstdlib>

struct element {
    unsigned short priority;
    slider_state_t value;
};

struct heap {
    unsigned long size, num_elements;
    element* data;
};

heap* new_heap(unsigned long size=16);

void push_heap(heap* pq, slider_state_t value, unsigned short priority);

void pop_heap(heap* pq);

element* peak_heap(heap* pq);

unsigned long size_heap(heap* pq);

bool is_empty_heap(heap* pq);

void free_heap(heap* pq);
