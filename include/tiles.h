#ifndef TILES_H
#define TILES_H

#define TILE_COORDINATES(x,y) ((y)*32+(x))

typedef enum TileDataType{
    TileSet,
    Spritesheet_other,
    Spritesheet_font,
    Spritesheet_clouds_normal,
    Spritesheet_clouds_holed
} TileDataType;

//TileIDs of certain tiles on the tileset
typedef enum TileID{
    Tile_Empty = TILE_COORDINATES(22, 0),
    Tile_SolidColor = TILE_COORDINATES(21,2),
    Tile_Column00 = TILE_COORDINATES(19,0),
    Tile_Column01 = TILE_COORDINATES(19+1,0),
    Tile_Column10 = TILE_COORDINATES(18,1),
    Tile_Column11 = TILE_COORDINATES(19+1,1),
    Tile_Column20 = TILE_COORDINATES(19,2),
    Tile_Column21 = TILE_COORDINATES(19+1,2),
    Tile_Column30 = TILE_COORDINATES(19,3),
    Tile_Column31 = TILE_COORDINATES(19+1,3),
    Tile_Crumble_Normal = TILE_COORDINATES(21,0),
    Tile_Crumble_Breaking = TILE_COORDINATES(21,1),
    // Checkpoint is made of 2 tiles. This is the upper tile of one animation frame. 
    // Lower tile is 1 row below this one.
    // Next animation frames are the next 14 columns, i.e. columns 0-14.
    Tile_Checkpoint_Animation0 = TILE_COORDINATES(0,4),
    Tile_Checkpoint_Normal = TILE_COORDINATES(18,0)
} TileID;

TileDataType get_current_cloud_type();
void load_tiledata(TileDataType type);

#endif