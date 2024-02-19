#ifndef CERROR_CODE_H
#define CERROR_CODE_H

#include <string>

#include "function.hpp"

namespace concurrency::util {

template<typename func_retval_T>
struct cerror_code {
private:
    typename func::function<
        std::string(const char* prefix, func_retval_T)
    > m_err_msg_gen;
    const char* m_err_prefix;
    func_retval_T m_valid_retval;
    func_retval_T m_retval;
    bool m_auto_throw;

private:
    cerror_code() = delete;

public:
    template<
        typename Callable,
        typename err_prefix_T
    >
    cerror_code(
        err_prefix_T err_prefix,
        Callable err_msg_gen,
        func_retval_T valid_retval,
        bool is_auto_throw = false
    )
        : m_err_msg_gen(err_msg_gen)
        , m_err_prefix(err_prefix)
        , m_valid_retval(valid_retval)

        , m_retval()
        , m_auto_throw(is_auto_throw)
    { }

    virtual ~cerror_code() {}

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

public:
    bool valid() const
    { return m_retval == m_valid_retval; }

    func_retval_T value() const
    { return m_retval; }

    operator bool() const
    { return !valid(); }

}; // class cerror_code

} // namespace concurrency::util

#endif
