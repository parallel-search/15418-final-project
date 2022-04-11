#include "heap.h"

#include<iostream>
#include<limits>
#include<list>
#include<queue>
#include<stdexcept>

template <typename T>
std::list<T> astar(T start, T goal, T (*get_next)(T), int depth=std::numeric_limits<int>::infinity());
