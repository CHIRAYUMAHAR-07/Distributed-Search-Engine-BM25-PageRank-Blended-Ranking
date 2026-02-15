#include "varint.h"

void VarInt::encode_uint32(uint32_t value, std::vector<uint8_t> &out)
{
    while (value > 127)
    {
        out.push_back((value & 0x7F) | 0x80);
        value >>= 7;
    }
    out.push_back(value & 0x7F);
}

uint32_t VarInt::decode_uint32(const std::vector<uint8_t> &data, size_t &offset)
{
    uint32_t result = 0;
    int shift = 0;

    while (true)
    {
        uint8_t byte = data[offset++];
        result |= (byte & 0x7F) << shift;
        if (!(byte & 0x80))
            break;
        shift += 7;
    }
    return result;
}

std::vector<uint8_t> VarInt::encode_postings(
    const std::vector<uint32_t> &doc_ids,
    const std::vector<uint32_t> &term_freqs)
{
    std::vector<uint8_t> out;
    uint32_t prev = 0;

    for (size_t i = 0; i < doc_ids.size(); i++)
    {
        uint32_t delta = doc_ids[i] - prev;
        encode_uint32(delta, out);
        encode_uint32(term_freqs[i], out);
        prev = doc_ids[i];
    }
    return out;
}
