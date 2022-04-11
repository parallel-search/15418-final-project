#include "astar.h"

void print_heap(heap<int>* pq) {
  printf("pq: ");
  for (int i = 0; i < pq->num_elements; ++i) {
    printf("%d, ", pq->data[i].priority);
  }
  printf("\n");
}

int main() {
  heap<int>* a = new_heap<int>(2);
  int ints[] = {0,1,2,3,4,5,6,7,8,9};
  print_heap(a);
  pop_heap(a);
  print_heap(a);
  printf("empty: %d\n", is_empty_heap(a));
  push_heap(a, &ints[3], ints[3]);
  print_heap(a);
  printf("size: %d\n", size_heap(a));
  push_heap(a, &ints[2], ints[2]);
  print_heap(a);
  printf("empty: %d\n", is_empty_heap(a));
  printf("size: %d\n", size_heap(a));
  push_heap(a, &ints[4], ints[4]);
  print_heap(a);
  printf("size: %d\n", size_heap(a));
  pop_heap(a);
  print_heap(a);
  printf("size: %d\n", size_heap(a));
  push_heap(a, &ints[9], ints[9]);
  print_heap(a);
  printf("size: %d\n", size_heap(a));
  push_heap(a, &ints[0], ints[0]);
  print_heap(a);
  printf("size: %d\n", size_heap(a));
  push_heap(a, &ints[1], ints[1]);
  print_heap(a);
  printf("size: %d\n", size_heap(a));
  pop_heap(a);
  push_heap(a, &ints[3], ints[3]);
  print_heap(a);
  push_heap(a, &ints[2], ints[2]);
  print_heap(a);
  push_heap(a, &ints[5], ints[5]);
  print_heap(a);
  pop_heap(a);
  print_heap(a);
  push_heap(a, &ints[6], ints[6]);
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
  return 0;
}
