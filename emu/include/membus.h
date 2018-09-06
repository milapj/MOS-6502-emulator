#pragma once

#include <base.h>
#include <reset_manager.h>
#include <stddef.h>
#include <stdint.h>

#define MEMBUS_PAGESIZE 256
#define MEMBUS_NPAGES 256

// The mappings for 'read' and 'write' behavior are stored separately, in two
// arrays. Each entry in an array can correspond to no mapping, a "handler
// mapping", or a "data mapping". If the `obj` of an entry is `NULL`, then there
// is no mapping for the corresponding page. Otherwise, if `offset_p1` is 0, the
// page maps directly to `data`. Otherwise, the page is mapped to a custom
// handler, and `offset_p1` is one plus the offset given when setting up the
// handler.
typedef struct membus {
	struct {
		union {
			uint8_t (* nullable handler)(void * nonnull, uint16_t, uint8_t * nonnull);
			uint8_t * nullable /*unowned*/ data;
		};
		size_t offset_p1; // offset + 1
		void * nullable /*strong*/ obj;
	} read_mappings[MEMBUS_NPAGES];

	struct {
		union {
			void (* nullable handler)(void * nonnull, uint16_t, uint8_t);
			uint8_t * nullable /*unowned*/ data;
		};
		size_t offset_p1; // offset + 1
		void * nullable /*strong*/ obj;
	} write_mappings[MEMBUS_NPAGES];

#ifndef OPEN_BUS_TO_VCC
	uint8_t data_lanes;
#endif
} membus_t;

// Allocates and initializes a new reference-counted memory bus. The passed
// reset manager is used to clear latent values on the bus lanes if
// `OPEN_BUS_TO_VCC` is not defined.
membus_t * nullable membus_new (reset_manager_t * nonnull rm);

// Reads a byte through the bus
uint8_t membus_read (membus_t * nonnull bus, uint16_t addr);

// Writes a byte through the bus
void membus_write (membus_t * nonnull bus, uint16_t addr, uint8_t val);

// Removes all mappings for a particular page
void membus_clear_page (membus_t * nonnull bus, size_t pagenum);

// Arranges for `handler` to be called to handle reads from `pagenum`.
// `handler` should be compatible with `membus_read_handler_t` (it's fine to
// ignore `lanemask`), and will be passed `obj` as its first argument at each
// invocation. Addresses passed to the handler are the sum of the address
// relative to the start of the page and the `offset`. `obj` will be strongly
// referenced for the duration of the mapping. This replaces any previous
// 'read' mappings for `pagenum`.
void membus_set_read_handler (membus_t * nonnull bus,
			      size_t pagenum,
			      void * nonnull obj,
			      size_t offset,
			      void * nonnull handler);

// Arranges for `handler` to be called to handle writes to `pagenum`.
// `handler` should be compatible with `membus_write_handler_t`, and will be
// passed `obj` as its first argument at each invocation. Addresses passed to
// `handler` are the sum of the address relative to the start of the page and
// `offset`. `obj` will be strongly referenced for the duration of the mapping.
// This replaces any previous 'write' mappings for `pagenum`.
void membus_set_write_handler (membus_t * nonnull bus,
			       size_t pagenum,
			       void * nonnull obj,
			       size_t offset,
			       void * nonnull handler);

// Arranges for reads from `pagenum` to be redirected to the native 256-byte
// memory region starting at `data`. This is the most performant way to map
// virtual RAMs and ROMs to a bus. `obj` will be strongly referenced for the
// duration of the mapping, but otherwise not used in any way. This replaces
// any previous 'read' mappings to `pagenum`.
void membus_set_read_memory (membus_t * nonnull bus,
			     size_t pagenum,
			     void * nonnull obj,
			     void * nonnull data);

// Arranges for writes to `pagenum` to be redirected to the native 256-byte
// memory region starting at `data`. This is the most performant way to map
// virtual RAMs to a bus. `obj` will be strongly referenced for the duration of
// the mapping, but otherwise not used in any way. This replaces any previous
// 'write' mappings to `pagenum`.
void membus_set_write_memory (membus_t * nonnull bus,
			      size_t pagenum,
			      void * nonnull obj,
			      void * nonnull data);
