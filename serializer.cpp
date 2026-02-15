#include "serializer.h"
#include <fstream>

void Serializer::save_index(
    const std::string &filepath,
    const std::unordered_map<std::string, std::vector<Posting>> &index,
    const std::unordered_map<uint32_t, uint32_t> &doc_lengths,
    size_t doc_count,
    uint64_t total_doc_length)
{
    std::ofstream out(filepath, std::ios::binary);

    out.write(reinterpret_cast<const char *>(&doc_count), sizeof(doc_count));
    out.write(reinterpret_cast<const char *>(&total_doc_length), sizeof(total_doc_length));

    size_t index_size = index.size();
    out.write(reinterpret_cast<const char *>(&index_size), sizeof(index_size));

    for (const auto &[term, postings] : index)
    {
        size_t term_size = term.size();
        out.write(reinterpret_cast<const char *>(&term_size), sizeof(term_size));
        out.write(term.c_str(), term_size);

        size_t postings_size = postings.size();
        out.write(reinterpret_cast<const char *>(&postings_size), sizeof(postings_size));

        for (const auto &posting : postings)
        {
            out.write(reinterpret_cast<const char *>(&posting.doc_id), sizeof(posting.doc_id));
            out.write(reinterpret_cast<const char *>(&posting.term_freq), sizeof(posting.term_freq));
        }
    }

    size_t doc_len_size = doc_lengths.size();
    out.write(reinterpret_cast<const char *>(&doc_len_size), sizeof(doc_len_size));

    for (const auto &[doc_id, length] : doc_lengths)
    {
        out.write(reinterpret_cast<const char *>(&doc_id), sizeof(doc_id));
        out.write(reinterpret_cast<const char *>(&length), sizeof(length));
    }
}

void Serializer::load_index(
    const std::string &filepath,
    std::unordered_map<std::string, std::vector<Posting>> &index,
    std::unordered_map<uint32_t, uint32_t> &doc_lengths,
    size_t &doc_count,
    uint64_t &total_doc_length)
{
    std::ifstream in(filepath, std::ios::binary);

    in.read(reinterpret_cast<char *>(&doc_count), sizeof(doc_count));
    in.read(reinterpret_cast<char *>(&total_doc_length), sizeof(total_doc_length));

    size_t index_size;
    in.read(reinterpret_cast<char *>(&index_size), sizeof(index_size));

    for (size_t i = 0; i < index_size; i++)
    {
        size_t term_size;
        in.read(reinterpret_cast<char *>(&term_size), sizeof(term_size));

        std::string term(term_size, ' ');
        in.read(&term[0], term_size);

        size_t postings_size;
        in.read(reinterpret_cast<char *>(&postings_size), sizeof(postings_size));

        std::vector<Posting> postings(postings_size);

        for (size_t j = 0; j < postings_size; j++)
        {
            in.read(reinterpret_cast<char *>(&postings[j].doc_id), sizeof(uint32_t));
            in.read(reinterpret_cast<char *>(&postings[j].term_freq), sizeof(uint32_t));
        }

        index[term] = postings;
    }

    size_t doc_len_size;
    in.read(reinterpret_cast<char *>(&doc_len_size), sizeof(doc_len_size));

    for (size_t i = 0; i < doc_len_size; i++)
    {
        uint32_t doc_id, length;
        in.read(reinterpret_cast<char *>(&doc_id), sizeof(uint32_t));
        in.read(reinterpret_cast<char *>(&length), sizeof(uint32_t));
        doc_lengths[doc_id] = length;
    }
}
