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
    native_handle_type m_handle;

private:
    basic_mutex(const basic_mutex& other) = delete;
    basic_mutex& operator=(const basic_mutex& other) = delete;

protected:
    basic_mutex()
        : m_handle()
    {/*no mutex is created. created by derived*/}

    ~basic_mutex();

public:
    void lock();

    bool try_lock();

    void unlock();

public:
    native_handle_type*
    native_handle()
    { return &m_handle; }

}; // class basic_mutex 

class mutex: public basic_mutex {
public:
    mutex();
}; // class mutex

class recursive_mutex: public basic_mutex {
public:
    recursive_mutex();
}; // class recursive_mutex

} // namespace concurrency

#endif