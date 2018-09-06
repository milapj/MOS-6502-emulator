#include <rc.h>
#include <base.h>
#include <ines.h>
#include <shell.h>
#include <fileio.h>
#include <memory.h>
#include <nes/ppu.h>
#include <nes/io_reg.h>

#include <SDL2/SDL.h>

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdnoreturn.h>

#include <getopt.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#define NES_NTSC_SYSCLK (236.25 / 11. * 1000000)

static const uint8_t hawknest_magic[4] = {'H', 'K', 'N', 'S'};
static const uint8_t ines_magic[4] = {0x4E, 0x45, 0x53, 0x1A};

static inline int
hawknest_rom_load (FILE * nonnull f, const char * nonnull path, reset_manager_t * nonnull rm, mos6502_t * nonnull cpu)
{
	int retcode = 0;

	memory_t * cartrom = memory_new(rm, 0x6000, false);
	if (!cartrom) {
		retcode = -1;
		goto ret;
	}
	if ((retcode = try_fread(f, path, cartrom->bytes, cartrom->size))) {
		retcode = -1;
		goto release_cartrom;
	}
	memory_map(cartrom, cpu->bus, 0xA000, (uint16_t)cartrom->size, 0);

	memory_t * ram = memory_new(rm, 32768, true);
	if (!ram) {
		retcode = -1;
		goto release_cartrom;
	}
	memory_map(ram, cpu->bus, 0, (uint16_t)ram->size, 0);
	rc_release(ram);

release_cartrom:
	rc_release(cartrom);
ret:
	return retcode;
}

static inline int
load_rom (const char * nonnull path,
	  reset_manager_t * nonnull rm,
	  mos6502_t * nonnull cpu,
	  const char * nonnull palette_path,
	  const char * nonnull cscheme_path,
	  int scale)
{
	int retcode = 0;

	FILE * f = try_fopen(path, "rb");
	if (!f) {
		retcode = -1;
		goto ret0;
	}

	uint8_t magic[4];
	if ((retcode = try_fread(f, path, magic, sizeof(magic)))) {
		goto ret1;
	}

	if (!memcmp(magic, hawknest_magic, sizeof(magic))) {
		retcode = hawknest_rom_load(f, path, rm, cpu);
		goto ret1;
	}
	else if (!memcmp(magic, ines_magic, sizeof(magic))) {
		retcode = inesrom_load(f, path, rm, cpu, palette_path, cscheme_path, scale);
		goto ret1;
	}

	retcode = -1;
	ERROR_PRINT("%s does not appear to be in a valid ROM format", path);

ret1:
	fclose(f);
ret0:
	return retcode;
}

static void
print_version (void)
{
	INFO_PRINT("Hawknest 6502v Emulator Version " HAWKNEST_VERSION_STRING);
	INFO_PRINT("Kyle C. Hale & Lucas C. Myers (c) 2018, Illinois Institute of Technology");
}

static void
print_usage (char ** argv)
{
	SUGGESTION_PRINT("Usage: " UNBOLD("%s [options] <rom-path>"), argv[0]);
	SUGGESTION_PRINT("Options:");
	SUGGESTION_PRINT("  " UNBOLD("--interactive ") "or " UNBOLD("-i        ") ": Start the Hawknest shell immediately");
	SUGGESTION_PRINT("  " UNBOLD("--palette     ") "or " UNBOLD("-p <path> ") ": Use the NES palette at " UNBOLD("<path>"));
	SUGGESTION_PRINT("  " UNBOLD("--cscheme     ") "or " UNBOLD("-c <path> ") ": Use the NES controller scheme at " UNBOLD("<path>"));
	SUGGESTION_PRINT("  " UNBOLD("--scale       ") "or " UNBOLD("-s <int>  ") ": Scale NES output by " UNBOLD("<int>"));
	SUGGESTION_PRINT("  " UNBOLD("--help        ") "or " UNBOLD("-h        ") ": Print this message");
	SUGGESTION_PRINT("  " UNBOLD("--version     ") "or " UNBOLD("-V        ") ": Print version information");
}

static struct option long_options[] = {
	{"interactive", no_argument, 0, 'i'},
	{"palette", required_argument, 0, 'p'},
	{"cscheme", required_argument, 0, 'c'},
	{"scale", required_argument, 0, 's'},
	{"help", no_argument, 0, 'h'},
	{"version", no_argument, 0, 'V'},
	{0, 0, 0, 0}};

int
main (int argc, char ** argv)
{
	if (argc == 1) {
		print_usage(argv);
		return 0;
	}

	int retcode = -1;

	char * cscheme_path = "cscheme";
	char * palette_path = "palette";
	bool interactive = false;
	int scale = 1;

	while (1) {
		int opt_idx = 0;
		int c = getopt_long(argc, argv, "p:c:s:hiV", long_options, &opt_idx);

		if (c == -1) {
			break;
		}

		switch (c) {
		case 0:
			break;
		case 'i':
			interactive = true;
			break;
		case 'p':
			palette_path = optarg;
			break;
		case 'c':
			cscheme_path = optarg;
			break;
		case 's':
			scale = atoi(optarg);
			break;
		case 'V':
			print_version();
			retcode = 0;
			goto ret;
		case 'h':
			print_usage(argv);
			retcode = 0;
			goto ret;
		case '?':
			break;
		default:
			ERROR_PRINT("Unknown option '%o'", c);
			goto ret;
		}
	}

	char * rom_path = argv[optind++];
	if (!rom_path) {
		ERROR_PRINT("No ROM path provided");
		print_usage(argv);
		goto ret;
	}

	uint16_t argc_ext = (uint16_t)(argc - optind);
	char ** argv_ext = &argv[optind];

	if (SDL_Init(0)) {
		ERROR_PRINT("Couldn't initialize SDL");
		goto ret;
	}

	reset_manager_t * rm = reset_manager_new();
	if (!rm) {
		ERROR_PRINT("Failed to create a reset manager");
		goto quit_sdl;
	}

	timekeeper_t * tk = timekeeper_new(rm, 1.0 / NES_NTSC_SYSCLK);
	if (!tk) {
		ERROR_PRINT("Failed to create a timekeeper");
		goto release_rm;
	}

	mos6502_t * cpu = mos6502_new(rm, tk, argc_ext, argv_ext);
	if (!cpu) {
		ERROR_PRINT("Failed to create a CPU");
		goto release_tk;
	}

	if (load_rom(rom_path, rm, cpu, palette_path, cscheme_path, scale)) {
		ERROR_PRINT("Couldn't initialize system");
		goto release_cpu;
	}

	reset_manager_issue_reset(rm);
	mos6502_reset(cpu);
	timekeeper_pause(cpu->tk);
	run_shell(cpu, interactive);

	retcode = 0;

release_cpu:
	rc_release(cpu);
release_tk:
	rc_release(tk);
release_rm:
	rc_release(rm);
quit_sdl:
	SDL_Quit();
ret:
	return retcode;
}
