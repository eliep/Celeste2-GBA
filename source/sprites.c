#include "sprites.h"

#include <gba_sprites.h>

// devkitpro somehow does not have those defined :(
#define SIZE_8 (ATTR1_SIZE_8>>14)
#define SIZE_16 (ATTR1_SIZE_16>>14)
#define SIZE_32 (ATTR1_SIZE_32>>14)
#define SIZE_64 (ATTR1_SIZE_64>>14)


const int berry_collect_sprites[] = {
	Sprite_Circle6,
	Sprite_Berry_Collect2, 
	Sprite_Berry_Collect1, 
	Sprite_Berry_Collect0, // can't fit last/biggest one into spritesheet => maybe load on the fly?
	Sprite_Berry_Collect0 // also maybe could solve by scaling affine sprite of outline circle, but might look bad
};

const int circle_sprites[] = {
	Sprite_Circle0,
	Sprite_Circle1,
	Sprite_Circle2,
	Sprite_Circle3,
	Sprite_Circle4,
	Sprite_Circle5,
	Sprite_Circle6,
	Sprite_Circle7,
	Sprite_Circle8,
};

const u16 circle_size[] = {
	SIZE_8,
	SIZE_8,
	SIZE_8,
	SIZE_8,
	SIZE_16,
	SIZE_16,
	SIZE_16,
	SIZE_16,
	SIZE_32
};

const SpriteCloudID clouds_small_sprite[] = {
	Sprite_CloudSmall3,
	Sprite_CloudSmall7,
	Sprite_CloudSmall11,
	Sprite_CloudSmall15,
	Sprite_CloudSmall19,
	Sprite_CloudSmall23,
	Sprite_CloudSmall27,
	Sprite_CloudSmall31
};

const SpriteCloudID clouds_big_sprite[] = {
	Sprite_CloudBig3,
	Sprite_CloudBig7,
	Sprite_CloudBig11,
	Sprite_CloudBig15,
	Sprite_CloudBig19,
	Sprite_CloudBig23,
	Sprite_CloudBig27,
	Sprite_CloudBig31
};

const u16 clouds_small_size[8] = {
	SIZE_8,
	SIZE_16,
	SIZE_32,
	SIZE_32,
	SIZE_32,
	SIZE_64,
	SIZE_64,
	SIZE_64
};

const u16 clouds_big_size[8] = {
	SIZE_16,
	SIZE_16,
	SIZE_64,
	SIZE_64,
	SIZE_64,
	SIZE_64,
	SIZE_64,
	SIZE_64
};

const u16 clouds_sprite_height[4] = {8, 8, 16, 32};

const u16 clouds_actual_height_background[8] = {5, 6, 8, 9, 10, 12, 13, 14};
const u16 clouds_actual_height_foreground[8] = {6, 8, 10, 11, 13, 15, 16, 18};