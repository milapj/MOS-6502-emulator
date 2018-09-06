#pragma once

#include <base.h>
#include <memory.h>

#include <stdint.h>
#include <stdbool.h>

// See the 'MMC1 Explainer.txt' for more detail

typedef enum mmc1_mirroring {
	MMC1_MIRRORING_ONE_SCREEN_NT0 = 0,
	MMC1_MIRRORING_ONE_SCREEN_NT1 = 1,
	MMC1_MIRRORING_VERTICAL = 2,
	MMC1_MIRRORING_HORIZONTAL = 3,
} PACKED mmc1_mirroring_t;

typedef enum mmc1_prgrom_fixation {
	MMC1_LOW_PRGROM_FIXED = 0,
	MMC1_HIGH_PRGROM_FIXED = 1,
} PACKED mmc1_prgrom_fixation_t;

typedef enum mmc1_prgrom_switching {
	MMC1_PRGROM_SWITCHING_32K = 0,
	MMC1_PRGROM_SWITCHING_16K = 1,
} PACKED mmc1_prgrom_switching_t;

typedef enum mmc1_chr_switching {
	MMC1_CHR_SWITCHING_8K = 0,
	MMC1_CHR_SWITCHING_4K = 1,
} PACKED mmc1_chr_switching_t;

typedef union mmc1_reg0 {
	uint8_t val;
	struct {
		mmc1_mirroring_t mirroring : 2;
		mmc1_prgrom_fixation_t prgrom_fixation : 1;
		mmc1_prgrom_switching_t prgrom_switching : 1;
		mmc1_chr_switching_t chr_switching : 1;
	};
} mmc1_reg0_t;

typedef union mmc1_reg3 {
	uint8_t val;
	struct {
		uint8_t banksel : 4;
		bool wram_disabled : 1;
	};
} mmc1_reg3_t;

// A virtual MMC1 device
typedef struct mmc1 {
	uint64_t last_cpu_cyclenum;
	uint8_t shiftreg;

	// DO NOT change the order of the registers in this struct, or insert
	// other fields between them, or make any of them bigger than a byte. We
	// rely on this packed ordering for evil memory reinterpretation hacks.
	mmc1_reg0_t reg0;
	uint8_t reg1;
	uint8_t reg2;
	mmc1_reg3_t reg3;
} mmc1_t;

// Puts an MMC1 into its reset state
void mmc1_reset (mmc1_t * nonnull mmc1);

// Handles a raw write into the MMC1's serial IO region
void mmc1_reg_write (mmc1_t * nonnull mmc1,
		     size_t regnum,
		     uint8_t val,
		     uint64_t cpu_cyclenum);

// Sets up VRAM mappings to a memory bus based on an MMC1's mapping state
void mmc1_map_vram (mmc1_t * nonnull mmc1,
		    membus_t * nonnull bus,
		    memory_t * nonnull vram);
