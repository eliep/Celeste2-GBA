from PIL import Image
import os
import sys

# make sure both images are multiples of 8x8
# tileset must have a max width of 32 tiles (256 pixels)
# this script uses always uses the first matching tile, that it finds (going rowwise left to right)
IMG_TILESET = "tileset.png"
IMG_MAP = None#"map0.png" # this is an example. This is given as an argument to the script
# Set to None and enable print down below for detection of tiles, that are not on the tileset
# This script only executes successfully, if this is set to a position
EMPTY_TILE_OBJECT = (22,0)
# set the bg palette for specific regions on the tileset
def GET_PALETTE(tile_pos_on_tileset):
    """
    tile_pos_on_tileset: position tuple (in tiles)
    """
    x,y,flip_v,flip_h = tile_pos_on_tileset
    # ground palette
    if x >= 7 and x <= 8 and y <= 3:
        return 3
    if x == 9 and y <= 2:
        return 3
    if x >=10 and x <= 11 and y == 0:
        return 3
    if x == 10 and y == 2:
        return 3
    if x == 18 and y == 2:
        return 3
    # default/normal palette
    return 0


def load_image(filename):
    try:
        img = Image.open(filename)
        img.load()
        img = img.convert("RGB")
        return img
    except Exception as e:
        print(f"Could not open \"{filename}\"")
        return None

def search_tile_on_tileset(img_map, img_tileset, tile_pos_on_map):
    """
    img_map: Image
    img_tileset: Image
    tile_pos_on_tileset: position tuple (in ########)
    """
    width, height = img_tileset.size

    for row in range(height//8): # counting in 8x8 pixel blocks on tileset
        for column in range(width//8):
            # check for flipped versions of the tile
            for flip_v in [False, True]:
                for flip_h in [False, True]:
                    same = True
                    # check if every pixel of map_tile and current tileset_tile is the same
                    for row_8x8 in range(8):
                        for column_8x8 in range(8):
                            pixel_position = 8*column+column_8x8, 8*row+row_8x8
                            # use flipped pixel position
                            if flip_v:
                                pixel_position = [pixel_position[0], 8*row+7-row_8x8]
                            if flip_h:
                                pixel_position = [8*column+7-column_8x8, pixel_position[1]]
                            
                            rgb_tileset = img_tileset.getpixel(pixel_position)
                            pixel_position = 8*tile_pos_on_map[0]+column_8x8, 8*tile_pos_on_map[1]+row_8x8
                            rgb_map = img_map.getpixel(pixel_position)
                            if rgb_tileset != rgb_map:
                                same = False
                                break
                        if not same:
                            break
                    if same:
                        return column,row,flip_v,flip_h
    return None

def convert_map(img_map, img_tileset):
    """
    img_map: Image
    img_tileset: Image
    """
    width, height = img_map.size
    tile_positions_on_tileset = []
    for row in range(height//8):
        row_tile_positions = []
        for column in range(width//8):
            pos = search_tile_on_tileset(img_map, img_tileset, (column, row))
            if pos is None:
                print(f"Couldn't find {(column, row)} in tileset.")
                row_tile_positions.append((*EMPTY_TILE_OBJECT,False,False))
            else:
                row_tile_positions.append(pos)
        tile_positions_on_tileset.append(row_tile_positions)
    return tile_positions_on_tileset

def tile_positions_to_bytes(tile_positions_on_tileset, tileset_width):
    """
    tile_positions_on_tileset: two-dimensional list of tuple positions (in tiles)
    tileset_width: integer (in tiles)
    """
    tile_bytes = []
    for row in tile_positions_on_tileset:
        for tile_pos in row:
            palette = GET_PALETTE(tile_pos)
            tile_id = tile_pos[1]*32 + tile_pos[0]

            byte0 = tile_id & 0xFF
            byte1 = ((tile_id & 0x300)) | (palette << 4)

            if(tile_pos[2]): # vertical flip
                byte1 = byte1|(1<<3)
            if(tile_pos[3]): # horizontal flip
                byte1 = byte1|(1<<2)
            
            if(tile_pos[0] == 1 and tile_pos[1] == 0):
                print(byte0,byte1)

            tile_bytes.append(byte0)
            tile_bytes.append(byte1)
    return tile_bytes

if(len(sys.argv) !=2):
    print("Usage: png2mapdata.py MAP_FILENAME.png")
    exit()
else:
    IMG_MAP = sys.argv[1]


img_tileset = load_image(IMG_TILESET)
if img_tileset is None:
    exit()
img_map = load_image(IMG_MAP)
if img_map is None:
    exit()

tile_positions = convert_map(img_map, img_tileset)
"""for i, el in enumerate(tile_positions):
    print(i)
    print(el)
    print()
"""
tile_bytes = tile_positions_to_bytes(tile_positions, img_tileset.size[0]//8)

output_file_name = IMG_MAP+".bin"
if len(tile_bytes)%4 != 0:
    print("Bytecount is not a multple of 4!")

with open(output_file_name, "wb") as output:
    output.write(bytes(tile_bytes))

    cname = output_file_name.replace('.','_')
    print(f"Size of {output_file_name} is {len(tile_bytes)} bytes")
    print()
    print("Usage help:\n")
    print(f"#include \"{cname+'.h'}\"")
