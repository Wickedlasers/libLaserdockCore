#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>



template <typename T>
class ldSharedQueue
{
public:
    ldSharedQueue();
    ~ldSharedQueue();

    T& front();
    void pop_front();

    void push_back(const T& item);
    void push_back(T&& item);

    int size();
    bool empty();
    void clear();

private:
    std::deque<T> queue_;
    std::mutex mutex_;
    std::condition_variable cond_;
};

template <typename T>
ldSharedQueue<T>::ldSharedQueue(){}

template <typename T>
ldSharedQueue<T>::~ldSharedQueue(){}

template <typename T>
T& ldSharedQueue<T>::front()
{
    std::unique_lock<std::mutex> mlock(mutex_);
    while (queue_.empty())
    {
        cond_.wait(mlock);
    }
    return queue_.front();
}

template <typename T>
void ldSharedQueue<T>::pop_front()
{
    std::unique_lock<std::mutex> mlock(mutex_);
    while (queue_.empty())
    {
        cond_.wait(mlock);
    }
    queue_.pop_front();    
}

template <typename T>
void ldSharedQueue<T>::push_back(const T& item)
{
    std::unique_lock<std::mutex> mlock(mutex_);
    queue_.push_back(item);
    mlock.unlock();     // unlock before notificiation to minimize mutex con
    cond_.notify_one(); // notify one waiting thread

}

template <typename T>
void ldSharedQueue<T>::push_back(T&& item)
{
    std::unique_lock<std::mutex> mlock(mutex_);
    queue_.push_back(std::move(item));
    mlock.unlock();     // unlock before notificiation to minimize mutex con
    cond_.notify_one(); // notify one waiting thread

}

template <typename T>
int ldSharedQueue<T>::size()
{
    std::unique_lock<std::mutex> mlock(mutex_);
    int size = queue_.size();
    mlock.unlock();
    return size;
}

template <typename T>
void ldSharedQueue<T>::clear()
{
    std::unique_lock<std::mutex> mlock(mutex_);
    queue_.clear();
    mlock.unlock();

}
