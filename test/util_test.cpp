#include <catch2/catch_all.hpp>

#include "util.h"

namespace concurrency::util {

TEST_CASE("defer_execution: creation & execution", "[defer_execution]") {
    defer_execution defered(
        [] () {
            REQUIRE(true);
        }
    );
}

TEST_CASE("defer_execution: cancel & assign", "[defer_execution]") {
    auto false_func =
        [] () {
            REQUIRE(false);
        };

    auto true_func =
        [] () {
            REQUIRE(true);
        };
    
    SECTION("cancel"){
        defer_execution defered(false_func);

        defered.cancel();
    }

    SECTION("assign (reassign)") {
        defer_execution defered(false_func);

        defered.assign(true_func);
    }
}

} // namespace concurrency::util