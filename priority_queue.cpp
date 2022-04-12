#include <thrust/host_vector.h>
#include <thrust/device_vector.h>

struct Node {
    int idx; // index of the last node in the path
    int f; // f(node)
    int g; // g(node)
    int *prev; // pointed to previous node that expanded the node
}

class PriorityQueue {
public:
    host_vector<Node> nodes;

    PriorityQueue();
    ~PriorityQueue();
    void push(int value);
    int pop();
    int top();
    bool empty();
    int size();
}