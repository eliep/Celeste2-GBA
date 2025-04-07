#ifndef GAMEOBJECTS_DRAW_H
#define GAMEOBJECTS_DRAW_H

#include <gba_types.h>
#include "gameobjects.h"
#include "sprites.h"

void print(char* text, int x, int y, unsigned int c);
void draw_time_rect(int x, int y);
void draw_background_cloud_small(int x, int y, int size);
void draw_background_cloud_big(int x, int y, int size);
void draw_foreground_cloud_small(int x, int y, int size, bool holed);
void draw_foreground_cloud_big(int x, int y, int size, bool holed);
void draw_screenwipe(int x, int y, bool inwipe);
void draw_snowflake(int x, int y, int size);
void draw_score_rect();

void draw_level_objects_sprites();
void draw_level_objects_tiles();

void object_draw(struct Position_data* self_Position, SpriteOtherID spriteID);
void grapplepickup_draw(struct Position_data* self_Position);
void snowball_draw(struct Position_data* self_Position);
void berry_draw(struct Berry_data* self);

void crumble_draw(struct Crumble_data* self);
void checkpoint_draw(struct Checkpoint_data* self);

void player_draw(struct Player_data* self);
void update_rendered_sprites();

#endif