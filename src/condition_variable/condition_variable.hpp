#ifndef CONDITION_VAR_H
#define CONDITION_VAR_H

#include <pthread.h>
#include "mutex.hpp"
#include "unique_lock.hpp"

namespace concurrency {

class condition_variable {

public:
    typedef pthread_cond_t native_type;

    condition_variable();

    condition_variable(const condition_variable& other) = delete;

    ~condition_variable();

    condition_variable& operator=(const condition_variable& other) = delete;
    
    void notify_one();

    void notify_all();

    void wait(unique_lock<mutex>& lock);

    template<typename Predicate>
    void wait(unique_lock<mutex>& lock, Predicate stop_waiting) {
        while(!stop_waiting())
            wait(lock);
    }

    native_type* native_handle()
    { return &m_cond_var; }

private:
    native_type m_cond_var;

}; // class condition_variable

} // namespace concurrency

#endif