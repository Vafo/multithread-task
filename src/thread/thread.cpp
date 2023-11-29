
#include "thread.hpp"
#include <pthread.h>
#include <assert.h>

#include <sstream>

namespace concurrency {

struct start_routine_args {
    func::function<void()> m_func_ptr;

    start_routine_args(
        const func::function<void()>& func_ptr
    ):
        m_func_ptr(func_ptr)
    {}
};

extern "C" {

static void* _start_routine(void* arg) {
    assert(arg != NULL);
    
    start_routine_args* routine_args = reinterpret_cast<start_routine_args*>(arg);

    (routine_args->m_func_ptr)();

    delete routine_args;

    return NULL;
}


} // extern "C"

void thread::create_thread(void_func& func_obj) {
    int err_num;
    pthread_attr_t attr;
    err_num = pthread_attr_init(&attr);

    std::ostringstream err_msg;

    if(err_num != 0) {
        err_msg << "create_thread: pthread_attr_init error: " << err_num;
        std::runtime_error(err_msg.str());
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

        err_msg << "create_thread: ";

        if(err_num == EAGAIN)
            err_msg << "could not allocate resources to create thread";
        else
            err_msg << "pthread_create error" << err_num;
        
        throw std::runtime_error(err_msg.str());
    }

    err_num = pthread_attr_destroy(&attr);
    if(err_num != 0) {
        // revert thread invocation (cancel it)
        pthread_cancel(m_thread_id);
        m_is_joinable = 0;

        // release allocated resources
        delete args;

        err_msg << "create_thread: pthread_attr_destroy error: " << err_num;

        throw std::runtime_error(err_msg.str());
    }
}


bool
thread::is_joinable() {
    return m_is_joinable;
}

void
thread::join() {
    int err_num;
    err_num = pthread_join(m_thread_id, NULL);

    if (err_num != 0) {
        std::string err_msg = "thread::join: ";
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
        }

        throw std::runtime_error(err_msg);
    }

    m_is_joinable = 0;
}


} // namespace concurrency