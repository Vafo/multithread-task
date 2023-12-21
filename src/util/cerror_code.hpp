#ifndef CERROR_CODE_H
#define CERROR_CODE_H

#include <string>

#include "function.hpp"

namespace concurrency::util {

// Option helper class
struct no_auto_throw_t {};

constexpr no_auto_throw_t no_auto_throw;


template<typename func_retval_T>
struct cerror_code_base {
protected:

    cerror_code_base() = delete;
    
    cerror_code_base(const cerror_code_base& other) = delete;

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

    cerror_code_base operator=(const cerror_code_base& other) = delete;

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

    cerror_code() = delete;

    cerror_code(const cerror_code& other) = delete;

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
        no_auto_throw_t /*disable auto throw*/
    )
    : base_class (
        err_prefix,
        err_msg_gen,
        valid_retval,
        false /*no auto throw*/
    )
    { }

    cerror_code operator=(const cerror_code& other) = delete;

    void operator=(
        func_retval_T retval
    )
    { base_class::operator=(retval); }

    operator bool() const
    { return base_class::operator bool(); }

}; // class cerror_code

} // namespace concurrency::util

#endif