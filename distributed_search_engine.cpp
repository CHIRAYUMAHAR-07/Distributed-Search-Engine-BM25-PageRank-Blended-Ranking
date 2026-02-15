#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <mutex>
#include <thread>
#include <queue>
#include <condition_variable>
#include <cmath>
#include <fstream>
#include <map>
#include <list>
#include <chrono>
#include <atomic>

using namespace std;

vector<string> tokenize(const string &text)
{
    vector<string> tokens;
    string word;
    stringstream ss(text);
    while (ss >> word)
    {
        transform(word.begin(), word.end(), word.begin(), ::tolower);
        word.erase(remove_if(word.begin(), word.end(),
                             [](char c)
                             { return !isalnum(c); }),
                   word.end());
        if (!word.empty())
            tokens.push_back(word);
    }
    return tokens;
}

void encode_uint32(uint32_t value, vector<uint8_t> &out)
{
    while (value > 127)
    {
        out.push_back((value & 0x7F) | 0x80);
        value >>= 7;
    }
    out.push_back(value);
}

template <typename K, typename V>
class LRUCache
{
    size_t capacity;
    list<pair<K, V>> items;
    unordered_map<K, typename list<pair<K, V>>::iterator> map;

public:
    LRUCache(size_t cap) : capacity(cap) {}

    bool get(const K &key, V &value)
    {
        if (!map.count(key))
            return false;
        items.splice(items.begin(), items, map[key]);
        value = map[key]->second;
        return true;
    }

    void put(const K &key, const V &value)
    {
        if (map.count(key))
            items.erase(map[key]);

        items.push_front({key, value});
        map[key] = items.begin();

        if (map.size() > capacity)
        {
            auto last = items.end();
            --last;
            map.erase(last->first);
            items.pop_back();
        }
    }
};

class ThreadPool
{
    vector<thread> workers;
    queue<function<void()>> tasks;
    mutex mtx;
    condition_variable cv;
    bool stop = false;

public:
    ThreadPool(size_t n)
    {
        for (size_t i = 0; i < n; i++)
        {
            workers.emplace_back([this]()
                                 {
                while(true){
                    function<void()> task;
                    {
                        unique_lock<mutex> lock(mtx);
                        cv.wait(lock,[this]{
                            return stop || !tasks.empty();
                        });
                        if(stop && tasks.empty()) return;
                        task = move(tasks.front());
                        tasks.pop();
                    }
                    task();
                } });
        }
    }

    void enqueue(function<void()> f)
    {
        {
            unique_lock<mutex> lock(mtx);
            tasks.push(f);
        }
        cv.notify_one();
    }

    ~ThreadPool()
    {
        {
            unique_lock<mutex> lock(mtx);
            stop = true;
        }
        cv.notify_all();
        for (auto &w : workers)
            w.join();
    }
};

class PageRank
{
    unordered_map<uint32_t, vector<uint32_t>> graph;
    unordered_map<uint32_t, double> rank;

public:
    void add_edge(uint32_t from, uint32_t to)
    {
        graph[from].push_back(to);
    }

    void compute(int iterations = 10, double d = 0.85)
    {
        size_t N = graph.size();
        for (auto &p : graph)
            rank[p.first] = 1.0 / N;

        for (int i = 0; i < iterations; i++)
        {
            unordered_map<uint32_t, double> new_rank;
            for (auto &p : graph)
                new_rank[p.first] = (1 - d) / N;

            for (auto &p : graph)
            {
                for (auto &dest : p.second)
                {
                    new_rank[dest] += d * (rank[p.first] / p.second.size());
                }
            }
            rank = new_rank;
        }
    }

    double get(uint32_t id)
    {
        if (!rank.count(id))
            return 0;
        return rank[id];
    }
};

class WAL
{
    string path;

public:
    WAL(string p) : path(p) {}

    void append(uint32_t doc_id, const string &content)
    {
        ofstream out(path, ios::binary | ios::app);
        size_t size = content.size();
        out.write((char *)&doc_id, sizeof(doc_id));
        out.write((char *)&size, sizeof(size));
        out.write(content.c_str(), size);
    }
};

double bm25(int tf, int df, int doc_len,
            double avgdl, int N)
{
    double k1 = 1.5, b = 0.75;
    double idf = log((N - df + 0.5) / (df + 0.5));
    return idf * ((tf * (k1 + 1)) /
                  (tf + k1 * (1 - b + b * doc_len / avgdl)));
}

class IndexEngine
{
    unordered_map<string, vector<pair<uint32_t, uint32_t>>> index;
    unordered_map<uint32_t, uint32_t> doc_len;
    size_t total_docs = 0;
    uint64_t total_len = 0;
    mutex mtx;

    LRUCache<string,
             vector<pair<uint32_t, double>>>
        cache{100};

    PageRank pagerank;
    WAL wal{"wal.log"};

public:
    void add_document(uint32_t id, const string &content)
    {
        wal.append(id, content);

        auto tokens = tokenize(content);
        unordered_map<string, uint32_t> tf;
        for (auto &t : tokens)
            tf[t]++;

        lock_guard<mutex> lock(mtx);
        for (auto &p : tf)
        {
            index[p.first].push_back({id, p.second});
        }

        doc_len[id] = tokens.size();
        total_len += tokens.size();
        total_docs++;
    }

    vector<pair<uint32_t, double>>
    search(const string &query, int k)
    {

        vector<pair<uint32_t, double>> result;
        if (cache.get(query, result))
            return result;

        auto terms = tokenize(query);
        unordered_map<uint32_t, double> scores;

        for (auto &term : terms)
        {
            if (!index.count(term))
                continue;
            int df = index[term].size();

            for (auto &post : index[term])
            {
                uint32_t doc_id = post.first;
                int tf = post.second;

                double score = bm25(
                    tf, df,
                    doc_len[doc_id],
                    (double)total_len / total_docs,
                    total_docs);

                score += 0.3 * pagerank.get(doc_id);

                scores[doc_id] += score;
            }
        }

        for (auto &s : scores)
            result.push_back(s);

        sort(result.begin(), result.end(),
             [](auto &a, auto &b)
             {
                 return a.second > b.second;
             });

        if (result.size() > k)
            result.resize(k);

        cache.put(query, result);
        return result;
    }
};

class ConsistentHash
{
    map<size_t, int> ring;

public:
    void add_node(int node)
    {
        size_t h = hash<int>{}(node);
        ring[h] = node;
    }

    int get_node(int key)
    {
        size_t h = hash<int>{}(key);
        auto it = ring.lower_bound(h);
        if (it == ring.end())
            return ring.begin()->second;
        return it->second;
    }
};

enum class State
{
    FOLLOWER,
    LEADER
};

class RaftNode
{
public:
    atomic<State> state{State::FOLLOWER};

    void become_leader()
    {
        state = State::LEADER;
        cout << "Node became leader\n";
    }
};

void benchmark(IndexEngine &engine)
{
    auto start = chrono::high_resolution_clock::now();
    for (int i = 0; i < 10000; i++)
        engine.search("distributed systems", 5);
    auto end = chrono::high_resolution_clock::now();
    cout << "10k queries in "
         << chrono::duration<double,
                             milli>(end - start)
                .count()
         << " ms\n";
}

int main()
{

    IndexEngine engine;
    ThreadPool pool(4);

    pool.enqueue([&]
                 { engine.add_document(1,
                                       "Distributed systems are scalable"); });

    pool.enqueue([&]
                 { engine.add_document(2,
                                       "Search engine uses inverted index"); });

    this_thread::sleep_for(
        chrono::milliseconds(500));

    auto results =
        engine.search("distributed search", 5);

    for (auto &r : results)
        cout << "Doc " << r.first
             << " Score " << r.second << "\n";

    benchmark(engine);

    return 0;
}
