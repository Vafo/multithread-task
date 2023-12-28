#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>
#include "function.hpp"

namespace concurrency {

class thread {
public:
    typedef pthread_t native_handle_type;

private:
    typedef func::function<void()> void_func;

private:
    native_handle_type m_thread_id;

private:
    thread(const thread& other) = delete;
    thread& operator=(const thread& other) = delete;

public:
    thread()
        : m_thread_id()
    {}
    
    template<typename Callable, typename ...Args>
    explicit
    thread(Callable callb, Args ...args) 
        : m_thread_id()
    {
        // arguments must be invocable after conversion to rvalues (no lvalue references)
        auto lambda_func = [=] () -> void { callb(args...); };

        void_func func(lambda_func);
        create_thread(func);
    }
    
    thread(thread&& other)
    { swap(other); };

    thread& operator=(thread&& other) {
        if(joinable())
            throw std::runtime_error("thread: operator=: thread was not joined");
        swap(other);
        return *this;
    }

    ~thread() noexcept(false) /*TODO: find alternative to suit >c++11*/ {
        if(joinable())
            /*since c++11 throw in destructor causes terminate*/
            throw std::runtime_error("thread: ~thread: thread was not joined");
    }

public:
    void join();

    void detach();

    bool joinable();

public:
    native_handle_type&
    get_id()
    { return m_thread_id; }

    const native_handle_type&
    get_id() const
    { return m_thread_id; }

public:    
    void swap(thread& other) {
        using std::swap;
        swap(m_thread_id, other.m_thread_id);
    }

    friend void swap(thread& lhs, thread& rhs)
    { lhs.swap(rhs); }

private:
    void create_thread(void_func& func_obj);

}; // class thread


namespace this_thread {

thread::native_handle_type get_native_id();

}; // namespace this_thread

} // namespace concurrency

#endif