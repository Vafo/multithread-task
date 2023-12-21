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

} // namespace concurrency

#endif