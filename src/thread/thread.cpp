
#include "thread.hpp"
#include <pthread.h>
#include <assert.h>

#include <sstream>

namespace concurrency {

struct start_routine_args {
    func::function<void()> m_func;

    start_routine_args(
        const func::function<void()>& func
    ):
        m_func(func)
    {}
};

extern "C" {

static void _cleanup_thread_routine(void* arg) {
    // release allocated memory for arguments
    /* How to release memory allocated by user within thread (?) */
    start_routine_args* routine_args = reinterpret_cast<start_routine_args*>(arg);
    delete routine_args;
}

static void* _start_routine(void* arg) {
    assert(arg != NULL);
    
    pthread_cleanup_push(_cleanup_thread_routine, arg);

    start_routine_args* routine_args = reinterpret_cast<start_routine_args*>(arg);
    
    (routine_args->m_func)();


    pthread_cleanup_pop(1);

    return NULL;
}


} // extern "C"

static std::string make_pthread_err_msg(std::string prefix, int err_num) {
    std::string err_msg( std::move(prefix) );

    switch(err_num) {
        case ESRCH:
            err_msg += "no thread exists with given ID";
            break;
        case EINVAL:
            err_msg += "thread is not joinable or is being joined";
            break;
        case EDEADLK:
            err_msg += "deadlock detected";
            break;
        case EAGAIN:
            err_msg += "could not allocate resources to create thread";
            break;
        default:
            err_msg += "error code: " + std::to_string(err_num);
            break;
    }
    
    return err_msg;
}

void thread::create_thread(void_func& func_obj) {
    int err_num;
    pthread_attr_t attr;
    err_num = pthread_attr_init(&attr);

    if(err_num != 0) {
        std::string err_msg = make_pthread_err_msg("create_thread: pthread_attr_init error: ", err_num);
        throw std::runtime_error(err_msg);
    }

    // create heap allocated arguments
    start_routine_args* args = new start_routine_args(func_obj);

    m_is_joinable = 1;
    err_num = pthread_create(&m_thread_id, &attr,
        _start_routine,
        reinterpret_cast<void *>(args)
    );

    if(err_num != 0) {
        // revet thread invocation
        m_is_joinable = 0;

        // release allocated resources
        delete args;

        std::string err_msg = make_pthread_err_msg("create_thread: pthread_create: ", err_num);
        throw std::runtime_error(err_msg);
    }

    err_num = pthread_attr_destroy(&attr);
    if(err_num != 0) {
        // revert thread invocation (cancel it)
        pthread_cancel(m_thread_id);
        m_is_joinable = 0;

        // release allocated resources
        delete args;

        std::string err_msg = make_pthread_err_msg("create_thread: pthread_attr_destroy error: ", err_num);
        throw std::runtime_error(err_msg);
    }
}

void
thread::join() {
    int err_num = 0;
    err_num = pthread_join(m_thread_id, NULL);

    if (err_num != 0) {
        std::string err_msg = make_pthread_err_msg("thread::join: pthread_join: ", err_num);
        throw std::runtime_error(err_msg);
    }

    m_is_joinable = 0;
}

bool
thread::is_joinable() {
    return m_is_joinable;
}

void
thread::detach() {
    int err_num = 0;
    err_num = pthread_detach(m_thread_id);

    if (err_num != 0) {
        std::string err_msg = make_pthread_err_msg("thread::detach: pthread_detach: ", err_num);
        throw std::runtime_error(err_msg);
    }

    m_is_joinable = 0;
}


} // namespace concurrency