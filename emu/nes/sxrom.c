#include <base.h>
#include <memory.h>
#include <nes/mmc1.h>
#include <nes/sxrom.h>
#include <rc.h>

typedef struct sxrom {
	mmc1_t mmc1;

	mos6502_t * nonnull /*unowned*/ cpu;
	ppu_t * nonnull /*unowned*/ ppu;

	memory_t * nonnull /*strong*/ prgrom;
	memory_t * nonnull /*strong*/ chrom;
	memory_t * nullable /*strong*/ wram;
	memory_t * nonnull /*strong*/ vram;
} sxrom_t;

// Remaps the PRGROM, CHROM, and VRAM based on the mapping state of `cart`.
static inline void
remap (sxrom_t * cart)
{
	mmc1_map_vram(&cart->mmc1, cart->ppu->bus, cart->vram);

	switch (cart->mmc1.reg0.prgrom_switching) {
	case MMC1_PRGROM_SWITCHING_32K:
		memory_map(
			cart->prgrom,
			cart->cpu->bus,
			0x8000,
			0x8000,
			(0x4000 * (cart->mmc1.reg3.banksel & ~1)) % cart->prgrom->size);
		break;
	case MMC1_PRGROM_SWITCHING_16K:
		switch (cart->mmc1.reg0.prgrom_fixation) {
		case MMC1_LOW_PRGROM_FIXED:
			memory_map(
				cart->prgrom,
				cart->cpu->bus,
				0x8000,
				0x4000,
				0x0000);
			memory_map(
				cart->prgrom,
				cart->cpu->bus,
				0xC000,
				0x4000,
				(cart->mmc1.reg3.banksel * 0x4000) % cart->prgrom->size);
			break;
		case MMC1_HIGH_PRGROM_FIXED:
			memory_map(
				cart->prgrom,
				cart->cpu->bus,
				0x8000,
				0x4000,
				(cart->mmc1.reg3.banksel * 0x4000) % cart->prgrom->size);
			memory_map(
				cart->prgrom,
				cart->cpu->bus,
				0xC000,
				0x4000,
				cart->prgrom->size - 0x4000);
			break;
		}
		break;
	}

	switch (cart->mmc1.reg0.chr_switching) {
	case MMC1_CHR_SWITCHING_8K:
		memory_map(
			cart->chrom,
			cart->ppu->bus,
			0x0000,
			0x2000,
			((cart->mmc1.reg1 & ~1) * 0x1000) % cart->chrom->size);
		break;
	case MMC1_CHR_SWITCHING_4K:
		memory_map(
			cart->chrom,
			cart->ppu->bus,
			0x0000,
			0x1000,
			(cart->mmc1.reg1 * 0x1000) % cart->chrom->size);
		memory_map(
			cart->chrom,
			cart->ppu->bus,
			0x1000,
			0x1000,
			(cart->mmc1.reg2 * 0x1000) % cart->chrom->size);
		break;
	}

	// TODO wram_en?
	if (cart->wram) {
		memory_map((memory_t * nonnull)cart->wram, cart->cpu->bus, 0x6000, 0x2000, 0x0000);
	}
}

static void
deinit (sxrom_t * cart)
{
	rc_release(cart->prgrom);
	rc_release(cart->chrom);
	rc_release(cart->vram);

	if (cart->wram) {
		rc_release((memory_t * nonnull)cart->wram);
	}
}

static void
reset (sxrom_t * cart)
{
	mmc1_reset(&cart->mmc1);
	remap(cart);
}

// Handles a write into the PRGROM/serial IO region
static void
reg_write (sxrom_t * cart, size_t addr, uint8_t val)
{
	mmc1_reg_write(&cart->mmc1, addr / 0x2000, val, (cart->cpu->tk->clk_cyclenum / MOS6502_CLKDIVISOR));
	remap(cart);
}

int
sxrom_setup (rominfo_t * info)
{
	if (info->wram && info->wram->size != 0x2000) {
		ERROR_PRINT("ROM has an invalid WRAM configuration:");
		ERROR_PRINT("  Expected 8192 bytes of WRAM, but ROM specifies %zu", info->wram->size);
		return -1;
	}

	if (!info->prgrom || info->prgrom->size % 0x8000) {
		ERROR_PRINT("ROM has an invalid PRGROM configuration:");
		if (info->prgrom) {
			ERROR_PRINT("  ROM's PRGROM size (%zu) is not a multiple of 32768", info->prgrom->size);
		}
		else {
			ERROR_PRINT("  ROM is missing PRGROM");
		}
		return -1;
	}

	if (!info->chrom || info->chrom->size % 0x2000) {
		ERROR_PRINT("ROM has an invalid CHROM configuration");
		if (info->chrom) {
			ERROR_PRINT("  ROM's CHROM size (%zu) is not a multiple of 8192", info->chrom->size);
		}
		else {
			ERROR_PRINT("  ROM is missing CHROM");
		}
		return -1;
	}

	if (!info->vram || info->vram->size != 0x0800) {
		ERROR_PRINT("ROM has an invalid VRAM configuration");
		if (info->vram) {
			ERROR_PRINT("  Expected 2048 bytes of VRAM, but ROM specifies %zu", info->vram->size);
		}
		else {
			ERROR_PRINT("  ROM specifies no VRAM wtf");
		}
		return -1;
	}

	if (info->chram) {
		ERROR_PRINT("ROM has an invalid CHRAM configuration");
		ERROR_PRINT("  The SxROM implementation does not yet support CHRAM, but the ROM requires it");
		return -1;
	}

	sxrom_t * cart = rc_alloc(sizeof(sxrom_t), deinit);
	reset_manager_add_device(info->rm, cart, reset);

	cart->cpu = info->cpu;
	cart->ppu = info->ppu;
	cart->prgrom = rc_retain((memory_t * nonnull)info->prgrom);
	cart->chrom = rc_retain((memory_t * nonnull)info->chrom);
	cart->vram = rc_retain((memory_t * nonnull)info->vram);

	if (info->wram) {
		cart->wram = rc_retain((memory_t * nonnull)info->wram);
	}

	for (size_t i = 0; i < 0x80; i++) {
		membus_set_write_handler(info->cpu->bus, 0x80 + i, cart, i << 8, reg_write);
	}

	rc_release(cart);
	return 0;
}
