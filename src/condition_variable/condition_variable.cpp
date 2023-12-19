#include "condition_variable.hpp"

#include <stdexcept>

#include "util.h"

namespace concurrency {

static std::string make_cond_var_err_msg(const std::string& prefix, int err_num) {
    std::string err_msg(prefix);
    
    switch(err_num) {
        case EAGAIN:
            err_msg += 
                "the system lacked the necessary resources (other than memory) to initialize another condition variable";
            break;
        case ENOMEM:
            err_msg += 
                "insufficient memory exists to initialize the condition variable";
            break;
        default:
            err_msg += "error code: " + std::to_string(err_num);
            break;
    }
    
    return err_msg;
}

condition_variable::condition_variable():
    m_cond_var()
{
    /*TODO: consider making it static object*/
    util::cerror_code<int> code(__FUNCTION__, make_cond_var_err_msg, 0);
    pthread_condattr_t cond_attr;
    int err_num = 0;

    code = pthread_condattr_init(&cond_attr);    
    code = pthread_cond_init(&m_cond_var, &cond_attr);

    pthread_condattr_destroy(&cond_attr);
}

condition_variable::~condition_variable() {
    pthread_cond_destroy(&m_cond_var);
}

void condition_variable::notify_one() {
    util::cerror_code<int> code(__FUNCTION__, make_cond_var_err_msg, 0, util::no_auto_throw);
    code = pthread_cond_signal(&m_cond_var);
}

void condition_variable::notify_all() {
    util::cerror_code<int> code(__FUNCTION__, make_cond_var_err_msg, 0, util::no_auto_throw);
    code = pthread_cond_broadcast(&m_cond_var);
}

void condition_variable::wait(unique_lock<mutex>& locker) {
    util::cerror_code<int> code(__FUNCTION__, make_cond_var_err_msg, 0, util::no_auto_throw);
    mutex::native_handle_type* native_mutex = locker.mutex()->native_handle();
    
    code = pthread_cond_wait(&m_cond_var, native_mutex);
}

} // namespace concurrency