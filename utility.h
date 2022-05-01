#include <stdint.h>
#include <stdlib.h>

inline uint64_t factorial(int n) {
    uint64_t product = 1;
    for (int i = 2; i <= n; ++i)
        product *= i;
    return product;
}

inline uint64_t npr(int n, int r) {
    uint64_t product = 1;
    for (int i = n - r + 1; i <= n; ++i)
        product *= i;
    return product;
}

inline uint64_t ncr(int n, int r) {
    return npr(n, r) / factorial(r);
}

inline uint64_t big_rand() {
    uint64_t rand_val = 0;
    for (uint64_t i = UINT64_MAX; i > 1; i /= RAND_MAX) {
        rand_val *= RAND_MAX; 
        rand_val += rand();
    }
    return rand_val;
}

inline uint64_t pow(int base, int exp) {
    if (exp == 0) return 1;
    uint64_t res = 1;
    if (exp % 2 == 1) res = base;
    uint64_t sub_res = pow(base, exp / 2);
    return res * sub_res * sub_res;
}
