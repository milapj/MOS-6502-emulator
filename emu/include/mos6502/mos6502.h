#pragma once

#include <base.h>
#include <timekeeper.h>

#include <stdint.h>
#include <stdbool.h>

#define MOS6502_CLKDIVISOR 12

// The 6502's status register
typedef union stat_reg {
	uint8_t val;
	struct {
		bool c : 1; // carry flag
		bool z : 1; // zero flag
		bool i : 1; // interrupt disable
		bool d : 1; // decimal mode
		bool b : 1; // break command
		bool : 1;   // unused
		bool v : 1; // overflow flag
		bool n : 1; // negative flag
	};
} stat_reg_t;

// Possible kinds of interrupts
typedef enum intr {
	INTR_NONE = 0,
	INTR_IRQ = 1,
	INTR_NMI = 2,
} intr_t;

// Possible addressing modes
typedef enum addr_mode {
	MODE_NONE = 0,    // No address calculation in decode
	MODE_ABS = 1,     // Absolute
	MODE_ABSX = 2,    // Absolute, indexed by X
	MODE_ABSY = 3,    // Absolute, indexed by Y
	MODE_ACC = 4,     // Accumulator
	MODE_IMM = 5,     // Immediate
	MODE_IMPL = 6,    // Implied
	MODE_IDXIND = 7,  // Indexed-indirect
	MODE_IND = 8,     // Indirect
	MODE_INDIDX = 9,  // Indirect-indexed
	MODE_REL = 10,    // Relative
	MODE_ZEROP = 11,  // Zero-page
	MODE_ZEROPX = 12, // Zero-page, indexed by X
	MODE_ZEROPY = 13, // Zero-page, indexed by Y
} addr_mode_t;

// An encapsulation of an emulated MOS6502
typedef struct mos6502 {
	struct membus * nonnull /*strong*/ bus;
	struct timekeeper * nonnull /*strong*/ tk;

	// Registers
	uint16_t pc;  // program counter
	uint8_t sp;   // stack pointer
	uint8_t a;    // accumulator
	uint8_t x;    // GPR 1
	uint8_t y;    // GPR 2
	stat_reg_t p; // processor status word

	// Whether or not an interrupt has been raised and needs to be
	// processed, and if so, what kind
	intr_t intr_status;

#if defined(REFERENCE) && !defined(DISABLE_CYCLECHECK)
	// LCM: The place to record the number of CPU cycles that elapsed
	// during instruction execution due to branch delays. This is
	// subsequently used in clock error detection and correction. We really
	// should have a better way to propagate this information up the stack,
	// but this works.
	uint64_t last_branch_delay;

	// LCM: The place to record the number of CPU cycles that elapsed
	// during a "takeover" of the CPU by a memory-mapped device. Right now,
	// this occurs when writing to the OAMDMA register during NES
	// emulation, which initiates a DMA sequence that suspends normal
	// execution. This is subsequently used in clock error detection and
	// correction.
	uint64_t last_takeover_delay;
#endif

	// Paravirtualization state
	uint16_t paravirt_argc;
	char * nullable * nonnull /*unowned*/ paravirt_argv;
} mos6502_t;

// The information passed to an opcode handler
typedef struct decode_info {
	mos6502_t * nonnull cpu;
	addr_mode_t mode;
	uint8_t opcode;
	uint16_t addr;
} decode_info_t;

typedef enum mos6502_step_result {
	MOS6502_STEP_RESULT_SUCCESS,
	MOS6502_STEP_RESULT_ILLEGAL_INSTRUCTION,
	MOS6502_STEP_RESULT_VMBREAK,
	MOS6502_STEP_RESULT_UNHANDLED_VMCALL,
} PACKED mos6502_step_result_t;

// Creates a new reference-counted CPU object
mos6502_t * nullable mos6502_new (reset_manager_t * rm,
				  timekeeper_t * nonnull tk,
				  uint16_t paravirt_argc,
				  char * nullable * nonnull paravirt_argv);

// Fills `buffer` with as many as `buflen` characters of the assembly
// representation of the instruction at `addr`
size_t mos6502_instr_repr (mos6502_t * nonnull cpu,
			   uint16_t addr,
			   char * nonnull buffer,
			   size_t buflen);

// Advances the CPU by one instruction, driving the timekeeper appropriately
mos6502_step_result_t mos6502_step (mos6502_t * nonnull cpu);

// Advances the global clock by `cycles` CPU cycles
void mos6502_advance_clk (mos6502_t * nonnull cpu, size_t cycles);

// Raises an IRQ
void mos6502_raise_irq (mos6502_t * nonnull cpu);

// Raises an NMI
void mos6502_raise_nmi (mos6502_t * nonnull cpu);

// Resets the CPU
void mos6502_reset (mos6502_t * nonnull cpu);
