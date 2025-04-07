#include "gameobjects_draw.h"

#include <gba_sprites.h>
#include <gba_video.h>
#include <gba_systemcalls.h>
#include "pico8.h"
#include "tiles.h"
#include "main.h"
//#include "mgba.h"

// used to clip to screen space. Also account for sprite size (looked up from spritesheet)
#define RETURN_IF_SPRITE_POSITION_OUT_OF_BOUNDS(x,y,width,height)   \
	if((x)+(width) <= CAMERA_OFFSET_X || (x) >= CAMERA_OFFSET_X+16*8 || (y)+(height) <= CAMERA_OFFSET_Y || (y) >= CAMERA_OFFSET_Y+16*8) return;

void player_draw_scarf(struct Player_data* self);
void draw_sine_h(struct Player_data* self);
void pset(u8* buffer, int tile_width, int x, int y, int color);
void line(u8* buffer, int tile_width, int x0, int y0, int x1, int y1, int color);

static u16 get_fontID(char c){
	if('0' <= c && c <= '9')// use for sprite ids? Works, whatever
		return 21*32 + (c-'0');
	else if('a' <= c && c <= 'e')
		return 20*32 + (c-'a');
	else if('g' <= c && c <= 'i')
		return 20*32 + (c-'a'-1);
	else if('l' <= c && c <= 'o')
		return 20*32 + (c-'a'-3);
	else if('r' <= c && c <= 't')
		return 20*32 + (c-'a'-5);
	else if(c == 'v')
		return 20*32 + 15;
	else if(c == 'y')
		return 20*32 + 16;

	else if(c == 'A')
		return 20*32 + 17;
	else if(c == 'E')
		return 20*32 + 18;
	else if(c == 'I')
		return 20*32 + 19;
	else if(c == 'K')
		return 20*32 + 20;
	else if(c == 'N')
		return 20*32 + 21;
	else if(c == 'R')
		return 20*32 + 22;
	else if(c == 'S')
		return 20*32 + 23;

	else if(c == '\'' || c == '-')
		return 21*32 + 10;

	return 0;
}

// x is [-256,255] and y is [-128, 127], camera offset is added
void print(char* text, int x, int y, unsigned int c){
	x += CAMERA_OFFSET_X -get_shake_offset_x();
	y += CAMERA_OFFSET_Y -get_shake_offset_y();
	for(int i = 0; text[i] != '\0'; i++){
		if(text[i] == ' ')
			continue;
		OBJATTR* obj = get_next_obj();
		obj->attr0 = ATTR0_SQUARE|ATTR0_COLOR_16|y;
		obj->attr1 = ATTR1_SIZE_8|(x+i*4);
		u16 attr2 = OBJ_PALETTE(0)|OBJ_PRIORITY(0)|get_fontID(text[i]);
		if(c == 7){
			// still palette 0
		}else if(c == 5)
			attr2 |= OBJ_PALETTE(8)|OBJ_PRIORITY(1);
		else if(c == 14)
			attr2 |= OBJ_PALETTE(6)|OBJ_PRIORITY(1);
		else if(c == 1)
			attr2 |= OBJ_PALETTE(7)|OBJ_PRIORITY(1);
		obj->attr2 = attr2;
	}
}

void draw_time_rect(int x, int y){
	x += CAMERA_OFFSET_X -get_shake_offset_x();
	y += CAMERA_OFFSET_Y -get_shake_offset_y();
	OBJATTR* obj = get_next_obj();
	obj->attr0 = ATTR0_WIDE|ATTR0_COLOR_16|y;
	obj->attr1 = ATTR1_SIZE_16|x;
	obj->attr2 = OBJ_PALETTE(4)|OBJ_PRIORITY(0)|Sprite_TimeRect0;

	obj = get_next_obj();
	obj->attr0 = ATTR0_SQUARE|ATTR0_COLOR_16|y;
	obj->attr1 = ATTR1_SIZE_8|(x+32);
	obj->attr2 = OBJ_PALETTE(4)|OBJ_PRIORITY(0)|Sprite_TimeRect1;
}

void draw_background_cloud_small(int x, int y, int size){
	x += CAMERA_OFFSET_X -get_shake_offset_x();
	y += CAMERA_OFFSET_Y -get_shake_offset_y();
	u16 y_ = y + (y < 0) * 256;
	
	OBJATTR* obj = get_next_obj();
	obj->attr0 = ATTR0_WIDE|ATTR0_COLOR_16|(y_&0xFF);
	obj->attr1 = OBJ_SIZE(clouds_small_size[size])|(x&0x1FF);
	obj->attr2 = OBJ_PALETTE(2)|OBJ_PRIORITY(3)|Clouds0(clouds_small_sprite[size]);
}

void draw_background_cloud_big(int x, int y, int size){
	x += CAMERA_OFFSET_X -get_shake_offset_x();
	y += CAMERA_OFFSET_Y -get_shake_offset_y();
	u16 y_ = y + (y < 0) * 256;
	
	OBJATTR* obj = get_next_obj();
	obj->attr0 = ATTR0_WIDE|ATTR0_COLOR_16|(y_&0xFF);
	obj->attr1 = OBJ_SIZE(clouds_big_size[size])|(x&0x1FF);
	obj->attr2 = OBJ_PALETTE(2)|OBJ_PRIORITY(3)|Clouds0(clouds_big_sprite[size]);
}

void draw_foreground_cloud_small(int x, int y, int size, bool holed){
	x += CAMERA_OFFSET_X -get_shake_offset_x();
	y += CAMERA_OFFSET_Y -get_shake_offset_y();
	u16 y_ = y + (y < 0) * 256;

	OBJATTR* obj = get_next_obj();
	obj->attr0 = ATTR0_WIDE|ATTR0_COLOR_16|(y_&0xFF);
	obj->attr1 = OBJ_SIZE(clouds_small_size[size])|(x&0x1FF);
	u16 attr2 = OBJ_PALETTE(0)|OBJ_PRIORITY(1);
	if(holed){
		if(((x&0x1) ^ (y&0x1) ^ (clouds_actual_height_foreground[size]&0x1)) == 0) // Bottom left pixel must be a hole.
			attr2 |= Clouds1(clouds_small_sprite[size]);
		else
			attr2 |= Clouds0(clouds_small_sprite[size]);
	}else{
		attr2 |= Clouds1(clouds_small_sprite[size]);
	}
	obj->attr2 = attr2;
}

void draw_foreground_cloud_big(int x, int y, int size, bool holed){
	x += CAMERA_OFFSET_X -get_shake_offset_x();
	y += CAMERA_OFFSET_Y -get_shake_offset_y();
	u16 y_ = y + (y < 0) * 256;

	OBJATTR* obj = get_next_obj();
	obj->attr0 = ATTR0_WIDE|ATTR0_COLOR_16|(y_&0xFF);
	obj->attr1 = OBJ_SIZE(clouds_big_size[size])|(x&0x1FF);
	u16 attr2 = OBJ_PALETTE(0)|OBJ_PRIORITY(1);
	if(holed){
		if(((x&0x1) ^ (y&0x1) ^ (clouds_actual_height_foreground[size]&0x1)) == 0) // Bottom left pixel must be a hole.
			attr2 |= Clouds1(clouds_big_sprite[size]);
		else
			attr2 |= Clouds0(clouds_big_sprite[size]);
	}else{
		attr2 |= Clouds1(clouds_big_sprite[size]);
	}
	obj->attr2 = attr2;
}

void draw_snowflake(int x, int y, int size){
	x += CAMERA_OFFSET_X -get_shake_offset_x();
	y += CAMERA_OFFSET_Y -get_shake_offset_y();
	OBJATTR* obj = get_next_obj();
	obj->attr0 = ATTR0_SQUARE|ATTR0_COLOR_16|(y-3);
	obj->attr1 = ATTR1_SIZE_8|(x-3);
	obj->attr2 = OBJ_PALETTE(0)|OBJ_PRIORITY(1)|(Sprite_Circle0 + 32*(size&1));
}

void draw_screenwipe(int x, int y, bool inwipe){
	x += CAMERA_OFFSET_X -get_shake_offset_x();
	y += CAMERA_OFFSET_Y -get_shake_offset_y();
	/*if(y < 0) // handled in _draw()
		y += 256;
	if(x < 0)
		x += 512;*/
	OBJATTR* obj = get_next_obj();
	obj->attr0 = ATTR0_SQUARE|ATTR0_COLOR_16|y;
	u16 attr1 = ATTR1_SIZE_64|x;
	if(inwipe)
		attr1 |= ATTR1_FLIP_X|ATTR1_FLIP_Y;
	obj->attr1 = attr1;
	obj->attr2 = OBJ_PALETTE(1)|OBJ_PRIORITY(0)|(Sprite_Screenwipe);
}

void draw_score_rect(){
	int x = 32+CAMERA_OFFSET_X -get_shake_offset_x(),
		y = 390+CAMERA_OFFSET_Y-get_camera_y();
        
    RETURN_IF_SPRITE_POSITION_OUT_OF_BOUNDS(x,y,67,45);
	
	OBJATTR* obj = get_next_obj();
	obj->attr0 = ATTR0_WIDE|ATTR0_COLOR_16|y;
	obj->attr1 = ATTR1_SIZE_64|x;
	obj->attr2 = OBJ_PALETTE(4)|OBJ_PRIORITY(1)|Sprite_ScoreRect0;

	obj = get_next_obj();
	obj->attr0 = ATTR0_TALL|ATTR0_COLOR_16|y;
	obj->attr1 = ATTR1_SIZE_16|(x+64);
	obj->attr2 = OBJ_PALETTE(4)|OBJ_PRIORITY(1)|Sprite_ScoreRect1;

	obj = get_next_obj();
	obj->attr0 = ATTR0_WIDE|ATTR0_COLOR_16|(y+32);
	obj->attr1 = ATTR1_SIZE_32|x;
	obj->attr2 = OBJ_PALETTE(4)|OBJ_PRIORITY(1)|Sprite_ScoreRect2;

	obj = get_next_obj();
	obj->attr0 = ATTR0_WIDE|ATTR0_COLOR_16|(y+32);
	obj->attr1 = ATTR1_SIZE_32|(x+32);
	obj->attr2 = OBJ_PALETTE(4)|OBJ_PRIORITY(1)|Sprite_ScoreRect3;

	obj = get_next_obj();
	obj->attr0 = ATTR0_TALL|ATTR0_COLOR_16|(y+32);
	obj->attr1 = ATTR1_SIZE_8|(x+64);
	obj->attr2 = OBJ_PALETTE(4)|OBJ_PRIORITY(1)|Sprite_ScoreRect4;
}

// Only updates sprites in buffers
void draw_level_objects_sprites(){
	if(current_gameobjects.grapplePickup_enabled)
		if(!current_gameobjects.grapplePickup.destroyed)
			grapplepickup_draw(&current_gameobjects.grapplePickup.Position);
	for(int i = 0; i < MAX_SNOWBALL_COUNT; i++)
		if(!current_gameobjects.snowballs[i].destroyed)
			snowball_draw(&current_gameobjects.snowballs[i].Holdable.Position);
	for(int i = 0; i < current_gameobjects.springboards_len; i++)
		if(!current_gameobjects.springboards[i].destroyed)
			object_draw(&current_gameobjects.springboards[i].Holdable.Position, 
                        current_gameobjects.springboards[i].spr_pressed ? Sprite_Springboard_Pressed
                                                                        : Sprite_Springboard_Normal);
    if(current_gameobjects.bridge_enabled)
        for(int i = 0; i < BRIDGE_COUNT; i++)
            object_draw(&current_gameobjects.bridge[i].Position, Sprite_Bridge);
    for(int i = 0; i < current_gameobjects.berries_len; i++){
        if(!current_gameobjects.berries[i].destroyed)
            berry_draw(current_gameobjects.berries+i);
	}

	// crumble and checkpoint are updated in actual draw frame instead

	player_draw(&current_gameobjects.player);

	/* Validate spike hitbox positions
	SpriteOtherID id;
	for(int i = 0; i < current_gameobjects.spikeGroups_len; i++){
		struct Position_data p;
		p.x = current_gameobjects.spikeGroups[i].Position.x + current_gameobjects.spikeGroups[i].Hitbox.hit_x;
		p.y = current_gameobjects.spikeGroups[i].Position.y + current_gameobjects.spikeGroups[i].Hitbox.hit_y;

		if(current_gameobjects.spikeGroups[i].hazard == SpikeDirection_Up){
			id = Sprite_Screenwipe;
		}else if(current_gameobjects.spikeGroups[i].hazard == SpikeDirection_Right){
			id = Sprite_Screenwipe;
		}else if(current_gameobjects.spikeGroups[i].hazard == SpikeDirection_Down){
			id = Sprite_Screenwipe;
		}else{
			id = Sprite_Screenwipe;
		}
		
		object_draw(&p, id);
		p.x+=current_gameobjects.spikeGroups[i].Hitbox.hit_w;
		p.y+=current_gameobjects.spikeGroups[i].Hitbox.hit_h;
		object_draw(&p, id);
	}*/
}

// Updates tiles directly on VRAM
void draw_level_objects_tiles(){
	for(int i = 0; i < current_gameobjects.crumbles_len; i++)
		crumble_draw(current_gameobjects.crumbles+i);
	for(int i = 0; i < current_gameobjects.checkpoints_len; i++)
		checkpoint_draw(current_gameobjects.checkpoints+i);
}

void object_draw(struct Position_data* self_Position, SpriteOtherID spriteID){
    int x = fixed2Int(self_Position->x) +CAMERA_OFFSET_X-get_camera_x(),
		y = fixed2Int(self_Position->y) +CAMERA_OFFSET_Y-get_camera_y();

    RETURN_IF_SPRITE_POSITION_OUT_OF_BOUNDS(x,y,8,8);

	OBJATTR* obj = get_next_obj();
	obj->attr0 = ATTR0_SQUARE|ATTR0_COLOR_16|(y);
	obj->attr1 = ATTR1_SIZE_8|(x);
	obj->attr2 = OBJ_PALETTE(0)|OBJ_PRIORITY(1)|(spriteID);
}

void grapplepickup_draw(struct Position_data* self_Position){
	// *8 to map from pixels to tiles?
	int x = fixed2Int(self_Position->x) +CAMERA_OFFSET_X-get_camera_x(),
		y = fixed2Int(self_Position->y + sin_fixed(time())*2) +CAMERA_OFFSET_Y-get_camera_y();

    RETURN_IF_SPRITE_POSITION_OUT_OF_BOUNDS(x,y,8,8);

	OBJATTR* obj = get_next_obj();
    // shouldn't need &FF and &1FF, because x and y should only be in bounds
	obj->attr0 = ATTR0_SQUARE|ATTR0_COLOR_16|(y);
	obj->attr1 = ATTR1_SIZE_8|(x);
	obj->attr2 = OBJ_PALETTE(0)|OBJ_PRIORITY(1)|(Sprite_GrapplePickup);
}

void snowball_draw(struct Position_data* self_Position){
	int x = fixed2Int(self_Position->x) +CAMERA_OFFSET_X-get_camera_x(),
		y = fixed2Int(self_Position->y) +CAMERA_OFFSET_Y-get_camera_y();

    RETURN_IF_SPRITE_POSITION_OUT_OF_BOUNDS(x,y,8,9);

	OBJATTR* obj = get_next_obj();
	obj->attr0 = ATTR0_TALL|ATTR0_COLOR_16|(y);
	obj->attr1 = ATTR1_SIZE_8|(x);
	obj->attr2 = OBJ_PALETTE(0)|OBJ_PRIORITY(1)|(Sprite_Snowball);
}

void berry_draw(struct Berry_data* self){
    if(self->timer < 5){
        int x = fixed2Int(self->Position.x) +CAMERA_OFFSET_X-get_camera_x(),
            y = fixed2Int(self->Position.y + sin_fixed(time())*2) +CAMERA_OFFSET_Y-get_camera_y();

        RETURN_IF_SPRITE_POSITION_OUT_OF_BOUNDS(x,y,8,8);

        OBJATTR* obj = get_next_obj();
        obj->attr0 = ATTR0_SQUARE|ATTR0_COLOR_16|(y);
        obj->attr1 = ATTR1_SIZE_8|(x);
        obj->attr2 = OBJ_PALETTE(0)|OBJ_PRIORITY(1)|(Sprite_Berry);
        
        if(self->flash > 0){
            OBJATTR* obj = get_next_obj();
            // please don't go OOB
            x += -12; // circle sprites are offset
            y += -12;
            if(self->flash == 1){
                x += 8; y += 8;
                obj->attr1 = ATTR1_SIZE_16;
            }else
                obj->attr1 = ATTR1_SIZE_32;
            obj->attr1 |= x;

            obj->attr0 = ATTR0_SQUARE|ATTR0_COLOR_16|(y);
            obj->attr2 = OBJ_PALETTE(0)|OBJ_PRIORITY(1)|(berry_collect_sprites[self->flash-1]);
        }
    }else{
        if(self->timer % 4 < 2)
            pal(7,7,PAL_SPRITES_1000);
        else
            pal(7,14,PAL_SPRITES_1000); 
        int x = fixed2Int(self->Position.x)-4 +CAMERA_OFFSET_X-get_camera_x(),
            y = fixed2Int(self->Position.y) +CAMERA_OFFSET_Y-get_camera_y();

        RETURN_IF_SPRITE_POSITION_OUT_OF_BOUNDS(x,y,15,6);

        OBJATTR* obj = get_next_obj();
        obj->attr0 = ATTR0_WIDE|ATTR0_COLOR_16|(y&0xFF);
        obj->attr1 = ATTR1_SIZE_8|(x&0x1FF);
        obj->attr2 = OBJ_PALETTE(3)|OBJ_PRIORITY(1)|(Sprite_1000);
    }
}

void crumble_draw(struct Crumble_data* self){
	int x = fixed2Int(self->Position.x),
	 	y = fixed2Int(self->Position.y);
	// Idk if shake offset messes may mess something up here. But has worked until now
    RETURN_IF_SPRITE_POSITION_OUT_OF_BOUNDS(x +CAMERA_OFFSET_X-get_camera_x(),y +CAMERA_OFFSET_Y-get_camera_y(),8,8);

	// need x%32, because background size is 32x32 tiles, but actual level (and therefore coordinates) may be wider
	switch(self->breaking){
	case 0:
		((u16*)SCREEN_BASE_BLOCK(13))[y/8*32+(x/8)%32] = CHAR_PALETTE(0)|Tile_Crumble_Normal; break;
	case 1:
		((u16*)SCREEN_BASE_BLOCK(13))[y/8*32+(x/8)%32] = CHAR_PALETTE(0)|Tile_Crumble_Breaking; break;
	case 2:
		((u16*)SCREEN_BASE_BLOCK(13))[y/8*32+(x/8)%32] = CHAR_PALETTE(0)|Tile_Empty; break;
	}
}

void checkpoint_draw(struct Checkpoint_data* self){
	int x = fixed2Int(self->Position.x),
	 	y = fixed2Int(self->Position.y);
	// idk about shake offset
    RETURN_IF_SPRITE_POSITION_OUT_OF_BOUNDS(x +CAMERA_OFFSET_X-get_camera_x(),y-2 +CAMERA_OFFSET_Y-get_camera_y(),8,10);

	if(level_checkpoint == self){
		// position is at lower tile.
		// only 15 animation frames => frames(max 30)/2
		int animationFrame = frames;
		if(animationFrame >= 15)
			animationFrame -= 15;
		((u16*)SCREEN_BASE_BLOCK(13))[(y-1)/8*32+(x/8)%32] = CHAR_PALETTE(0)|(Tile_Checkpoint_Animation0+animationFrame);
		((u16*)SCREEN_BASE_BLOCK(13))[(y)/8*32+(x/8)%32] = CHAR_PALETTE(0)|(Tile_Checkpoint_Animation0+32+animationFrame);
	}else{
		((u16*)SCREEN_BASE_BLOCK(13))[(y-1)/8*32+(x/8)%32] = CHAR_PALETTE(0)|Tile_Empty;
		((u16*)SCREEN_BASE_BLOCK(13))[(y)/8*32+(x/8)%32] = CHAR_PALETTE(0)|Tile_Checkpoint_Normal;
	}
}

// 32x32 sprite; 0,0 is the in the top right quadrant around the middle point.
// => ranges from -16 to 15
static u8 scarf_sprite_buffer[32*32/2];
// 64x32 sprite; y16 should be placed at placer pos
// is also used for the animation lines when collecting grapplePickup
static u8 grapple_sprite_buffer[64*32/2];
// only update VRAM when something was rendered on the grapple
static bool grapple_was_rendered = false;
// used so VRAM is only updated once for the lines
static bool grappleCollectLines_being_drawn = false;

void player_draw(struct Player_data* self){
	// death fx
	if(self->state == 99){
		//death circles
		FIXED_16 e = int2Fixed(current_gameobjects.player.wipe_timer)/10;
		int dx = fixed2Int(current_gameobjects.player.Position.x) -get_camera_x(), 
			dy = fixed2Int(current_gameobjects.player.Position.y) -get_camera_y() - 4;
		if(dx < 0) dx = 0;
		if(dx >= 128) dx = 127;
		if(dy < 0) dy = 0;
		if(dy >= 128) dy = 127;
		dx += CAMERA_OFFSET_X;
		dy += CAMERA_OFFSET_Y;
		
		if(e <= int2Fixed(1)){
			int size = abs_(fixed2Int((int2Fixed(1)-e)*8));
			// circle sprites of different sizes have different offsets
			int sprite_offset = size <= 3 ? -3 : -8;
			for(int i = 0; i < 8; i++){
				int x = dx + fixed2Int(mulFixed(cos_fixed(int2Fixed(i) / 8) * 32, e)) + sprite_offset,
					y = dy + fixed2Int(mulFixed(sin_fixed(int2Fixed(i) / 8) * 32, e)) + sprite_offset;

				OBJATTR* obj = get_next_obj();
				obj->attr0 = ATTR0_SQUARE|ATTR0_COLOR_16|(y&0xFF);
				obj->attr1 = OBJ_SIZE(circle_size[size])|(x&0x1FF);
				obj->attr2 = OBJ_PALETTE(5)|OBJ_PRIORITY(1)|(circle_sprites[size]);
			}
		}
		return;
	}

	// scarf
	player_draw_scarf(self);

	// grapple
	bool grapple_sprite_used = false;
	if(self->state >= 10 && self->state <= 12){
		draw_sine_h(self);
		grapple_sprite_used = true;
		grapple_was_rendered = true;
	}

	// retracting grapple
	if(self->grapple_retract){
		// adjust coordinates, as sprite will be drawn at player pos
		line(grapple_sprite_buffer, 8, 64*(self->grapple_dir==-1), -3+1+16, fixed2Int(self->grapple_x -self->Position.x)+64*(self->grapple_dir==-1), fixed2Int(self->grapple_y -self->Position.y)+1+16, 1);
		line(grapple_sprite_buffer, 8, 64*(self->grapple_dir==-1), -3+16, fixed2Int(self->grapple_x -self->Position.x)+64*(self->grapple_dir==-1), fixed2Int(self->grapple_y -self->Position.y)+16, 7);
		grapple_sprite_used = true;
		grapple_was_rendered = true;
	}

	if(grapple_sprite_used){
		int grapple_sprite_x = fixed2Int(self->Position.x) +CAMERA_OFFSET_X-get_camera_x()-64*(self->grapple_dir==-1);
		int grapple_sprite_y = fixed2Int(self->Position.y) +CAMERA_OFFSET_Y-get_camera_y()-16;
		OBJATTR* obj = get_next_obj();
		obj->attr0 = ATTR0_WIDE|ATTR0_COLOR_16|(grapple_sprite_y&0xFF);
		obj->attr1 = ATTR1_SIZE_64|(grapple_sprite_x&0x1FF);
		obj->attr2 = OBJ_PALETTE(0)|OBJ_PRIORITY(1)|(Sprite_Grapple);
	}

	// sprite
	int x = fixed2Int(self->Position.x)-4 +CAMERA_OFFSET_X-get_camera_x(),
		y = fixed2Int(self->Position.y)-8 +CAMERA_OFFSET_Y-get_camera_y();

	OBJATTR* obj = get_next_obj();
	obj->attr0 = ATTR0_SQUARE|ATTR0_COLOR_16|(y&0xFF);
	obj->attr1 = ATTR1_SIZE_8|(x&0x1FF);
	if(self->facing < 0)
	obj->attr1 |= ATTR1_FLIP_X;
	obj->attr2 = OBJ_PALETTE(0)|OBJ_PRIORITY(1)|(Sprite_PlayerID(fixed2Int(self->spr)));

	// grapple collect
	if(self->state == 50 && self->t_grapple_pickup > 0){
		if(!grappleCollectLines_being_drawn){
			// grapple sprite will not be rendered as grapple at this time
			// reuse this sprite for the rotating grapple collect lines
			grappleCollectLines_being_drawn = true;
			vu32 c=0;
			CpuFastSet((void*)&c, grapple_sprite_buffer, 64*4|FILL);
			line(grapple_sprite_buffer, 8, 0, 15, 23, 15, 7);
			grapple_was_rendered = true;
		}

		// need to do x-5 instead of -4 for correct pixel positioning
		int x = fixed2Int(self->Position.x)-5 +CAMERA_OFFSET_X-get_camera_x(),
			y = fixed2Int(self->Position.y)-18 +CAMERA_OFFSET_Y-get_camera_y();
		//RETURN_IF_SPRITE_POSITION_OUT_OF_BOUNDS(x,y,8,8);
		OBJATTR* obj = get_next_obj();
		// shouldn't need &FF and &1FF, because x and y should only be in bounds
		obj->attr0 = ATTR0_SQUARE|ATTR0_COLOR_16|(y);
		obj->attr1 = ATTR1_SIZE_8|(x);
		obj->attr2 = OBJ_PALETTE(0)|OBJ_PRIORITY(1)|(Sprite_GrapplePickup);

		for(int i = 0; i < 16; i++){
			int affine_id;
			OBJAFFINE* obj_affine = get_next_affine_obj(&affine_id);
			OBJATTR* obj = get_next_obj();

			FIXED_16 s = sin_fixed(time()*4 + int2Fixed(i)/16);
			FIXED_16 c = cos_fixed(time()*4 + int2Fixed(i)/16);
			
			/*
			Rotation happens around middle point of sprite => need to offset by sin/cos and by sprite size:
			-16, so x and y are middle points of the sprite
			2x sin/cos, so that after rotation the lines middle point lies on 2x the circle boundary.
				=> this way the start of the line always always touches the 1x circle boundary for all rotations.
			+sin/cos, because the line sprite is not actually symmetric
				=> for degree 180 the line will be y-1 off, for x at 90 degrees similar
				=> offset by that exact amount for every rotation
			x+abs(cos), because for some reason (discretization artifact?) the lines at degrees 0 and 180 are 1 pixel off
				=> offset them by, dependant on the rotation
			*/
			x = fixed2Int(self->Position.x+(c*16*2)+s+abs_(c))-16 +CAMERA_OFFSET_X-get_camera_x();
			y = fixed2Int(self->Position.y+(-s*16*2)+c)-14-16 +CAMERA_OFFSET_Y-get_camera_y();

			// shouldn't need &FF and &1FF, because x and y should only be in bounds
			obj->attr0 = ATTR0_SQUARE|ATTR0_COLOR_16|ATTR0_ROTSCALE|(y);
			obj->attr1 = ATTR1_SIZE_32|ATTR1_ROTDATA(affine_id&(0x1F))|(x);
			obj->attr2 = OBJ_PALETTE(0)|OBJ_PRIORITY(1)|(Sprite_Grapple);
			
			s16 s2 = fixed16tofixed8(s);
			s16 c2 = fixed16tofixed8(c);
			obj_affine->pa = c2;
			obj_affine->pb = -s2;
			obj_affine->pc = s2;
			obj_affine->pd = c2;
		}
	}else
		grappleCollectLines_being_drawn = false;
}

void player_draw_scarf(struct Player_data* self){
	struct Position_data last = {self->Position.x - int2Fixed(self->facing), self->Position.y - int2Fixed(3)};
	for(int i = 0; i < 5; i++){
		// approach last pos with an offset
		const FIXED_16 twothirds = int2Fixed(2)/3;
		self->scarf[i].x += mulFixed(last.x - self->scarf[i].x - int2Fixed(self->facing), twothirds);
		self->scarf[i].y += ((last.y - self->scarf[i].y) + sin_fixed(int2Fixed(i+1) / 4 + time()) *(i+1)/4) / 2;

		// don't let it get too far
		FIXED_16 dx = self->scarf[i].x - last.x;
		FIXED_16 dy = self->scarf[i].y - last.y;
		//mgba_printf(MGBA_LOG_DEBUG, "dx %f dy %f", fixed2Float(dx), fixed2Float(dy));
		FIXED_16 dist = mulFixed(dx, dx)+mulFixed(dy, dy);
		if(dist > float2Fixed(1.5f*1.5f)){
			dist = Sqrt(dist)<<(FIXED_SHIFT/2);
			FIXED_16 nx = divFixed((self->scarf[i].x -last.x), dist);
			FIXED_16 ny = divFixed((self->scarf[i].y -last.y), dist);
			self->scarf[i].x = last.x + nx *3/2;
			self->scarf[i].y = last.y + ny *3/2;
		}
		
		// fill
		pset(scarf_sprite_buffer, 4, fixed2Int(self->scarf[i].x)-fixed2Int(self->Position.x)+16, fixed2Int(self->scarf[i].y)-fixed2Int(self->Position.y)+16, 10);
		pset(scarf_sprite_buffer, 4, fixed2Int((self->scarf[i].x+last.x))/2-fixed2Int(self->Position.x)+16, fixed2Int((self->scarf[i].y+last.y))/2-fixed2Int(self->Position.y)+16, 10);

		last = self->scarf[i];
	}

	int sprite_x = fixed2Int(self->Position.x) +CAMERA_OFFSET_X-get_camera_x()-16;
	int sprite_y = fixed2Int(self->Position.y) +CAMERA_OFFSET_Y-get_camera_y()-16;
	OBJATTR* obj = get_next_obj();
	obj->attr0 = ATTR0_SQUARE|ATTR0_COLOR_16|(sprite_y&0xFF);
	obj->attr1 = ATTR1_SIZE_32|(sprite_x&0x1FF);
	obj->attr2 = OBJ_PALETTE(0)|OBJ_PRIORITY(1)|(Sprite_Scarf);
}

// this eats performance, but it seems to be fine
IWRAM_CODE void draw_sine_h(struct Player_data* self){
	// sprite will be placed at player position -y16. Therefore offset pixel positions by player position.
	FIXED_16 x0 = int2Fixed(0), 
		x1 = self->grapple_x -self->Position.x, 
		y = int2Fixed(-3); 
	
	const int col = 7;
	FIXED_16 amplitude = 2*self->grapple_wave;
	const int time_freq = 6;
	const FIXED_16 x_freq = float2Fixed(0.08f);
	const FIXED_16 fade_x_dist = int2Fixed(6);

	int x_sign = sgn(x1-x0);
	FIXED_16 x_max = abs_(x1-x0)-int2Fixed(1);
	FIXED_16 last_y = y;
	FIXED_16 this_y = int2Fixed(0);
	FIXED_16 ax = int2Fixed(0);
	FIXED_16 ay = int2Fixed(0);
	FIXED_16 fade = int2Fixed(1);

	for(int i = int2Fixed(1); i <= x_max; i+=int2Fixed(1)){
		if(i <= fade_x_dist)
			fade = divFixed(i, fade_x_dist+int2Fixed(1));
		else if(i > x_max - fade_x_dist + int2Fixed(1))
			fade = divFixed(x_max + int2Fixed(1) - i, fade_x_dist + int2Fixed(1));
		else 
			fade = int2Fixed(1);

		ax = x0 + i*x_sign;
		ay = y + mulFixed(mulFixed(sin_fixed(time()*time_freq + mulFixed(i, x_freq)), amplitude), fade);
		// need to offset psets to fit correctly on the sprite buffer
		pset(grapple_sprite_buffer, 8, fixed2Int(ax) +64*(self->grapple_dir==-1), fixed2Int(ay)+1+16, 1);
		pset(grapple_sprite_buffer, 8, fixed2Int(ax) +64*(self->grapple_dir==-1), fixed2Int(ay)+16, col);

		this_y = ay;
		while(abs_(ay-last_y) > int2Fixed(1)){
			ay -= int2Fixed(1)*sgn(this_y - last_y);
			pset(grapple_sprite_buffer, 8, fixed2Int(ax)-x_sign +64*(self->grapple_dir==-1), fixed2Int(ay)+1+16, 1);
			pset(grapple_sprite_buffer, 8, fixed2Int(ax)-x_sign +64*(self->grapple_dir==-1), fixed2Int(ay)+16, col);
		}
		last_y = this_y;
	}
}

void update_rendered_sprites(){
	// copy scarf sprite from buffer
	CpuFastSet(scarf_sprite_buffer, CHAR_BASE_BLOCK(4)+32*Sprite_Scarf, 32);
	CpuFastSet(scarf_sprite_buffer+32*4, CHAR_BASE_BLOCK(4)+32*Sprite_Scarf + 32*32*1, 32);
	CpuFastSet(scarf_sprite_buffer+32*8, CHAR_BASE_BLOCK(4)+32*Sprite_Scarf+ 32*32*2, 32);
	CpuFastSet(scarf_sprite_buffer+32*12, CHAR_BASE_BLOCK(4)+32*Sprite_Scarf+ 32*32*3, 32);
	vu32 c = 0;
	CpuFastSet((void*)&c, scarf_sprite_buffer, 32*4|FILL);

	if(!grapple_was_rendered)
		return;
	grapple_was_rendered = false;
	// copy grapple sprite from buffer
	CpuFastSet(grapple_sprite_buffer, CHAR_BASE_BLOCK(4)+32*Sprite_Grapple, 64);
	CpuFastSet(grapple_sprite_buffer+64*4, CHAR_BASE_BLOCK(4)+32*Sprite_Grapple + 32*32*1, 64);
	CpuFastSet(grapple_sprite_buffer+64*8, CHAR_BASE_BLOCK(4)+32*Sprite_Grapple+ 32*32*2, 64);
	CpuFastSet(grapple_sprite_buffer+64*12, CHAR_BASE_BLOCK(4)+32*Sprite_Grapple+ 32*32*3, 64);
	
	c=0;
	CpuFastSet((void*)&c, grapple_sprite_buffer, 64*4|FILL);
}

void pset(u8* buffer, int tile_width, int x, int y, int color){
	// same position should not be rewritten with another color
	if(x < 0 || y < 0 || x >= tile_width*8 || y >= 32){
		// should only draw out of bounds, when grapple line is small 
		// and therefore hidden by the player sprite anyway.
		return; 
		//mgba_printf(MGBA_LOG_DEBUG, "%d %d %d", x, y, grapple_x_dir);
#ifdef DEBUG
		//errorHalt("pset");
#endif
	}
	color = color<<(4*(x&1));
	x >>= 1;
	int tile_index = (x/4 + y/8*tile_width) * 32;
	int in_tile_index = (y%8)*4 + (x%4);
	int buffer_index = tile_index + in_tile_index;

	buffer[buffer_index] |= color;
}

void line(u8* buffer, int tile_width, int x0, int y0, int x1, int y1, int color){
	// Bresenham's line algorithm
	int dx = x1-x0;
	int dy = y1-y0;
	int sx = sgn(dx);
	int sy = sgn(dy);
	dx = abs_(dx);
	dy = abs_(dy);
	int err = dx-dy;

	while(true){
		pset(grapple_sprite_buffer, tile_width, x0, y0, color);
		if(x0 == x1 && y0 == y1) break;

		int e2 = err*2;
		if(e2 > -dy){
			err -= dy;
			x0 += sx;
		}
		if(e2 < dx){
			err += dx;
			y0 += sy;
		}
	}
}