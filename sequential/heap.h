template <typename T>
struct heap {
  struct element {
    int priority;
    T* value;
  };
  int size, num_elements;
  element data[];
};

template <typename T>
void push(heap<T>& pq, T* elem, int priority);

//template <typename T>
//std::list<T> astar(T start, T goal, T (*get_next)(T), int depth=std::numeric_limits<int>::infinity());
//
//
//
//
// children: i*2+1, i*2+2
// parent: (i-1)//2
