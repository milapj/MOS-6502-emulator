#pragma once

// Virtually any emulated device in Hawknest can be "reset" in a way that
// emulates receiving a reset signal. It's useful to be able to reset an entire
// system at once, but since not all devices have obvious "owners", a simple
// reset hierarchy would be awkward. Instead, a separate object called the
// reset manager keeps a singular reference to each object that should be reset
// during a system-wide reset, along with a pointer to the function that
// performs the reset procedure. Most `_new` functions take a reset manager as
// an argument, and register the newly created object with the reset manager
// before returning it.

#include <base.h>

#include <stddef.h>

#define RESET_MANAGER_MAX_DEVICES 256

typedef struct reset_manager {
	struct {
		void (*nullable handler)(void * nonnull dev);
		void * nullable /*weak*/ dev;
	} devices[RESET_MANAGER_MAX_DEVICES];
	size_t ndevices;
} reset_manager_t;

// Creates a new reference-counted reset manager
reset_manager_t * nullable reset_manager_new (void);

// Registers a new device with a reset manager
void reset_manager_add_device (reset_manager_t * nonnull rm, void * nonnull dev, void * nonnull handler);

// Issues a reset to all devices attached to a reset manager
void reset_manager_issue_reset (reset_manager_t * nonnull rm);
