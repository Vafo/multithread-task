#ifndef SCOPED_GUARD_H
#define SCOPED_GUARD_H

#include "function.hpp"

namespace concurrency::util {

// Scope deferred execution
class scoped_guard {
private:
    func::function<void()> m_func;

private:
    scoped_guard() = delete;
    scoped_guard(const scoped_guard& other) = delete;
    scoped_guard& operator=(const scoped_guard& other) = delete;

public:
    explicit
    scoped_guard(func::function<void()> defered_func)
        : m_func(defered_func)
    { }
 
    ~scoped_guard() {
        if(m_func) 
            m_func();
    }

public:
    void assign(func::function<void()> defered_func) { 
        m_func.assign(defered_func);
    }

    void release()
    { m_func.reset(); }

}; // class scoped_guard

} // namespace concurrency::util

#endif