#ifndef MUTEX_H
#define MUTEX_H

#include <pthread.h>

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

private:
    native_handle_type m_handle;

}; // class mutex

} // namespace concurrency

#endif