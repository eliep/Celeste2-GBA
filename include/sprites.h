#ifndef SPRITES_H
#define SPRITES_H

#include <gba_types.h>

#define SPRITE_COORDINATES(x,y) ((y)*32+(x))

typedef enum SpriteOtherID{
    Sprite_TimeRect0 = SPRITE_COORDINATES(18,6),
    Sprite_TimeRect1 = SPRITE_COORDINATES(18+4,6),

    Sprite_ScoreRect0 = SPRITE_COORDINATES(8,4),
    Sprite_ScoreRect1 = SPRITE_COORDINATES(16,4),
    Sprite_ScoreRect2 = SPRITE_COORDINATES(8,8),
    Sprite_ScoreRect3 = SPRITE_COORDINATES(12,8),
    Sprite_ScoreRect4 = SPRITE_COORDINATES(16,8),

    Sprite_Screenwipe = SPRITE_COORDINATES(0,0),

    Sprite_Springboard_Normal = SPRITE_COORDINATES(21,4),
    Sprite_Springboard_Pressed = SPRITE_COORDINATES(21,5),

    Sprite_Snowball = SPRITE_COORDINATES(17,5),
    Sprite_1000 = SPRITE_COORDINATES(18,5),
    Sprite_Berry = SPRITE_COORDINATES(20,5),
    Sprite_Berry_Collect0 = SPRITE_COORDINATES(20,0),
    Sprite_Berry_Collect1 = SPRITE_COORDINATES(24,0),
    Sprite_Berry_Collect2 = SPRITE_COORDINATES(28,0),
    Sprite_GrapplePickup = SPRITE_COORDINATES(22,5),
    Sprite_Bridge = SPRITE_COORDINATES(22,4),
    Sprite_OAM_Marker = SPRITE_COORDINATES(17,7)
} SpriteOtherID;

// for flash counting from 1 to 5
extern const int berry_collect_sprites[];

#define Sprite_PlayerID(num) (SPRITE_COORDINATES(17+(num),4))

typedef enum SpriteCircleID{
    Sprite_Circle0 = SPRITE_COORDINATES(23,4),
    Sprite_Circle1 = SPRITE_COORDINATES(23,5),
    Sprite_Circle2 = SPRITE_COORDINATES(23,6),
    Sprite_Circle3 = SPRITE_COORDINATES(23,7),
    Sprite_Circle4 = SPRITE_COORDINATES(24,4),
    Sprite_Circle5 = SPRITE_COORDINATES(24,6),
    Sprite_Circle6 = SPRITE_COORDINATES(26,4),
    Sprite_Circle7 = SPRITE_COORDINATES(26,6),
    Sprite_Circle8 = SPRITE_COORDINATES(28,4)
} SpriteCircleID;

enum SpriteRendered{
    Sprite_Grapple = SPRITE_COORDINATES(8,0),
    Sprite_Scarf = SPRITE_COORDINATES(16,0)
};

// for death circles
extern const int circle_sprites[];
// use with OBJ_SIZE() to get ATTR1
extern const u16 circle_size[];

typedef enum SpriteCloudID{ // Use with Clouds0() or Clouds1()
    Sprite_CloudSmall3 = SPRITE_COORDINATES(0,1),
    Sprite_CloudSmall7 = SPRITE_COORDINATES(4,1),
    Sprite_CloudSmall11 = SPRITE_COORDINATES(8,0),
    Sprite_CloudSmall15 = SPRITE_COORDINATES(12,0),
    Sprite_CloudSmall19 = SPRITE_COORDINATES(16,0),
    Sprite_CloudSmall23 = SPRITE_COORDINATES(24,-2),
    Sprite_CloudSmall27 = SPRITE_COORDINATES(0,2),
    Sprite_CloudSmall31 = SPRITE_COORDINATES(8,2),
    Sprite_CloudBig3 = SPRITE_COORDINATES(0,0),
    Sprite_CloudBig7 = SPRITE_COORDINATES(4,0),
    Sprite_CloudBig11 = SPRITE_COORDINATES(16,2),
    Sprite_CloudBig15 = SPRITE_COORDINATES(24,2),
    Sprite_CloudBig19 = SPRITE_COORDINATES(0,6),
    Sprite_CloudBig23 = SPRITE_COORDINATES(8,6),
    Sprite_CloudBig27 = SPRITE_COORDINATES(16,6),
    Sprite_CloudBig31 = SPRITE_COORDINATES(24,6)
} SpriteCloudID;

extern const SpriteCloudID clouds_small_sprite[];
extern const SpriteCloudID clouds_big_sprite[];

// use with OBJ_SIZE() to get ATTR1
extern const u16 clouds_small_size[8];
extern const u16 clouds_big_size[8];

// use clouds_xx_size[] as index
extern const u16 clouds_sprite_height[4];

extern const u16 clouds_actual_height_background[8];
extern const u16 clouds_actual_height_foreground[8];
#define Clouds0(s) ((s)+10*32)
#define Clouds1(s) ((s)+22*32)

#endif