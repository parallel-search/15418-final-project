#include <cstdlib>

struct uarray {
    unsigned int size, num_elements;
    unsigned int* data;
};

uarray* new_uarray(unsigned int size=16);

unsigned int get_uarray(uarray* arr, unsigned int idx);

void push_uarray(uarray* arr, unsigned int value);

void pop_uarray(uarray* arr);

unsigned int size_uarray(uarray* arr);

void free_heap(uarray* arr);
