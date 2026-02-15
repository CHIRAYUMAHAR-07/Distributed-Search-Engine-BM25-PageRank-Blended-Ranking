#pragma once
#include <string>
#include <cstdint>

class MMapLoader
{
public:
    static void *map_file(const std::string &path, size_t &size);
    static void unmap_file(void *addr, size_t size);
};
