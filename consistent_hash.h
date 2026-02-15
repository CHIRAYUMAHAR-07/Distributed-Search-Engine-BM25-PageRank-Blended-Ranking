#pragma once
#include <map>
#include <string>

class ConsistentHash
{
public:
    void add_node(const std::string &node);
    std::string get_node(const std::string &key);

private:
    std::map<size_t, std::string> ring;
};
