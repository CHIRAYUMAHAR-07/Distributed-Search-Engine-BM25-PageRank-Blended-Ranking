#include "bm25.h"
#include "lru_cache.h"
#include "index_engine.h"
#include "search.grpc.pb.h"
#include <grpcpp/grpcpp.h>

class SearchServiceImpl final : public SearchService::Service
{
    Status Search(ServerContext *context,
                  const QueryRequest *request,
                  QueryResponse *response) override
    {

        std::string query = request->query();

        std::vector<std::pair<uint32_t, double>> results =
            index_engine.search(query, request->top_k());

        for (auto &r : results)
        {
            auto *res = response->add_results();
            res->set_doc_id(r.first);
            res->set_score(r.second);
        }

        return Status::OK;
    }

private:
    IndexEngine index_engine;
};
