#include "utility.h"

#include <set>
#include <stdexcept>
#include <vector>

// Q1
struct point {
    unsigned int x, y;
};

struct edge {
    unsigned int to;
    unsigned int weight;
};

typedef struct map_config {
    unsigned int max_width, max_height;
} map_config_t;

typedef struct map {
    map_config_t config;
    std::vector<point> points; 
    std::vector<std::vector<edge>> connections;
    unsigned int start_idx, end_idx;
} map_t;

map_t gen_map(
    unsigned int max_width,
    unsigned int max_height,
    unsigned int num_pts,
    unsigned int num_edges,
    unsigned int max_weight,
    unsigned int seed
);
