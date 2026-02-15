#include "consistent_hash.h"
#include <functional>

void ConsistentHash::add_node(const std::string &node)
{
    size_t hash = std::hash<std::string>{}(node);
    ring[hash] = node;
}

std::string ConsistentHash::get_node(const std::string &key)
{
    size_t hash = std::hash<std::string>{}(key);
    auto it = ring.lower_bound(hash);
    if (it == ring.end())
        return ring.begin()->second;
    return it->second;
}
