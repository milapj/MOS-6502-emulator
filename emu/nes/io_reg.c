#include <rc.h>
#include <fileio.h>
#include <nes/io_reg.h>

#include <errno.h>

static inline void
set_strobe (io_reg_t * io, bool val)
{
	// The shift registers in each controller are updated on the falling
	// edge of the strobe signal.
	if (io->controller_strobe && !val) {
		// Sync up with real time.
		timekeeper_sync(io->cpu->tk);

		// Make sure SDL updates the keyboard state array before we
		// read from it.
		SDL_PumpEvents();

		// Update the shift registers according to the current key-press state.
		const uint8_t * kbstate = SDL_GetKeyboardState(NULL);
		for (size_t j = 0; j < 2; j++) {
			for (size_t i = 0; i < CONTROLLER_NBUTTONS; i++) {
				io->controller_shiftregs[j] |= (uint8_t)(kbstate[io->controller_mappings[i][j]] << i);
			}
		}
	}
	io->controller_strobe = val;
}

static uint8_t
read (io_reg_t * io, uint16_t addr, uint8_t * lanemask)
{
	switch (addr) {
	case 0x16:
	case 0x17:
		*lanemask = 0x1F; // Upper three lanes are undriven.

		uint8_t bit;

		// If the strobe is high, then a read will give the instantaneous
		// A-button state.
		if (io->controller_strobe) {
			timekeeper_sync(io->cpu->tk);
			SDL_PumpEvents();
			const uint8_t * kbstate = SDL_GetKeyboardState(NULL);
			bit = kbstate[io->controller_mappings[addr - 0x16][CONTROLLER_BUTTON_A]];
		}
		else {
			bit = io->controller_shiftregs[addr - 0x16] & 0x01;
			io->controller_shiftregs[addr - 0x16] >>= 1;
		}

		return bit;

	default:
		*lanemask = 0x00;
		return 0x00;
	}
}

static void
write (io_reg_t * io, uint16_t addr, uint8_t val)
{
	switch (addr) {
	case 0x16:
		set_strobe(io, val & 0x01);
		return;
	case 0x14: { // OAMDMA
		uint16_t readaddr = (uint16_t)(val << 8);

		// dummy read cycle
		mos6502_advance_clk(io->cpu, 1);

#if defined(REFERENCE) && !defined(DISABLE_CYCLECHECK)
		io->cpu->last_takeover_delay++;
#endif

		if ((io->cpu->tk->clk_cyclenum / MOS6502_CLKDIVISOR) % 2) {
			// Dummy read cycle (TODO of what?) if on an odd CPU cycle
			mos6502_advance_clk(io->cpu, 1);

#if defined(REFERENCE) && !defined(DISABLE_CYCLECHECK)
			io->cpu->last_takeover_delay++;
#endif
		}

		for (uint16_t i = 0; i < 256; i++) {
			uint8_t byte = membus_read(io->cpu->bus, readaddr + i);
			mos6502_advance_clk(io->cpu, 1);
			membus_write(io->cpu->bus, 0x2004, byte);
			mos6502_advance_clk(io->cpu, 1);

#if defined(REFERENCE) && !defined(DISABLE_CYCLECHECK)
			io->cpu->last_takeover_delay += 2;
#endif
		}

		return;
	}
	default:
		return;
	}
}

static void
reset (io_reg_t * io)
{
	memset(io->controller_shiftregs, 0, sizeof(io->controller_shiftregs));
	io->controller_strobe = false;
}

io_reg_t *
io_reg_new (reset_manager_t * rm, mos6502_t * cpu, const char * cscheme_path)
{
	io_reg_t * retval = NULL;

	io_reg_t * io = rc_alloc(sizeof(io_reg_t), NULL);
	reset_manager_add_device(rm, io, reset);
	io->cpu = cpu;

	FILE * f = try_fopen(cscheme_path, "r");
	if (!f) {
		goto release_io;
	}

	for (size_t playernum = 0; playernum < 2; playernum++) {
		for (size_t buttonnum = 0; buttonnum < CONTROLLER_NBUTTONS; buttonnum++) {
			char line_buffer[128];
			if (!fgets(line_buffer, sizeof(line_buffer), f)) {
				if (feof(f)) {
					ERROR_PRINT("Unexpected end of file in %s", cscheme_path);
					goto close_f;
				}
				else {
					ERROR_PRINT("Error reading from %s: %s", cscheme_path, strerror(errno));
					goto close_f;
				}
			}
			line_buffer[strlen(line_buffer) - 1] = 0; // Get rid of newline character
			io->controller_mappings[buttonnum][playernum] = SDL_GetScancodeFromName(line_buffer);
			if (io->controller_mappings[buttonnum][playernum] == SDL_SCANCODE_UNKNOWN) {
				ERROR_PRINT("Error parsing %s: unknown key '%s'", cscheme_path, line_buffer);
				goto close_f;
			}
		}
	}

	retval = rc_retain(io);

close_f:
	fclose(f);
release_io:
	rc_release(io);
	return retval;
}

int
io_reg_setup (reset_manager_t * rm, mos6502_t * cpu, const char * cscheme_path)
{
	int retcode = -1;

	io_reg_t * io = io_reg_new(rm, cpu, cscheme_path);
	if (!io) {
		ERROR_PRINT("Couldn't create an IO register device");
		goto ret;
	}

	membus_set_read_handler(cpu->bus, 0x40, io, 0, read);
	membus_set_write_handler(cpu->bus, 0x40, io, 0, write);

	retcode = 0;
	rc_release(io);

ret:
	return retcode;
}
