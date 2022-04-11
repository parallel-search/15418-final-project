#include <cstdlib>

template <typename T>
struct element {
  double priority;
  T* value;
};

template <typename T>
struct heap {
  int size, num_elements;
  element<T>* data;
};

template <typename T>
heap<T>* new_heap(int size=16) {
  heap<T>* pq = (heap<T>*)malloc(sizeof(heap<T>));
  pq->size = size;
  pq->num_elements = 0;
  pq->data = (element<T>*)malloc(pq->size * sizeof(element<T>));
  return pq;
}

template <typename T>
void push_heap(heap<T>* pq, T* value, double priority) {
  // Resize vector
  if (pq->size == pq->num_elements) {
    int new_size = pq->size * 2;
    element<T>* new_data = (element<T>*)malloc(new_size * sizeof(element<T>));
    for (int i = 0; i < pq->num_elements; ++i) new_data[i] = pq->data[i];
    free(pq->data);
    pq->data = new_data;
    pq->size = new_size;
  }

  // Append element
  element<T> elem;
  elem.priority = priority;
  elem.value = value;
  pq->data[pq->num_elements] = elem;

  // Rotate elements
  int idx = pq->num_elements;
  while (idx > 0 && pq->data[idx].priority < pq->data[(idx - 1) / 2].priority) {
    element<T> tmp = pq->data[idx];
    pq->data[idx] = pq->data[(idx - 1) / 2];
    pq->data[(idx - 1) / 2] = tmp;
    idx = (idx - 1) / 2;
  }

  ++pq->num_elements;
}

template <typename T>
void pop_heap(heap<T>* pq) {
  if (pq->num_elements == 0) return;
  --pq->num_elements;

  // Move last leaf to root
  pq->data[0] = pq->data[pq->num_elements];

  // Rotate elements
  int idx = 0;
  while (true) {
    int left = idx * 2 + 1;
    int right = left + 1;

    // Done if at leaf or has lower priority than children
    if (left >= pq->num_elements) break;
    if (pq->data[left].priority > pq->data[idx].priority
      && right >= pq->num_elements) break;

    element<T> tmp = pq->data[idx];
    if (right >= pq->num_elements || pq->data[left].priority < pq->data[right].priority) {
      pq->data[idx] = pq->data[left];
      pq->data[left] = tmp;
      idx = left;
    } else if (pq->data[left].priority > pq->data[right].priority) {
      pq->data[idx] = pq->data[right];
      pq->data[right] = tmp;
      idx = right;
    } else break;
  }

  // Resize vector
  if (pq->size / 4 >= pq->num_elements) {
    int new_size = pq->size / 2;
    element<T>* new_data = (element<T>*)malloc(new_size * sizeof(element<T>));
    for (int i = 0; i < pq->num_elements; ++i) new_data[i] = pq->data[i];
    free(pq->data);
    pq->data = new_data;
    pq->size = new_size;
  }
}

template <typename T>
element<T>* peak_heap(heap<T>* pq) {
  if (pq->num_elements == 0) return NULL;
  return pq->data;
}

template <typename T>
int size_heap(heap<T>* pq) {
  return pq->num_elements;
}

template <typename T>
bool is_empty_heap(heap<T>* pq) {
  return pq->num_elements == 0;
}

template <typename T>
void free_heap(heap<T>* pq) {
  free(pq->data);
  free(pq);
}