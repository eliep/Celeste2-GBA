#include "maps.h"

#include <gba_systemcalls.h>
#include <gba_video.h>

#include "columns_png_bin.h"
#include "map0_png_bin.h"
#include "map1_png_bin.h"
#include "map2_png_bin.h"
#include "map3_png_bin.h"
#include "map4_png_bin.h"
#include "map5_png_bin.h"
#include "map6_png_bin.h"
#include "map7_png_bin.h"
#include "map8_png_bin.h"

#include "pico8_math.h"
#include "tiles.h"
//#include "mgba.h"


static const uint8_t* get_map_bytes(int index);
static void clear_map();
static void scroll_horizontal(s16 camera_x, s16 camera_y);
static void scroll_vertical(s16 camera_x, s16 camera_y);


// leveldata
const Level levels[9] = {
    { // Level 0 Titlescreen
        .width = 16,
        .height = 16,
        .camera_barrier_x = 0,
        .camera_barrier_y = 0,
        .music = 38,
        .fogmode = 0,
        .clouds = 0,
        .columns = 0,
        .pal_mode = 0,
        .bg = 0,
        .title = "",
        .title_len = 0,
        .right_edge = false
    },
    { // Level 1
        .width = 96,
        .height = 16,
        .camera_barrier_x = 0,
        .camera_barrier_y = 0,
        .music = 38,
        .fogmode = 0,
        .clouds = 13,
        .columns = 0,
        .pal_mode = 0,
        .bg = 0,
        .title = "",
        .title_len = 0,
        .right_edge = false
    },
    { // Level 2
        .width = 32,
        .height = 32,
        .camera_barrier_x = 0,
        .camera_barrier_y = 0,
        .music = 36,
        .fogmode = 1,
        .clouds = 0,
        .columns = 1,
        .pal_mode = 0,
        .bg = 0,
        .title = "",
        .title_len = 0,
        .right_edge = false
    },
    { // Level 3
        .width = 128,
        .height = 22,
        .camera_barrier_x = 38,
        .camera_barrier_y = 6,
        .music = 2,
        .fogmode = 0,
        .clouds = 13,
        .columns = 0,
        .pal_mode = 0,
        .bg = 0,
        .title = "trailhead",
        .title_len = 9,
        .right_edge = false
    },
    { // Level 4
        .width = 128,
        .height = 32,
        .camera_barrier_x = 0,
        .camera_barrier_y = 0,
        .music = 2,
        .fogmode = 0,
        .clouds = 13,
        .columns = 1,
        .pal_mode = 1,
        .bg = 0,
        .title = "glacial caves",
        .title_len = 13,
        .right_edge = false
    },
    { // Level 5
        .width = 128,
        .height = 16,
        .camera_barrier_x = 0,
        .camera_barrier_y = 0,
        .music = 2,
        .fogmode = 2,
        .clouds = 15,
        .columns = 0,
        .pal_mode = 2,
        .bg = 13,
        .title = "golden valley",
        .title_len = 13,
        .right_edge = false
    },
    { // Level 6
        .width = 128,
        .height = 16,
        .camera_barrier_x = 105,
        .camera_barrier_y = 0,
        .music = 2,
        .fogmode = 2,
        .clouds = 15,
        .columns = 0,
        .pal_mode = 2,
        .bg = 13,
        .title = "",
        .title_len = 0,
        .right_edge = false
    },
    { // Level 7
        .width = 128,
        .height = 16,
        .camera_barrier_x = 0,
        .camera_barrier_y = 0,
        .music = 2,
        .fogmode = 2,
        .clouds = 7,
        .columns = 0,
        .pal_mode = 1,
        .bg = 13,
        .title = "",
        .title_len = 0,
        .right_edge = false
    },
    { // Level 8
        .width = 16,
        .height = 62,
        .camera_barrier_x = 0,
        .camera_barrier_y = 0,
        .music = 2,
        .fogmode = 2,
        .clouds = 7,
        .columns = 0,
        .pal_mode = 3,
        .bg = 15,
        .title = "destination",
        .title_len = 11,
        .right_edge = true
    }
};

// background management
#define BG_MAP_SIZE 256
#define CAMERA_SIZE 128

static int current_level = 0;
// Always a multiple of 8 pixels. Represents the first pixel of the first loaded tile column/row on the map.
static s16 seam_marker = -BG_MAP_SIZE;
void (*update_map)(s16, s16) = &scroll_horizontal;

static const uint8_t* get_map_bytes(int index){
    switch(index){
    case 0: return map0_png_bin;
    case 1: return map1_png_bin;
    case 2: return map2_png_bin;
    case 3: return map3_png_bin;
    case 4: return map4_png_bin;
    case 5: return map5_png_bin;
    case 6: return map6_png_bin;
    case 7: return map7_png_bin;
    case 8: return map8_png_bin;
    }
    return NULL;
}

void init_backgrounds(){
    // As char block 0 is occupied by tile data, screen blocks for backgrounds start from index 8.
    // => Screen blocks 8 to 11 are used for background map data.

    // BG0 - black overlay
    vu32 c = ((CHAR_PALETTE(1)|Tile_SolidColor)<<16) | (CHAR_PALETTE(1)|Tile_SolidColor);
    // fill all of the background, so shake effect still looks nice
    CpuFastSet((void*)&c, SCREEN_BASE_BLOCK(12), 512|FILL);

    // middle hole
    set_overlay(0,false);

    // BG1 - tiles
    clear_map();

    // BG2 - background columns
    // The columns move, but the pixel holes stay at the same places.
    // -> use 4 screen blocks for all possible hole-positions.
    for(int row = 0; row < 32; row++){
        CpuFastSet(columns_png_bin, SCREEN_BASE_BLOCK(8)+row*64, 32/2);
        CpuFastSet(columns_png_bin+64, SCREEN_BASE_BLOCK(9)+row*64, 32/2);
        CpuFastSet(columns_png_bin+128, SCREEN_BASE_BLOCK(10)+row*64, 32/2);
        CpuFastSet(columns_png_bin+192, SCREEN_BASE_BLOCK(11)+row*64, 32/2);
    }
    
    // BG3 - background color
    c = ((CHAR_PALETTE(2)|Tile_SolidColor)<<16) | (CHAR_PALETTE(2)|Tile_SolidColor);
    CpuFastSet((void*)&c, SCREEN_BASE_BLOCK(14), 512|FILL);
}

void set_overlay(unsigned int column_count, bool fill_left){
    // screenwipe black overlay. Doesn't account for offsets between screenwipes
    // 0 <= columncount <= 16

    vu32 c = ((CHAR_PALETTE(0)|Tile_Empty)<<16)|(CHAR_PALETTE(0)|Tile_Empty);
    int left_bound = 7;
    if(fill_left){
        for(int row = 2; row <= 17; row++){
            CpuSet((void*)&c, SCREEN_BASE_BLOCK(12)+row*32*2+(7+column_count)*2, (16-column_count)|FILL|COPY16);
        }
    }else{
        left_bound = 22+1 - column_count;
        for(int row = 2; row <= 17; row++){
            CpuSet((void*)&c, SCREEN_BASE_BLOCK(12)+row*32*2+(7)*2, (16-column_count)|FILL|COPY16);
        }
    }

    c = ((CHAR_PALETTE(1)|Tile_SolidColor)<<16) | (CHAR_PALETTE(1)|Tile_SolidColor);
    for(int row = 2; row <= 17; row++){
        CpuSet((void*)&c, SCREEN_BASE_BLOCK(12)+row*32*2+left_bound*2, column_count|FILL|COPY16);
    }
}

void set_overlay_infade(int start_pixel){
    // used together with screenwipe sprites
    // start_pixel is the left most pixel, of the left most screenwipe sprite.
    int start_tile = start_pixel/8;
    if(start_pixel < 0)
        start_tile--;

    vu32 c = ((CHAR_PALETTE(0)|Tile_Empty)<<16)|(CHAR_PALETTE(0)|Tile_Empty);
    for(int row = 0; row < 8; row++){
        CpuSet((void*)&c, (u16*)SCREEN_BASE_BLOCK(12)+7+2*32 + row*32, (min2(max(start_tile+7+2), 16))|FILL|COPY16);
    }
    for(int row = 8; row < 16; row++){
        CpuSet((void*)&c, (u16*)SCREEN_BASE_BLOCK(12)+7+2*32 + row*32, (min2(max(start_tile+7), 16))|FILL|COPY16);
    }

    c = ((CHAR_PALETTE(1)|Tile_SolidColor)<<16) | (CHAR_PALETTE(1)|Tile_SolidColor);
    for(int row = 0; row < 8; row++){
        CpuSet((void*)&c, (u16*)SCREEN_BASE_BLOCK(12)+7+2*32 + row*32 + max(start_tile+9),
            (max(16 - start_tile - 9))|FILL|COPY16);
    }
    for(int row = 8; row < 16; row++){
        CpuSet((void*)&c, (u16*)SCREEN_BASE_BLOCK(12)+7+2*32 + row*32 + max(start_tile+9-2), 
            (max(16 - start_tile - 9+2))|FILL|COPY16);
    }
}

void set_overlay_outfade(int start_pixel){
    // copied from above and slightly modified
    int start_tile = start_pixel/8;
    if(start_pixel < 0)
        start_tile--;
    start_tile -=4;

    vu32 c = ((CHAR_PALETTE(1)|Tile_SolidColor)<<16)|(CHAR_PALETTE(1)|Tile_SolidColor);
    for(int row = 0; row < 8; row++){
        CpuSet((void*)&c, (u16*)SCREEN_BASE_BLOCK(12)+7+2*32 + row*32, (min2(max(start_tile+7+2), 16))|FILL|COPY16);
    }
    for(int row = 8; row < 16; row++){
        CpuSet((void*)&c, (u16*)SCREEN_BASE_BLOCK(12)+7+2*32 + row*32, (min2(max(start_tile+7), 16))|FILL|COPY16);
    }

    c = ((CHAR_PALETTE(0)|Tile_Empty)<<16) | (CHAR_PALETTE(0)|Tile_Empty);
    for(int row = 0; row < 8; row++){
        CpuSet((void*)&c, (u16*)SCREEN_BASE_BLOCK(12)+7+2*32 + row*32 + max(start_tile+9),
            (min2(max(16 - start_tile - 9), 16))|FILL|COPY16);
    }
    for(int row = 8; row < 16; row++){
        CpuSet((void*)&c, (u16*)SCREEN_BASE_BLOCK(12)+7+2*32 + row*32 + max(start_tile+9-2), 
            (min2(max(16 - start_tile - 9+2), 16))|FILL|COPY16);
    }
}

static void clear_map(){
    vu32 c = ((CHAR_PALETTE(0)|Tile_Empty)<<16)|(CHAR_PALETTE(0)|Tile_Empty);
    // c is 32 bits. Size is how many 32bit-blocks should be filled. A tile is 16 bits.
    // => Set size as #tiles/2. Must be a multiple of 8.
    CpuFastSet((void*)&c, SCREEN_BASE_BLOCK(13),(32*32/2)|FILL);
}

void load_level_map(int index){
	clear_map();
	current_level = index;

	// set scroll mode
	// only one scroll direction is needed, so I implemented them seperately, 
	// as that was easier on my head.
	if(index == 8)
		update_map = &scroll_vertical;
	else
		update_map = &scroll_horizontal;

	// go off-screen, so that the map gets reloaded.
	seam_marker = -BG_MAP_SIZE;
}

static void scroll_horizontal(s16 camera_x, s16 camera_y){
	// Check if screen is in bounds of currently loaded map. 
	// Currently loaded map space goes from seam_marker to seam_marker+BG_MAP_SIZE-1.
	// If the left side of the screen (+2tiles padding) is on the right of seam_marker, 
	// then there are no tiles that need to be loaded. The padding is used, so that when 
	// the screen is scrolled, the tiles are loaded early enough.
	// If the left or right map edge is currently loaded and the screen padding extends 
	// outside of the map space, then the screen is still cosidered in-bounds.
	if(seam_marker <= camera_x-2*8 || (seam_marker == 0 && camera_x-2*8 < 0)){
		// inside left bound, check right bound
		if(camera_x+CAMERA_SIZE+2*8 <= seam_marker+BG_MAP_SIZE || (seam_marker+BG_MAP_SIZE == levels[current_level].width*8 && camera_x+CAMERA_SIZE+2*8 > levels[current_level].width*8)){
			// inside both bounds. No need to update tiles.
			return;
		}else{
			// out of bounds right
			// load tiles right to left, from camera_x+CAMERA_SIZE+2tiles until seam or next 32 tiles
			int load_start = (camera_x+CAMERA_SIZE)/8+2;
			if(load_start >= levels[current_level].width) // clip to right map edge
				load_start = levels[current_level].width-1;
			for(int column = load_start; column >= (seam_marker+BG_MAP_SIZE)/8 && column >= load_start-BG_MAP_SIZE/8+1; column--){
				// load column
				for(int row = 0; row < min2(levels[current_level].height, BG_MAP_SIZE/8); row++){
					((u16*)SCREEN_BASE_BLOCK(13))[row*32 + (column%32)] = ((u16*)get_map_bytes(current_level))[row*levels[current_level].width + column];
				}
			}
			//update seam_marker
			seam_marker = load_start*8-BG_MAP_SIZE+1*8;
		}
	}else{
		// out of bounds left
		// load tiles left to right, from camera_x-2tiles until seam or next 32 tiles 
		int load_start = camera_x/8-2;
		if(load_start < 0) // clip to left map edge
			load_start = 0;
		for(int column = load_start; column < seam_marker/8 && column < load_start+BG_MAP_SIZE/8; column++){
			// load column
			for(int row = 0; row < min2(levels[current_level].height, BG_MAP_SIZE/8); row++)
				((u16*)SCREEN_BASE_BLOCK(13))[row*32 + (column%32)] = ((u16*)get_map_bytes(current_level))[row*levels[current_level].width + column];
		}
		//update seam_marker
		seam_marker = load_start*8;
	}
}

static void scroll_vertical(s16 camera_x, s16 camera_y){
	if(seam_marker <= camera_y-2*8 || (seam_marker == 0 && camera_y-2*8 < 0)){
		// inside upper bound, check bottom bound
		if(camera_y+CAMERA_SIZE+2*8 <= seam_marker+BG_MAP_SIZE || (seam_marker+BG_MAP_SIZE == levels[current_level].height*8 && camera_x+CAMERA_SIZE+2*8 > levels[current_level].height*8)){
			// inside both bounds. No need to update tiles.
			return;
		}else{
			// out of bounds bottom
			// load tiles right to left, from camera_y+CAMERA_SIZE+2tiles until seam or next 32 tiles
			int load_start = (camera_y+CAMERA_SIZE)/8+2;
			if(load_start >= levels[current_level].height) // clip to right map edge
				load_start = levels[current_level].height-1;
			for(int row = load_start; row >= (seam_marker+BG_MAP_SIZE)/8 && row >= load_start-BG_MAP_SIZE/8+1; row--){
				// load row
				for(int column = 0; column < min2(levels[current_level].width, BG_MAP_SIZE/8); column++){
					((u16*)SCREEN_BASE_BLOCK(13))[(row%32)*32 + (column)] = ((u16*)get_map_bytes(current_level))[row*levels[current_level].width + column];
				}
			}
			//update seam_marker
			seam_marker = load_start*8-BG_MAP_SIZE+1*8;
		}
	}else{
		// out of bounds up
		// load tiles left to right, from camera_y-2tiles until seam or next 32 tiles 
		int load_start = camera_y/8-2;
		if(load_start < 0) // clip to left map edge
			load_start = 0;
		for(int row = load_start; row < seam_marker/8 && row < load_start+BG_MAP_SIZE/8; row++){
			// load row
			for(int column = 0; column < min2(levels[current_level].width, BG_MAP_SIZE/8); column++)
				((u16*)SCREEN_BASE_BLOCK(13))[(row%32)*32 + (column)] = ((u16*)get_map_bytes(current_level))[row*levels[current_level].width + column];
		}
		//update seam_marker
		seam_marker = load_start*8;
	}
}

static int is_columns_enabled = false;
void set_columns_enabled(int enabled){
    is_columns_enabled = enabled;
    if(enabled)
        SetMode(MODE_0 | OBJ_ENABLE | BG_ALL_ON);
    else
        SetMode(MODE_0 | OBJ_ENABLE | BG0_ON | BG1_ON | BG3_ON);
}

int get_columns_enabled(){
    return is_columns_enabled;
}