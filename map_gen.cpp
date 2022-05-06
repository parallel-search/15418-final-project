#include "map_gen.h"

map_t gen_map(
    unsigned int max_width,
    unsigned int max_height,
    unsigned int num_pts,
    unsigned int num_edges,
    unsigned int max_weight,
    unsigned int seed
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
    const uint64_t total_edges = ncr(num_pts, 2);
    const uint64_t total_edge_configurations = ncr(
        total_edges - (num_pts - 1),
        num_edges - (num_pts - 1)
    );
    const uint64_t total_weight_configurations = pow(max_weight, num_pts);
    srand(seed);

    const uint64_t pt_config = big_rand() % total_pt_configurations;
    const uint64_t start_end = big_rand() % total_start_goal_configurations;
    const uint64_t mst_config = big_rand() % mst_configs;
    const uint64_t edge_config = big_rand() % total_edge_configurations;
    const uint64_t weight_config = big_rand() % total_weight_configurations;

    const unsigned int start = start_end / (num_pts - 1);
    const unsigned int tmp_end = start_end % (num_pts - 1);
    const unsigned int end = tmp_end >= start ? tmp_end + 1 : tmp_end;

    std::vector<unsigned int> indices;
    for (unsigned int i = 0; i < num_pts; ++i) {
        indices.push_back(i);
    }

    unsigned int tmp_points[num_pts];
    kth_ncr(tmp_points, max_width * max_height, num_pts, pt_config);

    std::vector<point> points;
    for (unsigned int& point : tmp_points) {
        points.push_back({ .x=point%max_width, .y=point/max_width });
    }

    std::set<std::pair<unsigned int, unsigned int>> all_edges;
    for (unsigned int from = 0; from < num_pts; ++from) {
        for (unsigned int to = from + 1; to < num_pts; ++to) {
            all_edges.insert(std::make_pair(from, to));
        }
    }

    std::vector<std::vector<edge>> connections(num_pts);
    uint64_t tmp_weight_config = weight_config;
    uint64_t tmp_mst_config = mst_config;
    unsigned int tmp_on = tmp_mst_config % num_pts;
    tmp_mst_config /= num_pts;
    indices.erase(indices.begin()+tmp_on);
    for (unsigned int i = num_pts - 1; i > 0; --i) {
        unsigned int path = tmp_mst_config % i;
        unsigned int weight = tmp_weight_config % max_weight + 1;
        unsigned int v = indices[path];
        indices.erase(indices.begin()+path);
        connections[tmp_on].push_back({.to=v,.weight=weight});
        connections[v].push_back({.to=tmp_on,.weight=weight});
        all_edges.erase(std::make_pair(v, tmp_on));
        all_edges.erase(std::make_pair(tmp_on, v));
        tmp_mst_config /= i;
        tmp_weight_config /= max_weight;
    }

    unsigned int tmp_edges[num_edges - num_pts + 1];
    kth_ncr(
        tmp_edges,
        total_edges - num_pts + 1,
        num_edges - num_pts + 1,
        edge_config
    );

    std::set<std::pair<unsigned int, unsigned int>>::iterator it
        = all_edges.begin();
    unsigned int tmp_prev = 0;
    for (int i = num_edges - num_pts; i >= 0; --i) {
        unsigned int weight = tmp_weight_config % max_weight + 1;
        for (unsigned int j = 0; j < tmp_edges[i] - tmp_prev; ++j) {
            it++;
        }
        tmp_prev = tmp_edges[i];
        unsigned int v1 = (*it).first;
        unsigned int v2 = (*it).second;
        connections[v1].push_back({.to=v2,.weight=weight});
        connections[v2].push_back({.to=v1,.weight=weight});
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
  if (argc != 7)
      throw std::invalid_argument(
          "expected: max_width max_height num_pts num_edges max_weight seed"
      );
  int max_width = atoi(argv[1]);
  int max_height = atoi(argv[2]);
  int num_pts = atoi(argv[3]);
  int num_edges = atoi(argv[4]);
  int max_weight = atoi(argv[5]);
  int seed = atoi(argv[6]);
  map_t map = gen_map(
      max_width,
      max_height,
      num_pts,
      num_edges,
      max_weight,
      seed
  );
  std::printf("%d %d\n", map.config.max_width, map.config.max_height);
  std::printf("%lu\n", map.points.size());
  for (point& pt : map.points) {
      std::printf("%d %d\n", pt.x, pt.y);
  }
  std::printf("%d %d\n", map.start_idx, map.end_idx);
  for (std::vector<edge>& edges : map.connections) {
      std::printf("%lu\n", edges.size());
      for (edge& conn : edges) {
          std::printf("%d %d\n", conn.to, conn.weight);
      }
  }
}
