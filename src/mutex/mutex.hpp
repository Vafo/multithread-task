#ifndef MUTEX_H
#define MUTEX_H

#include <pthread.h>
#include <stdexcept>
#include <cassert>

namespace concurrency {

class basic_mutex {

public:
    typedef pthread_mutex_t native_handle_type;

protected:
    basic_mutex()
    : m_handle()
    {/*no platform-specific mutex is created. created by derived*/}

    basic_mutex(const basic_mutex& other) = delete;

    ~basic_mutex();
    
    basic_mutex& operator=(const basic_mutex& other) = delete;

public:
    void lock();

    bool try_lock();

    void unlock();

    native_handle_type*
    native_handle()
    { return &m_handle; }

protected:
    native_handle_type m_handle;

}; // class basic_mutex 

class mutex: public basic_mutex {
public:
    mutex();

    mutex(const mutex& other) = delete;
    
    mutex& operator=(const mutex& other) = delete;
}; // class mutex

class recursive_mutex: public basic_mutex {
public:
    recursive_mutex();

    recursive_mutex(const recursive_mutex& other) = delete;
    
    recursive_mutex& operator=(const recursive_mutex& other) = delete;
}; // class recursive_mutex


// Helper classes for mutex
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

    unique_lock()
    : m_mutex_ptr(nullptr)
    , m_owns(false)
    { }

    unique_lock(const unique_lock& other) = delete;
    
    explicit
    unique_lock(mutex_type& mut)
    : m_mutex_ptr(&mut)
    , m_owns(false)
    { lock(); }

    /*do not lock*/
    unique_lock(mutex_type& mut, defer_lock_t)
    : m_mutex_ptr(&mut)
    , m_owns(false)
    { }

    /*try to lock*/
    unique_lock(mutex_type& mut, try_to_lock_t)
    : m_mutex_ptr(&mut)
    , m_owns(false) 
    { m_owns = m_mutex_ptr->try_lock(); }

    /*caller thread has already locked mutex*/
    unique_lock(mutex_type& mut, adopt_lock_t)
    : m_mutex_ptr(&mut)
    , m_owns(true)
    { }

    ~unique_lock() {
        if(m_mutex_ptr && m_owns)
            unlock();
    }

    unique_lock& operator=(const unique_lock& other) = delete;
    
    void lock() {
        if(!m_mutex_ptr)
            std::runtime_error("unique_lock: lock: not owning any mutex");
        if(m_owns)
            std::runtime_error("unique_lock: lock: recursive lock"); /*what if mutex_type is recursive_mutex ? template specialization*/
        
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
            std::runtime_error("unique_lock: unlock: not owning a mutex to unlock it");
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

    mutex_type*
    mutex() const
    { return m_mutex_ptr; }

    bool
    owns_lock() const
    { return m_owns; }

    explicit 
    operator bool() const
    { return owns_lock(); }

private:
    mutex_type* m_mutex_ptr;
    bool m_owns;

}; // class unique_lock

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