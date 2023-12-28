#include "mutex.hpp"

#include <pthread.h>

#include "util.hpp"

namespace concurrency {

// Error Message Generation

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


basic_mutex::~basic_mutex() {
    /*no need to check for error code*/
    pthread_mutex_destroy(&m_handle);
}

void basic_mutex::lock() {
    util::cerror_code<int> code(
        "basic_mutex::lock: pthread_mutex_lock: ",
        make_mutex_lock_err_msg, 0
    );
    
    code = pthread_mutex_lock(&m_handle);
}

bool basic_mutex::try_lock() {
    util::cerror_code<int> code(
        "basic_mutex::try_lock: pthread_mutex_trylock: ",
        make_mutex_lock_err_msg, 0, util::no_auto_throw
    );
    code = pthread_mutex_trylock(&m_handle);
    if(code) {
        if(code.value() == EBUSY) /*mutex is already locked*/
            return false; /*lock failed*/

        code.throw_exception();
    }

    return true; /*lock success*/
}

void basic_mutex::unlock() {
    util::cerror_code<int> code(
        "basic_mutex::unlock: pthread_mutex_unlock: ",
        make_mutex_lock_err_msg, 0
    );

    code = pthread_mutex_unlock(&m_handle);
}

mutex::mutex():
    basic_mutex()
{
    pthread_mutexattr_t mutex_attr;
    util::cerror_code<int> code(
        "mutex::mutex: ",
        make_mutex_init_err_msg, 0
    );
    
    code = pthread_mutexattr_init(&mutex_attr);
    code = pthread_mutex_init(&m_handle, &mutex_attr);
    code = pthread_mutexattr_destroy(&mutex_attr);
}

recursive_mutex::recursive_mutex():
    basic_mutex()
{
    pthread_mutexattr_t mutex_attr;
    util::cerror_code<int> code(
        "recursive_mutex::recursive_mutex",
        make_mutex_init_err_msg, 0
    );
    
    code = pthread_mutexattr_init(&mutex_attr);
    code = pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_RECURSIVE);
    code = pthread_mutex_init(&m_handle, &mutex_attr);
    code = pthread_mutexattr_destroy(&mutex_attr);
}

} // namespace concurrency