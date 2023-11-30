#include <iostream>

#include "thread.hpp"
#include "mutex.hpp"


#include <chrono>
#include <thread>

concurrency::mutex cout_mutex;

void funcy(int a, int b, int sleep_ms) {
    while (a > 0) {
        // cout_mutex.lock();
        std::cout << "Thread " << b << " count: " << a-- << std::endl;
        std::this_thread::sleep_for( std::chrono::milliseconds(sleep_ms) );
        // cout_mutex.unlock();
    }
}

int main(int argc, char* argv[]) {

    concurrency::thread tr1(funcy, 5, 1, 900);
    concurrency::thread tr2(funcy, 5, 2, 300);

    tr1.join();
    tr2.join();
    // tr2.detach();
    
    return 0;
}