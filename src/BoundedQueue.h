#pragma once
#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>

// Thread-safe bounded queue using mutex + condition variables
template<typename T>
class BoundedQueue {
public:
    explicit BoundedQueue(size_t capacity)
        : capacity_(capacity), closed_(false) {}

    bool push(T&& value) {
        std::unique_lock<std::mutex> lk(m_);
        cv_not_full_.wait(lk, [&]{ return q_.size() < capacity_ || closed_; });
        if (closed_) return false;
        q_.push(std::move(value));
        lk.unlock();
        cv_not_empty_.notify_one();
        return true;
    }

    std::optional<T> pop() {
        std::unique_lock<std::mutex> lk(m_);
        cv_not_empty_.wait(lk, [&]{ return !q_.empty() || closed_; });
        if (q_.empty()) return std::nullopt;
        T v = std::move(q_.front());
        q_.pop();
        lk.unlock();
        cv_not_full_.notify_one();
        return v;
    }

    void close() {
        std::lock_guard<std::mutex> lk(m_);
        closed_ = true;
        cv_not_empty_.notify_all();
        cv_not_full_.notify_all();
    }

private:
    std::mutex m_;
    std::condition_variable cv_not_empty_, cv_not_full_;
    std::queue<T> q_;
    size_t capacity_;
    bool closed_;
};
