#include "map_gen.h"

map_t gen_map(
    int max_width,
    int max_height,
    int num_pts,
    int num_edges,
    int max_weight,
    int seed
) {
    if (num_pts < 2)
        throw std::invalid_argument("not enough points");

    if (num_pts >= max_width * max_height)
        throw std::invalid_argument("too many points");

    if (num_edges < num_pts - 1)
        throw std::invalid_argument("not enough edges per point");

    if (num_edges > ncr(num_pts, 2))
        throw std::invalid_argument("too many edges per point");

    if (max_weight < 1)
        throw std::invalid_argument("too low max weight");

    const uint64_t total_pt_locations = max_width * max_height;
    const uint64_t total_pt_configurations = ncr(total_pt_locations, num_pts);
    const uint64_t total_start_goal_configurations = npr(num_pts, 2);
    const uint64_t mst_configs = factorial(num_pts);
    const uint64_t total_edge_configurations = ncr(
        ncr(num_pts, 2) - (num_pts - 1),
        num_edges - (num_pts - 1)
    );
    const uint64_t total_weight_configurations = pow(max_weight, num_pts);
    srand(seed);

    const uint64_t pt_config = big_rand() % total_pt_configurations;
    const uint64_t start_end = big_rand() % total_start_goal_configurations;
    const uint64_t mst_config = big_rand() % mst_configs;
    const uint64_t edge_config = big_rand() % total_edge_configurations;
    const uint64_t weight_config = big_rand() % total_weight_configurations;

    const int start = start_end / (num_pts - 1);
    const int tmp_end = start_end % num_pts;
    const int end = tmp_end >= start ? tmp_end + 1 : tmp_end;

    int tmp_points[num_pts];
    std::vector<int> indices;
    for (int i = 0; i < num_pts; ++i) {
        tmp_points[i] = num_pts - i - 1;
        indices.push_back(i);
    }

    for (int i = 0; i < pt_config; ++i) {
        int moving = 0;
        while (++tmp_points[moving] >= max_width * max_height - moving) {
            moving++;
        }
        for (int j = moving; j > 0; --j) {
            tmp_points[j - 1] = tmp_points[j] + 1;
        }
    }
    std::vector<point> points;
    for (int& point : tmp_points) {
        points.push_back({ .x=point%max_width, .y=point/max_width });
    }

    std::set<std::pair<int, int>> all_edges;
    for (int from = 0; from < num_pts; ++from) {
        for (int to = from + 1; to < num_pts; ++to) {
            all_edges.insert(std::make_pair(from, to));
        }
    }

    std::vector<std::vector<edge>> connections;
    uint64_t tmp_weight_config = weight_config;
    uint64_t tmp_mst_config = mst_config;
    int tmp_on = tmp_mst_config % num_pts;
    indices.erase(indices.begin()+tmp_on);
    for (int i = num_pts - 1; i >= 0; --i) {
        int path = tmp_mst_config % i;
        int weight = tmp_weight_config % max_weight;
        int v = indices[path];
        indices.erase(indices.begin()+path);
        connections[tmp_on].push_back({.to=v, .weight=weight});
        connections[v].push_back({.to=tmp_on, .weight=weight});
        all_edges.erase(std::make_pair(v, tmp_on));
        all_edges.erase(std::make_pair(tmp_on, v));
        tmp_mst_config /= i;
        tmp_weight_config /= max_weight;
    }

    int tmp_edges[num_edges - num_pts + 1];
    for (int i = 0; i < num_edges; ++i) {
        tmp_edges[i] = num_edges - i - 1;
    }

    for (int i = 0; i < edge_config; ++i) {
        int moving = 0;
        while (++tmp_edges[moving] >= num_edges - num_pts + 1 - moving) {
            moving++;
        }
        for (int j = moving; j > 0; --j) {
            tmp_edges[j - 1] = tmp_edges[j] + 1;
        }
    }
    std::set<std::pair<int, int>>::iterator it = all_edges.begin();
    int tmp_prev = 0;
    for (int i = num_edges - 1; i >= 0; --i) {
        int weight = tmp_weight_config % max_weight;
        for (int j = 0; j < tmp_edges[i] - tmp_prev; ++j) {
            it++;
        }
        tmp_prev = tmp_edges[i];
        int v1 = (*it).first;
        int v2 = (*it).second;
        connections[v1].push_back({.to=v2, .weight=weight});
        connections[v2].push_back({.to=v1, .weight=weight});
        tmp_weight_config /= max_weight;
    }

    map_t map = {
        .config={
            .max_width = max_width,
            .max_height = max_height,
        },
        .points = points,
        .connections = connections,
        .start_idx = start,
        .end_idx = end,
    };
    return map;
}

int main(int argc, char *argv[]) {
    // gen_map();
}