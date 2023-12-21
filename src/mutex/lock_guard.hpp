#ifndef LOCK_GUARD_H
#define LOCK_GUARD_H

#include "mutex_common.hpp"

namespace concurrency {

template<typename Mutex_T>
class lock_guard {

public:
    typedef Mutex_T mutex_type;

    lock_guard() = delete;

    lock_guard(const lock_guard& other) = delete;
    
    lock_guard(mutex_type& mut)
    : m_mutex_ptr(&mut)
    { m_mutex_ptr->lock(); }

    lock_guard(mutex_type& mut, adopt_lock_t)
    : m_mutex_ptr(&mut)
    { }

    ~lock_guard() { 
        assert(m_mutex_ptr != nullptr);
        m_mutex_ptr->unlock();
    }
    
    lock_guard& operator=(const lock_guard& other) = delete;

private:
    mutex_type* const m_mutex_ptr;

}; // class lock_guard

} // namespace concurrency

#endif