#ifndef THREAD_SAFE_QUEUE_HPP
#define THREAD_SAFE_QUEUE_HPP

#include <cstddef>
#include <deque>
#include <mutex>
#include "semaphore.hpp"

namespace h72{

template<typename T>
class ThreadSafeQueue {
public:
    explicit ThreadSafeQueue() = default;
    ~ThreadSafeQueue() = default;

    void PushBack(const T& a_value);
    void PushBack(T&& a_value);

    void PushFront(const T& a_value);
    void PushFront(T&& a_value);

    void PopBack(T& a_value);
    void PopFront(T& a_value);

    bool IsEmpty() const;
    size_t Size() const;

private:
    std::deque<T> m_queue;
    mutable std::mutex m_mutex;
    threads::Semaphore m_numOfItems;
};

}

#include "detail/threadSafeQueue-inl.hxx"

#endif // SAFEQUEUE_HPP
