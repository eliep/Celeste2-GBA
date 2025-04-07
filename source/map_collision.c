#include "map_collision.h"

#include "map1_png_collision_bin.h"
#include "map2_png_collision_bin.h"
#include "map3_png_collision_bin.h"
#include "map4_png_collision_bin.h"
#include "map5_png_collision_bin.h"
#include "map6_png_collision_bin.h"
#include "map7_png_collision_bin.h"
#include "map8_png_collision_bin.h"

#include "map1_png_ice_bin.h"
//#include "map2_png_ice_bin.h"
//#include "map3_png_ice_bin.h"
#include "map4_png_ice_bin.h"
#include "map5_png_ice_bin.h"
#include "map6_png_ice_bin.h"
#include "map7_png_ice_bin.h"
//#include "map8_png_ice_bin.h"

#include "maps.h"

// x and y should be positive, level_index >= 1
// should be tile coordinates
// flag 1
bool tile_at_is_collider(int x, int y, int level_index){
    if(x < 0 || x >= levels[level_index].width || y < 0 || y >= levels[level_index].height)
        return false;

    const uint8_t* collision_data;
    switch (level_index)
    {
    case 1: collision_data = map1_png_collision_bin; break;
    case 2: collision_data = map2_png_collision_bin; break;
    case 3: collision_data = map3_png_collision_bin; break;
    case 4: collision_data = map4_png_collision_bin; break;
    case 5: collision_data = map5_png_collision_bin; break;
    case 6: collision_data = map6_png_collision_bin; break;
    case 7: collision_data = map7_png_collision_bin; break;
    case 8: collision_data = map8_png_collision_bin; break;
    default: return false;
    }
    // every bit corresponds to 1 tile on the map, which may or may not be have collision.
    return 0 < (collision_data[levels[level_index].width * y/8 + x/8] & (1<<(x%8)));
}

// x and y should be positive, level_index >= 1 and != 2,3,8
// flag 2
bool tile_at_is_grabbable(int x, int y, int level_index){
    if(x < 0 || x >= levels[level_index].width || y < 0 || y >= levels[level_index].height)
        return false;
        
    const uint8_t* collision_data;
    switch (level_index)
    {
    case 1: collision_data = map1_png_ice_bin; break;
    //case 2: collision_data = map2_png_ice_bin; break;
    //case 3: collision_data = map3_png_ice_bin; break;
    case 4: collision_data = map4_png_ice_bin; break;
    case 5: collision_data = map5_png_ice_bin; break;
    case 6: collision_data = map6_png_ice_bin; break;
    case 7: collision_data = map7_png_ice_bin; break;
    //case 8: collision_data = map8_png_ice_bin; break;
    default: return false;
    }
    // every bit corresponds to 1 tile on the map, which may or may not be have collision.
    return 0 < (collision_data[levels[level_index].width * y/8 + x/8] & (1<<(x%8)));
}