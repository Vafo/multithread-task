#ifndef UTIL_H
#define UTIL_H

#include <string>

#include <functional>
#include "function.hpp"

namespace concurrency::util {

struct no_auto_throw_t {};

constexpr no_auto_throw_t no_auto_throw;

template<typename func_retval_T>
struct cerror_code_base {
protected:
    template<
        typename Callable,
        typename err_prefix_T
    >
    cerror_code_base(
        err_prefix_T err_prefix,
        Callable err_msg_gen,
        func_retval_T valid_retval,
        bool is_auto_throw
    )
        : m_err_msg_gen(err_msg_gen)
        , m_err_prefix(err_prefix)
        , m_valid_retval(valid_retval)

        , m_auto_throw(is_auto_throw)
        , m_retval()
    { }

public:
    void operator=(
        func_retval_T retval
    ) {
        m_retval = retval;

        if(m_auto_throw && !valid()) {
            throw_exception();
        }
    }

    void throw_exception() {
        std::string err_msg = m_err_msg_gen(m_err_prefix, m_retval);
        throw std::runtime_error(err_msg);
    }

    bool valid() const
    { return m_retval == m_valid_retval; }

    func_retval_T value() const
    { return m_retval; }

    operator bool() const
    { return !valid(); }

private:
    typename func::function< 
        std::string(std::string, func_retval_T)
    > m_err_msg_gen;
    std::string m_err_prefix;
    func_retval_T m_valid_retval;
    func_retval_T m_retval;
    bool m_auto_throw;
};

template<typename func_retval_T>
class cerror_code: public cerror_code_base<func_retval_T> {
    typedef cerror_code_base<func_retval_T> base_class;
public:
    template<
        typename Callable,
        typename err_prefix_T
    >
    cerror_code(
        err_prefix_T err_prefix,
        Callable err_msg_gen,
        func_retval_T valid_retval
    )
        : base_class (
            err_prefix,
            err_msg_gen,
            valid_retval,
            true /*auto throw*/
        )
    { }

    template<
        typename Callable,
        typename err_prefix_T
    >
    cerror_code(
        err_prefix_T err_prefix,
        Callable err_msg_gen,
        func_retval_T valid_retval,
        no_auto_throw_t
    )
        : base_class (
            err_prefix,
            err_msg_gen,
            valid_retval,
            false /*no auto throw*/
        )
    { }

    void operator=(
        func_retval_T retval
    ) {
        base_class::operator=(retval);
    }

    operator bool() const
    { return base_class::operator bool(); }

}; // class cerror_code


// Scope deferred execution
class defer_execution {

public:
    defer_execution() = delete;

    explicit
    defer_execution(func::function<void()> defered_func)
        : m_func(defered_func)
        , to_execute(true)
    { }
 
    ~defer_execution() {
        if(to_execute) 
            m_func();
    }

    void assign(func::function<void()> defered_func) { 
        m_func = defered_func;
        to_execute = true;
    }

    void cancel()
    { to_execute = false; }

private:
    func::function<void()> m_func;
    bool to_execute;
}; // class defer_execution

} // namespace concurrency::util

#endif