#ifndef SCOPED_GUARD_H
#define SCOPED_GUARD_H

#include "function.hpp"

namespace concurrency::util {

// Scope deferred execution
class scoped_guard {

public:
    scoped_guard() = delete;

    scoped_guard(const scoped_guard& other) = delete;

    explicit
    scoped_guard(func::function<void()> defered_func)
        : m_func(defered_func)
        , to_execute(true)
    { }
 
    ~scoped_guard() {
        if(to_execute) 
            m_func();
    }

    scoped_guard& operator=(const scoped_guard& other) = delete;

    void assign(func::function<void()> defered_func) { 
        m_func = defered_func;
        to_execute = true;
    }

    void cancel()
    { to_execute = false; }

private:
    func::function<void()> m_func;
    bool to_execute;
}; // class scoped_guard

} // namespace concurrency::util

#endif