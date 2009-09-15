
#include <windows.h>
#include <winbase.h>
#include <math.h>
#include "keeperfx.h"

#include "bflib_math.h"
#include "bflib_memory.h"
#include "bflib_keybrd.h"
#include "bflib_datetm.h"
#include "bflib_bufrw.h"
#include "bflib_sprite.h"
#include "bflib_sprfnt.h"
#include "bflib_fileio.h"
#include "bflib_dernc.h"
#include "bflib_sndlib.h"
#include "bflib_fmvids.h"
#include "bflib_cpu.h"
#include "bflib_video.h"
#include "bflib_vidraw.h"
#include "bflib_guibtns.h"
#include "bflib_sound.h"
#include "bflib_mouse.h"
#include "bflib_filelst.h"
#include "bflib_network.h"

#include "front_simple.h"
#include "frontend.h"
#include "front_input.h"
#include "gui_draw.h"
#include "gui_tooltips.h"
#include "scrcapt.h"
#include "vidmode.h"
#include "kjm_input.h"
#include "packets.h"
#include "config.h"
#include "config_campaigns.h"
#include "config_terrain.h"
#include "config_rules.h"
#include "config_lenses.h"
#include "config_magic.h"
#include "config_creature.h"
#include "config_crtrmodel.h"
#include "lvl_script.h"
#include "lvl_filesdk1.h"
#include "thing_list.h"
#include "player_instances.h"
#include "game_saves.h"
#include "engine_render.h"
#include "engine_lenses.h"
#include "engine_camera.h"
#include "front_landview.h"
#include "thing_creature.h"
#include "slab_data.h"
#include "room_data.h"
#include "creature_control.h"

int test_variable;

// Max length of the command line
#define CMDLN_MAXLEN 259
char cmndline[CMDLN_MAXLEN+1];
unsigned short bf_argc;
char *bf_argv[CMDLN_MAXLEN+1];

const struct GameSettings default_settings = {
   0, 4, 3, 0, 1, 0, 127, 90, 1, 0, 1,
   {
    {KC_UP, KM_NONE},       {KC_DOWN, KM_NONE},
    {KC_LEFT, KM_NONE},     {KC_RIGHT, KM_NONE},
    {KC_LCONTROL, KM_NONE}, {KC_LSHIFT, KM_NONE},
    {KC_DELETE, KM_NONE},   {KC_PGDOWN, KM_NONE},
    {KC_HOME, KM_NONE},     {KC_END, KM_NONE},
    {KC_T, KM_NONE},        {KC_L, KM_NONE},
    {KC_L, KM_SHIFT},       {KC_P, KM_SHIFT},
    {KC_T, KM_ALT},         {KC_T, KM_SHIFT},
    {KC_H, KM_NONE},        {KC_W, KM_NONE},
    {KC_S, KM_NONE},        {KC_T, KM_CONTROL},
    {KC_G, KM_NONE},        {KC_B, KM_NONE},
    {KC_H, KM_SHIFT},       {KC_G, KM_SHIFT},
    {KC_B, KM_SHIFT},       {KC_F, KM_NONE},
    {KC_A, KM_NONE},        {KC_LSHIFT, KM_NONE},
    {KC_NUMPAD0, KM_NONE},  {KC_BACK, KM_NONE},
    {KC_P, KM_NONE},        {KC_M, KM_NONE},
   }, 1, 0, 6};

struct KeyToStringInit key_to_string_init[] = {
  {KC_A,  -65},
  {KC_B,  -66},
  {KC_C,  -67},
  {KC_D,  -68},
  {KC_E,  -69},
  {KC_F,  -70},
  {KC_G,  -71},
  {KC_H,  -72},
  {KC_I,  -73},
  {KC_J,  -74},
  {KC_K,  -75},
  {KC_L,  -76},
  {KC_M,  -77},
  {KC_N,  -78},
  {KC_O,  -79},
  {KC_P,  -80},
  {KC_Q,  -81},
  {KC_R,  -82},
  {KC_S,  -83},
  {KC_T,  -84},
  {KC_U,  -85},
  {KC_V,  -86},
  {KC_W,  -87},
  {KC_X,  -88},
  {KC_Y,  -89},
  {KC_Z,  -90},
  {KC_F1,  515},
  {KC_F2,  516},
  {KC_F3,  517},
  {KC_F4,  518},
  {KC_F5,  519},
  {KC_F6,  520},
  {KC_F7,  521},
  {KC_F8,  522},
  {KC_F9,  523},
  {KC_F10, 524},
  {KC_F11, 525},
  {KC_F12, 526},
  {KC_CAPITAL, 490},
  {KC_LSHIFT,  483},
  {KC_RSHIFT,  484},
  {KC_LCONTROL, 481},
  {KC_RCONTROL, 482},
  {KC_RETURN,  488},
  {KC_BACK,    491},
  {KC_INSERT,  492},
  {KC_DELETE,  493},
  {KC_HOME,    494},
  {KC_END,     495},
  {KC_PGUP,    496},
  {KC_PGDOWN,  497},
  {KC_NUMLOCK, 498},
  {KC_DIVIDE,  499},
  {KC_MULTIPLY, 500},
  {KC_NUMPADENTER, 503},
  {KC_DECIMAL, 504},
  {KC_NUMPAD0, 514},
  {KC_NUMPAD1, 505},
  {KC_NUMPAD2, 506},
  {KC_NUMPAD3, 507},
  {KC_NUMPAD4, 508},
  {KC_NUMPAD5, 509},
  {KC_NUMPAD6, 510},
  {KC_NUMPAD7, 511},
  {KC_NUMPAD8, 512},
  {KC_NUMPAD9, 513},
  {KC_UP,     527},
  {KC_DOWN,   528},
  {KC_LEFT,   529},
  {KC_RIGHT,  530},
  {  0,     0},
};

// Map size variables
int map_subtiles_x = 255;
int map_subtiles_y = 255;
int map_tiles_x = 85;
int map_tiles_y = 85;

unsigned short player_colors_map[] = {0, 1, 2, 3, 4, 5, 0, 0, 0, };

TbPixel const player_path_colours[] = {131, 90, 163, 181, 20, 4, };

unsigned short const player_cubes[] = {0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C7, 0x00C6 };

struct KeycodeString eastegg_feckoff_codes = {
    {KC_F,KC_E,KC_C,KC_K,KC_O,KC_F,KC_F,KC_UNASSIGNED}, 7,
};
struct KeycodeString eastegg_jlw_codes = {
    {KC_J,KC_L,KC_W,KC_UNASSIGNED}, 3,
};
struct KeycodeString eastegg_skeksis_codes = {
    {KC_S,KC_K,KC_E,KC_K,KC_S,KC_I,KC_S,KC_UNASSIGNED}, 7,
};

char onscreen_msg_text[255]="";
int onscreen_msg_turns = 0;

const char *sound_fname = "sound.dat";
const char *speech_fname = "speech.dat";

char sound_dir[64] = "SOUND";
char window_class_name[128]="Bullfrog Shell";
short default_loc_player = 0;
short hero_player = 4;
unsigned long gold_per_hoarde = 2000;
struct StartupParameters start_params;
#define TRACE LbNetLog

Thing_Class_Func class_functions[] = {
  NULL,
  update_object,
  update_shot,
  update_effect_element,
  update_dead_creature,
  update_creature,
  update_effect,
  process_effect_generator,
  update_trap,
  process_door,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
};

//long const imp_spangle_effects[] = {

const struct Around around[] = {
  {-1,-1},
  {-1, 0},
  {-1, 1},
  { 0,-1},
  { 0, 0},
  { 0, 1},
  { 1,-1},
  { 1, 0},
  { 1, 1},
  { 0, 0}, // this entry shouldn't be used
};

const char *blood_types[] = {
    "ARh+",
    "O",
    "MoO+",
    "BA",
    "PoE",
    "BO",
    "IkI",
    NULL,
};

const short door_names[] = {
    201, 590, 591, 592, 593, 0,
};

Phrase phrases[] = {
    0,  1,  2, 3,  4,   5,  6,  7,  8, 9,  10, 11, 12, 13, 14, 15,
   16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
   32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
   48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
   64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
   80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
   96, 97, 98, 99,100,101,102,103,104,105,106,107,108,109,110,111,
  112,113,114,115,116,117,118,119,120,121,122,123,124,125,
};

struct SMessage messages[] = {
  {  0, 0, 0},
  {  1, 1, 0},
  {  2, 1, 0},
  {  3, 1, 0},
  {  4, 1, 0},
  {  5, 1, 0},
  {  6, 1, 0},
  {  7, 1, 0},
  {  8, 1, 0},
  {  9, 1, 0},
  { 10, 1, 0},
  { 11, 1, 0},
  { 12, 1, 0},
  { 13, 1, 0},
  { 14, 1, 0},
  { 15, 1, 0},
  { 16, 1, 0},
  { 17, 1, 0},
  { 18, 1, 0},
  { 19, 1, 0},
  { 20, 1, 0},
  { 21, 1, 0},
  { 22, 1, 0},
  { 23, 1, 0},
  { 24, 1, 0},
  { 25, 1, 0},
  { 26, 1, 0},
  { 27, 1, 0},
  { 28, 1, 0},
  { 29, 1, 0},
  { 30, 1, 0},
  { 31, 1, 0},
  { 32, 1, 0},
  { 33, 1, 0},
  { 34, 1, 0},
  { 35, 1, 0},
  { 36, 1, 0},
  { 37, 1, 0},
  { 38, 1, 0},
  { 39, 1, 0},
  { 40, 1, 0},
  { 41, 1, 0},
  { 42, 1, 0},
  { 43, 1, 0},
  { 44, 1, 0},
  { 45, 1, 0},
  { 46, 1, 0},
  { 47, 1, 0},
  { 48, 1, 0},
  { 49, 1, 0},
  { 50, 1, 0},
  { 51, 1, 0},
  { 52, 1, 0},
  { 53, 1, 0},
  { 54, 1, 0},
  { 55, 1, 0},
  { 56, 1, 0},
  { 57, 1, 0},
  { 58, 1, 0},
  { 59, 1, 0},
  { 60, 1, 0},
  { 61, 1, 0},
  { 62, 1, 0},
  { 63, 1, 0},
  { 64, 1, 0},
  { 65, 1, 0},
  { 66, 1, 0},
  { 67, 1, 0},
  { 68, 1, 0},
  { 69, 1, 0},
  { 70, 1, 0},
  { 71, 1, 0},
  { 72, 1, 0},
  { 73, 1, 0},
  { 74, 1, 0},
  { 75, 1, 0},
  { 76, 1, 0},
  { 77, 1, 0},
  { 78, 1, 0},
  { 79, 1, 0},
  { 80, 1, 0},
  { 81, 1, 0},
  { 82, 1, 0},
  { 83, 1, 0},
  { 84, 1, 0},
  { 85, 1, 0},
  { 86, 1, 0},
  { 87, 1, 0},
  { 88, 1, 0},
  { 89, 1, 0},
  { 90, 1, 0},
  { 91, 1, 0},
  { 92, 1, 0},
  { 93, 1, 0},
  { 94, 1, 0},
  { 95, 1, 0},
  { 96, 1, 0},
  { 97, 1, 0},
  { 98, 1, 0},
  { 99, 1, 0},
  {100, 1, 0},
  {101, 1, 0},
  {102, 1, 0},
  {103, 1, 0},
  {104, 1, 0},
  {105, 1, 0},
  {106, 1, 0},
  {107, 1, 0},
  {108, 1, 0},
  {109, 1, 0},
  {110, 1, 0},
  {111, 1, 0},
  {112, 1, 0},
  {113, 1, 0},
  {114, 1, 0},
  {115, 1, 0},
  {116, 1, 0},
  {117, 1, 0},
  {118, 1, 0},
  {119, 1, 0},
  {120, 1, 0},
  {121, 1, 0},
  {122, 1, 0},
  {123, 1, 0},
  {124, 1, 0},
  {125, 1, 0},
};

unsigned short const player_state_to_spell[] = {
  0, 0, 0,  0,  0,  0, 6, 7, 5, 0, 18, 18, 0, 0, 0, 0,
  0,10, 0, 11, 12, 13, 8, 0, 2,16, 14, 15, 0, 3, 0, 0,
};

struct SpellData spell_data[] = {
  {36, 11, 0,   0,   0,   0,   0,   0,  0, NULL,                 0, 0},      //[0]
  { 0,  0, 0,   0,   0,   0,   0,   0,  0, NULL,                 0, 0},      //[1]
  {36, 24, 0,  95, 118, 631, 648, 831,  5, NULL,                 0, 0},      //[2]
  {97,  0, 0, 394, 452, 636, 653, 834,  0, NULL,                 0, 0},      //[3]
  { 0,  0, 0,   0,   0,   0,   0,   0,  0, NULL,                 0, 0},      //[4]
  {36,  8, 1,  85, 108, 632, 649, 828, 12, sight_of_evil_expand_check,0, 0}, //[5] Sight of Evil
  {36,  6, 1,  93, 116, 633, 650, 826,  0, call_to_arms_expand_check, 1, 1}, //[6] Call To Arms
  {36,  7, 1,  97, 120, 635, 652, 837, 10, general_expand_check, 0, 0},      //[7]
  {36, 22, 0,  87, 110, 644, 661, 829,  8, general_expand_check, 1, 0},      //[8]
  {41,  0, 0,  89, 112, 634, 651, 830,  0, general_expand_check, 0, 0},      //[9] Hold Audience
  {36, 17, 0, 101, 124, 640, 657, 833,  6, general_expand_check, 1, 1},      //[10]
  {36, 19, 0,  99, 122, 637, 654, 838, 11, general_expand_check, 1, 0},      //[11]
  {36, 20, 0, 103, 126, 638, 655, 825,  9, general_expand_check, 1, 0},      //[12]
  {36, 21, 0, 105, 128, 639, 656, 832,  1, general_expand_check, 1, 0},      //[13]
  {36, 26, 0, 310, 319, 642, 659, 835,  3, general_expand_check, 1, 1},      //[14]
  {36, 27, 0, 306, 314, 641, 658, 827,  2, general_expand_check, 1, 1},      //[15]
  {36, 25, 0, 308, 317, 643, 660, 839,  4, general_expand_check, 0, 0},      //[16]
  {36, 28, 0, 105, 128, 645, 662,   0,  0, NULL,                 0, 0},      //[17]
  {36, 11, 0,  91, 114, 630, 647, 836,  7, NULL,                 1, 0},      //[18] Possession
  {98,  0, 0, 312, 321, 646, 663, 824,  0, NULL,                 0, 0},      //[19] Armageddon
  { 0,  0, 0,   0,   0,   0,   0,   0,  0, NULL,                 0, 0},      //[20]
};

long instf_creature_fire_shot(struct Thing *thing, long *param);

//static
TbClockMSec last_loop_time=0;

#ifdef __cplusplus
extern "C" {
#endif
DLLIMPORT void _DK_engine_init(void);
DLLIMPORT long _DK_load_anim_file(void);
DLLIMPORT long _DK_load_cube_file(void);
DLLIMPORT void _DK_init_colours(void);
DLLIMPORT void _DK_close_sound_heap(void);
DLLIMPORT long _DK_init_sound_heap_two_banks(unsigned char *a1, long a2, char *a3, char *a4, long a5);
DLLIMPORT void _DK_draw_mini_things_in_hand(long x, long y);
DLLIMPORT void _DK_process_keeper_sprite(short x, short y, unsigned short a3, short a4, unsigned char a5, long a6);
DLLIMPORT long _DK_power_sight_explored(long stl_x, long stl_y, unsigned char plyr_idx);
DLLIMPORT long _DK_can_cast_spell_on_creature(long a1, struct Thing *thing, long a3);
DLLIMPORT unsigned char _DK_can_cast_spell_at_xy(unsigned char a1, unsigned char a2, unsigned char a3, unsigned char a4, long a5);
DLLIMPORT long _DK_update_navigation_triangulation(long start_x, long start_y, long end_x, long end_y);
DLLIMPORT void _DK_place_animating_slab_type_on_map(long a1, char a2, unsigned char a3, unsigned char a4, unsigned char a5);
DLLIMPORT struct Thing *_DK_get_spellbook_at_position(long x, long y);
DLLIMPORT struct Thing *_DK_get_special_at_position(long x, long y);
DLLIMPORT void _DK_draw_spell_cursor(unsigned char a1, unsigned short a2, unsigned char stl_x, unsigned char stl_y);
DLLIMPORT long _DK_take_money_from_dungeon(short a1, long a2, unsigned char a3);
DLLIMPORT unsigned char _DK_find_door_of_type(unsigned long a1, unsigned char a2);
DLLIMPORT void _DK_maintain_my_event_list(struct Dungeon *dungeon);
DLLIMPORT struct Event *_DK_event_create_event(long map_x, long map_y, unsigned char a3, unsigned char dngn_id, long msg_id);
DLLIMPORT void _DK_process_armageddon(void);
DLLIMPORT void _DK_update_breed_activities(void);
DLLIMPORT void _DK_maintain_my_battle_list(void);
DLLIMPORT void _DK_magic_use_power_chicken(unsigned char a1, struct Thing *thing, long a3, long a4, long a5);
DLLIMPORT void _DK_magic_use_power_disease(unsigned char a1, struct Thing *thing, long a3, long a4, long a5);
DLLIMPORT void _DK_magic_use_power_destroy_walls(unsigned char a1, long a2, long a3, long a4);
DLLIMPORT short _DK_magic_use_power_imp(unsigned short a1, unsigned short a2, unsigned short a3);
DLLIMPORT long _DK_remove_workshop_object_from_player(long a1, long a2);
DLLIMPORT void _DK_magic_use_power_heal(unsigned char a1, struct Thing *thing, long a3, long a4, long a5);
DLLIMPORT void _DK_magic_use_power_conceal(unsigned char a1, struct Thing *thing, long a3, long a4, long a5);
DLLIMPORT void _DK_magic_use_power_armour(unsigned char a1, struct Thing *thing, long a3, long a4, long a5);
DLLIMPORT void _DK_magic_use_power_speed(unsigned char a1, struct Thing *thing, long a3, long a4, long a5);
DLLIMPORT void _DK_magic_use_power_lightning(unsigned char a1, long a2, long a3, long a4);
DLLIMPORT unsigned char _DK_tag_cursor_blocks_place_trap(unsigned char a1, long a2, long a3);
DLLIMPORT long _DK_magic_use_power_sight(unsigned char a1, long a2, long a3, long a4);
DLLIMPORT void _DK_magic_use_power_cave_in(unsigned char a1, long a2, long a3, long a4);
DLLIMPORT long _DK_magic_use_power_call_to_arms(unsigned char a1, long a2, long a3, long a4, long a5);
DLLIMPORT void _DK_stop_creatures_around_hand(char a1, unsigned short a2, unsigned short a3);
DLLIMPORT struct Thing *_DK_get_queryable_object_near(unsigned short a1, unsigned short a2, long a3);
DLLIMPORT long _DK_object_is_gold_pile(struct Thing *thing);
DLLIMPORT int _DK_can_thing_be_queried(struct Thing *thing, long a2);
DLLIMPORT int _DK_can_thing_be_possessed(struct Thing *thing, long a2);
DLLIMPORT short _DK_magic_use_power_hand(unsigned short a1, unsigned short a2, unsigned short a3, unsigned short a4);
DLLIMPORT long _DK_tag_blocks_for_digging_in_rectangle_around(long a1, long a2, char a3);
DLLIMPORT void _DK_untag_blocks_for_digging_in_rectangle_around(long a1, long a2, char a3);
DLLIMPORT long _DK_destroy_door(struct Thing *thing);
DLLIMPORT short _DK_delete_room_slab(long x, long y, unsigned char gnd_slab);
DLLIMPORT void _DK_tag_cursor_blocks_sell_area(unsigned char a1, long a2, long a3, long a4);
DLLIMPORT long _DK_packet_place_door(long a1, long a2, long a3, long a4, unsigned char a5);
DLLIMPORT void _DK_delete_room_slabbed_objects(long a1);
DLLIMPORT unsigned char _DK_tag_cursor_blocks_place_door(unsigned char a1, long a2, long a3);
DLLIMPORT long _DK_remove_workshop_item(long a1, long a2, long a3);
DLLIMPORT struct Thing *_DK_create_trap(struct Coord3d *pos, unsigned short a1, unsigned short a2);
DLLIMPORT struct Room *_DK_place_room(unsigned char a1, unsigned char a2, unsigned short a3, unsigned short a4);
DLLIMPORT unsigned char _DK_tag_cursor_blocks_place_room(unsigned char a1, long a2, long a3, long a4);
DLLIMPORT short _DK_magic_use_power_slap(unsigned short a1, unsigned short a2, unsigned short a3);
DLLIMPORT unsigned char _DK_initialise_thing_state(struct Thing *thing, long a2);
DLLIMPORT short _DK_setup_person_move_to_position(struct Thing *thing, long pos_x, long pos_y, unsigned char a4);
DLLIMPORT void _DK_tag_cursor_blocks_dig(unsigned char a1, long a2, long a3, long a4);
DLLIMPORT void _DK_tag_cursor_blocks_thing_in_hand(unsigned char a1, long a2, long a3, int a4, long a5);
DLLIMPORT void _DK_create_power_hand(unsigned char a1);
DLLIMPORT struct Thing *_DK_create_gold_for_hand_grab(struct Thing *thing, long a2);
DLLIMPORT long _DK_remove_food_from_food_room_if_possible(struct Thing *thing);
DLLIMPORT struct Thing *_DK_create_effect_element(struct Coord3d *pos, unsigned short a2, unsigned short a3);
DLLIMPORT unsigned long _DK_object_is_pickable_by_hand(struct Thing *thing, long a2);
DLLIMPORT void _DK_set_power_hand_offset(struct PlayerInfo *player, struct Thing *thing);
DLLIMPORT struct Thing *_DK_process_object_being_picked_up(struct Thing *thing, long a2);
DLLIMPORT void _DK_set_power_hand_graphic(long a1, long a2, long a3);
DLLIMPORT long _DK_dump_thing_in_power_hand(struct Thing *thing, long a2);
DLLIMPORT void _DK_process_person_moods_and_needs(struct Thing *thing);
DLLIMPORT unsigned char _DK_can_change_from_state_to(struct Thing *thing, long a2, long a3);
DLLIMPORT struct Thing *_DK_get_door_for_position(long pos_x, long pos_y);
DLLIMPORT struct Thing *_DK_get_trap_for_position(long pos_x, long pos_y);
DLLIMPORT struct Thing *_DK_get_trap_for_slab_position(long slb_x, long slb_y);
DLLIMPORT long _DK_process_obey_leader(struct Thing *thing);
DLLIMPORT unsigned char _DK_external_set_thing_state(struct Thing *thing, long state);
DLLIMPORT long _DK_is_thing_passenger_controlled(struct Thing *thing);
DLLIMPORT void __cdecl _DK_setup_3d(void);
DLLIMPORT void __cdecl _DK_setup_stuff(void);
DLLIMPORT void __cdecl _DK_init_keeper(void);
DLLIMPORT long _DK_find_column(struct Column *col);
DLLIMPORT long _DK_create_column(struct Column *col);
DLLIMPORT void _DK_light_delete_light(long idx);
DLLIMPORT void _DK_light_initialise_lighting_tables(void);
DLLIMPORT void _DK_check_map_for_gold(void);
DLLIMPORT void _DK_set_thing_draw(struct Thing *thing, long a2, long a3, long a4, char a5, char a6, unsigned char a7);
DLLIMPORT void _DK_light_set_light_minimum_size_to_cache(long a1, long a2, long a3);
DLLIMPORT struct Thing *_DK_create_object(struct Coord3d *pos, unsigned short model, unsigned short owner, long a4);
DLLIMPORT struct Thing *_DK_find_base_thing_on_mapwho(unsigned char oclass, unsigned short model, unsigned short x, unsigned short y);
DLLIMPORT void _DK_delete_room_structure(struct Room *room);
DLLIMPORT int __cdecl _DK_get_gui_inputs(int);
DLLIMPORT unsigned long _DK_can_drop_thing_here(long x, long y, long a3, unsigned long a4);
DLLIMPORT long _DK_thing_in_wall_at(struct Thing *thing, struct Coord3d *pos);
DLLIMPORT void _DK_do_map_rotate_stuff(long a1, long a2, long *a3, long *a4, long a5);
DLLIMPORT char _DK_mouse_is_over_small_map(int, int);
DLLIMPORT unsigned char _DK_active_battle_exists(unsigned char a1);
DLLIMPORT unsigned char _DK_step_battles_forward(unsigned char a1);
DLLIMPORT void _DK_go_to_my_next_room_of_type(unsigned long rkind);
DLLIMPORT struct ActionPoint *_DK_allocate_free_action_point_structure_with_number(long apt_num);
DLLIMPORT unsigned long _DK_action_point_get_players_within(long apt_idx);
DLLIMPORT void _DK_instant_instance_selected(long a1);
DLLIMPORT void _DK_initialise_map_collides(void);
DLLIMPORT void _DK_initialise_map_health(void);
DLLIMPORT void _DK_initialise_extra_slab_info(unsigned long lv_num);
DLLIMPORT long _DK_add_gold_to_hoarde(struct Thing *thing, struct Room *room, long amount);
DLLIMPORT struct Thing *_DK_create_door(struct Coord3d *pos, unsigned short a1, unsigned char a2, unsigned short a3, unsigned char a4);
DLLIMPORT struct Thing *_DK_create_effect_generator(struct Coord3d *pos, unsigned short a1, unsigned short a2, unsigned short a3, long a4);
DLLIMPORT void _DK_clear_mapwho(void);
DLLIMPORT void _DK_clear_map(void);
DLLIMPORT long _DK_ceiling_init(unsigned long a1, unsigned long a2);
DLLIMPORT void _DK_init_top_texture_to_cube_table(void);
DLLIMPORT void _DK_init_columns(void);
DLLIMPORT void _DK_init_whole_blocks(void);
DLLIMPORT void _DK_draw_jonty_mapwho(struct JontySpr *jspr);
DLLIMPORT void _DK_draw_keepsprite_unscaled_in_buffer(unsigned short a1, short a2, unsigned char a3, unsigned char *a4);
DLLIMPORT void _DK_draw_engine_number(struct Number *num);
DLLIMPORT void _DK_draw_engine_room_flag_top(struct RoomFlag *rflg);
DLLIMPORT struct Thing *_DK_get_nearest_thing_for_hand_or_slap(unsigned char a1, long a2, long a3);
DLLIMPORT long _DK_screen_to_map(struct Camera *camera, long scrpos_x, long scrpos_y, struct Coord3d *mappos);
DLLIMPORT void _DK_draw_lens(unsigned char *dstbuf, unsigned char *srcbuf, unsigned long *lens_mem, int width, int height, int scanln);
DLLIMPORT void _DK_flyeye_blitsec(unsigned char *srcbuf, unsigned char *dstbuf, long srcwidth, long dstwidth, long n, long height);
DLLIMPORT void _DK_draw_swipe(void);
DLLIMPORT void _DK_flyeye_setup(long width, long height);
DLLIMPORT void _DK_draw_texture(long a1, long a2, long a3, long a4, long a5, long a6, long a7);
DLLIMPORT void _DK_draw_status_sprites(long a1, long a2, struct Thing *thing, long a4);
DLLIMPORT long _DK_element_top_face_texture(struct Map *map);
DLLIMPORT long _DK_thing_is_spellbook(struct Thing *thing);
DLLIMPORT long _DK_object_is_gold(struct Thing *thing);
DLLIMPORT void _DK_check_players_won(void);
DLLIMPORT void _DK_check_players_lost(void);
DLLIMPORT void _DK_process_dungeon_power_magic(void);
DLLIMPORT void _DK_process_dungeon_devastation_effects(void);
DLLIMPORT void _DK_process_entrance_generation(void);
DLLIMPORT void _DK_process_things_in_dungeon_hand(void);
DLLIMPORT void _DK_process_payday(void);
DLLIMPORT void _DK_remove_thing_from_mapwho(struct Thing *thing);
DLLIMPORT void _DK_place_thing_in_mapwho(struct Thing *thing);
DLLIMPORT long _DK_get_thing_height_at(struct Thing *thing, struct Coord3d *pos);
DLLIMPORT struct Room *_DK_player_has_room_of_type(long plr_idx, long roomkind);
DLLIMPORT struct Room *_DK_find_room_with_spare_room_item_capacity(unsigned char a1, signed char a2);
DLLIMPORT long _DK_create_workshop_object_in_workshop_room(long a1, long a2, long a3);
DLLIMPORT long _DK_get_next_manufacture(struct Dungeon *dungeon);
DLLIMPORT long _DK_cleanup_current_thing_state(struct Thing *thing);
DLLIMPORT unsigned long _DK_setup_move_off_lava(struct Thing *thing);
DLLIMPORT struct Thing *_DK_create_thing(struct Coord3d *pos, unsigned short a1, unsigned short a2, unsigned short a3, long a4);
DLLIMPORT void _DK_move_thing_in_map(struct Thing *thing, struct Coord3d *pos);
DLLIMPORT long _DK_get_floor_height_under_thing_at(struct Thing *thing, struct Coord3d *pos);
DLLIMPORT void _DK_process_spells_affected_by_effect_elements(struct Thing *thing);
DLLIMPORT long _DK_load_texture_map_file(unsigned long lv_num, unsigned char n);
DLLIMPORT long _DK_get_top_cube_at_pos(long mpos);
DLLIMPORT void _DK_apply_damage_to_thing_and_display_health(struct Thing *thing, long a1, char a2);
DLLIMPORT long _DK_get_foot_creature_has_down(struct Thing *thing);
DLLIMPORT void _DK_process_disease(struct Thing *thing);
DLLIMPORT void _DK_set_creature_graphic(struct Thing *thing);
DLLIMPORT void _DK_process_keeper_spell_effect(struct Thing *thing);
DLLIMPORT long _DK_creature_is_group_leader(struct Thing *thing);
DLLIMPORT void _DK_leader_find_positions_for_followers(struct Thing *thing);
DLLIMPORT long _DK_update_creature_levels(struct Thing *thing);
DLLIMPORT long _DK_process_creature_self_spell_casting(struct Thing *thing);
DLLIMPORT void _DK_process_thing_spell_effects(struct Thing *thing);
DLLIMPORT long _DK_update_object(struct Thing *thing);
DLLIMPORT long _DK_update_shot(struct Thing *thing);
DLLIMPORT long _DK_update_effect_element(struct Thing *thing);
DLLIMPORT long _DK_update_dead_creature(struct Thing *thing);
DLLIMPORT long _DK_update_creature(struct Thing *thing);
DLLIMPORT long _DK_update_effect(struct Thing *thing);
DLLIMPORT long _DK_process_effect_generator(struct Thing *thing);
DLLIMPORT long _DK_update_trap(struct Thing *thing);
DLLIMPORT long _DK_process_door(struct Thing *thing);
DLLIMPORT long _DK_light_is_light_allocated(long lgt_id);
DLLIMPORT void _DK_light_set_light_position(long lgt_id, struct Coord3d *pos);
DLLIMPORT void _DK_gui_set_button_flashing(long a1, long a2);
DLLIMPORT short _DK_send_creature_to_room(struct Thing *thing, struct Room *room);
DLLIMPORT struct Room *_DK_get_room_thing_is_on(struct Thing *thing);
DLLIMPORT short _DK_set_start_state(struct Thing *thing);
DLLIMPORT long _DK_load_stats_files(void);
DLLIMPORT void _DK_check_and_auto_fix_stats(void);
DLLIMPORT long _DK_update_dungeon_scores(void);
DLLIMPORT long _DK_update_dungeon_generation_speeds(void);
DLLIMPORT void _DK_calculate_dungeon_area_scores(void);
DLLIMPORT void _DK_setup_computer_players2(void);
DLLIMPORT long _DK_get_next_research_item(struct Dungeon *dungeon);
DLLIMPORT void _DK_clear_columns(void);
DLLIMPORT void _DK_delete_all_structures(void);
DLLIMPORT void _DK_clear_mapwho(void);
DLLIMPORT void _DK_light_initialise(void);
DLLIMPORT void _DK_clear_game(void);
DLLIMPORT void _DK_clear_game_for_save(void);
DLLIMPORT long _DK_update_cave_in(struct Thing *thing);
DLLIMPORT void _DK_update_thing_animation(struct Thing *thing);
DLLIMPORT long _DK_update_thing(struct Thing *thing);
DLLIMPORT long _DK_get_thing_checksum(struct Thing *thing);
DLLIMPORT long _DK_update_thing_sound(struct Thing *thing);
DLLIMPORT void _DK_update_power_sight_explored(struct PlayerInfo *player);
DLLIMPORT void _DK_init_messages(void);
DLLIMPORT void _DK_battle_initialise(void);
DLLIMPORT void _DK_event_initialise_all(void);
DLLIMPORT void _DK_add_thing_to_list(struct Thing *thing, struct StructureList *list);
DLLIMPORT struct Thing *_DK_allocate_free_thing_structure(unsigned char a1);
DLLIMPORT unsigned char _DK_i_can_allocate_free_thing_structure(unsigned char a1);
DLLIMPORT void _DK_message_add(char c);
DLLIMPORT void _DK_toggle_creature_tendencies(struct PlayerInfo *player, char val);
DLLIMPORT long _DK_event_move_player_towards_event(struct PlayerInfo *player, long var);
DLLIMPORT void _DK_turn_off_call_to_arms(long a);
DLLIMPORT long _DK_place_thing_in_power_hand(struct Thing *thing, long var);
DLLIMPORT short _DK_magic_use_power_obey(unsigned short plridx);
DLLIMPORT void _DK_set_player_state(struct PlayerInfo *player, unsigned char a1, long a2);
DLLIMPORT void _DK_event_delete_event(long plridx, long num);
DLLIMPORT long _DK_set_autopilot_type(long plridx, long aptype);
DLLIMPORT void _DK_set_player_mode(struct PlayerInfo *player, long val);
DLLIMPORT short _DK_dump_held_things_on_map(unsigned char a1, long a2, long a3, short a4);
DLLIMPORT void _DK_turn_off_sight_of_evil(long plridx);
DLLIMPORT void _DK_go_on_then_activate_the_event_box(long plridx, long val);
DLLIMPORT void _DK_directly_cast_spell_on_thing(unsigned char plridx, unsigned char a2, unsigned short a3, long a4);
DLLIMPORT void _DK_lose_level(struct PlayerInfo *player);
DLLIMPORT long _DK_magic_use_power_armageddon(unsigned char val);
DLLIMPORT long _DK_battle_move_player_towards_battle(struct PlayerInfo *player, long var);
DLLIMPORT void _DK_level_lost_go_first_person(long plridx);
DLLIMPORT void _DK_process_network_error(long);
DLLIMPORT void _DK_resync_game(void);
DLLIMPORT void __cdecl _DK_set_gamma(char, int);
DLLIMPORT void _DK_complete_level(struct PlayerInfo *player);
DLLIMPORT void _DK_free_swipe_graphic(void);
DLLIMPORT void _DK_draw_sound_stuff(void);
DLLIMPORT void _DK_draw_bonus_timer(void);
DLLIMPORT void _DK_draw_power_hand(void);
DLLIMPORT void _DK_update_explored_flags_for_power_sight(struct PlayerInfo *player);
DLLIMPORT void _DK_engine(struct Camera *cam);
DLLIMPORT void _DK_smooth_screen_area(unsigned char *a1, long a2, long a3, long a4, long a5, long a6);
DLLIMPORT void _DK_remove_explored_flags_for_power_sight(struct PlayerInfo *player);
DLLIMPORT void _DK_DrawBigSprite(long x, long y, struct BigSprite *bigspr, struct TbSprite *sprite);
DLLIMPORT void _DK_draw_gold_total(unsigned char a1, long a2, long a3, long a4);
DLLIMPORT void _DK_pannel_map_draw(long x, long y, long zoom);
DLLIMPORT void _DK_draw_overlay_things(long zoom);
DLLIMPORT void _DK_draw_overlay_compass(long a1, long a2);
DLLIMPORT unsigned char _DK_find_first_battle_of_mine(unsigned char idx);
DLLIMPORT void _DK_set_engine_view(struct PlayerInfo *player, long a2);
DLLIMPORT void _DK_startup_network_game(void);
DLLIMPORT void _DK_reinit_level_after_load(void);
DLLIMPORT void _DK_reinit_tagged_blocks_for_player(unsigned char idx);
DLLIMPORT void _DK_reset_gui_based_on_player_mode(void);
DLLIMPORT void _DK_init_animating_texture_maps(void);
DLLIMPORT void _DK_init_lookups(void);
DLLIMPORT long _DK_init_navigation(void);
DLLIMPORT int __cdecl _DK_load_settings(void);
DLLIMPORT void _DK_sound_reinit_after_load(void);
DLLIMPORT void _DK_restore_computer_player_after_load(void);
DLLIMPORT struct Thing *_DK_create_effect(struct Coord3d *pos, unsigned short a2, unsigned char a3);
DLLIMPORT void _DK_delete_thing_structure(struct Thing *thing, long a2);
DLLIMPORT void _DK_make_safe(struct PlayerInfo *player);
DLLIMPORT struct Thing *_DK_create_creature(struct Coord3d *pos, unsigned short a1, unsigned short a2);
DLLIMPORT void _DK_set_creature_level(struct Thing *thing, long nlvl);
DLLIMPORT void _DK_remove_events_thing_is_attached_to(struct Thing *thing);
DLLIMPORT unsigned long _DK_steal_hero(struct PlayerInfo *player, struct Coord3d *pos);
DLLIMPORT void _DK_creature_increase_level(struct Thing *thing);
DLLIMPORT void _DK_clear_slab_dig(long a1, long a2, char a3);
DLLIMPORT void _DK_magic_use_power_hold_audience(unsigned char idx);
DLLIMPORT void _DK_activate_dungeon_special(struct Thing *thing, struct PlayerInfo *player);
DLLIMPORT void _DK_resurrect_creature(struct Thing *thing, unsigned char a2, unsigned char a3, unsigned char a4);
DLLIMPORT void _DK_transfer_creature(struct Thing *tng1, struct Thing *tng2, unsigned char a3);
DLLIMPORT long _DK_thing_is_special(Thing *thing);
DLLIMPORT int __cdecl _DK_play_smacker_file(char *fname, int);
DLLIMPORT void __cdecl _DK_reset_eye_lenses(void);
DLLIMPORT void __cdecl _DK_reset_heap_manager(void);
DLLIMPORT void __cdecl _DK_reset_heap_memory(void);
DLLIMPORT int _DK_LoadMcgaData(void);
DLLIMPORT void _DK_initialise_eye_lenses(void);
DLLIMPORT void _DK_setup_eye_lens(long nlens);
DLLIMPORT void _DK_setup_heap_manager(void);
DLLIMPORT int _DK_setup_heap_memory(void);
DLLIMPORT long _DK_light_create_light(struct InitLight *ilght);
DLLIMPORT void _DK_light_set_light_never_cache(long idx);
DLLIMPORT void _DK_reset_player_mode(struct PlayerInfo *player, unsigned char a2);
DLLIMPORT void _DK_init_keeper_map_exploration(struct PlayerInfo *player);
DLLIMPORT void _DK_init_player_cameras(struct PlayerInfo *player);
DLLIMPORT void _DK_pannel_map_update(long x, long y, long w, long h);
DLLIMPORT void _DK_view_set_camera_y_inertia(struct Camera *cam, long a2, long a3);
DLLIMPORT void _DK_view_set_camera_x_inertia(struct Camera *cam, long a2, long a3);
DLLIMPORT void _DK_view_set_camera_rotation_inertia(struct Camera *cam, long a2, long a3);
DLLIMPORT void _DK_view_zoom_camera_in(struct Camera *cam, long a2, long a3);
DLLIMPORT void _DK_set_camera_zoom(struct Camera *cam, long val);
DLLIMPORT void _DK_view_zoom_camera_out(struct Camera *cam, long a2, long a3);
DLLIMPORT long _DK_get_camera_zoom(struct Camera *camera);
DLLIMPORT int __stdcall _DK_setup_game(void);
DLLIMPORT int __stdcall _DK_init_sound(void);
DLLIMPORT int __cdecl _DK_initial_setup(void);
DLLIMPORT long _DK_ceiling_set_info(long a1, long a2, long a3);
DLLIMPORT int __cdecl _DK_process_sound_heap(void);
DLLIMPORT void __cdecl _DK_startup_saved_packet_game(void);
#ifdef __cplusplus
}
#endif

short show_onscreen_msg_va(int nturns, const char *fmt_str, va_list arg)
{
  vsprintf(onscreen_msg_text, fmt_str, arg);
  LbSyncLog("Onscreen message: %s\n",onscreen_msg_text);
  onscreen_msg_turns = nturns;
  return 1;
}

short is_onscreen_msg_visible(void)
{
  return (onscreen_msg_turns > 0);
}

short show_onscreen_msg(int nturns, const char *fmt_str, ...)
{
    short result;
    va_list val;
    va_start(val, fmt_str);
    result=show_onscreen_msg_va(nturns, fmt_str, val);
    va_end(val);
    return result;
}

void reset_eye_lenses(void)
{
  if (eye_lens_memory != NULL)
  {
    LbMemoryFree(eye_lens_memory);
    eye_lens_memory = NULL;
  }
  if (eye_lens_spare_screen_memory != NULL)
  {
    LbMemoryFree(eye_lens_spare_screen_memory);
    eye_lens_spare_screen_memory = NULL;
  }
  set_flag_byte(&game.flags_cd,MFlg_EyeLensReady,false);
  game.numfield_1A = 0;
  game.numfield_1B = 0;
}

void view_set_camera_y_inertia(struct Camera *cam, long a2, long a3)
{
  _DK_view_set_camera_y_inertia(cam, a2, a3);
}

void view_set_camera_x_inertia(struct Camera *cam, long a2, long a3)
{
  _DK_view_set_camera_x_inertia(cam, a2, a3);
}

void view_set_camera_rotation_inertia(struct Camera *cam, long a2, long a3)
{
  _DK_view_set_camera_rotation_inertia(cam, a2, a3);
}

void view_zoom_camera_in(struct Camera *cam, long a2, long a3)
{
  _DK_view_zoom_camera_in(cam, a2, a3);
}

void set_camera_zoom(struct Camera *cam, long val)
{
  if (cam == NULL)
    return;
  _DK_set_camera_zoom(cam, val);
}

void view_zoom_camera_out(struct Camera *cam, long a2, long a3)
{
  _DK_view_zoom_camera_out(cam, a2, a3);
}

long get_camera_zoom(struct Camera *camera)
{
  return _DK_get_camera_zoom(camera);
}

void initialise_eye_lenses(void)
{
  static const char *func_name="initialise_eye_lenses";
  unsigned long screen_size;
#if (BFDEBUG_LEVEL > 7)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  if ((eye_lens_memory != NULL) || (eye_lens_spare_screen_memory != NULL))
  {
    //error(func_name, 17, "EyeLens Memory already allocated");
    reset_eye_lenses();
  }
  if ((features_enabled & Ft_EyeLens) == 0)
  {
    set_flag_byte(&game.flags_cd,MFlg_EyeLensReady,false);
    return;
  }

  //TODO: Hack for compatibility - eye lens supported only in 3 modes
  if ((lbDisplay.ScreenMode != Lb_SCREEN_MODE_320_200_8) &&
      (lbDisplay.ScreenMode != Lb_SCREEN_MODE_640_400_8) &&
      (lbDisplay.ScreenMode != Lb_SCREEN_MODE_640_480_8))
  {
    LbWarnLog("EyeLens not supported in current screen mode\n");
    set_flag_byte(&game.flags_cd,MFlg_EyeLensReady,false);
    return;
  }

  eye_lens_height = lbDisplay.GraphicsScreenHeight;
  eye_lens_width = lbDisplay.GraphicsScreenWidth;
  screen_size = eye_lens_width * eye_lens_height + 2;
  eye_lens_memory = (unsigned long *)LbMemoryAlloc(screen_size*sizeof(unsigned long));
  eye_lens_spare_screen_memory = (unsigned char *)LbMemoryAlloc(screen_size*sizeof(TbPixel));
  if ((eye_lens_memory == NULL) || (eye_lens_spare_screen_memory == NULL))
  {
    reset_eye_lenses();
    error(func_name, 141, "Cannot allocate EyeLens Memory");
    return;
  }
#if (BFDEBUG_LEVEL > 9)
    LbSyncLog("%s: Buffer dimensions (%d,%d)\n",func_name,eye_lens_width,eye_lens_height);
#endif
  set_flag_byte(&game.flags_cd,MFlg_EyeLensReady,true);
}

void flyeye_setup(long width, long height)
{
  _DK_flyeye_setup(width, height);
}

void setup_eye_lens(long nlens)
{
  static const char *func_name="setup_eye_lens";
  //_DK_setup_eye_lens(nlens);return;
  struct PlayerInfo *player;
  char *fname;

  if ((game.flags_cd & MFlg_EyeLensReady) == 0)
    return;
  //TODO: Temporary hack, until CMistFade is not rewritten
  if ((nlens >= 4) && (nlens <= 12))
  {
    _DK_setup_eye_lens(nlens);
    return;
  }
  player = &(game.players[my_player_number%PLAYERS_COUNT]);
  if ((game.numfield_1B >= 13) && (game.numfield_1B <= 14))
  {
      player->field_7 = 0;
      game.numfield_1A = 0;
  }
  if (nlens < 1)
  {
    game.numfield_1A = 0;
    game.numfield_1B = 0;
    return;
  }
  if (game.numfield_1A == nlens)
  {
    game.numfield_1B = nlens;
    return;
  }
  switch (nlens)
  {
  case 1:
  case 2:
      init_lens(eye_lens_memory, MyScreenWidth/pixel_size, MyScreenHeight/pixel_size,
              lbDisplay.GraphicsScreenWidth, nlens);
      break;
  case 3:
      flyeye_setup(MyScreenWidth/pixel_size, MyScreenHeight/pixel_size);
      break;
  case 4:
  case 5:
  case 6:
  case 7:
  case 8:
  case 9:
  case 10:
  case 11:
      fname = prepare_file_fmtpath(FGrp_StdData,"frac%02d.dat",nlens-4);
      LbFileLoadAt(fname, eye_lens_memory);
/*
      Mist->setup(eye_lens_memory, fade_tables, ghost);
      Mist->animset(0, 1024);
*/
      break;
  case 12:
      fname = prepare_file_fmtpath(FGrp_StdData,"frac%02d.dat",nlens-4);
      LbFileLoadAt(fname, eye_lens_memory);
/*
      Mist->setup(eye_lens_memory, &fade_tables[1024], ghost);
      Mist->animset(0, 1024);
*/
      break;
  case 13:
      player->field_4C9 = dog_palette;
      player->field_7 = dog_palette;
      break;
  case 14:
      player->field_4C9 = vampire_palette;
      player->field_7 = vampire_palette;
      break;
  default:
      error(func_name, 159, "Invalid lens effect");
      nlens = 0;
      break;
  }
  game.numfield_1B = nlens;
  game.numfield_1A = nlens;
}

void reinitialise_eye_lens(long nlens)
{
  static const char *func_name="reinitialise_eye_lens";
  initialise_eye_lenses();
  if ((game.flags_cd & MFlg_EyeLensReady) && (nlens>0))
  {
      game.numfield_1B = 0;
      setup_eye_lens(nlens);
  }
#if (BFDEBUG_LEVEL > 18)
    LbSyncLog("%s: Finished\n",func_name);
#endif
}

void draw_jonty_mapwho(struct JontySpr *jspr)
{
  _DK_draw_jonty_mapwho(jspr);
}

void draw_keepsprite_unscaled_in_buffer(unsigned short a1, short a2, unsigned char a3, unsigned char *a4)
{
  _DK_draw_keepsprite_unscaled_in_buffer(a1, a2, a3, a4);
}

void draw_engine_number(struct Number *num)
{
  _DK_draw_engine_number(num);
}

void draw_engine_room_flag_top(struct RoomFlag *rflg)
{
  _DK_draw_engine_room_flag_top(rflg);
}

long get_smaller_memory_amount(long amount)
{
  if (amount > 64)
    return 64;
  if (amount > 48)
    return 48;
  if (amount > 32)
    return 32;
  if (amount > 24)
    return 24;
  if (amount > 16)
    return 16;
  if (amount >  8)
    return  8;
  return 6;
}

void setup_heap_manager(void)
{
  static const char *func_name="setup_heap_manager";
#if (BFDEBUG_LEVEL > 8)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  _DK_setup_heap_manager();
}

/*
 * Allocates graphics heap.
 */
TbBool setup_heap_memory(void)
{
  static const char *func_name="setup_heap_memory";
  long i;
#if (BFDEBUG_LEVEL > 8)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  if (heap != NULL)
  {
    LbSyncLog("%s: Freeing old Graphics heap\n",func_name);
    LbMemoryFree(heap);
    heap = NULL;
  }
  i = mem_size;
  heap_size = get_best_sound_heap_size(i);
  while ( 1 )
  {
    heap = LbMemoryAlloc(heap_size);
    if (heap != NULL)
      break;
    i = get_smaller_memory_amount(i);
    if (i > 8)
    {
      heap_size = get_best_sound_heap_size(i);
    } else
    {
      if (heap_size < 524288)
      {
        error(func_name, 80, "Unable to allocate Graphic Heap");
        heap_size = 0;
        return false;
      }
      heap_size -= 16384;
    }
  }
  LbSyncLog("GraphicsHeap Size %d\n", heap_size);
  return true;
}

void reset_heap_manager(void)
{
  static const char *func_name="reset_heap_manager";
#if (BFDEBUG_LEVEL > 8)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  _DK_reset_heap_manager();
}

void reset_heap_memory(void)
{
  static const char *func_name="reset_heap_memory";
#if (BFDEBUG_LEVEL > 8)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  LbMemoryFree(heap);
  heap = NULL;
}

long light_create_light(struct InitLight *ilght)
{
  return _DK_light_create_light(ilght);
}

void light_set_light_never_cache(long idx)
{
  _DK_light_set_light_never_cache(idx);
}

long light_is_light_allocated(long lgt_id)
{
  return _DK_light_is_light_allocated(lgt_id);
}

void light_set_light_position(long lgt_id, struct Coord3d *pos)
{
  _DK_light_set_light_position(lgt_id, pos);
}

unsigned long scale_camera_zoom_to_screen(unsigned long zoom_lvl)
{
  // Note: I don't know if the zoom may be scaled for current resolution,
  // as there may be different resolution on another computer if playing MP game.
//  return ((zoom_lvl*units_per_pixel) >> 4)*pixel_size;
  return ((((zoom_lvl*units_per_pixel) >> 4)*(int)sqrt(pixel_size*units_per_pixel)) >> 2)*pixel_size;
}

void thing_play_sample(struct Thing *thing, short a2, unsigned short a3, char a4, unsigned char a5, unsigned char a6, long a7, long a8)
{
  struct Coord3d rcpos;
  long i;
  if (SoundDisabled)
    return;
  if (GetCurrentSoundMasterVolume() <= 0)
    return;
  rcpos.x.val = Receiver.pos_x;
  rcpos.y.val = Receiver.pos_y;
  rcpos.z.val = Receiver.pos_z;
  if (get_3d_box_distance(&rcpos, &thing->mappos) < MaxSoundDistance)
  {
    i = thing->field_66;
    if (i > 0)
    {
      S3DAddSampleToEmitterPri(i, a2, 0, a3, a8, a4, a5, a6 | 0x01, a7);
    } else
    {
      i = S3DCreateSoundEmitterPri(thing->mappos.x.val, thing->mappos.y.val, thing->mappos.z.val,
         a2, 0, a3, a8, a4, a6 | 0x01, a7);
     thing->field_66 = i;
    }
  }
}

void init_player_as_single_keeper(struct PlayerInfo *player)
{
  unsigned short idx;
  struct InitLight ilght;
  memset(&ilght, 0, sizeof(struct InitLight));
  player->field_4CD = 0;
  ilght.field_0 = 0x0A00;
  ilght.field_2 = 48;
  ilght.field_3 = 5;
  ilght.field_11 = 1;
  idx = light_create_light(&ilght);
  player->field_460 = idx;
  light_set_light_never_cache(idx);
}

TbPixel get_player_path_colour(unsigned short owner)
{
  return player_path_colours[player_colors_map[owner % PLAYERS_EXT_COUNT]];
}

void setup_3d(void)
{
  static const char *func_name="setup_3d";
  unsigned long seed;
  long i,k;
  seed = 0;
  for (i=0; i < 512; i++)
  {
    k = seed_check_random(127, &seed, func_name, 4791);
    randomisors[i] = k - 63;
  }
}

void setup_stuff(void)
{
  _DK_setup_stuff();
}

short send_creature_to_room(struct Thing *thing, struct Room *room)
{
  return _DK_send_creature_to_room(thing, room);
}

struct Room *get_room_thing_is_on(struct Thing *thing)
{
  return _DK_get_room_thing_is_on(thing);
}

short thing_touching_floor(struct Thing *thing)
{
  return (thing->field_60 == thing->mappos.z.val);
}

short set_start_state(struct Thing *thing)
{
  return _DK_set_start_state(thing);
}

void process_person_moods_and_needs(struct Thing *thing)
{
  _DK_process_person_moods_and_needs(thing);
}

unsigned long object_is_pickable_by_hand(struct Thing *thing, long a2)
{
  return _DK_object_is_pickable_by_hand(thing, a2);
}

short thing_is_pickable_by_hand(struct PlayerInfo *player,struct Thing *thing)
{
  if (thing_is_invalid(thing))
    return false;
  if (((thing->field_0 & 0x01) == 0) || (thing->field_9 != player->field_440))
    return false;
  // All creatures can be picked
  if (thing->class_id == TCls_Creature)
    return true;
  // Some objects can be picked
  if ((thing->class_id == TCls_Object) && object_is_pickable_by_hand(thing, player->field_2B))
    return true;
  // Other things are not pickable
  return false;
}

void set_power_hand_offset(struct PlayerInfo *player, struct Thing *thing)
{
  _DK_set_power_hand_offset(player, thing);
}

struct Thing *process_object_being_picked_up(struct Thing *thing, long plyr_idx)
{
  static const char *func_name="process_object_being_picked_up";
  struct PlayerInfo *player;
  struct Thing *picktng;
  struct Thing *tmptng;
  struct Coord3d pos;
  long i;
  switch (thing->model)
  {
    case 3:
    case 6:
    case 43:
      i = thing->long_13;
      if (i != 0)
      {
        pos.x.val = thing->mappos.x.val;
        pos.y.val = thing->mappos.y.val;
        pos.z.val = thing->mappos.z.val + 128;
        tmptng = create_effect_element(&pos, 41, thing->owner);
        if (tmptng != NULL)
          tmptng->long_13 = i;
      }
      picktng = thing;
      break;
    case 10:
      i = seed_check_random(3, &game.rand_14BB4E, func_name, 4534);
      thing_play_sample(thing, 109+i, 100, 0, 3, 0, 2, 256);
      i = convert_td_iso(122);
      set_thing_draw(thing, i, 256, -1, -1, 0, 2);
      remove_food_from_food_room_if_possible(thing);
      picktng = thing;
      break;
    case 52:
    case 53:
    case 54:
    case 55:
    case 56:
      picktng = create_gold_for_hand_grab(thing, plyr_idx);
      break;
    case 86:
    case 87:
    case 88:
    case 89:
    case 90:
    case 91:
    case 92:
    case 93:
      player = &(game.players[plyr_idx%PLAYERS_COUNT]);
      activate_dungeon_special(thing, player);
      picktng = NULL;
      break;
    default:
      error(func_name, 1020, "Picking up invalid object");
      picktng = NULL;
      break;
  }
  return picktng;
}

void set_power_hand_graphic(long plyr_idx, long a2, long a3)
{
  struct PlayerInfo *player;
  struct Thing *thing;
  player = &(game.players[plyr_idx%PLAYERS_COUNT]);
  if (player->field_10 >= game.play_gameturn)
  {
    if ((a2 == 786) || (a2 == 787))
      player->field_10 = 0;
  }
  if (player->field_10 < game.play_gameturn)
  {
    if (player->field_C != a2)
    {
      player->field_C = a2;
      thing = thing_get(player->field_43A);
      if ((a2 == 782) || (a2 == 781))
      {
        set_thing_draw(thing, a2, a3, 300, 0, 0, 2);
      } else
      {
        set_thing_draw(thing, a2, a3, 300, 1, 0, 2);
      }
      thing = get_first_thing_in_power_hand(player);
      set_power_hand_offset(player,thing);
    }
  }
}

TbBool power_hand_is_empty(struct PlayerInfo *player)
{
  struct Dungeon *dungeon;
  dungeon = &(game.dungeon[player->field_2B%DUNGEONS_COUNT]);
  return (dungeon->things_in_hand[0] == 0);
}

struct Thing *get_first_thing_in_power_hand(struct PlayerInfo *player)
{
  struct Dungeon *dungeon;
  dungeon = &(game.dungeon[player->field_2B%DUNGEONS_COUNT]);
  return thing_get(dungeon->things_in_hand[0]);
}

long dump_thing_in_power_hand(struct Thing *thing, long a2)
{
  return _DK_dump_thing_in_power_hand(thing, a2);
}

void place_thing_in_limbo(struct Thing *thing)
{
  remove_thing_from_mapwho(thing);
  thing->field_4F |= 0x01;
  thing->field_0 |= 0x10;
}

struct Thing *create_gold_for_hand_grab(struct Thing *thing, long a2)
{
  return _DK_create_gold_for_hand_grab(thing, a2);
}

long remove_food_from_food_room_if_possible(struct Thing *thing)
{
  return _DK_remove_food_from_food_room_if_possible(thing);
}

struct Thing *create_effect_element(struct Coord3d *pos, unsigned short a2, unsigned short a3)
{
  return _DK_create_effect_element(pos, a2, a3);
}

unsigned char can_change_from_state_to(struct Thing *thing, long a2, long a3)
{
  return _DK_can_change_from_state_to(thing, a2, a3);
}

struct Thing *get_door_for_position(long pos_x, long pos_y)
{
  return _DK_get_door_for_position(pos_x, pos_y);
}

struct Thing *get_trap_for_position(long pos_x, long pos_y)
{
  return _DK_get_trap_for_position(pos_x, pos_y);
}

struct Thing *get_trap_for_slab_position(MapSlabCoord slb_x, MapSlabCoord slb_y)
{
  return _DK_get_trap_for_slab_position(slb_x, slb_y);
}

unsigned char find_door_of_type(unsigned long a1, unsigned char a2)
{
  return _DK_find_door_of_type(a1, a2);
}

void process_armageddon(void)
{
  static const char *func_name="process_armageddon";
  if (game.field_150356 == 0)
    return;
#if (BFDEBUG_LEVEL > 6)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  _DK_process_armageddon();
}

long process_obey_leader(struct Thing *thing)
{
  return _DK_process_obey_leader(thing);
}

struct Thing *get_group_leader(struct Thing *thing)
{
  struct CreatureControl *cctrl;
  long i;
  cctrl = creature_control_get_from_thing(thing);
  i = cctrl->field_7A;// & 0xFFF;
  if ((i > 0) && (i < THINGS_COUNT))
    return game.things_lookup[i];
  return NULL;
}

TbBool player_is_friendly_or_defeated(int plyr_idx, int win_plyr_idx)
{
  struct PlayerInfo *player;
  struct PlayerInfo *win_player;
  struct Dungeon *dungeon;
  player = &(game.players[plyr_idx%PLAYERS_COUNT]);
  win_player = &(game.players[win_plyr_idx%PLAYERS_COUNT]);
  if ((player->field_0 & 0x01) != 0)
  {
      if ((win_plyr_idx == game.field_14E497) || (plyr_idx == game.field_14E497)     || ((win_player->field_2A & (1<<plyr_idx)) == 0)
       || (game.field_14E497 == plyr_idx)     || (win_plyr_idx == game.field_14E497) || ((player->field_2A & (1<<win_plyr_idx)) == 0))
      {
        dungeon = &(game.dungeon[plyr_idx%DUNGEONS_COUNT]);
        if (dungeon->field_0 > 0)
          return false;
      }
  }
  return true;
}

TbBool all_dungeons_destroyed(struct PlayerInfo *win_player)
{
  static const char *func_name="all_dungeons_destroyed";
  long win_plyr_idx;
  long i,k;
  win_plyr_idx = win_player->field_2B;
  for (i=0; i < PLAYERS_COUNT; i++)
  {
    if (i == win_plyr_idx)
      continue;
    if (!player_is_friendly_or_defeated(i,win_plyr_idx))
      return false;
  }
#if (BFDEBUG_LEVEL > 1)
    LbSyncLog("%s: Returning true for player %d\n",func_name,win_plyr_idx);
#endif
  return true;
}

short make_group_member_leader(struct Thing *leadtng)
{
  struct Thing *prvtng;
  prvtng = get_group_leader(leadtng);
  if (thing_is_invalid(prvtng))
    return false;
  if (prvtng != leadtng)
  {
    remove_creature_from_group(leadtng);
    add_creature_to_group_as_leader(leadtng, prvtng);
    return true;
  }
  return false;
}

long find_from_task_list(long plyr_idx, long srch_tsk)
{
  struct Dungeon *dungeon;
  struct MapTask *task;
  long i;
  dungeon = &(game.dungeon[plyr_idx%DUNGEONS_COUNT]);
  for (i=0; i < dungeon->field_AF7; i++)
  {
    task = &dungeon->task_list[i%MAPTASKS_COUNT];
    if (task->field_1 == srch_tsk)
      return i;
  }
  return -1;
}

void reset_player_mode(struct PlayerInfo *player, unsigned char a2)
{
  _DK_reset_player_mode(player, a2);
}

void init_keeper_map_exploration(struct PlayerInfo *player)
{
  _DK_init_keeper_map_exploration(player);
}

void init_player_cameras(struct PlayerInfo *player)
{
  _DK_init_player_cameras(player);
}

void init_dungeons_research(void)
{
  struct Dungeon *dungeon;
  int i;
  for (i=0; i < DUNGEONS_COUNT; i++)
  {
    dungeon = (&game.dungeon[i]);
    dungeon->field_F78 = get_next_research_item(dungeon);
  }
}

TbBool remove_all_research_from_player(long plyr_idx)
{
  struct Dungeon *dungeon;
  dungeon = &(game.dungeon[plyr_idx%DUNGEONS_COUNT]);
  dungeon->research_num = 0;
  dungeon->research_override = 1;
  return true;
}

TbBool research_overriden_for_player(long plyr_idx)
{
  struct Dungeon *dungeon;
  dungeon = &(game.dungeon[plyr_idx%DUNGEONS_COUNT]);
  return (dungeon->research_override != 0);
}

TbBool clear_research_for_all_players(void)
{
  struct Dungeon *dungeon;
  int plyr_idx;
  for (plyr_idx=0; plyr_idx < DUNGEONS_COUNT; plyr_idx++)
  {
    dungeon = &(game.dungeon[plyr_idx%DUNGEONS_COUNT]);
    dungeon->research_num = 0;
    dungeon->research_override = 0;
  }
  return true;
}

TbBool add_research_to_player(long plyr_idx, long rtyp, long rkind, long amount)
{
  static const char *func_name="add_research_to_player";
  struct Dungeon *dungeon;
  struct ResearchVal *resrch;
  long i;
  dungeon = &(game.dungeon[plyr_idx%DUNGEONS_COUNT]);
  i = dungeon->research_num;
  if (i >= DUNGEON_RESEARCH_COUNT)
  {
    LbErrorLog("%s: Too much research (%d items) for player %d\n", i, plyr_idx);
    return false;
  }
  resrch = &dungeon->research[i];
  resrch->rtyp = rtyp;
  resrch->rkind = rkind;
  resrch->req_amount = amount;
  dungeon->research_num++;
  return true;
}

TbBool add_research_to_all_players(long rtyp, long rkind, long amount)
{
  static const char *func_name="add_research_to_all_players";
  TbBool result;
  long i;
  result = true;
#if (BFDEBUG_LEVEL > 17)
  LbSyncLog("%s: Adding type %d, kind %d, amount %d\n", func_name, rtyp, rkind, amount);
#endif
  for (i=0; i < PLAYERS_COUNT; i++)
  {
    result &= add_research_to_player(i, rtyp, rkind, amount);
  }
  return result;
}

TbBool update_players_research_amount(long plyr_idx, long rtyp, long rkind, long amount)
{
  static const char *func_name="update_players_research_amount";
  struct Dungeon *dungeon;
  struct ResearchVal *resrch;
  long i;
  dungeon = &(game.dungeon[plyr_idx%DUNGEONS_COUNT]);
  for (i = 0; i < dungeon->research_num; i++)
  {
    resrch = &dungeon->research[i];
    if ((resrch->rtyp == rtyp) && (resrch->rkind = rkind))
    {
      resrch->req_amount = amount;
    }
    return true;
  }
  return false;
}

TbBool update_or_add_players_research_amount(long plyr_idx, long rtyp, long rkind, long amount)
{
  if (update_players_research_amount(plyr_idx, rtyp, rkind, amount))
    return true;
  return add_research_to_player(plyr_idx, rtyp, rkind, amount);
}

void init_spiral_steps(void)
{
  static const char *func_name="init_spiral_steps";
  struct MapOffset *sstep;
  long x,y;
  long i;
  y = 0;
  x = 0;
    sstep = &spiral_step[0];
    sstep->h = y;
    sstep->v = x;
    sstep->both = (short)y + ((short)x << 8);
  y = -1;
  x = -1;
  for (i=1; i < SPIRAL_STEPS_COUNT; i++)
  {
    sstep = &spiral_step[i];
    sstep->h = y;
    sstep->v = x;
    sstep->both = (short)y + ((short)x << 8);
    if ((y < 0) && (x-y == 1))
    {
      y--;
      x -= 2;
    } else
    if (x == y)
    {
      if (y < 0)
        y++;
      else
        y--;
    } else
    if (y+x == 0)
    {
      if (x >= 0)
        x--;
      else
        x++;
    } else
    if (abs(x) >= abs(y))
    {
      if (x < 0)
        y++;
      else
        y--;
    } else
    {
      if (y >= 0)
        x++;
      else
        x--;
    }
  }
}

void init_creature_scores(void)
{
  static const char *func_name="init_creature_scores";
  struct CreatureStats *crstat;
  long i,k;
  long score,max_score;
  max_score = 0;
  for (i=0; i < CREATURE_TYPES_COUNT; i++)
  {
    crstat = creature_stats_get(i);
    score = compute_creature_max_health(crstat->health,CREATURE_MAX_LEVEL-1)
        + compute_creature_max_defence(crstat->defence,CREATURE_MAX_LEVEL-1)
        + compute_creature_max_dexterity(crstat->dexterity,CREATURE_MAX_LEVEL-1)
        + compute_creature_max_armour(crstat->armour,CREATURE_MAX_LEVEL-1)
        + compute_creature_max_strength(crstat->strength,CREATURE_MAX_LEVEL-1);
    if ((score <= 0) && (i != 0) && (i != CREATURE_TYPES_COUNT-1))
    {
      LbErrorLog("Couldn't get creature %d score value\n", i);
      continue;
    }
    if (score > max_score)
    {
      max_score = score;
    }
  }
  if (max_score <= 0)
  {
    error(func_name, 4179, "Creatures have no score");
    return;
  }
  for (i=0; i < CREATURE_TYPES_COUNT; i++)
  {
    crstat = creature_stats_get(i);
    for (k=0; k < CREATURE_MAX_LEVEL; k++)
    {
      score = compute_creature_max_health(crstat->health,k)
          + compute_creature_max_defence(crstat->defence,k)
          + compute_creature_max_dexterity(crstat->dexterity,k)
          + compute_creature_max_armour(crstat->armour,k)
          + compute_creature_max_strength(crstat->strength,k);
      score = saturate_set_unsigned(200*score / max_score, 8);
      if ((score <= 0) && (i != 0) && (i != 31))
      {
        //LbWarnLog("Couldn't get creature %d score for lev %d\n", i, k);
        score = 1;
      }
      game.creature_scores[i].value[k] = score;
    }
  }
}

void init_creature_state(struct Thing *thing)
{
  struct Room *room;
  if (thing->owner != game.field_14E497)
  {
    room = get_room_thing_is_on(thing);
    if (room != NULL)
    {
        switch (room->kind)
        {
        case 4:
        case 5:
        case 16:
            if ( send_creature_to_room(thing, room) )
              return;
        default:
            break;
        }
    }
  }
  set_start_state(thing);
}

void clear_creature_pool(void)
{
  memset(&game.pool,0,sizeof(struct CreaturePool));
  game.pool.is_empty = true;
}

void clear_slab_dig(long a1, long a2, char a3)
{
  _DK_clear_slab_dig(a1, a2, a3);
}

long modem_initialise_callback(void)
{
  LbWindowsControl();
  if (is_key_pressed(KC_ESCAPE, KM_DONTCARE))
  {
    clear_key_pressed(KC_ESCAPE);
    return -7;
  }
  if (LbScreenLock() == 1)
  {
    draw_text_box(gui_strings[531]); // Initialising Modem
    LbScreenUnlock();
  }
  LbScreenSwap();
  return 0;
}

long modem_connect_callback(void)
{
  LbWindowsControl();
  if (is_key_pressed(KC_ESCAPE, KM_DONTCARE))
  {
    clear_key_pressed(KC_ESCAPE);
    return -7;
  }
  if (LbScreenLock() == 1)
  {
    draw_text_box(gui_strings[532]); // Connecting Modem
    LbScreenUnlock();
  }
  LbScreenSwap();
  return 0;
}

void ProperFadePalette(unsigned char *pal, long n, enum TbPaletteFadeFlag flg)
{
    if ( lbUseSdk )
    {
        TbClockMSec last_loop_time;
        last_loop_time = LbTimerClock();
        while (LbPaletteFade(pal, n, Lb_PALETTE_FADE_OPEN) < n)
        {
          if (!is_key_pressed(KC_SPACE,KM_DONTCARE) &&
              !is_key_pressed(KC_ESCAPE,KM_DONTCARE) &&
              !is_key_pressed(KC_RETURN,KM_DONTCARE) &&
              !is_mouse_pressed_lrbutton())
          {
            last_loop_time += 25;
            LbSleepUntil(last_loop_time);
          }
        }
    } else
    if ( pal != NULL )
    {
        LbPaletteSet(pal);
    } else
    {
        memset(palette_buf, 0, sizeof(palette_buf));
        LbPaletteSet(palette_buf);
    }
}

void ProperForcedFadePalette(unsigned char *pal, long n, enum TbPaletteFadeFlag flg)
{
    if (flg == Lb_PALETTE_FADE_OPEN)
    {
        LbPaletteFade(pal, n, flg);
        return;
    }
    if ( lbUseSdk )
    {
        TbClockMSec last_loop_time;
        last_loop_time = LbTimerClock();
        while (LbPaletteFade(pal, n, Lb_PALETTE_FADE_OPEN) < n)
        {
          last_loop_time += 25;
          LbSleepUntil(last_loop_time);
        }
    } else
    if (pal != NULL)
    {
        LbPaletteSet(pal);
    } else
    {
        memset(palette_buf, 0, sizeof(palette_buf));
        LbPaletteSet(palette_buf);
    }
}

void move_thing_in_map(struct Thing *thing, struct Coord3d *pos)
{
  static const char *func_name="move_thing_in_map";
#if (BFDEBUG_LEVEL > 18)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  if ((thing->mappos.x.stl.num == pos->x.stl.num) && (thing->mappos.y.stl.num == pos->y.stl.num))
  {
    thing->mappos.x.val = pos->x.val;
    thing->mappos.y.val = pos->y.val;
    thing->mappos.z.val = pos->z.val;
  } else
  {
    remove_thing_from_mapwho(thing);
    thing->mappos.x.val = pos->x.val;
    thing->mappos.y.val = pos->y.val;
    thing->mappos.z.val = pos->z.val;
    place_thing_in_mapwho(thing);
  }
  thing->field_60 = get_thing_height_at(thing, &thing->mappos);
}

long get_floor_height_under_thing_at(struct Thing *thing, struct Coord3d *pos)
{
  return _DK_get_floor_height_under_thing_at(thing, pos);
}

void process_spells_affected_by_effect_elements(struct Thing *thing)
{
  _DK_process_spells_affected_by_effect_elements(thing);
}

long get_top_cube_at_pos(long mpos)
{
  return _DK_get_top_cube_at_pos(mpos);
}

void apply_damage_to_thing_and_display_health(struct Thing *thing, long a1, char a2)
{
  _DK_apply_damage_to_thing_and_display_health(thing, a1, a2);
}

long get_foot_creature_has_down(struct Thing *thing)
{
  return _DK_get_foot_creature_has_down(thing);
}

void process_disease(struct Thing *thing)
{
  static const char *func_name="process_disease";
#if (BFDEBUG_LEVEL > 18)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  _DK_process_disease(thing);
}

void set_creature_graphic(struct Thing *thing)
{
  _DK_set_creature_graphic(thing);
}

void process_keeper_spell_effect(struct Thing *thing)
{
  _DK_process_keeper_spell_effect(thing);
}

void add_spell_to_player(long spl_idx, long plyr_idx)
{
  struct Dungeon *dungeon;
  long i;
  if ((spl_idx < 0) || (spl_idx >= KEEPER_SPELLS_COUNT))
    return;
  dungeon = &(game.dungeon[plyr_idx%DUNGEONS_COUNT]);
  i = dungeon->magic_level[spl_idx];
  if (i < 255)
  {
    dungeon->magic_level[spl_idx] = i+1;
    dungeon->magic_resrchable[spl_idx] = 1;
  }
}

unsigned char sight_of_evil_expand_check(void)
{
  struct PlayerInfo *player;
  struct Dungeon *dungeon;
  player = &(game.players[my_player_number%PLAYERS_COUNT]);
  dungeon = &(game.dungeon[player->field_2B%DUNGEONS_COUNT]);
  return (player->field_4D2 != 0) && (dungeon->field_5D8 == 0);
}

unsigned char call_to_arms_expand_check(void)
{
  struct PlayerInfo *player;
  struct Dungeon *dungeon;
  player = &(game.players[my_player_number%PLAYERS_COUNT]);
  dungeon = &(game.dungeon[player->field_2B%DUNGEONS_COUNT]);
  return (player->field_4D2 != 0) && (dungeon->field_884 == 0);
}

unsigned char general_expand_check(void)
{
  struct PlayerInfo *player;
  player = &(game.players[my_player_number%PLAYERS_COUNT]);
  return (player->field_4D2 != 0);
}

long creature_is_group_leader(struct Thing *thing)
{
  return _DK_creature_is_group_leader(thing);
}

void leader_find_positions_for_followers(struct Thing *thing)
{
  _DK_leader_find_positions_for_followers(thing);
}

unsigned char external_set_thing_state(struct Thing *thing, long state)
{
  return _DK_external_set_thing_state(thing, state);
}

long is_thing_passenger_controlled(struct Thing *thing)
{
  return _DK_is_thing_passenger_controlled(thing);
}

long instf_creature_fire_shot(struct Thing *thing, long *param)
{
  struct CreatureControl *cctrl;
  struct Thing *target;
  int i;
  cctrl = creature_control_get_from_thing(thing);
  if (cctrl->field_DA <= 0)
  {
    if ((thing->field_0 & 0x20) == 0)
      i = 4;
    else
      i = 1;
  } else
  if ((thing->field_0 & 0x20) != 0)
  {
    target = thing_get(cctrl->field_DA);
    if (target->class_id == 1)
      i = 1;
    else
      i = 2;
  } else
  {
    target = thing_get(cctrl->field_DA);
    if (target->class_id == 1)
      i = 1;
    else
    if (target->owner == thing->owner)
      i = 2;
    else
      i = 4;
  }
  if (cctrl->field_DA > 0)
    target = thing_get(cctrl->field_DA);
  else
    target = NULL;
  creature_fire_shot(thing, target, *param, 1, i);
  return 0;
}

/*
 * Computes max health of a creature on given level.
 */
long compute_creature_max_health(long base_health,unsigned short crlevel)
{
  long max_health;
  if (base_health <= 0)
    return 0;
  if (base_health > 100000)
    base_health = 100000;
  if (crlevel >= CREATURE_MAX_LEVEL)
    crlevel = CREATURE_MAX_LEVEL-1;
  max_health = base_health + (35*base_health*crlevel)/100;
  return saturate_set_signed(max_health, 16);
}

/*
 * Computes amount of gold a creature takes on payday.
 */
long compute_creature_max_pay(long base_pay,unsigned short crlevel)
{
  long max_pay;
  if (base_pay <= 0)
    return 0;
  if (base_pay > 100000)
    base_pay = 100000;
  if (crlevel >= CREATURE_MAX_LEVEL)
    crlevel = CREATURE_MAX_LEVEL-1;
  max_pay = base_pay + (35*base_pay*crlevel)/100;
  return saturate_set_signed(max_pay, 16);
}

/*
 * Computes 8-bit parameter (defence,dexterity) of a creature on given level.
 */
long compute_creature_max_parameter(long base_param,unsigned short crlevel)
{
  long max_param;
  if (base_param <= 0)
    return 0;
  if (base_param > 10000)
    base_param = 10000;
  if (crlevel >= CREATURE_MAX_LEVEL)
    crlevel = CREATURE_MAX_LEVEL-1;
  max_param = base_param + (35*base_param*crlevel)/100;
  return saturate_set_unsigned(max_param, 8);
}

/*
 * Computes strength of a creature on given level.
 */
long compute_creature_max_strength(long base_param,unsigned short crlevel)
{
  long max_param;
  if ((base_param <= 0) || (base_param > 60000))
    base_param = 60000;
  if (crlevel >= CREATURE_MAX_LEVEL)
    crlevel = CREATURE_MAX_LEVEL-1;
  max_param = base_param + (35*base_param*crlevel)/100;
  return saturate_set_unsigned(max_param, 15);
}

/*
 * Computes parameter (luck,armour) of a creature on given level,
 * where the level doesn't really matters.
 */
long compute_creature_max_unaffected(long base_param,unsigned short crlevel)
{
  if (base_param <= 0)
    return 0;
  if (base_param > 10000)
    base_param = 10000;
  return saturate_set_unsigned(base_param, 8);
}

short update_creature_health_to_max(struct Thing *thing)
{
  struct CreatureStats *crstat;
  struct CreatureControl *cctrl;
  crstat = creature_stats_get_from_thing(thing);
  cctrl = creature_control_get_from_thing(thing);
  thing->health = compute_creature_max_health(crstat->health,cctrl->explevel);
  return true;
}

long update_creature_levels(struct Thing *thing)
{
  static const char *func_name="update_creature_levels";
#if (BFDEBUG_LEVEL > 18)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  struct CreatureStats *crstat;
  struct PlayerInfo *player;
  struct Dungeon *dungeon;
  struct CreatureControl *cctrl;
  struct Thing *newtng;
  cctrl = creature_control_get_from_thing(thing);
  if ((cctrl->field_AD & 0x40) == 0)
    return 0;
  cctrl->field_AD &= 0xBF;
  if (game.field_14E497 != thing->owner)
  {
    dungeon = &(game.dungeon[thing->owner%DUNGEONS_COUNT]);
    dungeon->field_EA8 -= game.creature_scores[thing->model%CREATURE_TYPES_COUNT].value[cctrl->explevel%CREATURE_MAX_LEVEL];
  }
  // If a creature is not on highest level, just update the level
  if (cctrl->explevel+1 < CREATURE_MAX_LEVEL)
  {
    set_creature_level(thing, cctrl->explevel+1);
    return 1;
  }
  // If it is highest level, maybe we should transform the creature?
  crstat = creature_stats_get_from_thing(thing);
  if (crstat->grow_up == 0)
    return 0;
  // Transforming
  newtng = create_creature(&thing->mappos, crstat->grow_up, thing->owner);
  if (newtng == NULL)
  {
    error(func_name, 1549, "Could not create creature to transform to");
    return 0;
  }
  set_creature_level(newtng, crstat->grow_up_level-1);
  update_creature_health_to_max(newtng);
  cctrl = creature_control_get_from_thing(thing);
  cctrl->field_282 = 50;
  external_set_thing_state(newtng, 127);
  player = &(game.players[thing->owner%PLAYERS_COUNT]);
  // Switch control if this creature is possessed
  if (is_thing_passenger_controlled(thing))
  {
    leave_creature_as_controller(player, thing);
    control_creature_as_controller(player, newtng);
  }
  if (thing->field_1B == player->field_2F)
  {
    player->field_2F = newtng->field_1B;
    player->field_31 = newtng->field_9;
  }
  kill_creature(thing, game.things_lookup[0], -1, 1, 0, 1);
  return -1;
}

long process_creature_self_spell_casting(struct Thing *thing)
{
  return _DK_process_creature_self_spell_casting(thing);
}

void process_thing_spell_effects(struct Thing *thing)
{
  _DK_process_thing_spell_effects(thing);
}

short thing_is_special(struct Thing *thing)
{
  return (thing_to_special(thing) > 0);
}

long update_object(struct Thing *thing)
{
  static const char *func_name="update_object";
#if (BFDEBUG_LEVEL > 18)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  return _DK_update_object(thing);
}

long update_shot(struct Thing *thing)
{
  static const char *func_name="update_shot";
#if (BFDEBUG_LEVEL > 18)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  return _DK_update_shot(thing);
}

long update_effect_element(struct Thing *thing)
{
  static const char *func_name="update_effect_element";
#if (BFDEBUG_LEVEL > 18)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  return _DK_update_effect_element(thing);
}

long update_dead_creature(struct Thing *thing)
{
  static const char *func_name="update_dead_creature";
#if (BFDEBUG_LEVEL > 18)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  return _DK_update_dead_creature(thing);
}

short update_creature_movements(struct Thing *thing)
{
  static const char *func_name="update_creature_movements";
  struct CreatureControl *cctrl;
  short upd_done;
  int i;
#if (BFDEBUG_LEVEL > 18)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  cctrl = creature_control_get_from_thing(thing);
  if (creature_control_invalid(cctrl))
    return false;
  upd_done = 0;
  if (cctrl->field_AB != 0)
  {
    upd_done = 1;
    cctrl->pos_BB.x.val = 0;
    cctrl->pos_BB.y.val = 0;
    cctrl->pos_BB.z.val = 0;
    cctrl->field_C8 = 0;
    set_flag_byte(&cctrl->field_2,0x01,false);
  } else
  {
    if ( thing->field_0 & 0x20 )
    {
      if ( thing->field_25 & 0x20 )
      {
        if (cctrl->field_C8 != 0)
        {
          cctrl->pos_BB.x.val = (LbSinL(thing->field_52)>> 8)
                * (cctrl->field_C8 * LbCosL(thing->field_54) >> 8) >> 16;
          cctrl->pos_BB.y.val = -((LbCosL(thing->field_52) >> 8)
                * (cctrl->field_C8 * LbCosL(thing->field_54) >> 8) >> 8) >> 8;
          cctrl->pos_BB.z.val = cctrl->field_C8 * LbSinL(thing->field_54) >> 16;
        }
        if (cctrl->field_CA != 0)
        {
          cctrl->pos_BB.x.val +=   cctrl->field_CA * LbSinL(thing->field_52 - 512) >> 16;
          cctrl->pos_BB.y.val += -(cctrl->field_CA * LbCosL(thing->field_52 - 512) >> 8) >> 8;
        }
      } else
      {
        if (cctrl->field_C8 != 0)
        {
          upd_done = 1;
          cctrl->pos_BB.x.val =   cctrl->field_C8 * LbSinL(thing->field_52) >> 16;
          cctrl->pos_BB.y.val = -(cctrl->field_C8 * LbCosL(thing->field_52) >> 8) >> 8;
        }
        if (cctrl->field_CA != 0)
        {
          upd_done = 1;
          cctrl->pos_BB.x.val +=   cctrl->field_CA * LbSinL(thing->field_52 - 512) >> 16;
          cctrl->pos_BB.y.val += -(cctrl->field_CA * LbCosL(thing->field_52 - 512) >> 8) >> 8;
        }
      }
    } else
    if (cctrl->field_2 & 0x01)
    {
      upd_done = 1;
      set_flag_byte(&cctrl->field_2,0x01,false);
    } else
    if (cctrl->field_C8 != 0)
    {
      upd_done = 1;
      cctrl->pos_BB.x.val =   cctrl->field_C8 * LbSinL(thing->field_52) >> 16;
      cctrl->pos_BB.y.val = -(cctrl->field_C8 * LbCosL(thing->field_52) >> 8) >> 8;
      cctrl->pos_BB.z.val = 0;
    }
    if (((thing->field_25 & 0x20) != 0) && ((thing->field_0 & 0x20) == 0))
    {
      i = get_floor_height_under_thing_at(thing, &thing->mappos) - thing->mappos.z.val + 256;
      if (i > 0)
      {
        upd_done = 1;
        if (i >= 32)
          i = 32;
        cctrl->pos_BB.z.val += i;
      } else
      if (i < 0)
      {
        upd_done = 1;
        i = -i;
        if (i >= 32)
          i = 32;
        cctrl->pos_BB.z.val -= i;
      }
    }
  }
#if (BFDEBUG_LEVEL > 19)
    LbSyncLog("%s: Finished\n",func_name);
#endif
  if (upd_done)
    return true;
  else
    return ((cctrl->pos_BB.x.val != 0) || (cctrl->pos_BB.y.val != 0) || (cctrl->pos_BB.z.val != 0));
}

struct Thing *create_thing(struct Coord3d *pos, unsigned short a1, unsigned short a2, unsigned short a3, long a4)
{
  return _DK_create_thing(pos, a1, a2, a3, a4);
}

struct Thing *create_door(struct Coord3d *pos, unsigned short a1, unsigned char a2, unsigned short a3, unsigned char a4)
{
  return _DK_create_door(pos, a1, a2, a3, a4);
}

struct Thing *create_effect_generator(struct Coord3d *pos, unsigned short a1, unsigned short a2, unsigned short a3, long a4)
{
  return _DK_create_effect_generator(pos, a1, a2, a3, a4);
}

long cleanup_current_thing_state(struct Thing *thing)
{
  return _DK_cleanup_current_thing_state(thing);
}

unsigned long setup_move_off_lava(struct Thing *thing)
{
  return _DK_setup_move_off_lava(thing);
}

struct Thing *create_footprint_sine(struct Coord3d *crtr_pos, unsigned short phase, short nfoot, unsigned short model, unsigned short owner)
{
  struct Coord3d pos;
  unsigned int i;
  pos.x.val = crtr_pos->x.val;
  pos.y.val = crtr_pos->y.val;
  pos.z.val = crtr_pos->z.val;
  switch (nfoot)
  {
  case 1:
      i = (phase - 512);
      pos.x.val +=   (LbSinL(i) << 6) >> 16;
      pos.y.val += -((LbCosL(i) << 6) >> 8) >> 8;
      return create_thing(&pos, 3, model, owner, -1);
  case 2:
      i = (phase - 512);
      pos.x.val -=   (LbSinL(i) << 6) >> 16;
      pos.y.val -= -((LbCosL(i) << 6) >> 8) >> 8;
      return create_thing(&pos, 3, model, owner, -1);
  }
  return NULL;
}

void place_bloody_footprint(struct Thing *thing)
{
  struct CreatureControl *cctrl;
  short nfoot;
  cctrl = creature_control_get_from_thing(thing);
  if (creature_control_invalid(cctrl))
    return;
  nfoot = get_foot_creature_has_down(thing);
  switch (creatures[thing->model%CREATURE_TYPES_COUNT].field_6)
  {
  case 3:
  case 4:
      break;
  case 5:
      if (nfoot)
      {
        if (create_thing(&thing->mappos, 3, 23, thing->owner, -1) != NULL)
          cctrl->bloody_footsteps_turns--;
      }
      break;
  default:
      if (create_footprint_sine(&thing->mappos, thing->field_52, nfoot, 23, thing->owner) != NULL)
        cctrl->bloody_footsteps_turns--;
      break;
  }
}

void process_landscape_affecting_creature(struct Thing *thing)
{
  static const char *func_name="process_landscape_affecting_creature";
  struct CreatureStats *crstat;
  struct CreatureControl *cctrl;
  struct SlabMap *slb;
  int stl_idx;
  short nfoot;
  int i;
#if (BFDEBUG_LEVEL > 18)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  set_flag_byte(&thing->field_25,0x01,false);
  set_flag_byte(&thing->field_25,0x02,false);
  set_flag_byte(&thing->field_25,0x80,false);
  cctrl = creature_control_get_from_thing(thing);
  if (creature_control_invalid(cctrl))
    return;
  cctrl->field_B9 = 0;

  stl_idx = get_subtile_number(thing->mappos.x.stl.num,thing->mappos.y.stl.num);
  if (((game.mapflags[stl_idx] & 0xF) << 8) == thing->mappos.z.val)
  {
    i = get_top_cube_at_pos(stl_idx);
    if ((i & 0xFFFFFFFE) == 40)
    {
      crstat = creature_stats_get_from_thing(thing);
      apply_damage_to_thing_and_display_health(thing, crstat->hurt_by_lava, -1);
      thing->field_25 |= 0x02;
    } else
    if (i == 39)
    {
      thing->field_25 |= 0x01;
    }

    if (thing->field_25 & 0x01)
    {
      nfoot = get_foot_creature_has_down(thing);
      if (nfoot)
      {
        create_effect(&thing->mappos, 19, thing->owner);
      }
      cctrl->bloody_footsteps_turns = 0;
    } else
    // Bloody footprints
    if (cctrl->bloody_footsteps_turns != 0)
    {
      place_bloody_footprint(thing);
      nfoot = get_foot_creature_has_down(thing);
      if (create_footprint_sine(&thing->mappos, thing->field_52, nfoot, 23, thing->owner) != NULL)
        cctrl->bloody_footsteps_turns--;
    } else
    // Snow footprints
    if (game.texture_id == 2)
    {
      slb = get_slabmap_block(map_to_slab[thing->mappos.x.stl.num], map_to_slab[thing->mappos.y.stl.num]);
      if (slb->slab == SlbT_PATH)
      {
        thing->field_25 |= 0x80u;
        nfoot = get_foot_creature_has_down(thing);
        create_footprint_sine(&thing->mappos, thing->field_52, nfoot, 94, thing->owner);
      }
    }
    process_creature_standing_on_corpses_at(thing, &thing->mappos);
  }
  if (((thing->field_0 & 0x20) == 0) && ((thing->field_25 & 0x02) != 0))
  {
    crstat = creature_stats_get_from_thing(thing);
    if (crstat->hurt_by_lava)
    {
        if (thing->field_7 == 14)
          i = thing->field_8;
        else
          i = thing->field_7;
        if ((i != -113) && (cctrl->field_2FE + 64 < game.play_gameturn))
        {
            cctrl->field_2FE = game.play_gameturn;
            if ( cleanup_current_thing_state(thing) )
            {
              if ( setup_move_off_lava(thing) )
                thing->field_8 = 143;
              else
                set_start_state(thing);
            }
        }
    }
  }
#if (BFDEBUG_LEVEL > 19)
    LbSyncLog("%s: Finished\n",func_name);
#endif
}

long update_creature(struct Thing *thing)
{
  static const char *func_name="update_creature";
#if (BFDEBUG_LEVEL > 18)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  //return _DK_update_creature(thing);
  struct PlayerInfo *player;
  struct CreatureControl *cctrl;
  struct Thing *tngp;
  struct Map *map;
  int i;

  map = get_map_block(thing->mappos.x.stl.num, thing->mappos.y.stl.num);
  if ((thing->field_7 == 67) && (map->flags & 0x40))
  {
    kill_creature(thing, game.things_lookup[0], -1, 1, 0, 1);
    return 0;
  }
  if (thing->health < 0)
  {
    kill_creature(thing, game.things_lookup[0], -1, 0, 0, 0);
    return 0;
  }
  cctrl = creature_control_get_from_thing(thing);
  if (creature_control_invalid(cctrl))
    return 0;
  if (game.field_150356)
  {
    if ((cctrl->field_2EF != 0) && (cctrl->field_2EF <= game.play_gameturn))
    {
        cctrl->field_2EF = 0;
        create_effect(&thing->mappos, imp_spangle_effects[thing->owner], thing->owner);
        move_thing_in_map(thing, &game.armageddon.mappos);
    }
  }

  if (cctrl->field_B1 > 0)
    cctrl->field_B1--;
  if (cctrl->field_8B == 0)
    cctrl->field_8B = game.field_14EA4B;
  if (cctrl->field_302 == 0)
    process_creature_instance(thing);
  update_creature_count(thing);
  if (thing->field_0 & 0x20)
  {
    if (cctrl->field_AB == 0)
    {
      if (cctrl->field_302 != 0)
      {
        cctrl->field_302--;
      } else
      if (process_creature_state(thing))
      {
        error(func_name, 970, "A state return type for a human controlled creature?");
      }
    }
    cctrl = creature_control_get_from_thing(thing);
    player = &(game.players[thing->owner%PLAYERS_COUNT]);
    if (cctrl->field_AB & 0x02)
    {
      if ((player->field_3 & 0x04) == 0)
        PaletteSetPlayerPalette(player, blue_palette);
    } else
    {
      if ((player->field_3 & 0x04) != 0)
        PaletteSetPlayerPalette(player, _DK_palette);
    }
  } else
  {
    if (cctrl->field_AB == 0)
    {
      if (cctrl->field_302 > 0)
      {
        cctrl->field_302--;
      } else
      if (process_creature_state(thing))
      {
        return 0;
      }
    }
  }

  if (update_creature_movements(thing))
  {
    thing->pos_38.x.val += cctrl->pos_BB.x.val;
    thing->pos_38.y.val += cctrl->pos_BB.y.val;
    thing->pos_38.z.val += cctrl->pos_BB.z.val;
  }
  move_creature(thing);
  if ((thing->field_0 & 0x20) != 0)
  {
    if ((cctrl->field_0 & 0x4000) == 0)
      cctrl->field_C8 /= 2;
    if ((cctrl->field_0 & 0x8000) == 0)
      cctrl->field_CA /= 2;
  } else
  {
    cctrl->field_C8 = 0;
  }
  process_spells_affected_by_effect_elements(thing);
  process_landscape_affecting_creature(thing);
  process_disease(thing);
  move_thing_in_map(thing, &thing->mappos);
  set_creature_graphic(thing);
  if (cctrl->field_2B0)
    process_keeper_spell_effect(thing);

  if (thing->word_17 > 0)
    thing->word_17--;

  if (cctrl->field_7A & 0x0FFF)
  {
    if ( creature_is_group_leader(thing) )
      leader_find_positions_for_followers(thing);
  }

  if (cctrl->field_6E > 0)
  {
    tngp = thing_get(cctrl->field_6E);
    if (tngp->field_1 & 0x01)
      move_thing_in_map(tngp, &thing->mappos);
  }
  if (update_creature_levels(thing) == -1)
  {
    return 0;
  }
  process_creature_self_spell_casting(thing);
  cctrl->pos_BB.x.val = 0;
  cctrl->pos_BB.y.val = 0;
  cctrl->pos_BB.z.val = 0;
  set_flag_word(&cctrl->field_0,0x4000,false);
  set_flag_word(&cctrl->field_0,0x8000,false);
  set_flag_byte(&cctrl->field_AD,0x04,false);
  process_thing_spell_effects(thing);
  return 1;
}

long update_effect(struct Thing *thing)
{
  static const char *func_name="update_effect";
#if (BFDEBUG_LEVEL > 18)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  return _DK_update_effect(thing);
}

long process_effect_generator(struct Thing *thing)
{
  static const char *func_name="process_effect_generator";
#if (BFDEBUG_LEVEL > 18)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  return _DK_process_effect_generator(thing);
}

long update_trap(struct Thing *thing)
{
  static const char *func_name="update_trap";
#if (BFDEBUG_LEVEL > 18)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  return _DK_update_trap(thing);
}

long process_door(struct Thing *thing)
{
  static const char *func_name="process_door";
#if (BFDEBUG_LEVEL > 18)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  return _DK_process_door(thing);
}

void update_thing_animation(struct Thing *thing)
{
  static const char *func_name="update_thing_animation";
  int i;
  struct CreatureControl *cctrl;
#if (BFDEBUG_LEVEL > 18)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  if (thing->class_id == 5)
  {
    cctrl = creature_control_get_from_thing(thing);
    if (!creature_control_invalid(cctrl))
      cctrl->field_CE = thing->field_40;
  }
  if ((thing->field_3E != 0) && (thing->field_49 != 0))
  {
      thing->field_40 += thing->field_3E;
      i = (thing->field_49 << 8);
      if (i <= 0) i = 256;
      while (thing->field_40  < 0)
      {
        thing->field_40 += i;
      }
      if (thing->field_40 > i-1)
      {
        if (thing->field_4F & 0x40)
        {
          thing->field_3E = 0;
          thing->field_40 = i-1;
        } else
        {
          thing->field_40 %= i;
        }
      }
      thing->field_48 = (thing->field_40 >> 8) & 0xFF;
  }
  if (thing->field_4A != 0)
  {
    thing->field_46 += thing->field_4A;
    if (thing->field_46 > thing->field_4B)
    {
      if (thing->field_46 >= thing->field_4D)
      {
        thing->field_46 = thing->field_4D;
        if (thing->field_50 & 0x02)
          thing->field_4A = -thing->field_4A;
        else
          thing->field_4A = 0;
      }
    } else
    {
      thing->field_46 = thing->field_4B;
      if (thing->field_50 & 0x02)
        thing->field_4A = -thing->field_4A;
      else
        thing->field_4A = 0;
    }
  }
}

long update_thing(struct Thing *thing)
{
  static const char *func_name="update_thing";
  Thing_Class_Func classfunc;
  struct Coord3d pos;
#if (BFDEBUG_LEVEL > 18)
    LbSyncLog("%s: Starting\n",func_name);
#endif

  if ((thing->field_25 & 0x40) == 0)
  {
    if ((thing->field_1 & 0x04) != 0)
    {
      thing->pos_2C.x.val += thing->pos_32.x.val;
      thing->pos_2C.y.val += thing->pos_32.y.val;
      thing->pos_2C.z.val += thing->pos_32.z.val;
      thing->pos_32.x.val = 0;
      thing->pos_32.y.val = 0;
      thing->pos_32.z.val = 0;
      set_flag_byte(&thing->field_1,0x04,false);
    }
    thing->pos_38.x.val = thing->pos_2C.x.val;
    thing->pos_38.y.val = thing->pos_2C.y.val;
    thing->pos_38.z.val = thing->pos_2C.z.val;
    if (thing->field_1 & 0x08)
    {
      thing->pos_38.x.val += thing->pos_26.x.val;
      thing->pos_38.y.val += thing->pos_26.y.val;
      thing->pos_38.z.val += thing->pos_26.z.val;
      thing->pos_26.x.val = 0;
      thing->pos_26.y.val = 0;
      thing->pos_26.z.val = 0;
      set_flag_byte(&thing->field_1,0x08,false);
    }
  }
  classfunc = class_functions[thing->class_id%THING_CLASSES_COUNT];
  if (classfunc == NULL)
      return 0;
  if (classfunc(thing) < 0)
      return 0;
#if (BFDEBUG_LEVEL > 18)
    LbSyncLog("%s: Class function end ok\n",func_name);
#endif
  if ((thing->field_25 & 0x40) == 0)
  {
      if (thing->mappos.z.val > thing->field_60)
      {
        if (thing->pos_2C.x.val != 0)
          thing->pos_2C.x.val = thing->pos_2C.x.val * (256 - thing->field_24) / 256;
        if (thing->pos_2C.y.val != 0)
          thing->pos_2C.y.val = thing->pos_2C.y.val * (256 - thing->field_24) / 256;
        if ((thing->field_25 & 0x20) == 0)
        {
          thing->pos_32.z.val -= thing->field_20;
          thing->field_1 |= 0x04;
        }
      } else
      {
        if (thing->pos_2C.x.val != 0)
          thing->pos_2C.x.val = thing->pos_2C.x.val * (256 - thing->field_23) / 256;
        if (thing->pos_2C.y.val != 0)
          thing->pos_2C.y.val = thing->pos_2C.y.val * (256 - thing->field_23) / 256;
        thing->mappos.z.val = thing->field_60;
        if ((thing->field_25 & 0x08) != 0)
        {
          thing->pos_2C.z.val = 0;
        }
      }
  }
  update_thing_animation(thing);
  update_thing_sound(thing);
  if ((do_lights) && (thing->field_62 != 0))
  {
      if (light_is_light_allocated(thing->field_62))
      {
        pos.x.val = thing->mappos.x.val;
        pos.y.val = thing->mappos.y.val;
        pos.z.val = thing->mappos.z.val + thing->field_58;
        light_set_light_position(thing->field_62, &pos);
      } else
      {
        thing->field_62 = 0;
      }
  }
  return 1;
}

TbBigChecksum get_thing_checksum(struct Thing *thing)
{
  static const char *func_name="get_thing_checksum";
#if (BFDEBUG_LEVEL > 18)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  return _DK_get_thing_checksum(thing);
}

short update_thing_sound(struct Thing *thing)
{
  //return _DK_update_thing_sound(thing);
  if (thing->field_66)
  {
    if ( S3DEmitterHasFinishedPlaying(thing->field_66) )
    {
      S3DDestroySoundEmitter(thing->field_66);
      thing->field_66 = 0;
    } else
    {
      S3DMoveSoundEmitterTo(thing->field_66,
          thing->mappos.x.val, thing->mappos.y.val, thing->mappos.z.val);
    }
  }
  return true;
}

/*
 * Plays a smacker animation file and sets frontend state to nstate.
 * @param nstate Frontend state; -1 means no change, -2 means don't even
 *    change screen mode.
 * @return Returns false if fatal error occured and probram execution should end.
 */
short play_smacker_file(char *filename, int nstate)
{
  static const char *func_name="play_smacker_file";
  unsigned int movie_flags = 0;
  if ( SoundDisabled )
    movie_flags |= 0x01;
  short result;

  result = 1;
  if ((result)&&(nstate>-2))
  {
    if ( setup_screen_mode_minimal(get_movies_vidmode()) )
    {
      LbMouseChangeSprite(0);
      LbScreenClear(0);
      LbScreenSwap();
    } else
    {
      error(func_name, 2356, "Can't enter movies video mode to play a Smacker file");
      result=0;
    }
  }
  if (result)
  {
    // Fail in playing shouldn't set result=0, because result=0 means fatal error.
    if (play_smk_(filename, 0, movie_flags | 0x100) == 0)
    {
      error(func_name, 2357, "Smacker play error");
      result=0;
    }
  }
  if (nstate>-2)
  {
    if ( !setup_screen_mode_minimal(get_frontend_vidmode()) )
    {
      error(func_name, 2358, "Can't re-enter frontend video mode after playing Smacker file");
      FatalError = 1;
      exit_keeper = 1;
      return 0;
    }
  } else
  {
    memset(frontend_palette, 0, PALETTE_SIZE);
  }
  LbScreenClear(0);
  LbScreenSwap();
  LbPaletteSet(frontend_palette);
  if ( nstate >= 0 )
    frontend_set_state(nstate);
  lbDisplay.LeftButton = 0;
  lbDisplay.RightButton = 0;
  lbDisplay.MiddleButton = 0;
  if (nstate > -2)
    LbMouseSetPosition(lbDisplay.PhysicalScreenWidth/2, lbDisplay.PhysicalScreenHeight/2);
  return result;
}

void process_network_error(long errcode)
{
  _DK_process_network_error(errcode);
}

short setup_network_service(int srvidx)
{
  struct SerialInitData *init_data;
  long maxplayrs;

  switch (srvidx)
  {
  case 0:
      maxplayrs = 2;
      init_data = &_DK_net_serial_data;
      game.flags_font |= 0x10;
      LbSyncLog("Initializing %d-players serial network.\n",maxplayrs);
      break;
  case 1:
      maxplayrs = 2;
      init_data = &_DK_net_modem_data;
      game.flags_font |= 0x10;
      LbSyncLog("Initializing %d-players modem network.\n",maxplayrs);
      break;
  case 2:
      maxplayrs = 4;
      init_data = NULL;
      set_flag_byte(&game.flags_font,0x10,false);
      LbSyncLog("Initializing %d-players IPX network.\n",maxplayrs);
      break;
  default:
      maxplayrs = 4;
      init_data = NULL;
      set_flag_byte(&game.flags_font,0x10,false);
      LbSyncLog("Initializing %d-players type %d network.\n",maxplayrs,srvidx);
      break;
  }
  memset(&net_player_info[0], 0, sizeof(struct TbNetworkPlayerInfo));
  if ( LbNetwork_Init(srvidx, _DK_net_guid, maxplayrs, &net_screen_packet, 0xCu, &net_player_info[0], init_data) )
  {
    if (srvidx != 0)
      _DK_process_network_error(-800);
    return 0;
  }
  net_service_index_selected = srvidx;
  if (game.flags_font & 0x10)
    LbNetwork_ChangeExchangeTimeout(10);
  frontend_set_state(5);
  return 1;
}

void init_censorship(void)
{
  if (install_info.lang_id == 3) // German language
  {
    // Modification for Dark Mistress
    creature_graphics[20][14] = 48;
  }
}

long load_anim_file(void)
{
  return _DK_load_anim_file();
}

long load_cube_file(void)
{
  return _DK_load_cube_file();
}

void init_colours(void)
{
  _DK_init_colours();
}

/*
 * Fills the array of keyboard key names.
 */
void init_key_to_strings(void)
{
  struct KeyToStringInit *ktsi;
  long k;
  memset(key_to_string, 0, sizeof(key_to_string));
  for (ktsi = &key_to_string_init[0]; ktsi->chr != 0; ktsi++)
  {
    k = ktsi->chr;
    key_to_string[k] = ktsi->str_idx;
  }
}

short init_sound(void)
{
  static const char *func_name="init_sound";
  struct SoundSettings *snd_settng;
  unsigned long i;
#if (BFDEBUG_LEVEL > 8)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  if (SoundDisabled)
    return false;
  snd_settng = &game.sound_settings;
  SetupAudioOptionDefaults(snd_settng);
  snd_settng->field_E = 3;
  snd_settng->sound_type = 1622;
  snd_settng->sound_data_path = sound_dir;
  snd_settng->dir3 = sound_dir;
  snd_settng->field_12 = 1;
  snd_settng->stereo = 1;
  i = get_best_sound_heap_size(mem_size);
  if (i < 1048576)
    snd_settng->max_number_of_samples = 10;
  else
    snd_settng->max_number_of_samples = 16;
  snd_settng->danger_music = 0;
  snd_settng->no_load_music = 0;
  snd_settng->no_load_sounds = 1;
  snd_settng->field_16 = 1;
  if ((game.flags_font & 0x40) == 0)
    snd_settng->field_16 = 0;
  snd_settng->field_18 = 1;
  snd_settng->redbook_enable = 1;
  snd_settng->sound_system = 0;
  InitAudio(snd_settng);
  LoadMusic(0);
  if (!GetSoundInstalled())
  {
    SoundDisabled = 1;
    return false;
  }
  S3DInit();
  S3DSetNumberOfSounds(snd_settng->max_number_of_samples);
  S3DSetMaximumSoundDistance(5120);
  return true;
}

long init_sound_heap_two_banks(unsigned char *heap_mem, long heap_size, char *snd_fname, char *spc_fname, long a5)
{
  static const char *func_name="init_sound_heap_two_banks";
#if (BFDEBUG_LEVEL > 8)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  return _DK_init_sound_heap_two_banks(heap_mem, heap_size, snd_fname, spc_fname, a5);
}

void close_sound_heap(void)
{
  _DK_close_sound_heap();
/*
  if (sound_file != -1)
  {
    LbFileClose(sound_file);
    sound_file = -1;
  }
  if (sound_file2 != -1)
  {
    LbFileClose(sound_file2);
    sound_file2 = -1;
  }
  using_two_banks = 0;
*/
}

TbBool setup_heaps(void)
{
  static const char *func_name="setup_heaps";
  TbBool low_memory;
  char snd_fname[2048];
  char *spc_fname;
  long i,k;
#if (BFDEBUG_LEVEL > 8)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  //_DK_setup_heaps();LbSyncLog("GraphicsHeap Size %d\n", heap_size);LbSyncLog("SoundHeap Size %d\n", sound_heap_size);return 1;
  low_memory = false;
  if (!SoundDisabled)
  {
    StopAllSamples();
    close_sound_heap();
    if (sound_heap_memory != NULL)
    {
      LbMemoryFree(sound_heap_memory);
      sound_heap_memory = NULL;
    }
  }
  if (heap != NULL)
  {
    error(func_name, 1255, "Graphics heap already allocated");
    LbMemoryFree(heap);
    heap = NULL;
  }
  // Allocate sound heap
  if (!SoundDisabled)
  {
    i = mem_size;
    while (sound_heap_memory == NULL)
    {
      sound_heap_size = get_best_sound_heap_size(i);
      i = get_smaller_memory_amount(i);
      sound_heap_memory = LbMemoryAlloc(sound_heap_size);
      if ((i <= 8) && (sound_heap_memory == NULL))
      {
        low_memory = true;
        break;
      }
    }
  }
  // Allocate graphics heap
  i = mem_size;
  while (heap == NULL)
  {
    heap_size = get_best_sound_heap_size(i);
    i = get_smaller_memory_amount(i);
    heap = LbMemoryAlloc(heap_size);
    if ((i <= 8) && (heap == NULL))
    {
      low_memory = true;
      break;
    }
  }
  LbSyncLog("GraphicsHeap Size %d\n", heap_size);

  if (low_memory)
  {
    i = heap_size;
    k = sound_heap_size;
    while (heap != NULL)
    {
      if ((!SoundDisabled) && (sound_heap_memory == NULL))
      {
        break;
      }
      if (!SoundDisabled)
      {
        if (k < i)
        {
          i -= 16384;
        } else
        if (k == i)
        {
          i -= 16384;
          k -= 16384;
        } else
        {
          k -= 16384;
        }
        heap_size = i;
        sound_heap_size = k;
        if (k < 524288)
        {
          error(func_name, 1319, "Unable to allocate heaps (small_mem)");
          return false;
        }
      } else
      {
        i -= 16384;
        heap_size = i;
        sound_heap_size = k;
      }
      if (i < 524288)
      {
        if (sound_heap_memory != NULL)
        {
          LbMemoryFree(sound_heap_memory);
          sound_heap_memory = NULL;
        }
        error(func_name, 1334, "Unable to allocate heaps (small_mem)");
        return false;
        }
      }
      if (sound_heap_memory != NULL)
      {
        LbMemoryFree(sound_heap_memory);
        i = heap_size;
        k = sound_heap_size;
        sound_heap_memory = NULL;
      }
      heap_size = i;
      sound_heap_size = k;
      if (heap != NULL)
      {
        LbMemoryFree(heap);
        heap = NULL;
      }
      if (!SoundDisabled)
      {
        sound_heap_memory = LbMemoryAlloc(k);
      }
      heap = LbMemoryAlloc(i);
  }
  if (!SoundDisabled)
  {
    LbSyncLog("SoundHeap Size %d\n", sound_heap_size);
    // Prepare sound sample bank file names
    prepare_file_path_buf(snd_fname,FGrp_LrgSound,sound_fname);
    // language-specific speech file
    spc_fname = prepare_file_fmtpath(FGrp_LrgSound,"speech_%s.dat",get_conf_parameter_text(lang_type,install_info.lang_id));
    // default speech file
    if (!LbFileExists(spc_fname))
      spc_fname = prepare_file_path(FGrp_LrgSound,speech_fname);
    // speech file for english
    if (!LbFileExists(spc_fname))
      spc_fname = prepare_file_fmtpath(FGrp_LrgSound,"speech_%s.dat",get_conf_parameter_text(lang_type,1));
    // Initialize sample banks
    if (!init_sound_heap_two_banks(sound_heap_memory, sound_heap_size, snd_fname, spc_fname, 1622))
    {
      LbMemoryFree(sound_heap_memory);
      sound_heap_memory = NULL;
      SoundDisabled = true;
      error(func_name, 1379, "Unable to initialise sound heap");
      return false;
    }
  }
  return true;
}

void engine_init(void)
{
  _DK_engine_init(); return;
}

void init_iso_3d_conversion_tables(void)
{
  long i;
  for (i=0; i < TD_ISO_POINTS; i++)
  {
    td_iso[i] = -1;
    iso_td[i] = -1;
  }
  td_iso[48] = 49;
  iso_td[49] = 48;
  td_iso[404] = 405;
  td_iso[406] = 407;
  td_iso[402] = 403;
  td_iso[408] = 409;
  td_iso[412] = 413;
  td_iso[410] = 411;
  td_iso[420] = 421;
  td_iso[418] = 419;
  td_iso[416] = 417;
  td_iso[414] = 415;
  td_iso[422] = 423;
  td_iso[62] = 63;
  iso_td[405] = 404;
  iso_td[407] = 406;
  iso_td[403] = 402;
  iso_td[409] = 408;
  iso_td[413] = 412;
  iso_td[411] = 410;
  iso_td[421] = 420;
  iso_td[419] = 418;
  iso_td[417] = 416;
  iso_td[415] = 414;
  iso_td[423] = 422;
  iso_td[63] = 62;
  td_iso[250] = 251;
  td_iso[248] = 249;
  td_iso[252] = 253;
  td_iso[254] = 255;
  td_iso[258] = 259;
  td_iso[256] = 257;
  td_iso[266] = 267;
  td_iso[264] = 265;
  td_iso[262] = 263;
  td_iso[260] = 261;
  td_iso[268] = 269;
  td_iso[64] = 65;
  iso_td[251] = 250;
  iso_td[249] = 248;
  iso_td[253] = 252;
  iso_td[255] = 254;
  iso_td[259] = 258;
  iso_td[257] = 256;
  iso_td[267] = 266;
  iso_td[265] = 264;
  iso_td[263] = 262;
  iso_td[261] = 260;
  iso_td[269] = 268;
  iso_td[65] = 64;
  td_iso[26] = 27;
  td_iso[24] = 25;
  td_iso[28] = 29;
  td_iso[30] = 31;
  td_iso[34] = 35;
  td_iso[32] = 33;
  td_iso[42] = 43;
  td_iso[40] = 41;
  td_iso[38] = 39;
  td_iso[36] = 37;
  td_iso[44] = 45;
  iso_td[27] = 26;
  iso_td[25] = 24;
  iso_td[29] = 28;
  iso_td[31] = 30;
  iso_td[35] = 34;
  iso_td[33] = 32;
  iso_td[43] = 42;
  iso_td[41] = 40;
  iso_td[39] = 38;
  iso_td[37] = 36;
  iso_td[45] = 44;
  td_iso[732] = 733;
  td_iso[730] = 731;
  td_iso[734] = 735;
  td_iso[736] = 737;
  td_iso[740] = 741;
  td_iso[738] = 739;
  td_iso[748] = 749;
  td_iso[746] = 747;
  td_iso[744] = 745;
  td_iso[742] = 743;
  td_iso[750] = 751;
  td_iso[92] = 93;
  iso_td[733] = 732;
  iso_td[731] = 730;
  iso_td[735] = 734;
  iso_td[737] = 736;
  iso_td[741] = 740;
  iso_td[739] = 738;
  iso_td[749] = 748;
  iso_td[747] = 746;
  iso_td[745] = 744;
  iso_td[743] = 742;
  iso_td[751] = 750;
  iso_td[93] = 92;
  td_iso[710] = 711;
  td_iso[708] = 709;
  td_iso[712] = 713;
  td_iso[714] = 715;
  td_iso[718] = 719;
  td_iso[716] = 717;
  td_iso[726] = 727;
  td_iso[724] = 725;
  td_iso[722] = 723;
  td_iso[720] = 721;
  td_iso[728] = 729;
  td_iso[90] = 91;
  iso_td[711] = 710;
  iso_td[709] = 708;
  iso_td[713] = 712;
  iso_td[715] = 714;
  iso_td[719] = 718;
  iso_td[717] = 716;
  iso_td[727] = 726;
  iso_td[725] = 724;
  iso_td[723] = 722;
  iso_td[721] = 720;
  iso_td[729] = 728;
  iso_td[91] = 90;
  td_iso[236] = 237;
  td_iso[228] = 229;
  td_iso[230] = 231;
  td_iso[232] = 233;
  td_iso[234] = 235;
  td_iso[244] = 245;
  td_iso[242] = 243;
  td_iso[240] = 241;
  td_iso[238] = 239;
  td_iso[246] = 247;
  td_iso[60] = 61;
  iso_td[237] = 236;
  iso_td[229] = 228;
  iso_td[227] = 226;
  iso_td[231] = 230;
  iso_td[233] = 232;
  iso_td[235] = 234;
  iso_td[245] = 244;
  iso_td[243] = 242;
  iso_td[241] = 240;
  iso_td[239] = 238;
  iso_td[247] = 246;
  iso_td[61] = 60;
  td_iso[170] = 171;
  td_iso[162] = 163;
  td_iso[160] = 161;
  td_iso[164] = 165;
  td_iso[166] = 167;
  td_iso[168] = 169;
  td_iso[178] = 179;
  td_iso[176] = 177;
  td_iso[174] = 175;
  td_iso[172] = 173;
  td_iso[180] = 181;
  iso_td[171] = 170;
  iso_td[163] = 162;
  iso_td[161] = 160;
  iso_td[165] = 164;
  iso_td[167] = 166;
  iso_td[169] = 168;
  iso_td[179] = 178;
  iso_td[177] = 176;
  iso_td[175] = 174;
  iso_td[173] = 172;
  iso_td[181] = 180;
  td_iso[280] = 281;
  td_iso[270] = 271;
  td_iso[272] = 273;
  td_iso[274] = 275;
  td_iso[276] = 277;
  td_iso[288] = 289;
  td_iso[286] = 287;
  td_iso[284] = 285;
  td_iso[282] = 283;
  td_iso[278] = 279;
  td_iso[290] = 291;
  td_iso[58] = 59;
  iso_td[281] = 280;
  iso_td[271] = 270;
  iso_td[273] = 272;
  iso_td[275] = 274;
  iso_td[277] = 276;
  iso_td[289] = 288;
  iso_td[287] = 286;
  iso_td[285] = 284;
  iso_td[283] = 282;
  iso_td[279] = 278;
  iso_td[291] = 290;
  iso_td[59] = 58;
  td_iso[214] = 215;
  td_iso[206] = 207;
  td_iso[204] = 205;
  td_iso[208] = 209;
  td_iso[210] = 211;
  td_iso[212] = 213;
  td_iso[222] = 223;
  td_iso[220] = 221;
  td_iso[218] = 219;
  td_iso[216] = 217;
  td_iso[224] = 225;
  iso_td[215] = 214;
  iso_td[207] = 206;
  iso_td[205] = 204;
  iso_td[209] = 208;
  iso_td[211] = 210;
  iso_td[213] = 212;
  iso_td[223] = 222;
  iso_td[221] = 220;
  iso_td[219] = 218;
  iso_td[217] = 216;
  iso_td[225] = 224;
  td_iso[368] = 369;
  td_iso[360] = 361;
  td_iso[358] = 359;
  td_iso[362] = 363;
  td_iso[364] = 365;
  td_iso[366] = 367;
  td_iso[376] = 377;
  td_iso[374] = 375;
  td_iso[372] = 373;
  td_iso[370] = 371;
  td_iso[378] = 379;
  iso_td[369] = 368;
  iso_td[361] = 360;
  iso_td[359] = 358;
  iso_td[363] = 362;
  iso_td[365] = 364;
  iso_td[367] = 366;
  iso_td[377] = 376;
  iso_td[375] = 374;
  iso_td[373] = 372;
  iso_td[371] = 370;
  iso_td[379] = 378;
  td_iso[346] = 347;
  td_iso[338] = 339;
  td_iso[336] = 337;
  td_iso[340] = 341;
  td_iso[342] = 343;
  td_iso[344] = 345;
  td_iso[354] = 355;
  td_iso[352] = 353;
  td_iso[350] = 351;
  td_iso[348] = 349;
  td_iso[356] = 357;
  iso_td[347] = 346;
  iso_td[339] = 338;
  iso_td[337] = 336;
  iso_td[341] = 340;
  iso_td[343] = 342;
  iso_td[345] = 344;
  iso_td[355] = 354;
  iso_td[353] = 352;
  iso_td[351] = 350;
  iso_td[349] = 348;
  iso_td[357] = 356;
  td_iso[302] = 303;
  td_iso[294] = 295;
  td_iso[292] = 293;
  td_iso[296] = 297;
  td_iso[298] = 299;
  td_iso[300] = 301;
  td_iso[310] = 311;
  td_iso[308] = 309;
  td_iso[306] = 307;
  td_iso[304] = 305;
  td_iso[312] = 313;
  iso_td[303] = 302;
  iso_td[295] = 294;
  iso_td[293] = 292;
  iso_td[297] = 296;
  iso_td[299] = 298;
  iso_td[301] = 300;
  iso_td[311] = 310;
  iso_td[309] = 308;
  iso_td[307] = 306;
  iso_td[305] = 304;
  iso_td[313] = 312;
  td_iso[390] = 391;
  td_iso[382] = 383;
  td_iso[380] = 381;
  td_iso[384] = 385;
  td_iso[386] = 387;
  td_iso[388] = 389;
  td_iso[398] = 399;
  td_iso[396] = 397;
  td_iso[394] = 395;
  td_iso[392] = 393;
  td_iso[400] = 401;
  td_iso[54] = 55;
  iso_td[391] = 390;
  iso_td[383] = 382;
  iso_td[381] = 380;
  iso_td[385] = 384;
  iso_td[387] = 386;
  iso_td[389] = 388;
  iso_td[399] = 398;
  iso_td[397] = 396;
  iso_td[395] = 394;
  iso_td[393] = 392;
  iso_td[401] = 400;
  iso_td[55] = 54;
  td_iso[456] = 457;
  td_iso[448] = 449;
  td_iso[446] = 447;
  td_iso[450] = 451;
  td_iso[452] = 453;
  td_iso[454] = 455;
  td_iso[464] = 465;
  td_iso[462] = 463;
  td_iso[460] = 461;
  td_iso[458] = 459;
  td_iso[466] = 467;
  td_iso[68] = 69;
  iso_td[457] = 456;
  iso_td[449] = 448;
  iso_td[447] = 446;
  iso_td[451] = 450;
  iso_td[453] = 452;
  iso_td[455] = 454;
  iso_td[465] = 464;
  iso_td[463] = 462;
  iso_td[461] = 460;
  iso_td[459] = 458;
  iso_td[467] = 466;
  iso_td[69] = 68;
  td_iso[688] = 689;
  td_iso[686] = 687;
  td_iso[690] = 691;
  td_iso[692] = 693;
  td_iso[694] = 695;
  td_iso[696] = 697;
  td_iso[704] = 705;
  td_iso[702] = 703;
  td_iso[700] = 701;
  td_iso[698] = 699;
  td_iso[706] = 707;
  td_iso[88] = 89;
  iso_td[689] = 688;
  iso_td[687] = 686;
  iso_td[691] = 690;
  iso_td[693] = 692;
  iso_td[695] = 694;
  iso_td[697] = 696;
  iso_td[705] = 704;
  iso_td[703] = 702;
  iso_td[701] = 700;
  iso_td[699] = 698;
  iso_td[707] = 706;
  iso_td[89] = 88;
  td_iso[468] = 469;
  td_iso[470] = 471;
  td_iso[472] = 473;
  td_iso[474] = 475;
  td_iso[476] = 477;
  td_iso[478] = 479;
  td_iso[480] = 481;
  td_iso[482] = 483;
  td_iso[484] = 485;
  td_iso[486] = 487;
  td_iso[70] = 71;
  iso_td[469] = 468;
  iso_td[471] = 470;
  iso_td[473] = 472;
  iso_td[475] = 474;
  iso_td[477] = 476;
  iso_td[479] = 478;
  iso_td[481] = 480;
  iso_td[483] = 482;
  iso_td[485] = 484;
  iso_td[487] = 486;
  iso_td[71] = 70;
  td_iso[754] = 755;
  td_iso[752] = 753;
  td_iso[756] = 757;
  td_iso[758] = 759;
  td_iso[760] = 761;
  td_iso[762] = 763;
  td_iso[770] = 771;
  td_iso[768] = 769;
  td_iso[766] = 767;
  td_iso[764] = 765;
  td_iso[772] = 773;
  td_iso[94] = 95;
  iso_td[755] = 754;
  iso_td[753] = 752;
  iso_td[757] = 756;
  iso_td[759] = 758;
  iso_td[761] = 760;
  iso_td[763] = 762;
  iso_td[771] = 770;
  iso_td[769] = 768;
  iso_td[767] = 766;
  iso_td[765] = 764;
  iso_td[773] = 772;
  iso_td[95] = 94;
  td_iso[580] = 581;
  td_iso[578] = 579;
  td_iso[582] = 583;
  td_iso[584] = 585;
  td_iso[586] = 587;
  td_iso[588] = 589;
  td_iso[596] = 597;
  td_iso[594] = 595;
  td_iso[592] = 593;
  td_iso[590] = 591;
  td_iso[598] = 599;
  td_iso[78] = 79;
  iso_td[581] = 580;
  iso_td[579] = 578;
  iso_td[583] = 582;
  iso_td[585] = 584;
  iso_td[587] = 586;
  iso_td[589] = 588;
  iso_td[597] = 596;
  iso_td[595] = 594;
  iso_td[593] = 592;
  iso_td[591] = 590;
  iso_td[599] = 598;
  iso_td[79] = 78;
  td_iso[644] = 645;
  td_iso[646] = 647;
  td_iso[648] = 649;
  td_iso[650] = 651;
  td_iso[652] = 653;
  td_iso[658] = 659;
  td_iso[660] = 661;
  td_iso[656] = 657;
  td_iso[654] = 655;
  td_iso[662] = 663;
  td_iso[84] = 85;
  iso_td[645] = 644;
  iso_td[647] = 646;
  iso_td[649] = 648;
  iso_td[651] = 650;
  iso_td[653] = 652;
  iso_td[659] = 658;
  iso_td[661] = 660;
  iso_td[657] = 656;
  iso_td[655] = 654;
  iso_td[663] = 662;
  iso_td[85] = 84;
  td_iso[532] = 533;
  td_iso[534] = 535;
  td_iso[536] = 537;
  td_iso[538] = 539;
  td_iso[540] = 541;
  td_iso[542] = 543;
  td_iso[550] = 551;
  td_iso[548] = 549;
  td_iso[546] = 547;
  td_iso[544] = 545;
  td_iso[552] = 553;
  td_iso[76] = 77;
  iso_td[533] = 532;
  iso_td[535] = 534;
  iso_td[537] = 536;
  iso_td[539] = 538;
  iso_td[541] = 540;
  iso_td[543] = 542;
  iso_td[551] = 550;
  iso_td[549] = 548;
  iso_td[547] = 546;
  iso_td[545] = 544;
  iso_td[553] = 552;
  iso_td[77] = 76;
  td_iso[666] = 667;
  td_iso[668] = 669;
  td_iso[664] = 665;
  td_iso[670] = 671;
  td_iso[674] = 675;
  td_iso[672] = 673;
  td_iso[682] = 683;
  td_iso[680] = 681;
  td_iso[678] = 679;
  td_iso[676] = 677;
  td_iso[684] = 685;
  td_iso[86] = 87;
  iso_td[667] = 666;
  iso_td[669] = 668;
  iso_td[665] = 664;
  iso_td[671] = 670;
  iso_td[675] = 674;
  iso_td[673] = 672;
  iso_td[683] = 682;
  iso_td[681] = 680;
  iso_td[679] = 678;
  iso_td[677] = 676;
  iso_td[685] = 684;
  iso_td[87] = 86;
  td_iso[424] = 425;
  td_iso[426] = 427;
  td_iso[428] = 429;
  td_iso[430] = 431;
  td_iso[432] = 433;
  td_iso[434] = 435;
  td_iso[436] = 437;
  td_iso[438] = 439;
  td_iso[440] = 441;
  td_iso[442] = 443;
  td_iso[444] = 445;
  td_iso[52] = 53;
  iso_td[425] = 424;
  iso_td[427] = 426;
  iso_td[429] = 428;
  iso_td[431] = 430;
  iso_td[433] = 432;
  iso_td[435] = 434;
  iso_td[437] = 436;
  iso_td[439] = 438;
  iso_td[441] = 440;
  iso_td[443] = 442;
  iso_td[445] = 444;
  iso_td[53] = 52;
  td_iso[512] = 513;
  td_iso[510] = 511;
  td_iso[514] = 515;
  td_iso[516] = 517;
  td_iso[518] = 519;
  td_iso[520] = 521;
  td_iso[522] = 523;
  td_iso[524] = 525;
  td_iso[528] = 529;
  td_iso[526] = 527;
  td_iso[530] = 531;
  td_iso[50] = 51;
  iso_td[513] = 512;
  iso_td[511] = 510;
  iso_td[515] = 514;
  iso_td[517] = 516;
  iso_td[519] = 518;
  iso_td[521] = 520;
  iso_td[523] = 522;
  iso_td[525] = 524;
  iso_td[529] = 528;
  iso_td[527] = 526;
  iso_td[531] = 530;
  iso_td[51] = 50;
  td_iso[2] = 3;
  td_iso[0] = 1;
  td_iso[4] = 5;
  td_iso[6] = 7;
  td_iso[8] = 9;
  td_iso[10] = 11;
  td_iso[12] = 13;
  td_iso[14] = 15;
  td_iso[16] = 17;
  td_iso[18] = 19;
  td_iso[22] = 23;
  td_iso[20] = 21;
  td_iso[72] = 73;
  iso_td[3] = 2;
  iso_td[1] = 0;
  iso_td[5] = 4;
  iso_td[7] = 6;
  iso_td[9] = 8;
  iso_td[11] = 10;
  iso_td[13] = 12;
  iso_td[15] = 14;
  iso_td[19] = 18;
  iso_td[17] = 16;
  iso_td[23] = 22;
  iso_td[21] = 20;
  iso_td[73] = 72;
  td_iso[314] = 315;
  td_iso[316] = 317;
  td_iso[318] = 319;
  td_iso[320] = 321;
  td_iso[322] = 323;
  td_iso[324] = 325;
  td_iso[326] = 327;
  td_iso[328] = 329;
  td_iso[330] = 331;
  td_iso[332] = 333;
  td_iso[334] = 335;
  iso_td[315] = 314;
  iso_td[317] = 316;
  iso_td[319] = 318;
  iso_td[321] = 320;
  iso_td[323] = 322;
  iso_td[325] = 324;
  iso_td[327] = 326;
  iso_td[329] = 328;
  iso_td[331] = 330;
  iso_td[333] = 332;
  iso_td[335] = 334;
  td_iso[602] = 603;
  td_iso[600] = 601;
  td_iso[604] = 605;
  td_iso[606] = 607;
  td_iso[608] = 609;
  td_iso[610] = 611;
  td_iso[618] = 619;
  td_iso[616] = 617;
  td_iso[614] = 615;
  td_iso[612] = 613;
  td_iso[620] = 621;
  td_iso[80] = 81;
  iso_td[603] = 602;
  iso_td[601] = 600;
  iso_td[605] = 604;
  iso_td[607] = 606;
  iso_td[609] = 608;
  iso_td[611] = 610;
  iso_td[619] = 618;
  iso_td[617] = 616;
  iso_td[615] = 614;
  iso_td[613] = 612;
  iso_td[621] = 620;
  iso_td[81] = 80;
  td_iso[624] = 625;
  td_iso[622] = 623;
  td_iso[626] = 627;
  td_iso[628] = 629;
  td_iso[630] = 631;
  td_iso[632] = 633;
  td_iso[640] = 641;
  td_iso[638] = 639;
  td_iso[636] = 637;
  td_iso[634] = 635;
  td_iso[642] = 643;
  td_iso[82] = 83;
  iso_td[625] = 624;
  iso_td[623] = 622;
  iso_td[627] = 626;
  iso_td[629] = 628;
  iso_td[631] = 630;
  iso_td[633] = 632;
  iso_td[641] = 640;
  iso_td[639] = 638;
  iso_td[637] = 636;
  iso_td[635] = 634;
  iso_td[643] = 642;
  iso_td[83] = 82;
  td_iso[490] = 491;
  td_iso[488] = 489;
  td_iso[492] = 493;
  td_iso[494] = 495;
  td_iso[496] = 497;
  td_iso[498] = 499;
  td_iso[506] = 507;
  td_iso[504] = 505;
  td_iso[502] = 503;
  td_iso[500] = 501;
  td_iso[508] = 509;
  td_iso[74] = 75;
  iso_td[491] = 490;
  iso_td[489] = 488;
  iso_td[493] = 492;
  iso_td[495] = 494;
  iso_td[497] = 496;
  iso_td[499] = 498;
  iso_td[507] = 506;
  iso_td[505] = 504;
  iso_td[503] = 502;
  iso_td[501] = 500;
  iso_td[509] = 508;
  iso_td[75] = 74;
  td_iso[184] = 185;
  td_iso[182] = 183;
  td_iso[186] = 187;
  td_iso[188] = 189;
  td_iso[190] = 191;
  td_iso[192] = 193;
  td_iso[200] = 201;
  td_iso[198] = 199;
  td_iso[196] = 197;
  td_iso[194] = 195;
  td_iso[202] = 203;
  td_iso[66] = 67;
  iso_td[185] = 184;
  iso_td[183] = 182;
  iso_td[187] = 186;
  iso_td[189] = 188;
  iso_td[191] = 190;
  iso_td[193] = 192;
  iso_td[201] = 200;
  iso_td[199] = 198;
  iso_td[197] = 196;
  iso_td[195] = 194;
  iso_td[203] = 202;
  iso_td[67] = 66;
  td_iso[554] = 555;
  td_iso[556] = 557;
  td_iso[558] = 559;
  td_iso[560] = 561;
  td_iso[562] = 563;
  td_iso[564] = 565;
  td_iso[566] = 567;
  td_iso[568] = 569;
  td_iso[570] = 571;
  td_iso[572] = 573;
  td_iso[574] = 575;
  td_iso[576] = 577;
  td_iso[56] = 57;
  iso_td[555] = 554;
  iso_td[557] = 556;
  iso_td[559] = 558;
  iso_td[561] = 560;
  iso_td[563] = 562;
  iso_td[565] = 564;
  iso_td[567] = 566;
  iso_td[569] = 568;
  iso_td[571] = 570;
  iso_td[573] = 572;
  iso_td[575] = 574;
  iso_td[577] = 576;
  iso_td[57] = 56;
  td_iso[896] = 900;
  td_iso[819] = 0x335;
  td_iso[820] = 0x336;
  td_iso[930] = 931;
  td_iso[789] = 788;
  td_iso[791] = 790;
  td_iso[793] = 792;
  td_iso[794] = 0x31A;
  td_iso[795] = 0x31A;
  td_iso[892] = 0x37B;
  td_iso[893] = 0x381;
  td_iso[894] = 0x382;
  td_iso[895] = 0x383;
  td_iso[804] = 803;
  td_iso[806] = 805;
  td_iso[808] = 807;
  td_iso[810] = 809;
  td_iso[812] = 811;
  td_iso[814] = 813;
  td_iso[816] = 815;
  td_iso[818] = 817;
  td_iso[128] = 129;
  td_iso[132] = 133;
  td_iso[134] = 135;
  td_iso[136] = 137;
  td_iso[138] = 139;
  td_iso[140] = 141;
  td_iso[142] = 143;
  td_iso[144] = 145;
  td_iso[146] = 147;
  td_iso[148] = 149;
  td_iso[150] = 151;
  td_iso[152] = 153;
  td_iso[124] = 125;
  td_iso[154] = 155;
  td_iso[156] = 157;
  td_iso[158] = 159;
  td_iso[126] = 127;
  td_iso[226] = 227;
  td_iso[776] = 0x30B;
  td_iso[777] = 0x30A;
  td_iso[962] = 963;
  td_iso[950] = 951;
  td_iso[905] = 906;
  td_iso[932] = 933;
  td_iso[934] = 935;
  td_iso[948] = 949;
  td_iso[903] = 904;
  td_iso[840] = 0x34A;
  td_iso[841] = 0x34B;
  td_iso[844] = 0x34F;
  td_iso[845] = 0x350;
  td_iso[846] = 849;
  td_iso[836] = 0x347;
  td_iso[837] = 0x346;
  td_iso[825] = 826;
  td_iso[861] = 862;
  td_iso[936] = 0x3AD;
  td_iso[937] = 0x3AE;
  td_iso[938] = 0x3AF;
  td_iso[939] = 0x3B0;
  td_iso[940] = 945;
  td_iso[774] = 775;
  td_iso[922] = 923;
  td_iso[867] = 0x366;
  td_iso[868] = 0x367;
  td_iso[869] = 872;
  td_iso[873] = 0x36C;
  td_iso[874] = 0x36D;
  td_iso[875] = 878;
  td_iso[879] = 0x372;
  td_iso[880] = 0x373;
  td_iso[881] = 884;
  td_iso[885] = 0x378;
  td_iso[886] = 0x379;
  td_iso[887] = 890;
  td_iso[901] = 902;
  td_iso[130] = 131;
  iso_td[897] = 893;
  iso_td[900] = 896;
  iso_td[821] = 0x333;
  iso_td[822] = 0x334;
  iso_td[931] = 930;
  iso_td[788] = 789;
  iso_td[790] = 791;
  iso_td[792] = 793;
  iso_td[794] = 795;
  iso_td[803] = 804;
  iso_td[805] = 806;
  iso_td[807] = 808;
  iso_td[809] = 810;
  iso_td[811] = 812;
  iso_td[813] = 814;
  iso_td[815] = 816;
  iso_td[817] = 818;
  iso_td[129] = 128;
  iso_td[133] = 132;
  iso_td[135] = 134;
  iso_td[137] = 136;
  iso_td[139] = 138;
  iso_td[141] = 140;
  iso_td[143] = 142;
  iso_td[145] = 144;
  iso_td[147] = 146;
  iso_td[149] = 148;
  iso_td[151] = 150;
  iso_td[153] = 152;
  iso_td[125] = 124;
  iso_td[155] = 154;
  iso_td[157] = 156;
  iso_td[159] = 158;
  iso_td[963] = 962;
  iso_td[951] = 950;
  iso_td[906] = 905;
  iso_td[933] = 932;
  iso_td[935] = 934;
  iso_td[949] = 948;
  iso_td[904] = 903;
  iso_td[842] = 0x348;
  iso_td[843] = 0x349;
  iso_td[847] = 0x34C;
  iso_td[848] = 0x34D;
  iso_td[849] = 846;
  iso_td[838] = 837;
  iso_td[826] = 825;
  iso_td[862] = 861;
  iso_td[941] = 0x3A8;
  iso_td[942] = 0x3A9;
  iso_td[943] = 0x3AA;
  iso_td[944] = 0x3AB;
  iso_td[898] = 0x382;
  iso_td[899] = 0x37F;
  iso_td[945] = 940;
  iso_td[775] = 774;
  iso_td[923] = 922;
  iso_td[870] = 0x363;
  iso_td[871] = 0x364;
  iso_td[872] = 869;
  iso_td[876] = 0x369;
  iso_td[877] = 0x36A;
  iso_td[878] = 875;
  iso_td[882] = 0x36F;
  iso_td[883] = 0x370;
  iso_td[884] = 881;
  iso_td[888] = 0x375;
  iso_td[889] = 0x376;
  iso_td[890] = 0x377;
  iso_td[891] = 0x37C;
  iso_td[778] = 0x309;
  iso_td[779] = 0x308;
  iso_td[902] = 901;
  iso_td[131] = 130;
  td_iso[120] = 121;
  iso_td[121] = 120;
  td_iso[114] = 115;
  iso_td[115] = 114;
  td_iso[110] = 111;
  iso_td[111] = 110;
  td_iso[102] = 103;
  iso_td[103] = 102;
  td_iso[104] = 105;
  iso_td[105] = 104;
  td_iso[106] = 107;
  iso_td[107] = 106;
  td_iso[108] = 109;
  iso_td[109] = 108;
  td_iso[100] = 101;
  iso_td[101] = 100;
  td_iso[98] = 99;
  iso_td[99] = 98;
  td_iso[46] = 47;
  iso_td[47] = 46;
  td_iso[952] = 953;
  iso_td[953] = 952;
  td_iso[954] = 955;
  iso_td[955] = 954;
  td_iso[956] = 957;
  iso_td[957] = 956;
  td_iso[958] = 959;
  iso_td[959] = 958;
  td_iso[960] = 961;
  iso_td[961] = 960;
  td_iso[946] = 947;
  iso_td[947] = 946;
}

void init_objects(void)
{
  long i;
  game.objects_config[1].light = 0;
  game.objects_config[1].field_B = 0x00;
  game.objects_config[1].field_C[0] = 0;
  game.objects_config[1].health = 100;
  game.objects_config[1].field_4 = 20;
  game.objects_config[1].field_5 = 0;
  game.objects_config[2].health = 100;
  game.objects_config[2].field_4 = 0;
  game.objects_config[2].field_5 = 1;
  game.objects_config[2].field_1A = 0;
  game.objects_config[2].field_8 = 1;
  game.objects_config[49].health = 100;
  game.objects_config[49].field_4 = 0;
  game.objects_config[49].field_5 = 1;
  game.objects_config[49].field_1A = 0;
  game.objects_config[49].field_8 = 1;
  game.objects_config[3].health = 100;
  game.objects_config[3].field_4 = 20;
  game.objects_config[4].health = 100;
  game.objects_config[4].field_4 = 20;
  game.objects_config[4].field_5 = 1;
  game.objects_config[4].field_1A = 0;
  game.objects_config[4].field_8 = 1;
  game.objects_config[5].health = 1;
  game.objects_config[2].light = 0x0600;
  game.objects_config[2].field_B = 0x32;
  game.objects_config[2].field_C[0] = 5;
  game.objects_config[5].field_4 = 20;
  game.objects_config[5].field_5 = 0;
  game.objects_config[5].field_1A = 1;
  game.objects_config[5].field_6 = 1;
  game.objects_config[5].field_8 = 1;
  game.objects_config[6].field_4 = 8;
  game.objects_config[6].health = 50;
  game.objects_config[7].health = 100;
  game.objects_config[7].field_4 = 0;
  game.objects_config[7].field_5 = 1;
  game.objects_config[7].field_8 = 1;
  game.objects_config[8].health = 100;
  game.objects_config[8].field_4 = 20;
  game.objects_config[8].field_5 = 1;
  game.objects_config[10].health = 1000;
  game.objects_config[10].field_4 = 9;
  game.objects_config[28].health = 100;
  game.objects_config[49].light = 0x0A00u;
  game.objects_config[49].field_B = 0x28;
  game.objects_config[49].field_C[0] = 5;
  game.objects_config[4].light = 0x0700u;
  game.objects_config[4].field_B = 0x2F;
  game.objects_config[4].field_C[0] = 5;
  game.objects_config[5].light = 0x0E00u;
  game.objects_config[5].field_B = 0x24;
  game.objects_config[5].field_C[0] = 5;
  game.objects_config[28].field_4 = 0;
  game.objects_config[28].field_5 = 1;
  game.objects_config[28].field_1A = 0;
  game.objects_config[28].field_8 = 1;
  game.objects_config[11].light = 0x0400u;
  game.objects_config[11].field_B = 0x3E;
  game.objects_config[11].field_C[0] = 0;
  game.objects_config[11].field_4 = 10;
  game.objects_config[11].field_5 = 0;
  game.objects_config[11].field_1A = 0;
  game.objects_config[11].field_8 = 1;
  game.objects_config[12].light = 0x0400u;
  game.objects_config[12].field_B = 0x3E;
  game.objects_config[12].field_C[0] = 0;
  game.objects_config[12].field_4 = 10;
  game.objects_config[12].field_5 = 0;
  game.objects_config[12].field_1A = 0;
  game.objects_config[12].field_8 = 1;
  game.objects_config[13].field_4 = 10;
  game.objects_config[13].field_5 = 0;
  game.objects_config[13].light = 0x0400u;
  game.objects_config[13].field_B = 0x3E;
  game.objects_config[13].field_C[0] = 0;
  game.objects_config[13].field_1A = 0;
  game.objects_config[13].field_8 = 1;
  game.objects_config[14].light = 0x0400u;
  game.objects_config[14].field_B = 0x3E;
  game.objects_config[14].field_C[0] = 0;
  game.objects_config[14].field_4 = 10;
  game.objects_config[14].field_5 = 0;
  game.objects_config[14].field_1A = 0;
  game.objects_config[14].field_8 = 1;
  game.objects_config[15].field_4 = 10;
  game.objects_config[15].field_5 = 0;
  game.objects_config[15].light = 0x0400u;
  game.objects_config[15].field_B = 0x3E;
  game.objects_config[15].field_C[0] = 0;
  game.objects_config[15].field_1A = 0;
  game.objects_config[15].field_8 = 1;
  game.objects_config[16].light = 0x0400u;
  game.objects_config[16].field_B = 0x3E;
  game.objects_config[16].field_C[0] = 0;
  game.objects_config[16].field_4 = 10;
  game.objects_config[16].field_5 = 0;
  game.objects_config[16].field_1A = 0;
  game.objects_config[16].field_8 = 1;
  game.objects_config[17].field_4 = 10;
  game.objects_config[17].field_5 = 0;
  game.objects_config[17].light = 0x0400u;
  game.objects_config[17].field_B = 0x3E;
  game.objects_config[17].field_C[0] = 0;
  game.objects_config[17].field_1A = 0;
  game.objects_config[17].field_8 = 1;
  game.objects_config[43].field_4 = 8;
  game.objects_config[43].health = 50;
  game.objects_config[28].light = 0x0600u;
  game.objects_config[28].field_B = 0x2E;
  game.objects_config[28].field_C[0] = 5;
  game.objects_config[18].field_4 = 10;
  game.objects_config[18].field_5 = 0;
  game.objects_config[18].light = 0x0400u;
  game.objects_config[18].field_B = 0x3E;
  game.objects_config[18].field_C[0] = 0;
  game.objects_config[18].field_1A = 0;
  game.objects_config[19].light = 0x0400u;
  game.objects_config[19].field_B = 0x3E;
  game.objects_config[19].field_C[0] = 0;
  game.objects_config[18].field_8 = 1;
  game.objects_config[19].field_4 = 10;
  game.objects_config[19].field_5 = 0;
  game.objects_config[20].light = 0x0400u;
  game.objects_config[20].field_B = 0x3E;
  game.objects_config[20].field_C[0] = 0;
  game.objects_config[19].field_1A = 0;
  game.objects_config[19].field_8 = 1;
  game.objects_config[20].field_4 = 10;
  game.objects_config[20].field_5 = 0;
  game.objects_config[20].field_1A = 0;
  game.objects_config[21].light = 0x0400u;
  game.objects_config[21].field_B = 0x3E;
  game.objects_config[21].field_C[0] = 0;
  game.objects_config[20].field_8 = 1;
  game.objects_config[21].field_4 = 10;
  game.objects_config[21].field_5 = 0;
  game.objects_config[22].light = 0x0400u;
  game.objects_config[22].field_B = 0x3E;
  game.objects_config[22].field_C[0] = 0;
  game.objects_config[21].field_1A = 0;
  game.objects_config[21].field_8 = 1;
  game.objects_config[22].field_4 = 10;
  game.objects_config[22].field_5 = 0;
  game.objects_config[22].field_1A = 0;
  game.objects_config[23].light = 0x0400u;
  game.objects_config[23].field_B = 0x3E;
  game.objects_config[23].field_C[0] = 0;
  game.objects_config[22].field_8 = 1;
  game.objects_config[23].field_4 = 10;
  game.objects_config[23].field_5 = 0;
  game.objects_config[45].light = 0x0400u;
  game.objects_config[45].field_B = 0x3E;
  game.objects_config[45].field_C[0] = 0;
  game.objects_config[23].field_1A = 0;
  game.objects_config[23].field_8 = 1;
  game.objects_config[45].field_4 = 10;
  game.objects_config[45].field_5 = 0;
  game.objects_config[45].field_1A = 0;
  game.objects_config[46].light = 0x0400u;
  game.objects_config[46].field_B = 0x3E;
  game.objects_config[46].field_C[0] = 0;
  game.objects_config[45].field_8 = 1;
  game.objects_config[46].field_4 = 10;
  game.objects_config[46].field_5 = 0;
  game.objects_config[47].light = 0x0400u;
  game.objects_config[47].field_B = 0x3E;
  game.objects_config[47].field_C[0] = 0;
  game.objects_config[46].field_1A = 0;
  game.objects_config[46].field_8 = 1;
  game.objects_config[47].field_4 = 10;
  game.objects_config[47].field_5 = 0;
  game.objects_config[47].field_1A = 0;
  game.objects_config[134].light = 0x0400u;
  game.objects_config[134].field_B = 0x3E;
  game.objects_config[134].field_C[0] = 0;
  game.objects_config[47].field_8 = 1;
  game.objects_config[134].field_4 = 10;
  game.objects_config[134].field_5 = 0;
  game.objects_config[134].field_1A = 0;
  game.objects_config[87].light = 0x0400u;
  game.objects_config[87].field_B = 0x3E;
  game.objects_config[87].field_C[0] = 0;
  game.objects_config[134].field_8 = 1;
  game.objects_config[87].field_4 = 10;
  game.objects_config[87].field_5 = 0;
  game.objects_config[88].light = 0x0400u;
  game.objects_config[88].field_B = 0x3E;
  game.objects_config[88].field_C[0] = 0;
  game.objects_config[87].field_1A = 0;
  game.objects_config[88].field_4 = 10;
  game.objects_config[88].field_5 = 0;
  game.objects_config[89].light = 0x0400u;
  game.objects_config[89].field_B = 0x3E;
  game.objects_config[89].field_C[0] = 0;
  game.objects_config[88].field_1A = 0;
  game.objects_config[89].field_4 = 10;
  game.objects_config[89].field_5 = 0;
  game.objects_config[90].light = 0x0400u;
  game.objects_config[90].field_B = 0x3E;
  game.objects_config[90].field_C[0] = 0;
  game.objects_config[89].field_1A = 0;
  game.objects_config[90].field_4 = 10;
  game.objects_config[90].field_5 = 0;
  game.objects_config[91].light = 0x0400u;
  game.objects_config[91].field_B = 0x3E;
  game.objects_config[91].field_C[0] = 0;
  game.objects_config[90].field_1A = 0;
  game.objects_config[91].field_4 = 10;
  game.objects_config[91].field_5 = 0;
  game.objects_config[92].light = 0x0400u;
  game.objects_config[92].field_B = 0x3E;
  game.objects_config[92].field_C[0] = 0;
  game.objects_config[91].field_1A = 0;
  game.objects_config[92].field_4 = 10;
  game.objects_config[92].field_5 = 0;
  game.objects_config[93].light = 0x0400u;
  game.objects_config[93].field_B = 0x3E;
  game.objects_config[93].field_C[0] = 0;
  game.objects_config[92].field_1A = 0;
  game.objects_config[93].field_4 = 10;
  game.objects_config[93].field_5 = 0;
  game.objects_config[86].light = 0x0400u;
  game.objects_config[86].field_B = 0x3E;
  game.objects_config[86].field_C[0] = 0;
  game.objects_config[93].field_1A = 0;
  game.objects_config[86].field_4 = 10;
  game.objects_config[86].field_5 = 0;
  game.objects_config[86].field_1A = 0;
  game.objects_config[109].field_7 = 1;
  game.objects_config[109].field_8 = 1;
  game.objects_config[94].field_8 = 1;
  game.objects_config[95].field_8 = 1;
  game.objects_config[96].field_8 = 1;
  game.objects_config[97].field_8 = 1;
  game.objects_config[98].field_8 = 1;
  game.objects_config[99].field_8 = 1;
  game.objects_config[106].field_8 = 1;
  game.objects_config[107].field_8 = 1;
  game.objects_config[108].field_8 = 1;
  game.objects_config[128].field_4 = 10;
  for (i=57; i <= 85; i++)
  {
    game.objects_config[i].field_8 = 1;
  }
  game.objects_config[126].field_8 = 1;
  game.objects_config[26].field_8 = 1;
  game.objects_config[27].field_8 = 1;
  game.objects_config[31].field_8 = 1;
  game.objects_config[32].field_8 = 1;
  game.objects_config[114].field_8 = 1;
  game.objects_config[115].field_8 = 1;
  game.objects_config[117].field_8 = 1;
  game.objects_config[116].field_8 = 1;
  game.objects_config[118].field_8 = 1;
  game.objects_config[119].field_8 = 1;
  game.objects_config[125].field_8 = 1;
}

void init_keeper(void)
{
  //_DK_init_keeper();
  engine_init();
  init_iso_3d_conversion_tables();
  init_objects();
  init_colours();
  init_spiral_steps();
  init_key_to_strings();
  load_stats_files();
  check_and_auto_fix_stats();
  init_creature_scores();
  load_cube_file();
  init_top_texture_to_cube_table();
  load_anim_file();
  game.field_14E497 = 5;
  game.field_14EA34 = 4;
  game.field_14EA38 = 200;
  game.field_14EA28 = 256;
  game.field_14EA2A = 256;
  game.field_14EA2C = 256;
  game.field_14EA2E = 256;
  if (game.generate_speed <= 0)
    game.generate_speed = game.default_generate_speed;
  poly_pool_end = &poly_pool[sizeof(poly_pool)-128];
  lbDisplay.GlassMap = ghost;
  game.field_1517F8 = 1;
  lbDisplay.DrawColour = colours[15][15][15];
  game.field_1517F7 = 0;
  game.field_1517F9 = 0;
  game.field_1517FA = 0;
  game.field_1517FB = 0;
  game.field_1517FC = 0;
  game.numfield_C |= 0x40;
  game.numfield_D |= 0x20 | 0x40;
  init_censorship();
}

short ceiling_set_info(long height_max, long height_min, long step)
{
  static const char *func_name="ceiling_set_info";
#if (BFDEBUG_LEVEL > 6)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  //return _DK_ceiling_set_info(height_max, height_min, step);
  long dist;
  if (step <= 0)
  {
    error(func_name, 42, "Illegal ceiling step value");
    return 0;
  }
  if (height_max > 15)
  {
    error(func_name, 46, "Max height is too high");
    return 0;
  }
  if (height_min > height_max)
  {
    error(func_name, 50, "Ceiling max height is smaller than min height");
    return 0;
  }
  dist = (height_max - height_min) / step;
  if ( dist >= 2500 )
    dist = 2500;
  game.field_14A80C = dist;
  if (dist > 20)
  {
    error(func_name, 58, "Ceiling search distance too big");
    return 0;
  }
  game.field_14A804 = height_max;
  game.field_14A808 = height_min;
  game.field_14A814 = step;
  game.field_14A810 = (2*game.field_14A80C+1) * (2*game.field_14A80C+1);
  return 1;
}

short load_settings(void)
{
  static const char *func_name="load_settings";
#if (BFDEBUG_LEVEL > 6)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  // CPU status variable
  struct CPU_INFO cpu_info;
  char *fname;
  long len;
  // Do only a very basic setup
  cpu_detect(&cpu_info);
  fname = prepare_file_path(FGrp_Save,"settings.dat");
  len = LbFileLengthRnc(fname);
  if (len == sizeof(struct GameSettings))
  {
    if (LbFileLoadAt(fname, &settings) == sizeof(struct GameSettings))
      return true;
  }
  LbMemoryCopy(&settings, &default_settings, sizeof(struct GameSettings));
  settings.field_B = get_next_vidmode(Lb_SCREEN_MODE_INVALID);
  if ((((cpu_info.feature_intl>>8) & 0x0Fu) >= 6) && (is_feature_on(Ft_HiResVideo)))
    settings.field_B = get_next_vidmode(settings.field_B);
  LbFileSaveAt(fname, &settings, sizeof(struct GameSettings));
  return false;
}

/*
 * Initial video setup - loads only most importand files to show startup screens.
 */
short initial_setup(void)
{
  static const char *func_name="initial_setup";
  // setting this will force video mode change, even if previous one is same
  MinimalResolutionSetup = true;
  if (LbDataLoadAll(game_load_files))
  {
    error(func_name, 499, "Unable to load game_load_files");
    return false;
  }
  // was LoadMcgaData, but minimal should be enough at this point.
  if ( !LoadMcgaDataMinimal() )
  {
    error(func_name, 689, "Load MCGA files failed");
    return false;
  }
  load_pointer_file(0);
  update_screen_mode_data(320, 200);
  clear_game();
  lbDisplay.DrawFlags |= 0x4000u;
  return true;
}

/*
 * Displays 'legal' screens, intro and initializes basic game data.
 * If true is returned, then all files needed for startup were loaded,
 * and there should be the loading screen visible.
 * @return Returns true on success, false on error which makes the
 *   gameplay impossible (usually files loading failure).
 * @note The current screen resolution at end of this function may vary.
 */
short setup_game(void)
{
  static const char *func_name="setup_game";
  // CPU status variable
  struct CPU_INFO cpu_info;
  char *fname;
  // Do only a very basic setup
  cpu_detect(&cpu_info);
  LbSyncLog("CPU %s features %08x %08x\n",cpu_info.vendor,cpu_info.feature_intl,cpu_info.feature_edx);
  update_memory_constraits();

  // Enable features thar require more resources
  update_features(mem_size);

  // Configuration file
  if ( !load_configuration() )
  {
      error(func_name, 1912, "Configuration load error.");
      return 0;
  }

  LbIKeyboardOpen();

  if (LbDataLoadAll(legal_load_files) != 0)
  {
      error(func_name, 1911, "Error on allocation/loading of legal_load_files.");
      return 0;
  }

  short result;
  short need_clean;

  // View the legal screen

  if (!setup_screen_mode_zero(get_frontend_vidmode()))
  {
      return 0;
  }

  result = init_actv_bitmap_screen(RBmp_SplashLegal);
  if ( result )
  {
      result = show_actv_bitmap_screen(3000);
      free_actv_bitmap_screen();
  } else
      LbSyncLog("%s: Legal image skipped\n", func_name);

  // Now do more setup
  // Prepare the Game structure
  clear_complete_game();
  // Moon phase calculation
  calculate_moon_phase(true,true);
  // Start the sound system
  if (!init_sound())
    LbWarnLog("Sound system disabled.\n");

  // View second splash screen
  result = init_actv_bitmap_screen(RBmp_SplashFx);
  if ( result )
  {
      result = show_actv_bitmap_screen(4000);
      free_actv_bitmap_screen();
  } else
      LbSyncLog("%s: startup_fx image skipped\n", func_name);
  draw_clear_screen();

  // View Bullfrog company logo animation when new moon
  if ( is_new_moon )
    if ( !game.no_intro )
    {
      fname = prepare_file_path(FGrp_FxData,"bullfrog.smk");
      result = play_smacker_file(fname, -2);
      if ( !result )
        error(func_name, 1483, "Unable to play new moon movie");
    }

  result = 1;
  // The 320x200 mode is required only for the intro;
  // loading and no CD screens can run in both 320x2?0 and 640x4?0.
  if ( result && (!game.no_intro) )
  {
    int mode_ok = LbScreenSetup(get_movies_vidmode(), 320, 200, _DK_palette, 2, 0);
    if (mode_ok != 1)
    {
      error(func_name, 1500, "Can't enter movies screen mode to play intro");
      result=0;
    }
  }

  if ( result )
  {
      draw_clear_screen();
      result = wait_for_cd_to_be_available();
  }
  if ( result && (!game.no_intro) )
  {
      fname = prepare_file_path(FGrp_LoData,"intromix.smk");
      result = play_smacker_file(fname, -2);
  }
  // Intro problems shouldn't force the game to quit,
  // so we're re-setting the result flag
  result = 1;

  if ( result )
  {
      display_loading_screen();
  }
  LbDataFreeAll(legal_load_files);

  if ( result )
  {
      _DK_IsRunningMark();
      if ( !initial_setup() )
        result = 0;
  }

  if ( result )
  {
    load_settings();
    if ( !setup_gui_strings_data() )
      result = 0;
  }

  if ( result )
  {
    if ( !setup_heaps() )
      result = 0;
  }

  if ( result )
  {
      init_keeper();
      if (cpu_info.feature_intl != 0)
      {
          if ( ((cpu_info.feature_intl>>8) & 0x0Fu) >= 0x06 )
            gpoly_enable_pentium_pro(true);
      }
      set_gamma(settings.gamma_correction, 0);
      SetRedbookVolume(settings.redbook_volume);
      SetSoundMasterVolume(settings.sound_volume);
      SetMusicMasterVolume(settings.sound_volume);
      setup_3d();
      setup_stuff();
      init_lookups();
      result = 1;
  }
  return result;
}

unsigned short input_eastegg_keycodes(unsigned char *counter,short allow,struct KeycodeString const *codes)
{
  int keys_total;
  TbKeyCode currkey;
  unsigned short result;
  if (!allow)
  {
    (*counter) = 0;
    return 0;
  }
  result = 0;
  if ((*counter) < codes->length)
  {
    currkey = codes->keys[(*counter)];
    if (lbKeyOn[currkey])
    {
      (*counter)++;
      result = 1;
      if ((*counter) > 2)
      {
        clear_key_pressed(currkey);
        result = 2;
      }
    }
  }
  if ((*counter) == codes->length)
  {
    if (result > 0)
      result = 3;
    else
      result = 4;
  }
  return result;
}

void input_eastegg(void)
{
  static const char *func_name="input_eastegg";
  short allow;
  unsigned short state;
  // Maintain the FECKOFF cheat
  allow = (lbKeyOn[KC_LSHIFT] != 0);
  state = input_eastegg_keycodes(&game.eastegg01_cntr,allow,&eastegg_feckoff_codes);
  if ((state == 2) || (state == 3))
    play_non_3d_sample(60);
  // Maintain the JLW cheat
  if (game.flags_font & 0x20)
  {
    allow = (lbKeyOn[KC_LSHIFT]) && (lbKeyOn[KC_RSHIFT]);
    state = input_eastegg_keycodes(&game.eastegg02_cntr,allow,&eastegg_jlw_codes);
    if ((state == 1) || (state == 2)  || (state == 3))
      play_non_3d_sample(159);
  }
  // Maintain the SKEKSIS cheat
  allow = (lbKeyOn[KC_LSHIFT] != 0);
  state = input_eastegg_keycodes(&eastegg03_cntr,allow,&eastegg_skeksis_codes);
  if (state == 3)
    output_message(94, 0, 1); //'Your pants are definitely too tight'
}

void init_messages(void)
{
  _DK_init_messages();
}

void zero_messages(void)
{
  int i;
  game.active_messages_count = 0;
  for (i=0; i<3; i++)
  {
    memset(&game.messages[i], 0, sizeof(struct GuiMessage));
  }
}

void battle_initialise(void)
{
  _DK_battle_initialise();
}

void maintain_my_battle_list(void)
{
  _DK_maintain_my_battle_list();
}

void add_thing_to_list(struct Thing *thing, struct StructureList *list)
{
  _DK_add_thing_to_list(thing, list);
}

struct Thing *allocate_free_thing_structure(unsigned char a1)
{
  return _DK_allocate_free_thing_structure(a1);
}

unsigned char i_can_allocate_free_thing_structure(unsigned char a1)
{
  return _DK_i_can_allocate_free_thing_structure(a1);
}

struct Thing *create_ambient_sound(struct Coord3d *pos, unsigned short model, unsigned short owner)
{
  static const char *func_name="create_ambient_sound";
  struct Thing *thing;
  if ( !i_can_allocate_free_thing_structure(1) )
  {
    error(func_name, 3698, "'Told You So' error! Cannot create ambient sound because there are too many fucking things allocated.");
    return NULL;
  }
  thing = allocate_free_thing_structure(1);
  thing->class_id = 12;
  thing->model = model;
  thing->field_1D = thing->field_1B;
  memcpy(&thing->mappos,pos,sizeof(struct Coord3d));
  thing->owner = owner;
  thing->field_4F |= 0x01;
  add_thing_to_list(thing, &game.thing_lists[9]);
  return thing;
}

/*
 * Checks if the given screen point is over a gui menu.
 * @param x,y Screen coordinates to check.
 * @return Returns index of the menu, or -1 if there's no menu on this point.
 */
int point_is_over_gui_menu(long x, long y)
{
  int idx;
  int gidx = -1;
  for(idx=0;idx<ACTIVE_MENUS_COUNT;idx++)
  {
    struct GuiMenu *gmnu;
    gmnu=&active_menus[idx];
    if (gmnu->field_1 != 2)
        continue;
    if (gmnu->flgfield_1D == 0)
        continue;
    short gx = gmnu->pos_x;
    if ((x >= gx) && (x < gx+gmnu->width))
    {
        short gy = gmnu->pos_y;
        if ((y >= gy) && (y < gy+gmnu->height))
            gidx=idx;
    }
  }
  return gidx;
}

int first_monopoly_menu(void)
{
  int idx;
  const short gmax=8;//sizeof(active_menus)/sizeof(struct GuiMenu);
  struct GuiMenu *gmnu;
  for (idx=0;idx<gmax;idx++)
  {
    gmnu=&active_menus[idx];
    if ((gmnu->field_1!=0) && (gmnu->flgfield_1E!=0))
        return idx;
  }
  return -1;
}

void update_busy_doing_gui_on_menu(void)
{
  int gidx;
  gidx = point_is_over_gui_menu(GetMouseX(), GetMouseY());
  if (gidx == -1)
    busy_doing_gui = 0;
  else
    busy_doing_gui = 1;
}

TbBool gui_slider_button_inputs(int gbtn_idx)
{
  Gf_Btn_Callback callback;
  int mouse_x;
  int slide_start,slide_end;
  struct GuiButton *gbtn;
  if (gbtn_idx < 0)
    return false;
  gbtn = &active_buttons[gbtn_idx];
  mouse_x = GetMouseX();
  gbtn->field_1 = 1;
  slide_start = gbtn->pos_x+32;
  slide_end = gbtn->pos_x+gbtn->width-32;
  if (mouse_x < slide_start)
  {
    gbtn->slide_val = 0;
  } else
  if (mouse_x >= slide_end)
  {
    gbtn->slide_val = 255;
  } else
  if (gbtn->width > 64)
  {
    gbtn->slide_val = ((mouse_x-slide_start) << 8) / (gbtn->width-64);
  } else
  {
    gbtn->slide_val = ((mouse_x-gbtn->pos_x) << 8) / (gbtn->width+1);
  }
  *gbtn->field_33 = (gbtn->slide_val) * (gbtn->field_2D+1) >> 8;
  callback = gbtn->click_event;
  if (callback != NULL)
    callback(gbtn);
  return true;
}

TbBool gui_button_click_inputs(int gmbtn_idx)
{
  TbBool result;
  struct GuiButton *gbtn;
  if (gmbtn_idx < 0)
    return false;
  result = false;
  gbtn = &active_buttons[gmbtn_idx];
  Gf_Btn_Callback callback;
  if (lbDisplay.MLeftButton)
  {
      result = true;
      callback = gbtn->click_event;
      if ((callback != NULL) || (((gbtn->field_0 & 2)!=0) ||
         (gbtn->field_2F != 0) || (gbtn->gbtype == Lb_RADIOBTN)))
        if ((gbtn->field_0 & 0x08) != 0)
        {
          switch (gbtn->gbtype)
          {
          case 1:
            if ((gbtn->field_1 > 5) && (callback != NULL))
              callback(gbtn);
            else
              gbtn->field_1++;
            break;
          case 6:
            if (callback != NULL)
              callback(gbtn);
            break;
          }
        }
  } else
  if (lbDisplay.MRightButton)
  {
      result = true;
      callback = gbtn->rclick_event;
      if ((callback != NULL) && ((gbtn->field_0 & 8)!=0))
      {
        switch (gbtn->gbtype)
        {
        case 1:
          if ( (gbtn->field_2>5) && (callback!=NULL) )
            callback(gbtn);
          else
            gbtn->field_2++;
          break;
        case 6:
          if (callback!=NULL)
            callback(gbtn);
          break;
        }
      }
  }
  if ( left_button_clicked )
  {
      result = true;
      if (game.field_1516F3 != 0)
      {
        if (gbtn->id_num == game.field_1516F3)
          game.field_1516F3 = 0;
      }
      callback = gbtn->click_event;
      if ((callback != NULL) || (gbtn->field_0 & 0x02) ||
         (gbtn->field_2F) || (gbtn->gbtype == Lb_RADIOBTN))
      {
        left_button_clicked = 0;
        gui_last_left_button_pressed_id = gbtn->id_num;
        do_button_click_actions(gbtn, &gbtn->field_1, callback);
      }
  } else
  if ( right_button_clicked )
  {
      result = true;
      if (game.field_1516F3 != 0)
      {
        if (gbtn->id_num == game.field_1516F3)
          game.field_1516F3 = 0;
      }
      callback = gbtn->rclick_event;
      if ((callback != NULL))
      {
        right_button_clicked = 0;
        gui_last_right_button_pressed_id = gbtn->id_num;
        do_button_click_actions(gbtn, &gbtn->field_2, callback);
      }
  }
  return result;
}

void gui_clear_buttons_not_over_mouse(int gmbtn_idx)
{
  struct GuiButton *gbtn;
  int gidx;
  for (gidx=0;gidx<ACTIVE_BUTTONS_COUNT;gidx++)
  {
    gbtn = &active_buttons[gidx];
    if (gbtn->field_0 & 0x01)
      if ( ((gmbtn_idx == -1) || (gmbtn_idx != gidx)) &&
           (gbtn->gbtype != Lb_RADIOBTN) && (gbtn != input_button) )
      {
        set_flag_byte(&gbtn->field_0,0x10,false);
        gbtn->field_1 = 0;
        gbtn->field_2 = 0;
      }
  }
}

TbBool gui_button_release_inputs(int gmbtn_idx)
{
  static const char *func_name="gui_button_release_inputs";
  struct GuiButton *gbtn;
#if (BFDEBUG_LEVEL > 7)
  LbSyncLog("%s: Starting\n", func_name);
#endif
  if (gmbtn_idx < 0)
    return false;
  Gf_Btn_Callback callback;
  gbtn = &active_buttons[gmbtn_idx%ACTIVE_BUTTONS_COUNT];
  if ((gbtn->field_1) && (left_button_released))
  {
    callback = gbtn->click_event;
    if ((callback != NULL) || ((gbtn->field_0 & 0x02) != 0) ||
        (gbtn->field_2F != 0) || (gbtn->gbtype == Lb_RADIOBTN))
    {
      left_button_released = 0;
      do_button_release_actions(gbtn, &gbtn->field_1, callback);
    }
    return true;
  }
  if ((gbtn->field_2) && (right_button_released))
  {
    callback = gbtn->rclick_event;
    if (callback != NULL)
    {
      right_button_released = 0;
      do_button_release_actions(gbtn, &gbtn->field_2, callback);
    }
    return true;
  }
  return false;
}

short get_gui_inputs(short gameplay_on)
{
  static const char *func_name="get_gui_inputs";
  static char over_slider_button=-1;
#if (BFDEBUG_LEVEL > 7)
  LbSyncLog("%s: Starting\n", func_name);
#endif
  //return _DK_get_gui_inputs(gameplay_on);
  update_breed_activities();
  battle_creature_over = 0;
  gui_room_type_highlighted = -1;
  gui_door_type_highlighted = -1;
  gui_trap_type_highlighted = -1;
  gui_creature_type_highlighted = -1;
  if (gameplay_on)
    maintain_my_battle_list();
  if (!lbDisplay.MLeftButton)
  {
    drag_menu_x = -999;
    drag_menu_y = -999;
    int idx;
    for (idx=0; idx < ACTIVE_BUTTONS_COUNT; idx++)
    {
      struct GuiButton *gbtn = &active_buttons[idx];
      if ((gbtn->field_0 & 0x01) && (gbtn->gbtype == 6))
          gbtn->field_1 = 0;
    }
  }
  update_busy_doing_gui_on_menu();

  struct PlayerInfo *player;
  int fmmenu_idx;
  int gmbtn_idx;
  int gidx;
  fmmenu_idx = first_monopoly_menu();
  player = &(game.players[my_player_number%PLAYERS_COUNT]);
  gmbtn_idx = -1;
  struct GuiButton *gbtn;
  // Sweep through buttons
  for (gidx=0; gidx<ACTIVE_BUTTONS_COUNT; gidx++)
  {
    gbtn = &active_buttons[gidx];
    if ((gbtn->field_0 & 0x01) == 0)
      continue;
    if (!active_menus[gbtn->gmenu_idx].flgfield_1D)
      continue;
    Gf_Btn_Callback callback;
    callback = gbtn->field_17;
    if (callback != NULL)
      callback(gbtn);
    if (((gbtn->field_1B & 0x4000u) != 0) || mouse_is_over_small_map(player->mouse_x,player->mouse_y))
      continue;
    if (check_if_mouse_is_over_button(gbtn) && (!game_is_busy_doing_gui_string_input())
      || (gbtn->gbtype == 6) && (gbtn->field_1 != 0))
    {
      if ((fmmenu_idx==-1) || (gbtn->gmenu_idx == fmmenu_idx))
      {
        gmbtn_idx = gidx;
        set_flag_byte(&gbtn->field_0,0x10,true);
        busy_doing_gui = 1;
        callback = gbtn->field_F;
        if (callback != NULL)
          callback(gbtn);
        if (gbtn->gbtype == 6)
          break;
        if (gbtn->gbtype != Lb_SLIDER)
          over_slider_button = -1;
      } else
      {
        set_flag_byte(&gbtn->field_0,0x10,false);
      }
    } else
    {
      set_flag_byte(&gbtn->field_0,0x10,false);
    }
    if (gbtn->gbtype == Lb_SLIDER)
    {
      int mouse_x;
      int mouse_y;
      int btnsize;
      mouse_x = GetMouseX();
      btnsize = gbtn->scr_pos_x + ((gbtn->slide_val)*(gbtn->width-64) >> 8);
      if ((mouse_x>(btnsize+22)) && (mouse_x<=(btnsize+44)))
      {
        mouse_y = GetMouseY();
        if ((mouse_y>gbtn->pos_y) && (mouse_y<=(gbtn->pos_y+gbtn->height)))
        {
          if ( left_button_clicked )
          {
            left_button_clicked = 0;
            over_slider_button = gidx;
            do_sound_menu_click();
          }
        }
      }
    }
  }  // end for

  short result = 0;
  if (game_is_busy_doing_gui_string_input())
  {
    busy_doing_gui = 1;
    if (get_button_area_input(input_button,input_button->id_num) != 0)
        result = 1;
  }
  if ((over_slider_button != -1) && (left_button_released))
  {
      left_button_released = 0;
      if (gmbtn_idx!=-1)
        active_buttons[gmbtn_idx].field_1 = 0;
      over_slider_button = -1;
      do_sound_menu_click();
  }

  gui_button_tooltip_update(gmbtn_idx);
  if (gui_slider_button_inputs(over_slider_button))
    return true;
  result |= gui_button_click_inputs(gmbtn_idx);
  gui_clear_buttons_not_over_mouse(gmbtn_idx);
  result |= gui_button_release_inputs(gmbtn_idx);
  input_gameplay_tooltips(gameplay_on);
  return result;
}

void increase_level(struct PlayerInfo *player)
{
  static const char *func_name="increase_level";
  struct Dungeon *dungeon;
  struct Thing *thing;
  struct CreatureControl *cctrl;
  long i,k;
  dungeon = &(game.dungeon[player->field_2B%DUNGEONS_COUNT]);

  k = 0;
  i = dungeon->field_2D;
  while (i > 0)
  {
    thing = thing_get(i);
    if (thing_is_invalid(thing))
      break;
    creature_increase_level(thing);
    cctrl = creature_control_get_from_thing(thing);
    if (creature_control_invalid(cctrl))
      break;
    i = cctrl->thing_idx;
    k++;
    if (k > CREATURES_COUNT)
    {
      error(func_name,4519,"Infinite loop detected when sweeping creatures list");
      break;
    }
  }

  k = 0;
  i = dungeon->field_2F;
  while (i > 0)
  {
    thing = thing_get(i);
    if (thing_is_invalid(thing))
      break;
    creature_increase_level(thing);
    cctrl = creature_control_get_from_thing(thing);
    if (creature_control_invalid(cctrl))
      break;
    i = cctrl->thing_idx;
    k++;
    if (k > CREATURES_COUNT)
    {
      error(func_name,4518,"Infinite loop detected when sweeping creatures list");
      break;
    }
  }
}

short is_my_player(struct PlayerInfo *player)
{
  struct PlayerInfo *myplyr;
  myplyr = &game.players[my_player_number%PLAYERS_COUNT];
  return (player == myplyr);
}

void start_resurrect_creature(struct PlayerInfo *player, struct Thing *thing)
{
  struct Dungeon *dungeon;
  dungeon = &(game.dungeon[player->field_2B%DUNGEONS_COUNT]);
  if (dungeon->field_139F != 0)
  {
    if (is_my_player(player))
    {
      dungeon_special_selected = thing->field_1B;
      resurrect_creature_scroll_offset = 0;
      turn_off_menu(GMnu_DUNGEON_SPECIAL);
      turn_on_menu(GMnu_RESURRECT_CREATURE);
    }
  }
}

void start_transfer_creature(struct PlayerInfo *player, struct Thing *thing)
{
  struct Dungeon *dungeon;
  dungeon = &(game.dungeon[player->field_2B%DUNGEONS_COUNT]);
  if (dungeon->field_919 != 0)
  {
    if (is_my_player(player))
    {
      dungeon_special_selected = thing->field_1B;
      transfer_creature_scroll_offset = 0;
      turn_off_menu(GMnu_DUNGEON_SPECIAL);
      turn_on_menu(GMnu_TRANSFER_CREATURE);
    }
  }
}

short engine_point_to_map(struct Camera *camera, long screen_x, long screen_y, long *map_x, long *map_y)
{
  struct PlayerInfo *player;
  player = &(game.players[my_player_number%PLAYERS_COUNT]);
  *map_x = 0;
  *map_y = 0;
  if ( (pointer_x >= 0) && (pointer_y >= 0)
    && (pointer_x < (player->engine_window_width/pixel_size))
    && (pointer_y < (player->engine_window_height/pixel_size)) )
  {
    int i;
    i = player->work_state;
    if ( ((i == 1) && (player->field_454)) || (i == 2) ||
      (i == 18) || (i == 16) || (i == KSt_SightOfEvil) || (i == 23) )
    {
          *map_x = ( top_pointed_at_x << 8) + top_pointed_at_frac_x;
          *map_y = ( top_pointed_at_y << 8) + top_pointed_at_frac_y;
    } else
    {
          *map_x = (block_pointed_at_x << 8) + pointed_at_frac_x;
          *map_y = (block_pointed_at_y << 8) + pointed_at_frac_y;
    }
    // Clipping coordinates
    if ( *map_y < 0 )
      *map_y = 0;
    else
    if ( *map_y > 0xFEFFu )
      *map_y = 0xFEFFu;
    if ( *map_x < 0 )
      *map_x = 0;
    else
    if ( *map_x > 0xFEFFu )
      *map_x = 0xFEFFu;
    return true;
  }
  return false;
}

void restore_computer_player_after_load(void)
{
  _DK_restore_computer_player_after_load();
}

short point_to_overhead_map(struct Camera *camera, long screen_x, long screen_y, long *map_x, long *map_y)
{
  struct PlayerInfo *player;
  player = &(game.players[my_player_number%PLAYERS_COUNT]);
  *map_x = 0;
  *map_y = 0;
  if ((screen_x >= 150) && (screen_x < 490)
    && (screen_y >= 56) && (screen_y < 396))
  {
    *map_x = 3*256 * (screen_x-150) / 4 + 384;
    *map_y = 3*256 * (screen_y-56) / 4 + 384;
    return ((*map_x >= 0) && (*map_x < (map_subtiles_x+1)<<8) && (*map_y >= 0) && (*map_y < (map_subtiles_y+1)<<8));
  }
  return false;
}

short screen_to_map(struct Camera *camera, long screen_x, long screen_y, struct Coord3d *mappos)
{
//return _DK_screen_to_map(camera,screen_x,screen_y,mappos);
  short result;
  long x,y;
  if (camera != NULL)
  {
    switch (camera->field_6)
    {
      case 1:
      case 2:
      case 5:
        // 3D view mode
        result = engine_point_to_map(camera,screen_x,screen_y,&x,&y);
        break;
      case 3: //map mode
        result = point_to_overhead_map(camera,screen_x,screen_y,&x,&y);
        break;
      default:
        result = false;
        break;
    }
  }
  if ( result )
  {
    mappos->x.val = x;
    mappos->y.val = y;
  }
  if ( mappos->x.val > 0xFEFFu )
    mappos->x.val = 0xFEFFu;
  if ( mappos->y.val > 0xFEFFu )
    mappos->y.val = 0xFEFFu;
  return result;
}

void update_breed_activities(void)
{
  _DK_update_breed_activities();
}

short save_settings(void)
{
  static const char *func_name="save_settings";
  char *fname;
  fname=prepare_file_path(FGrp_Save,"settings.dat");
  LbFileSaveAt(fname, &settings, sizeof(struct GameSettings));
  return true;
}

void toggle_hero_health_flowers(void)
{
  const char *statstr;
  toggle_flag_byte(&game.flags_cd,0x80);
  if (game.flags_cd & 0x80)
  {
    statstr = "off";
  } else
  {
    do_sound_menu_click();
    statstr = "on";
  }
  show_onscreen_msg(2*game.num_fps, "Hero health flowers %s", statstr);
}

void zoom_to_map(void)
{
  struct PlayerInfo *player;
  turn_off_all_window_menus();
  if ((game.numfield_C & 0x20) == 0)
    set_flag_byte(&game.numfield_C,0x40,false);
  else
    set_flag_byte(&game.numfield_C,0x40,true);
  player=&(game.players[my_player_number%PLAYERS_COUNT]);
  if ((game.numfield_A & 0x01) || (lbDisplay.PhysicalScreenWidth > 320))
  {
    if (!toggle_status_menu(false))
      set_flag_byte(&game.numfield_C,0x40,false);
    set_players_packet_action(player, 119, 4, 0, 0, 0);
    turn_off_roaming_menus();
  } else
  {
    set_players_packet_action(player, 80, 5, 0, 0, 0);
    turn_off_roaming_menus();
  }
}

void zoom_from_map(void)
{
  struct PlayerInfo *player;
  player=&(game.players[my_player_number%PLAYERS_COUNT]);
  if (((game.numfield_A & 0x01) != 0) || (lbDisplay.PhysicalScreenWidth > 320))
  {
      if ((game.numfield_C & 0x40) != 0)
        toggle_status_menu(true);
      set_players_packet_action(player, 120,1,0,0,0);
  } else
  {
      set_players_packet_action(player, 80,6,0,0,0);
  }
}

void setup_engine_window(long x, long y, long width, long height)
{
  static const char *func_name="setup_engine_window";
  struct PlayerInfo *player;
#if (BFDEBUG_LEVEL > 6)
    LbSyncLog("%s: Setting coords (%d,%d) dimensions (%d,%d)\n",func_name,x,y,width,height);
#endif
  player=&(game.players[my_player_number%PLAYERS_COUNT]);
  if (game.numfield_C & 0x20)
  {
    if (x > MyScreenWidth)
      x = MyScreenWidth;
    if (x < status_panel_width)
      x = status_panel_width;
  } else
  {
    if (x > MyScreenWidth)
      x = MyScreenWidth;
    if (x < 0)
      x = 0;
  }
  if (y > MyScreenHeight)
    y = MyScreenHeight;
  if (y < 0)
    y = 0;
  if (x+width > MyScreenWidth)
    width = MyScreenWidth-x;
  if (width < 0)
    width = 0;
  if (y+height > MyScreenHeight)
    height = MyScreenHeight-y;
  if (height < 0)
    height = 0;
  player->engine_window_x = x;
  player->engine_window_y = y;
  player->engine_window_width = width;
  player->engine_window_height = height;
}

void store_engine_window(struct TbGraphicsWindow *ewnd,int divider)
{
  struct PlayerInfo *player;
  player=&(game.players[my_player_number%PLAYERS_COUNT]);
  if (divider <= 1)
  {
    ewnd->x = player->engine_window_x;
    ewnd->y = player->engine_window_y;
    ewnd->width = player->engine_window_width;
    ewnd->height = player->engine_window_height;
  } else
  {
    ewnd->x = player->engine_window_x/divider;
    ewnd->y = player->engine_window_y/divider;
    ewnd->width = player->engine_window_width/divider;
    ewnd->height = player->engine_window_height/divider;
  }
}

void load_engine_window(struct TbGraphicsWindow *ewnd)
{
  struct PlayerInfo *player;
  player=&(game.players[my_player_number%PLAYERS_COUNT]);
  player->engine_window_x = ewnd->x;
  player->engine_window_y = ewnd->y;
  player->engine_window_width = ewnd->width;
  player->engine_window_height = ewnd->height;
}

struct Thing *get_nearest_thing_for_hand_or_slap(unsigned char plyr_idx, long x, long y)
{
  return _DK_get_nearest_thing_for_hand_or_slap(plyr_idx, x, y);
}

long creature_instance_is_available(struct Thing *thing, long inum)
{
  struct CreatureControl *cctrl;
  cctrl = creature_control_get_from_thing(thing);
  if (creature_control_invalid(cctrl))
    return 0;
  return cctrl->instances[inum];
}

struct Thing *create_creature(struct Coord3d *pos, unsigned short a1, unsigned short a2)
{
  return _DK_create_creature(pos, a1, a2);
}

void creature_increase_level(struct Thing *thing)
{
  _DK_creature_increase_level(thing);
}

void remove_events_thing_is_attached_to(struct Thing *thing)
{
  _DK_remove_events_thing_is_attached_to(thing);
}

int get_spell_overcharge_level(struct PlayerInfo *player)
{
  int i;
  i = (player->field_4D2 >> 2);
  if (i >= MAGIC_OVERCHARGE_LEVELS)
    return MAGIC_OVERCHARGE_LEVELS-1;
  return i;
}

short update_spell_overcharge(struct PlayerInfo *player, int spl_idx)
{
  struct Dungeon *dungeon;
  struct MagicStats *mgstat;
  int i;
  dungeon = &(game.dungeon[player->field_2B%DUNGEONS_COUNT]);
  mgstat = &(game.magic_stats[spl_idx%SPELL_TYPES_COUNT]);
  i = (player->field_4D2+1) >> 2;
  if (i >= MAGIC_OVERCHARGE_LEVELS)
    i = MAGIC_OVERCHARGE_LEVELS-1;
  if (mgstat->cost[i] <= dungeon->field_AF9)
  {
    // If we have more money, increase overcharge
    player->field_4D2++;
  } else
  {
    // If we don't have money, decrease the charge
    while (mgstat->cost[i] > dungeon->field_AF9)
    {
      i--;
      if (i < 0) break;
    }
    if (i >= 0)
      player->field_4D2 = (i << 2) + 1;
    else
      player->field_4D2 = 0;
  }
  return (i < (MAGIC_OVERCHARGE_LEVELS-1));
}

long take_money_from_dungeon(short a1, long a2, unsigned char a3)
{
  return _DK_take_money_from_dungeon(a1, a2, a3);
}

unsigned long steal_hero(struct PlayerInfo *player, struct Coord3d *pos)
{
  return _DK_steal_hero(player, pos);
}

void make_safe(struct PlayerInfo *player)
{
  _DK_make_safe(player);
}

void reset_gui_based_on_player_mode(void)
{
  _DK_reset_gui_based_on_player_mode();
}

void sound_reinit_after_load(void)
{
  _DK_sound_reinit_after_load();
}

void reinit_tagged_blocks_for_player(unsigned char idx)
{
  _DK_reinit_tagged_blocks_for_player(idx);
}

TbBool load_stats_files(void)
{
  static const char *func_name="load_stats_files";
  int i;
  TbBool result;
  // Hack to make our shot function work - remove when it's not needed
  instance_info[1].func_cb = instf_creature_fire_shot;
  instance_info[2].func_cb = instf_creature_fire_shot;
  instance_info[4].func_cb = instf_creature_fire_shot;
  result = true;
  clear_research_for_all_players();
  if (!load_terrain_config(keeper_terrain_file,0))
    result = false;
  if (!load_lenses_config(keeper_lenses_file,0))
    result = false;
  if (!load_magic_config(keeper_magic_file,0))
    result = false;
  if (!load_creaturetypes_config(keeper_creaturetp_file,0))
    result = false;
  // note that rules file requires definitions of magic and creature types
  if (!load_rules_config(keeper_rules_file,0))
    result = false;
  for (i=0; i < crtr_conf.kind_count; i++)
  {
    if (!load_creaturemodel_config(i+1,0))
      result = false;
  }
  game.field_149E7B = game.tile_strength;
//LbFileSaveAt("!stat11", &game, sizeof(struct Game));
//LbFileSaveAt("!stat12", &shot_stats, sizeof(shot_stats));
//LbFileSaveAt("!stat13", &instance_info, sizeof(instance_info));
  return result;
}


void check_and_auto_fix_stats(void)
{
  _DK_check_and_auto_fix_stats();
}

long update_dungeon_scores(void)
{
  return _DK_update_dungeon_scores();
}

long update_dungeon_generation_speeds(void)
{
  return _DK_update_dungeon_generation_speeds();
}

void calculate_dungeon_area_scores(void)
{
  _DK_calculate_dungeon_area_scores();
}

void check_map_for_gold(void)
{
  _DK_check_map_for_gold();
}

void setup_computer_players2(void)
{
  struct PlayerInfo *player;
  int i;
  check_map_for_gold();
  for (i=0; i < COMPUTER_TASKS_COUNT; i++)
  {
    LbMemorySet(&game.computer_task[i], 0, sizeof(struct ComputerTask));
  }
  for (i=0; i < PLAYERS_COUNT; i++)
  {
    player = &(game.players[i]);
    if (player->field_0 & 0x01)
    {
      if (player->field_2C == 1)
      {
        setup_a_computer_player(i, 7);
      }
    }
  }
}

void gui_set_button_flashing(long a1, long a2)
{
  game.field_1516F3 = a1;
  game.field_1516F7 = a2;
}

long get_next_research_item(struct Dungeon *dungeon)
{
  return _DK_get_next_research_item(dungeon);
}

void instant_instance_selected(long a1)
{
  _DK_instant_instance_selected(a1);
}

unsigned char active_battle_exists(unsigned char a1)
{
  return _DK_active_battle_exists(a1);
}

unsigned char step_battles_forward(unsigned char a1)
{
  return _DK_step_battles_forward(a1);
}

short zoom_to_fight(unsigned char a1)
{
  struct PlayerInfo *player;
  struct Dungeon *dungeon;
  player = &(game.players[my_player_number%PLAYERS_COUNT]);
  if (active_battle_exists(a1))
  {
    dungeon = &(game.dungeon[my_player_number%DUNGEONS_COUNT]);
    set_players_packet_action(player, 104, dungeon->field_1174, 0, 0, 0);
    step_battles_forward(a1);
    return true;
  }
  return false;
}

short create_random_evil_creature(long x, long y, unsigned short owner, long max_lv)
{
  static const char *func_name="create_random_evil_creature";
  struct Thing *thing;
  struct Coord3d pos;
  long i;
  i = seed_check_random(17, &game.field_14BB4A, func_name, 24597) + 14;
  pos.x.val = x;
  pos.y.val = y;
  pos.z.val = 0;
  thing = create_creature(&pos, i, owner);
  if (thing_is_invalid(thing))
  {
    LbErrorLog("Cannot create evil creature type %ld at (%ld,%ld)\n",i,x,y);
    return false;
  }
  pos.z.val = get_thing_height_at(thing, &pos);
  if (thing_in_wall_at(thing, &pos))
  {
    delete_thing_structure(thing, 0);
    LbErrorLog("Evil creature type %ld at (%ld,%ld) deleted because is in wall\n",i,x,y);
    return false;
  }
  thing->mappos.x.val = pos.x.val;
  thing->mappos.y.val = pos.y.val;
  thing->mappos.z.val = pos.z.val;
  remove_first_creature(thing);
  set_first_creature(thing);
  set_start_state(thing);
  i = seed_check_random(max_lv, &game.field_14BB4A, func_name, 24741);
  set_creature_level(thing, i);
  return true;
}

short create_random_hero_creature(long x, long y, unsigned short owner, long max_lv)
{
  static const char *func_name="create_random_hero_creature";
  struct Thing *thing;
  struct Coord3d pos;
  long i;
  i = seed_check_random(13, &game.field_14BB4A, func_name, 45671) + 1;
  pos.x.val = x;
  pos.y.val = y;
  pos.z.val = 0;
  thing = create_creature(&pos, i, owner);
  if (thing_is_invalid(thing))
  {
    LbErrorLog("Cannot create hero creature type %ld at (%ld,%ld)\n",i,x,y);
    return false;
  }
  pos.z.val = get_thing_height_at(thing, &pos);
  if (thing_in_wall_at(thing, &pos))
  {
    delete_thing_structure(thing, 0);
    LbErrorLog("Hero creature type %ld at (%ld,%ld) deleted because is in wall\n",i,x,y);
    return false;
  }
  thing->mappos.x.val = pos.x.val;
  thing->mappos.y.val = pos.y.val;
  thing->mappos.z.val = pos.z.val;
  remove_first_creature(thing);
  set_first_creature(thing);
//  set_start_state(thing); - simplified to the following two commands
  game.field_14E498 = game.play_gameturn;
  game.field_14E49C++;
  i = seed_check_random(max_lv, &game.field_14BB4A, func_name, 24741);
  set_creature_level(thing, i);
  return true;
}

short zoom_to_next_annoyed_creature(void)
{
  struct PlayerInfo *player;
  struct Dungeon *dungeon;
  struct Thing *thing;
  player = &(game.players[my_player_number%PLAYERS_COUNT]);
  dungeon = &(game.dungeon[my_player_number%DUNGEONS_COUNT]);
  dungeon->field_1177 = find_next_annoyed_creature(player->field_2B,dungeon->field_1177);
  thing = thing_get(dungeon->field_1177);
  if (thing_is_invalid(thing))
  {
    return false;
  }
  set_players_packet_action(player, 87, thing->mappos.x.val, thing->mappos.y.val, 0, 0);
  return true;
}

void go_to_my_next_room_of_type(unsigned long rkind)
{
  _DK_go_to_my_next_room_of_type(rkind);
}

/*
 * Returns the loaded level number.
 */
LevelNumber get_loaded_level_number(void)
{
  return game.loaded_level_number;
}

/*
 * Sets the loaded level number. Does not make any cleanup or loading.
 */
LevelNumber set_loaded_level_number(LevelNumber lvnum)
{
  if (lvnum > 0)
    game.loaded_level_number = lvnum;
  return game.loaded_level_number;
}

/*
 * Returns the continue level number.
 */
LevelNumber get_continue_level_number(void)
{
  return game.continue_level_number;
}

/*
 * Sets the continue level number. The level informs of campaign progress.
 * Levels which are not part of campaign will be ignored.
 */
LevelNumber set_continue_level_number(LevelNumber lvnum)
{
  if (is_singleplayer_like_level(lvnum))
    game.continue_level_number = lvnum;
  return game.continue_level_number;
}

/*
 * Returns the selected level number. Selected level is loaded when staring game.
 */
LevelNumber get_selected_level_number(void)
{
  return game.selected_level_number;
}

/*
 * Sets the selected level number. Selected level is loaded when staring game.
 */
LevelNumber set_selected_level_number(LevelNumber lvnum)
{
  if (lvnum > 0)
    game.selected_level_number = lvnum;
  return game.selected_level_number;
}

/*
 * Returns if the given bonus level is visible in land view screen.
 */
short is_bonus_level_visible(struct PlayerInfo *player, long bn_lvnum)
{
  static const char *func_name="is_bonus_level_visible";
  int i,n,k;
  i = storage_index_for_bonus_level(bn_lvnum);
/* new way - enable when process_dungeon_control_packet_clicks() is rewritten
  n = i/8;
  k = (1 << (i%8));
  if ((n < 0) || (n >= BONUS_LEVEL_STORAGE_COUNT))
    return false;
  return ((game.bonuses_found[n] & k) != 0);
*/
  if ((i < 0) || (i >= BONUS_LEVEL_STORAGE_COUNT))
    return false;
  return (game.bonuses_found[i] != 0);
}

/*
 * Makes the bonus level visible on the land map screen.
 */
short set_bonus_level_visibility(long bn_lvnum, short visible)
{
  static const char *func_name="set_bonus_level_visibility";
  int i,n,k;
  i = storage_index_for_bonus_level(bn_lvnum);
/* new way - enable when process_dungeon_control_packet_clicks() is rewritten
  n = i/8;
  k = (1 << (i%8));
  if ((n < 0) || (n >= BONUS_LEVEL_STORAGE_COUNT))
    return false;
  set_flag_byte(&game.bonuses_found[n], k, visible);
*/
  if ((i < 0) || (i >= BONUS_LEVEL_STORAGE_COUNT))
    return false;
  game.bonuses_found[i] = visible;
  return true;
}

/*
 * Makes a bonus level for specified SP level visible on the land map screen.
 */
short set_bonus_level_visibility_for_singleplayer_level(struct PlayerInfo *player, unsigned long sp_lvnum, short visible)
{
  static const char *func_name="set_bonus_level_visibility_for_singleplayer_level";
  long bn_lvnum;
  bn_lvnum = bonus_level_for_singleplayer_level(sp_lvnum);
  if (!set_bonus_level_visibility(bn_lvnum, visible))
  {
    if (visible)
      LbWarnLog("Couldn't store bonus award for level %d\n",sp_lvnum);
    return false;
  }
  if (visible)
    LbSyncLog("Bonus award for level %d enabled\n",sp_lvnum);
  return true;
}

void hide_all_bonus_levels(struct PlayerInfo *player)
{
  int i;
  for(i=0; i < BONUS_LEVEL_STORAGE_COUNT; i++)
    game.bonuses_found[i] = 0;
}

/*
 * Returns if the given extra level is visible in land view screen.
 */
unsigned short get_extra_level_kind_visibility(unsigned short elv_kind)
{
  static const char *func_name="get_extra_level_kind_visibility";
  LevelNumber ex_lvnum;
  ex_lvnum = get_extra_level(elv_kind);
  if (ex_lvnum <= 0)
    return LvSt_Hidden;
  switch (elv_kind)
  {
  case ExLv_FullMoon:
    if (is_full_moon)
      return LvSt_Visible;
    if (is_near_full_moon)
      return LvSt_HalfShow;
  case ExLv_NewMoon:
    if (is_new_moon)
      return LvSt_Visible;
    if (is_near_new_moon)
      return LvSt_HalfShow;
  }
  return LvSt_Hidden;
}

/*
 * Returns if the given extra level is visible in land view screen.
 */
short is_extra_level_visible(struct PlayerInfo *player, long ex_lvnum)
{
  static const char *func_name="is_extra_level_visible";
  int i,n,k;
  i = array_index_for_extra_level(ex_lvnum);
  switch (i+1)
  {
  case ExLv_FullMoon:
    return is_full_moon;
  case ExLv_NewMoon:
    return is_new_moon;
  }
  return false;
}

void update_extra_levels_visibility(void)
{
}

/*
 * Makes a bonus level for current SP level visible on the land map screen.
 */
short activate_bonus_level(struct PlayerInfo *player)
{
  static const char *func_name="activate_bonus_level";
  short result;
  LevelNumber sp_lvnum;
#if (BFDEBUG_LEVEL > 5)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  game.flags_font |= 0x02;
  sp_lvnum = get_loaded_level_number();
  result = set_bonus_level_visibility_for_singleplayer_level(player, sp_lvnum, true);
  if (!result)
    LbErrorLog("No Bonus level assigned to level %d\n",(int)sp_lvnum);
  set_flag_byte(&game.numfield_C,0x02,false);
  return result;
}

void multiply_creatures(struct PlayerInfo *player)
{
  static const char *func_name="multiply_creatures";
  struct Dungeon *dungeon;
  struct Thing *thing;
  struct Thing *tncopy;
  struct CreatureControl *cctrl;
  long i,k;

  dungeon = &(game.dungeon[player->field_2B%DUNGEONS_COUNT]);
  k = 0;
  i = dungeon->field_2D;
  while (i > 0)
  {
    thing = thing_get(i);
    if (thing_is_invalid(thing))
      break;
    tncopy = create_creature(&thing->mappos, thing->model, player->field_2B);
    if (tncopy == NULL)
      break;
    set_creature_level(tncopy, thing->field_23);
    tncopy->health = thing->health;
    if (thing->field_64 >= CREATURES_COUNT)
      break;
    cctrl = creature_control_get_from_thing(thing);
    if (creature_control_invalid(cctrl))
      break;
    i = cctrl->thing_idx;
    k++;
    if (k > CREATURES_COUNT)
    {
      error(func_name,4119,"Infinite loop detected when sweeping creatures list");
      break;
    }
  }

  k = 0;
  i = dungeon->field_2F;
  while (i > 0)
  {
    thing = thing_get(i);
    if (thing_is_invalid(thing))
      break;
    tncopy = create_creature(&thing->mappos, thing->model, player->field_2B);
    if (tncopy == NULL)
      break;
    set_creature_level(tncopy, thing->field_23);
    tncopy->health = thing->health;
    if (thing->field_64 >= CREATURES_COUNT)
      break;
    cctrl = creature_control_get_from_thing(thing);
    if (creature_control_invalid(cctrl))
      break;
    i = cctrl->thing_idx;
    k++;
    if (k > CREATURES_COUNT)
    {
      error(func_name,4129,"Infinite loop detected when sweeping creatures list");
      break;
    }
  }
}

short toggle_computer_player(int idx)
{
  struct Dungeon *dungeon;
  if (idx >= DUNGEONS_COUNT)
    return false;
  dungeon = &(game.dungeon[idx]);
  toggle_flag_byte(&dungeon->computer_enabled,0x01);
  return true;
}

short load_texture_map_file(unsigned long tmapidx, unsigned char n)
{
  static const char *func_name="load_texture_map_file";
  //return _DK_load_texture_map_file(tmapidx, n);
  char *fname;
#if (BFDEBUG_LEVEL > 7)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  fname = prepare_file_fmtpath(FGrp_StdData,"tmapa%03d.dat",tmapidx);
  if (!wait_for_cd_to_be_available())
    return 0;
  if (!LbFileExists(fname))
  {
    LbWarnLog("Texture file \"%s\" doesn't exits.\n",fname);
    return 0;
  }
  // The texture file has always over 500kb
  if (LbFileLoadAt(fname, block_mem) < 65536)
  {
    LbWarnLog("Texture file \"%s\" can't be loaded or is too small.\n",fname);
    return 0;
  }
  return 1;
}

void reinit_level_after_load(void)
{
  static const char *func_name="reinit_level_after_load";
  //_DK_reinit_level_after_load(); return;
  struct PlayerInfo *player;
  int i;
#if (BFDEBUG_LEVEL > 6)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  player = &(game.players[my_player_number%PLAYERS_COUNT]);
  player->field_7 = 0;
  init_lookups();
  init_navigation();
  parchment_loaded = 0;
  for (i=0; i < PLAYERS_COUNT; i++)
  {
    player = &(game.players[i%PLAYERS_COUNT]);
    if (player->field_0 & 0x01)
      set_engine_view(player, player->field_37);
  }
  start_rooms = &game.rooms[1];
  end_rooms = &game.rooms[ROOMS_COUNT];
  load_texture_map_file(game.texture_id, 2);
  init_animating_texture_maps();
  init_gui();
  reset_gui_based_on_player_mode();
  player = &(game.players[my_player_number%PLAYERS_COUNT]);
  reinit_tagged_blocks_for_player(player->field_2B);
  restore_computer_player_after_load();
  sound_reinit_after_load();
}

void create_shadow_limits(long start, long end)
{
  if (start <= end)
  {
    memset(&game.shadow_limits[start], 1, end-start);
  } else
  {
    memset(&game.shadow_limits[start], 1, SHADOW_LIMITS_COUNT-1-start);
    memset(&game.shadow_limits[0], 1, end);
  }
}

void clear_light_system(void)
{
  memset(game.field_1DD41, 0, 0x28416u);
}

void clear_shadow_limits(void)
{
  memset(game.shadow_limits, 0, SHADOW_LIMITS_COUNT);
}

void clear_stat_light_map(void)
{
  unsigned long x,y,i;
  game.field_46149 = 32;
  game.field_4614D = 0;
  game.field_4614F = 0;
  for (y=0; y < (map_subtiles_y+1); y++)
  {
    for (x=0; x < (map_subtiles_x+1); x++)
    {
      i = get_subtile_number(x,y);
      game.stat_light_map[i] = 0;
    }
  }
}

void light_delete_light(long idx)
{
  _DK_light_delete_light(idx);
}

void light_initialise_lighting_tables(void)
{
  _DK_light_initialise_lighting_tables();
}


void light_initialise(void)
{
  struct Light *lgt;
  int i;
  //_DK_light_initialise();
  for (i=0; i < LIGHTS_COUNT; i++)
  {
    lgt = &game.lights[i];
    if (lgt->field_0 & 0x01)
      light_delete_light(lgt->field_E);
  }
  if (!game.field_4614E)
  {
    light_initialise_lighting_tables();
    for (i=0; i < 32; i++)
    {
      light_bitmask[i] = 1 << (31-i);
    }
    game.field_4614E = 1;
  }
}

struct ActionPoint *action_point_get(long apt_idx)
{
  if ((apt_idx < 1) || (apt_idx > ACTN_POINTS_COUNT))
    return &game.action_points[0];
  return &game.action_points[apt_idx];
}

struct ActionPoint *action_point_get_by_number(long apt_num)
{
  struct ActionPoint *apt;
  long apt_idx;
  for (apt_idx=0; apt_idx < ACTN_POINTS_COUNT; apt_idx++)
  {
    apt = &game.action_points[apt_idx];
    if (apt->num == apt_num)
      return apt;
  }
  return &game.action_points[0];
}

long action_point_number_to_index(long apt_num)
{
  struct ActionPoint *apt;
  long apt_idx;
  for (apt_idx=0; apt_idx < ACTN_POINTS_COUNT; apt_idx++)
  {
    apt = &game.action_points[apt_idx];
    if (apt->num == apt_num)
      return apt_idx;
  }
  return -1;
}

TbBool action_point_is_invalid(const struct ActionPoint *apt)
{
  return (apt == &game.action_points[0]) || (apt == NULL);
}

TbBool action_point_exists_idx(long apt_idx)
{
  struct ActionPoint *apt;
  apt = action_point_get(apt_idx);
  if (action_point_is_invalid(apt))
    return false;
  return ((apt->flags & 0x01) != 0);
}

TbBool action_point_exists_number(long apt_num)
{
  struct ActionPoint *apt;
  apt = action_point_get_by_number(apt_num);
  if (action_point_is_invalid(apt))
    return false;
  return ((apt->flags & 0x01) != 0);
}

TbBool action_point_reset_idx(long apt_idx)
{
  struct ActionPoint *apt;
  apt = action_point_get(apt_idx);
  if (action_point_is_invalid(apt))
    return false;
  apt->activated = 0;
  return ((apt->flags & 0x01) != 0);
}

/*
 * Returns an action point activation bitmask.
 * Bits which are set in the bitmask corresponds to players which have triggered action point.
 */
unsigned long get_action_point_activated_by_players_mask(long apt_idx)
{
  struct ActionPoint *apt;
  apt = action_point_get(apt_idx);
  return apt->activated;
}

PlayerFlags action_point_get_players_within(long apt_idx)
{
  return _DK_action_point_get_players_within(apt_idx);
}

TbBool process_action_points(void)
{
  static const char *func_name="process_action_points";
#if (BFDEBUG_LEVEL > 6)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  //return _DK_process_action_points();
  struct ActionPoint *apt;
  long i;
  for (i=1; i < ACTN_POINTS_COUNT; i++)
  {
    apt = &game.action_points[i];
    if (apt->flags & 0x01)
    {
      if (((apt->num + game.play_gameturn) & 0x1F) == 0)
      {
        apt->activated = action_point_get_players_within(i);
//if (i==1) show_onscreen_msg(2*game.num_fps, "APT PLYRS %d", (int)apt->activated);
      }
    }
  }
  return true;
}

/*
 * Sets to defaults some basic parameters which are
 * later copied into Game structure.
 */
short set_default_startup_parameters(void)
{
  memset(&start_params, 0, sizeof(struct StartupParameters));
  start_params.packet_checksum = 1;
  set_flag_byte(&start_params.flags_font,0x01,false);
  // Set levels to 0, as we may not have the campaign loaded yet
  start_params.selected_level_number = 0;
  start_params.num_fps = 20;
  start_params.one_player = 1;
  set_flag_byte(&start_params.flags_cd,MFlg_IsDemoMode,false);
  set_flag_byte(&start_params.flags_cd,0x40,true);
}

/*
 * Clears the Game structure completely, and copies statrup parameters
 * from start_params structure.
 */
void clear_complete_game(void)
{
  memset(&game, 0, sizeof(struct Game));
  game.numfield_149F42 = -1;
  game.numfield_149F46 = 0;
  game.packet_checksum = start_params.packet_checksum;
  game.numfield_1503A2 = -1;
  game.flags_font = start_params.flags_font;
  game.numfield_149F47 = 0;
  // Set levels to 0, as we may not have the campaign loaded yet
  set_continue_level_number(first_singleplayer_level());
  if ((start_params.numfield_C & 0x02) != 0)
    set_selected_level_number(start_params.selected_level_number);
  else
    set_selected_level_number(first_singleplayer_level());
  game.num_fps = start_params.num_fps;
  game.flags_cd = start_params.flags_cd;
  game.no_intro = start_params.no_intro;
  game.one_player = start_params.one_player;
  game.numfield_C = start_params.numfield_C;
  strncpy(game.packet_fname,start_params.packet_fname,150);
  game.packet_save_enable = start_params.packet_save_enable;
  game.packet_load_enable = start_params.packet_load_enable;
  my_player_number = default_loc_player;
}

void clear_slabsets(void)
{
  struct SlabSet *sset;
  struct SlabObj *sobj;
  int i;
  for (i=0; i < SLABSET_COUNT; i++)
  {
    sset = &game.slabset[i];
    memset(sset, 0, sizeof(struct SlabSet));
    game.slabobjs_idx[i] = -1;
  }
  game.slabset_num = SLABSET_COUNT;
  game.slabobjs_num = 0;
  for (i=0; i < SLABOBJS_COUNT; i++)
  {
    sobj = &game.slabobjs[i];
    memset(sobj, 0, sizeof(struct SlabObj));
  }
}

void clear_map(void)
{
  //_DK_clear_map();
  clear_mapmap();
  clear_slabs();
  clear_columns();
  clear_slabsets();
}

void make_solidmask(struct Column *col)
{
  int i;
  col->solidmask = 0;
  for (i=0; i<COLUMN_STACK_HEIGHT; i++)
  {
    if (col->cubes[i] != 0)
      col->solidmask |= (1 << i);
  }
}

unsigned short find_column_height(struct Column *col)
{
  unsigned short h;
  h = 0;
  if (col->solidmask == 0)
    return h;
  while (col->cubes[h] > 0)
  {
    h++;
    if (h >= COLUMN_STACK_HEIGHT)
      return COLUMN_STACK_HEIGHT;
  }
  return h;
}

long find_column(struct Column *col)
{
  return _DK_find_column(col);
}

long create_column(struct Column *col)
{
  return _DK_create_column(col);
}

void clear_columns(void)
{
  //  _DK_clear_columns();
  struct Column *col;
  int i;
  for (i=0; i < COLUMNS_COUNT; i++)
  {
    col = &game.columns[i];
    memset(col, 0, sizeof(struct Column));
    col->baseblock = 1;
    make_solidmask(col);
  }
  game.field_149E6E = -1;
  game.field_149E7C = 24;
  game.field_149E77 = 0;
  for (i=0; i < 18; i++)
  {
    game.field_14A818[i] = 0;
  }
}

void clear_things_and_persons_data(void)
{
  long i;
  for (i=0; i < THINGS_COUNT; i++)
  {
    memset(&game.things_data[i], 0, sizeof(struct Thing));
  }
  for (i=0; i < CREATURES_COUNT; i++)
  {
    memset(&game.cctrl_data[i], 0, sizeof(struct CreatureControl));
  }
}

void clear_action_points(void)
{
  long i;
  for (i=0; i < ACTN_POINTS_COUNT; i++)
  {
    memset(&game.action_points[i], 0, sizeof(struct ActionPoint));
  }
}

void clear_computer(void)
{
  static const char *func_name="clear_computer";
  long i;
#if (BFDEBUG_LEVEL > 8)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  for (i=0; i < COMPUTER_TASKS_COUNT; i++)
  {
    memset(&game.computer_task[i], 0, sizeof(struct ComputerTask));
  }
  for (i=0; i < GOLD_LOOKUP_COUNT; i++)
  {
    memset(&game.gold_lookup[i], 0, sizeof(struct GoldLookup));
  }
  for (i=0; i < PLAYERS_COUNT; i++)
  {
    memset(&game.computer[i], 0, sizeof(struct Computer2));
  }
}

void clear_players(void)
{
  int i;
  for (i=0; i < PLAYERS_COUNT; i++)
  {
    memset(&game.players[i], 0, sizeof(struct PlayerInfo));
  }
  game.field_14E496 = hero_player;
  game.field_14E495 = 0;
  game.flagfield_14EA4A = 2;
}

void clear_rooms(void)
{
  int i;
  for (i=0; i < ROOMS_COUNT; i++)
  {
    memset(&game.rooms[i], 0, sizeof(struct Room));
  }
}

void clear_dungeons(void)
{
  static const char *func_name="clear_dungeons";
#if (BFDEBUG_LEVEL > 6)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  int i;
  for (i=0; i < DUNGEONS_COUNT; i++)
  {
    memset(&game.dungeon[i], 0, sizeof(struct Dungeon));
  }
  game.field_14E4A4 = 0;
  game.field_14E4A0 = 0;
  game.field_14E49E = 0;
}

void clear_events(void)
{
  int i;
  for (i=0; i < EVENTS_COUNT; i++)
  {
    memset(&game.event[i], 0, sizeof(struct Event));
  }
  memset(&game.evntbox_text_shown, 0, MESSAGE_TEXT_LEN);
  memset(&game.evntbox_text_buffer, 0, MESSAGE_TEXT_LEN);
  memset(&game.evntbox_text_objective, 0, MESSAGE_TEXT_LEN);
  for (i=0; i < 5; i++)
  {
    memset(&game.bookmark[i], 0, sizeof(struct Bookmark));
  }
}

void init_keepers_map_exploration(void)
{
  struct PlayerInfo *player;
  int i;
  for (i=0; i < PLAYERS_COUNT; i++)
  {
    player = &(game.players[i%PLAYERS_COUNT]);
    if ((player->field_0 & 0x01) && (player->field_2C == 1))
    {
        if (player->field_0 & 0x40)
          init_keeper_map_exploration(player);
    }
  }
}

void clear_players_for_save(void)
{
  struct PlayerInfo *player;
  unsigned short mem1,mem2,memflg;
  struct Camera cammem;
  int i;
  for (i=0; i < PLAYERS_COUNT; i++)
  {
    player=&(game.players[i%PLAYERS_COUNT]);
    mem1 = player->field_2B;
    mem2 = player->field_2C;
    memflg = player->field_0;
    LbMemoryCopy(&cammem,&player->cameras[1],sizeof(struct Camera));
    memset(player, 0, sizeof(struct PlayerInfo));
    player->field_2B = mem1;
    player->field_2C = mem2;
    set_flag_byte(&player->field_0,0x01,((memflg & 0x01) != 0));
    set_flag_byte(&player->field_0,0x40,((memflg & 0x40) != 0));
    LbMemoryCopy(&player->cameras[1],&cammem,sizeof(struct Camera));
    player->acamera = &player->cameras[1];
  }
}

void delete_all_thing_structures(void)
{
  long i;
  struct Thing *thing;
  for (i=1; i < THINGS_COUNT; i++)
  {
    thing = game.things_lookup[i];
    if (thing != NULL)
    {
      if (thing->field_0 & 0x01)
        delete_thing_structure(thing, 1);
    }
  }
  for (i=0; i < THINGS_COUNT-1; i++)
  {
    game.free_things[i] = i+1;
  }
  game.free_things[THINGS_COUNT-1] = 0;
}

void delete_control_structure(struct CreatureControl *cctrl)
{
  memset(cctrl, 0, sizeof(struct CreatureControl));
}

void delete_all_control_structures(void)
{
  long i;
  struct CreatureControl *cctrl;
  for (i=1; i < CREATURES_COUNT; i++)
  {
    if (cctrl != NULL)
    {
      if (cctrl->field_0 & 0x0100)
        delete_control_structure(cctrl);
    }
  }
}

void delete_action_point_structure(struct ActionPoint *apt)
{
  if (apt->flags & 0x01)
  {
    memset(apt, 0, sizeof(struct ActionPoint));
  }
}

void delete_all_action_point_structures(void)
{
  struct ActionPoint *apt;
  long i;
  for (i=1; i < ACTN_POINTS_COUNT; i++)
  {
    apt = &game.action_points[i];
    if (apt != NULL)
    {
      delete_action_point_structure(apt);
    }
  }
}

void delete_all_structures(void)
{
  static const char *func_name="delete_all_structures";
#if (BFDEBUG_LEVEL > 6)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  delete_all_thing_structures();
  delete_all_control_structures();
  delete_all_room_structures();
  delete_all_action_point_structures();
  light_initialise();
}

void clear_game_for_summary(void)
{
  static const char *func_name="clear_game_for_summary";
#if (BFDEBUG_LEVEL > 6)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  delete_all_structures();
  clear_shadow_limits();
  clear_stat_light_map();
  clear_mapwho();
  game.field_14E938 = 0;
  game.field_14BB4A = 0;
  set_flag_byte(&game.numfield_C,0x04,false);
  clear_columns();
  clear_action_points();
  clear_players();
  clear_dungeons();
}

void clear_game(void)
{
  static const char *func_name="clear_game";
#if (BFDEBUG_LEVEL > 6)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  //_DK_clear_game(); return;
  clear_game_for_summary();
  game.audiotrack = 0;
  clear_map();
  clear_computer();
  clear_script();
  clear_events();
  clear_things_and_persons_data();
  ceiling_set_info(12, 4, 1);
  init_animating_texture_maps();
}

void clear_game_for_save(void)
{
  //_DK_clear_game_for_save(); return;
  delete_all_structures();
  light_initialise();
  clear_mapwho();
  game.field_14E938 = 0;
  game.field_14BB4A = 0;
  set_flag_byte(&game.numfield_C,0x04,false);
  clear_columns();
  clear_players_for_save();
  clear_dungeons();
}

void reset_creature_max_levels(void)
{
  struct Dungeon *dungeon;
  int i,k;
  for (i=0; i < DUNGEONS_COUNT; i++)
  {
    for (k=1; k < CREATURE_TYPES_COUNT; k++)
    {
      dungeon = &game.dungeon[i];
      dungeon->creature_max_level[k] = CREATURE_MAX_LEVEL+1;
    }
  }
}

void reset_script_timers_and_flags(void)
{
  struct Dungeon *dungeon;
  int i,k;
  for (i=0; i < DUNGEONS_COUNT; i++)
  {
    dungeon = &(game.dungeon[i%DUNGEONS_COUNT]);
    dungeon->field_1006 = 1;
    dungeon->field_FF2 = 1;
    for (k=0; k<TURN_TIMERS_COUNT; k++)
    {
      memset(&dungeon->turn_timers[k], 0, sizeof(struct TurnTimer));
      dungeon->turn_timers[k].state = 0;
    }
    for (k=0; k<SCRIPT_FLAGS_COUNT; k++)
    {
      dungeon->script_flags[k] = 0;
    }
  }
}

void init_good_player_as(long plr_idx)
{
  struct PlayerInfo *player;
  game.field_14E496 = plr_idx;
  player = &(game.players[plr_idx%PLAYERS_COUNT]);
  player->field_0 |= 0x01;
  player->field_0 |= 0x40;
  player->field_2B = game.field_14E496;
}

void resync_game(void)
{
  return _DK_resync_game();
}

void free_swipe_graphic(void)
{
  static const char *func_name="free_swipe_graphic";
#if (BFDEBUG_LEVEL > 6)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  //_DK_free_swipe_graphic();
  if (game.field_1516FF != -1)
  {
    LbDataFreeAll(swipe_load_file);
    game.field_1516FF = -1;
  }
}

void message_add(char c)
{
  _DK_message_add(c);
}

void light_stat_light_map_clear_area(long x1, long y1, long x2, long y2)
{
  _DK_light_stat_light_map_clear_area(x1, y1, x2, y2);
}

void light_signal_stat_light_update_in_area(long x1, long y1, long x2, long y2)
{
  _DK_light_signal_stat_light_update_in_area(x1, y1, x2, y2);
}

void light_set_lights_on(char state)
{
  if (state)
  {
    game.field_46149 = 10;
    game.field_4614D = 1;
  } else
  {
    game.field_46149 = 32;
    game.field_4614D = 0;
  }
  // Enable lights on all but bounding subtiles
  light_stat_light_map_clear_area(0, 0, map_subtiles_x, map_subtiles_y);
  light_signal_stat_light_update_in_area(1, 1, map_subtiles_x, map_subtiles_y);
}

void change_engine_window_relative_size(long w_delta, long h_delta)
{
  struct PlayerInfo *myplyr;
  myplyr=&(game.players[my_player_number%PLAYERS_COUNT]);
  setup_engine_window(myplyr->engine_window_x, myplyr->engine_window_y,
    myplyr->engine_window_width+w_delta, myplyr->engine_window_height+h_delta);
}

void PaletteSetPlayerPalette(struct PlayerInfo *player, unsigned char *pal)
{
  //_DK_PaletteSetPlayerPalette(player, pal);
  if (pal == blue_palette)
  {
    if ((player->field_3 & 0x04) == 0)
      return;
    player->field_3 |= 0x04;
  } else
  {
    player->field_3 &= 0xFB;
  }
  if ((player->field_7 == 0) || (pal != player->field_4C9) && (pal == player->field_7))
  {
    player->field_4C9 = pal;
    player->field_4C1 = 0;
    player->field_4C5 = 0;
    if (is_my_player(player))
    {
      LbScreenWaitVbi();
      LbPaletteSet(pal);
    }
  }
}

short set_gamma(char corrlvl, short do_set)
{
  static const char *func_name="set_gamma";
  char *fname;
  short result=1;
  if (corrlvl < 0)
    corrlvl = 0;
  else
  if (corrlvl > 4)
    corrlvl = 4;
  settings.gamma_correction = corrlvl;
  fname=prepare_file_fmtpath(FGrp_StdData,"pal%05d.dat",settings.gamma_correction);
  if (!LbFileExists(fname))
  {
    LbWarnLog("Palette file \"%s\" doesn't exist.\n", fname);
    result = 0;
  }
  if (result)
  {
    result = (LbFileLoadAt(fname, _DK_palette) != -1);
  }
  if ((result)&&(do_set))
  {
    struct PlayerInfo *myplyr;
    myplyr=&(game.players[my_player_number%PLAYERS_COUNT]);
    PaletteSetPlayerPalette(myplyr, _DK_palette);
  }
  if (!result)
    error(func_name, 3720, "Can't load palette file.");
  return result;
}

void centre_engine_window(void)
{
  long x1,y1;
  struct PlayerInfo *player=&(game.players[my_player_number%PLAYERS_COUNT]);
  if ((game.numfield_C & 0x20) != 0)
    x1 = (MyScreenWidth-player->engine_window_width-status_panel_width) / 2 + status_panel_width;
  else
    x1 = (MyScreenWidth-player->engine_window_width) / 2;
  y1 = (MyScreenHeight-player->engine_window_height) / 2;
  setup_engine_window(x1, y1, player->engine_window_width, player->engine_window_height);
}

void set_engine_view(struct PlayerInfo *player, long val)
{
  _DK_set_engine_view(player, val);
}

TbBool toggle_creature_tendencies(struct PlayerInfo *player, unsigned short tend_type)
{
  struct Dungeon *dungeon;
  //_DK_toggle_creature_tendencies(player, val);
  dungeon = &(game.dungeon[player->field_2B%DUNGEONS_COUNT]);
  switch (tend_type)
  {
  case 1:
      dungeon->creature_tendencies ^= 0x01;
      return true;
  case 2:
      dungeon->creature_tendencies ^= 0x02;
      return true;
  default:
      LbErrorLog("Can't toggle tendency; bad tendency type %d.\n",(int)tend_type);
      return false;
  }
}

TbBool set_creature_tendencies(struct PlayerInfo *player, unsigned short tend_type, TbBool val)
{
  struct Dungeon *dungeon;
  dungeon = &(game.dungeon[player->field_2B%DUNGEONS_COUNT]);
  switch (tend_type)
  {
  case 1:
      set_flag_byte(&dungeon->creature_tendencies, 0x01, val);
      return true;
  case 2:
      set_flag_byte(&dungeon->creature_tendencies, 0x02, val);
      return true;
  default:
      LbErrorLog("Can't set tendency; bad tendency type %d.\n",(int)tend_type);
      return false;
  }
}

void set_player_state(struct PlayerInfo *player, short a1, long a2)
{
  _DK_set_player_state(player, a1, a2);
}

void set_player_mode(struct PlayerInfo *player, long val)
{
  _DK_set_player_mode(player, val);
}

void turn_off_query(short a)
{
  _DK_turn_off_query(a);
}

void turn_off_call_to_arms(long a)
{
  _DK_turn_off_call_to_arms(a);
}

long battle_move_player_towards_battle(struct PlayerInfo *player, long var)
{
  return _DK_battle_move_player_towards_battle(player, var);
}

long place_thing_in_power_hand(struct Thing *thing, long var)
{
  return _DK_place_thing_in_power_hand(thing, var);
}

short dump_held_things_on_map(unsigned int plyridx, long a2, long a3, short a4)
{
  return _DK_dump_held_things_on_map(plyridx, a2, a3, a4);
}

void magic_use_power_armageddon(unsigned int plridx)
{
  _DK_magic_use_power_armageddon(plridx);
}

long set_autopilot_type(unsigned int plridx, long aptype)
{
  return _DK_set_autopilot_type(plridx, aptype);
}

short magic_use_power_obey(unsigned short plridx)
{
  return _DK_magic_use_power_obey(plridx);
}

void turn_off_sight_of_evil(long plridx)
{
  _DK_turn_off_sight_of_evil(plridx);
}

void reset_scroll_window(void)
{
  game.field_1512D9 = 0;
  game.field_1512DD = 0;
  game.field_1512DE = 0;
  game.field_1512E2 = 0;
}

unsigned char find_first_battle_of_mine(unsigned char idx)
{
  return _DK_find_first_battle_of_mine(idx);
}

void level_lost_go_first_person(long plridx)
{
  static const char *func_name="level_lost_go_first_person";
#if (BFDEBUG_LEVEL > 6)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  _DK_level_lost_go_first_person(plridx);
#if (BFDEBUG_LEVEL > 8)
    LbSyncLog("%s: Finished\n",func_name);
#endif
}

void find_map_location_coords(long location, long *x, long *y, const char *func_name)
{
  struct PlayerInfo *player;
  struct Dungeon *dungeon;
  struct ActionPoint *apt;
  struct Thing *thing;
  long pos_x,pos_y;
  long i;
#if (BFDEBUG_LEVEL > 15)
    LbSyncLog("%s: Location %ld, pos(%ld,%ld)\n",func_name, location, *x, *y);
#endif
  pos_y = 0;
  pos_x = 0;
  switch (get_map_location_type(location))
  {
  case MLoc_ACTIONPOINT:
      i = get_map_location_longval(location);
      apt = action_point_get_by_number(i);
      if (!action_point_is_invalid(apt))
      {
        pos_y = apt->mappos.y.stl.num;
        pos_x = apt->mappos.x.stl.num;
      } else
        LbWarnLog("%s: Action Point %d location for not found\n",func_name,i);
      break;
  case MLoc_HEROGATE:
      i = get_map_location_longval(location);
      thing = find_hero_gate_of_number(i);
      if (!thing_is_invalid(thing))
      {
        pos_y = thing->mappos.y.stl.num;
        pos_x = thing->mappos.x.stl.num;
      } else
        LbWarnLog("%s: Hero Gate %d location not found\n",func_name,i);
      break;
  case MLoc_PLAYERSHEART:
      i = get_map_location_longval(location);
      if (i < PLAYERS_COUNT)
      {
        player = &(game.players[i]);
        dungeon = &(game.dungeon[player->field_2B%DUNGEONS_COUNT]);
        thing = thing_get(dungeon->field_0);
      } else
        thing = NULL;
      if (!thing_is_invalid(thing))
      {
        pos_y = thing->mappos.y.stl.num;
        pos_x = thing->mappos.x.stl.num;
      } else
        LbWarnLog("%s: Dungeon Heart location for player %d not found\n",func_name,i);
      break;
  case MLoc_NONE:
      pos_y = *y;
      pos_x = *x;
      break;
  case MLoc_THING:
      i = get_map_location_longval(location);
      thing = thing_get(i);
      if (!thing_is_invalid(thing))
      {
        pos_y = thing->mappos.y.stl.num;
        pos_x = thing->mappos.x.stl.num;
      } else
        LbWarnLog("%s: Thing %d location not found\n",func_name,i);
      break;
  case MLoc_CREATUREKIND:
  case MLoc_OBJECTKIND:
  case MLoc_ROOMKIND:
  default:
        LbWarnLog("%s: Unsupported location %lu\n",func_name,location);
      break;
  }
  *y = pos_y;
  *x = pos_x;
}

void set_general_information(long msg_id, long target, long x, long y)
{
  static const char *func_name="set_general_information";
  struct PlayerInfo *player;
  long pos_x,pos_y;
  player = &(game.players[my_player_number%PLAYERS_COUNT]);
  find_map_location_coords(target, &x, &y, func_name);
  pos_x = 0;
  pos_y = 0;
  if ((x != 0) || (y != 0))
  {
    pos_y = (y << 8) + 128;
    pos_x = (x << 8) + 128;
  }
  event_create_event(pos_x, pos_y, 21, player->field_2B, -msg_id);
}

void set_quick_information(long msg_id, long target, long x, long y)
{
  static const char *func_name="set_quick_information";
  struct PlayerInfo *player;
  long pos_x,pos_y;
  player = &(game.players[my_player_number%PLAYERS_COUNT]);
  find_map_location_coords(target, &x, &y, func_name);
  pos_x = 0;
  pos_y = 0;
  if ((x != 0) || (y != 0))
  {
    pos_y = (y << 8) + 128;
    pos_x = (x << 8) + 128;
  }
  event_create_event(pos_x, pos_y, 27, player->field_2B, -msg_id);
}

void set_general_objective(long msg_id, long target, long x, long y)
{
  process_objective(campaign.strings[msg_id%STRINGS_MAX], target, x, y);
}

void process_objective(char *msg_text, long target, long x, long y)
{
  static const char *func_name="process_objective";
  struct PlayerInfo *player;
  long pos_x,pos_y;
  player = &(game.players[my_player_number%PLAYERS_COUNT]);
  find_map_location_coords(target, &x, &y, func_name);
  pos_y = y;
  pos_x = x;
  set_level_objective(msg_text);
  display_objectives(player->field_2B, pos_x, pos_y);
}

void event_initialise_all(void)
{
  _DK_event_initialise_all();
}

long event_move_player_towards_event(struct PlayerInfo *player, long var)
{
  return _DK_event_move_player_towards_event(player,var);
}

struct Event *event_create_event(long map_x, long map_y, unsigned char evkind, unsigned char dngn_id, long msg_id)
{
  static const char *func_name="event_create_event";
  struct Dungeon *dungeon;
  struct Event *event;
  long i,k;
//  return _DK_event_create_event(map_x, map_y, evkind, dngn_id, msg_id);
  if (dngn_id >= game.field_14E497)
    return NULL;
  if (evkind >= 28)
  {
    error(func_name, 52, "Illegal Event kind to be created");
    return NULL;
  }
  dungeon = &(game.dungeon[dngn_id%DUNGEONS_COUNT]);
  i = dungeon->field_13B4[evkind%EVENT_KIND_COUNT];
  if (i != 0)
  {
    k = event_button_info[evkind].field_C;
    if ((k != 0) && (k+i >= game.play_gameturn))
    {
      return NULL;
    }
  }
  event = event_allocate_free_event_structure();
  if (event == NULL)
    return NULL;
  event_initialise_event(event, map_x, map_y, evkind, dngn_id, msg_id);
  event_add_to_event_list(event, dungeon);
  return event;
}

void go_on_then_activate_the_event_box(long plridx, long evidx)
{
  static const char *func_name="go_on_then_activate_the_event_box";
  //_DK_go_on_then_activate_the_event_box(plridx, val); return;
  struct Dungeon *dungeon;
  struct Event *event;
  struct Thing *thing;
  char *text;
  int i,k;
  short other_off;
  dungeon = &(game.dungeon[plridx%DUNGEONS_COUNT]);
  event = &game.event[evidx];
#if (BFDEBUG_LEVEL > 6)
    LbSyncLog("%s: Starting for event kind %d\n",func_name,event->kind);
#endif
  dungeon->field_1173 = evidx;
  if (plridx == my_player_number)
  {
    i = event_button_info[event->kind].field_6;
    if (i != 201)
      strcpy(game.evntbox_text_shown, gui_strings[i%STRINGS_MAX]);
  }
  if (event->kind == 2)
    dungeon->field_1174 = find_first_battle_of_mine(plridx);
  if (plridx == my_player_number)
  {
    other_off = 0;
    switch (event->kind)
    {
    case 1:
    case 4:
        other_off = 1;
        turn_on_menu(GMnu_TEXT_INFO);
        break;
    case 2:
        turn_off_menu(GMnu_TEXT_INFO);
        turn_on_menu(GMnu_BATTLE);
        break;
    case 3:
        strcpy(game.evntbox_text_shown, game.evntbox_text_objective);
        for (i=EVENT_BUTTONS_COUNT; i >= 0; i--)
        {
          k = dungeon->field_13A7[i];
          if (game.event[k%EVENTS_COUNT].kind == 3)
          {
            other_off = 1;
            turn_on_menu(GMnu_TEXT_INFO);
            new_objective = 0;
            break;
          }
        }
        break;
    case 5:
        other_off = 1;
        i = room_data[event->field_C].field_13;
        text = buf_sprintf("%s:\n%s",game.evntbox_text_shown, gui_strings[i%STRINGS_MAX]);
        strncpy(game.evntbox_text_shown,text,MESSAGE_TEXT_LEN-1);
        turn_on_menu(GMnu_TEXT_INFO);
        break;
    case 6:
        other_off = 1;
        thing = thing_get(event->field_C);
        if (thing_is_invalid(thing))
          break;
        i = creature_data[thing->model % CREATURE_TYPES_COUNT].field_3;
        text = buf_sprintf("%s:\n%s", game.evntbox_text_shown, gui_strings[i%STRINGS_MAX]);
        strncpy(game.evntbox_text_shown,text,MESSAGE_TEXT_LEN-1);
        turn_on_menu(GMnu_TEXT_INFO);
        break;
    case 7:
        other_off = 1;
        i = spell_data[event->field_C % (SPELL_TYPES_COUNT+1)].field_D;
        text = buf_sprintf("%s:\n%s", game.evntbox_text_shown, gui_strings[i%STRINGS_MAX]);
        strncpy(game.evntbox_text_shown,text,MESSAGE_TEXT_LEN-1);
        turn_on_menu(GMnu_TEXT_INFO);
        break;
    case 8:
        other_off = 1;
        i = trap_data[event->field_C % MANUFCTR_TYPES_COUNT].name_stridx;
        text = buf_sprintf("%s:\n%s", game.evntbox_text_shown, gui_strings[i%STRINGS_MAX]);
        strncpy(game.evntbox_text_shown,text,MESSAGE_TEXT_LEN-1);
        turn_on_menu(GMnu_TEXT_INFO);
        break;
    case 9:
        other_off = 1;
        i = door_names[event->field_C % DOOR_TYPES_COUNT];
        text = buf_sprintf("%s:\n%s", game.evntbox_text_shown, gui_strings[i%STRINGS_MAX]);
        strncpy(game.evntbox_text_shown,text,MESSAGE_TEXT_LEN-1);
        turn_on_menu(GMnu_TEXT_INFO);
        break;
    case 10:
        other_off = 1;
        thing = thing_get(event->field_C);
        if (thing_is_invalid(thing))
          break;
        i = creature_data[thing->model % CREATURE_TYPES_COUNT].field_3;
        text = buf_sprintf("%s:\n%s", game.evntbox_text_shown, gui_strings[i%STRINGS_MAX]);
        strncpy(game.evntbox_text_shown,text,MESSAGE_TEXT_LEN-1);
        turn_on_menu(GMnu_TEXT_INFO);
        break;
    case 11:
    case 13:
        other_off = 1;
        turn_on_menu(GMnu_TEXT_INFO);
        break;
    case 12:
        other_off = 1;
        text = buf_sprintf("%s:\n %d", game.evntbox_text_shown, event->field_C);
        strncpy(game.evntbox_text_shown,text,MESSAGE_TEXT_LEN-1);
        turn_on_menu(GMnu_TEXT_INFO);
        break;
    case 14:
        other_off = 1;
        thing = thing_get(event->field_C);
        if (thing_is_invalid(thing))
          break;
        i = spell_data[object_to_magic[thing->model % OBJECT_TYPES_COUNT]].field_D;
        text = buf_sprintf("%s:\n %s", game.evntbox_text_shown, gui_strings[i%STRINGS_MAX]);
        strncpy(game.evntbox_text_shown,text,MESSAGE_TEXT_LEN-1);
        turn_on_menu(GMnu_TEXT_INFO);
        break;
    case 15:
        other_off = 1;
        i = room_data[event->field_C].field_13;
        text = buf_sprintf("%s:\n %s",game.evntbox_text_shown,gui_strings[i%STRINGS_MAX]);
        strncpy(game.evntbox_text_shown,text,MESSAGE_TEXT_LEN-1);
        turn_on_menu(GMnu_TEXT_INFO);
        break;
    case 16:
        other_off = 1;
        thing = thing_get(event->field_C);
        if (thing_is_invalid(thing))
          break;
        i = creature_data[thing->model % CREATURE_TYPES_COUNT].field_3;
        text = buf_sprintf("%s:\n %s", game.evntbox_text_shown, gui_strings[i%STRINGS_MAX]);
        strncpy(game.evntbox_text_shown,text,MESSAGE_TEXT_LEN-1);
        turn_on_menu(GMnu_TEXT_INFO);
        break;
    case 17:
    case 18:
    case 19:
    case 20:
    case 22:
    case 23:
        other_off = 1;
        turn_on_menu(GMnu_TEXT_INFO);
        break;
    case 21:
        i = (long)event->field_C;
        if (i < 0)
        {
          i = -i;
          event->field_C = i;
        }
        strncpy(game.evntbox_text_buffer, campaign.strings[i%STRINGS_MAX], MESSAGE_TEXT_LEN-1);
        strncpy(game.evntbox_text_shown, game.evntbox_text_buffer, MESSAGE_TEXT_LEN-1);
        other_off = 1;
        turn_on_menu(GMnu_TEXT_INFO);
        break;
    case 24:
        other_off = 1;
        thing = thing_get(event->field_C);
        if (thing_is_invalid(thing))
          break;
        i = trap_data[object_to_door_or_trap[thing->model % OBJECT_TYPES_COUNT]].name_stridx;
        text = buf_sprintf("%s:\n %s", game.evntbox_text_shown, gui_strings[i%STRINGS_MAX]);
        strncpy(game.evntbox_text_shown,text,MESSAGE_TEXT_LEN-1);
        turn_on_menu(GMnu_TEXT_INFO);
        break;
      case 25:
        other_off = 1;
        thing = thing_get(event->field_C);
        if (thing_is_invalid(thing))
          break;
        i = door_names[object_to_door_or_trap[thing->model % OBJECT_TYPES_COUNT]];
        text = buf_sprintf("%s:\n %s", game.evntbox_text_shown, gui_strings[i%STRINGS_MAX]);
        strncpy(game.evntbox_text_shown,text,MESSAGE_TEXT_LEN-1);
        turn_on_menu(GMnu_TEXT_INFO);
        break;
      case 26:
        other_off = 1;
        thing = thing_get(event->field_C);
        if (thing_is_invalid(thing))
          break;
        i = specials_text[thing_to_special(thing)];
        text = buf_sprintf("%s:\n %s", game.evntbox_text_shown, gui_strings[i%STRINGS_MAX]);
        strncpy(game.evntbox_text_shown,text,MESSAGE_TEXT_LEN-1);
        turn_on_menu(GMnu_TEXT_INFO);
        break;
      case 27:
        i = (long)event->field_C;
        if (i < 0)
        {
          i = -i;
          event->field_C = i;
        }
        strncpy(game.evntbox_text_buffer, quick_messages[i%QUICK_MESSAGES_COUNT], MESSAGE_TEXT_LEN-1);
        strncpy(game.evntbox_text_shown, game.evntbox_text_buffer, MESSAGE_TEXT_LEN-1);
        other_off = 1;
        turn_on_menu(GMnu_TEXT_INFO);
        break;
    default:
        text = buf_sprintf("Undefined event kind: %d", (int)event->kind);
        error(func_name, 9100, text);
        break;
    }
    reset_scroll_window();
    if (other_off)
    {
      turn_off_menu(34);
      turn_off_menu(27);
      turn_off_menu(28);
      turn_off_menu(29);
    }
  }
#if (BFDEBUG_LEVEL > 8)
    LbSyncLog("%s: Finished\n",func_name);
#endif
}

void directly_cast_spell_on_thing(long plridx, unsigned char a2, unsigned short a3, long a4)
{
  _DK_directly_cast_spell_on_thing(plridx, a2, a3, a4);
}

void clear_things_in_hand(struct PlayerInfo *player)
{
  struct Dungeon *dungeon;
  long i;
  dungeon = &(game.dungeon[player->field_2B%DUNGEONS_COUNT]);
  for (i=0; i < MAX_THINGS_IN_HAND; i++)
    dungeon->things_in_hand[i] = 0;
}

short winning_player_quitting(struct PlayerInfo *player, long *plyr_count)
{
  struct PlayerInfo *swplyr;
  int i,k,n;
  if (player->victory_state == VicS_LostLevel)
  {
    return 0;
  }
  k = 0;
  n = 0;
  for (i=0; i < PLAYERS_COUNT; i++)
  {
    swplyr = &(game.players[i]);
    if (swplyr->field_0 & 0x01)
    {
      if (swplyr->field_2C == 1)
      {
        k++;
        if (swplyr->victory_state == VicS_LostLevel)
          n++;
      }
    }
  }
  *plyr_count = k;
  return ((k - n) == 1);
}

short lose_level(struct PlayerInfo *player)
{
  long lvnum;
  //_DK_lose_level(player);
  if (!is_my_player(player))
    return false;
  if (game.numfield_A & 0x01)
  {
    LbNetwork_Stop();
  }
  quit_game = 1;
  return true;
}

short resign_level(struct PlayerInfo *player)
{
  long lvnum;
  if (!is_my_player(player))
    return false;
  if (game.numfield_A & 0x01)
  {
    LbNetwork_Stop();
  }
  quit_game = 1;
  return true;
}

short player_has_won(long plyr_idx)
{
  struct PlayerInfo *player;
  if ((plyr_idx < 0) || (plyr_idx > PLAYERS_COUNT))
    return false;
  player = &game.players[plyr_idx];
  return (player->victory_state == VicS_WonLevel);
}

short player_has_lost(long plyr_idx)
{
  struct PlayerInfo *player;
  if ((plyr_idx < 0) || (plyr_idx > PLAYERS_COUNT))
    return false;
  player = &game.players[plyr_idx];
  return (player->victory_state == VicS_LostLevel);
}

long compute_player_final_score(struct PlayerInfo *player,long gameplay_score)
{
  struct PlayerInfo *myplyr;
  long i;
  myplyr = &game.players[my_player_number%PLAYERS_COUNT];
  if (((game.numfield_A & 0x01) != 0) || !is_singleplayer_level(game.loaded_level_number))
    i = 2 * gameplay_score;
  else
    i = gameplay_score + 10 * gameplay_score * array_index_for_singleplayer_level(game.loaded_level_number) / 100;
  if (player_has_lost(player->field_2B))
    i /= 2;
  return i;
}

void set_player_as_won_level(struct PlayerInfo *player)
{
  struct PlayerInfo *myplyr;
  struct Dungeon *dungeon;
  if (player->victory_state != VicS_Undecided)
    return;
  myplyr = &game.players[my_player_number%PLAYERS_COUNT];
  if (myplyr->field_2B == player->field_2B)
    frontstats_initialise();
  player->victory_state = VicS_WonLevel;
  dungeon = &(game.dungeon[player->field_2B%DUNGEONS_COUNT]);
  // Computing player score
  dungeon->lvstats.player_score = compute_player_final_score(player, dungeon->field_AE5[3]);
  dungeon->lvstats.allow_save_score = 1;
  if ((game.numfield_A & 0x01) == 0)
    player->field_4EB = game.play_gameturn + 300;
  if (myplyr == player)
  {
    if (knight_in_prison())
      player->field_3 |= 0x10u;
    output_message(106, 0, 1);
  }
}

void set_player_as_lost_level(struct PlayerInfo *player)
{
  struct PlayerInfo *myplyr;
  struct Dungeon *dungeon;
  struct Thing *thing;
  if (player->victory_state != VicS_Undecided)
    return;
  myplyr = &game.players[my_player_number%PLAYERS_COUNT];
  if (myplyr->field_2B == player->field_2B)
    frontstats_initialise();
  player->victory_state = VicS_LostLevel;
  dungeon = &(game.dungeon[player->field_2B%DUNGEONS_COUNT]);
  // Computing player score
  dungeon->lvstats.player_score = compute_player_final_score(player, dungeon->field_AE5[3]);
  if (myplyr == player)
  {
    output_message(105, 0, 1);
    turn_off_all_menus();
    game.transfered_creature_kind = 0;
    game.transfered_creature_level = 0;
  }
  // This is probably 16-byte struct, or array
  clear_things_in_hand(player);
  dungeon->field_63 = 0;
  if (dungeon->field_884 != 0)
    turn_off_call_to_arms(player->field_2B);
  if (dungeon->field_5D8 > 0)
  {
    thing = thing_get(dungeon->field_5D8);
    delete_thing_structure(thing, 0);
    dungeon->field_5D8 = 0;
  }
  if (myplyr == player)
    gui_set_button_flashing(0, 0);
  set_player_mode(player, 1);
  set_player_state(player, 1, 0);
  if ((game.numfield_A & 0x01) == 0)
    player->field_4EB = game.play_gameturn + 300;
  if ((game.numfield_A & 0x01) != 0)
    reveal_whole_map(player);
  if (dungeon->computer_enabled & 0x01)
    toggle_computer_player(player->field_2B);
}

TbBool move_campaign_to_next_level(void)
{
  long lvnum;
  lvnum = next_singleplayer_level(get_continue_level_number());
  if (lvnum != LEVELNUMBER_ERROR)
  {
    set_continue_level_number(lvnum);
    return true;
  } else
  {
    set_continue_level_number(SINGLEPLAYER_NOTSTARTED);
    return false;
  }
}

TbBool move_campaign_to_prev_level(void)
{
  long lvnum;
  lvnum = prev_singleplayer_level(get_continue_level_number());
  if (lvnum != LEVELNUMBER_ERROR)
  {
    set_continue_level_number(lvnum);
    return true;
  } else
  {
    set_continue_level_number(SINGLEPLAYER_FINISHED);
    return false;
  }
}

short complete_level(struct PlayerInfo *player)
{
  static const char *func_name="complete_level";
  long lvnum;
#if (BFDEBUG_LEVEL > 6)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  //_DK_complete_level(player);
  if (player != &game.players[my_player_number%PLAYERS_COUNT])
    return false;
  if (game.numfield_A & 0x01)
  {
    LbNetwork_Stop();
    quit_game = 1;
    return true;
  }
  lvnum = get_continue_level_number();
  if (get_loaded_level_number() == lvnum)
  {
    move_campaign_to_next_level();
  }
  set_selected_level_number(get_continue_level_number());
  quit_game = 1;
  return true;
}

long init_navigation(void)
{
  return _DK_init_navigation();
}

long update_navigation_triangulation(long start_x, long start_y, long end_x, long end_y)
{
  return _DK_update_navigation_triangulation(start_x, start_y, end_x, end_y);
}

void place_animating_slab_type_on_map(long a1, char a2, unsigned char a3, unsigned char a4, unsigned char a5)
{
  _DK_place_animating_slab_type_on_map(a1,a2,a3,a4,a5);
}

void init_lookups(void)
{
  long i;
  for (i=0; i < THINGS_COUNT; i++)
  {
    game.things_lookup[i] = &game.things_data[i];
  }
  game.things_end = &game.things_data[THINGS_COUNT];

  memset(&game.persons, 0, sizeof(struct Persons));
  for (i=0; i < CREATURES_COUNT; i++)
  {
    game.persons.cctrl_lookup[i] = &game.cctrl_data[i];
  }
  game.persons.cctrl_end = &game.cctrl_data[CREATURES_COUNT];

  for (i=0; i < COLUMNS_COUNT; i++)
  {
    game.columns_lookup[i] = &game.columns[i];
  }
  game.columns_end = &game.columns[COLUMNS_COUNT];
}

void  toggle_ally_with_player(long plyridx, unsigned int allyidx)
{
  struct PlayerInfo *player=&(game.players[plyridx%PLAYERS_COUNT]);
  player->field_2A ^= (1 << allyidx);
}

void set_mouse_light(struct PlayerInfo *player)
{
  static const char *func_name="set_mouse_light";
#if (BFDEBUG_LEVEL > 6)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  _DK_set_mouse_light(player);
}

/*
 * Scales camera zoom level on resolution change. If prev_units_per_pixel_size
 * is zero, then the zoom level will be only clipped, without any scaling.
 */
void keep_camera_zoom_level(struct Camera *cam,unsigned long prev_units_per_pixel_size)
{
  long zoom_val;
  unsigned long zoom_min,zoom_max;
  zoom_min = scale_camera_zoom_to_screen(CAMERA_ZOOM_MIN);
  zoom_max = scale_camera_zoom_to_screen(CAMERA_ZOOM_MAX);
  zoom_val = get_camera_zoom(cam);
  // Note: I don't know if the zoom may be scaled for current resolution,
  // as there may be different resolution on another computer if playing MP game.
  if (prev_units_per_pixel_size > 0)
    zoom_val = zoom_val*units_per_pixel*pixel_size/prev_units_per_pixel_size;
  if (zoom_val < zoom_min)
  {
    zoom_val = zoom_min;
  } else
  if (zoom_val > zoom_max)
  {
    zoom_val = zoom_max;
  }
  set_camera_zoom(cam, zoom_val);
}

/*
 * Scales local player camera zoom level on resolution change. If prev_units_per_pixel_size
 * is zero, then the zoom level will be only clipped, without any scaling.
 */
void keep_local_camera_zoom_level(unsigned long prev_units_per_pixel_size)
{
  struct PlayerInfo *player;
  player = &(game.players[my_player_number%PLAYERS_COUNT]);
  if (player->acamera != NULL)
    keep_camera_zoom_level(player->acamera,prev_units_per_pixel_size);
}

/*
 * Conducts clipping to zoom level of given camera, based on current screen mode.
 */
void update_camera_zoom_bounds(struct Camera *cam,unsigned long zoom_max,unsigned long zoom_min)
{
  static const char *func_name="update_camera_zoom_bounds";
#if (BFDEBUG_LEVEL > 7)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  long zoom_val;
  zoom_val = get_camera_zoom(cam);
  if (zoom_val < zoom_min)
  {
    zoom_val = zoom_min;
  } else
  if (zoom_val > zoom_max)
  {
    zoom_val = zoom_max;
  }
  set_camera_zoom(cam, zoom_val);
}

void magic_use_power_hold_audience(unsigned char idx)
{
  _DK_magic_use_power_hold_audience(idx);
}

void delete_thing_structure(struct Thing *thing, long a2)
{
  _DK_delete_thing_structure(thing, a2);
}

struct Thing *create_effect(struct Coord3d *pos, unsigned short a2, unsigned char a3)
{
  return _DK_create_effect(pos, a2, a3);
}

void create_special_used_effect(struct Coord3d *pos, long a2)
{
  create_effect(pos, 67, a2);
}

void activate_dungeon_special(struct Thing *thing, struct PlayerInfo *player)
{
  static const char *func_name="activate_dungeon_special";
#if (BFDEBUG_LEVEL > 6)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  //_DK_activate_dungeon_special(thing, player); return;
  short used;
  struct Coord3d pos;
  int spkindidx;

  // Gathering data which we'll need if the special is used and disposed.
  memcpy(&pos,&thing->mappos,sizeof(struct Coord3d));
  spkindidx = thing->model - 86;
  used = 0;
  if ((thing->field_0 & 0x01) && is_dungeon_special(thing))
  {
    switch (thing->model)
    {
        case 86:
          reveal_whole_map(player);
          remove_events_thing_is_attached_to(thing);
          used = 1;
          delete_thing_structure(thing, 0);
          break;
        case 87:
          start_resurrect_creature(player, thing);
          break;
        case 88:
          start_transfer_creature(player, thing);
          break;
        case 89:
          if (steal_hero(player, &thing->mappos))
          {
            remove_events_thing_is_attached_to(thing);
            used = 1;
            delete_thing_structure(thing, 0);
          }
          break;
        case 90:
          multiply_creatures(player);
          remove_events_thing_is_attached_to(thing);
          used = 1;
          delete_thing_structure(thing, 0);
          break;
        case 91:
          increase_level(player);
          remove_events_thing_is_attached_to(thing);
          used = 1;
          delete_thing_structure(thing, 0);
          break;
        case 92:
          make_safe(player);
          remove_events_thing_is_attached_to(thing);
          used = 1;
          delete_thing_structure(thing, 0);
          break;
        case 93:
          activate_bonus_level(player);
          remove_events_thing_is_attached_to(thing);
          used = 1;
          delete_thing_structure(thing, 0);
          break;
        default:
          error(func_name, 360, buf_sprintf("Invalid dungeon special (Model %d)", (int)thing->model));
          break;
      }
      if ( used )
      {
        if (is_my_player(player))
          output_message(special_desc[spkindidx].field_8, 0, 1);
        create_special_used_effect(&pos, player->field_2B);
      }
  }
}

void resurrect_creature(struct Thing *thing, unsigned char a2, unsigned char a3, unsigned char a4)
{
  _DK_resurrect_creature(thing, a2, a3, a4);
}

void transfer_creature(struct Thing *tng1, struct Thing *tng2, unsigned char a3)
{
  _DK_transfer_creature(tng1, tng2, a3);
}

void process_rooms(void)
{
  static const char *func_name="process_rooms";
#if (BFDEBUG_LEVEL > 7)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  _DK_process_rooms();
#if (BFDEBUG_LEVEL > 9)
    LbSyncLog("%s: Finished\n",func_name);
#endif
}

void check_players_won(void)
{
  static const char *func_name="check_players_won";
#if (BFDEBUG_LEVEL > 8)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  _DK_check_players_won();
}

void check_players_lost(void)
{
  static const char *func_name="check_players_lost";
#if (BFDEBUG_LEVEL > 8)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  _DK_check_players_lost();
}

void process_dungeon_power_magic(void)
{
  static const char *func_name="process_dungeon_power_magic";
#if (BFDEBUG_LEVEL > 8)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  _DK_process_dungeon_power_magic();
}

void process_dungeon_devastation_effects(void)
{
  static const char *func_name="process_dungeon_devastation_effects";
#if (BFDEBUG_LEVEL > 8)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  _DK_process_dungeon_devastation_effects();
}

void process_entrance_generation(void)
{
  static const char *func_name="process_entrance_generation";
#if (BFDEBUG_LEVEL > 8)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  _DK_process_entrance_generation();
}

void process_things_in_dungeon_hand(void)
{
  _DK_process_things_in_dungeon_hand();
}

void process_payday(void)
{
  _DK_process_payday();
}

void count_dungeon_stuff(void)
{
  struct PlayerInfo *player;
  struct Dungeon *dungeon;
  int i;

  game.field_14E4A4 = 0;
  game.field_14E4A0 = 0;
  game.field_14E49E = 0;

  for (i=0; i < DUNGEONS_COUNT; i++)
  {
    dungeon = (&game.dungeon[i]);
    player = &(game.players[i%PLAYERS_COUNT]);
    if (player->field_0 & 0x01)
    {
      game.field_14E4A0 += dungeon->field_AF9;
      game.field_14E4A4 += dungeon->field_918;
      game.field_14E49E += dungeon->field_919;
    }
  }
}

void process_dungeons(void)
{
  static const char *func_name="process_dungeons";
#if (BFDEBUG_LEVEL > 7)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  //_DK_process_dungeons();
  check_players_won();
  check_players_lost();
  process_dungeon_power_magic();
  count_dungeon_stuff();
  process_dungeon_devastation_effects();
  process_entrance_generation();
  process_payday();
  process_things_in_dungeon_hand();
#if (BFDEBUG_LEVEL > 9)
    LbSyncLog("%s: Finished\n",func_name);
#endif
}

void output_message(long msg_idx, long delay, TbBool queue)
{
  static const char *func_name="output_message";
  struct SMessage *smsg;
  long i;
  smsg = &messages[msg_idx];
  if (game.play_gameturn < smsg->end_time)
    return;
  if (!speech_sample_playing())
  {
    i = get_phrase_sample(get_phrase_for_message(msg_idx));
    if (i == 0) return;
    if (play_speech_sample(i))
    {
      message_playing = msg_idx;
      smsg->end_time = game.play_gameturn + delay;
      return;
    }
  }
  if ((queue) && (msg_idx != message_playing) && (!message_already_in_queue(msg_idx)))
  {
    add_message_to_queue(msg_idx, delay);
  }
}

void process_messages(void)
{
  static const char *func_name="process_messages";
#if (BFDEBUG_LEVEL > 17)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  _DK_process_messages();
#if (BFDEBUG_LEVEL > 19)
    LbSyncLog("%s: Finished\n",func_name);
#endif
}

TbBool message_already_in_queue(long msg_idx)
{
  struct MessageQueueEntry *mqentry;
  long i;
  for (i=0; i < MESSAGE_QUEUE_COUNT; i++)
  {
    mqentry = &message_queue[i];
    if ((mqentry->state == 1) && (msg_idx == mqentry->msg_idx))
      return true;
  }
  return false;
}

TbBool add_message_to_queue(long msg_idx, long a2)
{
  struct MessageQueueEntry *mqentry;
  long i;
  for (i=0; i < MESSAGE_QUEUE_COUNT; i++)
  {
    mqentry = &message_queue[i];
    if (mqentry->state == 0)
    {
      mqentry->state = 1;
      mqentry->msg_idx = msg_idx;
      mqentry->field_5 = a2;
      return true;
    }
  }
  return false;
}

long get_phrase_for_message(long msg_idx)
{
  static const char *func_name="get_phrase_for_message";
  struct SMessage *smsg;
  long i;
  smsg = &messages[msg_idx];
  i = seed_check_random(smsg->count, &game.rand_14BB4E, func_name, 3451);
  return smsg->start_idx + i;
}

long get_phrase_sample(long phr_idx)
{
  return phrases[phr_idx];
}

/*
 * Returns if there is a bonus timer visible on the level.
 */
short bonus_timer_enabled(void)
{
  LevelNumber lvnum;
  lvnum = get_loaded_level_number();
  return (is_bonus_level(lvnum) || is_extra_level(lvnum));
}

void find_nearest_rooms_for_ambient_sound(void)
{
  _DK_find_nearest_rooms_for_ambient_sound();
}

void light_render_area(int startx, int starty, int endx, int endy)
{
  _DK_light_render_area(startx, starty, endx, endy);
}

void process_3d_sounds(void)
{
  _DK_process_3d_sounds();
}

void process_player_research(int plr_idx)
{
  _DK_process_player_research(plr_idx);
}

struct Room *player_has_room_of_type(long plr_idx, long roomkind)
{
  return _DK_player_has_room_of_type(plr_idx, roomkind);
}

struct Room *find_room_with_spare_room_item_capacity(unsigned char a1, signed char a2)
{
  return _DK_find_room_with_spare_room_item_capacity(a1, a2);
}

long create_workshop_object_in_workshop_room(long a1, long a2, long a3)
{
  return _DK_create_workshop_object_in_workshop_room(a1, a2, a3);
}

long get_next_manufacture(struct Dungeon *dungeon)
{
  return _DK_get_next_manufacture(dungeon);
}

void remove_thing_from_mapwho(struct Thing *thing)
{
  static const char *func_name="remove_thing_from_mapwho";
#if (BFDEBUG_LEVEL > 8)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  _DK_remove_thing_from_mapwho(thing);
}

void place_thing_in_mapwho(struct Thing *thing)
{
  static const char *func_name="place_thing_in_mapwho";
#if (BFDEBUG_LEVEL > 18)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  _DK_place_thing_in_mapwho(thing);
}

long get_thing_height_at(struct Thing *thing, struct Coord3d *pos)
{
  static const char *func_name="get_thing_height_at";
#if (BFDEBUG_LEVEL > 18)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  return _DK_get_thing_height_at(thing, pos);
}

long manufacture_required(long mfcr_type, unsigned long mfcr_kind, const char *func_name)
{
  switch (mfcr_type)
  {
  case 8:
      return game.traps_config[mfcr_kind%TRAP_TYPES_COUNT].manufct_required;
  case 9:
      return game.doors_config[mfcr_kind%DOOR_TYPES_COUNT].manufct_required;
  default:
      error(func_name, 310, "Invalid type of manufacture");
      return 0;
  }
}

short process_player_manufacturing(long plr_idx)
{
  struct Dungeon *dungeon;
  struct PlayerInfo *player;
  int i,k;
  static const char *func_name="process_player_manufacturing";
#if (BFDEBUG_LEVEL > 17)
    LbSyncLog("%s: Starting\n",func_name);
#endif
//  return _DK_process_player_manufacturing(plr_idx);

  dungeon = &(game.dungeon[plr_idx%DUNGEONS_COUNT]);
  if (player_has_room_of_type(plr_idx, 8) == NULL)
    return true;
  if (dungeon->field_1189 == 0)
  {
    get_next_manufacture(dungeon);
    return true;
  }
  k = manufacture_required(dungeon->field_1189, dungeon->field_118A, func_name);
  if (dungeon->field_1185 < (k << 8))
    return true;

  if (find_room_with_spare_room_item_capacity(plr_idx, 8) == NULL)
  {
    dungeon->field_1189 = 0;
    return false;
  }
  if (create_workshop_object_in_workshop_room(plr_idx, dungeon->field_1189, dungeon->field_118A) == 0)
  {
    error(func_name, 173, "Could not create manufactured item");
    return false;
  }

  switch (dungeon->field_1189)
  {
  case 8:
      i = dungeon->field_118A%TRAP_TYPES_COUNT;
      if (dungeon->trap_amount[i] >= 199)
      {
        error(func_name, 159, "Bad trap choice for manufacturing - limit reached");
        return false;
      }
      dungeon->trap_amount[i]++;
      dungeon->lvstats.manufactured_traps++;
      dungeon->trap_placeable[i] = 1;
      // If that's local player - make a message
      player=&(game.players[my_player_number%PLAYERS_COUNT]);
      if (player->field_2B == plr_idx)
        output_message(45, 0, 1);
      break;
  case 9:
      i = dungeon->field_118A%DOOR_TYPES_COUNT;
      if (dungeon->door_amount[i] >= 199)
      {
        error(func_name, 139, "Bad door choice for manufacturing - limit reached");
        return 0;
      }
      dungeon->door_amount[i]++;
      dungeon->lvstats.manufactured_doors++;
      dungeon->door_placeable[i] = 1;
      // If that's local player - make a message
      player=&(game.players[my_player_number%PLAYERS_COUNT]);
      if (player->field_2B == plr_idx)
        output_message(44, 0, 1);
      break;
  default:
      error(func_name, 166, "Invalid type of new manufacture");
      return false;
  }

  dungeon->field_1185 -= (k << 8);
  dungeon->field_118B = game.play_gameturn;
  dungeon->lvstats.manufactured_items++;
  get_next_manufacture(dungeon);
  return true;
}

struct Event *event_allocate_free_event_structure(void)
{
  struct Event *event;
  long i;
  for (i=1; i < EVENTS_COUNT; i++)
  {
    event = &game.event[i];
    if ((event->field_0 & 0x01) == 0)
    {
      event->field_0 |= 0x01;
      event->field_1 = i;
      return event;
    }
  }
  return NULL;
}

void event_initialise_event(struct Event *event, long map_x, long map_y, unsigned char evkind, unsigned char dngn_id, long msg_id)
{
  static const char *func_name="event_initialise_event";
  event->mappos_x = map_x;
  event->mappos_y = map_y;
  event->kind = evkind;
  event->owner = dngn_id;
  event->birth_turn = event_button_info[evkind].field_8;
  event->field_C = msg_id;
  event->field_14 = 1;
}

void event_delete_event_structure(long ev_idx)
{
  LbMemorySet(&game.event[ev_idx], 0, sizeof(struct Event));
}

void event_delete_event(long plyr_idx, long ev_idx)
{
  struct Dungeon *dungeon;
  struct Event *event;
  long i,k;
//  _DK_event_delete_event(plridx, num);
  event = &game.event[ev_idx];
  dungeon = &(game.dungeon[plyr_idx%DUNGEONS_COUNT]);
  dungeon->field_13B4[event->kind%EVENT_KIND_COUNT] = game.play_gameturn;
  for (i=0; i <= EVENT_BUTTONS_COUNT; i++)
  {
    k = dungeon->field_13A7[i];
    if (k == ev_idx)
    {
      turn_off_event_box_if_necessary(plyr_idx, k);
      dungeon->field_13A7[i] = 0;
      break;
    }
  }
  event_delete_event_structure(ev_idx);
}

void event_add_to_event_list(struct Event *event, struct Dungeon *dungeon)
{
  static const char *func_name="event_add_to_event_list";
  long i,k;
  for (i=EVENT_BUTTONS_COUNT; i > 0; i--)
  {
    k = dungeon->field_13A7[i];
    if (k == 0)
    {
      if (dungeon->field_E9F != event->owner)
      {
        error(func_name, 74, "Illegal my_event player allocation");
      }
      dungeon->field_13A7[i] = event->field_1;
      break;
    }
  }
  if (i == 0)
  {
    kill_oldest_my_event(dungeon);
    dungeon->field_13A7[EVENT_BUTTONS_COUNT] = event->field_1;
  }
}

void maintain_my_event_list(struct Dungeon *dungeon)
{
  _DK_maintain_my_event_list(dungeon); return;
}

void kill_oldest_my_event(struct Dungeon *dungeon)
{
  struct Event *event;
  long old_idx;
  long old_birth;
  long i,k;
  old_idx = -1;
  old_birth = 2147483647;
  for (i=12; i > 0; i--)
  {
    k = dungeon->field_13A7[i];
    event = &game.event[k];
    if ((event->birth_turn >= 0) && (event->birth_turn < old_birth))
    {
      old_idx = k;
      old_birth = event->birth_turn;
    }
  }
  if (old_idx >= 0)
    event_delete_event(dungeon->field_E9F, old_idx);
  maintain_my_event_list(dungeon);
}

void maintain_all_players_event_lists(void)
{
  struct PlayerInfo *player;
  struct Dungeon *dungeon;
  long i;
  for (i=0; i < PLAYERS_COUNT; i++)
  {
    player = &(game.players[i]);
    if ((player->field_0 & 0x01) != 0)
    {
      dungeon = &(game.dungeon[i%DUNGEONS_COUNT]);
      maintain_my_event_list(dungeon);
    }
  }
}

struct Thing *event_is_attached_to_thing(long ev_idx)
{
  struct Event *event;
  long i;
  event = &game.event[ev_idx];
  switch (event->kind)
  {
  case 3:
  case 6:
  case 10:
  case 14:
  case 16:
  case 17:
  case 24:
  case 25:
  case 26:
      i = event->field_C;
      break;
  default:
      i = 0;
      break;
  }
  if ((i > 0) && (i < THINGS_COUNT))
    return game.things_lookup[i];
  return NULL;
}

void event_process_events(void)
{
  _DK_event_process_events();
}

void update_all_events(void)
{
  struct Thing *thing;
  struct Event *event;
  long i;
//  _DK_update_all_events();
  for (i=EVENT_BUTTONS_COUNT; i > 0; i--)
  {
    thing = event_is_attached_to_thing(i);
    if (!thing_is_invalid(thing))
    {
      event = &game.event[i];
      if ((thing->class_id == TCls_Creature) && ((thing->field_0 & 0x10) || (thing->field_1 & 0x02)))
      {
        event->mappos_x = 0;
        event->mappos_y = 0;
      } else
      {
        event->mappos_x = thing->mappos.x.val;
        event->mappos_y = thing->mappos.y.val;
      }
    }
  }
  maintain_all_players_event_lists();
}

void process_level_script(void)
{
  static const char *func_name="process_level_script";
#if (BFDEBUG_LEVEL > 6)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  struct PlayerInfo *player;
  //_DK_process_level_script(); return;
  player = &(game.players[my_player_number%PLAYERS_COUNT]);
  if (((game.numfield_A & 0x01) == 0) && (player->victory_state != VicS_Undecided))
    return;
  process_conditions();
  process_check_new_creature_partys();
//script_process_messages(); is not here, but it is in beta - check why
  process_check_new_tunneller_partys();
  process_values();
  process_win_and_lose_conditions(player->field_2B);
//  show_onscreen_msg(8, "Flags %d %d %d %d %d %d", game.dungeon[0].script_flags[0],game.dungeon[0].script_flags[1],
//    game.dungeon[0].script_flags[2],game.dungeon[0].script_flags[3],game.dungeon[0].script_flags[4],game.dungeon[0].script_flags[5]);
#if (BFDEBUG_LEVEL > 19)
    LbSyncLog("%s: Finished\n",func_name);
#endif
}

long PaletteFadePlayer(struct PlayerInfo *player)
{
  return _DK_PaletteFadePlayer(player);
}

void message_update(void)
{
  static const char *func_name="message_update";
#if (BFDEBUG_LEVEL > 6)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  _DK_message_update();
}

long wander_point_update(struct Wander *wandr)
{
  return _DK_wander_point_update(wandr);
}

void update_player_camera(struct PlayerInfo *player)
{
  _DK_update_player_camera(player);
}

void update_research(void)
{
  static const char *func_name="update_research";
  int i;
  struct PlayerInfo *player;
#if (BFDEBUG_LEVEL > 6)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  for (i=0; i<PLAYERS_COUNT; i++)
  {
      player=&(game.players[i]);
      if ((player->field_0 & 0x01) && (player->field_2C == 1))
      {
          process_player_research(i);
      }
  }
}

void update_manufacturing(void)
{
  static const char *func_name="update_manufacturing";
  int i;
  struct PlayerInfo *player;
#if (BFDEBUG_LEVEL > 6)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  for (i=0; i<PLAYERS_COUNT; i++)
  {
      player=&(game.players[i]);
      if ((player->field_0 & 0x01) && (player->field_2C == 1))
      {
          process_player_manufacturing(i);
      }
  }
}

void update_all_players_cameras(void)
{
  static const char *func_name="update_all_players_cameras";
  int i;
  struct PlayerInfo *player;
#if (BFDEBUG_LEVEL > 6)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  for (i=0; i<PLAYERS_COUNT; i++)
  {
    player=&(game.players[i]);
    if ((player->field_0 & 0x01) && ((player->field_0 & 0x40) == 0))
    {
          update_player_camera(player);
    }
  }
}

#define LIGHT_MAX_RANGE 30
void update_light_render_area(void)
{
  static const char *func_name="update_light_render_area";
  int subtile_x,subtile_y;
  int delta_x,delta_y;
  int startx,endx,starty,endy;
  struct PlayerInfo *player;
#if (BFDEBUG_LEVEL > 6)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  player=&(game.players[my_player_number%PLAYERS_COUNT]);
  if (player->field_37 >= 1)
    if ((player->field_37 <= 2) || (player->field_37 == 5))
    {
        game.field_14BB5D = LIGHT_MAX_RANGE;
        game.field_14BB59 = LIGHT_MAX_RANGE;
    }
  delta_x=abs(game.field_14BB59);
  delta_y=abs(game.field_14BB5D);
  // Prepare the area constraits
  if (player->acamera != NULL)
  {
    subtile_y = player->acamera->mappos.y.stl.num;
    subtile_x = player->acamera->mappos.x.stl.num;
  } else
  {
    subtile_y = 0;
    subtile_x = 0;
  }
//LbSyncLog("LghtRng %d,%d CamTil %d,%d\n",game.field_14BB59,game.field_14BB5D,tile_x,tile_y);
  if (subtile_y > delta_y)
  {
    starty = subtile_y - delta_y;
    if (starty > map_subtiles_y) starty = map_subtiles_y;
  } else
    starty = 0;
  if (subtile_x > delta_x)
  {
    startx = subtile_x - delta_x;
    if (startx > map_subtiles_x) startx = map_subtiles_x;
  } else
    startx = 0;
  endy = subtile_y + delta_y;
  if (endy < starty) endy = starty;
  if (endy > map_subtiles_y) endy = map_subtiles_y;
  endx = subtile_x + delta_x;
  if (endx < startx) endx = startx;
  if (endx > map_subtiles_x) endx = map_subtiles_x;
  // Set the area
  light_render_area(startx, starty, endx, endy);
}

void update_flames_nearest_camera(struct Camera *camera)
{
  if (camera == NULL)
    return;
  _DK_update_flames_nearest_camera(camera);
}

void update_footsteps_nearest_camera(struct Camera *camera)
{
  static const char *func_name="update_footsteps_nearest_camera";
#if (BFDEBUG_LEVEL > 6)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  if (camera == NULL)
    return;
  _DK_update_footsteps_nearest_camera(camera);
}

void process_player_states(void)
{
  static const char *func_name="process_player_states";
#if (BFDEBUG_LEVEL > 6)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  _DK_process_player_states();
}

void update_power_sight_explored(struct PlayerInfo *player)
{
  static const char *func_name="update_power_sight_explored";
#if (BFDEBUG_LEVEL > 6)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  _DK_update_power_sight_explored(player);
}

void set_level_objective(char *msg_text)
{
  strncpy(game.evntbox_text_objective, msg_text, MESSAGE_TEXT_LEN);
  new_objective = 1;
}

void update_player_objectives(int plridx)
{
  static const char *func_name="update_player_objectives";
  struct PlayerInfo *player;
#if (BFDEBUG_LEVEL > 6)
    LbSyncLog("%s: Starting for player %d\n",func_name,plridx);
#endif
  player=&(game.players[plridx]);
  if (game.numfield_A & 0x01)
  {
    if ((!player->field_4EB) && (player->victory_state != VicS_Undecided))
      player->field_4EB = game.play_gameturn+1;
  }
  if (player->field_4EB == game.play_gameturn)
  {
    switch (player->victory_state)
    {
    case VicS_WonLevel:
        if (plridx == my_player_number)
          set_level_objective(gui_strings[0]); // Success message
        display_objectives(player->field_2B, 0, 0);
        break;
    case VicS_LostLevel:
        if (plridx == my_player_number)
          set_level_objective(gui_strings[335]); // Defeated message
        display_objectives(player->field_2B, 0, 0);
        break;
    }
  }
}

void process_players(void)
{
  static const char *func_name="process_players";
  int i;
  struct PlayerInfo *player;
#if (BFDEBUG_LEVEL > 5)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  process_player_instances();
  process_player_states();
  for (i=0; i<PLAYERS_COUNT; i++)
  {
      player=&(game.players[i]);
      if ((player->field_0 & 0x01) && (player->field_2C == 1))
      {
  #if (BFDEBUG_LEVEL > 6)
      LbSyncLog("%s: Doing updates for player %d\n",func_name,i);
  #endif
          wander_point_update(&player->wandr1);
          wander_point_update(&player->wandr2);
          update_power_sight_explored(player);
          update_player_objectives(i);
      }
  }
#if (BFDEBUG_LEVEL > 17)
    LbSyncLog("%s: Finished\n",func_name);
#endif
}

short update_animating_texture_maps(void)
{
  static const char *func_name="update_animating_texture_maps";
  int i;
#if (BFDEBUG_LEVEL > 18)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  anim_counter = (anim_counter+1) % 8;
  short result=true;
  for (i=0; i<TEXTURE_BLOCKS_ANIM_COUNT; i++)
  {
        short j = game.texture_animation[8*i+anim_counter];
        if ((j>=0) && (j<TEXTURE_BLOCKS_STAT_COUNT))
        {
          block_ptrs[TEXTURE_BLOCKS_STAT_COUNT+i] = block_ptrs[j];
        } else
        {
          result=false;
        }
  }
  return result;
}

void add_creature_to_pool(long kind, long amount, unsigned long a3)
{
  long prev_amount;
  prev_amount = game.pool.crtr_kind[kind%CREATURE_TYPES_COUNT];
  if ((a3 == 0) || (prev_amount != -1))
  {
    if ((amount != -1) && (amount != 0) && (prev_amount != -1))
      game.pool.crtr_kind[kind%CREATURE_TYPES_COUNT] = prev_amount+amount;
    else
      game.pool.crtr_kind[kind%CREATURE_TYPES_COUNT] = amount;
  }
}

short update_creature_pool_state(void)
{
  int i;
  game.pool.is_empty = true;
  for (i=1; i < CREATURE_TYPES_COUNT; i++)
  {
      if (game.pool.crtr_kind[i] > 0)
      { game.pool.is_empty = false; break; }
  }
  return true;
}

int clear_active_dungeons_stats(void)
{
  struct Dungeon *dungeon;
  int i;
  for (i=0; i<=(game.field_14E496%DUNGEONS_COUNT); i++)
  {
      dungeon = &(game.dungeon[i]);
      memset((char *)dungeon->field_64, 0, 480 * sizeof(short));
      memset((char *)dungeon->field_424, 0, CREATURE_TYPES_COUNT*3*sizeof(unsigned short));
      memset((char *)dungeon->field_4E4, 0, CREATURE_TYPES_COUNT*3*sizeof(unsigned short));
  }
  return i;
}

void draw_slab64k(long pos_x, long pos_y, long width, long height)
{
  _DK_draw_slab64k(pos_x, pos_y, width, height);
}

short update_3d_sound_receiver(struct PlayerInfo *player)
{
  struct Camera *camera;
  if (player->acamera == NULL)
    return false;
  camera = player->acamera;
  S3DSetSoundReceiverPosition(camera->mappos.x.val,camera->mappos.y.val,camera->mappos.z.val);
  S3DSetSoundReceiverOrientation(camera->orient_a,camera->orient_b,camera->orient_c);
  return true;
}

long update_cave_in(struct Thing *thing)
{
  return _DK_update_cave_in(thing);
}

void update_player_sounds(void)
{
  int k;
  struct PlayerInfo *player;
  static const char *func_name="update_player_sounds";
#if (BFDEBUG_LEVEL > 7)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  if ((game.numfield_C & 0x01) == 0)
  {
    player = &(game.players[my_player_number%PLAYERS_COUNT]);
    process_messages();
    if (!SoundDisabled)
    {
      if ((game.flags_cd & MFlg_NoMusic) == 0)
      {
        if (game.audiotrack > 0)
          PlayRedbookTrack(game.audiotrack);
      }
      update_3d_sound_receiver(player);
    }
    game.play_gameturn++;
  }
  find_nearest_rooms_for_ambient_sound();
  process_3d_sounds();
  k = (game.bonus_time-game.play_gameturn) / 2;
  if (bonus_timer_enabled())
  {
    if ((game.bonus_time == game.play_gameturn) ||
        (game.bonus_time > game.play_gameturn) && ((k<=100) && ((k % 10) == 0) ||
        (k<=300) && ((k % 50)==0) || ((k % 250)==0)) )
      play_non_3d_sample(89);
  }
#if (BFDEBUG_LEVEL > 9)
    LbSyncLog("%s: Finished\n",func_name);
#endif
}

void update(void)
{
  static const char *func_name="update";
  struct PlayerInfo *player;
  int i,k;
#if (BFDEBUG_LEVEL > 4)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  //_DK_update();return;

  if ((game.numfield_C & 0x01) == 0)
    update_light_render_area();
  process_packets();
  if (quit_game)
  {
    return;
  }
  if (game.flagfield_14EA4A == 1)
  {
    game.field_14EA4B = 0;
    return;
  }

  if ((game.numfield_C & 0x01) == 0)
  {
    player = &(game.players[my_player_number%PLAYERS_COUNT]);
    if (player->field_3 & 0x08)
    {
      PaletteSetPlayerPalette(player, _DK_palette);
      set_flag_byte(&player->field_3,0x08,false);
    }
    clear_active_dungeons_stats();
    update_creature_pool_state();
    if ((game.play_gameturn & 0x01) != 0)
      update_animating_texture_maps();
    update_things();
    process_rooms();
    process_dungeons();
    update_research();
    update_manufacturing();
    event_process_events();
    update_all_events();
    process_level_script();
    if (game.numfield_D & 0x04)
      process_computer_players2();
    process_players();
    process_action_points();
    player = &(game.players[my_player_number%PLAYERS_COUNT]);
    if (player->field_37 == 1)
      update_flames_nearest_camera(player->acamera);
    update_footsteps_nearest_camera(player->acamera);
    PaletteFadePlayer(player);
    process_armageddon();
  }

  message_update();
  update_all_players_cameras();
  update_player_sounds();

  // Rare message easter egg
  if ((game.play_gameturn != 0) && ((game.play_gameturn % 0x4E20) == 0))
  {
      if (seed_check_random(0x7D0u, &game.field_14BB4A, func_name, 4345) == 0)
      {
        if (seed_check_random(10, &game.rand_14BB4E, func_name, 4346) == 7)
        {
          output_message(94, 0, 1);// 'Your pants are definitely too tight'
        } else
        {
          output_message((game.rand_14BB4E % 10) + 91, 0, 1);
        }
      }
  }
  game.field_14EA4B = 0;
#if (BFDEBUG_LEVEL > 6)
    LbSyncLog("%s: Finished\n",func_name);
#endif
}

long map_fade_in(long a)
{
  static const char *func_name="map_fade_in";
#if (BFDEBUG_LEVEL > 6)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  return _DK_map_fade_in(a);
}

long map_fade_out(long a)
{
  static const char *func_name="map_fade_out";
#if (BFDEBUG_LEVEL > 6)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  return _DK_map_fade_out(a);
}

void draw_2d_map(void)
{
  _DK_draw_2d_map();
}

/*
 * Strange name to hide easter eggs ;). Displays easter egg messages on screen.
 */
void draw_sound_stuff(void)
{
  static const char *func_name="draw_sound_stuff";
  char *text;
  int i;
#if (BFDEBUG_LEVEL > 5)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  LbTextSetWindow(0, 0, MyScreenWidth, MyScreenHeight);
  if (eastegg03_cntr > 6)
  {
      unsigned char pos;
      eastegg03_cntr++;
      lbFontPtr = winfont;
      text=buf_sprintf("Dene says a big 'Hello' to Goth Buns, Tarts and Barbies");
      lbDisplay.DrawFlags = 0x40;
      for (i=0; i<30; i+=2)
      {
        pos = game.play_gameturn - i;
        lbDisplay.DrawColour = pos;
        LbTextDraw((LbCosL(16*pos) / 512 + 120) / pixel_size,
          (LbSinL(32*pos) / 512 + 200) / pixel_size, text);
      }
      set_flag_word(&lbDisplay.DrawFlags,0x0040,false);
      pos=game.play_gameturn;
      LbTextDraw((LbCosL(16*pos) / 512 + 120) / pixel_size,
        (LbSinL(32*pos) / 512 + 200) / pixel_size, text);
      if (eastegg03_cntr >= 255)
        eastegg03_cntr = 0;
  }
  _DK_draw_sound_stuff();
}

long power_sight_explored(long stl_x, long stl_y, unsigned char plyr_idx)
{
  return _DK_power_sight_explored(stl_x, stl_y, plyr_idx);
}

long can_cast_spell_on_creature(long a1, struct Thing *thing, long a3)
{
  return _DK_can_cast_spell_on_creature(a1, thing, a3);
}

unsigned char can_cast_spell_at_xy(unsigned char plyr_idx, unsigned char spl_id, unsigned char stl_x, unsigned char stl_y, long a5)
{
  struct PlayerInfo *player;
  struct Map *mapblk;
  struct SlabMap *slb;
  TbBool can_cast;
  //return _DK_can_cast_spell_at_xy(plyr_idx, spl_id, stl_x, stl_y, a5);
  mapblk = get_map_block(stl_x, stl_y);
  slb = get_slabmap_for_subtile(stl_x, stl_y);
  can_cast = false;
  switch (spl_id)
  {
  default:
      if ((mapblk->flags & 0x10) == 0)
      {
        can_cast = true;
      }
      break;
  case 2:
      if ((mapblk->flags & 0x10) == 0)
      {
        if (slabmap_owner(slb) == plyr_idx)
        {
          can_cast = true;
        }
      }
      break;
  case 5:
      can_cast = true;
      break;
  case 6:
      if ((mapblk->flags & 0x10) == 0)
      {
        if (map_block_revealed(mapblk, plyr_idx) || (a5 == 1))
        {
          can_cast = true;
        }
      }
      break;
  case 7:
      if ((mapblk->flags & 0x10) == 0)
      {
        if (power_sight_explored(stl_x, stl_y, plyr_idx) || map_block_revealed(mapblk, plyr_idx))
        {
          can_cast = true;
        }
      }
      break;
  case 10:
      if ((mapblk->flags & 0x10) == 0)
      {
        if (power_sight_explored(stl_x, stl_y, plyr_idx) || map_block_revealed(mapblk, plyr_idx))
        {
          player = &(game.players[plyr_idx%PLAYERS_COUNT]);
          if (player->field_4E3+20 < game.play_gameturn)
          {
            can_cast = true;
          }
        }
      }
      break;
  case 14:
  case 15:
      if (slabmap_owner(slb) == plyr_idx)
      {
        can_cast = true;
      }
      break;
  case 16:
      if (power_sight_explored(stl_x, stl_y, plyr_idx) || map_block_revealed(mapblk, plyr_idx))
      {
        if ((mapblk->flags & 0x10) != 0)
        {
          if ((mapblk->flags & 0x43) == 0)
          {
            if (slb->slab != 0)
              can_cast = true;
          }
        }
      }
      break;
  }
  return can_cast;
}

void draw_spell_cursor(unsigned char wrkstate, unsigned short tng_idx, unsigned char stl_x, unsigned char stl_y)
{
  static const char *func_name="draw_spell_cursor";
  struct PlayerInfo *player;
  struct Thing *thing;
  Expand_Check_Func chkfunc;
  TbBool allow_cast;
  long spl_id;
  long i;
  //_DK_draw_spell_cursor(wrkstate, tng_idx, stl_x, stl_y); return;
  spl_id = -1;
  if (wrkstate < PLAYER_STATES_COUNT)
    spl_id = player_state_to_spell[wrkstate];
#if (BFDEBUG_LEVEL > 5)
    LbSyncLog("%s: Starting for spell %ld\n",func_name,spl_id);
#endif
  if (spl_id <= 0)
  {
    set_pointer_graphic(0);
    return;
  }
  player = &(game.players[my_player_number%PLAYERS_COUNT]);
  thing = thing_get(tng_idx);
  allow_cast = false;
  if (!thing_is_invalid(thing) || (thing->owner == player->field_2B) || (spell_data[spl_id].flag_1A != 0))
  {
    if (can_cast_spell_at_xy(player->field_2B, spl_id, stl_x, stl_y, 0))
    {
      if (!thing_is_invalid(thing) || can_cast_spell_on_creature(player->field_2B, thing, spl_id))
        allow_cast = true;
    }
  }
  if (!allow_cast)
  {
    set_pointer_graphic(15);
    return;
  }
  chkfunc = spell_data[spl_id].field_15;
  if (chkfunc != NULL)
  {
    if (chkfunc())
    {
      i = player->field_4D2/4;
      if (i > 8)
        i = 8;
      set_pointer_graphic(16+i);
      draw_spell_cost = game.magic_stats[spl_id].cost[i];
      return;
    }
  }
  i = spell_data[spl_id].field_13;
  set_pointer_graphic_spell(i, game.play_gameturn);
}

void process_pointer_graphic(void)
{
  static const char *func_name="process_pointer_graphic";
  struct PlayerInfo *player;
  struct Dungeon *dungeon;
  struct Thing *thing;
  long i;
  //_DK_process_pointer_graphic(); return;
  player = &(game.players[my_player_number%PLAYERS_COUNT]);
  dungeon = &(game.dungeon[player->field_2B%DUNGEONS_COUNT]);
#if (BFDEBUG_LEVEL > 5)
    LbSyncLog("%s: Starting for view %d, player state %d, instance %d\n",func_name,(int)player->view_type,(int)player->work_state,(int)player->instance_num);
#endif
  switch (player->view_type)
  {
  case 1:
      if (player->instance_num == PI_MapFadeFrom)
      {
        set_pointer_graphic(0);
      } else
      if (((game.numfield_C & 0x20) != 0) && mouse_is_over_small_map(player->mouse_x, player->mouse_y))
      {
        if (game.small_map_state == 2)
          set_pointer_graphic(0);
        else
          set_pointer_graphic(1);
      } else
      if (battle_creature_over > 0)
      {
        i = -1;
        if (player->work_state < PLAYER_STATES_COUNT)
          i = player_state_to_spell[player->work_state];
        if ((i > 0) && (spell_data[i].flag_19))
        {
          thing = thing_get(battle_creature_over);
          draw_spell_cursor(player->work_state, battle_creature_over,
              thing->mappos.x.stl.num, thing->mappos.y.stl.num);
        } else
        {
          set_pointer_graphic_none();
        }
      } else
      if (game_is_busy_doing_gui())
      {
        set_pointer_graphic(1);
      } else
      switch (player->work_state)
      {
      case 1:
          if (player->field_455)
            i = player->field_455;
          else
            i = player->field_454;
          switch (i)
          {
          case 1:
              set_pointer_graphic(2);
              break;
          case 2:
              set_pointer_graphic(39);
              break;
          case 3:
              thing = thing_get(player->thing_under_hand);
              if ((!thing_is_invalid(thing)) && (player->field_4) && (dungeon->things_in_hand[0] != player->thing_under_hand)
                  && can_thing_be_possessed(thing, player->field_2B))
              {
                if (is_feature_on(Ft_BigPointer))
                {
                  set_pointer_graphic(96+(game.play_gameturn%i));
                } else
                {
                  set_pointer_graphic(47);
                }
                player->field_6 |= 0x01;
              } else
              if ((!thing_is_invalid(thing)) && (player->field_5) && (dungeon->things_in_hand[0] != player->thing_under_hand)
                  && can_thing_be_queried(thing, player->field_2B))
              {
                set_pointer_graphic(4);
                player->field_6 |= 0x01;
              } else
              {
                if ((player->field_3 & 0x02) != 0)
                  set_pointer_graphic(2);
                else
                  set_pointer_graphic(0);
              }
              break;
          default:
              if (player->field_10 <= game.play_gameturn)
                set_pointer_graphic(1);
              else
                set_pointer_graphic(0);
              break;
          }
          break;
      case 2:
          switch (player->field_4A3)
          {
          case 2:
              set_pointer_graphic(25);
              break;
          case 3:
              set_pointer_graphic(27);
              break;
          case 4:
              set_pointer_graphic(29);
              break;
          case 5:
              set_pointer_graphic(28);
              break;
          case 6:
              set_pointer_graphic(30);
              break;
          case 8:
              set_pointer_graphic(34);
              break;
          case 9:
              set_pointer_graphic(35);
              break;
          case 10:
              set_pointer_graphic(33);
              break;
          case 11:
              set_pointer_graphic(32);
              break;
          case 12:
              set_pointer_graphic(31);
              break;
          case 13:
              set_pointer_graphic(26);
              break;
          case 14:
              set_pointer_graphic(36);
              break;
          case 15:
              set_pointer_graphic(37);
              break;
          case 16:
              set_pointer_graphic(38);
              break;
          }
          return;
      case 5:
      case 9:
          set_pointer_graphic(0);
          break;
      case 6:
      case 7:
      case 8:
      case 10:
      case 11:
      case 17:
      case 19:
      case 20:
      case 21:
      case 22:
      case 24:
      case 25:
      case 26:
      case 27:
          draw_spell_cursor(player->work_state, 0, game.pos_14C006.x.stl.num, game.pos_14C006.y.stl.num);
          break;
      case 12:
      case 15:
          set_pointer_graphic(4);
          break;
      case 16:
          switch (player->field_4A5)
          {
          case 1:
              set_pointer_graphic(5);
              break;
          case 2:
              set_pointer_graphic(9);
              break;
          case 3:
              set_pointer_graphic(7);
              break;
          case 4:
              set_pointer_graphic(8);
              break;
          case 5:
              set_pointer_graphic(6);
              break;
          case 6:
              set_pointer_graphic(10);
              break;
          }
          return;
      case 18:
          switch (player->field_4A6)
          {
          case 1:
              set_pointer_graphic(11);
              break;
          case 2:
              set_pointer_graphic(12);
              break;
          case 3:
              set_pointer_graphic(13);
              break;
          case 4:
              set_pointer_graphic(14);
              break;
          }
          return;
      case 23:
          set_pointer_graphic(3);
          break;
      default:
          set_pointer_graphic(1);
          break;
      }
      break;
  case 2:
  case 3:
      if ((game.numfield_D & 0x08) != 0)
        set_pointer_graphic(1);
      else
        set_pointer_graphic(0);
      break;
  case 4:
  case 5:
  case 6:
      set_pointer_graphic(1);
      break;
  case 0:
      set_pointer_graphic_none();
      return;
  default:
      LbWarnLog("%s: Unsupported view type\n",func_name);
      set_pointer_graphic_none();
      return;
  }
}

void draw_bonus_timer(void)
{
  _DK_draw_bonus_timer(); return;
}

void message_draw(void)
{
  static const char *func_name="message_draw";
  int i,h;
  long x,y;
#if (BFDEBUG_LEVEL > 7)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  lbFontPtr = winfont;
  h = LbTextHeight("Wg");
  x = 148;
  y = 28;
  for (i=0; i < game.active_messages_count; i++)
  {
      LbTextSetWindow(0, 0, MyScreenWidth, MyScreenHeight);
      set_flag_word(&lbDisplay.DrawFlags,0x0040,false);
      LbTextDraw((x+32)/pixel_size, y/pixel_size, game.messages[i].text);
      draw_gui_panel_sprite_left(x, y, 488+game.messages[i].field_40);
      y += pixel_size * h;
  }
}

void draw_lens(unsigned char *dstbuf, unsigned char *srcbuf, unsigned long *lens_mem, int width, int height, int scanln)
{
  _DK_draw_lens(dstbuf, srcbuf, lens_mem, width, height, scanln);
}

void flyeye_blitsec(unsigned char *srcbuf, unsigned char *dstbuf, long srcwidth, long dstwidth, long n, long height)
{
  _DK_flyeye_blitsec(srcbuf, dstbuf, srcwidth, dstwidth, n, height);
}

void draw_mini_things_in_hand(long x, long y)
{
  _DK_draw_mini_things_in_hand(x, y);
}

void process_keeper_sprite(short x, short y, unsigned short a3, short a4, unsigned char a5, long a6)
{
  _DK_process_keeper_sprite(x, y, a3, a4, a5, a6);
}

void draw_power_hand(void)
{
  static const char *func_name="draw_power_hand";
#if (BFDEBUG_LEVEL > 7)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  struct PlayerInfo *player;
  struct Dungeon *dungeon;
  struct CreatureControl *cctrl;
  struct Thing *thing;
  struct Thing *picktng;
  struct Room *room;
  struct RoomData *rdata;
  long x,y;
  long i;
  //_DK_draw_power_hand(); return;
  player = &(game.players[my_player_number%PLAYERS_COUNT]);
  dungeon = &(game.dungeon[player->field_2B%DUNGEONS_COUNT]);
  if ((player->field_6 & 0x01) != 0)
    return;
  if (game.small_map_state == 2)
    return;
  lbDisplay.DrawFlags = 0x00;
  if (player->view_type != 1)
    return;
  if (((game.numfield_C & 0x20) != 0) && (game.small_map_state != 2)
    && mouse_is_over_small_map(player->mouse_x, player->mouse_y) )
  {
    x = game.field_1517FD;
    y = game.field_1517FF;
    room = subtile_room_get(x,y);
    if ((!room_is_invalid(room)) && (subtile_revealed(x, y, player->field_2B)))
    {
      rdata = room_data_get_for_room(room);
      draw_gui_panel_sprite_centered(GetMouseX()+24, GetMouseY()+32, rdata->numfield_1);
    }
    if ((!power_hand_is_empty(player)) && (game.small_map_state == 1))
    {
      draw_mini_things_in_hand(GetMouseX()+10, GetMouseY()+10);
    }
    return;
  }
  if (game_is_busy_doing_gui())
  {
    draw_mini_things_in_hand(GetMouseX()+10, GetMouseY()+10);
    return;
  }
  thing = thing_get(player->field_43A);
  if (thing_is_invalid(thing))
    return;
  if (player->field_10 > game.play_gameturn)
  {
    process_keeper_sprite((GetMouseX()+60) / pixel_size, (GetMouseY()+40)/pixel_size,
      thing->field_44, 0, thing->field_48, 64 / pixel_size);
    draw_mini_things_in_hand(GetMouseX()+60, GetMouseY());
    return;
  }
  if ((player->field_3 & 0x02) != 0)
  {
    draw_mini_things_in_hand(GetMouseX()+18, GetMouseY());
    return;
  }
  if (player->work_state != 5)
  {
    if ( (player->work_state != 1)
      || (player->field_455 != 3) && ((player->work_state != 1) || (player->field_455) || (player->field_454 != 3)) )
    {
      if ((player->instance_num != 1) && (player->instance_num != 2))
      {
        if (player->work_state == 9)
        {
          process_keeper_sprite((GetMouseX()+70) / pixel_size, (GetMouseY()+46) / pixel_size,
              thing->field_44, 0, thing->field_48, 64 / pixel_size);
        } else
        if (player->work_state == 1)
        {
          if ((player->field_455 == 2) || (player->field_454 == 2))
          {
            draw_mini_things_in_hand(GetMouseX()+18, GetMouseY());
          }
        }
        return;
      }
    }
  }
  picktng = get_first_thing_in_power_hand(player);
  if ((!thing_is_invalid(picktng)) && ((picktng->field_4F & 0x01) == 0))
  {
    switch (picktng->class_id)
    {
    case 5:
        cctrl = creature_control_get_from_thing(picktng);
        if ((cctrl->field_AD & 0x02) == 0)
        {
          x = GetMouseX() + creature_picked_up_offset[picktng->model].delta_x;
          y = GetMouseY() + creature_picked_up_offset[picktng->model].delta_y;
          if (creatures[picktng->model].field_7 )
            EngineSpriteDrawUsingAlpha = 1;
          process_keeper_sprite(x / pixel_size, y / pixel_size,
              picktng->field_44, 0, picktng->field_48, 64 / pixel_size);
          EngineSpriteDrawUsingAlpha = 0;
        } else
        {
          x = GetMouseX()+11;
          y = GetMouseY()+56;
          process_keeper_sprite(x / pixel_size, y / pixel_size,
              picktng->field_44, 0, picktng->field_48, 64 / pixel_size);
        }
        break;
    case 1:
        if (picktng->model == 10)
        {
          x = GetMouseX()+11;
          y = GetMouseY()+56;
          process_keeper_sprite(x / pixel_size, y / pixel_size,
              picktng->field_44, 0, picktng->field_48, 64 / pixel_size);
          break;
        } else
        if ((picktng->class_id == 1) && object_is_gold_pile(picktng))
          break;
    default:
        x = GetMouseX();
        y = GetMouseY();
        process_keeper_sprite(x / pixel_size, y / pixel_size,
              picktng->field_44, 0, picktng->field_48, 64 / pixel_size);
        break;
    }
  }
  if (player->field_C == 784)
  {
    x = GetMouseX()+58;
    y = GetMouseY()+6;
    process_keeper_sprite(x / pixel_size, y / pixel_size,
        thing->field_44, 0, thing->field_48, 64 / pixel_size);
    draw_mini_things_in_hand(GetMouseX()+60, GetMouseY());
  } else
  {
    x = GetMouseX()+60;
    y = GetMouseY()+40;
    process_keeper_sprite(x / pixel_size, y / pixel_size,
        thing->field_44, 0, thing->field_48, 64 / pixel_size);
    draw_mini_things_in_hand(GetMouseX()+60, GetMouseY());
  }
}

void draw_swipe(void)
{
  _DK_draw_swipe();
}

void do_map_rotate_stuff(long a1, long a2, long *a3, long *a4, long a5)
{
  _DK_do_map_rotate_stuff(a1, a2, a3, a4, a5);
}

short do_left_map_drag(long begin_x, long begin_y, long curr_x, long curr_y, long zoom)
{
  static const char *func_name="do_left_map_drag";
#if (BFDEBUG_LEVEL > 17)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  struct PlayerInfo *player;
  long x,y;
  if (!clicked_on_small_map)
  {
    grabbed_small_map = 0;
    return 0;
  }
  x = (curr_x - (MyScreenWidth >> 1)) / 2;
  y = (curr_y - (MyScreenHeight >> 1)) / 2;
  if ((abs(curr_x - old_mx) < 2) && (abs(curr_y - old_my) < 2))
    return 0;
  if (!grabbed_small_map)
  {
    grabbed_small_map = 1;
    x = 0;
    y = 0;
  }
  do_map_rotate_stuff(x, y, &curr_x, &curr_y, zoom);
  player = &(game.players[my_player_number%PLAYERS_COUNT]);
  game.field_1517FD = curr_x;
  game.field_1517FF = curr_y;
  if (subtile_has_slab(curr_x, curr_y))
  {
    set_players_packet_action(player, 26, curr_x, curr_y, 0, 0);
  }
  return 1;
}

short do_left_map_click(long begin_x, long begin_y, long curr_x, long curr_y, long zoom)
{
  static const char *func_name="do_left_map_click";
#if (BFDEBUG_LEVEL > 17)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  struct PlayerInfo *player;
  short result;
  result = 0;
  player = &(game.players[my_player_number%PLAYERS_COUNT]);
  if ((left_button_released) && (clicked_on_small_map))
  {
      if (grabbed_small_map)
      {
        game.small_map_state = 2;
        LbMouseSetPosition((begin_x+58)/pixel_size, (begin_y+58)/pixel_size);
      } else
      {
        do_map_rotate_stuff(curr_x-begin_x-58, curr_y-begin_y-58, &curr_x, &curr_y, zoom);
        game.field_1517FD = curr_x;
        game.field_1517FF = curr_y;
        if (subtile_has_slab(curr_x, curr_y))
        {
          result = 1;
          set_players_packet_action(player, 26, curr_x, curr_y, 0, 0);
        }
      }
    grabbed_small_map = 0;
    clicked_on_small_map = 0;
    left_button_released = 0;
  }
  return result;
}

void magic_use_power_chicken(unsigned char a1, struct Thing *thing, long a3, long a4, long a5)
{
  _DK_magic_use_power_chicken(a1, thing, a3, a4, a5);
}

void magic_use_power_disease(unsigned char a1, struct Thing *thing, long a3, long a4, long a5)
{
  _DK_magic_use_power_disease(a1, thing, a3, a4, a5);
}

void magic_use_power_destroy_walls(unsigned char a1, long a2, long a3, long a4)
{
  _DK_magic_use_power_destroy_walls(a1, a2, a3, a4);
}

short magic_use_power_imp(unsigned short a1, unsigned short a2, unsigned short a3)
{
  return _DK_magic_use_power_imp(a1, a2, a3);
}

long remove_workshop_object_from_player(long a1, long a2)
{
  return _DK_remove_workshop_object_from_player(a1, a2);
}

void magic_use_power_heal(unsigned char a1, struct Thing *thing, long a3, long a4, long a5)
{
  _DK_magic_use_power_heal(a1, thing, a3, a4, a5);
}

void magic_use_power_conceal(unsigned char a1, struct Thing *thing, long a3, long a4, long a5)
{
  _DK_magic_use_power_conceal(a1, thing, a3, a4, a5);
}

void magic_use_power_armour(unsigned char a1, struct Thing *thing, long a3, long a4, long a5)
{
  _DK_magic_use_power_armour(a1, thing, a3, a4, a5);
}

void magic_use_power_speed(unsigned char a1, struct Thing *thing, long a3, long a4, long a5)
{
  _DK_magic_use_power_speed(a1, thing, a3, a4, a5);
}

void magic_use_power_lightning(unsigned char a1, long a2, long a3, long a4)
{
  _DK_magic_use_power_lightning(a1, a2, a3, a4);
}

unsigned char tag_cursor_blocks_place_trap(unsigned char a1, long a2, long a3)
{
  return _DK_tag_cursor_blocks_place_trap(a1, a2, a3);
}

long magic_use_power_sight(unsigned char a1, long a2, long a3, long a4)
{
  return _DK_magic_use_power_sight(a1, a2, a3, a4);
}

void magic_use_power_cave_in(unsigned char a1, long a2, long a3, long a4)
{
  _DK_magic_use_power_cave_in(a1, a2, a3, a4);
}

long magic_use_power_call_to_arms(unsigned char a1, long a2, long a3, long a4, long a5)
{
  return _DK_magic_use_power_call_to_arms(a1, a2, a3, a4, a5);
}

void stop_creatures_around_hand(char a1, unsigned short a2, unsigned short a3)
{
  _DK_stop_creatures_around_hand(a1, a2, a3);
}

struct Thing *get_queryable_object_near(unsigned short a1, unsigned short a2, long a3)
{
  return _DK_get_queryable_object_near(a1, a2, a3);
}

unsigned char initialise_thing_state(struct Thing *thing, long a2)
{
  return _DK_initialise_thing_state(thing, a2);
}

short setup_person_move_to_position(struct Thing *thing, long pos_x, long pos_y, unsigned char a4)
{
  return _DK_setup_person_move_to_position(thing, pos_x, pos_y, a4);
}

void tag_cursor_blocks_dig(unsigned char a1, long a2, long a3, long a4)
{
  _DK_tag_cursor_blocks_dig(a1, a2, a3, a4);
}

void tag_cursor_blocks_thing_in_hand(unsigned char a1, long a2, long a3, int a4, long a5)
{
  _DK_tag_cursor_blocks_thing_in_hand(a1, a2, a3, a4, a5);
}

void create_power_hand(unsigned char a1)
{
  _DK_create_power_hand(a1);
}

unsigned long can_drop_thing_here(long x, long y, long a3, unsigned long a4)
{
  return _DK_can_drop_thing_here(x, y, a3, a4);
}

/*
 * Returns if a given player (owner) can dig the specified subtile.
 */
short can_dig_here(long stl_x, long stl_y, long plyr_idx)
{
  struct SlabMap *slb;
  long i;
  slb = get_slabmap_block(map_to_slab[stl_x],map_to_slab[stl_y]);
  if (slabmap_block_invalid(slb))
    return false;
  if (!subtile_revealed(stl_x, stl_y, plyr_idx))
    return true;
  if ((slb->slab >= 42) && (slb->slab <= 47))
  {
      if (slabmap_owner(slb) == plyr_idx)
        return false;
  }
  i = slab_attrs[slb->slab%SLAB_TYPES_COUNT].field_6;
  if ((i & 0x29) != 0)
    return true;
  return false;
}

long thing_in_wall_at(struct Thing *thing, struct Coord3d *pos)
{
  return _DK_thing_in_wall_at(thing, pos);
}

short can_place_thing_here(struct Thing *thing, long x, long y, long dngn_idx)
{
  struct Coord3d pos;
  if (!can_drop_thing_here(x, y, dngn_idx, thing->model == 23))
    return false;
  pos.x.val = (x << 8) + 128;
  pos.y.val = (y << 8) + 128;
  pos.z.val = get_thing_height_at(thing, &pos);
  return !thing_in_wall_at(thing, &pos);
}

short do_right_map_click(long start_x, long start_y, long curr_mx, long curr_my, long zoom)
{
  static const char *func_name="do_right_map_click";
  long x,y;
#if (BFDEBUG_LEVEL > 17)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  struct PlayerInfo *player;
  struct Dungeon *dungeon;
  struct Thing *thing;
  int i;
  do_map_rotate_stuff(curr_mx-start_x-58, curr_my-start_y-58, &x, &y, zoom);
  game.field_1517FD = x;
  game.field_1517FF = y;
  player = &(game.players[my_player_number%PLAYERS_COUNT]);
  dungeon = &(game.dungeon[player->field_2B%DUNGEONS_COUNT]);
  thing = get_first_thing_in_power_hand(player);
  if (!thing_is_invalid(thing))
  {
    if (can_place_thing_here(thing, x, y, player->field_2B))
      game.small_map_state = 1;
  }
  if (right_button_clicked)
    right_button_clicked = 0;
  if (right_button_released)
  {
    right_button_released = 0;
    if (subtile_has_slab(x, y))
    {
      set_players_packet_action(player, 91, x, y, 0, 0);
      return 1;
    }
  }
  return 0;
}

short mouse_is_over_small_map(long x, long y)
{
  long cmx,cmy;
  long px,py;
  cmx = GetMouseX();
  cmy = GetMouseY();
  px = (cmx-(x+SMALL_MAP_RADIUS));
  py = (cmy-(y+SMALL_MAP_RADIUS));
  return (LbSqrL(px*px + py*py) < SMALL_MAP_RADIUS);
}

void draw_whole_status_panel(void)
{
  struct Dungeon *dungeon;
  struct PlayerInfo *player;
  long i;
  player = &(game.players[my_player_number%PLAYERS_COUNT]);
  dungeon = &(game.dungeon[my_player_number%DUNGEONS_COUNT]);
  lbDisplay.DrawColour = colours[15][15][15];
  lbDisplay.DrawFlags = 0;
  DrawBigSprite(0, 0, &status_panel, gui_panel_sprites);
  draw_gold_total(player->field_2B, 60, 134, dungeon->field_AF9);
  if (pixel_size < 3)
      i = (player->minimap_zoom) / (3-pixel_size);
  else
      i = player->minimap_zoom;
  pannel_map_draw(player->mouse_x, player->mouse_y, i);
  draw_overlay_things(i);
}

void redraw_creature_view(void)
{
  static const char *func_name="redraw_creature_view";
#if (BFDEBUG_LEVEL > 6)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  struct TbGraphicsWindow ewnd;
  struct PlayerInfo *player;
  struct Thing *thing;
  long i;
  //_DK_redraw_creature_view(); return;
  player = &(game.players[my_player_number%PLAYERS_COUNT]);
  if (player->field_45F != 2)
    player->field_45F = 2;
  update_explored_flags_for_power_sight(player);
  thing = thing_get(player->field_2F);
  if (!thing_is_invalid(thing))
    draw_creature_view(thing);
  if (smooth_on)
  {
    store_engine_window(&ewnd,pixel_size);
    smooth_screen_area(lbDisplay.WScreen, ewnd.x, ewnd.y,
        ewnd.width, ewnd.height, lbDisplay.GraphicsScreenWidth);
  }
  remove_explored_flags_for_power_sight(player);
  draw_swipe();
  if ((game.numfield_C & 0x20) != 0)
    draw_whole_status_panel();
  draw_gui();
  if ((game.numfield_C & 0x20) != 0)
    draw_overlay_compass(player->mouse_x, player->mouse_y);
  message_draw();
  gui_draw_all_boxes();
  draw_tooltip();
}

void smooth_screen_area(unsigned char *scrbuf, long x, long y, long w, long h, long scanln)
{
  static const char *func_name="smooth_screen_area";
#if (BFDEBUG_LEVEL > 7)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  long i,k;
  unsigned char *buf;
  unsigned char *lnbuf;
  unsigned int ghpos;
  lnbuf = scrbuf + scanln*y + x;
  for (i = h-y-1; i>0; i--)
  {
    buf = lnbuf;
    for (k = w-x-1; k>0; k--)
    {
        ghpos = (buf[0] << 8) + buf[1];
        ghpos = (buf[scanln] << 8) + ghost[ghpos];
        buf[0] = ghpos;
        buf++;
    }
    lnbuf += scanln;
  }
}

void set_player_cameras_position(struct PlayerInfo *player, long pos_x, long pos_y)
{
  player->cameras[2].mappos.x.val = pos_x;
  player->cameras[3].mappos.x.val = pos_x;
  player->cameras[0].mappos.x.val = pos_x;
  player->cameras[2].mappos.y.val = pos_y;
  player->cameras[3].mappos.y.val = pos_y;
  player->cameras[0].mappos.y.val = pos_y;
}

void make_camera_deviations(struct PlayerInfo *player,struct Dungeon *dungeon)
{
  static const char *func_name="make_camera_deviations";
  long x,y;
  x = player->acamera->mappos.x.val;
  y = player->acamera->mappos.y.val;
  if (dungeon->field_EA0)
  {
    x += seed_check_random(80, &game.rand_14BB4E, func_name, 8653) - 40;
    y += seed_check_random(80, &game.rand_14BB4E, func_name, 8654) - 40;
  }
  if (dungeon->field_EA4)
  {
    x += ( (dungeon->field_EA4 * LbSinL(player->acamera->orient_a) >> 8) >> 8);
    y += (-(dungeon->field_EA4 * LbCosL(player->acamera->orient_a) >> 8) >> 8);
  }
  if ((dungeon->field_EA0) || (dungeon->field_EA4))
  {
    // bounding position
    if (x < 0)
    {
      x = 0;
    } else
    if (x > 65535)
    {
      x = 65535;
    }
    if (y < 0)
    {
      y = 0;
    } else
    if (y > 65535)
    {
      y = 65535;
    }
    // setting deviated position
    player->acamera->mappos.x.val = x;
    player->acamera->mappos.y.val = y;
  }
}

void redraw_isometric_view(void)
{
  static const char *func_name="redraw_isometric_view";
  struct PlayerInfo *player;
  struct Dungeon *dungeon;
  struct TbGraphicsWindow ewnd;
  struct Coord3d pos;
  int i;
#if (BFDEBUG_LEVEL > 6)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  //_DK_redraw_isometric_view(); return;

  player = &(game.players[my_player_number%PLAYERS_COUNT]);
  memcpy(&pos,&player->acamera->mappos,sizeof(struct Coord3d));
  if (player->field_45F != 1)
    player->field_45F = 1;
  dungeon = &(game.dungeon[my_player_number%DUNGEONS_COUNT]);
  // Camera position modifications
  make_camera_deviations(player,dungeon);
  update_explored_flags_for_power_sight(player);
  if (game.flags_font & 0x08)
  {
    store_engine_window(&ewnd,1);
    setup_engine_window(ewnd.x, ewnd.y, ewnd.width >> 1, ewnd.height >> 1);
  }
  engine(&player->cameras[0]);
  if (game.flags_font & 0x08)
  {
    load_engine_window(&ewnd);
  }
  if (smooth_on)
  {
    store_engine_window(&ewnd,pixel_size);
    smooth_screen_area(lbDisplay.WScreen, ewnd.x, ewnd.y,
        ewnd.width, ewnd.height, lbDisplay.GraphicsScreenWidth);
  }
  remove_explored_flags_for_power_sight(player);
  if ((game.numfield_C & 0x20) != 0)
    draw_whole_status_panel();
  draw_gui();
  if ((game.numfield_C & 0x20) != 0)
    draw_overlay_compass(player->mouse_x, player->mouse_y);
  message_draw();
  gui_draw_all_boxes();
  draw_power_hand();
  draw_tooltip();
  memcpy(&player->acamera->mappos,&pos,sizeof(struct Coord3d));
#if (BFDEBUG_LEVEL > 8)
    LbSyncLog("%s: Finished\n",func_name);
#endif
}

void redraw_frontview(void)
{
  static const char *func_name="redraw_frontview";
#if (BFDEBUG_LEVEL > 6)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  _DK_redraw_frontview();
  gui_draw_all_boxes();
}

void draw_texture(long a1, long a2, long a3, long a4, long a5, long a6, long a7)
{
  _DK_draw_texture(a1, a2, a3, a4, a5, a6, a7);
}

void draw_status_sprites(long a1, long a2, struct Thing *thing, long a4)
{
  _DK_draw_status_sprites(a1, a2, thing, a4);
}

long element_top_face_texture(struct Map *map)
{
  return _DK_element_top_face_texture(map);
}

long thing_is_spellbook(struct Thing *thing)
{
  return _DK_thing_is_spellbook(thing);
}

struct Thing *get_spellbook_at_position(long x, long y)
{
  return _DK_get_spellbook_at_position(x, y);
}

struct Thing *get_special_at_position(long x, long y)
{
  return _DK_get_special_at_position(x, y);
}

long object_is_gold(struct Thing *thing)
{
  return _DK_object_is_gold(thing);
}

void draw_zoom_box_things_on_mapblk(struct Map *mapblk,unsigned short subtile_size,int scr_x,int scr_y)
{
  static const char *func_name="draw_zoom_box_things_on_mapblk";
  struct PlayerInfo *player;
  struct Thing *thing;
  int spos_x,spos_y;
  TbPixel color;
  long spridx;
  unsigned long k;
  long i;
  player = &(game.players[my_player_number%PLAYERS_COUNT]);
  k = 0;
  i = ((mapblk->data & 0x3FF800u) >> 11);
  while (i > 0)
  {
    thing = thing_get(i);
    if (thing_is_invalid(thing))
      break;
    i = thing->field_2;
    if (((thing->field_0 & 0x10) == 0) && ((thing->field_1 & 0x02) == 0))
    {
      spos_x = ((subtile_size * thing->mappos.x.stl.pos) >> 8);
      spos_y = ((subtile_size * thing->mappos.y.stl.pos) >> 8);
      switch (thing->class_id)
      {
      case TCls_Creature:
        spridx = creature_graphics[thing->model][20];
        if ((game.play_gameturn & 0x04) != 0)
        {
          color = get_player_path_colour(thing->owner);
          draw_gui_panel_sprite_occentered(scr_x+spos_x, scr_y+spos_y, spridx, color);
        } else
        {
          draw_gui_panel_sprite_centered(scr_x+spos_x, scr_y+spos_y, spridx);
        }
        draw_status_sprites((spos_x+scr_x)/pixel_size - 10, (spos_y+scr_y-20)/pixel_size, thing, 4096);
        break;
      case TCls_Trap:
        if ((!thing->byte_17.h) && (player->field_2B != thing->owner))
          break;
        spridx = trap_data[thing->model].field_A;
        draw_gui_panel_sprite_centered(scr_x+spos_x, scr_y+spos_y, spridx);
        break;
      case TCls_Object:
        if (thing->model == 5)
        {
          spridx = 512;
          draw_gui_panel_sprite_centered(scr_x+spos_x, scr_y+spos_y, spridx);
        } else
        if (object_is_gold(thing))
        {
          spridx = 511;
          draw_gui_panel_sprite_centered(scr_x+spos_x, scr_y+spos_y, spridx);
        } else
        if ( thing_is_special(thing) )
        {
          spridx = 164;
          draw_gui_panel_sprite_centered(scr_x+spos_x, scr_y+spos_y, spridx);
        } else
        if ( thing_is_spellbook(thing) )
        {
          spridx = spell_data[object_to_magic[thing->model]].field_B;
          draw_gui_panel_sprite_centered(scr_x+spos_x, scr_y+spos_y, spridx);
        }
        break;
      default:
        break;
      }
    }
    k++;
    if (k > THINGS_COUNT)
    {
      error(func_name,4579,"Infinite loop detected when sweeping things list");
      break;
    }
  }
}

/*
 * Draws a box near mouse with more detailed top view of map.
 * Requires screen to be locked before.
 */
void draw_zoom_box(void)
{
  //_DK_draw_zoom_box(); return;
  struct PlayerInfo *player;
  struct Map *mapblk;
  const int subtile_size = 8;
  long map_tiles_x,map_tiles_y;
  long scrtop_x,scrtop_y;
  int map_dx,map_dy;
  int scr_x,scr_y;
  int map_x,map_y;
  int i,k;

  map_tiles_x = 13;
  map_tiles_y = 13;

  lbDisplay.DrawFlags = 0;
  scrtop_x = GetMouseX() + 24;
  scrtop_y = GetMouseY() + 24;
  map_x = (3*GetMouseX()-450) / 4 - 6;
  map_y = (3*GetMouseY()-168) / 4 - 6;

  // Draw only on map area
  if ((map_x < -map_tiles_x+4) || (map_x >= map_subtiles_x+1-map_tiles_x+6)
   || (map_y < -map_tiles_y+4) || (map_y >= map_subtiles_x+1-map_tiles_y+6))
    return;

  scrtop_x += 4;
  scrtop_y -= 4;
  setup_vecs(lbDisplay.WScreen, 0, lbDisplay.GraphicsScreenWidth, MyScreenWidth/pixel_size, MyScreenHeight/pixel_size);
  if (scrtop_y > MyScreenHeight-map_tiles_y*subtile_size)
    scrtop_y = MyScreenHeight-map_tiles_y*subtile_size;
  if (scrtop_y < 0)
      scrtop_y = 0;
  player = &(game.players[my_player_number%PLAYERS_COUNT]);

  scr_y = scrtop_y;
  for (map_dy=0; map_dy < map_tiles_y; map_dy++)
  {
    scr_x = scrtop_x;
    for (map_dx=0; map_dx < map_tiles_x; map_dx++)
    {
      mapblk = get_map_block(map_x+map_dx,map_y+map_dy);
      if (map_block_revealed(mapblk, player->field_2B))
      {
        k = element_top_face_texture(mapblk);
        draw_texture(scr_x, scr_y, subtile_size, subtile_size, k, 0, -1);
      } else
      {
        LbDrawBox(scr_x/pixel_size, scr_y/pixel_size, 8/pixel_size, 8/pixel_size, 1);
      }
      scr_x += subtile_size;
    }
    scr_y += subtile_size;
  }
  lbDisplay.DrawFlags |= 0x0010;
  LbDrawBox(scrtop_x/pixel_size, scrtop_y/pixel_size,
      (map_tiles_x*subtile_size)/pixel_size, (map_tiles_y*subtile_size)/pixel_size, 0);
  set_flag_word(&lbDisplay.DrawFlags,0x0010,false);
  LbScreenSetGraphicsWindow( (scrtop_x+2)/pixel_size, (scrtop_y+2)/pixel_size,
      (map_tiles_y*subtile_size-4)/pixel_size, (map_tiles_y*subtile_size-4)/pixel_size);
  scr_y = 0;
  for (map_dy=0; map_dy < map_tiles_y; map_dy++)
  {
    scr_x = 0;
    for (map_dx=0; map_dx < map_tiles_x; map_dx++)
    {
      mapblk = get_map_block(map_x+map_dx,map_y+map_dy);
      if (map_block_revealed(mapblk, player->field_2B))
      {
        draw_zoom_box_things_on_mapblk(mapblk,subtile_size,scr_x,scr_y);
      }
      scr_x += subtile_size;
    }
    scr_y += subtile_size;
  }
  LbScreenSetGraphicsWindow(0/pixel_size, 0/pixel_size, MyScreenWidth/pixel_size, MyScreenHeight/pixel_size);
  LbSpriteDraw((scrtop_x-24)/pixel_size, (scrtop_y-20)/pixel_size, &button_sprite[194]);
  LbSpriteDraw((scrtop_x+54)/pixel_size, (scrtop_y-20)/pixel_size, &button_sprite[195]);
  LbSpriteDraw((scrtop_x-24)/pixel_size, (scrtop_y+50)/pixel_size, &button_sprite[196]);
  LbSpriteDraw((scrtop_x+54)/pixel_size, (scrtop_y+50)/pixel_size, &button_sprite[197]);
  LbScreenSetGraphicsWindow(0/pixel_size, 0/pixel_size, MyScreenWidth/pixel_size, MyScreenHeight/pixel_size);
}

void update_explored_flags_for_power_sight(struct PlayerInfo *player)
{
  _DK_update_explored_flags_for_power_sight(player);
}

void update_block_pointed(int i,long x, long x_frac, long y, long y_frac)
{
  struct Map *map;
  short visible;
  unsigned int mask;
  long k;

  if (i > 0)
  {
    map = get_map_block(x,y);
    visible = (player_bit & (map->data >> 28) != 0);
    if ((!visible) || ((map->data & 0x7FF) > 0))
    {
      if (visible)
        k = map->data & 0x7FF;
      else
        k = game.field_149E77;
      mask = game.columns[k].solidmask;
      if ((temp_cluedo_mode) && (mask != 0))
      {
        if (visible)
          k = map->data & 0x7FF;
        else
          k = game.field_149E77;
        if (game.columns[k].solidmask >= 8)
        {
          if ((!visible) || ((get_map_flags(x,y) & 0x80) == 0) && ((map->flags & 0x02) == 0))
            mask &= 3;
        }
      }
      if (mask & (1 << (i-1)))
      {
        pointed_at_frac_x = x_frac;
        pointed_at_frac_y = y_frac;
        block_pointed_at_x = x;
        block_pointed_at_y = y;
        me_pointed_at = map;
      }
      if (((!temp_cluedo_mode) && (i == 5)) || ((temp_cluedo_mode) && (i == 2)))
      {
        top_pointed_at_frac_x = x_frac;
        top_pointed_at_frac_y = y_frac;
        top_pointed_at_x = x;
        top_pointed_at_y = y;
      }
    }
  } else
  {
      map = get_map_block(x,y);
      floor_pointed_at_x = x;
      floor_pointed_at_y = y;
      block_pointed_at_x = x;
      block_pointed_at_y = y;
      pointed_at_frac_x = x_frac;
      pointed_at_frac_y = y_frac;
      me_pointed_at = map;
  }
}

void update_blocks_pointed(void)
{
  long x,y;
  long x_frac,y_frac;
  long hori_ptr_y,vert_ptr_y;
  long hori_hdelta_y,vert_hdelta_y;
  long hori_ptr_x,vert_ptr_x;
  long hvdiv_x,hvdiv_y;
  long k;
  int i;
    if ((!vert_offset[1]) && (!hori_offset[1]))
    {
      block_pointed_at_x = 0;
      block_pointed_at_y = 0;
      me_pointed_at = get_map_block(0,0);
    } else
    {
      hori_ptr_y = hori_offset[0] * (pointer_y - y_init_off);
      vert_ptr_y = vert_offset[0] * (pointer_y - y_init_off);
      hori_hdelta_y = hori_offset[0] * (high_offset[1] >> 8);
      vert_hdelta_y = vert_offset[0] * (high_offset[1] >> 8);
      vert_ptr_x = (signed long)(vert_offset[1] * (pointer_x - x_init_off)) >> 1;
      hori_ptr_x = (signed long)(hori_offset[1] * (pointer_x - x_init_off)) >> 1;
      hvdiv_x = (signed long)(hori_offset[0] * vert_offset[1] - vert_offset[0] * hori_offset[1]) >> 11;
      hvdiv_y = (signed long)(vert_offset[0] * hori_offset[1] - hori_offset[0] * vert_offset[1]) >> 11;
      for (i=0; i < 8; i++)
      {
        k = (vert_ptr_x - (vert_ptr_y >> 1)) / hvdiv_x;
        x_frac = (k & 3) << 6;
        x = k >> 2;
        k = (hori_ptr_x - (hori_ptr_y >> 1)) / hvdiv_y;
        y_frac = (k & 3) << 6;
        y = k >> 2;
        if ((x >= 0) && (x < map_subtiles_x) && (y >= 0) && (y < map_subtiles_y))
        {
          update_block_pointed(i,x,x_frac,y,y_frac);
        }
        hori_ptr_y -= hori_hdelta_y;
        vert_ptr_y -= vert_hdelta_y;
      }
    }
}

void engine(struct Camera *cam)
{
  static const char *func_name="engine";
  struct TbGraphicsWindow grwnd;
  struct TbGraphicsWindow ewnd;
  unsigned short flg_mem;
  struct PlayerInfo *player;

#if (BFDEBUG_LEVEL > 9)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  //_DK_engine(cam); return;

  player = &(game.players[my_player_number%PLAYERS_COUNT]);
  flg_mem = lbDisplay.DrawFlags;
  update_engine_settings(player);
  mx = cam->mappos.x.val;
  my = cam->mappos.y.val;
  mz = cam->mappos.z.val;
  pointer_x = (GetMouseX() - player->engine_window_x) / pixel_size;
  pointer_y = (GetMouseY() - player->engine_window_y) / pixel_size;
  lens = cam->field_13 * MyScreenWidth/pixel_size / 320;
  if (lens_mode == 0)
    update_blocks_pointed();
  LbScreenStoreGraphicsWindow(&grwnd);
  store_engine_window(&ewnd,pixel_size);
  view_height_over_2 = ewnd.height/2;
  view_width_over_2 = ewnd.width/2;
  LbScreenSetGraphicsWindow(ewnd.x, ewnd.y, ewnd.width, ewnd.height);
  setup_vecs(lbDisplay.GraphicsWindowPtr, 0, lbDisplay.GraphicsScreenWidth,
      ewnd.width, ewnd.height);
  draw_view(cam, 0);
  lbDisplay.DrawFlags = flg_mem;
  thing_being_displayed = 0;
  LbScreenLoadGraphicsWindow(&grwnd);
}

void remove_explored_flags_for_power_sight(struct PlayerInfo *player)
{
  _DK_remove_explored_flags_for_power_sight(player);
}

void DrawBigSprite(long x, long y, struct BigSprite *bigspr, struct TbSprite *sprite)
{
  _DK_DrawBigSprite(x, y, bigspr, sprite);
}

void draw_gold_total(unsigned char a1, long a2, long a3, long a4)
{
  _DK_draw_gold_total(a1, a2, a3, a4);
}

void pannel_map_draw(long x, long y, long zoom)
{
  _DK_pannel_map_draw(x, y, zoom);
}

void draw_overlay_things(long zoom)
{
  static const char *func_name="draw_overlay_things";
#if (BFDEBUG_LEVEL > 7)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  _DK_draw_overlay_things(zoom);
}

void draw_overlay_compass(long a1, long a2)
{
  _DK_draw_overlay_compass(a1, a2);
}

void draw_map_level_name(void)
{
  struct LevelInformation *lvinfo;
  LevelNumber lvnum;
  char *lv_name;
  int x,y,w,h;
  // Retrieving name
  lv_name = NULL;
  lvnum = get_loaded_level_number();
  lvinfo = get_level_info(lvnum);
  if (lvinfo != NULL)
  {
    if ((lvinfo->name_id > 0) && (lvinfo->name_id < STRINGS_MAX))
      lv_name = campaign.strings[lvinfo->name_id];
    else
      lv_name = lvinfo->name;
  } else
  if (is_multiplayer_level(lvnum))
  {
    lv_name = level_name;
  }
  // Retrieving position
  x = 0;
  y = 0;
  w = 640;//MyScreenWidth;
  h = MyScreenHeight;
  // Drawing
  if (lv_name != NULL)
  {
    lbFontPtr = winfont;
    lbDisplay.DrawFlags = 0;
    LbTextSetWindow(x/pixel_size, y/pixel_size, w/pixel_size, h/pixel_size);
    LbTextDraw((w-pixel_size*LbTextStringWidth(lv_name))/2 / pixel_size, 32 / pixel_size, lv_name);
  }
}

void load_parchment_file(void)
{
  if ( !parchment_loaded )
  {
    reload_parchment_file(lbDisplay.PhysicalScreenWidth >= 640);
  }
}

void reload_parchment_file(short hires)
{
  char *fname;
  if (hires)
  {
    fname=prepare_file_path(FGrp_StdData,"gmaphi.raw");
    LbFileLoadAt(fname, hires_parchment);
  } else
  {
    fname=prepare_file_path(FGrp_StdData,"gmap.raw");
    LbFileLoadAt(fname, poly_pool);
  }
  parchment_loaded = 1;
}

void redraw_parchment_view(void)
{
  static const char *func_name="redraw_parchment_view";
#if (BFDEBUG_LEVEL > 5)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  load_parchment_file();
  draw_map_parchment();
  draw_2d_map();
  draw_gui();
  gui_draw_all_boxes();
  draw_zoom_box();
  draw_map_level_name();
  draw_tooltip();
}

void redraw_display(void)
{
  static const char *func_name="redraw_display";
  //_DK_redraw_display();return;
  char *text;
  struct PlayerInfo *player;
  int i;
#if (BFDEBUG_LEVEL > 5)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  player = &(game.players[my_player_number%PLAYERS_COUNT]);
  set_flag_byte(&player->field_6,0x01,false);
  if (game.flagfield_14EA4A == 1)
    return;
  if (game.small_map_state == 2)
    set_pointer_graphic_none();
  else
    process_pointer_graphic();
#if (BFDEBUG_LEVEL > 8)
//!!!!!!!remove when possession bug solved
    LbSyncLog("%s: Redrawing view %d\n",func_name,(int)player->field_37);
#endif
  switch (player->field_37)
  {
  case 0:
      break;
  case 1:
      redraw_creature_view();
      parchment_loaded = 0;
      break;
  case 2:
      redraw_isometric_view();
      parchment_loaded = 0;
      break;
  case 3:
      redraw_parchment_view();
      break;
  case 5:
      redraw_frontview();
      parchment_loaded = 0;
      break;
  case 6:
      parchment_loaded = 0;
      player->field_4BD = map_fade_in(player->field_4BD);
      break;
  case 7:
      parchment_loaded = 0;
      player->field_4BD = map_fade_out(player->field_4BD);
      break;
  default:
      LbErrorLog("%s: Unsupported drawing state, %d\n",func_name,(int)player->field_37);
      break;
  }
  //LbTextSetWindow(0, 0, MyScreenWidth, MyScreenHeight);
  lbFontPtr = winfont;
  lbDisplay.DrawFlags &= 0xFFBFu;
  LbTextSetWindow(0, 0, MyScreenWidth, MyScreenHeight);
  if (player->field_0 & 0x04)
  {
      text = buf_sprintf( ">%s_", player->strfield_463);
      LbTextDraw(148/pixel_size, 8/pixel_size, text);
  }
  if ( draw_spell_cost )
  {
      long pos_x,pos_y;
      unsigned short drwflags_mem;
      drwflags_mem = lbDisplay.DrawFlags;
      LbTextSetWindow(0, 0, MyScreenWidth, MyScreenHeight);
      lbDisplay.DrawFlags = 0;
      lbFontPtr = winfont;
      text = buf_sprintf("%d", draw_spell_cost);
      pos_y = GetMouseY() - pixel_size*LbTextStringHeight(text)/2 - 2;
      pos_x = GetMouseX() - pixel_size*LbTextStringWidth(text)/2;
      LbTextDraw(pos_x/pixel_size, pos_y/pixel_size, text);
      lbDisplay.DrawFlags = drwflags_mem;
      draw_spell_cost = 0;
  }
  if (bonus_timer_enabled())
    draw_bonus_timer();
  if (((game.numfield_C & 0x01) != 0) && ((game.numfield_C & 0x80) == 0))
  {
        lbFontPtr = winfont;
        text = gui_strings[320]; // "Paused"
        long pos_x,pos_y;
        long w,h;
        int i;
        i = 0;
        if (lbFontPtr != NULL)
          i = lbFontPtr[1].SWidth;
        w = pixel_size * (LbTextStringWidth(text) + 2*i);
        i = player->field_37;
        if ((i == 2) || (i == 5) || (i == 1))
          pos_x = player->engine_window_x + (MyScreenWidth-w-player->engine_window_x)/2;
        else
          pos_x = (MyScreenWidth-w)/2;
        pos_y=16;
        i = 0;
        if (lbFontPtr != NULL)
          i = lbFontPtr[1].SHeight;
        lbDisplay.DrawFlags = 0x0100;
        h = pixel_size*i + pixel_size*i/2;
        LbTextSetWindow(pos_x/pixel_size, pos_y/pixel_size, w/pixel_size, h/pixel_size);
        draw_slab64k(pos_x, pos_y, w, h);
        LbTextDraw(0/pixel_size, 0/pixel_size, text);
        LbTextSetWindow(0/pixel_size, 0/pixel_size, MyScreenWidth/pixel_size, MyScreenHeight/pixel_size);
  }
  if (game.field_150356 != 0)
  {
    long pos_x,pos_y;
    long w,h;
    int i;
    if (game.armageddon.count_down+game.field_150356 <= game.play_gameturn)
    {
      i = 0;
      if ( game.field_15035A - game.armageddon.duration <= game.play_gameturn )
        i = game.field_15035A - game.play_gameturn;
    } else
    {
      i = game.play_gameturn - game.field_150356 - game.armageddon.count_down;
    }
    lbFontPtr = winfont;
    text = buf_sprintf(" %s %03d", gui_strings[646], i/2); // Armageddon message
    i = 0;
    if (lbFontPtr != NULL)
      i = lbFontPtr[1].SWidth;
    w = pixel_size*LbTextStringWidth(text) + 6*i;
    pos_x = MyScreenWidth - w - 16;
    pos_y = 16;
    i = 0;
    if (lbFontPtr != NULL)
      i = lbFontPtr[1].SHeight;
    lbDisplay.DrawFlags = 0x0100;
    h = pixel_size*i + pixel_size*i/2;
    LbTextSetWindow(pos_x/pixel_size, pos_y/pixel_size, w/pixel_size, h/pixel_size);
    draw_slab64k(pos_x, pos_y, w, h);
    LbTextDraw(0/pixel_size, 0/pixel_size, text);
    LbTextSetWindow(0/pixel_size, 0/pixel_size, MyScreenWidth/pixel_size, MyScreenHeight/pixel_size);
  }
  draw_sound_stuff();
//show_onscreen_msg(8, "Physical(%d,%d) Graphics(%d,%d) Lens(%d,%d)", (int)lbDisplay.PhysicalScreenWidth, (int)lbDisplay.PhysicalScreenHeight, (int)lbDisplay.GraphicsScreenWidth, (int)lbDisplay.GraphicsScreenHeight, (int)eye_lens_width, (int)eye_lens_height);
#if (BFDEBUG_LEVEL > 7)
    LbSyncLog("%s: Finished\n",func_name);
#endif
}

void find_frame_rate(void)
{
  static TbClockMSec prev_time2=0;
  static TbClockMSec cntr_time2=0;
  unsigned long curr_time;
  curr_time = LbTimerClock();
  cntr_time2++;
  if (curr_time-prev_time2 >= 1000)
  {
    double time_fdelta = 1000.0*((double)(cntr_time2))/(curr_time-prev_time2);
    prev_time2 = curr_time;
    game.time_delta = (unsigned long)(time_fdelta*256.0);
    cntr_time2 = 0;
  }
}

void packet_load_find_frame_rate(unsigned long incr)
{
  static TbClockMSec start_time=0;
  static TbClockMSec extra_frames=0;
  TbClockMSec curr_time;
  curr_time = LbTimerClock();
  if ((curr_time-start_time) < 5000)
  {
    extra_frames += incr;
  } else
  {
    double time_fdelta = 1000.0*((double)(extra_frames+incr))/(curr_time-start_time);
    start_time = curr_time;
    game.time_delta = (unsigned long)(time_fdelta*256.0);
    extra_frames = 0;
  }
}

/*
 * Checks if the game screen needs redrawing.
 */
short display_should_be_updated_this_turn(void)
{
  if ((game.numfield_C & 0x01) != 0)
    return true;
  if ( (game.turns_fastforward==0) && (!game.numfield_149F38) )
  {
    find_frame_rate();
    if ( (game.frame_skip == 0) || ((game.play_gameturn % game.frame_skip) == 0))
      return true;
  } else
  if ( ((game.play_gameturn & 0x3F)==0) ||
       ((game.numfield_149F38) && ((game.play_gameturn & 7)==0)) )
  {
    packet_load_find_frame_rate(64);
    return true;
  }
  return false;
}

/*
 * Makes last updates to the video buffer, and swaps buffers to show
 * the new image.
 */
short keeper_screen_swap(void)
{
/*  // For resolution 640x480, move the graphics data 40 lines lower
  if ( lbDisplay.ScreenMode == Lb_SCREEN_MODE_640_480_8 )
    if ( LbScreenLock() == 1 )
    {
      int i;
      int scrmove_x=0;
      int scrmove_y=40;
      int scanline_len=640;
      for (i=400;i>=0;i--)
        memcpy(lbDisplay.WScreen+scanline_len*(i+scrmove_y)+scrmove_x, lbDisplay.WScreen+scanline_len*i, scanline_len-scrmove_x);
      memset(lbDisplay.WScreen, 0, scanline_len*scrmove_y);
      LbScreenUnlock();
    }*/
  LbScreenSwap();
  return 1;
}

/*
 * Waits until the next game turn. Delay is usually controlled by
 * num_fps variable.
 */
short keeper_wait_for_next_turn(void)
{
  if (game.numfield_D & 0x10)
  {
      // No idea when such situation occurs
      TbClockMSec sleep_end = last_loop_time + 1000;
      LbSleepUntil(sleep_end);
      last_loop_time = LbTimerClock();
      return 1;
  }
  if (game.frame_skip == 0)
  {
      // Standard delaying system
      TbClockMSec sleep_end = last_loop_time + 1000/game.num_fps;
      LbSleepUntil(sleep_end);
      last_loop_time = LbTimerClock();
      return 1;
  }
  return 0;
}

/*
 * Redraws the game display buffer.
 */
short keeper_screen_redraw(void)
{
  static const char *func_name="keeper_screen_redraw";
  struct PlayerInfo *player;
#if (BFDEBUG_LEVEL > 5)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  player = &(game.players[my_player_number%PLAYERS_COUNT]);
  LbScreenClear(0);
  if (LbScreenLock() == 1)
  {
    setup_engine_window(player->engine_window_x, player->engine_window_y,
        player->engine_window_width, player->engine_window_height);
    redraw_display();
    LbScreenUnlock();
    return 1;
  }
  return 0;
}

/*
 * Draws the crucial warning messages on screen.
 * Requires the screen to be locked before.
 */
short draw_onscreen_direct_messages(void)
{
  static const char *func_name="draw_onscreen_direct_messages";
  char *text;
  unsigned int msg_pos;
#if (BFDEBUG_LEVEL > 5)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  // Display in-game message for debug purposes
  if (onscreen_msg_turns > 0)
  {
      if ( LbScreenIsLocked() )
        LbTextDraw(260/pixel_size, 0/pixel_size, onscreen_msg_text);
      onscreen_msg_turns--;
  }
  msg_pos = 200;
  if (game.numfield_A & 0x02)
  {
      text = buf_sprintf("OUT OF SYNC (GameTurn %7d)", game.play_gameturn);
      error(func_name, 413, text);
      if ( LbScreenIsLocked() )
        LbTextDraw(300/pixel_size, msg_pos/pixel_size, "OUT OF SYNC");
      msg_pos += 20;
  }
  if (game.numfield_A & 0x04)
  {
      text = buf_sprintf("SEED OUT OF SYNC (GameTurn %7d)", game.play_gameturn);
      error(func_name, 427, text);
      if ( LbScreenIsLocked() )
        LbTextDraw(300/pixel_size, msg_pos/pixel_size, "SEED OUT OF SYNC");
      msg_pos += 20;
  }
  return 1;
}

void process_sound_heap(void)
{
    static const char *func_name="process_sound_heap";
    _DK_process_sound_heap();
}

void keeper_gameplay_loop(void)
{
    static const char *func_name="keeper_gameplay_loop";
    short do_draw;
    struct PlayerInfo *player;
#if (BFDEBUG_LEVEL > 5)
    LbSyncLog("%s: Starting\n",func_name);
#endif
    player = &(game.players[my_player_number%PLAYERS_COUNT]);
    PaletteSetPlayerPalette(player, _DK_palette);
    if ((game.numfield_C & 0x02) != 0)
      initialise_eye_lenses();
#if (BFDEBUG_LEVEL > 0)
    LbSyncLog("Entering the gameplay loop for level %d\n",(int)get_loaded_level_number());
#endif
    //the main gameplay loop starts
    while ((!quit_game) && (!exit_keeper))
    {
      if ((game.flags_font & 0x10) != 0)
      {
        if (game.play_gameturn == 4)
          LbNetwork_ChangeExchangeTimeout(0);
      }

      // Check if we should redraw screen in this turn
      do_draw = display_should_be_updated_this_turn() || (!LbIsActive());

      update_mouse();
      input_eastegg();
      input();
      update();

      if ( do_draw )
        keeper_screen_redraw();

      do {
        if ( !LbWindowsControl() )
        {
          if ((game.numfield_A & 0x01) == 0)
          {
            exit_keeper = 1;
            break;
          }
          LbSyncLog("%s - Alex's point reached\n",func_name);
        }
        if ((game.numfield_A & 0x01) || (LbIsActive()))
          break;
      } while ((!exit_keeper) && (!quit_game));

      // Direct information/error messages
      if (LbScreenLock() == 1)
      {
        if ( do_draw )
          perform_any_screen_capturing();
        draw_onscreen_direct_messages();
        LbScreenUnlock();
      }

      // Music and sound control
      if ( !SoundDisabled )
        if ((game.turns_fastforward==0) && (!game.numfield_149F38))
        {
            MonitorStreamedSoundTrack();
            process_sound_heap();
        }

      // Move the graphics window to center of screen buffer and swap screen
      if ( do_draw )
        keeper_screen_swap();

      // Make delay if the machine is too fast
      if ((!game.packet_load_enable) || (game.turns_fastforward == 0))
        keeper_wait_for_next_turn();
      if (game.numfield_149F42 == game.play_gameturn)
        exit_keeper = 1;
    } // end while
#if (BFDEBUG_LEVEL > 0)
    LbSyncLog("Gameplay loop finished after %u turns\n",game.play_gameturn);
#endif
}

void intro(void)
{
    char *fname;
    fname = prepare_file_path(FGrp_LoData, "intromix.smk");
#if (BFDEBUG_LEVEL > 0)
    LbSyncLog("Playing intro movie \"%s\"\n",fname);
#endif
    play_smacker_file(fname, 1);
}

void outro(void)
{
    char *fname;
    fname = prepare_file_path(FGrp_LoData, "outromix.smk");
#if (BFDEBUG_LEVEL > 0)
    LbSyncLog("Playing outro movie \"%s\"\n",fname);
#endif
    play_smacker_file(fname, 17);
}

unsigned long seed_check_random(unsigned long range, unsigned long *seed, const char *func_name, unsigned long place)
{
  if (range == 0)
    return 0;
  unsigned long i;
  i = 9377 * (*seed) + 9439;
  *seed = _lrotr(i, 13);
  i = (*seed) % range;
/*
  if (byte_5642DD & 0x01)
  {
      if (a2 == &dword_5E6742)
        LbSyncLog("%s: place %d, val %d\n", func_name, place, i);
  }
*/
  return i;
}

unsigned long convert_td_iso(unsigned long n)
{
  if ((lens_mode == 2) || (lens_mode == 3))
  {
      if (iso_td[n%TD_ISO_POINTS] >= 0)
        return iso_td[n%TD_ISO_POINTS];
  } else
  {
      if (td_iso[n%TD_ISO_POINTS] >= 0)
        return td_iso[n%TD_ISO_POINTS];
  }
  return n;
}

unsigned long get_creature_anim(struct Thing *thing, unsigned short frame)
{
  unsigned long idx;
  idx = creature_graphics[thing->model%CREATURE_TYPES_COUNT][frame%22];
  return convert_td_iso(idx);
}

unsigned char keepersprite_frames(unsigned short n)
{
  unsigned long i;
  i = creature_list[n%CREATURE_FRAMELIST_LENGTH];
  return creature_table[i].field_9;
}

void set_thing_draw(struct Thing *thing, long a2, long a3, long a4, char a5, char a6, unsigned char a7)
{
    static const char *func_name="set_thing_draw";
  //_DK_set_thing_draw(thing, a2, a3, a4, a5, a6, a7);
  unsigned long i;
  thing->field_44 = convert_td_iso(a2);
  thing->field_50 = (a7 << 2) ^ ((a7 << 2) ^ thing->field_50) & 0x03;
  thing->field_49 = keepersprite_frames(thing->field_44);
  if (a3 != -1)
  {
    thing->field_3E = a3;
  }
  if (a4 != -1)
  {
    thing->field_46 = a4;
  }
  if (a5 != -1)
  {
    thing->field_4F ^= (thing->field_4F ^ (a5 << 6)) & 0x40;
  }
  if (a6 == -2)
  {
    i = keepersprite_frames(thing->field_44) - 1;
    thing->field_48 = i;
    thing->field_40 = i << 8;
  } else
  if (a6 == -1)
  {
    i = seed_check_random(thing->field_49, &game.field_14BB4A, func_name, 34515);
    thing->field_48 = i;
    thing->field_40 = i << 8;
  } else
  {
    i = a6;
    thing->field_48 = i;
    thing->field_40 = i << 8;
  }
}

void light_set_light_minimum_size_to_cache(long a1, long a2, long a3)
{
  _DK_light_set_light_minimum_size_to_cache(a1, a2, a3);
}

struct Thing *create_object(struct Coord3d *pos, unsigned short model, unsigned short owner, long a4)
{
  static const char *func_name="create_object";
  struct Objects *obj;
  struct ObjectConfig *objconf;
  struct InitLight ilight;
  struct Thing *thing;
  long i,k;
  //thing = _DK_create_object(pos, model, owner, a4);

  if (!i_can_allocate_free_thing_structure(1))
  {
    error(func_name,3271,"Cannot create object because there are too many things allocated.");
    return NULL;
  }
  LbMemorySet(&ilight, 0, sizeof(struct InitLight));
  thing = allocate_free_thing_structure(1);
  thing->class_id = 1;
  thing->model = model;
  if (a4 == -1)
    thing->field_1D = -1;
  else
    thing->field_1D = a4;
  LbMemoryCopy(&thing->mappos, pos, sizeof(struct Coord3d));
  objconf = &game.objects_config[model];
  obj = &_DK_objects[thing->model%OBJECT_TYPES_COUNT];
  thing->field_56 = obj->field_9;
  thing->field_58 = obj->field_B;
  thing->field_5A = obj->field_9;
  thing->field_5C = obj->field_B;
  thing->health = saturate_set_signed(objconf->health,16);
  thing->field_20 = objconf->field_4;
  thing->field_23 = 204;
  thing->field_24 = 51;
  thing->field_22 = 0;
  thing->field_25 |= 0x08;

  set_flag_byte(&thing->field_25, 0x40, objconf->field_8);
  thing->owner = owner;
  thing->field_9 = game.play_gameturn;

  if (!obj->field_2)
  {
    i = convert_td_iso(obj->field_5);
    k = 0;
  } else
  {
    i = convert_td_iso(obj->field_5);
    k = -1;
  }
  set_thing_draw(thing, i, obj->field_7, obj->field_D, 0, k, obj->field_11);
  set_flag_byte(&thing->field_4F, 0x02, objconf->field_5);
  set_flag_byte(&thing->field_4F, 0x01, obj->field_3 & 0x01);
  set_flag_byte(&thing->field_4F, 0x10, obj->field_F & 0x01);
  set_flag_byte(&thing->field_4F, 0x20, obj->field_F & 0x02);
  thing->field_7 = obj->field_0;
  if (objconf->light != 0)
  {
    LbMemoryCopy(&ilight.mappos, &thing->mappos, sizeof(struct Coord3d));
    ilight.field_0 = objconf->light;
    ilight.field_2 = objconf->field_B;
    ilight.field_3 = objconf->field_C[0];
    ilight.field_11 = objconf->field_1A;
    thing->field_62 = light_create_light(&ilight);
    if (thing->field_62 == 0)
      error(func_name, 3215, "Cannot allocate light to object");
  }
  switch (thing->model)
  {
    case 3:
      thing->long_13 = game.chest_gold_hold;
      break;
    case 5:
      thing->byte_13.h = 1;
      light_set_light_minimum_size_to_cache(thing->field_62, 0, 56);
      break;
    case 6:
      thing->long_13 = game.pot_of_gold_holds;
      break;
    case 33:
      set_flag_byte(&thing->field_4F, 0x10, false);
      set_flag_byte(&thing->field_4F, 0x20, true);
      break;
    case 43:
      thing->long_13 = game.gold_pile_value;
      break;
    case 49:
      i = get_free_hero_gate_number();
      if (i > 0)
      {
        thing->byte_13.l = i;
      } else
      {
        thing->byte_13.l = 0;
        error(func_name, 3260, "Could not allocate number for hero gate");
      }
      break;
    default:
      break;
  }
  add_thing_to_list(thing, &game.thing_lists[2]);
  place_thing_in_mapwho(thing);
  return thing;
}

struct Thing *find_base_thing_on_mapwho(unsigned char oclass, unsigned short model, unsigned short x, unsigned short y)
{
  return _DK_find_base_thing_on_mapwho(oclass, model, x, y);
}

void init_dungeons(void)
{
  int i,k;
  struct Dungeon *dungeon;
  for (i=0; i < DUNGEONS_COUNT; i++)
  {
    dungeon = &(game.dungeon[game.field_14E496%DUNGEONS_COUNT]);
    dungeon->hates_player[i%DUNGEONS_COUNT] = game.fight_max_hate;
    dungeon = &(game.dungeon[i%DUNGEONS_COUNT]);
    dungeon->hates_player[game.field_14E496%DUNGEONS_COUNT] = game.fight_max_hate;
    dungeon->field_918 = 0;
    dungeon->field_919 = 0;
    dungeon->field_2D = 0;
    dungeon->field_2F = 0;
    dungeon->field_E9F = i;
    dungeon->max_creatures = game.default_max_crtrs_gen_entrance;
    dungeon->field_139F = 0;
    dungeon->field_13A3 = 0;
    for (k=0; k < DUNGEONS_COUNT; k++)
    {
      if (k == i)
        dungeon->hates_player[k] = game.fight_max_love;
      else
        dungeon->hates_player[k] = game.fight_max_hate;
    }
    LbMemorySet(dungeon->field_1489, 0, 32);
  }
}

short thing_model_is_gold_hoarde(unsigned short model)
{
  return ((model >= 52) && (model <= 56));
}

long add_gold_to_hoarde(struct Thing *thing, struct Room *room, long amount)
{
  return _DK_add_gold_to_hoarde(thing, room, amount);
}

long object_is_gold_pile(struct Thing *thing)
{
  return _DK_object_is_gold_pile(thing);
}

int can_thing_be_queried(struct Thing *thing, long a2)
{
  return _DK_can_thing_be_queried(thing, a2);
}

int can_thing_be_possessed(struct Thing *thing, long a2)
{
  return _DK_can_thing_be_possessed(thing, a2);
}

short magic_use_power_hand(unsigned short a1, unsigned short a2, unsigned short a3, unsigned short a4)
{
  return _DK_magic_use_power_hand(a1, a2, a3, a4);
}

long tag_blocks_for_digging_in_rectangle_around(long a1, long a2, char a3)
{
  return _DK_tag_blocks_for_digging_in_rectangle_around(a1, a2, a3);
}

void untag_blocks_for_digging_in_rectangle_around(long a1, long a2, char a3)
{
  _DK_untag_blocks_for_digging_in_rectangle_around(a1, a2, a3);
}

long destroy_door(struct Thing *thing)
{
  return _DK_destroy_door(thing);
}

short destroy_trap(struct Thing *thing)
{
  delete_thing_structure(thing, 0);
  return true;
}

short delete_room_slab(long x, long y, unsigned char gnd_slab)
{
  static const char *func_name="delete_room_slab";
#if (BFDEBUG_LEVEL > 7)
    LbSyncLog("%s: Starting on (%ld,%ld)\n",func_name,x,y);
#endif
  return _DK_delete_room_slab(x, y, gnd_slab);
}

void tag_cursor_blocks_sell_area(unsigned char a1, long a2, long a3, long a4)
{
  _DK_tag_cursor_blocks_sell_area(a1, a2, a3, a4);
}

long packet_place_door(long a1, long a2, long a3, long a4, unsigned char a5)
{
  return _DK_packet_place_door(a1, a2, a3, a4, a5);
}

void delete_room_slabbed_objects(long a1)
{
  _DK_delete_room_slabbed_objects(a1);
}

unsigned char tag_cursor_blocks_place_door(unsigned char a1, long a2, long a3)
{
  return _DK_tag_cursor_blocks_place_door(a1, a2, a3);
}

long remove_workshop_item(long a1, long a2, long a3)
{
  return _DK_remove_workshop_item(a1, a2, a3);
}

struct Thing *create_trap(struct Coord3d *pos, unsigned short a1, unsigned short a2)
{
  return _DK_create_trap(pos, a1, a2);
}

struct Room *place_room(unsigned char a1, unsigned char a2, unsigned short a3, unsigned short a4)
{
  return _DK_place_room(a1, a2, a3, a4);
}

unsigned char tag_cursor_blocks_place_room(unsigned char a1, long a2, long a3, long a4)
{
  return _DK_tag_cursor_blocks_place_room(a1, a2, a3, a4);
}

short magic_use_power_slap(unsigned short plyr_idx, unsigned short stl_x, unsigned short stl_y)
{
  return _DK_magic_use_power_slap(plyr_idx, stl_x, stl_y);
}

/*
 * Updates thing interaction with rooms. Sometimes deletes the given thing.
 * @return Returns true if everything is ok, false if the thing was incorrect.
 */
short check_and_asimilate_thing_by_room(struct Thing *thing)
{
  struct Room *room;
  unsigned long n;
  if (thing_model_is_gold_hoarde(thing->model))
  {
    room = get_room_thing_is_on(thing);
    if (room == NULL)
    {
      delete_thing_structure(thing, 0);
      return false;
    }
    n = (gold_per_hoarde/5)*(thing->model-51);
    thing->owner = room->owner;
    add_gold_to_hoarde(thing, room, n);
  }
  return true;
}

short thing_create_thing(struct InitThing *itng)
{
  static const char *func_name="thing_create_thing";
  struct Thing *thing;
  char *text;
  if (itng->owner == 7)
  {
    text = buf_sprintf("Invalid owning player %d, fixing to %d", (int)itng->owner, (int)game.field_14E496);
    error(func_name, 1225, text);
    itng->owner = game.field_14E496;
  } else
  if (itng->owner == 8)
  {
    text = buf_sprintf("Invalid owning player %d, fixing to %d", (int)itng->owner, (int)game.field_14E497);
    error(func_name, 1231, text);
    itng->owner = game.field_14E497;
  }
  if (itng->owner > 5)
  {
    text = buf_sprintf("Invalid owning player %d, thing discarded", (int)itng->owner);
    error(func_name, 1237, text);
    return false;
  }
  switch (itng->oclass)
  {
  case TCls_Object:
      thing = create_thing(&itng->mappos, itng->oclass, itng->model, itng->owner, itng->index);
      if (thing != NULL)
      {
        if (itng->model == 49)
          thing->byte_13.l = itng->params[1];
        check_and_asimilate_thing_by_room(thing);
        // make sure we don't have invalid pointer
        thing = game.things_lookup[0];
      }
      break;
  case TCls_Creature:
      thing = create_creature(&itng->mappos, itng->model, itng->owner);
      if (thing != NULL)
      {
        init_creature_level(thing, itng->params[1]);
      }
      break;
  case TCls_EffectGen:
      thing = create_effect_generator(&itng->mappos, itng->model, itng->range, itng->owner, itng->index);
      break;
  case TCls_Trap:
      thing = create_thing(&itng->mappos, itng->oclass, itng->model, itng->owner, itng->index);
      break;
  case TCls_Door:
      thing = create_door(&itng->mappos, itng->model, itng->params[0], itng->owner, itng->params[1]);
      break;
  case 10:
  case 11:
      thing = create_thing(&itng->mappos, itng->oclass, itng->model, itng->owner, itng->index);
      break;
  default:
      text = buf_sprintf("Invalid class %d, thing discarded", (int)itng->oclass);
      error(func_name, 1306, text);
      return false;
  }
  if (thing == NULL)
  {
    text = buf_sprintf("Couldn't create thing of class %d, model %d", (int)itng->oclass, (int)itng->model);
    error(func_name, 1306, text);
    return false;
  }
  return true;
}


void init_top_texture_to_cube_table(void)
{
  _DK_init_top_texture_to_cube_table();
}

void init_columns(void)
{
  _DK_init_columns();
}

void init_whole_blocks(void)
{
  struct Column col;
  long i;
  //_DK_init_whole_blocks(); return;
  game.field_149E6E = -1;
  memset(&col, 0, sizeof(col));
  // Prepare the local column
  col.baseblock = 22;
  col.cubes[0] = 10;
  col.cubes[1] = 1;
  col.cubes[2] = 1;
  col.cubes[3] = 1;
  col.cubes[4] = 141;
  make_solidmask(&col);
  // Find it or add to column list
  i = find_column(&col);
  if (i == 0)
    i = create_column(&col);
  // Update its parameters
  game.columns[i].bitfileds |= 0x01;
  game.field_149E7C = 24;
  game.field_149E77 = i;
}

struct ActionPoint *allocate_free_action_point_structure_with_number(long apt_num)
{
  return _DK_allocate_free_action_point_structure_with_number(apt_num);
}

struct ActionPoint *actnpoint_create_actnpoint(struct InitActionPoint *iapt)
{
  struct ActionPoint *apt;
  apt = allocate_free_action_point_structure_with_number(iapt->num);
  if (action_point_is_invalid(apt))
    return &game.action_points[0];
  apt->mappos.x.val = iapt->mappos.x.val;
  apt->mappos.y.val = iapt->mappos.y.val;
  apt->range = iapt->range;
  return apt;
}

void initialise_map_collides(void)
{
  static const char *func_name="initialise_map_collides";
#if (BFDEBUG_LEVEL > 7)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  _DK_initialise_map_collides();
}

void initialise_map_health(void)
{
  static const char *func_name="initialise_map_health";
#if (BFDEBUG_LEVEL > 7)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  _DK_initialise_map_health();
}

RoomKind slab_to_room_type(SlabType slab_type)
{
  switch (slab_type)
  {
  case SlbT_ENTRANCE:
      return RoK_ENTRANCE;
  case SlbT_TREASURE:
      return RoK_TREASURE;
  case SlbT_LIBRARY:
      return RoK_LIBRARY;
  case SlbT_PRISON:
      return RoK_PRISON;
  case SlbT_TORTURE:
      return RoK_TORTURE;
  case SlbT_TRAINING:
      return RoK_TRAINING;
  case SlbT_DUNGHEART:
      return RoK_DUNGHEART;
  case SlbT_WORKSHOP:
      return RoK_WORKSHOP;
  case SlbT_SCAVENGER:
      return RoK_SCAVENGER;
  case SlbT_TEMPLE:
      return RoK_TEMPLE;
  case SlbT_GRAVEYARD:
      return RoK_GRAVEYARD;
  case SlbT_GARDEN:
      return RoK_GARDEN;
  case SlbT_LAIR:
      return RoK_LAIR;
  case SlbT_BARRACKS:
      return RoK_BARRACKS;
  case SlbT_BRIDGE:
      return RoK_BRIDGE;
  case SlbT_GUARDPOST:
      return RoK_GUARDPOST;
  default:
      return RoK_NONE;
  }
}

long slabs_count_near(long tx,long ty,long rad,unsigned short slbtype)
{
  long dx,dy;
  long x,y;
  long count;
  count=0;
  struct SlabMap *slb;
  for (dy=-rad; dy <= rad; dy++)
  {
    y = ty+dy;
    if ((y>=0) && (y<map_tiles_y))
      for (dx=-rad; dx <= rad; dx++)
      {
        x = tx+dx;
        if ((x>=0) && (x<map_tiles_x))
        {
          slb = get_slabmap_block(x, y);
          if (slb->slab == slbtype)
            count++;
        }
      }
  }
  return count;
}

short initialise_map_rooms(void)
{
  static const char *func_name="initialise_map_rooms";
  struct SlabMap *slb;
  struct Room *room;
  unsigned long x,y;
  RoomKind rkind;
#if (BFDEBUG_LEVEL > 7)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  for (y=0; y < map_tiles_y; y++)
    for (x=0; x < map_tiles_x; x++)
    {
      slb = get_slabmap_block(x, y);
      rkind = slab_to_room_type(slb->slab);
      if (rkind > 0)
        room = create_room(slb->field_5 & 7, rkind, 3*x+1, 3*y+1);
      else
        room = NULL;
      if (room != NULL)
      {
        set_room_efficiency(room);
        set_room_capacity(room, 0);
      }
    }
  return true;
}

long ceiling_init(unsigned long a1, unsigned long a2)
{
  return _DK_ceiling_init(a1, a2);
}

void clear_messages(void)
{
  int i;
  for (i=0; i<MESSAGE_QUEUE_COUNT; i++)
  {
    memset(&message_queue[i], 0, sizeof(struct MessageQueueEntry));
  }
}

void do_creature_swap(long ncrt_id, long crtr_id)
{
//TODO: rewrite from DD
  LbWarnLog("Swaping creatures is only supported in Deeper Dungeons\n");
}

TbBool swap_creature(long ncrt_id, long crtr_id)
{
  static const char *func_name="swap_creature";
  char *text;
  if ((crtr_id < 0) || (crtr_id >= CREATURE_TYPES_COUNT))
  {
    text = buf_sprintf("Creature index %d is invalid", crtr_id);
    error(func_name, 81, text);
    return false;
  }
  if (creature_swap_idx[crtr_id] > 0)
  {
    text = buf_sprintf("Creature of index %d already swapped", crtr_id);
    error(func_name, 83, text);
    return false;
  }
  do_creature_swap(ncrt_id, crtr_id);
  return true;
}

void init_dungeon_owner(unsigned short owner)
{
  static const char *func_name="init_dungeon_owner";
  struct Dungeon *dungeon;
  struct Thing *thing;
  int i,k;
  k = 0;
  i = game.thing_lists[2].index;
  while (i>0)
  {
    thing = thing_get(i);
    if (thing_is_invalid(thing))
      break;
    i = thing->next_of_class;
    if ((game.objects_config[thing->model].field_6) && (thing->owner == owner))
    {
      dungeon = &(game.dungeon[owner%DUNGEONS_COUNT]);
      dungeon->field_0 = thing->field_1B;
      break;
    }
    k++;
    if (k > THINGS_COUNT)
    {
      error(func_name,4579,"Infinite loop detected when sweeping things list");
      break;
    }
  }
}

void init_level(void)
{
  static const char *func_name="init_level";
#if (BFDEBUG_LEVEL > 6)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  struct Thing *thing;
  struct Coord3d pos;
  unsigned short mem1,mem2;
  int i,k;
  //_DK_init_level(); return;

  mem2 = game.transfered_creature_kind;
  mem1 = game.transfered_creature_level;
  memset(&pos,0,sizeof(struct Coord3d));
  game.field_14BB4A = 1;
  free_swipe_graphic();
  game.field_1516FF = -1;
  game.play_gameturn = 0;
  clear_game();
  reset_heap_manager();
  lens_mode = 0;
  setup_heap_manager();
  load_computer_player_config();
  init_good_player_as(hero_player);

  light_set_lights_on(1);
  start_rooms = &game.rooms[1];
  end_rooms = &game.rooms[ROOMS_COUNT];

  init_dungeons();
  preload_script(get_selected_level_number());
  load_map_file(get_selected_level_number());

  init_navigation();
  clear_messages();
  LbStringCopy(game.campaign_fname,campaign.fname,sizeof(game.campaign_fname));
  // Initialize unsynchnonized random seed (the value may be different
  // on computers in MP, as it shouldn't affect game actions)
  game.rand_14BB4E = (unsigned long)LbTimeSec();
  if (!SoundDisabled)
  {
    game.field_14BB54 = (seed_check_random(67, &game.rand_14BB4E, func_name, 5712) % 3 + 1);
    game.field_14BB55 = 0;
  }
  light_set_lights_on(1);
  init_dungeon_owner(game.field_14E496);
  game.numfield_D |= 0x04;
  game.transfered_creature_kind = mem2;
  game.transfered_creature_level = mem1;
  event_initialise_all();
  battle_initialise();
  thing = create_ambient_sound(&pos, 1, game.field_14E497);
  if (thing != NULL)
    game.field_14E906 = thing->field_1B;
  else
    error(func_name, 481, "Could not create ambient sound object");
  zero_messages();
  game.field_150356 = 0;
  game.field_15035A = 0;
  init_messages();
  game.field_1517FB = 0;
  game.field_1517FC = 0;
  game.field_15033A = 0;
  game.field_151801 = 0;
  game.field_151805 = 0;
  game.field_151809 = 0;
  game.chosen_spell_type = 0;
  game.chosen_spell_look = 0;
  game.chosen_spell_tooltip = 0;
  game.numfield_151819 = 0;
  game.numfield_15181D = 0;
  game.numfield_151821 = 0;
}

void pannel_map_update(long x, long y, long w, long h)
{
  _DK_pannel_map_update(x, y, w, h);
}

void set_chosen_spell(long sptype, long sptooltip)
{
  if ((sptype < 0) || (sptype >= SPELL_TYPES_COUNT))
    sptype = 0;
  game.chosen_spell_type = sptype;
  game.chosen_spell_look = spell_data[sptype].field_9;
  game.chosen_spell_tooltip = sptooltip;
}

void init_player_music(struct PlayerInfo *player)
{
  static const char *func_name="init_player_music";
  LevelNumber lvnum;
  lvnum = get_loaded_level_number();
  game.audiotrack = ((lvnum - 1) % -4) + 3;
  StopMusic();
  switch (seed_check_random(3, &game.rand_14BB4E, func_name, 363))
  {
  case 0:
      if (LoadAwe32Soundfont("bullfrog"))
        StartMusic(1, 127);
      break;
  case 1:
      if (LoadAwe32Soundfont("atmos1"))
        StartMusic(1, 127);
      break;
  case 2:
      if (LoadAwe32Soundfont("atmos2"))
        StartMusic(1, 127);
      break;
  }
}

void init_player(struct PlayerInfo *player, short no_explore)
{
  static const char *func_name="init_player";
#if (BFDEBUG_LEVEL > 5)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  //_DK_init_player(player, no_explore); return;
  player->mouse_x = 10;
  player->mouse_y = 12;
  player->minimap_zoom = 256;
  player->field_4D1 = player->field_2B;
  setup_engine_window(0, 0, MyScreenWidth, MyScreenHeight);
  player->field_456 = 1;
  player->work_state = 1;
  player->field_14 = 2;
  player->field_4C9 = _DK_palette;
  if (is_my_player(player))
  {
    set_flag_byte(&game.numfield_C,0x40,true);
    set_gui_visible(true);
    init_gui();
    turn_on_menu(GMnu_MAIN);
    turn_on_menu(GMnu_ROOM);
  }
  switch (game.flagfield_14EA4A)
  {
  case 2:
    init_player_as_single_keeper(player);
    init_player_start(player);
    reset_player_mode(player, 1);
    if ( !no_explore )
      init_keeper_map_exploration(player);
    break;
  case 5:
    if (player->field_2C != 1)
    {
      error(func_name, 290, "Non Keeper in Keeper game");
      break;
    }
    init_player_as_single_keeper(player);
    init_player_start(player);
    reset_player_mode(player, 1);
    init_keeper_map_exploration(player);
    break;
  default:
    error(func_name, 309, "How do I set up this player?");
    break;
  }
  init_player_cameras(player);
  pannel_map_update(0, 0, map_subtiles_x+1, map_subtiles_y+1);
  player->strfield_463[0] = '\0';
  if (is_my_player(player))
  {
    init_player_music(player);
  }
  player->field_2A = (1 << player->field_2B);
  player->field_10 = 0;
}

void init_players(void)
{
  struct PlayerInfo *player;
  int i;
  for (i=0;i<PLAYERS_COUNT;i++)
  {
    player=&(game.players[i]);
    player->field_0 ^= (player->field_0 ^ ((game.packet_save_head.field_C & (1 << i)) >> i)) & 1;
    if (player->field_0 & 0x01)
    {
      player->field_2B = i;
      player->field_0 ^= (player->field_0 ^ (((game.packet_save_head.field_D & (1 << i)) >> i) << 6)) & 0x40;
      if ((player->field_0 & 0x40) == 0)
      {
        game.field_14E495++;
        player->field_2C = 1;
        game.flagfield_14EA4A = 5;
        init_player(player, 0);
      }
    }
  }
}

void post_init_level(void)
{
  //_DK_post_init_level(); return;
  if (game.packet_save_enable)
    open_new_packet_file_for_save();
  calculate_dungeon_area_scores();
  init_animating_texture_maps();
  int i,k;
  for (i=0; i < DUNGEONS_COUNT; i++)
  {
    for (k=0; k < CREATURE_TYPES_COUNT; k++)
    {
      game.dungeon[i].creature_max_level[k] = 10;
    }
  }
  clear_creature_pool();
  setup_computer_players2();
  load_stats_files();
  check_and_auto_fix_stats();
  load_script(get_loaded_level_number());
  init_dungeons_research();
  struct PlayerInfo *player;
  struct Thing *thing;
  struct Dungeon *dungeon;
  struct Coord3d *pos;
  if (game.transfered_creature_kind > 0)
  {
    player = &(game.players[my_player_number%PLAYERS_COUNT]);
    dungeon = &(game.dungeon[player->field_2B%DUNGEONS_COUNT]);
    thing = thing_get(dungeon->field_0);
    pos = &(thing->mappos);
    thing = create_creature(pos, game.transfered_creature_kind, 5);
    if (thing != NULL)
      init_creature_level(thing, game.transfered_creature_level);
    game.transfered_creature_kind = 0;
    game.transfered_creature_level = 0;
  }
  update_dungeon_scores();
  update_dungeon_generation_speeds();
  init_traps();
  init_all_creature_states();
  init_keepers_map_exploration();
}

void post_init_players(void)
{
  _DK_post_init_players(); return;
}

short init_animating_texture_maps(void)
{
  static const char *func_name="init_animating_texture_maps";
  int i;
#if (BFDEBUG_LEVEL > 8)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  //_DK_init_animating_texture_maps(); return;
  anim_counter = 7;
  return update_animating_texture_maps();
}

void setup_alliances(void)
{
  int i;
  struct PlayerInfo *player;
  for (i=0; i<PLAYERS_COUNT; i++)
  {
      player = &(game.players[i]);
      if ((i != my_player_number) && (player->field_0 & 0x01))
      {
          if (frontend_is_player_allied(my_player_number, i))
          {
            toggle_ally_with_player(my_player_number, i);
            toggle_ally_with_player(i, my_player_number);
          }
      }
  }
}

/*
 * Exchanges verification packets between all players.
 * @return Returns true if all players return same checksum.
 */
short perform_checksum_verification()
{
  static const char *func_name="perform_checksum_verification";
  struct PlayerInfo *player;
  struct Packet *pckt;
  struct Thing *thing;
  unsigned long checksum_mem;
  short result;
  int i;
  player = &(game.players[my_player_number%PLAYERS_COUNT]);
  result = true;
  checksum_mem = 0;
  for (i=1; i<THINGS_COUNT; i++)
  {
      thing = game.things_lookup[i];
      if (thing_is_invalid(thing))
        continue;
      if ((thing->field_0 & 0x01) != 0)
      {
        checksum_mem += thing->mappos.z.val + thing->mappos.y.val + thing->mappos.x.val;
      }
  }
  clear_packets();
  pckt = &game.packets[player->packet_num%PACKETS_COUNT];
  set_packet_action(pckt, 12, 0, 0, 0, 0);
  pckt->chksum = checksum_mem + game.field_14BB4A;
  if (LbNetwork_Exchange(pckt))
  {
    error(func_name, 210, "Network exchange failed on level checksum verification");
    result = false;
  }
  if ( checksums_different() )
  {
    error(func_name, 219, "Level checksums different for network players");
    result = false;
  }
  return result;
}

short setup_select_player_number(void)
{
  static const char *func_name="setup_select_player_number";
  struct PlayerInfo *player;
  short is_set;
  int i,k;
  is_set = 0;
  k = 0;
#if (BFDEBUG_LEVEL > 6)
  LbSyncLog("%s: Starting\n",func_name);
#endif
  for (i=0; i<NET_PLAYERS_COUNT; i++)
  {
      player=&(game.players[i%PLAYERS_COUNT]);
      if ( net_player_info[i].field_20 )
      {
        player->packet_num = i;
        if ((!is_set) && (my_player_number == i))
        {
          is_set = 1;
          my_player_number = k;
        }
        k++;
      }
  }
  return is_set;
}

void setup_exchange_player_number(void)
{
  static const char *func_name="setup_exchange_player_number";
  struct PlayerInfo *player;
  struct Packet *pckt;
  int i,k;
#if (BFDEBUG_LEVEL > 6)
  LbSyncLog("%s: Starting\n",func_name);
#endif
  clear_packets();
  player = &(game.players[my_player_number%PLAYERS_COUNT]);
  pckt = &game.packets[my_player_number];
  set_packet_action(pckt, 10, player->field_2C, settings.field_3, 0, 0);
  if (LbNetwork_Exchange(pckt))
      error(func_name, 156, "LbNetwork_Exchange failed");
  k = 0;
  for (i=0; i<NET_PLAYERS_COUNT; i++)
  {
      pckt = &game.packets[i];
      if ((net_player_info[i].field_20) && (pckt->action == 10))
      {
          player = &(game.players[k%PLAYERS_COUNT]);
          player->field_2B = k;
          player->field_0 |= 0x01;
          if (pckt->field_8 < 1)
            player->field_4B5 = 2;
          else
            player->field_4B5 = 5;
          player->field_2C = pckt->field_6;
          init_player(player, 0);
          strncpy(player->field_15,net_player[i].name,sizeof(struct TbNetworkPlayerName));
          k++;
      }
  }
}

void init_players_local_game(void)
{
  static const char *func_name="init_players_local_game";
  struct PlayerInfo *player;
#if (BFDEBUG_LEVEL > 4)
  LbSyncLog("%s: Starting\n",func_name);
#endif
  player = &(game.players[my_player_number%PLAYERS_COUNT]);
  player->field_2B = my_player_number;
  player->field_0 |= 0x01;
  if (settings.field_3 < 1u)
    player->field_4B5 = 2;
  else
    player->field_4B5 = 5;
  init_player(player, 0);
}

void init_players_network_game(void)
{
  static const char *func_name="init_players_network_game";
#if (BFDEBUG_LEVEL > 4)
  LbSyncLog("%s: Starting\n",func_name);
#endif
  if (LbNetwork_ChangeExchangeBuffer(game.packets, sizeof(struct Packet)))
      error(func_name, 119, "Unable to reinitialise ExchangeBuffer");
  setup_select_player_number();
  setup_exchange_player_number();
  perform_checksum_verification();
  setup_alliances();
}

void setup_count_players(void)
{
  int i;
  if (game.flagfield_14EA4A == 2)
  {
    game.field_14E495 = 1;
  } else
  {
    game.field_14E495 = 0;
    for (i=0; i<NET_PLAYERS_COUNT; i++)
    {
      if (net_player_info[i].field_20)
        game.field_14E495++;
    }
  }
}

void startup_saved_packet_game(void)
{
  struct PlayerInfo *player;
  int i;
  //_DK_startup_saved_packet_game(); return;
  clear_packets();
  open_packet_file_for_load(game.packet_fname);
  set_selected_level_number(game.packet_save_head.level_num);
  lbDisplay.DrawColour = colours[15][15][15];
  game.pckt_gameturn = 0;
#if (BFDEBUG_LEVEL > 0)
  LbSyncLog("Initialising level %d\n", (int)get_selected_level_number());
  LbSyncLog("Packet Loading Active (File contains %d turns)\n", game.field_149F30);
  if ( game.packet_checksum )
    LbSyncLog("Packet Checksum Active\n");
  LbSyncLog("Fast Forward through %d game turns\n", game.turns_fastforward);
  if (game.numfield_149F42 != -1)
    LbSyncLog("Packet Quit at %d\n", game.numfield_149F42);
  if (game.packet_load_enable)
  {
    if (game.numfield_149F3E != game.numfield_149F3A)
      LbSyncLog("Logging things, game turns %d -> %d\n", game.numfield_149F3A, game.numfield_149F3E);
  }
#endif
  game.flagfield_14EA4A = 2;
  if (!(game.packet_save_head.field_C & (1 << game.numfield_149F46))
    || (game.packet_save_head.field_D & (1 << game.numfield_149F46)) )
    my_player_number = 0;
  else
    my_player_number = game.numfield_149F46;
  init_level();
  init_players();
  if (game.field_14E495 == 1)
    game.flagfield_14EA4A = 2;
  if (game.field_149F30 < game.turns_fastforward)
    game.turns_fastforward = game.field_149F30;
  post_init_level();
  post_init_players();
  set_selected_level_number(0);
}

void faststartup_saved_packet_game(void)
{
  struct PlayerInfo *player;
  reenter_video_mode();
  startup_saved_packet_game();
  player = &(game.players[my_player_number%PLAYERS_COUNT]);
  player->field_6 &= 0xFDu;
  set_gui_visible(false);
  set_flag_byte(&game.numfield_C,0x40,false);
}

void startup_network_game(void)
{
  static const char *func_name="startup_network_game";
#if (BFDEBUG_LEVEL > 0)
  LbSyncLog("Starting up network game.\n");
#endif
  //_DK_startup_network_game(); return;
  unsigned int flgmem;
  struct PlayerInfo *player;
  setup_count_players();
  player = &(game.players[my_player_number%PLAYERS_COUNT]);
  flgmem = player->field_2C;
  init_level();
  player = &(game.players[my_player_number%PLAYERS_COUNT]);
  player->field_2C = flgmem;
  if (game.flagfield_14EA4A == 2)
  {
    init_players_local_game();
  } else
  {
    init_players_network_game();
  }
  if (fe_computer_players)
    setup_computer_players();
  post_init_level();
  post_init_players();
  post_init_packets();
  set_selected_level_number(0);
//!CHEAT! instant win
//player->field_29 = 1;
}

void faststartup_network_game(void)
{
  static const char *func_name="faststartup_network_game";
  struct PlayerInfo *player;
  reenter_video_mode();
  my_player_number = default_loc_player;
  game.flagfield_14EA4A = 2;
  if (!is_campaign_loaded())
  {
    if (!change_campaign(""))
      error(func_name, 867, "Unable to load campaign");
  }
  player=&(game.players[my_player_number%PLAYERS_COUNT]);
  player->field_2C = 1;
  startup_network_game();
  player = &(game.players[my_player_number%PLAYERS_COUNT]);
  player->field_6 &= 0xFDu;
}

int setup_old_network_service(void)
{
    return setup_network_service(_DK_net_service_index_selected);
}

void wait_at_frontend(void)
{
  static const char *func_name="wait_at_frontend";
  //_DK_wait_at_frontend(); return;
#if (BFDEBUG_LEVEL > 0)
  LbSyncLog("Falling into frontend menu.\n");
#endif
  // Moon phase calculation
  calculate_moon_phase(true,false);
  update_extra_levels_visibility();
  // Returning from Demo Mode
  if (game.flags_cd & MFlg_IsDemoMode)
  {
    close_packet_file();
    game.packet_load_enable = 0;
  }
  game.numfield_15 = -1;
  // Make sure campaign is loaded
  if (!load_campaigns_list())
  {
    error(func_name, 727, "No valid campaign files found");
    exit_keeper = 1;
    return;
  }
  // Init load/save catalogue
  initialise_load_game_slots();
  // Prepare to enter PacketLoad game
  if ( (game.packet_load_enable) && (!game.numfield_149F47) )
  {
    faststartup_saved_packet_game();
    return;
  }
  // Prepare to enter network/standard game
  if ((game.numfield_C & 0x02) != 0)
  {
    faststartup_network_game();
    return;
  }

  if ( !setup_screen_mode_minimal(get_frontend_vidmode()) )
  {
    FatalError = 1;
    exit_keeper = 1;
    return;
  }
  LbScreenClear(0);
  LbScreenSwap();
  if ( !frontend_load_data() )
  {
    error(func_name, 738, "Unable to load frontend data");
    exit_keeper = 1;
    return;
  }
  memset(scratch, 0, PALETTE_SIZE);
  LbPaletteSet(scratch);
  frontend_set_state(get_startup_menu_state());

  short finish_menu = 0;
  set_flag_byte(&game.flags_cd,0x40,false);
  // Begin the frontend loop
  long last_loop_time = LbTimerClock();
  do
  {
    if ((!LbWindowsControl()) && ((game.numfield_A & 0x01) == 0))
    {
      exit_keeper = 1;
#if (BFDEBUG_LEVEL > 0)
      LbSyncLog("%s: Windows Control exit condition invoked\n",func_name);
#endif
      break;
    }
//LbSyncLog("update_mouse\n");
    update_mouse();
    update_key_modifiers();
    old_mouse_over_button = frontend_mouse_over_button;
    frontend_mouse_over_button = 0;

    frontend_input();
    if ( exit_keeper )
    {
#if (BFDEBUG_LEVEL > 0)
      LbSyncLog("%s: Frontend Input exit condition invoked\n",func_name);
#endif
      break; // end while
    }

//LbSyncLog("frontend_update\n");
    frontend_update(&finish_menu);
    if ( exit_keeper )
    {
#if (BFDEBUG_LEVEL > 0)
      LbSyncLog("%s: Frontend Update exit condition invoked\n",func_name);
#endif
      break; // end while
    }

    if ((!finish_menu) && (LbIsActive()))
    {
//LbSyncLog("frontend_draw\n");
      frontend_draw();
      LbScreenSwap();
    }

    if (!SoundDisabled)
    {
      process_3d_sounds();
      process_sound_heap();
      MonitorStreamedSoundTrack();
    }

    if (fade_palette_in)
    {
      fade_in();
      fade_palette_in = 0;
    } else
    {
      LbSleepUntil(last_loop_time + 30);
    }
    last_loop_time = LbTimerClock();
  } while (!finish_menu);

  LbPaletteFade(0, 8, Lb_PALETTE_FADE_CLOSED);
  LbScreenClear(0);
  LbScreenSwap();
  short prev_state;
  prev_state = frontend_menu_state;
  frontend_set_state(0);
  if (exit_keeper)
  {
    game.players[my_player_number].field_6 &= 0xFDu;
    return;
  }
  reenter_video_mode();

  display_loading_screen();
  short flgmem;
  switch (prev_state)
  {
  case 7:
        my_player_number = default_loc_player;
        game.flagfield_14EA4A = 2;
        game.numfield_A &= 0xFFFEu;
        game.players[my_player_number].field_2C = 1;
        startup_network_game();
        break;
  case 8:
        game.numfield_A |= 0x01;
        game.flagfield_14EA4A = 5;
        game.players[my_player_number].field_2C = 1;
        startup_network_game();
        break;
  case 10:
        flgmem = game.numfield_15;
        game.numfield_A &= 0xFFFEu;
        if ( game.numfield_15 == -2 )
        {
          error(func_name, 1012, "Why are we here");
          game.numfield_15 = flgmem;
        } else
        {
          LbScreenClear(0);
          LbScreenSwap();
          load_game(game.numfield_15);
          game.numfield_15 = flgmem;
        }
        break;
  case 25:
        game.flags_cd |= MFlg_IsDemoMode;
        startup_saved_packet_game();
        set_gui_visible(false);
        set_flag_byte(&game.numfield_C,0x40,false);
        break;
  }
  game.players[my_player_number].field_6 &= 0xFDu;
}

void game_loop(void)
{
  //_DK_game_loop(); return;
  unsigned long random_seed;
  unsigned long playtime;
  playtime = 0;
  random_seed = 0;
#if (BFDEBUG_LEVEL > 0)
  LbSyncLog("Entering gameplay loop.\n");
#endif
  while ( !exit_keeper )
  {
    update_mouse();
    wait_at_frontend();
    if ( exit_keeper )
      break;
    struct PlayerInfo *player;
    player = &(game.players[my_player_number%PLAYERS_COUNT]);
    if ( game.flagfield_14EA4A == 2 )
    {
      if ( game.numfield_15 == -1 )
      {
        set_player_instance(player, 11, 0);
      } else
      {
        game.numfield_15 = -1;
        set_flag_byte(&game.numfield_C,0x01,false);
      }
    }
    unsigned long starttime;
    unsigned long endtime;
    starttime = LbTimerClock();
    game.dungeon[my_player_number%DUNGEONS_COUNT].lvstats.time1 = timeGetTime();//starttime;
    game.dungeon[my_player_number%DUNGEONS_COUNT].lvstats.time2 = timeGetTime();//starttime;
    LbScreenClear(0);
    LbScreenSwap();
    keeper_gameplay_loop();
    set_pointer_graphic_none();
    LbScreenClear(0);
    LbScreenSwap();
    StopRedbookTrack();
    StopMusic();
    turn_off_all_menus();
    delete_all_structures();
    clear_mapwho();
    endtime = LbTimerClock();
    quit_game = 0;
    if ((game.numfield_C & 0x02) != 0)
        exit_keeper=true;
    playtime += endtime-starttime;
#if (BFDEBUG_LEVEL > 0)
    LbSyncLog("Play time is %d seconds\n",playtime>>10);
#endif
    random_seed += game.play_gameturn;
    reset_eye_lenses();
    close_packet_file();
  } // end while
  // Stop the movie recording if it's on
  if (game.numfield_A & 0x08)
    movie_record_stop();
}

short reset_game(void)
{
  static const char *func_name="reset_game";
#if (BFDEBUG_LEVEL > 6)
    LbSyncLog("%s: Starting\n",func_name);
#endif
  _DK_IsRunningUnmark();
  LbMouseSuspend();
  LbIKeyboardClose();
  LbScreenReset();
  LbDataFreeAll(game_load_files);
  free_gui_strings_data();
  FreeAudio();
  return LbMemoryReset();
}

short process_command_line(unsigned short argc, char *argv[])
{
  static const char *func_name="process_command_line";
  char fullpath[CMDLN_MAXLEN+1];
  strncpy(fullpath, argv[0], CMDLN_MAXLEN);

  sprintf( keeper_runtime_directory, fullpath);
  char *endpos=strrchr( keeper_runtime_directory, '\\');
  if (endpos==NULL)
      endpos=strrchr( keeper_runtime_directory, '/');
  if (endpos!=NULL)
      *endpos='\0';

  SoundDisabled = 0;
  // Note: the working log file is set up in LbBullfrogMain
  _DK_LbErrorLogSetup(0, 0, 1);

  set_default_startup_parameters();

  short bad_param;
  LevelNumber level_num;
  bad_param = 0;
  unsigned short narg;
  level_num = LEVELNUMBER_ERROR;
  narg = 1;
  while ( narg < argc )
  {
      char *par;
      par = argv[narg];
      if ( (par == NULL) || ((par[0] != '-') && (par[0] != '/')) )
          return -1;
      char parstr[CMDLN_MAXLEN+1];
      char pr2str[CMDLN_MAXLEN+1];
      strncpy(parstr, par+1, CMDLN_MAXLEN);
      if (narg+1 < argc)
        strncpy(pr2str,  argv[narg+1], CMDLN_MAXLEN);
      else
        pr2str[0]='\0';

      if (stricmp(parstr, "nointro") == 0)
      {
        start_params.no_intro = 1;
      } else
      if (stricmp(parstr, "nocd") == 0)
      {
          set_flag_byte(&start_params.flags_cd,MFlg_NoMusic,true);
      } else
      if (stricmp(parstr, "1player") == 0)
      {
          start_params.one_player = 1;
      } else
      if ((stricmp(parstr, "s") == 0) || (stricmp(parstr, "nosound") == 0))
      {
          SoundDisabled = 1;
      } else
      if (stricmp(parstr, "fps") == 0)
      {
          narg++;
          start_params.num_fps = atoi(pr2str);
      } else
      if (stricmp(parstr, "human") == 0)
      {
          narg++;
          default_loc_player = atoi(pr2str);
      } else
      if (stricmp(parstr, "usersfont") == 0)
      {
          start_params.flags_font |= 0x40;
      } else
      if (stricmp(parstr, "vidsmooth") == 0)
      {
          smooth_on = 1;
      } else
      if ( stricmp(parstr,"level") == 0 )
      {
        set_flag_byte(&start_params.numfield_C,0x02,true);
        level_num = atoi(pr2str);
        narg++;
      } else
      if (stricmp(parstr,"packetload") == 0)
      {
         if (start_params.packet_save_enable)
            LbWarnLog("PacketSave disabled to enable PacketLoad.\n");
         start_params.packet_load_enable = true;
         start_params.packet_save_enable = false;
         strncpy(start_params.packet_fname,pr2str,149);
         narg++;
      } else
      if (stricmp(parstr,"packetsave") == 0)
      {
         if (start_params.packet_load_enable)
            LbWarnLog("PacketLoad disabled to enable PacketSave.\n");
         start_params.packet_load_enable = false;
         start_params.packet_save_enable = true;
         strncpy(start_params.packet_fname,pr2str,149);
         narg++;
      } else
      if (stricmp(parstr,"q") == 0)
      {
         set_flag_byte(&start_params.numfield_C,0x02,true);
      } else
      if (stricmp(parstr,"columnconvert") == 0)
      {
         set_flag_byte(&start_params.numfield_C,0x08,true);
      } else
      if (stricmp(parstr,"lightconvert") == 0)
      {
         set_flag_byte(&start_params.numfield_C,0x10,true);
      } else
      if (stricmp(parstr,"alex") == 0)
      {
         set_flag_byte(&start_params.flags_font,0x20,true);
      } else
      {
#if (BFDEBUG_LEVEL > 0)
        LbWarnLog("Unrecognized command line parameter '%s'.\n",parstr);
#endif
        bad_param=narg;
      }
      narg++;
  }

  if (level_num == LEVELNUMBER_ERROR)
    level_num = first_singleplayer_level();
  start_params.selected_level_number = level_num;
  my_player_number = default_loc_player;
  return (bad_param==0);
}

void close_video_context(void)
{
  if ( _DK_lpDDC != NULL )
  {
    TDDrawBaseVTable *vtable=_DK_lpDDC->vtable;
    //Note: __thiscall is for functions with no arguments same as __fastcall,
    // so we're using __fastcall and passing the "this" pointer manually
    vtable->reset_screen(_DK_lpDDC);
    if ( _DK_lpDDC != NULL )
    {
      vtable=_DK_lpDDC->vtable;
      // Destructor requires a flag passed in AX
      asm volatile ("  movl $1,%eax\n");
      vtable->dt(_DK_lpDDC);
    }
  }
}

int LbBullfrogMain(unsigned short argc, char *argv[])
{
  static const char *func_name="LbBullfrogMain";
  //return _DK_LbBullfrogMain(argc, argv);
  short retval;
  retval=0;
  LbErrorLogSetup("/", log_file_name, 5);
  strcpy(window_class_name, PROGRAM_NAME);
  LbTimerInit();
  LbSetIcon(110);
  srand(LbTimerClock());

  retval=process_command_line(argc,argv);
  if ( retval < 1 )
  {
      static const char *msg_text="Command line parameters analysis failed.\n";
      error_dialog_fatal(func_name, 1, msg_text);
      close_video_context();
      LbErrorLogClose();
      return 0;
  }

  retval = setup_game();
  if ( retval )
    game_loop();
  reset_game();
  close_video_context();
  if ( !retval )
  {
      static const char *msg_text="Setting up game failed.\n";
      error_dialog_fatal(func_name, 2, msg_text);
  } else
  {
#if (BFDEBUG_LEVEL > 0)
    LbSyncLog("%s finished properly.\n",func_name);
#endif
  }
  LbErrorLogClose();
  return 0;
}

void get_cmdln_args(unsigned short &argc, char *argv[])
{
  char *ptr;
  const char *cmndln_orig;
  cmndln_orig = GetCommandLineA();
  strncpy(cmndline, cmndln_orig, CMDLN_MAXLEN);
  ptr = cmndline;
  bf_argc = 0;
  while (*ptr != '\0')
  {
      if ((*ptr == '\t') || (*ptr == ' '))
      {
          ptr++;
          continue;
      }
      if (*ptr == '\"')
      {
          ptr++;
          bf_argv[bf_argc] = ptr;
          bf_argc++;
          while (*ptr != '\0')
          {
            if (*ptr == '\"')
            {
              *ptr++ = '\0';
              break;
            }
            ptr++;
          }
      } else
      {
          bf_argv[bf_argc] = ptr;
          bf_argc++;
          while (*ptr != '\0')
          {
            if ((*ptr == '\t') || (*ptr == ' '))
            {
              *ptr++ = '\0';
              break;
            }
            ptr++;
          }
      }
  }
}

int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nFunsterStil)
{
//  return _DK_WinMain(hThisInstance,hPrevInstance,lpszArgument,nFunsterStil);
  static const char *func_name="WinMain";
  char *text;
  _DK_hInstance = hThisInstance;
  _DK_lpDDC = NULL;

  get_cmdln_args(bf_argc, bf_argv);

//TODO: delete when won't be needed anymore
  memcpy(_DK_menu_list,menu_list,40*sizeof(struct GuiMenu *));
  memcpy(_DK_player_instance_info,player_instance_info,17*sizeof(struct PlayerInstanceInfo));
#if (BFDEBUG_LEVEL > 1)
/*  {
      struct PlayerInfo *player=&(game.players[0]);
      text = buf_sprintf("Position of the first Player is %06x, first Camera is %06x bytes.\n",((int)player) - ((int)&_DK_game),((int)&(player->acamera)) - ((int)player));
      error_dialog(func_name, 1, text);
      return 0;
  }
  {
      struct Dungeon *dungeon=&(game.dungeon[0]);
      text = buf_sprintf("Position of the first Dungeon is %06x, field_ACF is at %06x bytes.\n",
                  ((int)dungeon) - ((int)&game),((int)(&dungeon->field_ACF)) - ((int)dungeon));
      error_dialog(func_name, 1, text);
      return 0;
  }*/
  if (sizeof(struct PlayerInfo) != SIZEOF_PlayerInfo)
  {
      text = buf_sprintf("Bad compilation - struct PlayerInfo has wrong size!\nThe difference is %d bytes.\n",sizeof(struct PlayerInfo)-SIZEOF_PlayerInfo);
      error_dialog(func_name, 1, text);
      return 0;
  }
  if (sizeof(struct Dungeon) != SIZEOF_Dungeon)
  {
      text = buf_sprintf("Bad compilation - struct Dungeon has wrong size!\nThe difference is %d bytes.\n",sizeof(struct Dungeon)-SIZEOF_Dungeon);
      error_dialog(func_name, 1, text);
      return 0;
  }
  if (sizeof(struct Game) != SIZEOF_Game)
  {
      text = buf_sprintf("Bad compilation - struct Game has wrong size!\nThe difference is %d bytes.\n",sizeof(struct Game)-SIZEOF_Game);
      error_dialog(func_name, 1, text);
      return 0;
  }
#endif

  LbBullfrogMain(bf_argc, bf_argv);

//  LbFileSaveAt("!tmp_file", &_DK_game, sizeof(struct Game));

  return 0;
}
