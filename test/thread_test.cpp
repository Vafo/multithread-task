#include <catch2/catch_all.hpp>

#include "thread.hpp"
#include "mutex.hpp"

namespace concurrency {

void increment_by_1(int* var) {
    *var += 1;
}

void lock_guard_incrementer_func(int* var, int iterations, mutex* mut) {
    lock_guard<mutex> locker(*mut);
    for(int i = 0; i < iterations; ++i)
        *var += 1;
}

void unique_lock_incrementer_func(int* var, int iterations, mutex* mut) {
    unique_lock<mutex> locker(*mut); /*acts like lock_guard*/
    for(int i = 0; i < iterations; ++i)
        *var += 1;
}

void unsafe_incrementer_func(int* var, int iterations) {
    for(int i = 0; i < iterations; ++i)
        *var += 1;
}

void do_nothing() {
    int a;
}

TEST_CASE("thread: creation and deletion", "[thread]") {
    thread tr1(do_nothing);

    REQUIRE_NOTHROW(tr1.join());
}

TEST_CASE("thread: execution of user function", "[thread]") {
    const int val_init = 0;
    int val = val_init;
    thread tr(increment_by_1, &val);

    tr.join();

    REQUIRE(val == val_init + 1);
}

TEST_CASE("thread: joining thread", "[thread]") {
    thread tr_empty;

    REQUIRE_THROWS(tr_empty.join());
    REQUIRE_THROWS(tr_empty.detach());

    REQUIRE(tr_empty.joinable() == false);

    thread tr(do_nothing);
    
    REQUIRE(tr.joinable() == true);

    tr.join(); 

    REQUIRE(tr.joinable() == false);
    REQUIRE_THROWS(tr.join());

}

TEST_CASE("jthread: creation and deletion", "[thread]") {
    const int val_init = 0;
    int val = val_init;
    jthread tr_add(increment_by_1, &val);

    REQUIRE_NOTHROW(tr_add.join());
    REQUIRE(val == val_init + 1);
}

/* Copy of thread test case */
TEST_CASE("jthread: empty jthread object", "[thread]") {
    jthread tr_empty;

    REQUIRE_THROWS(tr_empty.join());
    REQUIRE_THROWS(tr_empty.detach());

    REQUIRE(tr_empty.joinable() == false);

    thread tr(do_nothing);
    
    REQUIRE(tr.joinable() == true);

    tr.join(); 

    REQUIRE(tr.joinable() == false);
    REQUIRE_THROWS(tr.join());
}

TEST_CASE("jthread: no explicit join() call", "[thread]") {
    jthread tr1(do_nothing);
    int val = 0;
    jthread tr2(increment_by_1, &val);

    /* should execute terminate on non-joined threads */
    /* have to add explicit check if this execution happens */
}

TEST_CASE("jthread: jthreads and lock_guard", "[thread]") {
    const int val_init = 0;
    const int iterations = 1'000'000;
    int val = val_init;
    {
        mutex mut;
        jthread tr1(lock_guard_incrementer_func, &val, iterations, &mut);
        jthread tr2(lock_guard_incrementer_func, &val, iterations, &mut);
    }
    REQUIRE(val == iterations * 2);
}

TEST_CASE("jthread: jthreads and unique_lock", "[thread]") {
    const int val_init = 0;
    const int iterations = 1'000'000;
    int val = val_init;
    {
        mutex mut;
        jthread tr1(unique_lock_incrementer_func, &val, iterations, &mut);
        jthread tr2(unique_lock_incrementer_func, &val, iterations, &mut);
    }
    REQUIRE(val == iterations * 2);
}

} // namespace concurrency