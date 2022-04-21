#include <iostream>
#include <cstdlib>

struct Node {
    int id;
    int g;
    int f;
}

struct HashTable {
    int size;
    int num_elems;
    Node *table;
};

int hash_fn1(int key, int size) {
    // can modify this later
    return key % size;
}

int hash_fn2(int key, int size) {
    // can modify this later
    return 1 + (key % (size - 1));
}

HashTable *create_hash_table(int size) {
    HashTable *hash_table = malloc(sizeof(HashTable));
    hash_table->size = size;
    hash_table->num_elems = 0;
    hash_table->table = malloc(sizeof(Node) * size);
    for (int i = 0; i < size; i++) {
        hash_table->table[i] = NULL;
    }
    return hash_table;
}

// num_nodes is size of node_list
int* insert_deduplicate(HashTable *hash_table, Node *node_list, int num_nodes) {
    // dedup_list_mask indicates which indices in node_list have been deduped
    // initially all are valid
    int *dedup_list_mask = malloc(sizeof(int) * num_nodes);
    int num_dedup_nodes = num_nodes;
    for (int i = 0; i < num_nodes; i++) {
        dedup_list_mask[i] = 1;
    }

    // run for loop in parallel
    for (int i = 0; i < hash_table->size; i++) {
        // check for unoccupied slots with different hash functions
        int z = 0;

        int ind0 = hash_fn1(node_list[i].id, hash_table->size);
        int ind1 = hash_fn2(node_list[i].id, hash_table->size);

        if (hash_table->table[ind0] == node_list[i] || hash_table->table[ind0] == NULL) {
            z = 0;
        } else if (hash_table->table[ind1] == node_list[i] || hash_table->table[ind1] == NULL) {
            z = 1;
        }

        Node t = node_list[i];

        // START ATOMIC
        Node temp = t;
        if (z == 0) {
            t = hash_table->table[ind0];
            hash_table->table[ind0] = temp;
        } else if (z == 1) {
            t = hash_table->table[ind1];
            hash_table->table[ind1] = temp;
        }
        // END ATOMIC

        if (t.id == node_list[i].id) {
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
    Node *dedup_list = malloc(sizeof(Node) * num_dedup_nodes);
    int j = 0;
    for (int i = 0; i < num_nodes; i++) {
        if (dedup_list_mask[i] == 1) {
            dedup_list[j] = node_list[i];
            j++;
        }
    }

    return dedup_list;
}

// check if node exists in hash table and if it beats lowest g value
// true if this node should be kept for insertion (i.e. either it's not in the table or new node's g is lower)
// false otherwise (i.e. node is in the table and new node's g is higher)
Node* query(HashTable *hash_table, Node node) {
    int ind0 = hash_fn1(node.id, hash_table->size);
    int ind1 = hash_fn2(node.id, hash_table->size);

    // if it exists in hash table 1 and has a lower g value, then remove current node
    bool flag1 = true;
    if (hash_table->table[ind0] != NULL && hash_table->table[ind0].id == node.id) {
        if (hash_table->table[ind0].g < node.g) {
            flag1 = false;
        } else {
            flag1 = true;
        }
    } 
    
    bool flag2 = true;
    if (hash_table->table[ind1] != NULL && hash_table->table[ind1].id == node.id) {
        if (hash_table->table[ind1].g < node.g) {
            flag2 = false;
        } else {
            flag2 = true;
        }
    }

    return flag1 && flag2;
}