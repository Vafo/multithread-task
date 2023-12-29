#include "mutex.hpp"

#include <pthread.h>

#include "util.hpp"
#include "concurrency_common.hpp"

namespace concurrency {

basic_mutex::~basic_mutex() {
    /*no need to check for error code*/
    pthread_mutex_destroy(&m_handle);
}

void basic_mutex::lock() {
    util::posix_error_throws code(__FUNCTION__);
    code = pthread_mutex_lock(&m_handle);
}

bool basic_mutex::try_lock() {
    util::posix_error code(__FUNCTION__);
    code = pthread_mutex_trylock(&m_handle);
    
    if(code) {
        if(code.value() == EBUSY) /*mutex is already locked*/
            return false; /*lock failed*/

        code.throw_exception();
    }

    return true; /*lock success*/
}

void basic_mutex::unlock() {
    util::posix_error_throws code(__FUNCTION__);
    code = pthread_mutex_unlock(&m_handle);
}


mutex::mutex()
    : basic_mutex()
{
    util::posix_error_throws code(__FUNCTION__);
    pthread_mutexattr_t mutex_attr;

    code = pthread_mutexattr_init(&mutex_attr);
    code = pthread_mutex_init(&m_handle, &mutex_attr);
    code = pthread_mutexattr_destroy(&mutex_attr);
}

recursive_mutex::recursive_mutex()
    : basic_mutex()
{
    util::posix_error_throws code(__FUNCTION__);
    pthread_mutexattr_t mutex_attr;

    code = pthread_mutexattr_init(&mutex_attr);
    code = pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_RECURSIVE);
    code = pthread_mutex_init(&m_handle, &mutex_attr);
    code = pthread_mutexattr_destroy(&mutex_attr);
}

} // namespace concurrency