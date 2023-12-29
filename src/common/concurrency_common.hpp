#ifndef CONCURRENCY_COMMON_H
#define CONCURRENCY_COMMON_H

#include "cerror_code.hpp"

namespace concurrency::util {

inline std::string make_posix_err_msg(const char* prefix, int err_num) {
    std::string err_msg(prefix);
    
    err_msg += "error code: " + std::to_string(err_num);
    
    return err_msg;
}

template<bool is_throwing>
class posix_error_base: public cerror_code<int> {
public:
    typedef cerror_code<int> base_class;

public:
    explicit
    posix_error_base(const char* err_prefix)
        : base_class(
            err_prefix,
            make_posix_err_msg,
            0, /*valid val*/
            is_throwing)
    {}

    void operator=(
        int retval
    )
    { base_class::operator=(retval); }

public:
    operator bool() const
    { return base_class::operator bool(); }

};

using posix_error = posix_error_base<false>;
using posix_error_throws = posix_error_base<true>;


}; // namespace util

#endif