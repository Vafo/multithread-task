#include <iostream>

#include "thread.hpp"

int funcy(int a, int b) {
    
    while (a > 0)
        std::cout << "Thread " << b << " count: " << a-- << std::endl;


}

int main(int argc, char* argv[]) {

    concurrency::thread tr1(funcy, 5, 1);
    concurrency::jthread tr2(funcy, 5, 2);

    tr1.join();
    // tr2.detach();
    
    return 0;
}