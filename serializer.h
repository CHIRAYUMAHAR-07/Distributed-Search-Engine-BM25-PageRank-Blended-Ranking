#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include "index_engine.h"

class Serializer
{
public:
    static void save_index(
        const std::string &filepath,
        const std::unordered_map<std::string, std::vector<Posting>> &index,
        const std::unordered_map<uint32_t, uint32_t> &doc_lengths,
        size_t doc_count,
        uint64_t total_doc_length);

    static void load_index(
        const std::string &filepath,
        std::unordered_map<std::string, std::vector<Posting>> &index,
        std::unordered_map<uint32_t, uint32_t> &doc_lengths,
        size_t &doc_count,
        uint64_t &total_doc_length);
};
