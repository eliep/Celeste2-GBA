#include "gameobjects.h"

#include "main.h"
#include "pico8.h"
#include "map_collision.h"
#include "soundbank.h" // will be generated on first build
//#include "mgba.h"

// need *8, because have to convert from tile coordinates to pixel coordinates
#define INIT_POSITION(X, Y) {.x = int2Fixed((X)*8), .y = int2Fixed((Y)*8)}

const struct Hitbox_data DEFAULT_HITBOX = {.hit_x = int2Fixed(0), .hit_y = int2Fixed(0), .hit_w = int2Fixed(8), .hit_h = int2Fixed(8)};
const struct Movement_data NO_MOVEMENT = {.remainder_x = int2Fixed(0), .remainder_y = int2Fixed(0), .speed_x = int2Fixed(0), .speed_y = int2Fixed(0)};

struct CurrentObjectData current_gameobjects;

const struct LevelObjectData level_object_data[8] = {
    { // Level 1
    .player = INIT_POSITION(6,12),
    .bridge = {INIT_POSITION(88, 11), INIT_POSITION(89, 11)},
    .bridge_enabled = true,
    .grapplePickup = {0},
    .grapplePickup_enabled = false,
    .spikeGroups = {{{0}, 0, 0}},
    .spikeGroups_len = 0,
    .berries = {{{0},0}},
    .berries_len = 0,
    .checkpoints = {{0}},
    .checkpoints_len = 0,
    .grapplers = {{0}},
    .grapplers_len = 0,
    .crumbles = {{0}},
    .crumbles_len = 0,
    .snowballs = {INIT_POSITION(24, 9), INIT_POSITION(32, 3)},
    .snowballs_len = 2,
    .springboards = {{0}},
    .springboards_len = 0,
    .spawners = {{0}},
    .spawners_dir = {0},
    .spawners_len = 0
    },
    { // Level 2
    .player = INIT_POSITION(7, 0),
    .bridge = {{0}},
    .bridge_enabled = false,
    .grapplePickup = INIT_POSITION(28, 25),
    .grapplePickup_enabled = true,
    .spikeGroups = {{{0}, 0, 0}},
    .spikeGroups_len = 0,
    .berries = {{INIT_POSITION(22, 2), 0}},
    .berries_len = 1,
    .checkpoints = {{0}},
    .checkpoints_len = 0,
    .grapplers = {{0}},
    .grapplers_len = 0,
    .crumbles = {{0}},
    .crumbles_len = 0,
    .snowballs = {{0}},
    .snowballs_len = 0,
    .springboards = {{0}},
    .springboards_len = 0,
    .spawners = {{0}},
    .spawners_dir = {0},
    .spawners_len = 0
    },
    { // Level 3
    .player = INIT_POSITION(1, 18),
    .bridge = {{0}},
    .bridge_enabled = false,
    .grapplePickup = {0},
    .grapplePickup_enabled = false,
    .spikeGroups = {{INIT_POSITION(6, 16), SpikeDirection_Left, 3}, {INIT_POSITION(23, 17), SpikeDirection_Left, 5}, {INIT_POSITION(28, 18), SpikeDirection_Up, 9},
                    {INIT_POSITION(30, 12), SpikeDirection_Right, 3}, {INIT_POSITION(33, 7), SpikeDirection_Left, 7}, {INIT_POSITION(40, 9), SpikeDirection_Right, 3},
                    {INIT_POSITION(43, 13), SpikeDirection_Up, 1}, {INIT_POSITION(49, 9), SpikeDirection_Up, 2}, {INIT_POSITION(50, 18), SpikeDirection_Right, 1},
                    {INIT_POSITION(51, 11), SpikeDirection_Up, 1}, {INIT_POSITION(52, 14), SpikeDirection_Right, 1}, {INIT_POSITION(83, 11), SpikeDirection_Left, 2},
                    {INIT_POSITION(83, 15), SpikeDirection_Left, 2}, {INIT_POSITION(89, 13), SpikeDirection_Right, 2}, {INIT_POSITION(89, 17), SpikeDirection_Right, 2},
                    {INIT_POSITION(118, 13), SpikeDirection_Up, 1}, {INIT_POSITION(123, 10), SpikeDirection_Left, 3}},
    .spikeGroups_len = 17,
    .berries = {{INIT_POSITION(16, 17), 1}, {INIT_POSITION(36, 16), 2}, {INIT_POSITION(54, 10), 3}, {INIT_POSITION(66, 3), 4}, {INIT_POSITION(118, 11), 5}},
    .berries_len = 5,
    .checkpoints = {INIT_POSITION(27, 12), INIT_POSITION(40, 20), INIT_POSITION(59, 19), INIT_POSITION(91, 10)},
    .checkpoints_len = 4,
    .grapplers = {INIT_POSITION(21, 11), INIT_POSITION(69, 13)},
    .grapplers_len = 2,
    .crumbles = {INIT_POSITION(36, 20), INIT_POSITION(43, 17), INIT_POSITION(43, 18), INIT_POSITION(50, 17),  INIT_POSITION(51, 17),  INIT_POSITION(52, 17),
                 INIT_POSITION(99, 17), INIT_POSITION(99, 18), INIT_POSITION(106, 17), INIT_POSITION(106, 18), 
                 INIT_POSITION(98, 20), INIT_POSITION(99, 20), INIT_POSITION(100, 20), INIT_POSITION(105, 20), INIT_POSITION(106, 20), INIT_POSITION(107, 20)},
    .crumbles_len = 16,
    .snowballs = {{0}},
    .snowballs_len = 0,
    .springboards = {INIT_POSITION(62, 19), INIT_POSITION(110, 20), INIT_POSITION(118, 20)},
    .springboards_len = 3,
    .spawners = {{0}},
    .spawners_dir = {0},
    .spawners_len = 0
    },
    { // Level 4
    .player = INIT_POSITION(1, 9),
    .bridge = {{0}},
    .bridge_enabled = false,
    .grapplePickup = {0},
    .grapplePickup_enabled = false,
    .spikeGroups = {{INIT_POSITION(3, 11), SpikeDirection_Up, 2}, {INIT_POSITION(5, 10), SpikeDirection_Up, 1}, {INIT_POSITION(7, 10), SpikeDirection_Up, 1},
                    {INIT_POSITION(8, 12), SpikeDirection_Up, 2}, {INIT_POSITION(20, 10), SpikeDirection_Up, 2}, {INIT_POSITION(22, 11), SpikeDirection_Up, 6},
                    {INIT_POSITION(43, 6), SpikeDirection_Left, 3}, {INIT_POSITION(49, 14), SpikeDirection_Up, 7}, {INIT_POSITION(56, 2), SpikeDirection_Up, 1},
                    {INIT_POSITION(56, 12), SpikeDirection_Up, 1}, {INIT_POSITION(57, 14), SpikeDirection_Up, 6}, {INIT_POSITION(46, 25), SpikeDirection_Left, 3},
                    {INIT_POSITION(29, 26), SpikeDirection_Up, 1}, { INIT_POSITION(22, 30), SpikeDirection_Up, 7}, /*{INIT_POSITION(15, 30), SpikeDirection_Up, 3},*/
                    {INIT_POSITION(7, 30), SpikeDirection_Up, 7}, {INIT_POSITION(2, 29), SpikeDirection_Up, 5}, {INIT_POSITION(84, 17), SpikeDirection_Down, 2},
                    {INIT_POSITION(86, 9), SpikeDirection_Up, 1}, {INIT_POSITION(87, 10), SpikeDirection_Up, 5}, {INIT_POSITION(92, 9), SpikeDirection_Up, 3},
                    {INIT_POSITION(65, 14), SpikeDirection_Up, 14}, {INIT_POSITION(99, 6), SpikeDirection_Up, 10}, {INIT_POSITION(109, 5), SpikeDirection_Up, 1},
                    {INIT_POSITION(98, 14), SpikeDirection_Up, 13}, {INIT_POSITION(99, 22), SpikeDirection_Up, 2}, {INIT_POSITION(101, 23), SpikeDirection_Right, 1},
                    {INIT_POSITION(99, 24), SpikeDirection_Down, 2}, {INIT_POSITION(109, 17), SpikeDirection_Left, 3}, {INIT_POSITION(109, 26), SpikeDirection_Up, 2},
                    {INIT_POSITION(108, 27), SpikeDirection_Left, 1}, {INIT_POSITION(109, 28), SpikeDirection_Down, 2}, {INIT_POSITION(117, 18), SpikeDirection_Right, 9},
                    {INIT_POSITION(126, 20), SpikeDirection_Left, 12}},
    .spikeGroups_len = 33,
    .berries = {{INIT_POSITION(61, 8), 6}, {INIT_POSITION(46, 17), 7}, {INIT_POSITION(23, 18), 8}, {INIT_POSITION(11, 29), 9}, {INIT_POSITION(93, 28), 10}, {INIT_POSITION(78, 12), 11}},
    .berries_len = 6,
    .checkpoints = {INIT_POSITION(13, 18), INIT_POSITION(29, 24), INIT_POSITION(50, 3), INIT_POSITION(50, 30), INIT_POSITION(66, 25), 
                    INIT_POSITION(76, 3), INIT_POSITION(82, 14), INIT_POSITION(82, 29), INIT_POSITION(99, 4), INIT_POSITION(100, 30), 
                    INIT_POSITION(114, 21)},
    .checkpoints_len = 11,
    .grapplers = {INIT_POSITION(56, 27), INIT_POSITION(66, 8), INIT_POSITION(71, 4), INIT_POSITION(71, 29), INIT_POSITION(73, 10), INIT_POSITION(73, 24), 
                  INIT_POSITION(76, 19), INIT_POSITION(89, 28), INIT_POSITION(102, 23), INIT_POSITION(106, 19), INIT_POSITION(107, 27), 
                  INIT_POSITION(121, 22), INIT_POSITION(121, 26), INIT_POSITION(122, 20), INIT_POSITION(122, 24), INIT_POSITION(122, 28)},
    .grapplers_len = 16,
    .crumbles = {INIT_POSITION(25, 28), INIT_POSITION(26, 28), INIT_POSITION(60, 1), INIT_POSITION(60, 2)},
    .crumbles_len = 4,
    .snowballs = {INIT_POSITION(6, 10), INIT_POSITION(1, 21), INIT_POSITION(92, 14), INIT_POSITION(97, 4)},
    .snowballs_len = 4,
    .springboards = {INIT_POSITION(1, 27), INIT_POSITION(30, 26), INIT_POSITION(36, 10), INIT_POSITION(52, 9), INIT_POSITION(84, 14)},
    .springboards_len = 5,
    .spawners = {{0}},
    .spawners_dir = {0},
    .spawners_len = 0
    },
    { // Level 5
    .player = INIT_POSITION(1, 12),
    .bridge = {{0}},
    .bridge_enabled = false,
    .grapplePickup = {0},
    .grapplePickup_enabled = false,
    .spikeGroups = {{INIT_POSITION(23, 4), SpikeDirection_Left, 1}, {INIT_POSITION(24, 3), SpikeDirection_Up, 1}, {INIT_POSITION(25, 4), SpikeDirection_Right, 1},
                    {INIT_POSITION(30, 7), SpikeDirection_Left, 1}, {INIT_POSITION(31, 6), SpikeDirection_Up, 1}, {INIT_POSITION(32, 7), SpikeDirection_Right, 1},
                    {INIT_POSITION(77, 7), SpikeDirection_Left, 3}, {INIT_POSITION(78, 6), SpikeDirection_Up, 2}, {INIT_POSITION(80, 7), SpikeDirection_Up, 2},
                    {INIT_POSITION(82, 8), SpikeDirection_Right, 3}, {INIT_POSITION(104, 12), SpikeDirection_Up, 4}, {INIT_POSITION(122, 5), SpikeDirection_Left, 3}},
    .spikeGroups_len = 12,
    .berries = {{INIT_POSITION(48, 11), 12}},
    .berries_len = 1,
    .checkpoints = {INIT_POSITION(87, 10)},
    .checkpoints_len = 1,
    .grapplers = {INIT_POSITION(37, 11), INIT_POSITION(54, 11), INIT_POSITION(62, 9), INIT_POSITION(75, 3), INIT_POSITION(118, 5)},
    .grapplers_len = 5,
    .crumbles = {INIT_POSITION(8, 12), INIT_POSITION(9, 12), INIT_POSITION(13, 9), INIT_POSITION(14, 9), INIT_POSITION(24, 5), INIT_POSITION(24, 6), 
                 INIT_POSITION(31, 8), INIT_POSITION(31, 9), INIT_POSITION(42, 10), INIT_POSITION(43, 10), INIT_POSITION(47, 8), INIT_POSITION(48, 8), 
                 INIT_POSITION(52, 7), INIT_POSITION(53, 7), INIT_POSITION(67, 9), INIT_POSITION(92, 12), INIT_POSITION(95, 8), INIT_POSITION(100, 10), 
                 INIT_POSITION(101, 10)},
    .crumbles_len = 19,
    .snowballs = {{0}},
    .snowballs_len = 0,
    .springboards = {INIT_POSITION(67, 8), INIT_POSITION(92, 11), INIT_POSITION(95, 7), INIT_POSITION(103, 1)},
    .springboards_len = 4,
    .spawners = {{0}},
    .spawners_dir = {0},
    .spawners_len = 0
    },
    { // Level 6
    .player = INIT_POSITION(1, 8),
    .bridge = {{0}},
    .bridge_enabled = false,
    .grapplePickup = {0},
    .grapplePickup_enabled = false,
    .spikeGroups = {{INIT_POSITION(34, 10), SpikeDirection_Left, 3}, {INIT_POSITION(40, 9), SpikeDirection_Up, 11}, {INIT_POSITION(53, 11), SpikeDirection_Up, 8},
                    {INIT_POSITION(86, 4), SpikeDirection_Left, 2}, {INIT_POSITION(87, 3), SpikeDirection_Up, 2}, {INIT_POSITION(89, 4), SpikeDirection_Right, 4},
                    {INIT_POSITION(126, 12), SpikeDirection_Left, 4}},
    .spikeGroups_len = 7,
    .berries = {{INIT_POSITION(88, 1), 13}, {INIT_POSITION(125, 13), 14}},
    .berries_len = 2,
    .checkpoints = {INIT_POSITION(35, 8), INIT_POSITION(76, 9), INIT_POSITION(102, 9), INIT_POSITION(106, 9)},
    .checkpoints_len = 4,
    .grapplers = {INIT_POSITION(9, 6), INIT_POSITION(29, 7), INIT_POSITION(43, 6), INIT_POSITION(69, 7), INIT_POSITION(97, 11), 
                  INIT_POSITION(101, 6), INIT_POSITION(123, 3)},
    .grapplers_len = 7,
    .crumbles = {INIT_POSITION(15, 8), INIT_POSITION(15, 14), INIT_POSITION(20, 13)},
    .crumbles_len = 3,
    .snowballs = {{0}},
    .snowballs_len = 0,
    .springboards = {INIT_POSITION(15, 7), INIT_POSITION(19, 1)},
    .springboards_len = 2,
    .spawners = {INIT_POSITION(35, 0), INIT_POSITION(106, 0), INIT_POSITION(115, 0), INIT_POSITION(117, 0), INIT_POSITION(126, 0)},
    .spawners_dir = {1, 1, -1, 1, -1},
    .spawners_len = 5
    },
    { // Level 7
    .player = INIT_POSITION(1, 11),
    .bridge = {{0}},
    .bridge_enabled = false,
    .grapplePickup = {0},
    .grapplePickup_enabled = false,
    .spikeGroups = {{INIT_POSITION(8, 4), SpikeDirection_Up, 1}, {INIT_POSITION(8, 7), SpikeDirection_Down, 1}, {INIT_POSITION(16, 2), SpikeDirection_Up, 1},
                    {INIT_POSITION(16, 5), SpikeDirection_Down, 1}, {INIT_POSITION(20, 5), SpikeDirection_Left, 3}, {INIT_POSITION(21, 8), SpikeDirection_Down, 1},
                    {INIT_POSITION(40, 13), SpikeDirection_Up, 1}, {INIT_POSITION(42, 12), SpikeDirection_Up, 1},
                    {INIT_POSITION(60, 9), SpikeDirection_Left, 1}, {INIT_POSITION(61, 8), SpikeDirection_Up, 1}, {INIT_POSITION(62, 9), SpikeDirection_Right, 1},
                    {INIT_POSITION(68, 9), SpikeDirection_Left, 1}, {INIT_POSITION(69, 8), SpikeDirection_Up, 1}, {INIT_POSITION(70, 9), SpikeDirection_Right, 1},
                    {INIT_POSITION(79, 5), SpikeDirection_Left, 1}, {INIT_POSITION(80, 4), SpikeDirection_Up, 1}, {INIT_POSITION(81, 5), SpikeDirection_Right, 1},
                    {INIT_POSITION(89, 8), SpikeDirection_Left, 1}, {INIT_POSITION(90, 7), SpikeDirection_Up, 1}, {INIT_POSITION(91, 8), SpikeDirection_Right, 1},
                    {INIT_POSITION(82, 10), SpikeDirection_Left, 1}, {INIT_POSITION(83, 9), SpikeDirection_Up, 1}, {INIT_POSITION(84, 10), SpikeDirection_Right, 1},
                    {INIT_POSITION(87, 12), SpikeDirection_Left, 1}, {INIT_POSITION(88, 11), SpikeDirection_Up, 1}, {INIT_POSITION(89, 12), SpikeDirection_Right, 1},
                    {INIT_POSITION(123, 5), SpikeDirection_Left, 6}},
    .spikeGroups_len = 27,
    .berries = {{INIT_POSITION(85, 12), 15}},
    .berries_len = 1,
    .checkpoints = {INIT_POSITION(28, 7), INIT_POSITION(54, 11), INIT_POSITION(104, 12)},
    .checkpoints_len = 3,
    .grapplers = {INIT_POSITION(50, 13), INIT_POSITION(96, 6), INIT_POSITION(114, 10)},
    .grapplers_len = 3,
    .crumbles = {INIT_POSITION(8, 13), INIT_POSITION(73, 12)},
    .crumbles_len = 2,
    .snowballs = {{0}},
    .snowballs_len = 0,
    .springboards = {INIT_POSITION(8, 12), INIT_POSITION(24, 13), INIT_POSITION(73, 11)},
    .springboards_len = 3,
    .spawners = {INIT_POSITION(36, 0)},
    .spawners_dir = {-1},
    .spawners_len = 1
    },
    { // Level 8
    .player = INIT_POSITION(1,3),
    .bridge = {{0}},
    .bridge_enabled = false,
    .grapplePickup = {0},
    .grapplePickup_enabled = false,
    .spikeGroups = {{{0}, 0, 0}},
    .spikeGroups_len = 0,
    .berries = {{{0},0}},
    .berries_len = 0,
    .checkpoints = {{0}},
    .checkpoints_len = 0,
    .grapplers = {{0}},
    .grapplers_len = 0,
    .crumbles = {{0}},
    .crumbles_len = 0,
    .snowballs = {{0}},
    .snowballs_len = 0,
    .springboards = {{0}},
    .springboards_len = 0,
    .spawners = {{0}},
    .spawners_dir = {0},
    .spawners_len = 0
    }
};

bool collected[BERRY_COUNT] = {false};
int berry_count = 0;
struct Checkpoint_data* level_checkpoint = NULL;

// forward declarations

static int tile_y(int py);

static bool object_move_x(struct Position_data* self_Position, const struct Hitbox_data* self_Hitbox, struct Movement_data* self_Movement, FIXED_16 x, bool (*on_collide)(void*, int), void* self);
static bool object_move_x(struct Position_data* self_Position, const struct Hitbox_data* self_Hitbox, struct Movement_data* self_Movement, FIXED_16 y, bool (*on_collide)(void*, int), void* self);
static bool object_on_collide_x(const struct Position_data* self_Position, const struct Hitbox_data* self_Hitbox, struct Movement_data* self_Movement, int target);
static bool object_on_collide_y(const struct Position_data* self_Position, const struct Hitbox_data* self_Hitbox, struct Movement_data* self_Movement, int target);
static bool object_overlaps(const struct Position_data* self_Position, const struct Hitbox_data* self_Hitbox, 
                     const struct Position_data* other_Position, const struct Hitbox_data* other_Hitbox, 
                     FIXED_16 ox, FIXED_16 oy);
// contains is only used for grapple check
static bool object_contains(const struct Position_data* self_Position, const struct Hitbox_data* self_Hitbox, FIXED_16 px, FIXED_16 py);
static bool object_check_solid(const struct Position_data* self_Position, const struct Hitbox_data* self_Hitbox, FIXED_16 ox, FIXED_16 oy);
// func is only used with player.correction_func, therefore replace with boolean
static bool object_corner_correct(struct Position_data* self_Position, const struct Hitbox_data* self_Hitbox, 
                           int dir_x, int dir_y, int side_dist, int look_ahead, int only_sign, bool use_correction_func);

static void grapplepickup_init(struct GrapplePickup_data* self, const struct Position_data* position);

static void spikeGroup_init(struct SpikeGroup_data* self, const struct SpikeGroup_definition* spikegroup_definition);

static void snowball_init(struct Snowball_data* self, const struct Position_data* position, FIXED_16 speed_x, FIXED_16 speed_y);
static void snowball_update(struct Snowball_data* self);
static bool snowball_on_collide_x(struct Snowball_data* self, int total);
static bool snowball_on_collide_y(struct Snowball_data* self, int total);
static void snowball_on_release(struct Holdable_data* self, bool thrown);
static bool snowball_hurt(struct Snowball_data* self);
static bool snowball_bounce_overlaps(struct Snowball_data* self, const struct Player_data* other);
static bool snowball_contains(const struct Snowball_data* self, FIXED_16 px, FIXED_16 py);

static void springboard_init(struct Springboard_data* self, const struct Position_data* position);
static void springboard_update(struct Springboard_data* self);
static bool springboard_on_collide_x(struct Springboard_data* self, int total);
static bool springboard_on_collide_y(struct Springboard_data* self, int total);
static void springboard_on_release(struct Holdable_data* self, bool thrown);

static void grappler_init(struct Grappler_data* self, const struct Position_data* position);

static void bridge_init(struct Bridge_data* self, const  struct Position_data* position);
static void bridge_update(struct Bridge_data* self);

static void berry_init(struct Berry_data* self, const struct Berry_definition* berry_definition);
static void berry_update(struct Berry_data* self);
static void berry_collect(struct Berry_data* self);

static void crumble_init(struct Crumble_data* self, const struct Position_data* position);
static void crumble_update(struct Crumble_data* self);

static void checkpoint_init(struct Checkpoint_data* self, const struct Position_data* position);

static void spawner_init(struct Spawner_data* self, const struct Position_data* position, int dir);
static void spawner_update(struct Spawner_data* self);

static bool consume_jump_press();
static bool consume_grapple_press();

static void player_init(struct Player_data* self, const struct Position_data* position);
static void player_update(struct Player_data* self);
static void player_start_grapple(struct Player_data* self);
static int player_grapple_check(struct Player_data* self, FIXED_16 x, FIXED_16 y);
static void player_jump(struct Player_data* self);
static void player_bounce(struct Player_data* self, FIXED_16 x, FIXED_16 y);
static void player_spring(struct Player_data* self);
static void player_wall_jump(struct Player_data* self, int dir);
static void player_grapple_jump(struct Player_data* self);
static bool player_bounce_check(struct Player_data* self, FIXED_16 other_speed_y, FIXED_16 other_y);
static void player_die(struct Player_data* self);
static bool player_hazard_check(const struct Player_data* self, FIXED_16 ox, FIXED_16 oy);
static bool player_correction_func(const struct Player_data* self, FIXED_16 ox, FIXED_16 oy);
static bool player_on_collide_x(struct Player_data* self, int target);
static bool player_on_collide_y(struct Player_data* self, int target);

// implementations

void update_level_objects(){
    for(int i = 0; i < current_gameobjects.spawners_len; i++)
        spawner_update(current_gameobjects.spawners+i);
    if(current_gameobjects.bridge_enabled)
        for(int i = 0; i < BRIDGE_COUNT; i++)
            bridge_update(current_gameobjects.bridge+i);
    for(int i = 0; i < MAX_SNOWBALL_COUNT; i++)
        if(!current_gameobjects.snowballs[i].destroyed){
            if(current_gameobjects.snowballs[i].Holdable.freeze > 0)
                current_gameobjects.snowballs[i].Holdable.freeze -= 1;
            else
                snowball_update(current_gameobjects.snowballs+i);
        }
    for(int i = 0; i < current_gameobjects.berries_len; i++)
        if(!current_gameobjects.berries[i].destroyed)
            berry_update(current_gameobjects.berries+i);
    for(int i = 0; i < current_gameobjects.crumbles_len; i++)
        crumble_update(current_gameobjects.crumbles+i);
    for(int i = 0; i < current_gameobjects.springboards_len; i++)
        if(!current_gameobjects.springboards[i].destroyed){
            if(current_gameobjects.springboards[i].Holdable.freeze > 0)
                current_gameobjects.springboards[i].Holdable.freeze -= 1;
            else
                springboard_update(current_gameobjects.springboards+i);
        }
    if(current_gameobjects.player.freeze > 0)
        current_gameobjects.player.freeze -= 1;
    else{
        player_update(&current_gameobjects.player);
    }
}

void load_level_objects(int level_index){
    if(level_index < 1) return;

    const struct LevelObjectData* lvlod = &(level_object_data[level_index-1]);

    // set position to checkpoint, if available (will be set NULL on goto_level)
    if(level_checkpoint == NULL)
        player_init(&(current_gameobjects.player), &(lvlod->player));
    else
        player_init(&(current_gameobjects.player), &level_checkpoint->Position);
    current_gameobjects.bridge_enabled = lvlod->bridge_enabled;
    if(current_gameobjects.bridge_enabled)
        for(int i = 0; i < BRIDGE_COUNT; i++)
            bridge_init(current_gameobjects.bridge+i, lvlod->bridge+i);
    current_gameobjects.grapplePickup_enabled = lvlod->grapplePickup_enabled;
    if(current_gameobjects.grapplePickup_enabled)
        grapplepickup_init(&current_gameobjects.grapplePickup, &lvlod->grapplePickup);
    current_gameobjects.spikeGroups_len = lvlod->spikeGroups_len;
    for(int i = 0; i < current_gameobjects.spikeGroups_len; i++)
        spikeGroup_init(current_gameobjects.spikeGroups+i, lvlod->spikeGroups+i);
    current_gameobjects.berries_len = lvlod->berries_len;
    for(int i = 0; i < current_gameobjects.berries_len; i++)
        if(!collected[lvlod->berries[i].id])
            berry_init(current_gameobjects.berries+i, lvlod->berries+i);
    current_gameobjects.checkpoints_len = lvlod->checkpoints_len;
    for(int i = 0; i < current_gameobjects.checkpoints_len; i++)
        checkpoint_init(current_gameobjects.checkpoints+i, lvlod->checkpoints+i);
    current_gameobjects.grapplers_len = lvlod->grapplers_len;
    for(int i = 0; i < current_gameobjects.grapplers_len; i++)
        grappler_init(current_gameobjects.grapplers+i, lvlod->grapplers+i);
    current_gameobjects.crumbles_len = lvlod->crumbles_len;
    for(int i = 0; i < current_gameobjects.crumbles_len; i++)
        crumble_init(current_gameobjects.crumbles+i, lvlod->crumbles+i);
    for(int i = 0; i < lvlod->snowballs_len; i++)
        snowball_init(current_gameobjects.snowballs+i, lvlod->snowballs+i, int2Fixed(0), int2Fixed(0));
    for(int i = lvlod->snowballs_len; i < MAX_SNOWBALL_COUNT; i++)
        current_gameobjects.snowballs[i].destroyed = true;
    current_gameobjects.springboards_len = lvlod->springboards_len;
    for(int i = 0; i < current_gameobjects.springboards_len; i++)
        springboard_init(current_gameobjects.springboards+i, lvlod->springboards+i);
    current_gameobjects.spawners_len = lvlod->spawners_len;
    for(int i = 0; i < current_gameobjects.spawners_len; i++)
        spawner_init(current_gameobjects.spawners+i, lvlod->spawners+i, lvlod->spawners_dir[i]);
}


// get position of a pixel on tile grid
static int tile_y(int py){
	return max2(0, min2((py / 8), level.height - 1));
}

// Object functions
static bool object_move_x(struct Position_data* self_Position, const struct Hitbox_data* self_Hitbox, struct Movement_data* self_Movement, FIXED_16 x, bool (*on_collide)(void*, int), void* self){
    self_Movement->remainder_x += x;
    int mx = fixed2Int(self_Movement->remainder_x + float2Fixed(0.5f));
    self_Movement->remainder_x -= int2Fixed(mx);
    
    int total = mx;
    int mxs = sgn(mx);
    while(mx != 0){
        //mgba_printf(MGBA_LOG_DEBUG, " mx: %d!=0", mx);
        if(object_check_solid(self_Position, self_Hitbox, int2Fixed(mxs), int2Fixed(0))){
            if(on_collide != NULL)
                return on_collide(self, total);
            return true;
        }else{
            self_Position->x += int2Fixed(mxs);
            mx -= mxs;
        }
    }

    return false;
}

static bool object_move_y(struct Position_data* self_Position, const struct Hitbox_data* self_Hitbox, struct Movement_data* self_Movement, FIXED_16 y, bool (*on_collide)(void*, int), void* self){
    self_Movement->remainder_y += y;
    int my = fixed2Int(self_Movement->remainder_y + float2Fixed(0.5f));
    self_Movement->remainder_y -= int2Fixed(my);

    int total = my;
    int mys = sgn(my);
    while(my != 0){
        if(object_check_solid(self_Position, self_Hitbox, int2Fixed(0), int2Fixed(mys))){
            if(on_collide != NULL)
                return on_collide(self, total);
            return true;
        }else{
            self_Position->y += int2Fixed(mys);
            my -= mys;
        }
    }

    return false;
}

static bool object_on_collide_x(const struct Position_data* self_Position, const struct Hitbox_data* self_Hitbox, struct Movement_data* self_Movement, int target){ // moved is useless
    self_Movement->remainder_x = int2Fixed(0);
    self_Movement->speed_x = int2Fixed(0);
    return true;
}

static bool object_on_collide_y(const struct Position_data* self_Position, const struct Hitbox_data* self_Hitbox, struct Movement_data* self_Movement, int target){ // moved is useless
    self_Movement->remainder_y = int2Fixed(0);
    self_Movement->speed_y = int2Fixed(0);
    return true;
}

// ox and oy are 0 by default
static bool object_overlaps(const struct Position_data* self_Position, const struct Hitbox_data* self_Hitbox, 
                     const struct Position_data* other_Position, const struct Hitbox_data* other_Hitbox, 
                     FIXED_16 ox, FIXED_16 oy){
    if(self_Hitbox == other_Hitbox)
        return false;

    return ox +self_Position->x +self_Hitbox->hit_x +self_Hitbox->hit_w > other_Position->x + other_Hitbox->hit_x &&
           oy +self_Position->y +self_Hitbox->hit_y +self_Hitbox->hit_h > other_Position->y + other_Hitbox->hit_y &&
           ox +self_Position->x +self_Hitbox->hit_x < other_Position->x +other_Hitbox->hit_x +other_Hitbox->hit_w &&
           oy +self_Position->y +self_Hitbox->hit_y < other_Position->y +other_Hitbox->hit_y +other_Hitbox->hit_h;
}

static bool object_contains(const struct Position_data* self_Position, const struct Hitbox_data* self_Hitbox, FIXED_16 px, FIXED_16 py){
    return px >= self_Position->x +self_Hitbox->hit_x &&
           px < self_Position->x +self_Hitbox->hit_x +self_Hitbox->hit_w &&
           py >= self_Position->y +self_Hitbox->hit_y &&
           py < self_Position->y +self_Hitbox->hit_y +self_Hitbox->hit_h;
}

// checks if object overlaps with a collider tile or a crumble object
// ox and oy are 0 by default
static bool object_check_solid(const struct Position_data* self_Position, const struct Hitbox_data* self_Hitbox, FIXED_16 ox, FIXED_16 oy){
    FIXED_16 start_x = (ox +self_Position->x +self_Hitbox->hit_x) / 8;
    FIXED_16 end_x =   (ox +self_Position->x +self_Hitbox->hit_x +self_Hitbox->hit_w -int2Fixed(1)) / 8;
    FIXED_16 start_y = (oy +self_Position->y +self_Hitbox->hit_y);
    FIXED_16 end_y =   (oy +self_Position->y +self_Hitbox->hit_y +self_Hitbox->hit_h -int2Fixed(1));
    
    /*mgba_printf(MGBA_LOG_DEBUG, "   check_solid x: %f %f, y: %f %f", fixed2Float(start_x), fixed2Float(end_x), fixed2Float(start_y), fixed2Float(end_y));
    mgba_printf(MGBA_LOG_DEBUG, "   tile_y %d %d", tile_y(fixed2Int(start_y)), tile_y(fixed2Int(end_y)));
    mgba_printf(MGBA_LOG_DEBUG, "   tile_y/8 %d %d", fixed2Int((start_y)/8), fixed2Int((end_y)/8));
    mgba_printf(MGBA_LOG_DEBUG, "   ox x: %f %f", fixed2Float(ox), fixed2Float(oy));*/
    
    for(int i = fixed2Int(start_x); i <= fixed2Int(end_x); i++){
        for(int j = tile_y(fixed2Int(start_y)); j <= tile_y(fixed2Int(end_y)); j++){
            //mgba_printf(MGBA_LOG_DEBUG, "    i:%d j:%d", i, j);
            if(tile_at_is_collider(i, j, level_index)){
                //mgba_printf(MGBA_LOG_DEBUG, "    tile at is %d %d", i, j);
                return true;
            }
        }
    }
    //mgba_printf(MGBA_LOG_DEBUG, "    tile at is NOT %d %d");

    // object collision; crumbles are the only ones with attribute o.solid
    for(int i = 0; i < current_gameobjects.crumbles_len; i++){
        if(current_gameobjects.crumbles[i].breaking != Crumble_Broken)
            if(object_overlaps(self_Position, self_Hitbox, &current_gameobjects.crumbles[i].Position, &current_gameobjects.crumbles[i].Hitbox, ox, oy))
                return true;
    }

    return false;
}

static bool object_corner_correct(struct Position_data* self_Position, const struct Hitbox_data* self_Hitbox, 
                           int dir_x, int dir_y, int side_dist, int look_ahead, int only_sign, bool use_correction_func){
    if(dir_x != 0){
        for(int i = 1; i <= side_dist; i++){
            for(int s = 1; s >= -2; s-=2){
                if(s == -only_sign)
                    goto continue_x;
                
                bool check_solid = object_check_solid(self_Position, self_Hitbox, int2Fixed(dir_x), int2Fixed(i * s));
                bool check_func = true;
                if(use_correction_func)
                    check_func = player_correction_func(&current_gameobjects.player, int2Fixed(dir_x), int2Fixed(i * s));

                if(!check_solid && check_func){
                    self_Position->x += int2Fixed(dir_x);
                    self_Position->y += int2Fixed(i * s);
                    return true;
                }

                continue_x:
            }
        }
    }else if(dir_y != 0){
        for(int i = 1; i <= side_dist; i++){
            for(int s = 1; s >= -1; s-=2){
                if(s == -only_sign)
                    goto continue_y;
                
                bool check_solid = object_check_solid(self_Position, self_Hitbox, int2Fixed(i * s), int2Fixed(dir_y));
                bool check_func = true;
                if(use_correction_func)
                    check_func = player_correction_func(&current_gameobjects.player, int2Fixed(i * s), int2Fixed(dir_y));

                if(!check_solid && check_func){
                    self_Position->x += int2Fixed(i * s);
                    self_Position->y += int2Fixed(dir_y);
                    return true;
                }

                continue_y:
            }
        }
    }

    return false;
}

// GrapplePickup function
static void grapplepickup_init(struct GrapplePickup_data* self, const struct Position_data* position){
    self->Position = *position;
    self->destroyed = false;
}

// SpikeGroup function
static void spikeGroup_init(struct SpikeGroup_data* self, const struct SpikeGroup_definition* spikegroup_definition){
    self->Position = spikegroup_definition->Position;
    self->hazard = spikegroup_definition->dir;

    if((self->hazard&1) == 0){ // vertical spike
        self->Hitbox.hit_x = int2Fixed(0);
        self->Hitbox.hit_w = int2Fixed(8)*spikegroup_definition->count;
        self->Hitbox.hit_h = int2Fixed(3);
        if(self->hazard == SpikeDirection_Up)
            self->Hitbox.hit_y = int2Fixed(5);
        else
            self->Hitbox.hit_y = int2Fixed(0);
    }else{ // horizontal spike
        self->Hitbox.hit_y = int2Fixed(0);
        self->Hitbox.hit_h = int2Fixed(8)*spikegroup_definition->count;
        self->Hitbox.hit_w = int2Fixed(3);
        if(self->hazard == SpikeDirection_Left)
            self->Hitbox.hit_x = int2Fixed(5);
        else
            self->Hitbox.hit_x = int2Fixed(0);
    }
}

// Snowball functions
static void snowball_init(struct Snowball_data* self, const struct Position_data* position, FIXED_16 speed_x, FIXED_16 speed_y){
    self->Holdable.Position = *position;
    self->Holdable.Hitbox = DEFAULT_HITBOX;
    self->Holdable.Movement.remainder_x = int2Fixed(0);
    self->Holdable.Movement.remainder_y = int2Fixed(0);
    self->Holdable.Movement.speed_x = speed_x;
    self->Holdable.Movement.speed_y = speed_y;
    self->Holdable.thrown_timer = 0;
    self->Holdable.freeze = 0;
    self->hp = 6;
    self->destroyed = false;
    self->Holdable.held = false;
    self->stop = false;
}

static void snowball_update(struct Snowball_data* self){
    if(!self->Holdable.held){
        self->Holdable.thrown_timer -= 1;

        //speed
        if(self->stop){
            self->Holdable.Movement.speed_x = approach(self->Holdable.Movement.speed_x, int2Fixed(0), float2Fixed(0.25f));
            if(self->Holdable.Movement.speed_x == int2Fixed(0))
                self->stop = false;
        }else if(self->Holdable.Movement.speed_x != int2Fixed(0)){
            self->Holdable.Movement.speed_x = approach(self->Holdable.Movement.speed_x, int2Fixed(sgn(self->Holdable.Movement.speed_x) * 2), float2Fixed(0.1f));
        }

        //gravity
        if(!object_check_solid(&self->Holdable.Position, &self->Holdable.Hitbox, int2Fixed(0), int2Fixed(1))){
            self->Holdable.Movement.speed_y = approach(self->Holdable.Movement.speed_y, int2Fixed(4), float2Fixed(0.4f));
        }

        //apply
        object_move_x(&self->Holdable.Position, &self->Holdable.Hitbox, &self->Holdable.Movement, self->Holdable.Movement.speed_x, (bool (*)(void*, int))snowball_on_collide_x, self);
        object_move_y(&self->Holdable.Position, &self->Holdable.Hitbox, &self->Holdable.Movement, self->Holdable.Movement.speed_y, (bool (*)(void*, int))snowball_on_collide_y, self);

        //bounds
        if(fixed2Int(self->Holdable.Position.y) > level.height*8+24)
            self->destroyed = true;
    }
}
static bool snowball_on_collide_x(struct Snowball_data* self, int total){
    if(object_corner_correct(&self->Holdable.Position, &self->Holdable.Hitbox, sgn(self->Holdable.Movement.speed_x), 0, 2, 2, 1, false))
        return false;

    if(snowball_hurt(self))
        return true;

    self->Holdable.Movement.speed_x *= -1;
    self->Holdable.Movement.remainder_x = int2Fixed(0);
    self->Holdable.freeze = 1;
    psfx(SFX_SFX17_0_2, -1);
    return true;
}
static bool snowball_on_collide_y(struct Snowball_data* self, int total){ // moved is useless
    if(self->Holdable.Movement.speed_y < int2Fixed(0)){
        self->Holdable.Movement.speed_y = int2Fixed(0);
        self->Holdable.Movement.remainder_y = int2Fixed(0);
        return true;
    }
    if(self->Holdable.Movement.speed_y >= int2Fixed(4)){
        self->Holdable.Movement.speed_y = int2Fixed(-2);
        psfx(SFX_SFX17_0_2, -1);
    }else if(self->Holdable.Movement.speed_y >= int2Fixed(1)){
        self->Holdable.Movement.speed_y = int2Fixed(-1);
        psfx(SFX_SFX17_0_2, -1);
    }else{
        self->Holdable.Movement.speed_y = int2Fixed(0);
    }
    self->Holdable.Movement.remainder_y = int2Fixed(0);
    return true;
}
static void snowball_on_release(struct Holdable_data* self, bool thrown){
    if(!thrown)
        ((struct Snowball_data*)self)->stop = true;
    self->thrown_timer = 8;
}
static bool snowball_hurt(struct Snowball_data* self){
    self->hp -= 1;
    if(self->hp <= 0){
        psfx(SFX_SFX8_16_4, -1);
        self->destroyed = true;
        return true;
    }
    return false;
}
// only used by player
static bool snowball_bounce_overlaps(struct Snowball_data* self, const struct Player_data* other){
    if(self->Holdable.Movement.speed_x != int2Fixed(0)){
        self->Holdable.Hitbox.hit_w = int2Fixed(12);
        self->Holdable.Hitbox.hit_x = int2Fixed(-2);
        bool ret = object_overlaps(&self->Holdable.Position, &self->Holdable.Hitbox, &other->Position, &other->Hitbox, int2Fixed(0), int2Fixed(0));
        self->Holdable.Hitbox.hit_w = int2Fixed(8);
        self->Holdable.Hitbox.hit_x = int2Fixed(0);
        return ret;
    }else{
        return object_overlaps(&self->Holdable.Position, &self->Holdable.Hitbox, &other->Position, &other->Hitbox, int2Fixed(0), int2Fixed(0));
    }
}
// used for grapple check
static bool snowball_contains(const struct Snowball_data* self, FIXED_16 px, FIXED_16 py){
    return px >= self->Holdable.Position.x &&
            px < self->Holdable.Position.x +int2Fixed(8) &&
            py >= self->Holdable.Position.y -int2Fixed(1) &&
            py < self->Holdable.Position.y +int2Fixed(10);
}

// Springboard functions
static void springboard_init(struct Springboard_data* self, const struct Position_data* position){
    self->Holdable.Position = *position;
    self->Holdable.Hitbox = DEFAULT_HITBOX;
    self->Holdable.Movement = NO_MOVEMENT;
    self->Holdable.thrown_timer = 0;
    self->Holdable.freeze = 0;
    self->destroyed = false;
    self->Holdable.held = false;
    self->player_assigned = false;
    self->spr_pressed = false;
}

static void springboard_update(struct Springboard_data* self){
    if(!self->Holdable.held){
        self->Holdable.thrown_timer -= 1;
        
        //friction and gravity
        if(object_check_solid(&self->Holdable.Position, &self->Holdable.Hitbox, int2Fixed(0), int2Fixed(1))){
            self->Holdable.Movement.speed_x = approach(self->Holdable.Movement.speed_x, int2Fixed(0), int2Fixed(1));
        }else{
            self->Holdable.Movement.speed_x = approach(self->Holdable.Movement.speed_x, int2Fixed(0), float2Fixed(0.2f));
            self->Holdable.Movement.speed_y = approach(self->Holdable.Movement.speed_y, int2Fixed(4), float2Fixed(0.4f));
        }

        //apply
        object_move_x(&self->Holdable.Position, &self->Holdable.Hitbox, &self->Holdable.Movement, self->Holdable.Movement.speed_x, (bool (*)(void*, int))springboard_on_collide_x, self);
        object_move_y(&self->Holdable.Position, &self->Holdable.Hitbox, &self->Holdable.Movement, self->Holdable.Movement.speed_y, (bool (*)(void*, int))springboard_on_collide_y, self);

        if(self->player_assigned)
            object_move_y(&current_gameobjects.player.Position, &current_gameobjects.player.Hitbox, &current_gameobjects.player.Movement, self->Holdable.Movement.speed_y, NULL, NULL);
        
        if(fixed2Int(self->Holdable.Position.y) > level.height*8 +24)
            self->destroyed = true;
    }
}
static bool springboard_on_collide_x(struct Springboard_data* self, int total){
    self->Holdable.Movement.speed_x = mulFixed(self->Holdable.Movement.speed_x, float2Fixed(-0.2f));
    self->Holdable.Movement.remainder_x = int2Fixed(0);
    self->Holdable.freeze = 1;
    return true;
}
static bool springboard_on_collide_y(struct Springboard_data* self, int total){
    if(self->Holdable.Movement.speed_y < int2Fixed(0)){
        self->Holdable.Movement.speed_y = int2Fixed(0);
        self->Holdable.Movement.remainder_y = int2Fixed(0);
        return true;
    }
    if(self->Holdable.Movement.speed_y >= int2Fixed(2)){
        self->Holdable.Movement.speed_y = mulFixed(self->Holdable.Movement.speed_y, float2Fixed(-0.4f));
    }else{
        self->Holdable.Movement.speed_y = int2Fixed(0);
    }
    self->Holdable.Movement.remainder_y = int2Fixed(0);
    self->Holdable.Movement.speed_x = mulFixed(self->Holdable.Movement.speed_x, float2Fixed(0.5f));
    return true;
}
static void springboard_on_release(struct Holdable_data* self, bool thrown){
    if(thrown)
        self->thrown_timer = 5;
}

// Grappler function
static void grappler_init(struct Grappler_data* self, const struct Position_data* position){
    self->Position = *position;
    self->Hitbox.hit_x = int2Fixed(-1);
    self->Hitbox.hit_y = int2Fixed(-1);
    self->Hitbox.hit_w = int2Fixed(10);
    self->Hitbox.hit_h = int2Fixed(10);
}

// Bridge functions
static void bridge_init(struct Bridge_data* self, const  struct Position_data* position){
    self->Position = *position;
    self->falling = false;
}

static void bridge_update(struct Bridge_data* self){
    if(self->falling)
        self->Position.y += int2Fixed(3);
}

// Berry functions
static void berry_init(struct Berry_data* self, const struct Berry_definition* berry_definition){
    self->Position = berry_definition->Position;
    self->id = berry_definition->id;
    self->Hitbox = DEFAULT_HITBOX;
    self->collected = false;
    self->destroyed = false;
    self->flash = 0;
    self->ground = 0;
    self->timer = 0;

    self->picked_up = false;
}

static void berry_update(struct Berry_data* self){
    if(self->collected){
        self->timer += 1;
        if(self->timer > 5)
            self->Position.y -= float2Fixed(0.2f);
        self->destroyed = self->timer > 30;
    }else if(self->picked_up){
        self->Position.x += (current_gameobjects.player.Position.x - self->Position.x) / 8;
        self->Position.y += (current_gameobjects.player.Position.y - int2Fixed(4) - self->Position.y) / 8;
        self->flash -= 1;

        if(current_gameobjects.player.state != 99){
            if(object_check_solid(&current_gameobjects.player.Position, &current_gameobjects.player.Hitbox, int2Fixed(0), int2Fixed(1))){
                self->ground += 1;
            }else{
                self->ground = 0;
            }
        }else{
            self->ground = 0;
        }

        if(self->ground > 3 || 
           current_gameobjects.player.Position.x > int2Fixed(level.width*8-7) || 
           current_gameobjects.player.last_berry_id != self->id){
            psfx(SFX_SFX8_8_8, 20);
            collected[self->id] = true;
            berry_count += 1;
            self->collected = true;
            self->timer = 0;
            // self.draw = score // ??
        }
    }
}
static void berry_collect(struct Berry_data* self){
    if(!self->picked_up){
        self->picked_up = true;
        current_gameobjects.player.last_berry_id = self->id;
        self->flash = 5;
        self->ground = 0;
        psfx(SFX_SFX7_12_4, -1);
    }
}

// Crumble functions
static void crumble_init(struct Crumble_data* self, const struct Position_data* position){
    self->Position = *position;
    self->Hitbox = DEFAULT_HITBOX;
    self->time = 0;
    self->breaking = Crumble_Normal;
}

static void crumble_update(struct Crumble_data* self){
    if(self->breaking != Crumble_Normal){
        self->time += 1;
        if(self->time > 10)
            self->breaking = Crumble_Broken;
        if(self->time > 90){
            if(object_overlaps(&self->Position, &self->Hitbox, 
                                &current_gameobjects.player.Position, &current_gameobjects.player.Hitbox, int2Fixed(0), int2Fixed(0))){
                return;
            }
            for(int i = 0; i < current_gameobjects.springboards_len; i++)
                if(object_overlaps(&self->Position, &self->Hitbox, 
                                &current_gameobjects.springboards[i].Holdable.Position, &current_gameobjects.springboards[i].Holdable.Hitbox, int2Fixed(0), int2Fixed(0))){
                    return;
                }
            for(int i = 0; i < current_gameobjects.berries_len; i++)
                if(object_overlaps(&self->Position, &self->Hitbox, 
                                &current_gameobjects.berries[i].Position, &current_gameobjects.berries[i].Hitbox, int2Fixed(0), int2Fixed(0))){
                    return;
                }
            for(int i = 0; i < MAX_SNOWBALL_COUNT; i++)
                if(object_overlaps(&self->Position, &self->Hitbox, 
                                &current_gameobjects.snowballs[i].Holdable.Position, &current_gameobjects.snowballs[i].Holdable.Hitbox, int2Fixed(0), int2Fixed(0))){
                    return;
                }

            self->breaking = Crumble_Normal;
            self->time = 0;
            psfx(SFX_SFX17_5_3, -1);
        }
    }
}

// Checkpoint function
static void checkpoint_init(struct Checkpoint_data* self, const struct Position_data* position){
    self->Position = *position;
    // player is already spawned at checkpoint position in load_level_objects()
}

// Spawner functions
static void spawner_init(struct Spawner_data* self, const struct Position_data* position, int dir){
    self->Position = *position;
    self->dir = dir;
    self->timer = fixed2Int(position->x/8)%32;
}

static void spawner_update(struct Spawner_data* self){
    self->timer += 1;

    if(self->timer >= 32){
        if(abs_(fixed2Int(self->Position.x) - 64 - get_camera_x()) < 128){
            self->timer = 0;
#ifdef DEBUG
            bool spawned = false;
#endif
            for(int i = 0; i < MAX_SNOWBALL_COUNT; i++){
                if(current_gameobjects.snowballs[i].destroyed){
                    self->Position.y -= int2Fixed(8);
                    snowball_init(current_gameobjects.snowballs+i, &self->Position, int2Fixed(self->dir*2), int2Fixed(4));
                    self->Position.y += int2Fixed(8);
                    psfx(SFX_SFX17_5_3, -1);
#ifdef DEBUG
                    spawned = true;
#endif
                    break;
                }
            }

#ifdef DEBUG
            if(!spawned)
                errorHalt("2many balls");
#endif
        }
    }
}

// Player input functions
static int input_x = 0;
static bool input_jump = false;
static int input_jump_pressed = 0;
static bool input_grapple = false;
static int input_grapple_pressed = 0;
static int axis_x_value = 0;
static bool axis_x_turned = false;
void update_input(){
	// axes
	int prev_x = axis_x_value;
	if(btn(KEY_BTN0)){
		if(btn(KEY_BTN1)){
			if(axis_x_turned){
				axis_x_value = prev_x;
				input_x = prev_x;
			}else{
				axis_x_turned = true;
				axis_x_value = -prev_x;
				input_x = -prev_x;
			}
		}else{
			axis_x_turned = false;
			axis_x_value = -1;
			input_x = -1;
		}
	}else if(btn(KEY_BTN1)){
		axis_x_turned = false;
		axis_x_value = 1;
		input_x = 1;
	}else{
		axis_x_turned = false;
		axis_x_value = 0;
		input_x = 0;
	}

	// input_jump
	bool jump = btn(KEY_BTN4);
	if(jump && !input_jump){
		input_jump_pressed = 4;
	}else{
		input_jump_pressed = jump ? max2(0, input_jump_pressed-1) : 0;
	}
	input_jump = jump;

	// input_grapple
	bool grapple = btn(KEY_BTN5);
	if(grapple && !input_grapple){
		input_grapple_pressed = 4;
	}else{
		input_grapple_pressed = grapple ? max2(0, input_grapple_pressed - 1) : 0;
	}
	input_grapple = grapple;
}

static bool consume_jump_press(){
	bool val = input_jump_pressed > 0;
	input_jump_pressed = 0;
	return val;
}

static bool consume_grapple_press(){
	bool val = input_grapple_pressed > 0;
	input_grapple_pressed = 0;
	return val;
}

// Player functions
static void player_init(struct Player_data* self, const struct Position_data* position){
    self->Position.x = position->x + int2Fixed(4);
    self->Position.y = position->y + int2Fixed(8);
    self->Hitbox.hit_x = int2Fixed(-3);
    self->Hitbox.hit_y = int2Fixed(-6);
    self->Hitbox.hit_w = int2Fixed(6);
    self->Hitbox.hit_h = int2Fixed(6);
    self->Movement = NO_MOVEMENT;

    self->scarf_len = 5;
    for(int i = 0; i < self->scarf_len; i++){
        self->scarf[i].x = self->Position.x;
        self->scarf[i].y = self->Position.y;
    }

    self->t_grapple_jump_grace = 0;
    self->t_var_jump = 0;
    self->var_jump_speed = int2Fixed(0);
    self->auto_var_jump = false;
    self->grapple_x = int2Fixed(0);
    self->grapple_y = int2Fixed(0);
    self->grapple_dir = 0;
    self->grapple_hit_type = GrappleHit_Nothing;
    self->grapple_hit_object = NULL;
    self->grapple_wave = int2Fixed(0);
    self->grapple_boost = false;
    self->t_grapple_cooldown = 0;
    self->grapple_retract = false;
    self->holding_type = GrappleHit_Nothing;
    self->holding_object = NULL;
    self->wipe_timer = 0;
    self->finished = false;
    self->t_grapple_jump_grace = 0;
    self->t_grapple_pickup = 0;

    self->state = 0;
    self->freeze = 0;

    self->jump_grace_y = int2Fixed(0);
    self->grapple_jump_grace_y = int2Fixed(0);
    self->t_jump_grace = 0;
    self->last_berry_id = -1;
    self->facing = 1;

    self->springboard = NULL;

    // camera
    camera_modes[level_index](fixed2Int(self->Position.x), fixed2Int(self->Position.y));
    camera_x = camera_target_x;
    camera_y = camera_target_y;
    camera(camera_x, camera_y);
}

static void player_start_grapple(struct Player_data* self){
    self->state = 10;
    self->Movement.speed_x = int2Fixed(0);
    self->Movement.speed_y = int2Fixed(0);
    self->Movement.remainder_x = int2Fixed(0);
    self->Movement.remainder_y = int2Fixed(0);
    self->grapple_x = self->Position.x;
    self->grapple_y = self->Position.y - int2Fixed(3);
    self->grapple_wave = int2Fixed(0);
    self->grapple_retract = false;
    self->t_grapple_cooldown = 6;
    self->t_var_jump = 0;

    if(input_x != 0)
        self->grapple_dir = input_x;
    else
        self->grapple_dir = self->facing;
    self->facing = self->grapple_dir;
    psfx(SFX_SFX8_0_5, -1);
}
// 0-nothing, 1-hit, 2-fail
static int player_grapple_check(struct Player_data* self, FIXED_16 x, FIXED_16 y){
    //mgba_printf(MGBA_LOG_DEBUG, "grapple check: %d %d", fixed2Int(x), fixed2Int(y));
    int tilex = fixed2Int(x/8),
        tiley = tile_y(fixed2Int(y));
    if(tile_at_is_collider(tilex, tiley, level_index)){
        self->grapple_hit_type = GrappleHit_Nothing;
        // self.grapple_hit = nil
        if(tile_at_is_grabbable(tilex, tiley, level_index))
            return 2;
        else
            return 1;
    }

    for(int i = 0; i < MAX_SNOWBALL_COUNT; i++){
        if(current_gameobjects.snowballs[i].destroyed)
            continue;
        if(snowball_contains(current_gameobjects.snowballs+i, x, y)){
            //set grapple_hit to object
            self->grapple_hit_object = current_gameobjects.snowballs+i;
            self->grapple_hit_type = GrappleHit_Snowball;
            return 1;
        }
    }
    for(int i = 0; i < current_gameobjects.springboards_len; i++){
        if(current_gameobjects.springboards[i].destroyed)
            continue;
        if(object_contains(&current_gameobjects.springboards[i].Holdable.Position, &current_gameobjects.springboards[i].Holdable.Hitbox, x, y)){
            //set grapple_hit to object
            self->grapple_hit_object = current_gameobjects.springboards+i;
            self->grapple_hit_type = GrappleHit_Springboard;
            return 1;
        }
    }
    for(int i = 0; i < current_gameobjects.grapplers_len; i++){
        if(object_contains(&current_gameobjects.grapplers[i].Position, &current_gameobjects.grapplers[i].Hitbox, x, y)){
            //set grapple_hit to object
            self->grapple_hit_object = current_gameobjects.grapplers+i;
            self->grapple_hit_type = GrappleHit_Grappler;
            return 1;
        }
    }
    for(int i = 0; i < current_gameobjects.crumbles_len; i++){
        if(current_gameobjects.crumbles[i].breaking == Crumble_Broken)
            continue;
        if(object_contains(&current_gameobjects.crumbles[i].Position, &current_gameobjects.crumbles[i].Hitbox, x, y)){
            //set grapple_hit to object
            self->grapple_hit_object = current_gameobjects.crumbles+i;
            self->grapple_hit_type = GrappleHit_Crumble;
            return 1;
        }
    }

    return 0;
}

// Helpers

static void player_jump(struct Player_data* self){
    //mgba_printf(MGBA_LOG_DEBUG, "player_jump");
    consume_jump_press();
    self->state = 0;
    self->Movement.speed_y = int2Fixed(-4);
    self->var_jump_speed = int2Fixed(-4);
    self->Movement.speed_x += input_x * float2Fixed(0.2f);
    self->t_var_jump = 4;
    self->t_jump_grace = 0;
    self->auto_var_jump = false;
    object_move_y(&self->Position, &self->Hitbox, &self->Movement, self->jump_grace_y - self->Position.y, NULL, NULL);
    psfx(SFX_SFX7_0_4, -1);
}
static void player_bounce(struct Player_data* self, FIXED_16 x, FIXED_16 y){
    self->state = 0;
    self->Movement.speed_y = int2Fixed(-4);
    self->var_jump_speed = int2Fixed(-4);
    self->t_var_jump = 4;
    self->t_jump_grace = 0;
    self->auto_var_jump = true;
    self->Movement.speed_x += sgn(self->Position.x - x) * float2Fixed(0.5f);
    object_move_y(&self->Position, &self->Hitbox, &self->Movement, y - self->Position.y, NULL, NULL);
}
static void player_spring(struct Player_data* self){ // FIXED_16 y unused
    consume_jump_press();
    if(input_jump){
        psfx(SFX_SFX17_2_3, -1);
    }else{
        psfx(SFX_SFX17_0_2, -1);
    }
    self->state = 0;
    self->Movement.speed_y = int2Fixed(-5);
    self->var_jump_speed = int2Fixed(-5);
    self->t_var_jump = 6;
    self->t_jump_grace = 0;
    self->Movement.remainder_y = int2Fixed(0);
    self->auto_var_jump = false;
    self->springboard->player_assigned = false;

    for(int i = 0; i < current_gameobjects.crumbles_len; i++){
        if(current_gameobjects.crumbles[i].breaking == Crumble_Broken)
            continue;
        if(object_overlaps(&self->springboard->Holdable.Position, &self->springboard->Holdable.Hitbox, 
                        &current_gameobjects.crumbles[i].Position, &current_gameobjects.crumbles[i].Hitbox, 
                        int2Fixed(0), int2Fixed(4))){
            current_gameobjects.crumbles[i].breaking = Crumble_Breaking;
            psfx(SFX_SFX8_20_4, -1);
        }
    }
}
static void player_wall_jump(struct Player_data* self, int dir){
    consume_jump_press();
    self->state = 0;
    self->Movement.speed_y = int2Fixed(-3);
    self->var_jump_speed = int2Fixed(-3);
    self->Movement.speed_x = int2Fixed(3) * dir;
    self->t_var_jump = 4;
    self->auto_var_jump = false;
    self->facing = dir;
    object_move_x(&self->Position, &self->Hitbox, &self->Movement, -dir * int2Fixed(3), NULL, NULL);
    psfx(SFX_SFX7_4_4, -1);
}
static void player_grapple_jump(struct Player_data* self){
    consume_jump_press();
    psfx(SFX_SFX17_2_3, -1);
    self->state = 0;
    self->t_grapple_jump_grace = 0;
    self->Movement.speed_y = int2Fixed(-3);
    self->var_jump_speed = int2Fixed(-3);
    self->t_var_jump = 4;
    self->auto_var_jump = false;
    self->grapple_retract = true;
    if(abs_(self->Movement.speed_x) > int2Fixed(4))
        self->Movement.speed_x = sgn(self->Movement.speed_x) * int2Fixed(4);
    object_move_y(&self->Position, &self->Hitbox, &self->Movement, self->grapple_jump_grace_y - self->Position.y, NULL, NULL);
}
static bool player_bounce_check(struct Player_data* self, FIXED_16 other_speed_y, FIXED_16 other_y){
    return self->Movement.speed_y >= int2Fixed(0) && self->Position.y - self->Movement.speed_y  < other_y + other_speed_y + int2Fixed(4);
}
static void player_die(struct Player_data* self){
    self->state = 99;
    freeze_time = 2;
    shake = 5;
    death_count += 1;
    psfx(SFX_SFX14_16_16, 120);
}

// ox and oy are 0 by default
static bool player_hazard_check(const struct Player_data* self, FIXED_16 ox, FIXED_16 oy){
    for(int i = 0; i < current_gameobjects.spikeGroups_len; i++){
        switch(current_gameobjects.spikeGroups[i].hazard){
        // > and < are correct, as logic is flipped here compared to original
        case SpikeDirection_Up:
            if(self->Movement.speed_y < int2Fixed(0)) 
                continue;
            break;
        case SpikeDirection_Down:
            if(self->Movement.speed_y > int2Fixed(0)) 
                continue;
            break;
        case SpikeDirection_Right:
            if(self->Movement.speed_x > int2Fixed(0)) 
                continue;
            break;
        case SpikeDirection_Left:
            if(self->Movement.speed_x < int2Fixed(0)) 
                continue;
            break;
        }
        if(object_overlaps(&self->Position, &self->Hitbox, &current_gameobjects.spikeGroups[i].Position, &current_gameobjects.spikeGroups[i].Hitbox, ox, oy))
            return true;
    }
    return false;
}

static bool player_correction_func(const struct Player_data* self, FIXED_16 ox, FIXED_16 oy){
    return !player_hazard_check(self, ox, oy);
}

// Grappled Objects
static bool pull_collide_x(struct Holdable_data* self, int target){
    return !object_corner_correct(&self->Position, &self->Hitbox,
        sgn(target), 0, 4, 2, 0, false);
}

static void player_release_holding(struct Player_data* self,
    struct Holdable_data* other, enum GrappleHit_Type other_type, FIXED_16 x, FIXED_16 y, bool thrown){
    other->held = false;
    other->Movement.speed_x = x;
    other->Movement.speed_y = y;
    switch(other_type){
        case GrappleHit_Snowball:
            snowball_on_release(other, thrown);
            break;
        case GrappleHit_Springboard:
            springboard_on_release(other, thrown);
            break;
        default:
#ifdef DEBUG
            // Only holdable objects should be possible
            errorHalt("release holding");
#endif
            break;
    }
    psfx(SFX_SFX7_24_6, -1);
    self->holding_type = GrappleHit_Nothing;
}

static void player_update(struct Player_data* self){
    //mgba_printf(MGBA_LOG_DEBUG, "state: %d", self->state);
    //mgba_printf(MGBA_LOG_DEBUG, "   wipe: %d", self->wipe_timer);
    bool on_ground = object_check_solid(&self->Position, &self->Hitbox, int2Fixed(0), int2Fixed(1));
    if(on_ground){
        //mgba_printf(MGBA_LOG_DEBUG, "on_ground");
        self->t_jump_grace = 4;
        self->jump_grace_y = self->Position.y;
    }else{
        self->t_jump_grace = max2(0, self->t_jump_grace -1);
        //mgba_printf(MGBA_LOG_DEBUG, "NOT on_ground");
    }
    
    self->t_grapple_jump_grace = max(self->t_grapple_jump_grace -1);

    if(self->t_grapple_cooldown > 0 && self->state < 1)
        self->t_grapple_cooldown -= 1;

    // grapple retract
    if(self->grapple_retract){
		//mgba_printf(MGBA_LOG_DEBUG, "retracting%d %d %d %d", self->grapple_x, self->Position.x, self->grapple_y, self->Position.y);
        self->grapple_x = approach(self->grapple_x, self->Position.x, int2Fixed(12));
        self->grapple_y = approach(self->grapple_y, self->Position.y -int2Fixed(3), int2Fixed(6));

        if(self->grapple_x == self->Position.x && self->grapple_y == self->Position.y -int2Fixed(3))
            self->grapple_retract = false;
    }

    /* player states:
        0     - normal
        1    - lift
        2     - springboard bounce
        10     - throw grapple
        11     - grapple attached to solid
        12    - grapple pulling in holdable
        50  - get grapple!!
        99     - dead
        100 - finished level
     */

    if(self->state == 0){
        // normal state

        // facing
        if(input_x != 0)
            self->facing = input_x;

        // running
        FIXED_16 target = int2Fixed(0), accel = float2Fixed(0.2f);
        if(abs_(self->Movement.speed_x) > int2Fixed(2) && input_x == sgn(self->Movement.speed_x)){
            target = int2Fixed(2);
            accel = float2Fixed(0.1f);
        }else if(on_ground){
            target = int2Fixed(2);
            accel = float2Fixed(0.8f);
        }else if(input_x != 0){
            target = int2Fixed(2);
            accel = float2Fixed(0.4f);
        }
        self->Movement.speed_x = approach(self->Movement.speed_x, input_x * target, accel);

        // gravity
        if(!on_ground){
            FIXED_16 max_gravity = float2Fixed(4.4f);
            if(btn(KEY_BTN3))
                max_gravity = float2Fixed(5.2f);
            if(abs_(self->Movement.speed_y) < float2Fixed(0.2f) && input_jump)
                self->Movement.speed_y = min2(self->Movement.speed_y + float2Fixed(0.4f), max_gravity);
            else
                self->Movement.speed_y = min2(self->Movement.speed_y + float2Fixed(0.8f), max_gravity);
        }

        // variable jumping
        if(self->t_var_jump > 0){
            if(input_jump || self->auto_var_jump){
                self->Movement.speed_y = self->var_jump_speed;
                self->t_var_jump -= 1;
            }else
                self->t_var_jump = 0;
        }

        // jumping
        //mgba_printf(MGBA_LOG_DEBUG, "jump_pressed %d", input_jump_pressed);
        if(input_jump_pressed > 0){
            if(self->t_jump_grace > 0)
                player_jump(self);
            else if(object_check_solid(&self->Position, &self->Hitbox, int2Fixed(2), int2Fixed(0)))
                player_wall_jump(self, -1);
            else if(object_check_solid(&self->Position, &self->Hitbox, int2Fixed(-2), int2Fixed(0)))
                player_wall_jump(self, 1);
            else if(self->t_grapple_jump_grace > 0)
                player_grapple_jump(self);
        }

        // throw holding
        if(self->holding_type != GrappleHit_Nothing && !input_grapple){
            struct Holdable_data* holding = self->holding_object;
            if(!object_check_solid(&holding->Position, &holding->Hitbox, int2Fixed(0), int2Fixed(-2))){
                holding->Position.y -= int2Fixed(2);
                if(btn(KEY_BTN3))
                    player_release_holding(self, holding, self->holding_type, int2Fixed(2*self->facing), int2Fixed(0), false);
                else
                    player_release_holding(self, holding, self->holding_type, int2Fixed(4*self->facing), int2Fixed(-1), true);
            }
        }

        // throw grapple
        if(have_grapple && self->holding_type==GrappleHit_Nothing && self->t_grapple_cooldown <= 0)
            if(consume_grapple_press())
                player_start_grapple(self);
    }
    else if(self->state == 1){
        // lift state // end of pulling?
        struct Holdable_data* hold = self->grapple_hit_object;
        hold->Position.x = approach(hold->Position.x, self->Position.x - int2Fixed(4), int2Fixed(4));
        hold->Position.y = approach(hold->Position.y, self->Position.y - int2Fixed(14), int2Fixed(4));
        if(hold->Position.x == self->Position.x -int2Fixed(4) &&
            hold->Position.y == self->Position.y -int2Fixed(14)){
            self->state = 0;
            self->holding_object = self->grapple_hit_object;
            self->holding_type = self->grapple_hit_type;
        }
    }
    else if(self->state == 2){
        // springboard bounce state
#ifdef DEBUG
        if(self->springboard == NULL)
            errorHalt("Spring NULL");
#endif
        FIXED_16 at_x = approach(self->Position.x, self->springboard->Holdable.Position.x + int2Fixed(4), float2Fixed(0.5f));
        object_move_x(&self->Position, &self->Hitbox, &self->Movement, at_x - self->Position.x, NULL, NULL);
        
        FIXED_16 at_y = approach(self->Position.y, self->springboard->Holdable.Position.y + int2Fixed(4), float2Fixed(0.2f));
        object_move_y(&self->Position, &self->Hitbox, &self->Movement, at_y - self->Position.y, NULL, NULL);

        if(!self->springboard->spr_pressed && self->Position.y >= self->springboard->Holdable.Position.y + int2Fixed(2)){
            self->springboard->spr_pressed = true;
        }else if(self->Position.y == self->springboard->Holdable.Position.y + int2Fixed(4)){
            player_spring(self); // y unused
            self->springboard->spr_pressed = false;
        }
    }
    else if(self->state == 10){
        // throw grapple state

        // grapple movement and hitting stuff
        int amount = fixed2Int(min2(int2Fixed(64) - abs_(self->grapple_x - self->Position.x), int2Fixed(6)));
        bool grabbed = false;
        for(int i = 1; i <= amount; i++){
            int hit = player_grapple_check(self, self->grapple_x+int2Fixed(self->grapple_dir), self->grapple_y);
            if(hit == 0)
                hit = player_grapple_check(self, self->grapple_x+int2Fixed(self->grapple_dir), self->grapple_y - int2Fixed(1));
            if(hit == 0)
                hit = player_grapple_check(self, self->grapple_x+int2Fixed(self->grapple_dir), self->grapple_y + int2Fixed(1));

            int mode = 0;
            switch(self->grapple_hit_type){
            case GrappleHit_Snowball:
            case GrappleHit_Springboard:
                mode = 3; break;
            case GrappleHit_Grappler:
                mode = 2; break;
            case GrappleHit_Crumble:
                mode = 1; break;
            default:
                mode = 0; break;
            }

            if(hit == 0)
                self->grapple_x += int2Fixed(self->grapple_dir*2);
            else if(hit == 1){
                    if(mode == 2){
                        struct Grappler_data* grappler = self->grapple_hit_object;
                        self->grapple_x = grappler->Position.x + int2Fixed(4);
                        self->grapple_y = grappler->Position.y + int2Fixed(4);
                    }else if(mode == 3){
                        ((struct Holdable_data*)self->grapple_hit_object)->held = true;
                        grabbed = true;
                    }

                    if(mode == 3)
                        self->state = 12;
                    else
                        self->state = 11;
                    self->grapple_wave = int2Fixed(2);
                    self->grapple_boost = false;
                    self->freeze = 2;
                    psfx(SFX_SFX14_0_5, -1);
                    break;
            }

            if(hit == 2 || (hit == 0 && abs_(self->grapple_x - self->Position.x) >= int2Fixed(64))){
                if(hit == 2)
                    psfx(SFX_SFX7_8_3, -1);
                else
                    psfx(SFX_SFX14_8_3, -1);
                self->grapple_retract = true;
                self->freeze = 2;
                self->state = 0;
                break;
            }
        }

        // grapple wave
        self->grapple_wave = approach(self->grapple_wave, int2Fixed(1), float2Fixed(0.2f));
        self->spr = int2Fixed(1);

        // release
        if(!grabbed && (!input_grapple || abs_(self->Position.y - self->grapple_y) > int2Fixed(8))){
            self->state = 0;
            self->grapple_retract = true;
            //psfx(-2) // useless, as no sfx loops?
        }
    }
    else if(self->state == 11){
        // grapple attached state // only Grappler and Crumble

        // start boost
        if(!self->grapple_boost){
            self->grapple_boost = true;
            self->Movement.speed_x = int2Fixed(self->grapple_dir * 8);
        }

        // acceleration
        self->Movement.speed_x = approach(self->Movement.speed_x, int2Fixed(self->grapple_dir*5), float2Fixed(0.25f));
        self->Movement.speed_y = approach(self->Movement.speed_y, int2Fixed(0), float2Fixed(0.4f));

        // y-correction
        if(self->Movement.speed_y == int2Fixed(0)){
            if(self->Position.y - int2Fixed(3) > self->grapple_y)
                object_move_y(&self->Position, &self->Hitbox, &self->Movement, float2Fixed(-0.5f), NULL, NULL);
            if(self->Position.y - int2Fixed(3) < self->grapple_y)
                object_move_y(&self->Position, &self->Hitbox, &self->Movement, float2Fixed(0.5f), NULL, NULL);
        }

        // wall pose
        if(self->spr != int2Fixed(2) && object_check_solid(&self->Position, &self->Hitbox, int2Fixed(self->grapple_dir), int2Fixed(0))){
            self->spr = int2Fixed(2);
            psfx(SFX_SFX14_8_3, -1);
        }

        // jumps
        if(consume_jump_press()){
            if(object_check_solid(&self->Position, &self->Hitbox, int2Fixed(self->grapple_dir * 2), int2Fixed(0)))
                player_wall_jump(self, -self->grapple_dir);
            else{
                self->grapple_jump_grace_y = self->Position.y;
                player_grapple_jump(self);
            }
        }

        // grapple wave
        self->grapple_wave = approach(self->grapple_wave, int2Fixed(0), float2Fixed(0.6f));

        // release
        if(!input_grapple /*|| (self->grapple_hit_type!=GrappleHit_Nothing)*/){ // Grappler and Crumble can not be destroyed
            self->state = 0;
            self->t_grapple_jump_grace = 4;
            self->grapple_jump_grace_y = self->Position.y;
            self->grapple_retract = true;
            self->facing *= -1;
            if(abs_(self->Movement.speed_x) > int2Fixed(5))
                self->Movement.speed_x = int2Fixed(sgn(self->Movement.speed_x) * 5);
            else if(abs_(self->Movement.speed_x) <= float2Fixed(0.5f))
                self->Movement.speed_x = int2Fixed(0);
        }

        // release if beyond grapple point
        if(sgn(self->Position.x - self->grapple_x) == self->grapple_dir){
            self->state = 0;
            if(self->grapple_hit_type == GrappleHit_Grappler){
                self->t_grapple_jump_grace = 4;
                self->grapple_jump_grace_y = self->Position.y;
            }
            if(abs_(self->Movement.speed_x) > int2Fixed(5))
                self->Movement.speed_x = int2Fixed(sgn(self->Movement.speed_x) * 5);
        }
    }
    else if(self->state == 12){
        // grapple pull state
        struct Holdable_data* obj = self->grapple_hit_object;
        // pull
        if(object_move_x(&obj->Position, &obj->Hitbox, &obj->Movement,
            -int2Fixed(self->grapple_dir * 6), (bool (*)(void*, int))pull_collide_x, obj)){
            self->state = 0;
            self->grapple_retract = true;
            switch(self->grapple_hit_type){
                case GrappleHit_Snowball:
                    snowball_on_release(obj, true); break;
                case GrappleHit_Springboard:
                    springboard_on_release(obj, true); break;
                default:
                    // should not happen
#ifdef DEBUG
                    errorHalt("pullstate");
                    break;
#endif
            }
            obj->held = false;
            return;
        }else
            self->grapple_x = approach(self->grapple_x, self->Position.x, int2Fixed(6));
        // y-correct
        if(obj->Position.y != self->Position.y - int2Fixed(7))
            object_move_y(&obj->Position, &obj->Hitbox, &obj->Movement,
                        sgn(self->Position.y - obj->Position.y - int2Fixed(7)) * float2Fixed(0.5f), NULL, NULL);
        // grapple wave
        self->grapple_wave = approach(self->grapple_wave, int2Fixed(0), float2Fixed(0.6f));
        // hold
        if(object_overlaps(&self->Position, &self->Hitbox, &obj->Position, &obj->Hitbox,
                int2Fixed(0), int2Fixed(0))){
            self->state = 1;
            psfx(SFX_SFX7_16_6, -1);
        }
        // release
        if(!input_grapple || abs_(obj->Position.y - self->Position.y + int2Fixed(7)) > int2Fixed(8) ||
            sgn(obj->Position.x + int2Fixed(4) - self->Position.x) == -(self->grapple_dir)){
            self->state = 0;
            self->grapple_retract = true;
            player_release_holding(self, obj, self->grapple_hit_type, -int2Fixed(self->grapple_dir*5), int2Fixed(0), true);
        }
    }
    else if(self->state == 50){
        // grapple pickup state
        self->Movement.speed_y = min2(self->Movement.speed_y + float2Fixed(0.8f), float2Fixed(4.5f));
        self->Movement.speed_x = approach(self->Movement.speed_x, int2Fixed(0), float2Fixed(0.2f));
        if(on_ground){
            if(self->t_grapple_pickup == 0) music(39);
            if(self->t_grapple_pickup == 61) music(-1);
            if(self->t_grapple_pickup == 70) music(22);
            if(self->t_grapple_pickup > 80) self->state = 0;
            self->t_grapple_pickup += 1;
        }
    }
    else if(self->state == 99 || self->state == 100){
        // dead / finished state
        if(self->state == 100){
            self->Position.x += int2Fixed(1);
            if(self->wipe_timer == 5 && level_index > 1){
                psfx(SFX_SFX17_24_9, -1);
            }
        }

        self->wipe_timer += 1;
        if(self->wipe_timer > 20){
            if(self->state == 99)
                restart_level();
            else
                next_level();
        }
        return;
    }
    
    // apply
    //mgba_printf(MGBA_LOG_DEBUG, "move x| pos: %f %f move: %f %f", fixed2Float(self->Position.x), fixed2Float(self->Position.y), fixed2Float(self->Movement.speed_x), fixed2Float(self->Movement.remainder_x));
    object_move_x(&self->Position, &self->Hitbox, &self->Movement, self->Movement.speed_x, (bool (*)(void*, int))player_on_collide_x, self);
    object_move_y(&self->Position, &self->Hitbox, &self->Movement, self->Movement.speed_y, (bool (*)(void*, int))player_on_collide_y, self);

    // holding
    if(self->holding_type != GrappleHit_Nothing){
        ((struct Holdable_data*)self->holding_object)->Position.x = self->Position.x - int2Fixed(4);
        ((struct Holdable_data*)self->holding_object)->Position.y = self->Position.y - int2Fixed(14);
    }

    // sprite
    if(self->state == 50 && self->t_grapple_pickup > 0)
        self->spr = int2Fixed(3);
    else if(self->state != 11){
        if(!on_ground)
            self->spr = int2Fixed(1);
        else if(input_x != 0){
            self->spr += float2Fixed(0.25f);
            self->spr = int2Fixed(0 + fixed2Int(self->spr)%2)+getDecimals(self->spr);
        }else
            self->spr = int2Fixed(0);
    }

    // object interactions
    // grapple pickup
    if(current_gameobjects.grapplePickup_enabled)
        if(!current_gameobjects.grapplePickup.destroyed)
        if(object_overlaps(&self->Position, &self->Hitbox, &current_gameobjects.grapplePickup.Position, &DEFAULT_HITBOX, int2Fixed(0), int2Fixed(0))){
            current_gameobjects.grapplePickup.destroyed = true;
            have_grapple = true;
            psfx(SFX_SFX7_12_4, -1);
            self->state = 50;
        }
    // falling bridge tile
    if(current_gameobjects.bridge_enabled)
        for(int i = 0; i < BRIDGE_COUNT; i++){
            if(!current_gameobjects.bridge[i].falling)
                if(object_overlaps(&self->Position, &self->Hitbox, &current_gameobjects.bridge[i].Position, &DEFAULT_HITBOX, int2Fixed(0), int2Fixed(0))){
                    current_gameobjects.bridge[i].falling = true;
                    self->freeze = 1;
                    shake = 2;
                    psfx(SFX_SFX8_16_4, -1);
                }
        }
    // snowball
    for(int i = 0; i < MAX_SNOWBALL_COUNT; i++){
        if(!current_gameobjects.snowballs[i].destroyed)
        if(!current_gameobjects.snowballs[i].Holdable.held){
            if(player_bounce_check(self, current_gameobjects.snowballs[i].Holdable.Movement.speed_y, current_gameobjects.snowballs[i].Holdable.Position.y) &&
                    snowball_bounce_overlaps(current_gameobjects.snowballs+i, self)){
                player_bounce(self, current_gameobjects.snowballs[i].Holdable.Position.x + int2Fixed(4), current_gameobjects.snowballs[i].Holdable.Position.y);
                psfx(SFX_SFX17_0_2, -1);
                current_gameobjects.snowballs[i].Holdable.freeze = 1;
                current_gameobjects.snowballs[i].Holdable.Movement.speed_y = int2Fixed(-1);
                snowball_hurt(current_gameobjects.snowballs+i);
            }else if(current_gameobjects.snowballs[i].Holdable.Movement.speed_x != int2Fixed(0) && current_gameobjects.snowballs[i].Holdable.thrown_timer <= 0){
                if(object_overlaps(&self->Position, &self->Hitbox, &current_gameobjects.snowballs[i].Holdable.Position, &current_gameobjects.snowballs[i].Holdable.Hitbox, 
                                    int2Fixed(0), int2Fixed(0))){
                    player_die(self);
                    return;
                }
            }
        }
    }
    // springboard
    for(int i = 0; i < current_gameobjects.springboards_len; i++){
        if(!current_gameobjects.springboards[i].destroyed)
        if(self->state != 2 && !current_gameobjects.springboards[i].Holdable.held)
            if(player_bounce_check(self, current_gameobjects.springboards[i].Holdable.Movement.speed_y, current_gameobjects.springboards[i].Holdable.Position.y))
                if(object_overlaps(&self->Position, &self->Hitbox, &current_gameobjects.springboards[i].Holdable.Position, &current_gameobjects.springboards[i].Holdable.Hitbox, 
                                    int2Fixed(0), int2Fixed(0))){
                    self->state = 2;
                    self->Movement.speed_x = int2Fixed(0);
                    self->Movement.speed_y = int2Fixed(0);
                    self->t_jump_grace = 0;
                    self->springboard = current_gameobjects.springboards+i;
                    self->Movement.remainder_y = int2Fixed(0);
                    current_gameobjects.springboards[i].player_assigned = true;
                    object_move_y(&self->Position, &self->Hitbox, &self->Movement, 
                                    current_gameobjects.springboards[i].Holdable.Position.y + int2Fixed(4) - self->Position.y,
                                    NULL, NULL);
                }
    }
    // berry
    for(int i = 0; i < current_gameobjects.berries_len; i++){
        if(!current_gameobjects.berries[i].destroyed)
        if(object_overlaps(&self->Position, &self->Hitbox, &current_gameobjects.berries[i].Position, &current_gameobjects.berries[i].Hitbox, int2Fixed(0), int2Fixed(0)))
            berry_collect(current_gameobjects.berries+i);
    }
    // crumble
    for(int i = 0; i < current_gameobjects.crumbles_len; i++){
        if(current_gameobjects.crumbles[i].breaking == Crumble_Normal){
            if(self->state == 0 && object_overlaps(&self->Position, &self->Hitbox, &current_gameobjects.crumbles[i].Position, &current_gameobjects.crumbles[i].Hitbox,
                                                    int2Fixed(0), int2Fixed(1))){
                current_gameobjects.crumbles[i].breaking = Crumble_Breaking;
                psfx(SFX_SFX8_20_4, -1);
            }else if(self->state == 11){
                if(object_overlaps(&self->Position, &self->Hitbox, &current_gameobjects.crumbles[i].Position, &current_gameobjects.crumbles[i].Hitbox,
                                    int2Fixed(self->grapple_dir), int2Fixed(0)) ||
                    object_overlaps(&self->Position, &self->Hitbox, &current_gameobjects.crumbles[i].Position, &current_gameobjects.crumbles[i].Hitbox,
                                    int2Fixed(self->grapple_dir), int2Fixed(3)) ||
                    object_overlaps(&self->Position, &self->Hitbox, &current_gameobjects.crumbles[i].Position, &current_gameobjects.crumbles[i].Hitbox,
                                    int2Fixed(self->grapple_dir), int2Fixed(-2))){
                        current_gameobjects.crumbles[i].breaking = Crumble_Breaking;
                        psfx(SFX_SFX8_20_4, -1);
                    }
            }
        }
    }
    // checkpoint
    for(int i = 0; i < current_gameobjects.checkpoints_len; i++){
        if(level_checkpoint != current_gameobjects.checkpoints+i)
            if(object_overlaps(&self->Position, &self->Hitbox, &current_gameobjects.checkpoints[i].Position, &DEFAULT_HITBOX, int2Fixed(0), int2Fixed(0))){
                level_checkpoint = current_gameobjects.checkpoints+i;
                psfx(SFX_SFX8_24_6, 20);
            }
    }

    // death
    if(self->state < 99 && (self->Position.y > int2Fixed(level.height*8+16) || player_hazard_check(self, int2Fixed(0), int2Fixed(0)))){
        if(level_index == 1 && self->Position.x > int2Fixed(level.width*8-64)){
            self->state = 100;
            self->wipe_timer = -15;
        }else
            player_die(self);
        return;
    }

    // bounds
    if(self->Position.y < int2Fixed(-16)){
        self->Position.y = int2Fixed(-16);
        self->Movement.speed_y = int2Fixed(0);
    }
    if(self->Position.x < int2Fixed(3)){
        self->Position.x = int2Fixed(3);
        self->Movement.speed_x = int2Fixed(0);
    }else if(self->Position.x > int2Fixed(level.width*8-3)){
        if(level.right_edge){
            self->Position.x = int2Fixed(level.width*8-3);
            self->Movement.speed_x = int2Fixed(0);
        }else
            self->state = 100;
    }

    // intro bridge music
    if(current_music == levels[1].music && self->Position.x > int2Fixed(61*8)){
        current_music = 37;
        music(37);
        psfx(SFX_SFX17_24_9, -1);
    }

    // ending music
    if(level_index == 8){
        if(current_music != 40 && self->Position.y > int2Fixed(40)){
            current_music = 40;
            music(40);
        }
        if(self->Position.y > int2Fixed(376)) show_score += 1;
        if(show_score == 120) music(38);
    }

    // camera
    camera_modes[level_index](fixed2Int(self->Position.x), fixed2Int(self->Position.y));
    camera_x = approach(camera_x, camera_target_x, 5);
    camera_y = approach(camera_y, camera_target_y, 5);
    camera(camera_x, camera_y);
}

static bool player_on_collide_x(struct Player_data* self, int target){
    if(self->state == 0){
        if(sgn(target) == input_x && object_corner_correct(&self->Position, &self->Hitbox, input_x, 0, 2, 2, -1, true))
            return false;
    }else if(self->state == 11){
        if(object_corner_correct(&self->Position, &self->Hitbox, self->grapple_dir, 0, 4, 2, 0, true))
            return false;
    }
    return object_on_collide_x(&self->Position, &self->Hitbox, &self->Movement, target);
}

static bool player_on_collide_y(struct Player_data* self, int target){
    if(target < 0 && object_corner_correct(&self->Position, &self->Hitbox, 0, -1, 2, 1, input_x, true))
        return false;
    self->t_var_jump = 0;
    return object_on_collide_y(&self->Position, &self->Hitbox, &self->Movement, target);
}