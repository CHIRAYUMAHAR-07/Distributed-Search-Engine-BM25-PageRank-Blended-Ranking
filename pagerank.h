#pragma once
#include <unordered_map>
#include <vector>

class PageRank
{
public:
    void build_graph(
        const std::unordered_map<uint32_t,
                                 std::vector<uint32_t>> &adjacency);

    void compute(int iterations = 15, double damping = 0.85);

    double get_rank(uint32_t doc_id) const;

private:
    std::unordered_map<uint32_t, std::vector<uint32_t>> graph;
    std::unordered_map<uint32_t, double> ranks;
};
