#include <chrono>
#include <iostream>
#include "index_engine.h"

int main()
{
    IndexEngine engine;
    engine.load("data/index.bin");

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 10000; i++)
    {
        engine.search("distributed systems", 10);
    }

    auto end = std::chrono::high_resolution_clock::now();
    double ms = std::chrono::duration<double, std::milli>(end - start).count();

    std::cout << "Executed 10k queries in " << ms << " ms\n";
}
