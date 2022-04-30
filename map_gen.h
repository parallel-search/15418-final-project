#include "utility.h"

#include<set>
#include<stdexcept>
#include<vector>

struct point {
    int x, y;
};

typedef struct map_config {
    int max_width, max_height;
} map_config_t;

typedef struct map {
    map_config_t config;
    std::vector<point> points; 
    std::vector<std::vector<int>> connections;
    int start_idx, end_idx;
} map_t;

map_t gen_map(
    int max_width,
    int max_height,
    int num_pts,
    int max_edges_per_pt,
    int max_weight,
    int seed
);

