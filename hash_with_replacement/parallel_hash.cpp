#include <iostream>
#include <cstdlib>

struct Node {
    int id;
    int g;
    int f;
};

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
    HashTable *hash_table = (HashTable *) malloc(sizeof(HashTable));
    hash_table->size = size;
    hash_table->num_elems = 0;
    hash_table->table = (Node *) malloc(sizeof(Node) * size);
    for (int i = 0; i < size; i++) {
        hash_table->table[i] = {
            -1,
            -1,
            -1
        };
    }
    return hash_table;
}

// num_nodes is size of node_list
Node* insert_deduplicate(HashTable *hash_table, Node *node_list, int num_nodes) {
    // dedup_list_mask indicates which indices in node_list have been deduped
    // initially all are valid
    int *dedup_list_mask = (int *) malloc(sizeof(int) * num_nodes);
    int num_dedup_nodes = num_nodes;
    for (int i = 0; i < num_nodes; i++) {
        dedup_list_mask[i] = 1;
    }

    // run for loop in parallel
    for (int i = 0; i < num_nodes; i++) {
        // check for unoccupied slots with different hash functions
        int z = 0;

        int ind0 = hash_fn1(node_list[i].id, hash_table->size);
        int ind1 = hash_fn2(node_list[i].id, hash_table->size);

        if (hash_table->table[ind0].id == node_list[i].id || hash_table->table[ind0].id == -1) {
            z = 0;
        } else if (hash_table->table[ind1].id == node_list[i].id || hash_table->table[ind1].id == -1) {
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

// check if node exists in hash table and if it beats lowest g value
// true if this node should be kept for insertion (i.e. either it's not in the table or new node's g is lower)
// false otherwise (i.e. node is in the table and new node's g is higher)
bool query(HashTable *hash_table, Node node) {
    int ind0 = hash_fn1(node.id, hash_table->size);
    int ind1 = hash_fn2(node.id, hash_table->size);

    // if it exists in hash table 1 and has a lower g value, then remove current node
    bool flag1 = true;
    if (hash_table->table[ind0].id == node.id && hash_table->table[ind0].g < node.g) {
        flag1 = false;
    }
    printf("%d %d\n", hash_table->table[ind0].id == node.id, hash_table->table[ind0].g < node.g);
    if (!flag1) {
        printf("%d %d %d\n", hash_table->table[ind0].id, hash_table->table[ind0].g, node.g);
    }
    
    bool flag2 = true;
    if (hash_table->table[ind1].id == node.id && hash_table->table[ind1].g < node.g) {
        flag2 = false;
    }
    printf("%d %d\n", hash_table->table[ind1].id == node.id, hash_table->table[ind1].g < node.g);
    if (!flag2) {
        printf("%d %d %d\n", hash_table->table[ind1].id, hash_table->table[ind1].g, node.g);
    }

    return flag1 && flag2;
}

int main() {
    HashTable *hash_table = create_hash_table(20);

    Node node1 = {
        0,
        10,
        10
    };

    Node node2 = {
        1,
        9,
        5
    };

    Node node3 = {
        2,
        3,
        6
    };

    Node node4 = {
        2,
        2,
        10
    };

    Node node5 = {
        1,
        15,
        6
    };

    Node node_list1 [] = {node1, node2, node3};
    int num_elems1 = 3;

    for (int i = 0; i < num_elems1; i++) {
        printf("id: %d\t g: %d\t f: %d\n", node_list1[i].id, node_list1[i].g, node_list1[i].f);
    }

    printf("--------------\n");

    Node *dedup = insert_deduplicate(hash_table, node_list1, num_elems1);

    // for (int i = 0; i < 3; i++) {
    //     printf("id: %d\t g: %d\t f: %d\n", dedup[i].id, dedup[i].g, dedup[i].f);
    // }

    for (int i = 0; i < hash_table->size; i++) {
        Node n = hash_table->table[i];
        printf("id: %d\t g: %d\t f: %d\n", n.id, n.g, n.f);
    }

    Node node_list2 [] = {node4, node5};
    int num_elems2 = 2;

    bool *query_mask = (bool *) malloc(sizeof(bool) * num_elems2);
    int num_left = num_elems2;
    for (int i = 0; i < num_elems2; i++) {
        printf("id: %d\t g: %d\t f: %d\n", node_list2[i].id, node_list2[i].g, node_list2[i].f);
        query_mask[i] = query(hash_table, node_list2[i]);
        if (!query_mask[i]) {
            num_left--;
        }
    }

    Node *node_list3 = (Node *) malloc(sizeof(Node) * num_left);
    int j = 0;
    for (int i = 0; i < num_elems2; i++) {
        if (query_mask[i]) {
            node_list3[j] = node_list2[i];
            j++;
        }
    }

    Node * dedup2 = insert_deduplicate(hash_table, node_list3, num_left);

    for (int i = 0; i < hash_table->size; i++) {
        Node n = hash_table->table[i];
        printf("id: %d\t g: %d\t f: %d\n", n.id, n.g, n.f);
    }

    return 0;
}