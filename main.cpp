#include <iostream>

#include "jthread.hpp"
#include "thread.hpp"

#include "mutex.hpp"
#include "lock_guard.hpp"

#include "condition_variable.hpp"

concurrency::mutex cout_mutex;

volatile long val = 0;
concurrency::mutex val_mutex;

void incr_func(int amount) {
    int tmp;
    while (amount-- > 0) {
        concurrency::lock_guard<concurrency::mutex> locker(val_mutex);
        // some redundant work
        tmp = val + 1;
        tmp--;
        val = tmp + 1;
    }
}

void funcy(int a, int b, int sleep_ms) {
    while (a > 0) {
        // cout_mutex.lock();
        for(int i = 0; i < 3; ++i)
            std::cout << "Thread " << b << " " << i << " count: " << a << std::endl;
        --a;

        // std::this_thread::sleep_for( std::chrono::milliseconds(sleep_ms) );
        // cout_mutex.unlock();
    }
}

void infinite_func(int tr_idx) {
    for(;;) {
        for(int i = 0; i < 25; ++i)
            std::cout << tr_idx << " ";
        std::cout << std::endl;
    }
}

void increment_var(
    int* var, concurrency::mutex* var_mutex, int iterations,
    int* command, concurrency::mutex* command_mutex, concurrency::condition_variable* command_cv,
    int* ready, concurrency::mutex* ready_mutex, concurrency::condition_variable* ready_cv,
    int command_valid
) {
    using concurrency::mutex;
    using concurrency::condition_variable;
    using concurrency::lock_guard;
    using concurrency::unique_lock;

    /*wait for command*/
    {
        unique_lock<mutex> command_locker(*command_mutex);
        command_cv->wait(
            command_locker,
            [command, command_valid] () { return *command == command_valid; }
        );
        /*command received, release command mutex*/
    }

    /*modify value*/
    {
        lock_guard<mutex> locker(*var_mutex);
        for(int i = 0; i < iterations; ++i)
            *var += 1;
    }

    /*notify on finishing*/
    {
        lock_guard<mutex> locker(*ready_mutex);
        *ready += 1;
        ready_cv->notify_all();
    }
}

int main(int argc, char* argv[]) {
    using concurrency::jthread;
    using concurrency::mutex;
    using concurrency::condition_variable;
    using concurrency::lock_guard;
    using concurrency::unique_lock;

    const int val_init = 0;
    const int iterations = 5000;

    /*Shared command*/
    int command = 0;
    condition_variable command_cv;
    mutex command_mutex;
    
    /*Shared ready*/
    int ready = 0;
    condition_variable ready_cv;
    mutex ready_mutex;

    /*Value 1*/
    int count1 = val_init;
    mutex count1_mutex;

    /*Value 2*/
    int count2 = val_init;
    mutex count2_mutex;

    /*Threads creation*/
    jthread count1_worker1(
        increment_var,
        &count1, &count1_mutex, iterations,
        &command, &command_mutex, &command_cv,
        &ready, &ready_mutex, &ready_cv,
        1 /*command for worker1*/
    );

    jthread count1_worker2(
        increment_var,
        &count1, &count1_mutex, iterations,
        &command, &command_mutex, &command_cv,
        &ready, &ready_mutex, &ready_cv,
        2 /*command for worker2*/
    );

    jthread count2_worker1(
        increment_var,
        &count2, &count2_mutex, iterations,
        &command, &command_mutex, &command_cv,
        &ready, &ready_mutex, &ready_cv,
        1 /*command for worker1*/
    );

    jthread count2_worker2(
        increment_var,
        &count2, &count2_mutex, iterations,
        &command, &command_mutex, &command_cv,
        &ready, &ready_mutex, &ready_cv,
        2 /*command for worker2*/
    );

    /*First iteration*/
    std::cout << "first iteration" << std::endl;
    std::cout << "count1 " << count1 << "\tcount2 " << count2 << std::endl;

    /*Issue command 1*/
    {
        lock_guard<mutex> locker(command_mutex);
        command = 1;
        command_cv.notify_all();
    }

    /*wait for workers 1 to be ready*/
    {
        unique_lock<mutex> locker(ready_mutex);
        // int* ready_ptr = &ready;
        ready_cv.wait(
            locker,
            [&ready] () { 
                return ready == 2;
            }
        );
        /* workers are ready, release mutex*/
    }


    std::cout << "count1 " << count1 << "\tcount2 " << count2 << std::endl;

    /*Issue command 2*/
    {
        lock_guard<mutex> locker(command_mutex);
        command = 2;
        command_cv.notify_all();
    }

    /*wait for workers 2 to be ready*/
    {
        unique_lock<mutex> locker(ready_mutex);
        // int* ready_ptr = &ready;
        ready_cv.wait(
            locker,
            [&ready] () { 
                return ready == 4;
            }
        );
        /* workers are ready, release mutex*/
    }

    std::cout << "Result" << std::endl;
    std::cout << "count1 " << count1 << "\tcount2 " << count2 << std::endl;

    // /*Second iteration*/
    // std::cout << "second iteration" << std::endl;
    // std::cout << "count1 " << count1 << "\tcount2 " << count2 << std::endl;
    // /*Issue command 2*/
    // {
    //     lock_guard<mutex> locker(command_mutex);
    //     command = 2;
    //     command_cv.notify_all();
    // }

    // /*wait for workers 2 to be ready*/
    // {
    //     unique_lock<mutex> locker(ready_mutex);
    //     // int* ready_ptr = &ready;
    //     ready_cv.wait(
    //         locker,
    //         [&ready] () { 
    //             return ready == 2;
    //         }
    //     );
    //     /* workers are ready, release mutex*/
    // }


    // std::cout << "count1 " << count1 << "\tcount2 " << count2 << std::endl;

    // /*Issue command 1*/
    // {
    //     lock_guard<mutex> locker(command_mutex);
    //     command = 1;
    //     command_cv.notify_all();
    // }

    // /*wait for workers 1 to be ready*/
    // {
    //     unique_lock<mutex> locker(ready_mutex);
    //     // int* ready_ptr = &ready;
    //     ready_cv.wait(
    //         locker,
    //         [&ready] () { 
    //             return ready == 4;
    //         }
    //     );
    //     /* workers are ready, release mutex*/
    // }

    // std::cout << "Result" << std::endl;
    // std::cout << "count1 " << count1 << "\tcount2 " << count2 << std::endl;

}