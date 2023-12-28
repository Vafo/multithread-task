#ifndef SCOPED_GUARD_H
#define SCOPED_GUARD_H

#include "function.hpp"

namespace concurrency::util {

// Scope deferred execution
class scoped_guard {
private:
    func::function<void()> m_func;
    bool to_execute;

private:
    scoped_guard() = delete;
    scoped_guard(const scoped_guard& other) = delete;
    scoped_guard& operator=(const scoped_guard& other) = delete;

public:
    explicit
    scoped_guard(func::function<void()> defered_func)
        : m_func(defered_func)
        , to_execute(true)
    { }
 
    ~scoped_guard() {
        if(to_execute) 
            m_func();
    }

public:
    void assign(func::function<void()> defered_func) { 
        m_func = defered_func;
        to_execute = true;
    }

    void cancel()
    { to_execute = false; }

}; // class scoped_guard

} // namespace concurrency::util

#endif