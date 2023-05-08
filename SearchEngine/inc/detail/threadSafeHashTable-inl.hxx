#include <mutex>
#include <random>

#include "threadSafeHashTable.hpp"

namespace h72 {   

template <typename K, typename V>
ThreadSafeHashTable<K, V>::ThreadSafeHashTable(size_t a_sizeOfTable, size_t a_numOfThreads)
: m_mutexs(a_numOfThreads)
, m_map(a_sizeOfTable)
{
}

// template <typename K, typename V>
// V ThreadSafeHashTable<K, V>::Get(K const& a_key, V& a_value)  
// {
//     size_t bucket = m_map.bucket(a_key);
//     std::shared_lock<std::shared_mutex> lock(m_mutexs[bucket % m_mutexs.size()]);

//     auto it = m_map.find(a_key);

//     if(it != m_map.end()) {
//         return a_value = it->second;
//     }

//     return false;
// }

template <typename K, typename V>
std::optional<V> ThreadSafeHashTable<K, V>::Get(K const& a_key) const 
{
    size_t bucket = m_map.bucket(a_key);
    std::shared_lock<std::shared_mutex> lock(m_mutexs[bucket % m_mutexs.size()]);

    auto it = m_map.find(a_key);

    if(it != m_map.end()) {
        return it->second;
    }

    return {};
}

template <typename K, typename V>
void ThreadSafeHashTable<K, V>::Set(K const& a_key, V const& a_value) 
{
    size_t bucket = m_map.bucket(a_key);
    std::unique_lock<std::shared_mutex> lock(m_mutexs[bucket%m_mutexs.size()]);

    if(CantInsert()) {
        throw std::out_of_range("full capacity!");
    }

    auto itr = m_map.insert({ a_key, a_value });

    if(!itr.second) {
        itr.first->second = a_value;
    }
}

template <typename K, typename V>
void ThreadSafeHashTable<K, V>::Set(K&& a_key, V&& a_value) 
{
    size_t bucket = m_map.bucket(a_key);
    std::unique_lock<std::shared_mutex> lock(m_mutexs[bucket%m_mutexs.size()]);

    if(CantInsert()) {
        throw std::out_of_range("full capacity!");
    }

    auto itr = m_map.insert({ std::move(a_key), std::move(a_value) });

    if(!itr.second) {
        itr.first->second = a_value;
    }
}

template <typename K, typename V>
bool ThreadSafeHashTable<K, V>::Update(K const& a_key, std::function<void(V&)>&& a_func)
{
    size_t bucket = m_map.bucket(a_key);
    std::unique_lock<std::shared_mutex> lock(m_mutexs[bucket%m_mutexs.size()]);

    auto it = m_map.find(a_key);

    if (it == m_map.end()) {
        return false;
    }

    try {
        a_func(it->second);
    } catch (...) {
        throw;
    }

    return true;
}

template <typename K, typename V>
void ThreadSafeHashTable<K, V>::Upsert(K const& a_key, std::function<void(V&)>&& a_updater, std::function<V()>&& a_factory)
{
    size_t bucket = m_map.bucket(a_key);
    std::unique_lock<std::shared_mutex> lock(m_mutexs[bucket%m_mutexs.size()]);

    auto it = m_map.find(a_key);

    if (it != m_map.end()) {
        try {
            a_updater(it->second);
        } catch (...) {
            throw;
        }
    } else {
        try {

            if(CantInsert()) {
                throw std::out_of_range("full capacity!");
            }
            m_map.insert({std::move(a_key), a_factory()});
        } catch (...) {
            throw;
        }
    }
}

template <typename K, typename V>
size_t ThreadSafeHashTable<K, V>::Size() const {
    size_t size = 0;

    for (auto& mutex : m_mutexs) {
        std::shared_lock<std::shared_mutex> lock(mutex);
        mutex.lock_shared();
    }
    
    size = m_map.size();

    for (auto& mutex : m_mutexs) {
        std::shared_lock<std::shared_mutex> lock(mutex);
        mutex.unlock_shared();
    }

    return size;
}

template <typename K, typename V>
bool ThreadSafeHashTable<K, V>::Contains(K const& a_key) const 
{
    size_t bucket = m_map.bucket(a_key);
    std::shared_lock<std::shared_mutex> lock(m_mutexs[bucket%m_mutexs.size()]);
    return m_map.count(a_key) > 0;
}

template <typename K, typename V>
std::optional<V> ThreadSafeHashTable<K, V>::Erase(K const& a_key) 
{
    size_t bucket = m_map.bucket(a_key);
    std::unique_lock<std::shared_mutex> lock(m_mutexs[bucket%m_mutexs.size()]);
    
    auto it = m_map.find(a_key);

    if(it != m_map.end()) {
        auto temp = it->second;
        m_map.erase(a_key);
        return temp;
    }

    return {};
}

template <typename K, typename V>
ThreadSafeHashTable<K, V>::operator std::unordered_map<K, V>() const 
{
    std::unordered_map<K, V> map;
    for (const auto& [key, value] : m_map) {
        map[key] = value;
    }
    return map;
}

template <typename K, typename V>
void ThreadSafeHashTable<K, V>::Print() const
{
    std::cout << "ThreadSafeHashTable:" << std::endl;
    for (const auto& [key, value] : m_map)
    {
        std::cout << key << ": ";
        for (const auto& v : value)
        {
            std::cout << v.first << " ";
            std::cout << v.second << " ";

        }
        std::cout << std::endl;
    }
}

template <typename K, typename V>
bool ThreadSafeHashTable<K, V>::CantInsert() const
{
    return m_map.size() + 1 > m_map.max_load_factor() * m_map.bucket_count();
}

}
