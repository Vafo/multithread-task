#include <catch2/catch_all.hpp>

#include <string>

#include "util.hpp"

namespace concurrency::util {

std::string error_msg_gen(std::string prefix, int err_code) {
    return prefix + "error code: " + std::to_string(err_code);
}

TEST_CASE("cerror_code: creation", "[cerror_code]") {
    const int valid_val = 0;
    const int invalid_val = 123;
    static_assert(valid_val != invalid_val);

    cerror_code<int> code("func", error_msg_gen, valid_val);
    
    REQUIRE_THROWS(code = invalid_val);
    REQUIRE(!code.valid());
    REQUIRE(code.value() == invalid_val);

    REQUIRE_NOTHROW(code = valid_val);
    REQUIRE(code.valid());
    REQUIRE(code.value() == valid_val);
}

TEST_CASE("cerror_code: no_auto_throw", "[cerror_code]") {
    const int valid_val = 0;
    const int invalid_val = 123;
    static_assert(valid_val != invalid_val);

    cerror_code<int> code(
        "func", error_msg_gen,
        valid_val, no_auto_throw
    );

    REQUIRE_NOTHROW(code = invalid_val);

    REQUIRE(code); /*invalid retval*/
    REQUIRE_THROWS(code.throw_exception());

    REQUIRE_NOTHROW(code = valid_val);
    REQUIRE_FALSE(code); /*valid retval*/
    REQUIRE_THROWS(code.throw_exception());
}

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