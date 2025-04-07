from PIL import Image
import os
import sys

# If False, the data will be split into multiple files (1 for every tile row)
ONE_BIG_FILE=True
# if True, every row will be padded, to that it contains 32 tiles (GBA width).
# (Actually just increases file size) Maybe useful if ONE_BIG_FILE is True.
PADDING = False  
# Sorted RGB values.
PALETTE = [ 
    (0x00,0x00,0x00),   # black
    (0x1D,0x2B,0x53),   # dark-blue
    (0x7E,0x25,0x53),   # dark-purple
    (0x00,0x87,0x51),   # dark-green
    (0xAB,0x52,0x36),   # brown
    (0x5F,0x57,0x4F),   # dark-gray
    (0xC2,0xC3,0xC7),   # light-gray
    (0xFF,0xF1,0xE8),   # white
    (0xFF,0x00,0x4D),   # red
    (0xFF,0xA3,0x00),   # orange
    (0xFF,0xEC,0x27),   # yellow
    (0x00,0xE4,0x36),   # green
    (0x29,0xAD,0xFF),   # blue
    (0x83,0x76,0x9C),   # lavender
    (0xFF,0x77,0xA8),   # pink
    (0xFF,0xCC,0xAA)    # light-peach
]

def load_image(filename):
    try:
        img = Image.open(filename)
        img.load()
        img = img.convert("RGB")
        return img
    except Exception as e:
        print(f"Could not open \"{filename}\"")
        return None

def convert_image(img, padding=False):
    width, height = img.size
    if width%8 != 0 or height%8 != 0:
        print(f"image sizes {img.size} are not a multiple of 8. Leftover pixels on the right or bottom will be cut off.")

    img_bytes = [] # stores palette ids for every row
    for row in range(height//8): # counting in 8x8 pixel blocks
        for column in range(width//8):

            for row_8x8 in range(8): # 2 pixels per byte
                for column_8x8 in range(4):
                    try:
                        pixel_position = 8*column+2*column_8x8, 8*row+row_8x8
                        rgb = img.getpixel(pixel_position)
                        palette_id0 = PALETTE.index(rgb)

                        pixel_position = 8*column+2*column_8x8 + 1, 8*row+row_8x8
                        rgb = img.getpixel(pixel_position)
                        palette_id1 = PALETTE.index(rgb)
                    except Exception as e:
                        print(f"Color {rgb} at {pixel_position} or is not in PALETTE.")
                        print(e)
                        return None

                    num = (palette_id0 | (palette_id1<<4)) & 0xFF
                    img_bytes.append(num)
        if padding:
            for i in range(32-width//8):
                for j in range(32):
                    img_bytes.append(0)
    return img_bytes

IMG_TILESET = "tileset.png"
if len(sys.argv) != 2:
    print("Usage: png2tiledata.py TILESET_FILENAME.png")
    exit()
else:
    IMG_TILESET = sys.argv[1]

img = load_image(IMG_TILESET)
if img is None:
    exit()
img_bytes = convert_image(img, PADDING)
if img_bytes is None:
    exit()
#print([hex(x) for x in img_bytes])

if ONE_BIG_FILE:
    output_file_name = IMG_TILESET+".bin" # os.path.splitext(IMG_TILESET)[0]+".bin"
    if len(img_bytes)%4 != 0:
        print("Bytecount is not a multple of 4!")

    with open(output_file_name, "wb") as output:
        output.write(bytes(img_bytes))

    cname = output_file_name.replace('.','_')
    print(f"Size of {output_file_name} is {len(img_bytes)} bytes")
    print()
    print("Usage help:\n")
    print(f"#include \"{cname+'.h'}\"")
    print(f"CpuFastSet({cname}, CHAR_BASE_BLOCK(4), {cname+'_size'}/4);")
else:
    width, height = img.size
    width = width//8
    height = height//8
    if PADDING:
        width = 32
        
    if (len(img_bytes)//height)%4 != 0:
        print("Bytecounts are not multples of 4!")
    for row in range(height):
        output_file_name = IMG_TILESET+f"{row}.bin"
        row_bytes = img_bytes[32*width*row : 32*width*(row+1)]
        with open(output_file_name, "wb") as output:
            output.write(bytes(row_bytes))
        
        print(f"Size of {output_file_name} is {len(row_bytes)} bytes")
    print(f"=> {len(img_bytes)} bytes")
    cname = IMG_TILESET.replace('.','_')
    print()
    print("Usage help:\n")
    for row in range(height):
        print(f"#include \"{cname+f'{row}_bin.h'}\"")
    print()
    for row in range(height):
        if row == 0:
            print(f"CpuFastSet({cname+f'{row}_bin'}, CHAR_BASE_BLOCK(4), {cname+f'{row}_bin_size'}/4);")
        else:
            print(f"CpuFastSet({cname+f'{row}_bin'}, CHAR_BASE_BLOCK(4)+{hex(32*32*row)}, {cname+f'{row}_bin_size'}/4);")