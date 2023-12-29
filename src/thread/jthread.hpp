#ifndef JTHREAD_H
#define JTHREAD_H

#include "thread.hpp"

namespace concurrency {

class jthread: public thread {
// private:
//     thread m_thread;

// private:
//     jthread(const jthread& other) = delete;
//     jthread& operator=(const jthread& other) = delete;

public:
    jthread()
        : thread()
    {}

    template<typename Callable, typename ...Args>
    jthread(Callable cb, Args ...args)
        : thread(cb, args...)
    {}

    jthread(jthread&& other)
    { swap(other); }

    jthread& operator=(jthread&& other) {
        jthread(std::move(other)).swap(*this); // move to tmp & swap
        return *this;
    };

    ~jthread() {
        if(joinable()) {
            join();
        }
    }

// public:
//     void join()
//     { m_thread.join(); }

//     void detach()
//     { m_thread.detach(); }

//     bool joinable()
//     { return m_thread.joinable(); }

// public:
//     pthread_t&
//     get_id()
//     { return m_thread.get_id(); }

//     const pthread_t&
//     get_id() const
//     { return m_thread.get_id(); }

public:
    void swap(jthread& other) { 
        using std::swap;

        thread::swap(other);
        // swap(m_thread, other.m_thread);
    }

    friend void swap(jthread& lhs, jthread& rhs)
    { lhs.swap(rhs); }

}; // class jthread

} // namespace concurrency

#endif