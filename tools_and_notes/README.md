# Tools and Notes
This folder features material that may be useful if you want to understand how this game was built.

__celeste2_random_notes.txt__ contains random notes with which I tried to understand the general architecture of the game and planned how I wanted to port the features to the GBAs architecture.  
__celeste2_object_notes.txt__ contains notes only about the "objects" defined in the original source code.

---
__check_rom.py__ is used to grab and validate some Header info of the ROM.  
__calc/__ contains files with which I reverse engineered the actual pixel values of some graphical functions. There are also files with which I generated my lookup tables.  

---
I setup my maps and sprites as simple `.png` files and wrote the following scripts to convert them to relevant binary data, which then can be directly used on the GBA. Map files are always dependant on the tileset. The resulting `.bin` are then manually copied to `data/` in the root of this project.

__tiles_etc/png2tiledata.py__ is used to convert the spritesheet and tileset to tiledata, which can be loaded on the GBA.  
__tiles_etc/png2mapdata.py__ is used to convert the spritesheet and tileset to mapdata (arrangement of tiles), which can be loaded on the GBA.  
__tiles_etc/png2collisiondata.py__ is used to extract binary collision data from the maps, where every bit corresponds to a specific tile on a map. Tiles are marked on the tileset as having collision. The ice maps are also created with this.  
__tiles_etc/collisiondata2png.py__ is used to convert a binary collision map to an image to verify that the correct tiles were set as collidable.  
__tiles_etc/png2findpositions.py__ is used to get all positions of a specified tile on a map.

__tiles_etc/tiledata/__ contains all tiledata as images, which are convertable with the previous scripts.
__tiles_etc/tiledata/extra/__ contains files with my analysis of some graphical effects of the original game, which I used to recreate them on the GBA.  
__tiles_etc/map/__ contains the mapdata as full images of all maps.

---
The conversion of the extracted music files to maxmod compatible tracker files was pain, because the PICO8 does not output tracker files, but `.wav` files. I most likely did not do it optimally, but it's the only solution I found. I cut the songs into small samples and packaged them into trackers using OpenMPT.

__audio/music/__ contains the processed samples and the actual `.it` tracker files used in the game.
__audio/sounds/__ contains the processed sound effects as used in the game.  
__audio/help_mpt.py__ is a script, with which I bruteforced the best settings for OpenMPT for every music file. More specific info written in the file itself. I used this to create the tracker files.
