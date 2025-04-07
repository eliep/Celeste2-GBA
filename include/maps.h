#ifndef MAPS_H
#define MAPS_H

#include <gba_types.h>

// leveldata
typedef struct Level{
    int width,
        height,
        //camera_mode, // is same as level index
        camera_barrier_x,
        camera_barrier_y,
        music,
        fogmode,
        clouds,
        columns,
        // 0 - None
        // 1 - function() pal(2, 12) pal(5, 2) end
        // 2 - function() pal(2, 14) pal(5, 2) end
        // 3 - function() pal(2, 1) pal(7, 11) end
        pal_mode,
        bg;
    char title[13+1];
    unsigned int title_len;
    bool right_edge;
} Level;

extern const Level levels[9];

// background management
void init_backgrounds();

void set_overlay(unsigned int column_count, bool fill_left);
void set_overlay_infade(int start_pixel);
void set_overlay_outfade(int start_pixel);

void load_level_map(int index);
extern void (*update_map)(s16, s16);

void set_columns_enabled(int enabled);
int get_columns_enabled();

#endif