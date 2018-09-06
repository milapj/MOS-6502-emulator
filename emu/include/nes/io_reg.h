#pragma once

// The NES has a section of the CPU address space reserved for memory-mapped IO
// registers. Of those registers, we care about the two that enable reading
// button status on the two controllers, and the OAMDMA register that is used
// to quickly update the list of onscreen sprites for the PPU.

#include <membus.h>
#include <timekeeper.h>
#include <mos6502/mos6502.h>

#include <SDL2/SDL.h>

#include <stdint.h>
#include <stdbool.h>

#define CONTROLLER_NBUTTONS 8

// Numeric codes for each button on a controller
typedef enum controller_button {
	CONTROLLER_BUTTON_A = 0,
	CONTROLLER_BUTTON_B = 1,
	CONTROLLER_BUTTON_SELECT = 2,
	CONTROLLER_BUTTON_START = 3,
	CONTROLLER_BUTTON_UP = 4,
	CONTROLLER_BUTTON_DOWN = 5,
	CONTROLLER_BUTTON_LEFT = 6,
	CONTROLLER_BUTTON_RIGHT = 7,
} PACKED controller_button_t;

typedef struct io_reg {
	mos6502_t * nonnull /*unowned*/ cpu;
	bool controller_strobe;
	uint8_t controller_shiftregs[2];
	SDL_Scancode controller_mappings[CONTROLLER_NBUTTONS][2];
} io_reg_t;

// Creates a new "IO Register Device", which encompasses the memory-mapped IO
// registers we implement
io_reg_t * nullable io_reg_new (reset_manager_t * nonnull rm, mos6502_t * nonnull cpu, const char * nonnull cscheme_path);

// Maps an `io_reg_t` to the appropriate part of a CPU memory bus
void io_reg_map (io_reg_t * nonnull io, membus_t * nonnull bus);

// Creates a new `io_reg_t` via `io_reg_new`, and then maps it via `io_reg_map`
// to `cpu->bus`
int io_reg_setup (reset_manager_t * nonnull rm, mos6502_t * nonnull cpu, const char * nonnull cscheme_path);
