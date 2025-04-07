#ifndef NIER_H
#define NIER_H

#include <gba_types.h>
#include "maps.h"

extern Level level;
extern int level_index;
extern bool have_grapple;
extern int freeze_time;
extern int shake;
extern int death_count;
extern int current_music;
extern int show_score;
extern int sfx_timer;
extern s16 camera_x, camera_y,
            camera_target_x, 
            camera_target_y;
extern int frames;

extern bool paused;

int approach(int x, int target, int max_delta);
extern const void (*camera_modes[])(int px, int py);
void next_level();
void restart_level();

#endif