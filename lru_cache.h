#pragma once
#include <unordered_map>
#include <list>

template <typename K, typename V>
class LRUCache
{
public:
    LRUCache(size_t capacity) : capacity(capacity) {}

    bool get(const K &key, V &value)
    {
        if (map.find(key) == map.end())
            return false;
        items.splice(items.begin(), items, map[key]);
        value = map[key]->second;
        return true;
    }

    void put(const K &key, const V &value)
    {
        if (map.find(key) != map.end())
        {
            items.erase(map[key]);
        }
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

private:
    size_t capacity;
    std::list<std::pair<K, V>> items;
    std::unordered_map<K, typename std::list<std::pair<K, V>>::iterator> map;
};
