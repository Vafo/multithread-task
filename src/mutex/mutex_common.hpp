#ifndef MUTEX_COMMON_H
#define MUTEX_COMMON_H

// Helper classes for mutex
struct defer_lock_t {};
struct try_to_lock_t {};
struct adopt_lock_t {};

constexpr defer_lock_t	defer_lock { };
constexpr try_to_lock_t	try_to_lock { };
constexpr adopt_lock_t	adopt_lock { };

#endif