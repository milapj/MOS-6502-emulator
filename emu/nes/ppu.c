/* >>=PPU Emulation [LCM]=<<
 * The NES's PPU is simple enough to emulate at full speed with a state
 * machine, but complicated enough to for correct emulation to be difficult.
 * I'm going to assume some familiarity with the high-level workings of the
 * PPU; [https://wiki.nesdev.com/w/index.php/PPU] is a good resource. This
 * documentation is going to focus on implementation details, since this is
 * probably the second-largest file in the entire codebase as of writing this.
 *
 * As with any sort of emulation, what we're really looking to do is emulate
 * all _observable_ aspects of the original hardware. Unfortunately for us,
 * most aspects of the original hardware were observable. Since the PPU's
 * memory bus connected to game cartridges, its memory activity was completely
 * observable, and in some mappers was used to deduce when the horizontal
 * blanking interval started. The PPU registers mapped onto the CPU's memory
 * bus were accessible at all points during the PPU's operation, and had
 * different behavior depending on exactly what the PPU was doing at any given
 * time.
 *
 * In this implementation, we aim for complete cycle-accurate emulation of all
 * observable behavior, with a few notable exceptions:
 *
 * - The OAMADDR and OAMDATA registers are "weakly" emulated. We maintain a
 *   virtual OAM that is accessible (by the CPU) through these two registers
 *   (as well as OAMDMA), but the PPU implementation accesses the contents of
 *   OAM without using OAMADDR and without any timing-accuracy, so any game
 *   that relies on mid-scanline updates of OAM or observing OAMADDR will not
 *   work correctly. Since I'm not aware of any games that do this, nor any
 *   sane individual who would want to write a game to do this, I don't expect
 *   this to become an issue.
 *
 * - Though the actual hardware had clock-crossing logic in place for the
 *   memory-mapped PPU registers, this (along with the potential phase shift
 *   between the PPU and CPU) is not at all emulated. This is similarly not
 *   expected to become an issue.
 *
 * - The actual hardware directly produced composite NTSC video, which led to
 *   some interesting visual artifacts. Emulating composite video generation
 *   and decoding sounded complicated, so we instead produce very boring
 *   pixelated images.
 *
 * - There are likely yet-undiscovered issues with this implementation.
 *
 * With these disclaimers out of the way, let's get to how this thing actually
 * works.
 *
 * TODO write the rest of this
 */

#include <rc.h>
#include <base.h>
#include <membus.h>
#include <nes/ppu.h>
#include <SDL2/SDL.h>
#include <mos6502/mos6502.h>

// Displays the current frame of video to the screen, and processes SDL_QUIT
// events
static inline void
present_frame (ppu_t * nonnull ppu)
{
	// Check if we should quit (e.g. the user clicked the close-window button)
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			INFO_PRINT("Goodbye!");
			exit(EXIT_SUCCESS);
		}
	}

	// "Commit" the new frame contents to the backing `SDL_Texture` object
	SDL_UnlockTexture(ppu->tex);

	// Copy the frame to the backbuffer, upscaling it if required
	SDL_RenderCopy(ppu->rend, ppu->tex, NULL, NULL);

	// Swap buffers to display the new frame, synchronously blocking until
	// a new backbuffer is available
	SDL_RenderPresent(ppu->rend);

	// Lock the frame texture so that it can be overwritten with the next
	// frame
	int pitch;
	SDL_LockTexture(ppu->tex, NULL, (void **)&ppu->texdata, &pitch);
	ppu->texpitch = (size_t)pitch;
}

// Increments the coarse (8-pixel) x-position for background scrolling,
// properly handling nametable wrapping. Stolen from
// [https://wiki.nesdev.com/w/index.php/PPU_scrolling].
static inline void
inc_coarse_x (ppu_t * nonnull ppu)
{
	if ((ppu->vram_addr & 0x001F) == 31) {
		ppu->vram_addr &= ~0x001F;
		ppu->vram_addr ^= 0x0400;
	}
	else {
		ppu->vram_addr += 1;
	}
}

// Increments the y position for background scrolling, properly handling both
// coarse and fine increments, and nametable wrapping. Stolen from
// [https://wiki.nesdev.com/w/index.php/PPU_scrolling].
static inline void
inc_y (ppu_t * nonnull ppu)
{
	if ((ppu->vram_addr & 0x7000) != 0x7000) {
		ppu->vram_addr += 0x1000;
		return;
	}
	ppu->vram_addr &= ~0x7000;
	int y = (ppu->vram_addr & 0x03E0) >> 5;
	if (y == 29) {
		y = 0;
		ppu->vram_addr ^= 0x0800;
	}
	else if (y == 31) {
		y = 0;
	}
	else {
		y += 1;
	}
	ppu->vram_addr = (ppu->vram_addr & ~0x03E0) | (uint16_t)(y << 5);
}

// Performs the increment that happens to the VRAM address register when the
// CPU accesses $2007. This has weird behavior if done during rendering; see
// [https://wiki.nesdev.com/w/index.php/PPU_scrolling#.242007_reads_and_writes]
// for more.
static inline void
inc_vram_addr_rw (ppu_t * nonnull ppu)
{
	if (!(ppu->bg_en || ppu->sprite_en) || (ppu->slnum >= 240 && ppu->slnum != 261)) {
		ppu->vram_addr += ppu->vram_addr_inc ? 32 : 1;
		return;
	}
	inc_coarse_x(ppu);
	inc_y(ppu);
}

// If `addr` overlaps the palette memory region, returns a native pointer to
// the byte in palette (`ppu->palette_mem`) memory. Otherwise, returns NULL.
static inline uint8_t * nullable
palette_loc (ppu_t * nonnull ppu, uint16_t addr)
{
	if (addr < 0x3F00 || addr > 0x3FFF) {
		return NULL; // `addr` not in palette memory
	}

	// Handle special-case mirroring
	uint16_t reladdr = (addr - 0x3F00) % 0x20;
	size_t offset;
	switch (reladdr) {
	case 0x10:
		offset = 0x00;
		break;
	case 0x14:
		offset = 0x04;
		break;
	case 0x18:
		offset = 0x08;
		break;
	case 0x1C:
		offset = 0x0C;
		break;
	default:
		offset = reladdr;
		break;
	}

	ASSERT(offset < sizeof(ppu->palette_mem));
	return &ppu->palette_mem[offset];
}

// Sets the status registers that we previously determined needed to be set.
// This means setting the `sprite0_hit` flag on the dot after it was detected,
// and the `sprite_overflow` flag on dot determined during sprite evaluation.
static inline void
set_delayed_regs (ppu_t * nonnull ppu)
{
	if (ppu->sprite0_hit_shouldset) {
		ppu->sprite0_hit = true;
		ppu->sprite0_hit_shouldset = false;
	}

	if (ppu->dotnum == ppu->overflow_dotnum && ppu->overflow_dotnum) {
		ppu->sprite_overflow = true;
		ppu->overflow_dotnum = 0;
	}
}

// Clears status registers as appropriate
static inline void
clear_regs (ppu_t * nonnull ppu)
{
	if (ppu->dotnum == 1 && ppu->slnum == 261) {
		ppu->sprite0_hit = false;
		ppu->sprite0_hit_shouldset = false;
		ppu->sprite_overflow = false;
		ppu->write_toggle = false;
		ppu->vblank = false;
	}
}

// If rendering is enabled and the cursor is within the visible part of the
// frame, computes and sets the appropriate pixel
static inline void
draw_pixel (ppu_t * ppu)
{
	if (!(ppu->bg_en || ppu->sprite_en) || ppu->dotnum < 1 || ppu->dotnum > 256 || ppu->slnum >= 240) {
		return;
	}

	// Fetch the next color and palette bits from the background shiftregs
	uint8_t bg_color = 0, bg_palette = 0;
	bg_color |= ((ppu->bg_bmp_shiftregs[0] << ppu->fine_xscroll) >> 15) & 0x1;
	bg_color |= ((ppu->bg_bmp_shiftregs[1] << ppu->fine_xscroll) >> 14) & 0x2;
	bg_palette |= ((ppu->bg_attr_shiftregs[0] << ppu->fine_xscroll) >> 7) & 0x1;
	bg_palette |= ((ppu->bg_attr_shiftregs[1] << ppu->fine_xscroll) >> 6) & 0x2;

	// Decrement the relative x-position of each sprite
	for (size_t i = 0; i < 8; i++) {
		if (!ppu->sprite_xs[i]) {
			continue;
		}
		ppu->sprite_xs[i]--;
	}

	// Process pixels from all active sprites, in order of priority
	uint8_t sprite_color = 0, sprite_palette = 0;
	bool sprite_behind_bg = false, is_sprite0 = false;

	for (size_t i = 0; i < 8; i++) {
		if (ppu->sprite_xs[i]) {
			continue;
		}

		if (!sprite_color) {
			// Current sprite pixel is transparent
			sprite_color |= (ppu->sprite_bmp_shiftregs[i][0] >> 7) & 0x1;
			sprite_color |= (ppu->sprite_bmp_shiftregs[i][1] >> 6) & 0x2;
			sprite_palette = ppu->sprite_attrs[i].palette;
			sprite_behind_bg = ppu->sprite_attrs[i].behind_bg;
			is_sprite0 = !i && ppu->scanline_has_sprite0;
		}

		// Shift the sprite shiftregs, so that the next dot gets the
		// next pixel in the sprite
		ppu->sprite_bmp_shiftregs[i][0] <<= 1;
		ppu->sprite_bmp_shiftregs[i][1] <<= 1;
	}

	// When sprites are disabled, force the sprite to be transparent
	if (!ppu->sprite_en || (!ppu->left_sprite_en && ppu->dotnum <= 8)) {
		sprite_color = 0;
	}

	// When the background is disabled, force the background to be
	// transparent
	if (!ppu->bg_en || (!ppu->left_bg_en && ppu->dotnum <= 8)) {
		bg_color = 0;
	}

	uint16_t paladdr;

	if (!bg_color) {
		if (!sprite_color) {
			paladdr = 0x3F00;
			goto output;
		}
		goto sprite;
	}

	if (sprite_color && is_sprite0 && ppu->dotnum != 256) {
		ppu->sprite0_hit_shouldset = true;
	}

	if (!sprite_color || sprite_behind_bg) {
		goto bg;
	}

	// fallthrough

sprite:
	paladdr = 0x3F11 + 4 * sprite_palette + sprite_color - 1;
	goto output;
bg:
	paladdr = 0x3F01 + 4 * bg_palette + bg_color - 1;
	// fallthrough
output : {
	uint16_t pixel_color = *palette_loc(ppu, paladdr);
	pixel_color |= ppu->emph_red << 6;
	pixel_color |= ppu->emph_green << 7;
	pixel_color |= ppu->emph_blue << 8;

	uint8_t * pixdata = ppu->texdata + ppu->slnum * ppu->texpitch + (ppu->dotnum - 1) * 4;
	pixdata[0] = ppu->palette_srgb[pixel_color][0];
	pixdata[1] = ppu->palette_srgb[pixel_color][1];
	pixdata[2] = ppu->palette_srgb[pixel_color][2];
	pixdata[3] = 255;
}}

// Perform the sprite evaluation process in a single step. This involves
// determining which sprites overlap the next scanline (at most 8), saving
// their information separately, and computing when (and if) the sprite
// overflow flag should be set along the current scanline.
static inline void
spriteeval (ppu_t * nonnull ppu)
{
	// This process is only performed on the first dot of each visible
	// scanline, when rendering is enabled
	if (!(ppu->bg_en || ppu->sprite_en) || ppu->dotnum != 0 || ppu->slnum >= 240) {
		return;
	}

	// Clear the state set by the last sprite evaluation
	memset(ppu->eval_sprites, 0xFF, sizeof(ppu->eval_sprites));
	ppu->eval_nsprites = 0;

	// Propagate the `has_sprite0` flags
	ppu->scanline_has_sprite0 = ppu->next_scanline_has_sprite0;
	ppu->next_scanline_has_sprite0 = false;

	// Evaluate sprites
	size_t spriteheight = ppu->spritesize == PPU_SPRITESIZE_8x8 ? 8 : 16;
	size_t spritenum = 0;
	for (; ppu->eval_nsprites < 8 && spritenum < 64; spritenum++) {
		ppu_sprite_t sprite = ppu->sprites[spritenum];
		if (ppu->slnum < sprite.ypos || ppu->slnum >= sprite.ypos + spriteheight) {
			continue;
		}

		if (!spritenum) {
			ppu->next_scanline_has_sprite0 = true;
		}

		ppu->eval_sprites[ppu->eval_nsprites] = sprite;
		ppu->eval_nsprites++;
	}

	// Check excess sprites for overflow
	for (; spritenum < 64; spritenum++) {
		if (ppu->slnum >= ppu->sprites[spritenum].ypos || ppu->slnum < ppu->sprites[spritenum].ypos + spriteheight) {
			// [https://forums.nesdev.com/viewtopic.php?f=2&t=15870]
			// Eight cycles pass for each of the 8 sprites that we
			// *will* render, along with 2 cycles for each of the
			// sprites that we found not to overlap this scanline,
			// and finally 2 cycles while examining the overflowing
			// sprite before the overflow flag is set.
			ppu->overflow_dotnum = 8 * 8 + (spritenum + 1 - 8) * 2;
			break;
		}
	}
}

// Compute the address of the byte of first-plane bitmap data to be fetched,
// based on the byte loaded earlier from the nametable. The corresponding byte
// from the second plane can be found simply by adding `8` to this address,
// since the two color planes are stored sequentially in each tile.
static inline uint16_t
bg_bmp_addr (ppu_t * nonnull ppu)
{
	uint16_t addr = ppu->bg_chr_baseaddr == PPU_CHR_BASEADDR_1000 ? 0x1000 : 0x0000;
	addr += ppu->nt_latch * 16;
	addr += ppu->fine_yscroll;
	return addr;
}

// Bitswap a byte in-place, using a small number of efficient integer
// operations (no divisions). Stolen from
// [https://graphics.stanford.edu/~seander/bithacks.html#ReverseByteWith64Bits].
static inline void
bitswap (uint8_t * nonnull val)
{
	*val = ((*val * 0x80200802ULL) & 0x0884422110ULL) * 0x0101010101ULL >> 32;
}

// Compute the address of the nametable byte to be fetched, based on the
// current VRAM address. This is stolen from
// [https://wiki.nesdev.com/w/index.php/PPU_scrolling].
static inline uint16_t
nt_addr (ppu_t * nonnull ppu)
{
	return 0x2000 | (ppu->vram_addr & 0x0FFF);
}

// Compute the address of the attribute byte to be fetched, based on the
// current VRAM address. This is stolen from
// [https://wiki.nesdev.com/w/index.php/PPU_scrolling].
static inline uint16_t
attr_addr (ppu_t * nonnull ppu)
{
	return 0x23C0 | (ppu->vram_addr & 0x0C00) | ((ppu->vram_addr >> 4) & 0x38) | ((ppu->vram_addr >> 2) & 0x07);
}

// Read background bitmap, nametable, and attribute table data as appropriate
static inline void
bg_memfetch (ppu_t * nonnull ppu)
{
	// Background data is fetched constantly during normal rendering, and
	// also at the end of each visible scanline, to prepare for the next
	if ((ppu->dotnum < 1 || ppu->dotnum > 256) && (ppu->dotnum < 321 || ppu->dotnum > 336)) {
		return;
	}

	switch ((ppu->dotnum - 1) % 8) {
	case 1:
		ppu->nt_latch = membus_read(ppu->bus, nt_addr(ppu));
		break;
	case 3:
		ppu->attr_latch = membus_read(ppu->bus, attr_addr(ppu));
		break;
	case 5:
		ppu->bmp_latch[0] = membus_read(ppu->bus, bg_bmp_addr(ppu));
		break;
	case 7:
		ppu->bmp_latch[1] = membus_read(ppu->bus, bg_bmp_addr(ppu) + 8);
		break;
	}
}

// Read sprite bitmap data as appropriate, along with some garbage bytes from
// nametable memory. Horizontal sprite flipping is handled at this time by
// bitswapping the loaded bitmap data.
static inline void
sprite_memfetch (ppu_t * nonnull ppu)
{
	// Sprite information is only fetched from dots 257 through 320
	if (ppu->dotnum < 257 || ppu->dotnum > 320) {
		return;
	}

	size_t spritenum = (ppu->dotnum - 257) / 8;
	ppu_sprite_t sprite = ppu->eval_sprites[spritenum];

	uint16_t bmp_addr;
	uint8_t tile = sprite.tile;
	if (ppu->spritesize) {
		bmp_addr = 0x1000 * (tile & 0x1);
		tile &= ~0x1;
	}
	else {
		bmp_addr = ppu->sprite_chr_baseaddr ? 0x1000 : 0x0000;
	}

	uint8_t y_offset = ppu->coarse_yscroll * 8 + ppu->fine_yscroll - 1 - sprite.ypos;
	if (ppu->spritesize == PPU_SPRITESIZE_8x16) {
		tile += (y_offset / 8) ^ sprite.attr.verti_flipped;
		y_offset %= 8;
	}

	if (sprite.attr.verti_flipped) {
		y_offset = 7 - y_offset;
	}

	bmp_addr += tile * 16 + y_offset;

	switch ((ppu->dotnum - 1) % 8) {
	case 1:
		membus_read(ppu->bus, nt_addr(ppu));
		break;
	case 3:
		membus_read(ppu->bus, attr_addr(ppu));
		break;
	case 5:
		ppu->sprite_bmp_shiftregs[spritenum][0] = membus_read(ppu->bus, bmp_addr);
		if (sprite.attr.horiz_flipped) {
			bitswap(&ppu->sprite_bmp_shiftregs[spritenum][0]);
		}
		if (spritenum >= ppu->eval_nsprites) {
			ppu->sprite_bmp_shiftregs[spritenum][0] = 0;
		}
		break;
	case 7:
		ppu->sprite_bmp_shiftregs[spritenum][1] = membus_read(ppu->bus, bmp_addr + 8);
		if (sprite.attr.horiz_flipped) {
			bitswap(&ppu->sprite_bmp_shiftregs[spritenum][1]);
		}
		if (spritenum >= ppu->eval_nsprites) {
			ppu->sprite_bmp_shiftregs[spritenum][1] = 0;
		}

		ppu->sprite_xs[spritenum] = sprite.xpos;
		ppu->sprite_attrs[spritenum] = sprite.attr;
		break;
	}
}

// Perform the garbage reads that occur at the end of each visible scanline,
// since some mappers track these reads for timing
static inline void
dummy_memfetch (ppu_t * nonnull ppu)
{
	// TODO implement
}

// Perform read transfers for background and sprite data, as appropriate
static inline void
memfetch (ppu_t * nonnull ppu)
{
	// Don't perform any transfers when rendering is disabled, or within
	// vblank
	if (!(ppu->bg_en || ppu->sprite_en) || (ppu->slnum >= 240 && ppu->slnum != 261)) {
		return;
	}
	bg_memfetch(ppu);
	sprite_memfetch(ppu);
	dummy_memfetch(ppu);
}

// Update the VRAM address register as appropriate for the current clock cycle
// See [https://wiki.nesdev.com/w/index.php/PPU_scrolling] for more details
static inline void
update_vram_addr (ppu_t * nonnull ppu)
{
	// Do nothing if rendering is disabled
	if (!ppu->bg_en && !ppu->sprite_en) {
		return;
	}

	// Only do x and y increments outside of vblank
	if (ppu->slnum < 240) {
		// Check if it's time for an increment of the coarse x-scroll
		if ((ppu->dotnum >= 328 || ppu->dotnum <= 257) && ppu->dotnum != 1 &&
		    !((ppu->dotnum - 1) % 8)) {
			inc_coarse_x(ppu);
			// Check if it's also time for an increment of the
			// y-scroll
			if (ppu->dotnum == 257) {
				inc_y(ppu);
			}
		}
		// Do weird stuff on dot 258
		else if (ppu->dotnum == 258) {
			ppu->vram_addr &= ~0x041F;
			ppu->vram_addr |= ppu->tmp_vram_addr & 0x041F;
		}
	}
	// Do more weird stuff from dots 280 to 304 of the pre-render scanline
	else if (ppu->slnum == 261 && ppu->dotnum >= 280 && ppu->dotnum <= 304) {
		ppu->vram_addr &= ~0x3BE0;
		ppu->vram_addr |= ppu->tmp_vram_addr & 0x3BE0;
	}
}

// Update the `dotnum` and `slnum`
static inline void
move_cursor (ppu_t * nonnull ppu)
{
	size_t sl_length;
	if (ppu->slnum == 261 && ppu->framenum % 2) {
		sl_length = 340;
	}
	else {
		sl_length = 341;
	}

	if (++ppu->dotnum != sl_length) {
		return;
	}

	ppu->dotnum = 0;

	if (++ppu->slnum != 262) {
		return;
	}

	ppu->slnum = 0;
	ppu->framenum++;
}

// Loads the background bitmap shift registers with new bytes previously loaded
// from memory, and updates the attribute bit latches
static inline void
load_shiftregs (ppu_t * nonnull ppu)
{
	// Check if reloading actually occurs during this cycle
	if ((ppu->dotnum - 1) % 8 ||
	    ppu->dotnum == 1 ||
	    ppu->dotnum == 321 ||
	    (ppu->dotnum >= 257 && ppu->dotnum <= 320)) {
		return;
	}

	ppu->bg_bmp_shiftregs[0] &= 0xFF00;
	ppu->bg_bmp_shiftregs[1] &= 0xFF00;
	ppu->bg_bmp_shiftregs[0] |= ppu->bmp_latch[0];
	ppu->bg_bmp_shiftregs[1] |= ppu->bmp_latch[1];

	uint8_t attr_x = (ppu->coarse_xscroll / 2) % 2;
	uint8_t attr_y = (ppu->coarse_yscroll / 2) % 2;
	uint8_t attr_idx = attr_y * 2 + attr_x;
	uint8_t attr_shift = attr_idx * 2;
	ppu->bg_palette = (ppu->attr_latch >> attr_shift) & 0x3;
}

// Performs the actual shift of the background shift registers
static inline void
shift_shiftregs (ppu_t * nonnull ppu)
{
	// Check if shifting actually occurs during this cycle
	if (!(ppu->dotnum >= 2 && ppu->dotnum <= 257) && !(ppu->dotnum >= 322 && ppu->dotnum <= 337)) {
		return;
	}

	// Shift zeros into the bitmap registers
	ppu->bg_bmp_shiftregs[0] <<= 1;
	ppu->bg_bmp_shiftregs[1] <<= 1;

	// Shift the values from the attribute latches into the attribute
	// registers
	ppu->bg_attr_shiftregs[0] = (uint8_t)(ppu->bg_attr_shiftregs[0] << 1) | (ppu->bg_palette & 0x1);
	ppu->bg_attr_shiftregs[1] = (uint8_t)(ppu->bg_attr_shiftregs[1] << 1) | (ppu->bg_palette >> 1);
}

static void
step (ppu_t * nonnull ppu)
{
	ppu->clk_countdown = PPU_CLKDIVISOR;

	if (ppu->slnum == 241 && ppu->dotnum == 1) {
		ppu->vblank = true;
		if (ppu->nmi_en) {
			mos6502_raise_nmi(ppu->cpu);
		}
		present_frame(ppu);
	}

	set_delayed_regs(ppu);
	clear_regs(ppu);

	memfetch(ppu);
	shift_shiftregs(ppu);
	load_shiftregs(ppu);
	update_vram_addr(ppu);
	spriteeval(ppu);

	draw_pixel(ppu);

	move_cursor(ppu);
}

// TODO handle latent values in PPU registers
static uint8_t
read (ppu_t * nonnull ppu, uint16_t addr)
{
	uint8_t val = 0, *palloc = NULL;
	uint16_t regnum = addr % 8;
	switch (regnum) {
	case 2: // PPUSTATUS
		val |= ppu->vblank << 7;
		val |= ppu->sprite0_hit << 6;
		val |= ppu->sprite_overflow << 5;

		ppu->write_toggle = false;
		ppu->vblank = false;
		break;

	case 4: // OAMDATA
		val = ppu->oam[ppu->oam_addr];
		break;

	case 7: // PPUDATA
		if ((palloc = palette_loc(ppu, ppu->vram_addr))) {
			val = *palloc;
		}
		else {
			val = membus_read(ppu->bus, ppu->vram_addr);
		}
		inc_vram_addr_rw(ppu);
		break;

	default:
		val = 0xFF;
		break;
	}

	return val;
}

static void
write (ppu_t * nonnull ppu, uint16_t addr, uint8_t val)
{
	uint8_t * palloc = NULL;
	uint16_t regnum = addr % 8;
	switch (regnum) {
	case 0: // PPUCTRL
		ppu->tmp_nt_baseaddr = val & 0x3;
		ppu->vram_addr_inc = !!(val & 0x4);
		ppu->sprite_chr_baseaddr = !!(val & 0x8);
		ppu->bg_chr_baseaddr = !!(val & 0x10);
		ppu->spritesize = !!(val & 0x20);
		// ignore EXT mode select

		bool old_nmi_en = ppu->nmi_en;
		ppu->nmi_en = !!(val & 0x80);
		if (ppu->nmi_en != old_nmi_en && ppu->vblank) {
			mos6502_raise_nmi(ppu->cpu);
		}

		break;

	case 1: // PPUMASK
		ppu->mask = val;
		break;

	case 3: // OAMADDR
		ppu->oam_addr = val;
		break;

	case 4: // OAMDATA
		ppu->oam[ppu->oam_addr++] = val;
		break;

	case 5: // PPUSCROLL
		if (!ppu->write_toggle) {
			ppu->tmp_coarse_xscroll = val >> 3;
			ppu->fine_xscroll = val & 0x7;
		}
		else {
			ppu->tmp_coarse_yscroll = val >> 3;
			ppu->tmp_fine_yscroll = val & 0x7;
		}
		ppu->write_toggle = !ppu->write_toggle;
		break;

	case 6: // PPUADDR
		if (!ppu->write_toggle) {
			ppu->tmp_vram_addr &= ~0x3F00;
			ppu->tmp_vram_addr |= (val & 0x3F) << 8;
		}
		else {
			ppu->tmp_vram_addr &= ~0xFF;
			ppu->tmp_vram_addr |= val;
			ppu->vram_addr = ppu->tmp_vram_addr;
		}
		ppu->write_toggle = !ppu->write_toggle;
		break;

	case 7: // PPUDATA
		if ((palloc = palette_loc(ppu, ppu->vram_addr))) {
			*palloc = val;
		}
		else {
			membus_write(ppu->bus, ppu->vram_addr, val);
		}

		inc_vram_addr_rw(ppu);
		break;

	default:
		break;
	}
}

static void
reset (ppu_t * nonnull ppu)
{
	ppu->framenum = 0;
	ppu->clk_countdown = PPU_CLKDIVISOR;

	ppu->slnum = 261;
	ppu->dotnum = 0;
	ppu->overflow_dotnum = 0;

	ppu->mask = 0;

	ppu->vram_addr_inc = 0;
	ppu->bg_chr_baseaddr = 0;
	ppu->sprite_chr_baseaddr = 0;
	ppu->spritesize = 0;

	ppu->nmi_en = 0;
	ppu->vblank = 0;
	ppu->sprite0_hit = 0;
	ppu->sprite_overflow = 0;
	ppu->write_toggle = 0;

	ppu->sprite0_hit_shouldset = 0;

	ppu->next_scanline_has_sprite0 = 0;
	ppu->scanline_has_sprite0 = 0;

	ppu->vram_addr = 0;
	ppu->tmp_vram_addr = 0;

	ppu->fine_xscroll = 0;

	memset(ppu->oam, 0x00, sizeof(ppu->oam));
	memset(ppu->palette_mem, 0x00, sizeof(ppu->palette_mem));
}

static void
deinit (ppu_t * nonnull ppu)
{
	rc_release(ppu->bus);

	SDL_DestroyTexture(ppu->tex);
	SDL_DestroyRenderer(ppu->rend);
	SDL_DestroyWindow(ppu->win);
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

ppu_t *
ppu_new (reset_manager_t * rm, mos6502_t * cpu, int scale)
{
	ppu_t * ppu = rc_alloc(sizeof(ppu_t), deinit);
	reset_manager_add_device(rm, ppu, reset);
	timekeeper_add_timer(cpu->tk, ppu, step, &ppu->clk_countdown);

	ppu->cpu = cpu;

	membus_t * nullable bus = membus_new(rm);
	if (!bus) {
		goto initerror;
	}
	ppu->bus = (membus_t * nonnull)bus;

	if (SDL_InitSubSystem(SDL_INIT_VIDEO)) {
		ERROR_PRINT("Could not init SDL video: %s", SDL_GetError());
		goto initerror;
	}

	ppu->win = SDL_CreateWindow(
		"Hawknest",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		PPU_OUTPUT_WIDTH * scale,
		PPU_OUTPUT_HEIGHT * scale,
		SDL_WINDOW_ALLOW_HIGHDPI);

	if (!ppu->win) {
		ERROR_PRINT("Could not create window: %s", SDL_GetError());
		goto winerror;
	}

	ppu->rend = SDL_CreateRenderer(ppu->win, -1, SDL_RENDERER_PRESENTVSYNC);
	if (!ppu->rend) {
		ERROR_PRINT("Could not create renderer: %s", SDL_GetError());
		goto renderror;
	}

	// Make sure the texture stays pixelated if `scale > 1`.
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

	// Create the texture, setting the access mode to `STREAMING` so that
	// we can efficiently replace its contents.
	ppu->tex = SDL_CreateTexture(
		ppu->rend,
		SDL_PIXELFORMAT_RGBA32,
		SDL_TEXTUREACCESS_STREAMING,
		PPU_OUTPUT_WIDTH,
		PPU_OUTPUT_HEIGHT);

	if (!ppu->tex) {
		ERROR_PRINT("Could not create texture: %s", SDL_GetError());
		goto texerror;
	}

	int pitch;
	SDL_LockTexture(ppu->tex, NULL, (void **)&ppu->texdata, &pitch);
	ppu->texpitch = (size_t)pitch;

	return ppu;
texerror:
	SDL_DestroyRenderer(ppu->rend);
renderror:
	SDL_DestroyWindow(ppu->win);
winerror:
	rc_release(ppu);
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
initerror:
	return NULL;
}

void
ppu_map (ppu_t * ppu)
{
	for (size_t i = 0x20; i < 0x40; i++) {
		membus_set_read_handler(ppu->cpu->bus, i, ppu, 0, read);
		membus_set_write_handler(ppu->cpu->bus, i, ppu, 0, write);
	}
}
