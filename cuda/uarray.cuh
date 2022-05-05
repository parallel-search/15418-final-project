#include <cstdlib>

struct uarray {
    unsigned int size, num_elements;
    unsigned int* data;
};

uarray* new_uarray(unsigned int size) {
    uarray* arr= (uarray*)malloc(sizeof(uarray));
    arr->size = size;
    arr->num_elements = 0;
    arr->data = (unsigned int*)malloc(arr->size * sizeof(unsigned int));
    return arr;
}

unsigned int get_uarray(uarray* arr, unsigned int idx) {
    return arr->data[idx];
}

void push_uarray(uarray* arr, unsigned int value) {
    if (arr->size == arr->num_elements) {
        unsigned int new_size = arr->size * 2;
        unsigned int* new_data =
            (unsigned int*)malloc(new_size * sizeof(unsigned int));
        for (unsigned int i = 0; i < arr->num_elements; ++i)
            new_data[i] = arr->data[i];
        free(arr->data);
        arr->data = new_data;
        arr->size = new_size;
    }
    arr->data[arr->num_elements] = value;
    ++arr->num_elements;
}

void pop_uarray(uarray* arr) {
    --arr->num_elements;

    // Resize vector
    if (arr->size / 4 >= arr->num_elements) {
        int new_size = arr->size / 2;
        unsigned int* new_data =
            (unsigned int*)malloc(new_size * sizeof(unsigned int));
        for (unsigned int i = 0; i < arr->num_elements; ++i)
            new_data[i] = arr->data[i];
        free(arr->data);
        arr->data = new_data;
        arr->size = new_size;
    }
}

unsigned int size_uarray(uarray* arr) {
    return arr->num_elements;
}

void free_uarray(uarray* arr) {
    free(arr->data);
    free(arr);
}