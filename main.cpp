#include "index_engine.h"
#include <iostream>

int main()
{
    IndexEngine engine;

    engine.add_document(1, "Distributed systems are scalable systems");
    engine.add_document(2, "Search engines use inverted index");
    engine.add_document(3, "BM25 ranking improves search relevance");

    engine.build();

    engine.save("data/index.bin");

    std::cout << "Indexed documents: " << engine.total_docs() << std::endl;
    std::cout << "Average doc length: " << engine.get_avg_doc_length() << std::endl;

    return 0;
}
