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
    util::try_call<int> check_call(__FUNCTION__, make_cond_var_err_msg);
    pthread_condattr_t cond_attr;
    int err_num = 0;

    check_call(
        pthread_condattr_init(&cond_attr),
        0 /*valid val*/
    );
    
    check_call(
        pthread_cond_init(&m_cond_var, &cond_attr),
        0 /*valid val*/
    );

    pthread_condattr_destroy(&cond_attr);
}

condition_variable::~condition_variable() {
    pthread_cond_destroy(&m_cond_var);
}

void condition_variable::notify_one() {
    util::try_call<int> check_call(__FUNCTION__, make_cond_var_err_msg);
    check_call(
        pthread_cond_signal(&m_cond_var),
        0 /*valid val*/
    );
}

void condition_variable::notify_all() {
    util::try_call<int> check_call(__FUNCTION__, make_cond_var_err_msg);
    check_call(
        pthread_cond_broadcast(&m_cond_var),
        0 /*valid val*/
    );
}

void condition_variable::wait(unique_lock<mutex>& locker) {
    util::try_call<int> check_call(__FUNCTION__, make_cond_var_err_msg);
    mutex::native_handle_type* native_mutex = locker.mutex()->native_handle();
    /*unlock wrapper*/
    // locker.fake_unlock();
    
    check_call(
        pthread_cond_wait(&m_cond_var, native_mutex),
        0 /*valid val*/
    );
    
    // meeting postcondition of wait
    // locker.fake_lock();
}

} // namespace concurrency