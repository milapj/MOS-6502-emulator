#include <shell.h>
#include <membus.h>
#include <timekeeper.h>
#include <mos6502/mos6502.h>

#include <SDL2/SDL.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdnoreturn.h>

#include <signal.h>

#include <readline/history.h>
#include <readline/readline.h>

// Returns the next whitespace-delimited token in the string pointed to by
// `line_ptr`. A token which starts with the first character in `*line_ptr`
// still counts. This destructively modifies `*line_ptr` by writing a NULL
// character into the token boundry. `*line_ptr` is modified to point to the
// character immediately after that which ended the token, or the end of the
// string if it's reached. If `*line_ptr` contains only whitespace or is empty,
// an empty string is returned. The returned string aliases contents from
// `*line_ptr`, and therefore has the same lifetime.
static inline char *
next_token (char ** line_ptr)
{
	while (isspace(**line_ptr)) {
		(*line_ptr)++;
	}

	char * token_start = *line_ptr;

	bool done = false;
	while (!isspace(**line_ptr) && !(done = !**line_ptr)) {
		(*line_ptr)++;
	}

	if (!done) {
		**line_ptr = 0;
		(*line_ptr)++;
	}

	return token_start;
}

// Tries to parse the next whitespace-delimited token in `*line_ptr` as an
// unsigned hexadecimal integer. If successful, the parsed value is stored in
// `result`, and `NULL` is returned. Otherwise, the contents of `result` are
// undefined, and the token which failed to parse is returned.
static inline char *
next_hex (char ** line_ptr, size_t * result)
{
	char * original_token = next_token(line_ptr);

	if (!*original_token) {
		return original_token;
	}

	char * remaining_token = original_token;
	*result = 0;
	goto skip_shift;

	do {
		*result *= 16;
	skip_shift:
		if (*remaining_token > 47 && *remaining_token < 58) {
			*result += (size_t)(*remaining_token - '0');
		}
		else if (*remaining_token > 64 && *remaining_token < 71) {
			*result += (size_t)(*remaining_token - 'A' + 10);
		}
		else if (*remaining_token > 96 && *remaining_token < 103) {
			*result += (size_t)(*remaining_token - 'a' + 10);
		}
		else {
			return original_token;
		}
		remaining_token++;
	} while (*remaining_token);

	return NULL;
}

// Tries to parse the next whitespace-delimited token in `*line_ptr` as an
// unsigned hexadecimal integer. If successful, the parsed value is stored in
// `result`, and `NULL` is returned. Otherwise, the contents of `result` are
// undefined, and the token which failed to parse is returned.
static inline char *
next_dec (char ** line_ptr, size_t * result)
{
	char * original_token = next_token(line_ptr);

	if (!*original_token) {
		return original_token;
	}

	char * remaining_token = original_token;
	*result = 0;
	goto skip_shift;

	do {
		*result *= 10;
	skip_shift:
		if (*remaining_token > 47 && *remaining_token < 58) {
			*result += (size_t)(*remaining_token - '0');
		}
		else {
			return original_token;
		}
		remaining_token++;
	} while (*remaining_token);

	return NULL;
}

// Similar to `next_hex`, except it `ERROR_PRINT`s on failure and returns a
// non-zero error code.
static inline int
try_next_hex (char ** line_ptr, size_t * result)
{
	char * token = next_hex(line_ptr, result);
	if (token) {
		ERROR_PRINT("  '%s' is not a valid positive hexadecimal integer", token);
		return -1;
	}
	return 0;
}

// Similar to `next_dec`, except it `ERROR_PRINT`s on failure and returns a
// non-zero error code.
static inline int
try_next_dec (char ** line_ptr, size_t * result)
{
	char * token = next_dec(line_ptr, result);
	if (token) {
		ERROR_PRINT("  '%s' is not a valid positive decimal integer", token);
		return -1;
	}
	return 0;
}

// Checked during CPU stepping to abort early on SIGINT
static bool sigint_received;

// break point level 2
static uint8_t * bptl2[256];

#define BP_L2_IDX(x) (((x) >> 8) & 0xFF)
#define BP_L1_IDX(x) (((x)&0xFF))
#define BP_PRESENT(x) ((x)&0x1)
#define BP_SET_PRESENT(x) ((x) | 0x1)
#define BP_SET_NOT_PRESENT(x) ((x)&0xFE)

static int
insert_bp (uint16_t bp_addr)
{
	uint8_t bpt_idx = BP_L2_IDX(bp_addr);
	uint8_t * bpt = NULL;
	uint8_t bpte;

	// no bpt yet
	if (!bptl2[bpt_idx]) {
		// allocate new bpt
		bptl2[bpt_idx] = malloc(256);
		memset(bptl2[bpt_idx], 0, 256);
	}

	bpt = bptl2[bpt_idx];
	bpte = bpt[BP_L1_IDX(bp_addr)];

	if (BP_PRESENT(bpte)) {
		ERROR_PRINT("  Breakpoint at $%04X already exists", bp_addr);
		return -1;
	}
	else {
		bpt[BP_L1_IDX(bp_addr)] = BP_SET_PRESENT(bpte);
	}

	return 0;
}

static void
bp_list (void)
{
	int c = 0;
	printf("Breakpoint List:\n");
	for (size_t i = 0; i < 256; i++) {
		if (bptl2[i]) {
			uint8_t * bpt = bptl2[i];
			for (size_t j = 0; j < 256; j++) {
				uint8_t bpte = bpt[j];
				if (BP_PRESENT(bpte)) {
					INFO_PRINT("  %d: $%04x", c++, (uint16_t)((i << 8) | j));
				}
			}
		}
	}
}

static bool
is_valid_bp (uint16_t bp_addr)
{
	uint8_t bpt_idx = BP_L2_IDX(bp_addr);
	uint8_t * bpt = NULL;
	uint8_t bpte;

	if (!bptl2[bpt_idx]) {
		return 0;
	}

	bpt = bptl2[bpt_idx];
	bpte = bpt[BP_L1_IDX(bp_addr)];

	return BP_PRESENT(bpte);
}

static int
remove_bp (uint16_t bp_addr)
{
	uint8_t bpt_idx = BP_L2_IDX(bp_addr);
	uint8_t * bpt = NULL;
	uint8_t bpte;

	if (!bptl2[bpt_idx]) {
		return -1;
	}

	bpt = bptl2[bpt_idx];
	bpte = bpt[BP_L1_IDX(bp_addr)];

	if (BP_PRESENT(bpte)) {
		bpt[BP_L1_IDX(bp_addr)] = BP_SET_NOT_PRESENT(bpte);
		return 0;
	}

	return -1;
}

// Prints a helpful message (for when the PC changes) that indicates the new PC
// and the corresponding instruction
static void
print_pc_update (mos6502_t * cpu)
{
	char buffer[32];
	mos6502_instr_repr(cpu, cpu->pc, buffer, sizeof(buffer));
	INFO_PRINT("  PC now at $%04x: %s", cpu->pc, buffer);
}

static int
check_step_result (mos6502_t * cpu, mos6502_step_result_t step_result)
{
	switch (step_result) {
	case MOS6502_STEP_RESULT_SUCCESS:
		return 0;
	case MOS6502_STEP_RESULT_UNHANDLED_VMCALL:
		INFO_PRINT("  Breaking due to unhandled VMCALL");
		break;
	case MOS6502_STEP_RESULT_ILLEGAL_INSTRUCTION:
		ERROR_PRINT("  Illegal instruction");
		break;
	case MOS6502_STEP_RESULT_VMBREAK:
		INFO_PRINT("  VMCALL breakpoint reached");
		break;
	default:
		UNREACHABLE();
	}

	return -1;
}

#define GET_HEX_ADDR(id)                                           \
	if (try_next_hex(&args, &id)) {                            \
		return -1;                                         \
	}                                                          \
	if (id > UINT16_MAX) {                                     \
		ERROR_PRINT("  Address $%zx is out of range", id); \
		return 0;                                          \
	}

static void print_usage(void);
static int
cmd_help (mos6502_t * cpu, char * args)
{
	print_usage();
	return 0;
}

static int
cmd_step (mos6502_t * cpu, char * args)
{
	size_t n;
	if (*args) {
		if (try_next_dec(&args, &n)) {
			return -1;
		}
	}
	else {
		n = 1;
	}

	bool bp_hit = false;
	mos6502_step_result_t step_result = MOS6502_STEP_RESULT_SUCCESS;
	timekeeper_resume(cpu->tk);
	for (; n && !(bp_hit = is_valid_bp(cpu->pc)) && !step_result && !sigint_received; n--) {
		step_result = mos6502_step(cpu);
	}
	timekeeper_pause(cpu->tk);

	check_step_result(cpu, step_result);

	if (bp_hit) {
		INFO_PRINT("Breakpoint at $%04x reached", cpu->pc);
		remove_bp(cpu->pc);
	}

	print_pc_update(cpu);
	return 0;
}

static int
cmd_jump (mos6502_t * cpu, char * args)
{
	size_t addr;
	GET_HEX_ADDR(addr);

	cpu->pc = (uint16_t)addr;
	print_pc_update(cpu);
	return 0;
}

static int
cmd_regs (mos6502_t * cpu, char * args)
{
	INFO_PRINT("  PC -> 0x%04x", cpu->pc);
	INFO_PRINT("  SP -> 0x%02x", cpu->sp);
	INFO_PRINT("   A -> 0x%02x", cpu->a);
	INFO_PRINT("   X -> 0x%02x", cpu->x);
	INFO_PRINT("   Y -> 0x%02x", cpu->y);
	INFO_PRINT("   P -> 0x%02x", cpu->p.val);
	return 0;
}

static int
cmd_peek (mos6502_t * cpu, char * args)
{
	size_t addr;
	GET_HEX_ADDR(addr);

	INFO_PRINT("  $%04x: %02x", (uint16_t)addr, membus_read(cpu->bus, (uint16_t)addr));
	return 0;
}

static int
cmd_poke (mos6502_t * cpu, char * args)
{
	size_t addr;
	GET_HEX_ADDR(addr);

	size_t val;
	if (try_next_hex(&args, &val)) {
		return -1;
	}
	if (val > UINT8_MAX) {
		ERROR_PRINT("  Byte value $%zx is out of range", val);
	}

	membus_write(cpu->bus, (uint16_t)addr, (uint8_t)val);
	return 0;
}

static int
cmd_dumpmem (mos6502_t * cpu, char * args)
{
	size_t addr;
	GET_HEX_ADDR(addr);

	size_t count = 0;
	if (try_next_dec(&args, &count)) {
		return -1;
	}

	for (uint16_t n = (uint16_t)((count + 3) / 4); n; n--, addr += 4) {
		INFO_PRINT("  $%04x: %02x %02x %02x %02x",
			   (uint16_t)addr,
			   membus_read(cpu->bus, (uint16_t)addr),
			   membus_read(cpu->bus, (uint16_t)addr + 1),
			   membus_read(cpu->bus, (uint16_t)addr + 2),
			   membus_read(cpu->bus, (uint16_t)addr + 3));
	}

	return 0;
}

static int
cmd_irq (mos6502_t * cpu, char * args)
{
	mos6502_raise_irq(cpu);
	return 0;
}

static int
cmd_nmi (mos6502_t * cpu, char * args)
{
	mos6502_raise_nmi(cpu);
	return 0;
}

static int
cmd_print_instr (mos6502_t * cpu, char * args)
{
	char buffer[32];
	mos6502_instr_repr(cpu, cpu->pc, buffer, sizeof(buffer));
	INFO_PRINT("  $%04x: %s", cpu->pc, buffer);
	return 0;
}

static int
cmd_cont (mos6502_t * cpu, char * args)
{
	bool hit_bp = false;
	mos6502_step_result_t step_result = MOS6502_STEP_RESULT_SUCCESS;
	timekeeper_resume(cpu->tk);
	while (!(hit_bp = is_valid_bp(cpu->pc)) && !step_result && !sigint_received) {
		step_result = mos6502_step(cpu);
	}
	timekeeper_pause(cpu->tk);

	check_step_result(cpu, step_result);

	if (hit_bp) {
		INFO_PRINT("  Breakpoint at $%04x reached", cpu->pc);
		remove_bp(cpu->pc);
	}

	print_pc_update(cpu);
	return 0;
}

static int
cmd_break_rm (mos6502_t * cpu, char * args)
{
	size_t addr;
	GET_HEX_ADDR(addr);

	if (remove_bp((uint16_t)addr)) {
		ERROR_PRINT("  Couldn't remove a breakpoint at $%04x", (uint16_t)addr);
		return 0;
	}

	INFO_PRINT("  Breakpoint at $%04x removed", (uint16_t)addr);
	return 0;
}

static int
cmd_break_list (mos6502_t * cpu, char * args)
{
	bp_list();
	return 0;
}

static int
cmd_break (mos6502_t * cpu, char * args)
{
	size_t addr;
	GET_HEX_ADDR(addr);

	if (insert_bp((uint16_t)addr)) {
		ERROR_PRINT("  Couldn't set a breakpoint at $%04x", (uint16_t)addr);
		return 0;
	}

	INFO_PRINT("  Breakpoint set at $%04x", (uint16_t)addr);
	return 0;
}

static noreturn int
cmd_quit (mos6502_t * cpu, char * args)
{
	printf("  Quitting. Goodbye.\n");
	exit(0);
}

#define SPELLINGS(...) ((const char * []){__VA_ARGS__, NULL})

typedef struct command_descriptor {
	const char * const * spellings;
	const char * usage;
	const char * description;
	int (*handler)(mos6502_t *, char *);
} command_descriptor_t;

static const command_descriptor_t commands[] = {
	{SPELLINGS("help", "?", "h"),
		"",
		"Prints a list of the available commands",
		cmd_help},

	{SPELLINGS("step", "s"),
		"[dec n] ",
		"Steps the CPU by n instructions (default 1)",
		cmd_step},

	{SPELLINGS("continue", "c"),
		"",
		"Continues program execution",
		cmd_cont},

	{SPELLINGS("jump", "j"),
		"<hex16 addr> ",
		"Sets the program counter (PC) to addr",
		cmd_jump},

	{SPELLINGS("regs", "r"),
		"",
		"Prints the values of each CPU register",
		cmd_regs},

	{SPELLINGS("peek", "pk"),
		"<hex16 addr> ",
		"Prints the byte at addr",
		cmd_peek},

	{SPELLINGS("poke", "po"),
		"<hex16 addr> <hex8 value> ",
		"Sets the byte at addr to value",
		cmd_poke},

	{SPELLINGS("dumpmem", "dm"),
		"<hex16 start> <dec length> ",
		"Dumps the memory in the address range [start, start+length)",
		cmd_dumpmem},

	{SPELLINGS("quit", "exit", "q"),
		"",
		"Quits this program",
		cmd_quit},

	{SPELLINGS("irq"),
		"",
		"Raises an IRQ",
		cmd_irq},

	{SPELLINGS("nmi"),
		"",
		"Raises an NMI",
		cmd_nmi},

	{SPELLINGS("pr", "print"),
		"",
		"Prints the current instruction",
		cmd_print_instr},

	{SPELLINGS("break-rm", "b-rm"),
		"<hex16 addr> ",
		"Removes a breakpoint at addr",
		cmd_break_rm},

	{SPELLINGS("break-list", "b-list"),
		"",
		"Lists all active breakpoints",
		cmd_break_list},

	{SPELLINGS("break", "b"),
		"<hex16 addr> ",
		"Sets a breakpoint at addr",
		cmd_break},
};

static void
print_usage (void)
{
	for (size_t i = 0; i < sizeof(commands) / sizeof(*commands); i++) {
		const char * const * spelling_ptr = commands[i].spellings;
		SUGGESTION_PRINT_NOBRK("  ");
		goto skip_or;

		for (; *spelling_ptr; spelling_ptr++) {
			SUGGESTION_PRINT_NOBRK(UNBOLD(" or "));
		skip_or:
			SUGGESTION_PRINT_NOBRK("%s", *spelling_ptr);
		}

		SUGGESTION_PRINT("  %s" UNBOLD("-- %s"), commands[i].usage, commands[i].description);
	}
}

static int
handle_cmd (mos6502_t * cpu, char * line)
{
	char * cmd = next_token(&line);
	for (size_t i = 0; i < sizeof(commands) / sizeof(command_descriptor_t); i++) {
		for (const char * const * spelling_ptr = commands[i].spellings; *spelling_ptr; spelling_ptr++) {
			if (!strcmp(cmd, *spelling_ptr)) {
				sigint_received = false;

				int retcode = commands[i].handler(cpu, line);
				if (retcode) {
					ERROR_PRINT("  Invalid syntax");
					SUGGESTION_PRINT("  Usage: %s %s", *spelling_ptr, commands[i].usage);
				}

				return retcode;
			}
		}
	}
	ERROR_PRINT("  Unknown command");
	return -1;
}

static void
handle_sigint (int signum)
{
	sigint_received = true;
}

static const struct sigaction sigint_action = {.sa_handler = handle_sigint};

void
run_shell (mos6502_t * cpu, bool interactive)
{
	if (sigaction(SIGINT, &sigint_action, NULL)) {
		ERROR_PRINT("  Couldn't register a SIGINT handler");
		return;
	}

	if (interactive) {
		goto prompt;
	}

	char * line = NULL;
	do {
		if (line && line[0]) {
			add_history(line);
			handle_cmd(cpu, line);
		} else if (!line) {
			sigint_received = false;
			cmd_cont(cpu, "");
		}

		if (line) {
			free(line);
		}
	prompt:
		continue;
	} while ((line = readline(PROMPT_STR)));
}
