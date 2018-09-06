#include <base.h>
#include <nes/nrom.h>

int
nrom_setup (rominfo_t * info)
{
	if (info->wram && info->wram->size > 0x2000) {
		ERROR_PRINT("ROM has invalid WRAM configuration");
		return -1;
	}

	if (!info->prgrom || info->prgrom->size > 0x8000) {
		ERROR_PRINT("ROM has invalid PRGROM configuration");
		return -1;
	}

	if (!info->chrom || info->chrom->size != 0x2000) {
		ERROR_PRINT("ROM has invalid CHROM configuration");
		return -1;
	}

	if (!info->vram || info->vram->size != 0x0800) {
		ERROR_PRINT("ROM has invalid VRAM configuration");
		return -1;
	}

	if (info->chram) {
		ERROR_PRINT("ROM has invalid CHRAM configuration");
		return -1;
	}

	if (info->wram) {
		memory_map_mirroring((memory_t * nonnull)info->wram,
				     info->cpu->bus,
				     0x6000,
				     (uint16_t)info->wram->size,
				     0x0000, 0x2000 / info->wram->size);
	}
	memory_map_mirroring((memory_t * nonnull)info->prgrom,
			     info->cpu->bus,
			     0x8000,
			     (uint16_t)info->prgrom->size,
			     0x0000,
			     0x8000 / info->prgrom->size);

	memory_map((memory_t * nonnull)info->chrom, info->ppu->bus, 0x0000, (uint16_t)info->chrom->size, 0x0000);

	switch (info->mirroring) {
	case INES_MIRRORING_HORIZONTAL:
		memory_map_mirroring((memory_t * nonnull)info->vram,
				     info->ppu->bus,
				     0x2000,
				     (uint16_t)info->vram->size/2,
				     0,
				     2);

		memory_map_mirroring((memory_t * nonnull)info->vram,
				     info->ppu->bus,
				     0x2800,
				     (uint16_t)info->vram->size/2,
				     info->vram->size/2,
				     2);
		break;
	case INES_MIRRORING_VERTICAL:
		memory_map((memory_t * nonnull)info->vram,
			   info->ppu->bus,
			   0x2000,
			   (uint16_t)info->vram->size/2,
			   0);

		memory_map((memory_t * nonnull)info->vram,
			   info->ppu->bus,
			   0x2400,
			   (uint16_t)info->vram->size/2,
			   info->vram->size/2);

		memory_map((memory_t * nonnull)info->vram,
			   info->ppu->bus,
			   0x2800,
			   (uint16_t)info->vram->size/2,
			   0);

		memory_map((memory_t * nonnull)info->vram,
			   info->ppu->bus,
			   0x2C00,
			   (uint16_t)info->vram->size/2,
			   info->vram->size/2);
		break;
	}

	return 0;
}
