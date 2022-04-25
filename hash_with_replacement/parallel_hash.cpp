#include "parallel_hash.h"

int main() {
    HashTable *hash_table = create_hash_table(20);

    Node node1 = Node(0, 10, 10);

    Node node2 = Node(1, 9, 5);

    Node node3 = Node(2, 3, 6);

    Node node4 = Node(2, 2, 10);

    Node node5 = Node(1, 15, 6);

    Node node_list1 [] = {node1, node2, node3};
    int num_elems1 = 3;

    Node *dedup = insert_deduplicate(hash_table, node_list1, num_elems1);

    for (int i = 0; i < hash_table->size; i++) {
        Node n = hash_table->table[i];
        printf("id: %d\t g: %d\t f: %d\n", n.id, n.g, n.f);
    }

    Node node_list2 [] = {node4, node5};
    int num_elems2 = 2;

    printf("---------------------------\n");

    bool *query_mask = (bool *) malloc(sizeof(bool) * num_elems2);
    int num_left = num_elems2;
    for (int i = 0; i < num_elems2; i++) {
        printf("id: %d\t g: %d\t f: %d\n", node_list2[i].id, node_list2[i].g, node_list2[i].f);
        query_mask[i] = query_cost_check(hash_table, node_list2[i]);
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

    printf("---------------------------\n");

    for (int i = 0; i < hash_table->size; i++) {
        Node n = hash_table->table[i];
        printf("id: %d\t g: %d\t f: %d\n", n.id, n.g, n.f);
    }

    return 0;
}