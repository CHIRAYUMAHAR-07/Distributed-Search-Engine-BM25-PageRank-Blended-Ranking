#include "pagerank.h"

void PageRank::build_graph(
    const std::unordered_map<uint32_t,
                             std::vector<uint32_t>> &adjacency)
{
    graph = adjacency;
    double init = 1.0 / graph.size();

    for (auto &[doc, _] : graph)
    {
        ranks[doc] = init;
    }
}

void PageRank::compute(int iterations, double d)
{
    size_t N = graph.size();

    for (int iter = 0; iter < iterations; iter++)
    {
        std::unordered_map<uint32_t, double> new_ranks;

        for (auto &[doc, _] : graph)
            new_ranks[doc] = (1 - d) / N;

        for (auto &[doc, out_links] : graph)
        {
            for (auto &dest : out_links)
            {
                new_ranks[dest] +=
                    d * (ranks[doc] / out_links.size());
            }
        }
        ranks = new_ranks;
    }
}

double PageRank::get_rank(uint32_t doc_id) const
{
    if (ranks.find(doc_id) == ranks.end())
        return 0.0;
    return ranks.at(doc_id);
}
