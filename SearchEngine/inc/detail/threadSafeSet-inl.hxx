#include <cstddef>
#include <deque>
#include <mutex>
#include <iostream>

#include "threadSafeSet.hpp"

namespace h72 {

template<typename T>
void ThreadSafeSet<T>::Insert(const T& a_value)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_set.insert(a_value);
    m_numOfItems.release();
}

template<typename T>
void ThreadSafeSet<T>::Insert(T&& a_value)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_set.insert(std::move(a_value));
    m_numOfItems.release();
}

template<typename T>
void ThreadSafeSet<T>::Extract(T& a_value)
{
    m_numOfItems.acquire();
    std::lock_guard<std::mutex> lock(m_mutex);
    a_value = std::move(m_set);
    m_set.extract();
}

template<typename T>
bool ThreadSafeSet<T>::IsEmpty() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_set.empty();
}

template<typename T>
size_t ThreadSafeSet<T>::Size() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_set.size();
}

template<typename T>
size_t ThreadSafeSet<T>::Count(T& a_value) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_set.count(a_value);
}

}
