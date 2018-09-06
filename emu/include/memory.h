#pragma once

#include <base.h>
#include <membus.h>
#include <reset_manager.h>

#include <stdint.h>
#include <stdbool.h>

// A virtual memory device, implemented as a byte vector
typedef struct memory {
	size_t size;
	bool writeable;
	uint8_t bytes[];
} memory_t;

// Allocates and initializes a new reference-counted memory object
memory_t * nullable memory_new (reset_manager_t * nonnull rm,
				size_t size,
				bool writeable);

// Maps a `size` region of `mem` (starting at `start`) to `bus` (starting at
// `bus_start`)
void memory_map (memory_t * nonnull mem,
		 membus_t * nonnull bus,
		 uint16_t bus_start,
		 uint16_t size,
		 size_t start);

// Works identically to `memory_map`, except repeats the mapping `nmirrors`
// times over, to successive regions of `bus`
void memory_map_mirroring (memory_t * nonnull mem,
			   membus_t * nonnull bus,
			   uint16_t bus_start,
			   uint16_t size,
			   size_t start,
			   size_t nmirrors);
