#pragma once
#include <string>

class WAL
{
public:
    WAL(const std::string &path);

    void append(uint32_t doc_id,
                const std::string &content);

    void replay(class IndexEngine &engine);

private:
    std::string log_path;
};
