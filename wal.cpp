#include "wal.h"
#include <fstream>

WAL::WAL(const std::string &path) : log_path(path) {}

void WAL::append(uint32_t doc_id,
                 const std::string &content)
{
    std::ofstream out(log_path,
                      std::ios::binary | std::ios::app);

    size_t size = content.size();
    out.write(reinterpret_cast<char *>(&doc_id), sizeof(doc_id));
    out.write(reinterpret_cast<char *>(&size), sizeof(size));
    out.write(content.c_str(), size);
}

void WAL::replay(IndexEngine &engine)
{
    std::ifstream in(log_path, std::ios::binary);

    while (in.peek() != EOF)
    {
        uint32_t doc_id;
        size_t size;

        in.read(reinterpret_cast<char *>(&doc_id), sizeof(doc_id));
        in.read(reinterpret_cast<char *>(&size), sizeof(size));

        std::string content(size, ' ');
        in.read(&content[0], size);

        engine.add_document(doc_id, content);
    }
}
