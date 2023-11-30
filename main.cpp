#include <iostream>

#include "thread.hpp"
#include "mutex.hpp"


#include <chrono>
#include <thread>

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

        std::this_thread::sleep_for( std::chrono::milliseconds(sleep_ms) );
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

int main(int argc, char* argv[]) {

    // concurrency::thread tr1(funcy, 5, 1, 900);
    // concurrency::thread tr2(funcy, 5, 2, 300);

    // tr1.join();
    // tr2.join();
    // tr2.detach();
    
    // concurrency::thread tr1(incr_func, 1'000'000);
    // concurrency::thread tr2(incr_func, 1'000'000);
    // concurrency::thread tr3(incr_func, 1'000'000);

    {
        concurrency::jthread tr1(incr_func, 1'000'000);
        concurrency::jthread tr2(incr_func, 1'000'000);
        concurrency::jthread tr3(incr_func, 1'000'000);
    }

    std::cout << val << std::endl;

    return 0;
}