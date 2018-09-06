#pragma once

#include <membus.h>
#include <timekeeper.h>
#include <mos6502/mos6502.h>

#include <stdbool.h>

#define PPU_OUTPUT_WIDTH 256
#define PPU_OUTPUT_HEIGHT 240
#define PPU_CLKDIVISOR 4

typedef enum ppu_vram_addr_inc {
	PPU_VRAM_ADDR_INC_1 = 0,
	PPU_VRAM_ADDR_INC_32 = 1,
} PACKED ppu_vram_addr_inc_t;

typedef enum ppu_chr_baseaddr {
	PPU_CHR_BASEADDR_0000 = 0,
	PPU_CHR_BASEADDR_1000 = 1,
} PACKED ppu_chr_baseaddr_t;

typedef enum ppu_nt_baseaddr {
	PPU_NT_BASEADDR_2000 = 0,
	PPU_NT_BASEADDR_2400 = 1,
	PPU_NT_BASEADDR_2800 = 2,
	PPU_NT_BASEADDR_2C00 = 3,
} PACKED ppu_nt_baseaddr_t;

typedef enum ppu_spritesize {
	PPU_SPRITESIZE_8x8 = 0,
	PPU_SPRITESIZE_8x16 = 1,
} ppu_spritesize_t;

typedef struct ppu_spriteattr {
	uint8_t palette : 2;
	uint8_t : 3;
	bool behind_bg : 1;
	bool horiz_flipped : 1;
	bool verti_flipped : 1;
} ppu_spriteattr_t;

typedef struct ppu_sprite {
	uint8_t ypos;
	uint8_t tile;
	ppu_spriteattr_t attr;
	uint8_t xpos;
} ppu_sprite_t;

typedef struct ppu_pixel {
	uint8_t palette_color : 6;
	bool emph_red         : 1;
	bool emph_green       : 1;
	bool emph_blue        : 1;
} PACKED ppu_pixel_t;

typedef struct ppu {
	struct membus * nonnull /*strong*/ bus;
	struct mos6502 * nonnull /*unowned*/ cpu;

	size_t framenum;
	uint64_t clk_countdown;

	size_t slnum;
	size_t dotnum;
	size_t overflow_dotnum;

	struct SDL_Window * nonnull /*owned*/ win;
	struct SDL_Renderer * nonnull /*owned*/ rend;
	struct SDL_Texture * nonnull /*owned*/ tex;

	uint8_t * nonnull /*unowned*/ texdata;
	size_t texpitch;

	// Mask register
	union {
		struct {
			bool greyscale_en   : 1;
			bool left_bg_en     : 1;
			bool left_sprite_en : 1;
			bool bg_en          : 1;
			bool sprite_en      : 1;
			bool emph_red       : 1;
			bool emph_green     : 1;
			bool emph_blue      : 1;
		};
		uint8_t mask;
	};

	// Various small registers & flags
	struct {
		ppu_vram_addr_inc_t vram_addr_inc      : 1;
		ppu_chr_baseaddr_t bg_chr_baseaddr     : 1;
		ppu_chr_baseaddr_t sprite_chr_baseaddr : 1;
		ppu_spritesize_t spritesize            : 1;

		bool nmi_en          : 1;
		bool vblank          : 1;
		bool sprite0_hit     : 1;
		bool sprite_overflow : 1;
		bool write_toggle    : 1;

		bool sprite0_hit_shouldset : 1;

		bool next_scanline_has_sprite0 : 1;
		bool scanline_has_sprite0      : 1;
	};

	union {
		struct {
			uint8_t coarse_xscroll        : 5;
			uint8_t coarse_yscroll        : 5;
			ppu_nt_baseaddr_t nt_baseaddr : 2;
			uint8_t fine_yscroll          : 3;
		} PACKED;
		uint16_t vram_addr;
	};
	union {
		struct {
			uint8_t tmp_coarse_xscroll        : 5;
			uint8_t tmp_coarse_yscroll        : 5;
			ppu_nt_baseaddr_t tmp_nt_baseaddr : 2;
			uint8_t tmp_fine_yscroll          : 3;
		} PACKED;
		uint16_t tmp_vram_addr;
	};

	uint8_t fine_xscroll;

	uint8_t bmp_latch[2], nt_latch, attr_latch;

	uint16_t bg_bmp_shiftregs[2];
	uint8_t bg_attr_shiftregs[2];
	uint8_t bg_palette;

	uint8_t sprite_bmp_shiftregs[8][2];
	ppu_spriteattr_t sprite_attrs[8];
	uint8_t sprite_xs[8];

	uint8_t eval_nsprites;
	ppu_sprite_t eval_sprites[8];

	uint8_t oam_addr;
	union {
		ppu_sprite_t sprites[64];
		uint8_t oam[sizeof(ppu_sprite_t) * 64];
	};

	uint8_t palette_mem[32];
	uint8_t palette_srgb[512][3];
} ppu_t;

// Allocates and initializes a new reference-counted PPU
ppu_t * nullable ppu_new (reset_manager_t * nonnull rm, mos6502_t * nonnull cpu, int scale);

// Creates the page mappings on the CPU's memory bus for the PPU registers
void ppu_map (ppu_t * nonnull ppu);
