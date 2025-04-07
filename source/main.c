#include "main.h"

#include <stdlib.h> // int to string itoa
#include <gba_systemcalls.h>
#include <maxmod.h>

#include "pico8.h"
#include "pico8_math.h"
#include "tiles.h"
#include "maps.h"
#include "gameobjects.h"
#include "gameobjects_draw.h"
#include "soundbank.h"
//#include "mgba.h"

// might hit sprite-pixels per scanline limit if too high.
#define CLOUD_COUNT_BG (26)
#define CLOUD_COUNT_FG (16)
#define CLOUDS_SIZE max2(CLOUD_COUNT_BG, CLOUD_COUNT_FG)
#define SNOW_SIZE (25)

// DPAD cam
//int a = 0, b=0;

// other global game variables
int level_index = 0,
	level_intro = 0;
int current_music = 0,
	infade = 0;
Level level;
s16 camera_target_x = 0,
	camera_target_y = 0;
bool have_grapple = false;

// game_start() global variables
FIXED_16 snow_y[SNOW_SIZE];
int snow_x[SNOW_SIZE],
	clouds[CLOUDS_SIZE][3],
	freeze_time = 0,
	frames = 0,
	seconds = 0,
	minutes = 0,
	hours = 0,
	shake = 0,
	sfx_timer = 0,
	death_count = 0;
s16 camera_x = 0, 
	camera_y = 0;
int show_score = 0,
	titlescreen_flash = -100;

bool paused = false;


void __init(); // _init() already exists in toolchain
void _update();
void _draw1();
void _draw2();
void draw_time(int x, int y, bool draw_rect);
void draw_background_clouds(int color);
void draw_foreground_clouds(int fogmode/*, int color*/);
void draw_snow();
void print_center(char* text, unsigned int text_len, int x, int y, int c);
//void snap_camera(); // unused?
void goto_level(int level);
void set_random_clouds();
void reset_game();
int main();


void __init(){ // _init() already exists in toolchain
	// clouds[][] is initialized when the start button is first pressed
	// in _update() using set_random_clouds(), so that clouds are more random.
	for(int i = 0; i < SNOW_SIZE; i++){
		snow_x[i] = rnd(132);
		snow_y[i] = int2Fixed(rnd(132));
	}

	// goto titlescreen or level
	// Titlescreen is now a normal map. No need for a special case here.
	goto_level(level_index);
}

void _update(){
	// titlescreen
	if(level_index == 0){
		if(titlescreen_flash > -100){
			titlescreen_flash -= 1;
			if(titlescreen_flash < -30) goto_level(1);
		}else if(btnp(KEY_A|KEY_B|KEY_START)){
			titlescreen_flash = 50;
			psfx(SFX_SFX22_3, -1);

			set_random_clouds();
		}
			
		// cycle rnd() for better pseudo-random numbers
		rnd(2);
	}

	// level intro card
	else if(level_intro > 0){
		level_intro -= 1;
		if(level_intro == 0) 
			psfx(SFX_SFX17_24_9, -1);
	}

	// normal level
	else{
		// timers
		sfx_timer = max(sfx_timer - 1);
		shake = max(shake - 1);
		infade = min2(infade + 1, 60);
		if(level_index != 8) frames += 1;
		if(frames == 30){seconds += 1; frames = 0;}
		if(seconds == 60){minutes += 1; seconds = 0;}
		if(minutes == 60){hours += 1; minutes = 0;}

		update_input();

		// freeze
		if(freeze_time > 0){
			freeze_time -= 1;
		}else{
			// objects
			update_level_objects();
		}

		// dpad cam. need to disable cam in player for this
	    /*camera_modes[level_index](a, b);
	    camera_x = approach(camera_x, camera_target_x, 5);
	    camera_y = approach(camera_y, camera_target_y, 5);
		camera(camera_x, camera_y);*/
	}
}

// split up so some draws can be done still in the update frame
// for better performance distribution
void _draw1(){
	pal(0, 0, PAL_RESET);

	if(level_index == 0){
		cls(0);

		if(titlescreen_flash > -100){
			int c = 10;
			if(titlescreen_flash > 10){
				if(titlescreen_flash % 10 < 5) c = 7;
			}else if(titlescreen_flash > 5) c = 2;
			else if(titlescreen_flash > 0) c = 1;
			else c = 0;
			if(c < 10) pal(0, c, PAL_TITLESCREEN_FLASH);
		}

		// sprite and rect are encoded in mapdata 
        print_center("lANI'S tREK",11, 64, 68, 14);
        print_center("a game by",9, 64, 80, 1);
        print_center("maddy thorson",13, 64, 87, 5);
        print_center("noel berry",10, 64, 94, 5);
        print_center("lena raine",10, 64, 101, 5);
        draw_snow();
		return;
	}

	if(level_intro > 0){
		cls(0);
		camera(0, 0);
		draw_time(4,4, false);
		if(level_index != 8){
			char level_text[8]={'l','e','v','e','l',' ','0'+level_index-2,'\0'};
			print_center(level_text, 7, 64, 64 - 8, 7);
		}
        print_center(level.title, level.title_len, 64, 64, 7);
		return;
	}

	// not needed anymore, as camera_x and camera_y are global variables anyways?
	//s16 camera_x, camera_y;
	//get_camera(&camera_x, &camera_y);

	if(shake > 0){
		s16 shake_x = -2 + rnd(5);
		s16 shake_y = -2 + rnd(5);
		set_shake_offsets(shake_x, shake_y);
		camera(camera_x + shake_x, camera_y + shake_y);
	}else
		set_shake_offsets(0, 0);

	// clear screen
	cls(level.bg);

	// draw clouds
	// draw_clouds(1, 0, 0, 1, 1, level.clouds or 13, #clouds)
	draw_background_clouds(level.clouds);

	// columns
	// not, columns are loaded into BG2 at the start. Will be enabled or disabled in goto_level()
	// scrolls in update_pico() by moving BG2.

	// draw tileset
	switch(level.pal_mode){
		case 1: pal(2, 12, PAL_BG_FLOOR); pal(5, 2, PAL_BG_FLOOR); break;
		case 2: pal(2, 14, PAL_BG_FLOOR); pal(5, 2, PAL_BG_FLOOR); break;
		case 3: pal(2, 1, PAL_BG_FLOOR); pal(7, 11, PAL_BG_FLOOR); break;
		default: break;
	}
	// not, tileset is loaded in goto_level() and updated in update_pico8()

	// score
	if(show_score > 105){
		draw_score_rect();
		char stats_text[10];
		itoa(berry_count, stats_text, 10);
		print(stats_text, 56+8, 398-camera_y, 7);
        draw_time(56, 408-camera_y, false);
		itoa(death_count, stats_text, 10);
		print(stats_text, 56+8, 421-camera_y, 7);
	}

	// draw objects
	draw_level_objects_sprites();
}

// called in actual draw frame
void _draw2(){
	if(level_index == 0 || level_intro > 0)
		return;

	// draw more objects
	draw_level_objects_tiles();

	// draw snow
	draw_snow();

	// draw FG clouds
	// draw_clouds(1.25, 0, level.height * 8 + 1, 1, 0, 7, #clouds - 10)
	if(level.fogmode != 0)
		draw_foreground_clouds(level.fogmode/*, 7*/);

	// screen wipes
	if(current_gameobjects.player.wipe_timer > 5){
		int e = (127 + 64) * (current_gameobjects.player.wipe_timer-5) / 12;
		draw_screenwipe(e -46, 0, false);
		draw_screenwipe(e -46 - 15, 60, false);
		draw_screenwipe(e -46 - 2*15, 120, false);
		// make other tiles black
		set_overlay_outfade(e -46 -2*15);
	}
	
	if(infade < 15){
		int e = (127 + 64) * infade / 12;
		// Sorry for the pixel math.
		draw_screenwipe(e -20, -64 + 3 + 4+4 + 256, true);
		draw_screenwipe(e -20 - 15, 3 + 4, true);
		draw_screenwipe(e -20 - 2*15, 64 + 3, true);

		set_overlay_infade(e -20 - 2*15);
	}
	// game timer
	if(infade < 45){
		draw_time(4, 4, true);
	}

	camera(camera_x, camera_y);
}

char draw_time_text[9] = {'9','9',' ','5','9',' ','5','9','\0'}; // ':' embedded in rect sprite
ARM_CODE void draw_time(int x, int y, bool draw_rect){
	if(draw_rect) 
		draw_time_rect(x,y);

	if(hours < 10){
		draw_time_text[0] = '0';
		draw_time_text[1] = '0'+hours;
	}else if(hours < 99){
		draw_time_text[0] = '0'+hours/10;
		draw_time_text[1] = '0'+mod_10(hours);
	}else{
		draw_time_text[0] = '9';
		draw_time_text[1] = '9';
	}
	if(minutes < 10){
		draw_time_text[3] = '0';
		draw_time_text[4] = '0'+minutes;
	}else{
		draw_time_text[3] = '0'+minutes/10;
		draw_time_text[4] = '0'+mod_10(minutes);
	}
	if(seconds < 10){
		draw_time_text[6] = '0';
		draw_time_text[7] = '0'+seconds;
	}else{
		draw_time_text[6] = '0'+seconds/10;
		draw_time_text[7] = '0'+mod_10(seconds);
	}
	print(draw_time_text,x+1,y+1,7);
}

/*
draw_clouds(1, 0, 0, 1, 1, level.clouds or 13, #clouds)

s is 16-48 originally
*/
ARM_CODE void draw_background_clouds(int color){
	pal(7, color, PAL_SPRITES_BG_CLOUD_COLOR);
	for(int i = 0; i < CLOUD_COUNT_BG; i++){
		int s = 16 + (clouds[i][2]+1)*4-1;
		if(color > 0){
			// changed centering to left edge instead of middle. s works for wrapping, probably because sprite images are at left edge
			int x = positive_modulo(clouds[i][0]/4 - camera_x * 9/10, 128 + s) - s;
			int y = positive_modulo(clouds[i][1] - camera_y * 9/10, 128 + clouds_actual_height_background[clouds[i][2]]) 
					- clouds_actual_height_background[clouds[i][2]];
			if(i % 2 == 0){
				draw_background_cloud_big(x, y, clouds[i][2]);
			}
			else{
				draw_background_cloud_small(x, y, clouds[i][2]);
			}
		}
		// Only counting how many multiples of 0.25f instead of the exact number.
		// When calculating x you get the actual number by dividing by 4 (or shift by 2)
		clouds[i][0] += (4 - (i%4));
		if(clouds[i][0] > (128 + s)*4)
			clouds[i][0] -= (128 + s)*4;
	}
}
// draw_clouds(1.25, 0, level.height * 8 + 1, 1, 0, 7, #clouds - 10)
ARM_CODE void draw_foreground_clouds(int fogmode/*, int color*/){
	for(int i = 0; i < CLOUD_COUNT_FG; i++){
		int s = 16 + (clouds[i][2]+1)*4-1;
		if(camera_y+128 >= level.height*8-32){
			int x = positive_modulo(clouds[i][0]/4 - camera_x * 9/10, 128 + s) - s; // changed to left edge instead of middle
			if(i % 2 == 0){
				int y = level.height*8 - clouds_actual_height_foreground[clouds[i][2]] - camera_y;
				draw_foreground_cloud_big(x, y, clouds[i][2], fogmode == 1);
			}
			else{
				int y = level.height*8 - clouds_actual_height_foreground[clouds[i][2]] - camera_y;
				draw_foreground_cloud_small(x, y, clouds[i][2], fogmode == 1);
			}
		}
		// Only counting how many multiples of 0.25f instead of the exact number.
		// When calculating x you get the actual number by dividing by 4 (or shift by 2)
		clouds[i][0] += (4 - (i%4));
		if(clouds[i][0] > (128 + s)*4)
			clouds[i][0] -= (128 + s)*4;
	}
}

void draw_snow(){
	for(int i = 0; i < SNOW_SIZE; i++){
		int sx = mod_132(snow_x[i]-camera_x/2) - 2;
		int sy = mod_132(fixed2Int(snow_y[i])-camera_y/2) - 2;

		draw_snowflake(sx, sy, i&1);

		snow_x[i] = ((unsigned)snow_x[i] + (4 - (i % 4)));
		if(snow_x[i] > 132) snow_x[i] -= 132;

		snow_y[i] = snow_y[i] + sin_fixed(mulFixed(time(), float2Fixed(0.25f)) + float2Fixed(0.1f)*i);
		if(snow_y[i] > int2Fixed(132)) snow_y[i] -= int2Fixed(132);
		if(snow_y[i] < int2Fixed(0)) snow_y[i] += int2Fixed(132);
	}
}

void print_center(char* text, unsigned int text_len, int x, int y, int c){
	x -= (text_len*4-1)/2;
	print(text, x, y, c);
}

int approach(int x, int target, int max_delta){
	if(x < target)
		return min2(x + max_delta, target);
	return max2(x - max_delta, target);
}

void camera_x_barrier(int tile_x, int px, int py){
	int bx = tile_x * 8;
	if(px < bx - 8){
		camera_target_x = min2(camera_target_x, bx - 128);
	}else if(px > bx + 8){
		camera_target_x = max2(camera_target_x, bx);
	}
}

static int c_offset = 0;
static bool c_flag = false;

// 0: Titlescreen
void camera_mode0(int px, int py){
	camera_target_x = 0;
	camera_target_y = 0;
}
// 1: Intro
void camera_mode1(int px, int py){
	if(px < 42)
		camera_target_x = 0;
	else
		camera_target_x = max2(40, min2(level.width * 8 - 128, px - 48));
}
// 2: Intro 2
void camera_mode2(int px, int py){
	if(px < 120)
		camera_target_x = 0;
	else if(px > 136){
		camera_target_x = 128;
	}else{
		camera_target_x = px - 64;
	}
	camera_target_y = max(min2(level.height * 8 - 128, py - 64));
}
// 3: Level 1
void camera_mode3(int px, int py){
	camera_target_x = max(min2(level.width * 8 - 128, px - 56));
	camera_x_barrier(level.camera_barrier_x, px, py);

	if(py < level.camera_barrier_y * 8 + 3){
		camera_target_y = 0;
	}else{
		camera_target_y = level.camera_barrier_y * 8;
	}
}
// 4: Level 2
void camera_mode4(int px, int py){
	if(px % 128 > 8 || px % 128 < 120){
		px = (px & ~(128-1)) + 64; // flr(px / 128) * 128 + 64; // get next lowest multiple of 128
	}
	if(px % 128 > 4 || px % 128 < 124){
		py = (py & ~(128-1)) + 64;
	}
	camera_target_x = max(min2(level.width * 8 - 128, px - 64));
    camera_target_y = max(min2(level.height * 8 - 128, py - 64));
}
// 5: Level 3-1
void camera_mode5(int px, int py){
	camera_target_x = max(min2(level.width * 8 - 128, px - 32));
}
// 6: Level 3-2
void camera_mode6(int px, int py){
	if(px > 848){
		c_offset = 48;
	}else if(px < 704){
		c_flag = false;
		c_offset = 32;
	}else if(px > 808){
		c_flag = true;
		c_offset = 96;
	}

	camera_target_x = max(min2(level.width * 8 - 128, px - c_offset));

	camera_x_barrier(level.camera_barrier_x, px, py);

	if(c_flag){
		camera_target_x = max2(camera_target_x, 672);
	}
}
// 7: Level 3-3
void camera_mode7(int px, int py){
	if(px > 420){
		if(px < 436){
			c_offset = 32 + px - 420;
		}else if(px > 808){
			c_offset = 48 - min2(16, px - 808);
		}else{
			c_offset = 48;
		}
	}else{
		c_offset = 32;
	}
	camera_target_x = max2(0, min2(level.width * 8 - 128, px - c_offset));
}
// 8: End
void camera_mode8(int px, int py){
	camera_target_y = max2(0, min2(level.height * 8 - 128, py - 32));
}
const void (*camera_modes[])(int px, int py) = {
	&camera_mode0,
	&camera_mode1,
	&camera_mode2,
	&camera_mode3,
	&camera_mode4,
	&camera_mode5,
	&camera_mode6,
	&camera_mode7,
	&camera_mode8
};

/*void snap_camera(){ // unused
	camera_x = camera_target_x;
	camera_y = camera_target_y;
	camera(camera_x, camera_y);
}*/

void goto_level(int index){
	// set level
	level = levels[index];
	level_index = index;
	level_checkpoint = NULL;

	if(level.title[0] != '\0')
		level_intro = 60;

	if(level_index == 2)
		psfx(SFX_SFX17_8_16, -1);

	set_columns_enabled(level.columns);

	// load into ram
	if(level_index == 2){
		load_tiledata(Spritesheet_clouds_holed);
	}else{
		load_tiledata(Spritesheet_clouds_normal);
	}

	load_level_map(index);

	// start music
	if(current_music != level.music){
		current_music = level.music;
		music(level.music);
	}

	// load level contents
	restart_level();
}

void next_level(){
	level_index += 1;
	goto_level(level_index);
}

void restart_level(){
	camera_x = 0;
	camera_y = 0;
	camera_target_x = 0;
	camera_target_y = 0;
	//objects = {}
	infade = 0;
	have_grapple = level_index > 2;
	sfx_timer = 0;

	//create objects
	load_level_objects(level_index);
}



void set_random_clouds(){
	for(int i = 0; i < CLOUDS_SIZE; i++){
		clouds[i][0] = rnd(132)*4; // *4 because of bigger range (reason in draw_clouds)
		// could've used fixed points, but I only implemented those much later
		clouds[i][1] = rnd(132);
		// only 8 different clound sizes instead of 32
		// sprite sizes are distributed from 3 to 31
		// calculate actual size with: 16 + (s+1)*4-1
		clouds[i][2] = rnd(8);
	}
}

void reset_game(){
    // clear map and middlescreen
    // reset globals
    level_index = 0;
    level_intro = 0;

    freeze_time = 0;
    frames = 0;
	seconds = 0;
	minutes = 0;
	hours = 0;
	shake = 0;
    sfx_timer = 0;
	show_score = 0;
	berry_count = 0;
    death_count = 0;
	current_music = 0;
    for(int i = 0; i < BERRY_COUNT; i++)
		collected[i] = false;
    titlescreen_flash = -100;

    c_offset = 0;
    c_flag = false;

	level_checkpoint = NULL;

	camera(0,0);
	set_shake_offsets(0,0);
	set_overlay(0,false);
	music(-1);
	set_random_clouds();
	paused = false;

    __init();
}

int main(){
	//mgba_open();
	//char str[10];
    init_pico8();
    reset_game();

	while(1){
		/*if(btn(KEY_LEFT))
			a-=3;
		if(btn(KEY_RIGHT))
			a+=3;
		if(btn(KEY_UP))
			b-=3;
		if(btn(KEY_DOWN))
			b+=3;*/
		if(btn(KEY_L)&&btn(KEY_R)&&btn(KEY_DOWN)&&btn(KEY_B)&&btnp(KEY_START)){
			/*if(level_index == 0)
				set_random_clouds();
			next_level();*/
			reset_game();
			// Resets newly pressed buttons.
			// Otherwise game start may get triggered instantly
			update_keys();
		}
		if(btnp(KEY_SELECT)){
			paused = !paused;
			// somehow seems to not pause soundeffects.
			// seems like maxmod does not support that :(
			if(paused)
				mmPause();
			else
				mmResume();
		}

		VBlankIntrWait();
		mmFrame();

		if(!paused){
			_update();
			_draw1();
		}
		//itoa(REG_VCOUNT, str, 10);
		///print(str, 10, 60, 14);

		VBlankIntrWait();
		mmFrame();
		
		if(!paused)
			_draw2();
		//itoa(REG_VCOUNT, str, 10);
		//print(str, 10, 68, 14);

		// need to update pico in draw frame, so that objects are updated at the same time as tiles.
		update_pico8();
	}
}
