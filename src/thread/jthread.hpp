#ifndef JTHREAD_H
#define JTHREAD_H

#include "thread.hpp"

namespace concurrency {

class jthread: public thread {
public:
    jthread()
        : thread()
    {}

    template<typename Callable, typename ...Args>
    jthread(Callable cb, Args ...args)
        : thread(cb, args...)
    {}

    jthread(jthread&& other)
    { swap(other); }

    jthread& operator=(jthread&& other) {
        jthread(std::move(other)).swap(*this); // move to tmp & swap
        return *this;
    };

    ~jthread() {
        if(joinable()) {
            join();
        }
    }

public:
    void swap(jthread& other) { 
        using std::swap;

        thread::swap(other);
    }

    friend void swap(jthread& lhs, jthread& rhs)
    { lhs.swap(rhs); }

}; // class jthread

} // namespace concurrency

#endif