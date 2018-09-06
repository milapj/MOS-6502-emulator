#pragma once

#include <base.h>
#include <stddef.h>
#include <stdint.h>

// The "header" that precedes every reference-counted allocation. This header
// implicitly exists in such allocations, but is not included in any pointers
// returned by or given to the reference-counting routines.
typedef struct rc {
	size_t weak_count;
	size_t strong_count;
	void (*nullable deinit)(void * nonnull obj);
} rc_t;

// Allocates a new reference-counted block of memory with a strong reference
// count of 1 and a weak reference count of 0, and zero-initializes it. If
// `deinit` is non-null, it is called on the "object" when its reference count
// hits zero.
void * nonnull rc_alloc (size_t size, void * nullable deinit);

// Increments the strong reference count of `obj`, and returns it.
void * nonnull rc_retain (void * nonnull obj);

// Decrements the strong reference count of `obj`. The deinitializer will be run
// as soon as the strong reference count hits zero, but the object's memory will
// not be freed until the weak reference count is also zero.
void rc_release (void * nonnull obj);

// Increments the weak reference count of `obj` and returns it. While the weak
// reference count remains nonzero, the object's memory remains allocated, but
// its deinitializer will have been run if the strong reference count is zero.
void * nonnull rc_weak_retain (void * nonnull obj);

// Decrements the weak reference count of `obj`. If the weak reference count
// hits zero and the strong reference count is already zero, then the object's
// memory will be immediately freed.
void rc_weak_release (void * nonnull obj);

// Checks that the object pointed to be `*obj_ptr` has a nonzero strong
// reference count (i.e. it is still valid). If it does, `*obj_ptr` is
// unmodified and returned. Otherwise, an `rc_weak_release()` is performed and
// `*obj_ptr` is set to NULL and returned (thus returning `NULL`). This should
// be used to ensure object validity whenever accessing a weakly-referenced
// object, and ideally frequently to allow the memory held by weakly-referenced
// objects to be freed.
void * nullable rc_weak_check (void * nullable * nonnull obj_ptr);
