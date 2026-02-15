#include "mmap_loader.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

void *MMapLoader::map_file(const std::string &path, size_t &size)
{
    int fd = open(path.c_str(), O_RDONLY);
    struct stat sb;
    fstat(fd, &sb);
    size = sb.st_size;

    void *addr = mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);
    return addr;
}

void MMapLoader::unmap_file(void *addr, size_t size)
{
    munmap(addr, size);
}
