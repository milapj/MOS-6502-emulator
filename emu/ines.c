#include <rc.h>
#include <base.h>
#include <ines.h>
#include <fileio.h>
#include <nes/nrom.h>
#include <nes/sxrom.h>
#include <nes/io_reg.h>

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

typedef enum tv_type {
	TV_NTSC = 0,
	TV_PAL = 1,
} PACKED tv_type_t;

typedef struct flags6 {
	ines_mirroring_t mirroring : 1;
	bool wram_present : 1;
	bool trainer_present : 1;
	bool four_screen_vram : 1;
	uint8_t mapper_nib_low : 4;
} flags6_t;

typedef struct flags7 {
	bool vs_unisystem : 1;
	bool playchoice_10 : 1;
	uint8_t version : 2;
	uint8_t mapper_nib_high : 4;
} flags7_t;

typedef struct ines1_flags9 {
	tv_type_t tv_type : 1;
	uint8_t reserved : 7;
} ines1_flags9_t;

typedef struct ines2_flags8 {
	uint8_t submapper : 4;
	uint8_t mapper_nib_higher : 4;
} ines2_flags8_t;

typedef struct ines2_flags9 {
	uint8_t chrom_size_nib : 4;
	uint8_t prgrom_size_nib : 4;
} ines2_flags9_t;

typedef struct ines2_flags10 {
	uint8_t nbb_wram_size : 4;
	uint8_t bb_wram_size : 4;
} ines2_flags10_t;

typedef struct ines2_flags11 {
	uint8_t nbb_chram_size : 4;
	uint8_t bb_chram_size : 4;
} ines2_flags11_t;

typedef struct ines2_flags12 {
	tv_type_t tv_type : 1;
	bool universal : 1;
	uint8_t reserved : 6;
} ines2_flags12_t;

// The common components of both ines1 and ines2 headers
typedef struct common_header {
	uint8_t prgrom_size;
	uint8_t chrom_size;
	flags6_t flags6;
	flags7_t flags7;
} common_header_t;

// The header components unique to ines1
typedef struct ines1_header {
	uint8_t wram_size;
	ines1_flags9_t flags9;
	uint8_t ignored[6];
} ines1_header_t;

// The header components unique to ines2
typedef struct ines2_header {
	ines2_flags8_t flags8;
	ines2_flags9_t flags9;
	ines2_flags10_t flags10;
	ines2_flags11_t flags11;
	ines2_flags12_t flags12;
	uint8_t ignored[3];
} ines2_header_t;

// Decodes an ines-encoded RAM size
static inline size_t
decode_ram_size (size_t encoded_size)
{
	if (encoded_size == 0) {
		return 0;
	} else {
		return 64u << encoded_size;
	}
}

// Sets up the common, mapper-independent parts of the NES memory map, and
// returns a newly-created PPU instance
static inline ppu_t * nullable
setup_common (reset_manager_t * nonnull rm,
	      mos6502_t * nonnull cpu,
	      const char * nonnull palette_path,
	      const char * cscheme_path,
	      int scale)
{
	ppu_t * retval = NULL;

	memory_t * ram = memory_new(rm, 0x0800, true);
	if (!ram) {
		goto ret;
	}
	memory_map(ram, cpu->bus, 0x0000, (uint16_t)ram->size, 0x0000);

	if (io_reg_setup(rm, cpu, cscheme_path)) {
		goto release_ram;
	}

	ppu_t * ppu = ppu_new(rm, cpu, scale);
	if (!ppu) {
		goto release_ram;
	}
	ppu_map(ppu);

	FILE * f = try_fopen(palette_path, "rb");
	if (!f) {
		goto release_ppu;
	}

	if (try_fread(f, palette_path, ppu->palette_srgb, sizeof(ppu->palette_srgb))) {
		goto close_f;
	}

	retval = rc_retain(ppu);

close_f:
	fclose(f);
release_ppu:
	rc_release(ppu);
release_ram:
	rc_release(ram);
ret:
	return retval;
}

int
inesrom_load (FILE * f,
	      const char * path,
	      reset_manager_t * rm,
	      mos6502_t * cpu,
	      const char * palette_path,
	      const char * cscheme_path,
	      int scale)
{
	int retcode = -1;

	uint8_t header[sizeof(common_header_t) + sizeof(ines1_header_t)];
	if (try_fread(f, path, header, sizeof(header))) {
		goto ret;
	}

	common_header_t * common = (common_header_t *)header;
	ines1_header_t * ines1 = (ines1_header_t *)(common + 1);
	ines2_header_t * ines2 = (ines2_header_t *)(common + 1);

	uint8_t version = common->flags7.version;

	rominfo_t info;
	memset(&info, 0x00, sizeof(info));

	info.rm = rm;
	info.cpu = cpu;
	info.mirroring = common->flags6.mirroring;

	size_t mapper = common->flags6.mapper_nib_low;
	mapper += (size_t)common->flags7.mapper_nib_high << 4;
	if (version == 2) {
		mapper += (size_t)ines2->flags8.mapper_nib_higher << 8u;
	}

	size_t prgrom_size = 16384 * common->prgrom_size;
	size_t chrom_size = 8192 * common->chrom_size;
	if (version == 2) {
		prgrom_size += (size_t)(ines2->flags9.prgrom_size_nib << 8u) * 16384u;
		chrom_size += (size_t)(ines2->flags9.chrom_size_nib << 8u) * 8192u;
	}

	size_t wram_size, chram_size;
	if (version == 2) {
		wram_size = decode_ram_size(ines2->flags10.nbb_wram_size) + decode_ram_size(ines2->flags10.bb_wram_size);
		chram_size = decode_ram_size(ines2->flags11.nbb_chram_size) + decode_ram_size(ines2->flags11.bb_chram_size);
	} else {
		if (common->flags6.wram_present) {
			if (ines1->wram_size) {
				wram_size = 8192 * ines1->wram_size;
			} else {
				wram_size = 8192;
			}
		} else {
			wram_size = 0;
		}

		if (chrom_size == 0) {
			chram_size = 8192;
		} else {
			chram_size = 0;
		}
	}

	if (common->flags6.trainer_present) {
		ERROR_PRINT("%s requires trainer support", path);
		retcode = -1;
		goto ret;
	}

	if (common->flags7.vs_unisystem) {
		ERROR_PRINT("%s requires Vs. Unisystem support", path);
		retcode = -1;
		goto ret;
	}

	if (common->flags7.playchoice_10) {
		ERROR_PRINT("%s requires Playchoice 10 support", path);
		retcode = -1;
		goto ret;
	}

	if ((version == 2 && !ines2->flags12.universal && ines2->flags12.tv_type != TV_NTSC) ||
	    (version != 2 && ines1->flags9.tv_type != TV_NTSC)) {
		WARNING_PRINT("%s expects a PAL system", path);
	}

	// Temporary nullable binding
	ppu_t * ppu = setup_common(rm, cpu, palette_path, cscheme_path, scale);
	if (!ppu) {
		goto ret;
	}
	info.ppu = ppu;

	if (wram_size) {
		info.wram = memory_new(rm, wram_size, true);
		if (!info.wram) {
			goto release_ppu;
		}
	}

	if (prgrom_size) {
		info.prgrom = memory_new(rm, prgrom_size, false);
		if (!info.prgrom) {
			goto release_wram;
		}
	}

	if (chrom_size) {
		info.chrom = memory_new(rm, chrom_size, false);
		if (!info.chrom) {
			goto release_prgrom;
		}
	}

	if (chram_size) {
		info.chram = memory_new(rm, chram_size, true);
		if (!info.chram) {
			goto release_chrom;
		}
	}

	info.vram = memory_new(rm, 0x0800, true);
	if (!info.vram) {
		goto release_chram;
	}

	if (info.prgrom &&
	    try_fread(f, path, info.prgrom->bytes, info.prgrom->size)) {
		goto release_vram;
	}

	if (info.chrom &&
	    try_fread(f, path, info.chrom->bytes, info.chrom->size)) {
		goto release_vram;
	}

	switch (mapper) {
	case 0:
		retcode = nrom_setup(&info);
		break;
	case 1:
		retcode = sxrom_setup(&info);
		break;
	default:
		ERROR_PRINT("%s requires mapper #%zu", path, mapper);
		INFO_PRINT("  Only mappers #0 and #1 are supported");
		break;
	}

release_vram:
	rc_release((memory_t * nonnull)info.vram);
release_chram:
	if (info.chram) {
		rc_release((memory_t * nonnull)info.chram);
	}
release_chrom:
	if (info.chrom) {
		rc_release((memory_t * nonnull)info.chrom);
	}
release_prgrom:
	if (info.prgrom) {
		rc_release((memory_t * nonnull)info.prgrom);
	}
release_wram:
	if (info.wram) {
		rc_release((memory_t * nonnull)info.wram);
	}
release_ppu:
	rc_release(info.ppu);
ret:
	return retcode;
}
