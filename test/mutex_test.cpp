#include <catch2/catch_all.hpp>

#include "thread.hpp"
#include "mutex.hpp"

#include <iostream>

namespace concurrency {

void factorial_multiply(unsigned long long* val, int n, mutex* mx) {
    for(int i = 2; i <= n; ++i) {
        mx->lock();
        *val *= i;
        mx->unlock();
    }
}

TEST_CASE("mutex: creation and deletion", "[mutex]") {
    mutex mx;

    REQUIRE_NOTHROW(mx.lock());
    REQUIRE(mx.try_lock() == false);
    /*locking non-recursive mutex*/
    REQUIRE_NOTHROW(mx.unlock());
    REQUIRE(mx.try_lock() == true);
    REQUIRE_NOTHROW(mx.unlock());
}

TEST_CASE("mutex: integration with threads", "[mutex]") {
    const int val_init = 1;
    const int val_amount_1 = 10;
    const int val_amount_2 = 11;
    const int val_amount_3 = 12;
    const int val_amount_4 = 13;
    
    unsigned long long val = val_init;
    mutex mx;
    
    thread tr1(factorial_multiply, &val, val_amount_1, &mx);
    thread tr2(factorial_multiply, &val, val_amount_2, &mx);
    thread tr3(factorial_multiply, &val, val_amount_3, &mx);
    thread tr4(factorial_multiply, &val, val_amount_4, &mx);

    tr1.join();
    tr2.join();
    tr3.join();
    tr4.join();
    
    // Serial calculation of factorials multiplication
    unsigned long long real_val = val_init;
    factorial_multiply(&real_val, val_amount_1, &mx);
    factorial_multiply(&real_val, val_amount_2, &mx);
    factorial_multiply(&real_val, val_amount_3, &mx);
    factorial_multiply(&real_val, val_amount_4, &mx);

    REQUIRE(val == real_val);
}

} // namespace concurrency