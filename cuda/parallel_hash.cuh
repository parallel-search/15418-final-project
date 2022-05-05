#include <iostream>
#include <cstdlib>

#include "heap.cuh"
#include "uarray.cuh"

struct Node {
    slider_state_t id;
    int prev_action; // the action to get from prev_id to id
    slider_state_t prev_id;
    int g;
    int f;
    Node() {
        slider_state_t def = slider_state_t();
        Node(def, -1, id, -1, -1);
    }
    Node(slider_state_t id, int g, int f) {
        Node(id, -1, id, g, f);
    }
    Node(slider_state_t id, int prev_action, slider_state_t prev_id, int g, int f) {
        this->id = id;
        this->prev_action = prev_action;
        this->prev_id = prev_id;
        this->g = g;
        this->f = f;
    }
};

struct HashTable {
    int size;
    int num_elems;
    Node *table;
};

unsigned long slider_hash(slider_state_t state) {
    unsigned long hash = 0;
    unsigned long base = 1;
    for (unsigned char i = 0; i < DIM_X * DIM_Y; ++i) {
        hash += state.board[i] * base;
        base *= DIM_X * DIM_Y;
    }
    return hash;
}

__device__ inline int hash_fn1(slider_state_t slide_key, int size) {
    // can modify this later
    unsigned long key = slider_hash(slide_key);
    return key % size;
}

__device__ inline int hash_fn2(slider_state_t slide_key, int size) {
    // can modify this later
    unsigned long key = slider_hash(slide_key);
    return 1 + (key % (size - 1));
}

__device__ inline HashTable *create_hash_table(int size) {
    HashTable *hash_table = (HashTable *) malloc(sizeof(HashTable));
    hash_table->size = size;
    hash_table->num_elems = 0;
    hash_table->table = (Node *) malloc(sizeof(Node) * size);
    for (int i = 0; i < size; i++) {
        slider_state_t new_state = slider_state();
        hash_table->table[i] = Node(new_state, -1, -1);
    }
    return hash_table;
}

// check if node exists in hash table and if it beats lowest g value
// true if this node should be kept for insertion (i.e. either it's not in the table or new node's g is lower)
// false otherwise (i.e. node is in the table and new node's g is higher)
__device__ inline bool query_cost_check(HashTable *hash_table, Node node) {
    int ind0 = hash_fn1(node.id, hash_table->size);
    int ind1 = hash_fn2(node.id, hash_table->size);

    // if it exists in hash table 1 and has a lower g value, then remove current node
    bool flag1 = true;
    if (hash_table->table[ind0].id == node.id && hash_table->table[ind0].g < node.g) {
        flag1 = false;
    }
    
    bool flag2 = true;
    if (hash_table->table[ind1].id == node.id && hash_table->table[ind1].g < node.g) {
        flag2 = false;
    }

    return flag1 && flag2;
}

// returns Node corresponding to id
__device__ inline Node query(HashTable *hash_table, slider_state_t node) {
    int ind0 = hash_fn1(node, hash_table->size);
    int ind1 = hash_fn2(node, hash_table->size);

    if (hash_table->table[ind0].id == node) {
        return hash_table->table[ind0];
    } else if (hash_table->table[ind1].id == node) {
        return hash_table->table[ind1];
    } else {
        // indicate node does not exist
        return Node(slider_state(), -1, -1);
    }
}

// num_nodes is size of node_list
// we have only one heap as input which will be decided by the thread in
// the kernel function
__device__ inline Node* insert_deduplicate(HashTable *hash_table, Node *node_list, int num_nodes, heap* open_set) {
    // dedup_list_mask indicates which indices in node_list have been deduped
    // initially all are valid
    int *dedup_list_mask = (int *) malloc(sizeof(int) * num_nodes);
    int num_dedup_nodes = num_nodes;
    for (int i = 0; i < num_nodes; i++) {
        dedup_list_mask[i] = 1;
    }

    // run for loop in parallel
    for (int i = 0; i < num_nodes; i++) {
        // insert in random priority queue
        push_heap(open_set, node_list[i].id, node_list[i].f);

        // check for unoccupied slots with different hash functions
        int z = 0;

        int ind0 = hash_fn1(node_list[i].id, hash_table->size);
        int ind1 = hash_fn2(node_list[i].id, hash_table->size);

        if (hash_table->table[ind0].id == node_list[i].id || hash_table->table[ind0].id.zero_idx == DIM_X * DIM_Y) {
            z = 0;
        } else if (hash_table->table[ind1].id == node_list[i].id || hash_table->table[ind1].id.zero_idx == DIM_X * DIM_Y) {
            z = 1;
        }

        Node t = node_list[i];

        // START ATOMIC
        // need to maintain that among the same nodes in node_list that the node
        // with the smallest cost is the one that is the one that we keep
        bool to_keep = query_cost_check(hash_table, t);
        Node temp = t;
        if (z == 0 && to_keep) {
            t = hash_table->table[ind0];
            hash_table->table[ind0] = temp;
        } else if (z == 1 && to_keep) {
            t = hash_table->table[ind1];
            hash_table->table[ind1] = temp;
        }
        // END ATOMIC

        if (t.id == node_list[i].id && to_keep) {
            dedup_list_mask[i] = 0;
            num_dedup_nodes--;
            continue;
        }

        if (z == 0 && hash_table->table[ind1].id == node_list[i].id) {
            dedup_list_mask[i] = 0;
            num_dedup_nodes--;
            continue;
        } else if (z == 1 && hash_table->table[ind0].id == node_list[i].id) {
            dedup_list_mask[i] = 0;
            num_dedup_nodes--;
            continue;
        }
    }

    // create new list of deduped nodes
    Node *dedup_list = (Node *) malloc(sizeof(Node) * num_dedup_nodes);
    int j = 0;
    for (int i = 0; i < num_nodes; i++) {
        if (dedup_list_mask[i] == 1) {
            dedup_list[j] = node_list[i];
            j++;
        }
    }

    return dedup_list;
}
