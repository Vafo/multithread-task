#include <catch2/catch_all.hpp>

#include "condition_variable.hpp"

#include "jthread.hpp"
#include "thread.hpp"
#include "mutex.hpp"

namespace concurrency {

void increment_var(
    int* var, mutex* var_mutex, int iterations,
    int* command, mutex* command_mutex, condition_variable* command_cv,
    int* ready, mutex* ready_mutex, condition_variable* ready_cv,
    int command_valid
) {
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

TEST_CASE("condition_variable: creation and deletion", "[condition_variable]") {
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

    {
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

        SECTION("commands 1 -> 2") {
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

            REQUIRE(count1 == iterations);
            REQUIRE(count2 == iterations);

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

            REQUIRE(count1 == 2 * iterations);
            REQUIRE(count2 == 2 * iterations);
        }

        SECTION("commands 2 -> 1") {
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
                        return ready == 2;
                    }
                );
                /* workers are ready, release mutex*/
            }

            REQUIRE(count1 == iterations);
            REQUIRE(count2 == iterations);

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
                        return ready == 4;
                    }
                );
                /* workers are ready, release mutex*/
            }

            REQUIRE(count1 == 2 * iterations);
            REQUIRE(count2 == 2 * iterations);
        }

    }

}

} // namespace concurrency