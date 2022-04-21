#include <iostream>
#include <cstdlib>

struct HashTable {
    int size;
    int num_elems;
    int *table;
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
    hash_table->table = malloc(sizeof(int) * size);
    for (int i = 0; i < size; i++) {
        hash_table->table[i] = -1;
    }
    return hash_table;
}

int* deduplicate(HashTable *hash_table, int *node_list, int num_nodes) {
    // num_nodes is size of node_list

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

        int ind0 = hash_fn1(node_list[i], hash_table->size);
        int ind1 = hash_fn2(node_list[i], hash_table->size);

        if (hash_table->table[ind0] == node_list[i] || hash_table->table[ind0] == -1) {
            z = 0;
        } else if (hash_table->table[ind1] == node_list[i] || hash_table->table[ind1] == -1) {
            z = 1;
        }

        int t = node_list[i];

        // START ATOMIC
        int temp = t;
        if (z == 0) {
            t = hash_table->table[ind0];
            hash_table->table[ind0] = temp;
        } else if (z == 1) {
            t = hash_table->table[ind1];
            hash_table->table[ind1] = temp;
        }
        // END ATOMIC

        if (t == node_list[i]) {
            dedup_list_mask[i] = 0;
            num_dedup_nodes--;
            continue;
        }

        if (z == 0 && hash_table->table[ind1] == node_list[i]) {
            dedup_list_mask[i] = 0;
            num_dedup_nodes--;
            continue;
        } else if (z == 1 && hash_table->table[ind0] == node_list[i]) {
            dedup_list_mask[i] = 0;
            num_dedup_nodes--;
            continue;
        }
    }

    // create new list of deduped nodes
    int *dedup_list = malloc(sizeof(int) * num_dedup_nodes);
    for (int i = 0; i < num_nodes; i++) {
        if (dedup_list_mask[i] == 1) {
            dedup_list[i] = node_list[i];
            i++;
        }
    }

    return dedup_list;
}

void insert(HashTable *hash_table, int key) {
    throw "Not implemented";
}

int query(HashTable *hash_table, int key) {
    throw "Not implemented";
}