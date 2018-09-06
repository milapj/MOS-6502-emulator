#pragma once

#include <memory.h>
#include <nes/ppu.h>
#include <reset_manager.h>
#include <mos6502/mos6502.h>

// The two VRAM mirroring options in the ines header
typedef enum ines_mirroring {
	INES_MIRRORING_HORIZONTAL = 0,
	INES_MIRRORING_VERTICAL = 1,
} PACKED ines_mirroring_t;

// Everything extracted from a ines ROM, along with the relevant NES hardware
// objects, to be passed to a routine that sets up the system memory map
typedef struct rominfo {
	reset_manager_t * nonnull /*unowned*/ rm;
	mos6502_t * nonnull /*unowned*/ cpu;
	ppu_t * nonnull /*unowned*/ ppu;
	ines_mirroring_t mirroring;

	memory_t * nullable /*unowned*/ wram;
	memory_t * nullable /*unowned*/ prgrom;
	memory_t * nullable /*unowned*/ chrom;
	memory_t * nullable /*unowned*/ chram;
	memory_t * nullable /*unowned*/ vram;
} rominfo_t;

// Loads an ines-formatted ROM given a `FILE` object opened at `path`, setting
// up both the CPU and PPU memory maps in the process. Returns a nonzero exit
// code if an error occurs.
int inesrom_load (FILE * nonnull f,
		  const char * nonnull path,
		  reset_manager_t * nonnull rm,
		  mos6502_t * nonnull cpu,
		  const char * nonnull palette_path,
		  const char * nonnull cscheme_path,
		  int scale);
