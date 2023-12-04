#ifndef THREAD_H
#define THREAD_H

#include "function.hpp"

#include <pthread.h>

namespace concurrency {

class thread {

public:
    typedef pthread_t native_handle_type;

    thread(): m_is_joinable(0), m_thread_id() {}
    
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

    /* TODO: Add move assignment so as to assign empty thread objects */

    ~thread() {
        if(joinable())
            std::terminate();
    }

    bool
    joinable();

    void
    join();

    void
    detach();

    native_handle_type&
    get_id() {
        return m_thread_id;
    }

    const native_handle_type&
    get_id() const {
        return m_thread_id;
    }

private:
    typedef func::function<void()> void_func;

    native_handle_type m_thread_id;
    int m_is_joinable;

    void create_thread(void_func& func_obj);

}; // class thread

class jthread {

public:
    jthread(): m_thread() {}

    template<typename Callable, typename ...Args>
    jthread(Callable cb, Args ...args): m_thread(cb, args...) {}

    jthread(const jthread& other) = delete;
    jthread& operator=(const jthread& other) = delete;

    ~jthread() {
        if(m_thread.joinable()) {
            m_thread.join();
        }
    }

    bool
    joinable()
    { return m_thread.joinable(); }

    void
    join()
    { m_thread.join(); }

    void
    detach()
    { m_thread.detach(); }

    pthread_t&
    get_id() {
        return m_thread.get_id();
    }

    const pthread_t&
    get_id() const {
        return m_thread.get_id();
    }


private:
    thread m_thread;
    /* TODO: Add stop source support */

}; // class jthread


namespace this_thread {

thread::native_handle_type get_native_id();

}; // namespace this_thread

} // namespace concurrency

#endif