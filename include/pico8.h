#ifndef PICO8_H
#define PICO8_H

#include <gba_input.h>
#include <gba_sprites.h>
#include "pico8_math.h"

void init_pico8();
void update_keys();
void update_pico8();

#define CAMERA_OFFSET_X (7*8)
#define CAMERA_OFFSET_Y (2*8)

void camera(s16 x, s16 y);
void get_camera(s16* x, s16* y);
s16 get_camera_x();
s16 get_camera_y();
// Used for the shake effect. Used by drawn objects/backgrounds, which are not affected by the normal shake camera offset in _draw().
void set_shake_offsets(s16 x, s16 y);
s16 get_shake_offset_x();
s16 get_shake_offset_y();

enum PaletteID{ // offsets in palette memory
	PAL_RESET = 0xFEEF,

	PAL_BG_NORMAL = 0*16,
	PAL_BG_ALL_BLACK = 1*16, // overlay and screenwipe tiles
	PAL_BG_BACKGROUND_COLOR = 2*16,
	PAL_BG_FLOOR = 3*16, // (ice level etc)

	PAL_SPRITES_NORMAL = (16+0)*16,
	PAL_SPRITES_ALL_BLACK = (16+1)*16, // screenwipe sprite
	PAL_SPRITES_BG_CLOUD_COLOR = (16+2)*16,
	PAL_SPRITES_1000 = (16+3)*16, // 1000 point number
	PAL_SPRITES_LAVENDER_AS_BLACK = (16+4)*16, // ending screen stats and time
	PAL_SPRITES_WHITE_AS_YELLOW = (16+5)*16, // death circles
	PAL_SPRITES_WHITE_AS_PINK = (16+6)*16, // font
	PAL_SPRITES_WHITE_AS_DARK_BLUE = (16+7)*16, // font
	PAL_SPRITES_WHITE_AS_DARK_GRAY = (16+8)*16, // font

	PAL_TITLESCREEN_FLASH = 0xFFFF
};

void pal(unsigned int c0, unsigned int c1, enum PaletteID paletteID);
void cls(unsigned int color);

OBJATTR* get_next_obj();
OBJAFFINE* get_next_affine_obj(int* affine_id);

void music(int n);
void psfx(int sample_ID, int lock);


#define KEY_BTN0 KEY_LEFT
#define KEY_BTN1 KEY_RIGHT
#define KEY_BTN2 KEY_UP
#define KEY_BTN3 KEY_DOWN
#define KEY_BTN4 (KEY_A|KEY_L) // jump
#define KEY_BTN5 (KEY_B|KEY_R) // grapple
bool btn(KEYPAD_BITS key);
bool btnp(KEYPAD_BITS key);

FIXED_16 time();

#ifdef DEBUG
void errorHalt(char* errCode);
#endif

#endif
