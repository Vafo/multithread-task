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

} // namespace concurrency