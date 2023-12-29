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
    
    SECTION("release"){
        scoped_guard defered(false_func);

        defered.release();
    }

    SECTION("assign (reassign)") {
        scoped_guard defered(false_func);

        defered.assign(true_func);
    }
}


struct bad_obj {
    bad_obj() {
        throw std::runtime_error("bad object");
    }

    int big_data;
};

template<typename Allocator>
void leak_safe_constructor(bad_obj* ptr) {
    using allocator_t = std::allocator<bad_obj>;
    Allocator bad_alloc;

    scoped_ptr<bad_obj> holder(ptr);
    std::allocator_traits< allocator_t >::construct(bad_alloc, ptr); // throws

    scoped_release(holder);
}

TEST_CASE("scoped_ptr: bad constructor", "[scoped_ptr]") {
    using allocator_t = std::allocator<bad_obj>;
    allocator_t bad_alloc;
    bad_obj* ptr = std::allocator_traits< allocator_t >::allocate(bad_alloc, 1);

    REQUIRE_THROWS(leak_safe_constructor<allocator_t>(ptr));
}

TEST_CASE("scoped_ptr: get & move", "[scoped_ptr]") {
    const int test_val = 123;
    scoped_ptr<int> int_ptr; // scoped ptr is just a unique_ptr
    REQUIRE(int_ptr.get() == nullptr);
    REQUIRE(int_ptr.release() == nullptr);

    int* int_raw_ptr = new int(test_val);
    scoped_ptr<int> ptr_holder(int_raw_ptr);
    REQUIRE(ptr_holder.get() == int_raw_ptr);
    
    {
        scoped_ptr<int> ptr_taker( std::move(ptr_holder) );
        REQUIRE(ptr_holder.get() == nullptr);
        REQUIRE(ptr_taker.get() == int_raw_ptr);
    } /*memory released*/

    REQUIRE(ptr_holder.get() == nullptr);
}

} // namespace concurrency::util