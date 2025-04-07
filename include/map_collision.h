#ifndef MAP_COLLISION_H
#define MAP_COLLISION_H

#include <gba_types.h>

bool tile_at_is_collider(int x, int y, int level_index);
bool tile_at_is_grabbable(int x, int y, int level_index);

#endif