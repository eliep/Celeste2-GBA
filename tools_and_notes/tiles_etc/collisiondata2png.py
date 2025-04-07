from PIL import Image
import numpy as np
import sys

COLLISION_FILE = "map4.png.collision.bin"
LEVEL_WIDTH = 128
LEVEL_HEIGHT = 32

def bin_data_to_image(binary_data, width):
    """bits = []
    for y in range(LEVEL_HEIGHT):
        for x in range(LEVEL_WIDTH):
            d = binary_data[(LEVEL_WIDTH*y+x)//8]
            d = d&(1<<(x&0x7))
            if d>0:
                bits.append(1)
            else:
                bits.append(0)"""
    bits = [int(bit) for byte in binary_data for bit in f'{byte:08b}'[::-1]]

    height = (len(bits) + width - 1) // width
    
    bits += [0] * (width * height - len(bits))

    image_data = np.array(bits).reshape((height, width))

    img_array = np.zeros((height, width, 3), dtype=np.uint8)
    
    img_array[image_data == 0] = [0, 0, 0]
    img_array[image_data == 1] = [255, 0, 0]

    img = Image.fromarray(img_array, 'RGB')
    return img

if(len(sys.argv) < 3):
    print("Usage: collisiondata2png.py COLLISION_DATA.bin LEVEL_WIDTH <LEVEL_HEIGHT>")
    exit()
else:
    COLLISION_FILE = sys.argv[1]
    LEVEL_WIDTH = int(sys.argv[2])
    if len(sys.argv) >= 4:
        LEVEL_HEIGHT = int(sys.argv[3])

with open(COLLISION_FILE,"rb") as file:
    data = file.read()
    image = bin_data_to_image(data, LEVEL_WIDTH)
    image.save(f"{COLLISION_FILE}.png") 