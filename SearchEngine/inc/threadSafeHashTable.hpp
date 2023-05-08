#ifndef THREAD_SAFE_HASH_TABLE_HPP
#define THREAD_SAFE_HASH_TABLE_HPP

#include <shared_mutex>
#include <unordered_map>
#include <functional>
#include <optional>
#include <thread>
#include <vector>

namespace h72 {

template <typename K, typename V>
class ThreadSafeHashTable {
 public:
    explicit ThreadSafeHashTable(size_t a_sizeOfTable = 1000, size_t a_numOfThreads = std::thread::hardware_concurrency());
    ThreadSafeHashTable(ThreadSafeHashTable const& a_other) = delete;
    ThreadSafeHashTable& operator=(ThreadSafeHashTable const& a_other) = delete;
    ~ThreadSafeHashTable() = default;

    operator std::unordered_map<K, V>() const;

    void Set(K const& a_key, V const& a_value);
    void Set(K&& a_key, V&& a_value);

    bool Update(K const& a_key, std::function<void(V&)>&& a_func);

    void Upsert(K const& a_key, std::function<void(V&)>&& a_updater, std::function<V()>&& a_factory);

    std::optional<V> Erase(K const& a_key);

    std::optional<V> Get(K const& a_key) const;
    // bool Get(K const& a_key, V& a_value);
    size_t Size() const;
    bool Contains(K const& a_key) const;
    void Print() const;

private:
    bool CantInsert() const;

private:
    mutable std::vector<std::shared_mutex> m_mutexs;
    std::unordered_map<K, V> m_map;
};

} // namespaces

#include "detail/threadSafeHashTable-inl.hxx"

#endif
