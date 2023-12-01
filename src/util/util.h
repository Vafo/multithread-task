#ifndef UTIL_H
#define UTIL_H

#include <string>

#include <functional>
#include "function.hpp"

namespace concurrency::util {

template<typename func_retval_T>
class try_call {

public:
    template<
        typename Callable,
        typename err_prefix_T
    >
    try_call(
        err_prefix_T err_prefix,
        Callable err_msg_gen
    ):
        m_err_msg_gen(err_msg_gen),
        m_err_prefix(err_prefix)
    {}

    func_retval_T operator()(
        func_retval_T retval,
        func_retval_T valid_val
    ) {
        if(retval != valid_val) {
            std::string err_msg = m_err_msg_gen(m_err_prefix, retval);
            throw std::runtime_error(err_msg);
        }

        return retval;
    }

private:
    typename func::function< std::string(std::string, func_retval_T) > m_err_msg_gen;
    std::string m_err_prefix;
}; // class try_call

} // namespace concurrency::util

#endif