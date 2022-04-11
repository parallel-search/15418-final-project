#include "astar.h"
#include "heap.h"

int main() {
  heap<int> a;
  int b = 4;
  push(a, &b, 0);
  return 0;
}
