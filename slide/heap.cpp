#include "heap.h"

heap* new_heap(unsigned long size) {
    heap* pq = (heap*)malloc(sizeof(heap));
    pq->size = size;
    pq->num_elements = 0;
    pq->data = (element*)malloc(pq->size * sizeof(element));
    return pq;
}

void push_heap(heap* pq, slider_state_t value, unsigned short priority) {
    // Resize vector
    if (pq->size == pq->num_elements) {
        unsigned long new_size = pq->size * 2;
        element* new_data = (element*)malloc(new_size * sizeof(element));
        for (unsigned long i = 0; i < pq->num_elements; ++i)
            new_data[i] = pq->data[i];
        free(pq->data);
        pq->data = new_data;
        pq->size = new_size;
    }

    // Append element
    element elem;
    elem.priority = priority;
    elem.value = value;
    pq->data[pq->num_elements] = elem;

    // Rotate elements
    unsigned long idx = pq->num_elements;
    while (idx > 0 && pq->data[idx].priority < pq->data[(idx - 1) / 2].priority) {
        element tmp = pq->data[idx];
        pq->data[idx] = pq->data[(idx - 1) / 2];
        pq->data[(idx - 1) / 2] = tmp;
        idx = (idx - 1) / 2;
    }

    ++pq->num_elements;
}

void pop_heap(heap* pq) {
    --pq->num_elements;

    // Move last leaf to root
    pq->data[0] = pq->data[pq->num_elements];

    // Rotate elements
    unsigned long idx = 0;
    while (true) {
        unsigned long left = idx * 2 + 1;
        unsigned long right = left + 1;

        // Done if at leaf or has lower priority than children
        if (left >= pq->num_elements) break;
        if (pq->data[left].priority > pq->data[idx].priority
          && right >= pq->num_elements) break;

        element tmp = pq->data[idx];
        unsigned long next;
        if (right >= pq->num_elements || pq->data[left].priority < pq->data[right].priority)
            next = left;
        else if (pq->data[left].priority >= pq->data[right].priority)
            next = right;
        else break;
        pq->data[idx] = pq->data[next];
        pq->data[next] = tmp;
        idx = next;
    }

    // Resize vector
    if (pq->size / 4 >= pq->num_elements) {
        unsigned long new_size = pq->size / 2;
        element* new_data = (element*)malloc(new_size * sizeof(element));
        for (unsigned long i = 0; i < pq->num_elements; ++i)
            new_data[i] = pq->data[i];
        free(pq->data);
        pq->data = new_data;
        pq->size = new_size;
    }
}

element* peak_heap(heap* pq) {
    if (pq->num_elements == 0) return NULL;
    return pq->data;
}

unsigned long size_heap(heap* pq) {
    return pq->num_elements;
}

bool is_empty_heap(heap* pq) {
    return pq->num_elements == 0;
}

void free_heap(heap* pq) {
    free(pq->data);
    free(pq);
}
