#include <cstddef>
#include <deque>
#include <mutex>
#include <iostream>

#include "threadSafeQueue.hpp"

namespace h72 {

template<typename T>
void ThreadSafeQueue<T>::PushBack(const T& a_value)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_queue.push_back(a_value);
    m_numOfItems.release();
}

template<typename T>
void ThreadSafeQueue<T>::PushBack(T&& a_value)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_queue.push_back(std::move(a_value));
    m_numOfItems.release();
}

template<typename T>
void ThreadSafeQueue<T>::PushFront(const T& a_value)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_queue.push_front(a_value);
    m_numOfItems.release();
}

template<typename T>
void ThreadSafeQueue<T>::PushFront(T&& a_value)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_queue.push_front(std::move(a_value));
    m_numOfItems.release();
}

template<typename T>
void ThreadSafeQueue<T>::PopBack(T& a_value)
{
    m_numOfItems.acquire();
    std::lock_guard<std::mutex> lock(m_mutex);
    a_value = std::move(m_queue.back());
    m_queue.pop_back();
}

template<typename T>
void ThreadSafeQueue<T>::PopFront(T& a_value)
{
    m_numOfItems.acquire();
    std::lock_guard<std::mutex> lock(m_mutex);
    a_value = std::move(m_queue.front());
    m_queue.pop_front();
}

template<typename T>
bool ThreadSafeQueue<T>::IsEmpty() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_queue.empty();
}

template<typename T>
size_t ThreadSafeQueue<T>::Size() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_queue.size();
}

}
