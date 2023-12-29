#include <catch2/catch_all.hpp>

#include <string>

#include "util.hpp"
#include "concurrency_common.hpp"

namespace concurrency::util {

std::string error_msg_gen(std::string prefix, int err_code) {
    return prefix + "error code: " + std::to_string(err_code);
}

TEST_CASE("cerror_code: creation", "[cerror_code]") {
    const int valid_val = 0;
    const int invalid_val = 123;
    static_assert(valid_val != invalid_val);

    cerror_code<int> code("func", error_msg_gen, valid_val, true);
    
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
        valid_val, false
    );

    REQUIRE_NOTHROW(code = invalid_val);

    REQUIRE(code); /*invalid retval*/
    REQUIRE_THROWS(code.throw_exception());

    REQUIRE_NOTHROW(code = valid_val);
    REQUIRE_FALSE(code); /*valid retval*/
    REQUIRE_THROWS(code.throw_exception());
}

/*Copy of cerror_code tests*/
TEST_CASE("posix_error: default", "[posix_error]") {
    const int valid_val = 0;
    const int invalid_val = 123;
    static_assert(valid_val != invalid_val);

    posix_error code(__FUNCTION__);

    REQUIRE_NOTHROW(code = invalid_val);

    REQUIRE(code); /*invalid retval*/
    REQUIRE_THROWS(code.throw_exception());

    REQUIRE_NOTHROW(code = valid_val);
    REQUIRE_FALSE(code); /*valid retval*/
    REQUIRE_THROWS(code.throw_exception());
}

TEST_CASE("posix_error_throws: test", "[posix_error]") {
    const int valid_val = 0;
    const int invalid_val = 123;
    static_assert(valid_val != invalid_val);

    posix_error_throws code(__FUNCTION__);
    
    REQUIRE_THROWS(code = invalid_val);
    REQUIRE(!code.valid());
    REQUIRE(code.value() == invalid_val);

    REQUIRE_NOTHROW(code = valid_val);
    REQUIRE(code.valid());
    REQUIRE(code.value() == valid_val);
}

TEST_CASE("scoped_guard: creation & execution", "[scoped_guard]") {
    scoped_guard defered(
        [] () {
            REQUIRE(true);
        }
    );
}

TEST_CASE("scoped_guard: cancel & assign", "[scoped_guard]") {
    auto false_func =
        [] () {
            REQUIRE(false);
        };

    auto true_func =
        [] () {
            REQUIRE(true);
        };
    
    SECTION("cancel"){
        scoped_guard defered(false_func);

        defered.cancel();
    }

    SECTION("assign (reassign)") {
        scoped_guard defered(false_func);

        defered.assign(true_func);
    }
}

} // namespace concurrency::util