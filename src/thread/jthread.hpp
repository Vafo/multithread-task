#ifndef JTHREAD_H
#define JTHREAD_H

#include "thread.hpp"

namespace concurrency {

class jthread {

public:
    jthread()
    : m_thread()
    {}

    template<typename Callable, typename ...Args>
    jthread(Callable cb, Args ...args)
    : m_thread(cb, args...)
    { }

    jthread(jthread&& other): m_thread()
    { swap(other); }

    jthread(const jthread& other) = delete;

    ~jthread() {
        if(m_thread.joinable()) {
            m_thread.join();
        }
    }

    jthread& operator=(const jthread& other) = delete;
    
    jthread& operator=(jthread&& other) {
        jthread(std::move(other)).swap(*this); // move to tmp & swap
        return *this;
    };

    bool joinable()
    { return m_thread.joinable(); }

    void join()
    { m_thread.join(); }

    void detach()
    { m_thread.detach(); }

    pthread_t&
    get_id()
    { return m_thread.get_id(); }

    const pthread_t&
    get_id() const
    { return m_thread.get_id(); }

    void swap(jthread& other) { 
        using std::swap;
        swap(m_thread, other.m_thread);
    }

    friend void swap(jthread& lhs, jthread& rhs)
    { lhs.swap(rhs); }

private:
    thread m_thread;

}; // class jthread

} // namespace concurrency

#endif