#include <catch2/catch_all.hpp>

#include "thread.hpp"

namespace concurrency {

void increment_by_1(int* var) {
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
}

} // namespace concurrency