#include "mutex.hpp"
#include <pthread.h>
#include <stdexcept>

namespace concurrency {

static std::string make_mutex_lock_err_msg(std::string prefix, int err_num) {
    std::string err_msg( std::move(prefix) );

    switch(err_num) {
        case EAGAIN:
            err_msg += 
                "the mutex could not be acquired because max number of recursive locks for mutex have been exceeded";
            break;
        case EINVAL:
            err_msg += "calling thread's priority is higher than mutex's current priority ceiling";
            break;
        case ENOTRECOVERABLE:
            err_msg += "state protected by mutex is not recoverable";
            break;
        case EOWNERDEAD:
            err_msg += "previous mutex owner terminated";
            break;
        case EDEADLK:
            err_msg += "deadlock condition was detected";
            break;
        case EPERM:
            err_msg +=
                "mutex type is [errorcheck] or [recursive] or mutex is a robust mutex and current thread does not own the mutex";
            break;
        default:
            err_msg += "error code: " + std::to_string(err_num);
            break;
    }
    
    return err_msg;
}

static std::string make_mutex_init_err_msg(std::string prefix, int err_num) {
    std::string err_msg( std::move(prefix) );

    switch(err_num) {
        case EAGAIN:
            err_msg += 
                "the system lacked necessary resources (other than memory) to initialize mutex";
            break;
        case ENOMEM:
            err_msg += 
                "insufficient memory exists to initialize mutex";
            break;
        case EPERM:
            err_msg += 
                "caller does not have the privilege to perform the operation";
            break;
        default:
            err_msg += "error code: " + std::to_string(err_num);
            break;
    }
    
    return err_msg;
}

mutex::mutex():
    m_handle()
{
    pthread_mutexattr_t mutex_attr;
    int err_num = 0;
    
    err_num = pthread_mutexattr_init(&mutex_attr);
    if(err_num != 0) {
        std::string err_msg = make_mutex_init_err_msg("mutex::mutex: pthread_mutexattr_init: ", err_num);
        throw std::runtime_error(err_msg);
    }

    err_num = pthread_mutex_init(&m_handle, &mutex_attr);
    if(err_num != 0) {
        std::string err_msg = make_mutex_init_err_msg("mutex::mutex: pthread_mutex_init: ", err_num);
        throw std::runtime_error(err_msg);
    }

    err_num = pthread_mutexattr_destroy(&mutex_attr);
    if(err_num != 0) {
        std::string err_msg = make_mutex_init_err_msg("mutex::mutex: pthread_mutexattr_destroy: ", err_num);
        throw std::runtime_error(err_msg);
    }
}

mutex::~mutex() {
    /*no need to check for error code*/
    pthread_mutex_destroy(&m_handle);
}

void mutex::lock() {
    int err_num = 0;
    err_num = pthread_mutex_lock(&m_handle);
    if(err_num != 0) {
        std::string err_msg = make_mutex_lock_err_msg("mutex::lock: pthread_mutex_lock: ", err_num);
        throw std::runtime_error(err_msg);
    }
}

bool mutex::try_lock() {
    int err_num = 0;
    err_num = pthread_mutex_trylock(&m_handle);
    if(err_num != 0) {
        if(err_num == EBUSY) /*mutex is already locked*/
            return !err_num; /*return zero*/

        std::string err_msg = make_mutex_lock_err_msg("mutex::try_lock: pthread_mutex_trylock: ", err_num);
        throw std::runtime_error(err_msg);
    }

    return !err_num; /*return non zero*/
}

void mutex::unlock() {
    int err_num = 0;
    err_num = pthread_mutex_unlock(&m_handle);
    if(err_num != 0) {
        std::string err_msg = make_mutex_lock_err_msg("mutex::unlock: pthread_mutex_unlock: ", err_num);
        throw std::runtime_error(err_msg);
    }
}

} // namespace concurrency