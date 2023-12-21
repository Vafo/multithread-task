#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>
#include "function.hpp"

namespace concurrency {

class thread {

public:
    typedef pthread_t native_handle_type;

    thread()
    : m_thread_id()
    { }
    
    template<typename Callable, typename ...Args>
    explicit
    thread(Callable callb, Args ...args) 
    : m_thread_id() {
        // arguments must be invocable after conversion to rvalues (no lvalue references)
        auto lambda_func = [=] () -> void { callb(args...); };

        void_func func(lambda_func);
        create_thread(func);
    }

    thread(const thread& other) = delete;
    
    thread(thread&& other)
    { swap(other); };

    ~thread() noexcept(false) /*TODO: find alternative to suit >c++11*/ {
        if(joinable())
            /*since c++11 throw in destructor causes terminate*/
            throw std::runtime_error("thread: ~thread: thread was not joined");
    }

    thread& operator=(const thread& other) = delete;
    
    thread& operator=(thread&& other) {
        if(joinable())
            throw std::runtime_error("thread: operator=: thread was not joined");
        swap(other);
        return *this;
    }

    bool joinable();

    void join();

    void detach();

    native_handle_type&
    get_id()
    { return m_thread_id; }

    const native_handle_type&
    get_id() const
    { return m_thread_id; }
    
    void swap(thread& other) {
        using std::swap;
        swap(m_thread_id, other.m_thread_id);
    }

    friend void swap(thread& lhs, thread& rhs)
    { lhs.swap(rhs); }

private:
    typedef func::function<void()> void_func;

    void create_thread(void_func& func_obj);

    native_handle_type m_thread_id;

}; // class thread

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


namespace this_thread {

thread::native_handle_type get_native_id();

}; // namespace this_thread

} // namespace concurrency

#endif