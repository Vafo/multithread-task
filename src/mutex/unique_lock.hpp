#ifndef UNIQUE_LOCK_H
#define UNIQUE_LOCK_H

#include "mutex_common.hpp"
#include <stdexcept>

namespace concurrency {

template<typename Mutex_T>
class unique_lock {
public:
    typedef Mutex_T mutex_type;

private:
    mutex_type* m_mutex_ptr;
    bool m_owns;

private:
    unique_lock(const unique_lock& other) = delete;
    unique_lock& operator=(const unique_lock& other) = delete;

public:
    unique_lock()
        : m_mutex_ptr(nullptr)
        , m_owns(false)
    {}
   
	unique_lock(unique_lock&& other)
		: m_mutex_ptr(other.m_mutex_ptr)
		, m_owns(other.m_owns)
	{ other.release(); }

	unique_lock& operator=(unique_lock&& other) {
		unique_lock(std::move(other)).swap(*this);
		return *this;
	}

    ~unique_lock() {
        if(m_mutex_ptr && m_owns)
            unlock();
    }

public:
	/* lock */
    explicit
    unique_lock(mutex_type& mut)
        : m_mutex_ptr(&mut)
        , m_owns(false)
    { lock(); }

    /*do not lock*/
    unique_lock(mutex_type& mut, defer_lock_t)
        : m_mutex_ptr(&mut)
        , m_owns(false)
    {}

    /*try to lock*/
    unique_lock(mutex_type& mut, try_to_lock_t)
        : m_mutex_ptr(&mut)
        , m_owns(false) 
    { m_owns = m_mutex_ptr->try_lock(); }

    /*caller thread has already locked mutex*/
    unique_lock(mutex_type& mut, adopt_lock_t)
        : m_mutex_ptr(&mut)
        , m_owns(true)
    {}
    
public:
    void lock() {
        if(!m_mutex_ptr)
            throw std::runtime_error("unique_lock: lock: not owning any mutex");
        if(m_owns)
            throw std::runtime_error("unique_lock: lock: recursive lock"); /*what if mutex_type is recursive_mutex ? template specialization*/
        
        m_mutex_ptr->lock();
        m_owns = true;
    }

    bool try_lock() {
        if(!m_mutex_ptr)
            throw std::runtime_error("unique_lock: lock: not owning any mutex");
        if(m_owns)
            throw std::runtime_error("unique_lock: lock: recursive lock");
        
        return m_owns = m_mutex_ptr->try_lock();
    }

    void unlock() {
        if(!owns_lock())
            throw std::runtime_error("unique_lock: unlock: not owning a mutex to unlock it");
        else if(m_mutex_ptr) {
            m_mutex_ptr->unlock();
            m_owns = false;
        }
    }

public:
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

public:
    bool
    owns_lock() const
    { return m_owns && (m_mutex_ptr != nullptr); }

    explicit 
    operator bool() const
    { return owns_lock(); }

public:
	void swap(unique_lock& other) {
		using std::swap;
		swap(m_mutex_ptr, other.m_mutex_ptr);
		swap(m_owns, other.m_owns);
	}

	friend
	void swap(unique_lock& a, unique_lock& b)
	{ a.swap(b); }

}; // class unique_lock

} // namespace concurrency

#endif
