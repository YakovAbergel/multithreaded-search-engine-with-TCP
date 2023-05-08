#ifndef THREAD_SAFE_SET_HPP
#define THREAD_SAFE_SET_HPP

#include <cstddef>
#include <set>
#include <mutex>
#include "semaphore.hpp"

namespace h72{

template<typename T>
class ThreadSafeSet {
public:
    explicit ThreadSafeSet() = default;
    ~ThreadSafeSet() = default;

    void Insert(const T& a_value);
    void Insert(T&& a_value);

    void Extract(T& a_value);

    bool IsEmpty() const;
    size_t Size() const;
    size_t Count(T& a_value) const;

private:
    std::set<T> m_set;
    mutable std::mutex m_mutex;
    threads::Semaphore m_numOfItems;
};

}

#include "detail/threadSafeSet-inl.hxx"

#endif // SAFEQUEUE_HPP