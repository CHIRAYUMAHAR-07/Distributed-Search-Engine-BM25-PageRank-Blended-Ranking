#pragma once
#include <vector>
#include <cstdint>

class VarInt
{
public:
    static void encode_uint32(uint32_t value, std::vector<uint8_t> &out);
    static uint32_t decode_uint32(const std::vector<uint8_t> &data, size_t &offset);

    static std::vector<uint8_t> encode_postings(
        const std::vector<uint32_t> &doc_ids,
        const std::vector<uint32_t> &term_freqs);

    static void decode_postings(
        const std::vector<uint8_t> &data,
        std::vector<uint32_t> &doc_ids,
        std::vector<uint32_t> &term_freqs);
};
