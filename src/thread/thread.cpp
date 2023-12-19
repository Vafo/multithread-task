#include "thread.hpp"

#include <pthread.h>
#include <assert.h>

#include "util.h"

namespace concurrency {

// Error Message Generation

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

// C function & struct parameter for thread (pthread) to execute

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


void thread::create_thread(void_func& func_obj) {
    util::cerror_code<int> code(
        "create_thread: ",
        make_pthread_err_msg, 0
    );

    pthread_attr_t attr;
    code = pthread_attr_init(&attr);

    // create heap allocated arguments
    start_routine_args* args = new start_routine_args(func_obj);
    util::defer_execution def_exec(
        [&] () -> void {
            // reset thread id
            m_thread_id = native_handle_type();
            // release allocated resources
            delete args;
        }
    );
    
    code = pthread_create(&m_thread_id, &attr,
        _start_routine,
        reinterpret_cast<void *>(args)
    );

    def_exec.assign(
        [&] () -> void {
            // revert thread invocation (cancel it)
            pthread_cancel(m_thread_id);
            m_thread_id = native_handle_type();
        }
    );
    
    code = pthread_attr_destroy(&attr);
    def_exec.cancel();
}

void
thread::join() {
    util::cerror_code<int> code(
        "thread::detach: pthread_detach: ",
        make_pthread_err_msg, 0
    );
    code = pthread_join(m_thread_id, NULL);

    m_thread_id = native_handle_type(); // reset handle
}

bool
thread::joinable() {
    return m_thread_id != native_handle_type();
}

void
thread::detach() {
    util::cerror_code<int> code(
        "thread::detach: pthread_detach: ",
        make_pthread_err_msg, 0
    );
    code = pthread_detach(m_thread_id);

    m_thread_id = native_handle_type(); // reset handle
}


namespace this_thread {

thread::native_handle_type get_native_id() {
    return pthread_self();
}

}; // namespace this_thread

} // namespace concurrency