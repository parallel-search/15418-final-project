#include <cstdlib>

struct uarray {
    unsigned int size, num_elements;
    unsigned char* data;
};

uarray* new_uarray(unsigned int size=16);

unsigned char get_uarray(uarray* arr, unsigned int idx);

void push_uarray(uarray* arr, unsigned char value);

void pop_uarray(uarray* arr);

void reverse_uarray(uarray* arr);

unsigned int size_uarray(uarray* arr);

void free_uarray(uarray* arr);
