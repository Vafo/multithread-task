#ifndef MUTEX_H
#define MUTEX_H

#include <pthread.h>
#include <stdexcept>

namespace concurrency {

class mutex {

public:
    typedef pthread_mutex_t native_handle_type;

    mutex();

    mutex(const mutex& other) = delete;
    mutex& operator=(const mutex& other) = delete;

    ~mutex();

    void lock();

    bool try_lock();

    void unlock();

    native_handle_type*
    native_handle()
    { return &m_handle; }

private:
    native_handle_type m_handle;

}; // class mutex


// Helper classes for unique_lock
// explicit defer_lock_t() = default;
struct defer_lock_t {};
struct try_to_lock_t {};
struct adopt_lock_t {};

constexpr defer_lock_t	defer_lock { };
constexpr try_to_lock_t	try_to_lock { };
constexpr adopt_lock_t	adopt_lock { };

template<typename Mutex_T>
class unique_lock {

public:
    typedef Mutex_T mutex_type;

    unique_lock(): m_mutex_ptr(nullptr), m_owns(false) {}

    explicit
    unique_lock(mutex_type& mut): m_mutex_ptr(&mut), m_owns(false) { 
        lock();
    }

    /*do not lock*/
    unique_lock(mutex_type& mut, defer_lock_t): m_mutex_ptr(&mut), m_owns(false) {}

    /*try to lock*/
    unique_lock(mutex_type& mut, try_to_lock_t): m_mutex_ptr(&mut), m_owns(false) 
    { m_owns = m_mutex_ptr->try_lock(); }

    /*calling thread already locked mutex*/
    unique_lock(mutex_type& mut, adopt_lock_t): m_mutex_ptr(&mut), m_owns(true) {}

    unique_lock(const unique_lock& other) = delete;
    unique_lock& operator=(const unique_lock& other) = delete;

    ~unique_lock() {
        if(m_mutex_ptr && m_owns)
            unlock();
    }

    void lock() {
        if(!m_mutex_ptr)
            std::runtime_error("unique_lock: lock: not owning any mutex");
        if(m_owns)
            std::runtime_error("unique_lock: lock: recursive lock");
        
        m_mutex_ptr->lock();
        m_owns = true;
    }

    bool try_lock() {
        if(!m_mutex_ptr)
            std::runtime_error("unique_lock: lock: not owning any mutex");
        if(m_owns)
            std::runtime_error("unique_lock: lock: recursive lock");
        
        return m_owns = m_mutex_ptr->try_lock();
    }

    void unlock() {
        if(!m_owns)
            std::runtime_error("unique_lock: unlock: not owning lock to unlock it");
        else if(m_mutex_ptr) {
            m_mutex_ptr->unlock();
            m_owns = false;
        }
    }

    mutex_type*
    release() {
        mutex_type* ret = m_mutex_ptr;
        m_mutex_ptr = nullptr;
        m_owns = false;
        return ret;
    }

    bool
    owns_lock() const
    { return m_owns; }

    explicit 
    operator bool() const
    { return owns_lock(); }

    mutex_type*
    mutex() const
    { return m_mutex_ptr; }

private:
    friend class condition_variable;
    void fake_lock()
    { m_owns = true; }

    void fake_unlock()
    { m_owns = false; }

    mutex_type* m_mutex_ptr;
    bool m_owns;

}; // class unique_lock

template<typename Mutex_T>
class lock_guard {

public:
    typedef Mutex_T mutex_type;

    lock_guard(mutex_type& mut): m_mutex_ptr(&mut)
    { m_mutex_ptr->lock(); }

    lock_guard(mutex_type& mut, adopt_lock_t): m_mutex_ptr(&mut) {}

    lock_guard(const lock_guard& other) = delete;
    lock_guard& operator=(const lock_guard& other) = delete;

    ~lock_guard()
    { m_mutex_ptr->unlock(); }

private:
    mutex_type* const m_mutex_ptr;

}; // class lock_guard

} // namespace concurrency

#endif