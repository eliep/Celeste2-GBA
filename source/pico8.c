#include "pico8.h"

#include <stdlib.h>
#include <gba_interrupt.h>
#include <gba_video.h>
#include <gba_systemcalls.h>
#include <maxmod.h>
#include "soundbank_bin.h"
#include "soundbank.h"

#include "maps.h"
#include "sprites.h"
#include "tiles.h"
#include "gameobjects_draw.h"
#include "main.h"


static mm_word songEventHandler(mm_word msg, mm_word param);
static void init_palettes();


static const u16 palette[16] = {
    RGB8(0x00, 0x00, 0x00),	// black
    RGB8(0x1D, 0x2B, 0x53), // dark-blue
    RGB8(0x7E, 0x25, 0x53), // dark-purple
    RGB8(0x00, 0x87, 0x51), // dark-green
    RGB8(0xAB, 0x52, 0x36), // brown
    RGB8(0x5F, 0x57, 0x4F), // dark-gray
    RGB8(0xC2, 0xC3, 0xC7), // light-gray
    RGB8(0xFF, 0xF1, 0xE8), // white
    RGB8(0xFF, 0x00, 0x4D), // red
    RGB8(0xFF, 0xA3, 0x00), // orange
    RGB8(0xFF, 0xEC, 0x27), // yellow
    RGB8(0x00, 0xE4, 0x36), // green
    RGB8(0x29, 0xAD, 0xFF), // blue
    RGB8(0x83, 0x76, 0x9C), // lavender
    RGB8(0xFF, 0x77, 0xA8), // pink
    RGB8(0xFF, 0xCC, 0xAA)  // light-peach
};

static s16 bg_offset_x = 0, bg_offset_y = 0;
static s16 shake_offset_x = 0, shake_offset_y = 0;

static u16 kHeld = 0;
static u16 kDown = 0;

static unsigned int frames_since_start = 0;
static FIXED_16 time_since_start = 0;

static u16 palette_buffer[16*16*2] = {0};

static OBJATTR obj_buffer[128] /*__attribute__((aligned(4)))*/ = {0};
static OBJAFFINE* obj_buffer_affine = (OBJAFFINE*)obj_buffer;
static int obj_count = 0; // count of currently displayed sprites
static int obj_affine_count = 0; // count of currently used affine sprites
static int _current_music = -1; // if -1, then uninitialized or song should have looped

void init_pico8(){
	// Set up the interrupt handlers
	irqInit();
	// Maxmod requires the vblank interrupt to reset sound DMA.
	// Link the VBlank interrupt to mmVBlank, and enable it. 
	irqSet(IRQ_VBLANK, mmVBlank);
	// Enable Vblank Interrupt to allow VblankIntrWait
	irqEnable(IRQ_VBLANK);
	// initialise maxmod with soundbank and 8 channels
    mmInitDefault((mm_addr)soundbank_bin, 8);
    mmSetModuleVolume(1024);
    mmSetEventHandler(songEventHandler);

    // Setup graphics
    init_palettes();

    load_tiledata(TileSet);
    load_tiledata(Spritesheet_other);
    load_tiledata(Spritesheet_font);
    load_tiledata(Spritesheet_clouds_normal);
    init_backgrounds();

    // Setup backgrounds
	REG_BG0CNT = SCREEN_BASE(12) | BG_16_COLOR | TEXTBG_SIZE_256x256 | CHAR_BASE(0) | BG_PRIORITY(0); // black overlay
	REG_BG1CNT = SCREEN_BASE(13) | BG_16_COLOR | TEXTBG_SIZE_256x256 | CHAR_BASE(0) | BG_PRIORITY(1); // tiles
	REG_BG2CNT = SCREEN_BASE( 8) | BG_16_COLOR | TEXTBG_SIZE_256x256 | CHAR_BASE(0) | BG_PRIORITY(2); // background columns
	REG_BG3CNT = SCREEN_BASE(14) | BG_16_COLOR | TEXTBG_SIZE_256x256 | CHAR_BASE(0) | BG_PRIORITY(3); // background color
	SetMode(MODE_0 | OBJ_ENABLE | BG0_ON | BG1_ON | BG3_ON); // 2D Sprite Mapping

    srand(177013); // my favourite prime ( ͡° ͜ʖ ͡°)
}

void update_keys(){
	scanKeys();
	kHeld = keysHeld();
	kDown = keysDown();
}

ARM_CODE void update_pico8(){
	// input
	update_keys();

	// don't update graphics or time when paused
	if(paused)
		return;

	frames_since_start++;
	if(frames_since_start >= 30*16) // might help with time precision // ?why? => divisible by 30 and won't overflow?
		frames_since_start = 0;
	time_since_start = int2Fixed(frames_since_start)/30;

	// map scroll
	update_map(bg_offset_x, bg_offset_y);
	REG_BG1HOFS = bg_offset_x-CAMERA_OFFSET_X +shake_offset_x; // screen wraps around by itself
	REG_BG1VOFS = bg_offset_y-CAMERA_OFFSET_Y +shake_offset_y;

	// background columns scroll
	if(get_columns_enabled()){
		REG_BG2CNT = SCREEN_BASE(8+((u16)bg_offset_x&0x3)) | BG_16_COLOR | TEXTBG_SIZE_256x256 | CHAR_BASE(0) | BG_PRIORITY(2);
		// mod 14 tiles, to allow simple wrapping. No need to load more column tiles.
		// Also bg_offset_x > 0 always.
		REG_BG2HOFS = ((bg_offset_x-bg_offset_x/10)%(14*8))-CAMERA_OFFSET_X +shake_offset_x;
		REG_BG2VOFS = -CAMERA_OFFSET_Y +shake_offset_y;
	}

	REG_BG0HOFS = shake_offset_x;
	REG_BG0VOFS = shake_offset_y;


	// update OAM

	/*char str[10];
	itoa(obj_count, str, 10);
	print(str, 10, 60+16, 14);*/

	for (int i = 0; i < 128-obj_count; i++){ // disable unused objects, mGBA OAM Viewer will still show those objects for whatever reason 
		obj_buffer[i].attr0 = OBJ_DISABLE;
		obj_buffer[i].attr1 = 0;
		obj_buffer[i].attr2 = OBJ_PRIORITY(0);
	}
	if(obj_count < 127)
		obj_buffer[127-obj_count].attr2 = ATTR0_WIDE|Sprite_OAM_Marker; // mark end of OAM
#ifdef DEBUG
	if(obj_count > 128) // Too many objects
		errorHalt("2much OAM");
#endif
	CpuFastSet(obj_buffer, OAM, 1024/4); // copy obj_buffer to OAM
	obj_count = 0;
	obj_affine_count = 0;

	// update palettes
	CpuFastSet(palette_buffer, BG_PALETTE, 1024/4); // BG_PALETTE is start of palette memory

	// update rendered sprites
	update_rendered_sprites();
}

void camera(s16 x, s16 y){
	bg_offset_x = x;
	bg_offset_y = y;
}

void get_camera(s16* camera_x, s16* camera_y){
	*camera_x = bg_offset_x;
	*camera_y = bg_offset_y;
}

s16 get_camera_x(){
	return bg_offset_x;
}
s16 get_camera_y(){
	return bg_offset_y;
}

void set_shake_offsets(s16 x, s16 y){
	shake_offset_x = x;
	shake_offset_y = y;
}
s16 get_shake_offset_x(){
	return shake_offset_x;
}
s16 get_shake_offset_y(){
	return shake_offset_y;
}

static void init_palettes(){ // palettes colors which won't change
	pal(0,0,PAL_RESET);
	for(int i = 1; i < 16; i++){
		palette_buffer[PAL_BG_ALL_BLACK + i] = RGB8(0, 0, 0);
		palette_buffer[PAL_SPRITES_ALL_BLACK + i] = RGB8(0, 0, 0);
		palette_buffer[PAL_SPRITES_LAVENDER_AS_BLACK + i] = palette[i];
	}
    palette_buffer[PAL_SPRITES_LAVENDER_AS_BLACK + 13] = RGB8(0, 0, 0);
    palette_buffer[PAL_SPRITES_WHITE_AS_YELLOW + 7] = palette[10];
	palette_buffer[PAL_SPRITES_1000+7] = palette[7];
	palette_buffer[PAL_SPRITES_1000+8] = palette[8];
}

void pal(unsigned int c0, unsigned int c1, enum PaletteID paletteID){
	if(paletteID == PAL_RESET){ // reset all
		for(int i = 1; i < 16; i++){
            palette_buffer[PAL_BG_NORMAL + i] = palette[i];
            palette_buffer[PAL_BG_FLOOR + i] = palette[i];

            palette_buffer[PAL_SPRITES_NORMAL + i] = palette[i]; 
        }
        palette_buffer[PAL_SPRITES_BG_CLOUD_COLOR + 7] = palette[0];
        palette_buffer[PAL_SPRITES_WHITE_AS_PINK + 7] = palette[14];
        palette_buffer[PAL_SPRITES_WHITE_AS_DARK_BLUE + 7] = palette[1];
        palette_buffer[PAL_SPRITES_WHITE_AS_DARK_GRAY + 7] = palette[5];
	}else if(paletteID == PAL_TITLESCREEN_FLASH){
		for(int i = 1; i < 16; i++){
            palette_buffer[PAL_BG_NORMAL + i] = palette[c1];

            palette_buffer[PAL_SPRITES_NORMAL + i] = palette[c1];
        }
        palette_buffer[PAL_SPRITES_WHITE_AS_PINK + 7] = palette[c1];
        palette_buffer[PAL_SPRITES_WHITE_AS_DARK_BLUE + 7] = palette[c1];
        palette_buffer[PAL_SPRITES_WHITE_AS_DARK_GRAY + 7] = palette[c1];
	}else{
		palette_buffer[paletteID + c0] = palette[c1];
	}
}

void cls(unsigned int color){
	pal(13, color, PAL_BG_BACKGROUND_COLOR);
}

OBJATTR* get_next_obj(){
#ifdef DEBUG
	if(obj_count == 128)
		errorHalt("2many obj");
#endif
	return obj_buffer + (127 - obj_count++);
}

OBJAFFINE* get_next_affine_obj(int* affine_id){
#ifdef DEBUG
	if(obj_affine_count == 32)
		errorHalt("2many affine");
#endif
	*affine_id = 31-obj_affine_count;
	return obj_buffer_affine + (31 - obj_affine_count++);
}

static mm_word songEventHandler(mm_word msg, mm_word param){
	switch(msg){
    case MMCB_SONGFINISHED:
		if(_current_music == 22)
        	mmStart(MOD_MUS24, MM_PLAY_LOOP);
    case MMCB_SONGMESSAGE:
        break;
    }
    return 0;
}

void music(int n)
{
	mmStop();
	_current_music = -1;

	switch (n)
	{
		case  2: mmStart(MOD_MUS2,  MM_PLAY_LOOP); break;
		case 22: mmStart(MOD_MUS22, MM_PLAY_ONCE); _current_music = 22; break;
		case 24: mmStart(MOD_MUS24, MM_PLAY_LOOP); break;
		case 36: mmStart(MOD_MUS36, MM_PLAY_LOOP); break;
		case 37: mmStart(MOD_MUS37, MM_PLAY_LOOP); break;
		case 38: mmStart(MOD_MUS38, MM_PLAY_LOOP); break;
		case 39: mmStart(MOD_MUS39, MM_PLAY_ONCE); _current_music = 39; break;
		case 40: mmStart(MOD_MUS40, MM_PLAY_ONCE); _current_music = 40; break;
		default: break;
	}
}

void psfx(int sample_ID, int lock)
{
	if(sfx_timer >= 0 || lock >= 0){
		mmEffect(sample_ID);
		if(lock >= 0)
			sfx_timer = lock;
	}
}

bool btn(KEYPAD_BITS key){
	return (kHeld & key) ? true : false;
}
bool btnp(KEYPAD_BITS key){ // later remove?
	return (kDown & key) ? true : false;
}

FIXED_16 time(){
	return time_since_start;
}

#ifdef DEBUG
volatile int errTime = 0;
void errorHalt(char* errCode){
	update_pico8();
	print(errCode, 60,0, 14);
	update_pico8();
	while(1){errTime++;VBlankIntrWait();};
}
#endif