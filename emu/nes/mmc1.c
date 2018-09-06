#include <rc.h>
#include <base.h>
#include <string.h>
#include <nes/mmc1.h>
#include <timekeeper.h>

static inline void
reset_shiftreg (mmc1_t * mmc1)
{
	// Leave a bit that'll let us know when 5 writes have completed
	mmc1->shiftreg = 0x20;
}

void
mmc1_reset (mmc1_t * mmc1)
{
	reset_shiftreg(mmc1);

	mmc1->reg0.prgrom_fixation = MMC1_LOW_PRGROM_FIXED;
	mmc1->reg0.prgrom_switching = MMC1_PRGROM_SWITCHING_16K;
	
	mmc1->reg3.banksel = 0xF;
	mmc1->reg3.wram_disabled = false;

	mmc1->last_cpu_cyclenum = UINT64_MAX;
}

void
mmc1_map_vram (mmc1_t * mmc1, membus_t * bus, memory_t * vram)
{
	switch (mmc1->reg0.mirroring) {
	case MMC1_MIRRORING_ONE_SCREEN_NT0:
		memory_map(vram, bus, 0x2000, 0x0400, 0x0000);
		memory_map(vram, bus, 0x2400, 0x0400, 0x0000);
		memory_map(vram, bus, 0x2800, 0x0400, 0x0000);
		memory_map(vram, bus, 0x2C00, 0x0400, 0x0000);
		break;
	case MMC1_MIRRORING_ONE_SCREEN_NT1:
		memory_map(vram, bus, 0x2000, 0x0400, 0x0400);
		memory_map(vram, bus, 0x2400, 0x0400, 0x0400);
		memory_map(vram, bus, 0x2800, 0x0400, 0x0400);
		memory_map(vram, bus, 0x2C00, 0x0400, 0x0400);
		break;
	case MMC1_MIRRORING_VERTICAL:
		memory_map(vram, bus, 0x2000, 0x0400, 0x0000);
		memory_map(vram, bus, 0x2400, 0x0400, 0x0400);
		memory_map(vram, bus, 0x2800, 0x0400, 0x0000);
		memory_map(vram, bus, 0x2C00, 0x0400, 0x0400);
		break;
	case MMC1_MIRRORING_HORIZONTAL:
		memory_map(vram, bus, 0x2000, 0x0400, 0x0000);
		memory_map(vram, bus, 0x2400, 0x0400, 0x0000);
		memory_map(vram, bus, 0x2800, 0x0400, 0x0400);
		memory_map(vram, bus, 0x2C00, 0x0400, 0x0400);
		break;
	}
}

void
mmc1_reg_write (mmc1_t * mmc1, size_t regnum, uint8_t val, uint64_t cpu_cyclenum)
{
	ASSERT(regnum < 4);

	// Ignore consecutive writes
	if (cpu_cyclenum - mmc1->last_cpu_cyclenum == 1) {
		goto end;
	}

	// Handle a "reset" byte
	if (val & 0x80) {
		reset_shiftreg(mmc1);
		return;
	}

	// Shift in the next bit. Note that we insert at the 6th bit, not the
	// 5th, because we use a sentinel LSB to track how many shifts have
	// occured.
	mmc1->shiftreg >>= 1;
	mmc1->shiftreg |= (val & 0x1) << 5;

	// Check if the sentinel has been shifted into the 1's position, in
	// which case 5 bits have been written in and a register needs to be
	// updated.
	if (mmc1->shiftreg & 0x1) {
		// Shhh evil memory reinterpretation.
		uint8_t * regs = (__typeof(regs))&mmc1->reg0;
		regs[regnum] = mmc1->shiftreg >> 1;

		reset_shiftreg(mmc1);
	}

end:
	mmc1->last_cpu_cyclenum = cpu_cyclenum;
	return;
}
