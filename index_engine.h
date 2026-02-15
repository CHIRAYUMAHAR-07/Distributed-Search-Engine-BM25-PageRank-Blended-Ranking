#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>

struct Posting
{
    uint32_t doc_id;
    uint32_t term_freq;
};

class IndexEngine
{
public:
    IndexEngine();

    void add_document(uint32_t doc_id, const std::string &content);
    void build();
    void save(const std::string &filepath);
    void load(const std::string &filepath);

    const std::unordered_map<std::string, std::vector<Posting>> &get_index() const;
    uint32_t get_doc_length(uint32_t doc_id) const;
    double get_avg_doc_length() const;
    size_t total_docs() const;

private:
    std::unordered_map<std::string, std::vector<Posting>> inverted_index;
    std::unordered_map<uint32_t, uint32_t> doc_lengths;
    size_t document_count;
    uint64_t total_doc_length;

    std::mutex index_mutex;
};
