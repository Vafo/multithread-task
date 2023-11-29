#ifndef THREAD_H
#define THREAD_H

#include "function.hpp"

#include <pthread.h>
#include <exception>

namespace concurrency {

class thread {

public:
    
    thread(): m_is_joinable(0) {}
    
    template<typename Callable, typename ...Args>
    explicit
    thread(Callable callb, Args ...args) 
    :   
        m_is_joinable(0)
    {
        // arguments must be invocable after conversion to rvalues (no lvalue references)
        auto lambda_func = [=] () -> void { callb(args...); };

        void_func func(lambda_func);
        create_thread(func);
    }

    thread(const thread& other) = delete;
    thread& operator=(const thread& other) = delete;

    bool
    is_joinable();

    void
    join();

    void
    detach();

    pthread_t&
    get_id() {
        return m_thread_id;
    }

    const pthread_t&
    get_id() const {
        return m_thread_id;
    }

    ~thread() {
        if(is_joinable())
            std::terminate();
    }

private:
    typedef func::function<void()> void_func;

    pthread_t m_thread_id;
    int m_is_joinable;

    void create_thread(void_func& func_obj);

};

} // namespace concurrency

#endif