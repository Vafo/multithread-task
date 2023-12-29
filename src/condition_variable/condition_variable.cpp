#include "condition_variable.hpp"

#include "util.hpp"
#include "concurrency_common.hpp"

namespace concurrency {

condition_variable::condition_variable():
    m_cond_var()
{
    util::posix_error_throws code(__FUNCTION__);
    pthread_condattr_t cond_attr;
    int err_num = 0;

    code = pthread_condattr_init(&cond_attr);    
    code = pthread_cond_init(&m_cond_var, &cond_attr);

    code = pthread_condattr_destroy(&cond_attr);
}

condition_variable::~condition_variable() {
    pthread_cond_destroy(&m_cond_var);
}

void condition_variable::notify_one() {
    util::posix_error_throws code(__FUNCTION__);
    code = pthread_cond_signal(&m_cond_var);
}

void condition_variable::notify_all() {
    util::posix_error_throws code(__FUNCTION__);
    code = pthread_cond_broadcast(&m_cond_var);
}

void condition_variable::wait(unique_lock<mutex>& locker) {
    util::posix_error_throws code(__FUNCTION__);
    mutex::native_handle_type* native_mutex = locker.mutex()->native_handle();
    
    code = pthread_cond_wait(&m_cond_var, native_mutex);
}

} // namespace concurrency