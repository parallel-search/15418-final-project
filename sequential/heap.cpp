#include "heap.h"

heap* new_heap(unsigned int size) {
    heap* pq = (heap*)malloc(sizeof(heap));
    pq->size = size;
    pq->num_elements = 0;
    pq->data = (element*)malloc(pq->size * sizeof(element));
    return pq;
}

void push_heap(heap* pq, int value, double priority) {
    // Resize vector
    if (pq->size == pq->num_elements) {
        int new_size = pq->size * 2;
        element* new_data = (element*)malloc(new_size * sizeof(element));
        for (unsigned int i = 0; i < pq->num_elements; ++i)
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
    int idx = pq->num_elements;
    while (idx > 0 && pq->data[idx].priority < pq->data[(idx - 1) / 2].priority) {
        element tmp = pq->data[idx];
        pq->data[idx] = pq->data[(idx - 1) / 2];
        pq->data[(idx - 1) / 2] = tmp;
        idx = (idx - 1) / 2;
    }

    ++pq->num_elements;
}

void pop_heap(heap* pq) {
    if (pq->num_elements == 0) return;
    --pq->num_elements;

    // Move last leaf to root
    pq->data[0] = pq->data[pq->num_elements];

    // Rotate elements
    unsigned int idx = 0;
    while (true) {
        unsigned int left = idx * 2 + 1;
        unsigned int right = left + 1;

        // Done if at leaf or has lower priority than children
        if (left >= pq->num_elements) break;
        if (pq->data[left].priority > pq->data[idx].priority
          && right >= pq->num_elements) break;

        element tmp = pq->data[idx];
        unsigned int next;
        if (right >= pq->num_elements || pq->data[left].priority < pq->data[right].priority)
            next = left;
        else if (pq->data[left].priority > pq->data[right].priority)
            next = right;
        else break;
        pq->data[idx] = pq->data[next];
        pq->data[next] = tmp;
        idx = next;
    }

    // Resize vector
    if (pq->size / 4 >= pq->num_elements) {
        int new_size = pq->size / 2;
        element* new_data = (element*)malloc(new_size * sizeof(element));
        for (unsigned int i = 0; i < pq->num_elements; ++i)
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

int size_heap(heap* pq) {
    return pq->num_elements;
}

bool is_empty_heap(heap* pq) {
    return pq->num_elements == 0;
}

void free_heap(heap* pq) {
    free(pq->data);
    free(pq);
}
