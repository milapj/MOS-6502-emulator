#include <rc.h>
#include <membus.h>
#include <mos6502/mos6502.h>

static void
deinit (mos6502_t * cpu)
{
	rc_release(cpu->bus);
	rc_release(cpu->tk);
}

mos6502_t *
mos6502_new (reset_manager_t * rm,
	     timekeeper_t * tk,
	     uint16_t paravirt_argc,
	     char ** paravirt_argv)
{
	mos6502_t * retval = NULL;

	mos6502_t * cpu = rc_alloc(sizeof(mos6502_t), deinit);

	// Temporary nullable handle
	membus_t * bus = membus_new(rm);
	if (!bus) {
		goto release_cpu;
	}
	cpu->bus = rc_retain(bus);

	cpu->tk = rc_retain(tk);
	cpu->paravirt_argc = paravirt_argc;
	cpu->paravirt_argv = paravirt_argv;

	retval = rc_retain(cpu);
	rc_release(bus);

release_cpu:
	rc_release(cpu);
	return retval;
}

void
mos6502_advance_clk (mos6502_t * cpu, size_t ncycles)
{
	timekeeper_advance_clk(cpu->tk, ncycles * MOS6502_CLKDIVISOR);
}

void
mos6502_raise_irq (mos6502_t * cpu)
{
	cpu->intr_status = INTR_IRQ;
}

void
mos6502_raise_nmi (mos6502_t * cpu)
{
	cpu->intr_status = INTR_NMI;
}

// See https://wiki.nesdev.com/w/index.php/CPU_power_up_state
// This simulates power-up state, as opposed to reset state
void
mos6502_reset (mos6502_t * cpu)
{
	cpu->pc = membus_read(cpu->bus, 0xfffc) | (uint16_t)(membus_read(cpu->bus, 0xfffd) << 8);
	cpu->a = 0;
	cpu->x = 0;
	cpu->y = 0;
	cpu->sp = 0xfd; // decrement stack pointer by 3
	cpu->p.val = 0x34; // interrupt disable on
	mos6502_advance_clk(cpu, 8); // https://www.pagetable.com/?p=410
}
