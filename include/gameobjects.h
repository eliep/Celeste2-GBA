#ifndef GAMEOBJECTS_H
#define GAMEOBJECTS_H

#include "pico8_math.h"

// max number of snowballs existing at one time.
#define MAX_SNOWBALL_COUNT (10)
// should be at least the sum of all berries_len in level_object_data 
#define BERRY_COUNT 16
#define MAX_BERRY_COUNT 6
// should be at least the sum of all spikeGroups_len in level_object_data 
#define MAX_SPIKEGROUPS_COUNT 34
// one bridge is 2 objects
#define BRIDGE_COUNT 2

struct Position_data{
    FIXED_16 x, y;
};

struct Hitbox_data{
    FIXED_16 hit_x, hit_y;
    FIXED_16 hit_w, hit_h;
};

struct Movement_data{
    FIXED_16 remainder_x, remainder_y;
    FIXED_16 speed_x, speed_y;
};

struct GrapplePickup_data{
    struct Position_data Position;

    bool destroyed;
};

enum SpikeDirection{
    SpikeDirection_Up, SpikeDirection_Left, SpikeDirection_Down, SpikeDirection_Right
};

// Represents a line of spikes
// used to make collision more efficient
struct SpikeGroup_data{
    struct Position_data Position;
    struct Hitbox_data Hitbox; 

    enum SpikeDirection hazard;
};

struct Holdable_data{
    struct Position_data Position;
    struct Hitbox_data Hitbox;
    struct Movement_data Movement;

    // grapple_mode = 3, 
    // bool holdable; // unused
    int thrown_timer, freeze;
    bool held;
};

// because Holdable is at first position I should be able to cast Snowballs and Springboard into Holdables
struct Snowball_data{
    struct Holdable_data Holdable;
    int hp;
    bool destroyed;
    bool stop;
};

struct Springboard_data{
    struct Holdable_data Holdable;
    bool destroyed;
    bool player_assigned; // used instead of player struct from current level objects.
    bool spr_pressed; // used instead of setting sprite id; 12-pressed, 11-normal
};

struct Grappler_data{ // Red air thing
    struct Position_data Position;
    struct Hitbox_data Hitbox;
    // grapple_mode = 2
};

struct Bridge_data{
    struct Position_data Position;
    bool falling;
};

struct Berry_data{
    struct Position_data Position;
    struct Hitbox_data Hitbox;

    int timer, flash, ground, id;
    bool destroyed;
    bool collected;
    bool picked_up; // used instead of player struct from current level objects.
};

enum CrumbleState{
    Crumble_Normal,
    Crumble_Breaking,
    Crumble_Broken//i.e. gone
};

struct Crumble_data{
    struct Position_data Position;
    struct Hitbox_data Hitbox;
    // solid = true, grapple_mode = 1
    
    //FIXED_16 ox, oy; // unused here, instead just draw empty tile
    int time;
    // instead of bool use breaking state
    // also needed for collision
    enum CrumbleState breaking;
};

struct Checkpoint_data{
    struct Position_data Position;
};

struct Spawner_data{
    struct Position_data Position;
    int dir, timer;
};

enum GrappleHit_Type{
    GrappleHit_Nothing = -1,
    GrappleHit_Snowball,
    GrappleHit_Springboard,
    GrappleHit_Grappler,
    GrappleHit_Crumble
};

struct Player_data{
    struct Position_data Position;
    struct Hitbox_data Hitbox;
    struct Movement_data Movement;

    struct Position_data scarf[5];
    int scarf_len;

    struct Springboard_data* springboard;
    enum GrappleHit_Type grapple_hit_type;
    /* I'm not happy about the void*, but I don't know how to nicely overload.
      _Generic or union types => I have to write type distinctions anyways
      => I guess void* it is. */
    void* grapple_hit_object;
    enum GrappleHit_Type holding_type;
    void* holding_object;

    FIXED_16 grapple_x, grapple_y,
        var_jump_speed,
        jump_grace_y,
        grapple_jump_grace_y,
        grapple_wave;
    int t_jump_grace,
        t_var_jump,
        grapple_dir,
        t_grapple_cooldown,
        wipe_timer,
        t_grapple_jump_grace,
        t_grapple_pickup,
        state,
        freeze;
    int last_berry_id,
        facing;
    FIXED_16 spr; // numbered 0-3 instead of 2-5, walking animation is done with fixed point stuff. Probably could have just used an if instead. Whatever.
    bool auto_var_jump,
         grapple_boost,
         grapple_retract,
         finished;
};

extern struct CurrentObjectData{
    struct Player_data player;
    struct Bridge_data bridge[BRIDGE_COUNT];
    bool bridge_enabled;
    struct GrapplePickup_data grapplePickup;
    bool grapplePickup_enabled;
    struct SpikeGroup_data spikeGroups[MAX_SPIKEGROUPS_COUNT];
    int spikeGroups_len;
    struct Berry_data berries[MAX_BERRY_COUNT];
    int berries_len;
    struct Checkpoint_data checkpoints[11];
    int checkpoints_len;
    struct Grappler_data grapplers[16];
    int grapplers_len;
    struct Crumble_data crumbles[19];
    int crumbles_len;
    struct Snowball_data snowballs[MAX_SNOWBALL_COUNT]; // can be more than level_object_data->snowballs_len, because spawners can make more
    struct Springboard_data springboards[5];
    int springboards_len;
    struct Spawner_data spawners[5];
    int spawners_len;
} current_gameobjects;

struct SpikeGroup_definition{
    struct Position_data Position; // position of upper left most spike of group
    enum SpikeDirection dir; // defines direction, in which spikes are pointing
    int count; // count of spike tiles in each group
};

struct Berry_definition{
    struct Position_data Position;
    int id; // used for tracking whichs berries have been collected
};

extern const struct LevelObjectData {
    struct Position_data player;
    struct Position_data bridge[BRIDGE_COUNT];
    bool bridge_enabled;
    struct Position_data grapplePickup;
    bool grapplePickup_enabled;
    struct SpikeGroup_definition spikeGroups[MAX_SPIKEGROUPS_COUNT];
    int spikeGroups_len;
    struct Berry_definition berries[MAX_BERRY_COUNT]; // array length should fit max number found in all levels
    int berries_len;
    struct Position_data checkpoints[11];
    int checkpoints_len;
    struct Position_data grapplers[16];
    int grapplers_len;
    struct Position_data crumbles[19];
    int crumbles_len;
    struct Position_data snowballs[4]; // snowballs which just lie on the ground
    int snowballs_len;
    struct Position_data springboards[5];
    int springboards_len;
    struct Position_data spawners[5];
    int spawners_dir[5];
    int spawners_len;
} level_object_data[8];

extern bool collected[BERRY_COUNT];
extern int berry_count;
extern struct Checkpoint_data* level_checkpoint;

void update_level_objects();
void load_level_objects(int level_index);

void update_input();


#endif