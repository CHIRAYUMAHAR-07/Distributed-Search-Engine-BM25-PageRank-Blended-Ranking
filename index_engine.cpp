#include "index_engine.h"
#include "tokenizer.h"
#include "serializer.h"

IndexEngine::IndexEngine()
{
    document_count = 0;
    total_doc_length = 0;
}

void IndexEngine::add_document(uint32_t doc_id, const std::string &content)
{
    Tokenizer tokenizer;
    auto tokens = tokenizer.tokenize(content);

    std::unordered_map<std::string, uint32_t> term_freq;

    for (const auto &token : tokens)
    {
        term_freq[token]++;
    }

    std::lock_guard<std::mutex> lock(index_mutex);

    for (const auto &[term, freq] : term_freq)
    {
        inverted_index[term].push_back({doc_id, freq});
    }

    doc_lengths[doc_id] = tokens.size();
    total_doc_length += tokens.size();
    document_count++;
}

void IndexEngine::build()
{
    // For large-scale system, sorting and compression would go here.
}

void IndexEngine::save(const std::string &filepath)
{
    Serializer::save_index(filepath, inverted_index, doc_lengths,
                           document_count, total_doc_length);
}

void IndexEngine::load(const std::string &filepath)
{
    Serializer::load_index(filepath, inverted_index, doc_lengths,
                           document_count, total_doc_length);
}

const std::unordered_map<std::string, std::vector<Posting>> &
IndexEngine::get_index() const
{
    return inverted_index;
}

uint32_t IndexEngine::get_doc_length(uint32_t doc_id) const
{
    return doc_lengths.at(doc_id);
}

double IndexEngine::get_avg_doc_length() const
{
    return document_count == 0 ? 0 : static_cast<double>(total_doc_length) / document_count;
}

size_t IndexEngine::total_docs() const
{
    return document_count;
}
