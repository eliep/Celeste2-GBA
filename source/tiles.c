#include "tiles.h"

#include <gba_systemcalls.h>
#include <gba_video.h>

#include "spritesheet_other_png_bin.h"
#include "spritesheet_clouds_holed0_png_bin.h"
#include "spritesheet_clouds_holed0_extra_png_bin.h"
#include "spritesheet_clouds_normal0_png_bin.h"
#include "spritesheet_clouds_normal0_extra_png_bin.h"
#include "spritesheet_font_png_bin.h"
#include "spritesheet_clouds_holed1_png_bin.h"
#include "spritesheet_clouds_holed1_extra_png_bin.h"
#include "spritesheet_clouds_normal1_png_bin.h"
#include "spritesheet_clouds_normal1_extra_png_bin.h"
#include "tileset_png_bin.h"


static TileDataType current_cloud_type = Spritesheet_other; // will be initialized on goto_level(0)

TileDataType get_current_cloud_type(){
    return current_cloud_type;
}

void load_tiledata(TileDataType type){
    // There are 6 char blocks. 4 for backgrounds, 2 for sprites. They are used for tiles.
    // A char block may instead be split into 8 screen blocks.
    // Here char block 0 is used for background tiles and blocks 4 and 5 are fully used for 
    // sprite tiles.

    // Every row is 32 (bytes) * 32 (tiles per line) = 1024 = 0x400 wide.

    switch(type){
    case TileSet:
        CpuFastSet(tileset_png_bin, CHAR_BASE_BLOCK(0), tileset_png_bin_size/4);
        break;
    case Spritesheet_other:
        CpuFastSet(spritesheet_other_png_bin, CHAR_BASE_BLOCK(4), spritesheet_other_png_bin_size/4);
        break;
    case Spritesheet_clouds_normal:
        if(current_cloud_type == Spritesheet_clouds_normal)
            return;
        CpuFastSet(spritesheet_clouds_normal0_png_bin, CHAR_BASE_BLOCK(4)+0x2800, spritesheet_clouds_normal0_png_bin_size/4);
        for(int i = 0; i < 4; i++)
            CpuFastSet(spritesheet_clouds_normal0_extra_png_bin+i*0x100, CHAR_BASE_BLOCK(4)+0x2300+i*0x400, spritesheet_clouds_normal0_extra_png_bin_size/4/4);
        CpuFastSet(spritesheet_clouds_normal1_png_bin, CHAR_BASE_BLOCK(4)+0x5800, spritesheet_clouds_normal1_png_bin_size/4);
        for(int i = 0; i < 4; i++)
            CpuFastSet(spritesheet_clouds_normal1_extra_png_bin+i*0x100, CHAR_BASE_BLOCK(4)+0x5300+i*0x400, spritesheet_clouds_normal1_extra_png_bin_size/4/4);
        current_cloud_type = Spritesheet_clouds_normal;
        break;
    case Spritesheet_clouds_holed:
        if(current_cloud_type == Spritesheet_clouds_holed)
            return;
        CpuFastSet(spritesheet_clouds_holed0_png_bin, CHAR_BASE_BLOCK(4)+0x2800, spritesheet_clouds_holed0_png_bin_size/4);
        for(int i = 0; i < 4; i++)
            CpuFastSet(spritesheet_clouds_holed0_extra_png_bin+i*0x100, CHAR_BASE_BLOCK(4)+0x2300+i*0x400, spritesheet_clouds_holed0_extra_png_bin_size/4/4);
        CpuFastSet(spritesheet_clouds_holed1_png_bin, CHAR_BASE_BLOCK(4)+0x5800, spritesheet_clouds_holed1_png_bin_size/4);
        for(int i = 0; i < 4; i++)
            CpuFastSet(spritesheet_clouds_holed1_extra_png_bin+i*0x100, CHAR_BASE_BLOCK(4)+0x5300+i*0x400, spritesheet_clouds_holed1_extra_png_bin_size/4/4);
        current_cloud_type = Spritesheet_clouds_holed;
        break;
    case Spritesheet_font:
        CpuFastSet(spritesheet_font_png_bin, CHAR_BASE_BLOCK(4)+0x5000, spritesheet_font_png_bin_size/4);
        break;
    }
}