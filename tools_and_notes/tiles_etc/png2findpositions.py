from PIL import Image
import os
import sys

# make sure both images are multiples of 8x8
# tileset must have a max width of 32 tiles (256 pixels)
# this script uses always uses the first matching tile, that it finds (going rowwise left to right)
IMG_TILESET = "tileset.png"
IMG_MAP = "map1.png"

EMPTY_TILE_OBJECT = (22,0)

# Defines whether this tile position should be output
def CHECK_IF_SEARCHED(tile_pos_on_tileset):
    """
    tile_pos_on_tileset: position tuple (in tiles)
    """
    x,y,flip_v,flip_h = tile_pos_on_tileset
    # grappler (10,1)
    # crumble (21,0)
    # berry on spritesheet (20,5)
    if x==20 and y==5:
        return True
    return False

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
                row_tile_positions.append((*EMPTY_TILE_OBJECT,False,False))
            else:
                row_tile_positions.append(pos)
        tile_positions_on_tileset.append(row_tile_positions)
    return tile_positions_on_tileset

def tile_positions_to_prints(tile_positions_on_tileset):
    """
    tile_positions_on_tileset: two-dimensional list of tuple positions (in tiles)
    """
    for y, row in enumerate(tile_positions_on_tileset):
        for x, tile_pos in enumerate(row):
            if CHECK_IF_SEARCHED(tile_pos):
                print(f"({x},{y})")

if(len(sys.argv) !=2):
    print("Usage: png2findpositions.py MAP_FILENAME.png")
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
tile_positions_to_prints(tile_positions)
