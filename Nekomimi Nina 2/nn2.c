// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#include "../nonnon/neutral/time.c"


#include "../nonnon/neutral/bmp/ui/rectframe.c"
#include "../nonnon/neutral/bmp/ui/roundframe.c"


#include "../nonnon/game/helper.c"
#include "../nonnon/game/progressbar.c"
#include "../nonnon/game/transition.c"


#include "../nonnon/win32/gdi.c"




#define N_NN2_SOUND_OFF ( 0 )
#define N_NN2_SOUND_ON  ( 1 )




#ifdef DEBUG

//#define N_NN2_TITLE_ON

#define N_NN2_SOUND_ONOFF        N_NN2_SOUND_ON

//#define N_NN2_STAGE_SHUFFLE

#define N_NN2_STAGE_START        &n_nn2_stage_2

#define N_NN2_STAGE_NUMBER_CHICK ( 2 )
#define N_NN2_STAGE_NUMBER_KUINA ( 1 )
#define N_NN2_STAGE_NUMBER_ROCKS ( 1 )

#define N_NN2_STAGE_7_TIMEUP     ( 1000 * 10 )

#define N_NN2_DRAW_DOKAN
#define N_NN2_DOKAN_LABEL_SHOW

//#define N_NN2_KUINA_AUTO

//#define N_NN2_DEBUG_WEATHER_BUTTON

#define N_NN2_BELL_DEFAULT ( 30 )

#define N_NN2_TREASURE_ONOFF

#define N_NN2_DEBUG_LOOP FALSE

void
n_nn2_debug_hangup_detector( NSString *nsstr )
{

	NSLog( @"%@", nsstr );

	return;
}

#else

#define N_NN2_TITLE_ON

#define N_NN2_SOUND_ONOFF N_NN2_SOUND_ON

#define N_NN2_STAGE_SHUFFLE

#define N_NN2_STAGE_START &n_nn2_stage_1

#define N_NN2_STAGE_NUMBER_CHICK ( 2 )
#define N_NN2_STAGE_NUMBER_KUINA ( 1 )
#define N_NN2_STAGE_NUMBER_ROCKS ( 2 )

#define N_NN2_STAGE_7_TIMEUP ( 1000 * 15 )

#define N_NN2_DRAW_DOKAN

#define N_NN2_KUINA_AUTO

#define N_NN2_BELL_DEFAULT ( 0 )

#define N_NN2_TREASURE_ONOFF

#define N_NN2_DEBUG_LOOP FALSE
#define n_nn2_debug_hangup_detector

#endif




#include "_sprite.c"




#define n_bmp_a_mac n_bmp_a
#define n_bmp_r_mac n_bmp_b
#define n_bmp_g_mac n_bmp_g
#define n_bmp_b_mac n_bmp_r

#define n_nn2_bmp_new(       bmp, sx, sy ) n_nn2_bmp_new_internal( bmp, sx, sy, n_bmp_black_invisible )
#define n_nn2_bmp_new_white( bmp, sx, sy ) n_nn2_bmp_new_internal( bmp, sx, sy, n_bmp_white_invisible )

void
n_nn2_bmp_new_internal( n_bmp *bmp, n_type_gfx sx, n_type_gfx sy, u32 color )
{

	n_bmp_zero( bmp );

	n_bmp_new_fast( bmp, sx, sy );

	n_bmp_flush( bmp, color );


	return;
}

void
n_nn2_bmp_flush_mixer( n_bmp *bmp, u32 color_mix, n_type_real blend )
{

	if ( n_bmp_error( bmp ) ) { return; }


	n_type_gfx c = N_BMP_SX( bmp ) * N_BMP_SY( bmp );
	n_type_gfx i = 0;
	n_posix_loop
	{

		u32 color_f = N_BMP_PTR( bmp )[ i ];
		if ( 0 != n_bmp_a( color_f ) )
		{
			u32 color_t = n_bmp_blend_pixel( color_f, color_mix, blend );
			if ( color_f != color_t )
			{
				N_BMP_PTR( bmp )[ i ] = color_t;
			}
		}

		i++;
		if ( i >= c ) { break; }
	}


	return;
}




CGFloat
n_nn2_flowy_effect( CGFloat maxim, CGFloat step )
{
	return maxim * fabs( sin( 2.0 * M_PI * step ) );
}

BOOL
n_nn2_flowy_effect_inc( CGFloat maxim, CGFloat *step, CGFloat boost )
{

	(*step) += 0.25 / maxim * boost;
	if ( (*step) >= 0.25 )
	{
		(*step) = 0.25;
		return TRUE;
	}

	return FALSE;
}




void
n_nn2_rc_load_raw( NSString *name, n_bmp *bmp )
{

	n_bmp_free( bmp );
//return;

	NSBundle *main = [NSBundle mainBundle];
	if ( main == NULL ) { return; }

	NSString *path = [main pathForResource:name ofType:@"png"];
	if ( path == NULL ) { return; }

	NSImage *image = [[NSImage alloc] initWithContentsOfFile:path];
	if ( image == nil ) { return; }

	n_mac_image_nsimage2nbmp( image, bmp );

	n_bmp_mac_color( bmp );


	return;
}

void
n_nn2_rc_load( NSString *name, n_bmp *bmp, int scaler )
{

	n_nn2_rc_load_raw( name, bmp );


	int i = 0;
	n_posix_loop
	{
		if ( i >= scaler ) { break; }

		n_bmp_flush_antialias( bmp, 1.0 );
		n_bmp_flush_antialias( bmp, 1.0 );

		n_bmp_scaler_lil( bmp, 2 );

		i++;
	}


	return;
}

void
n_nn2_rc_load_map( NSString *name, n_bmp *bmp )
{

	n_nn2_rc_load_raw( name, bmp );


	return;
}




#define N_NN2_INPUT_NEUTRAL ( 0 << 0 )
#define N_NN2_INPUT_U       ( 1 << 0 )
#define N_NN2_INPUT_D       ( 1 << 1 )
#define N_NN2_INPUT_L       ( 1 << 2 )
#define N_NN2_INPUT_R       ( 1 << 3 )


#define N_NN2_DIRECTION_NONE  ( 0 )
#define N_NN2_DIRECTION_LEFT  ( 1 )
#define N_NN2_DIRECTION_RIGHT ( 2 )


#define N_NN2_DASH_NONE ( 0 )
#define N_NN2_DASH_INIT ( 1 )
#define N_NN2_DASH_DONE ( 2 )


#define N_NN2_DASH_BRAKE_NONE ( 0 )
#define N_NN2_DASH_BRAKE_INIT ( 1 )
#define N_NN2_DASH_BRAKE_DONE ( 2 )

#define N_NN2_DASH_BRAKE_MSEC ( 200 )


#define N_NN2_JUMP_INTERVAL   ( 5 )
#define N_NN2_SWIM_INTERVAL   ( 2 )

#define N_NN2_JUMP_STATE_NONE ( 0 )
#define N_NN2_JUMP_STATE_RISE ( 1 )
#define N_NN2_JUMP_STATE_FALL ( 2 )
#define N_NN2_JUMP_STATE_DONE ( 3 )


#define N_NN2_DRAW_NONE ( 0 )
#define N_NN2_DRAW_WALK ( 1 )
#define N_NN2_DRAW_DASH ( 2 )
#define N_NN2_DRAW_IDLE ( 3 )
#define N_NN2_DRAW_JUMP ( 4 )
#define N_NN2_DRAW_SUCK ( 5 )
#define N_NN2_DRAW_SLIP ( 6 )
#define N_NN2_DRAW_LAND ( 7 )
#define N_NN2_DRAW_DUCK ( 8 )
#define N_NN2_DRAW_TURN ( 9 )


#define N_NN2_COSTUME_BLUE ( 0 )
#define N_NN2_COSTUME_PINK ( 1 )


#define N_NN2_CHIP_UNIT  ( 32 * p->zoom )

#define N_NN2_STEP_INIT  (  2 * p->zoom )
#define N_NN2_STEP_WALK  (  4 * p->zoom )
#define N_NN2_STEP_DASH  ( 10 * p->zoom )
#define N_NN2_STEP_FALL  ( 12 * p->zoom )
#define N_NN2_STEP_BRAKE (  6 * p->zoom )
#define N_NN2_STEP_FORCE (  4 * p->zoom )

#define N_OBJ_STEP_LIFT  (   1 * p->zoom )
#define N_OBJ_STEP_MOVE  (   2 * p->zoom )
#define N_OBJ_STEP_FALL  (  12 * p->zoom )
#define N_OBJ_JUMP_MAX   ( 128 * p->zoom )
#define N_OBJ_ON_LIFT    (  16 * p->zoom )

#define N_NN2_BRICK_MOVE_STEP (  4 * p->zoom )
#define N_NN2_BRICK_MOVE_MAX  ( 12 * p->zoom )

#define N_NN2_STEP_DUCK_HALF ( 23 * p->zoom )
#define N_NN2_STEP_DUCK_FULL ( 50 * p->zoom )


#define N_NN2_BGCLOUD_MAX ( 4 )
#define N_NN2_LIFT_MAX    N_NN2_BGCLOUD_MAX


#define N_OBJECT_KIRAKIRA_MAX ( 100 )


#define N_NN2_SPOTLIGHT_SIZE ( 333 )


#define N_NN2_TIMEUP_BAR_SIZE ( 12 )


#define N_NN2_TREASURE_BMP_MAX ( 30 )


#define N_NN2_FONT_NAME_TITLE     "Trebuchet MS"
#define N_NN2_FONT_NAME_INDICATOR "Hiragino Maru Gothic Pro"


#define N_NN2_SPLASH_SIZE  ( 24 )
#define N_NN2_SPLASH_COUNT ( 16 )




#define N_NN2_BUBBLE_MAX ( 3 )

typedef struct {

	BOOL       onoff;

	n_type_gfx x, y, sx, sy;

} n_particle;




typedef struct {

	// [!] : Main

	n_bmp       canvas_background;
	n_bmp       canvas_middleground;
	n_bmp       canvas_debug;

	n_type_gfx  nina_x;
	n_type_gfx  nina_y;

	BOOL        middleground_onoff;

	n_bmp       bmp_stage_number;


	// [!] : Map

	n_bmp       map;
	n_bmp       map_dokan;
	n_bmp       map_queue;
	n_bmp       map_move_h;
	n_bmp       map_move_v;
	n_bmp       map_lift_guideline;
	n_bmp       map_pass_thru;
	n_bmp       map_timeup;
	n_bmp       map_bell;
	n_bmp       map_label;

	n_type_gfx  map_sx;
	n_type_gfx  map_sy;
	n_type_gfx  map_max_sx;
	n_type_gfx  map_max_sy;


	// [!] : Camerawork

	n_type_gfx  center_x, center_y;

	n_type_gfx  camera_x, camera_y;
	n_type_gfx  camera_oy;
	int         camera_phase;
	BOOL        camera_first;
	n_type_gfx  camera_step;
	int         camera_mode;
	BOOL        camera_lock;
	BOOL        camera_reset;
	n_type_gfx  camera_wave_offset;
	BOOL        camera_wave_under;


	// [!] : Chips

	n_sprite   *chip_map;

	n_bmp       chip_earth[ 6 + 2 ];
	n_bmp       chip_block;
	n_bmp       chip_brick_1;
	n_bmp       chip_brick_2;
	n_bmp       chip_brick_3;
	n_bmp       chip_rocks_1;
	n_bmp       chip_rocks_2;
	n_bmp       chip_rocks_3;
	n_bmp       chip_special_lite;
	n_bmp       chip_special_dark;
	n_bmp       chip_hidden;
	n_bmp       chip_debug_1;
	n_bmp       chip_debug_2;
	n_bmp       chip_debug_3;


	// [!] : Objects

	n_sprite    cld_h_0_sprite;
	n_sprite    cld_h_1_sprite;
	n_sprite    cld_v_0_sprite;
	n_sprite    cld_v_1_sprite;
	//n_sprite    cld_v_2_sprite;
	//n_sprite    cld_v_3_sprite;

	n_sprite    bgcloud_sprite  [ N_NN2_BGCLOUD_MAX ];
	n_bmp       bgcloud_bmp     [ N_NN2_BGCLOUD_MAX ];
	u32         bgcloud_interval[ N_NN2_BGCLOUD_MAX ];
	u32         bgcloud_timer   [ N_NN2_BGCLOUD_MAX ];

	BOOL        treasure_ready;
	int         treasure_phase;
	int         treasure_state;
	n_type_gfx  treasure_x;
	n_type_gfx  treasure_y;

} n_nn2_stage;

#define n_nn2_stage_zero( p ) n_memory_zero( p, sizeof( n_nn2_stage ) )




typedef struct {

	n_sprite debug_move;
	n_sprite debug_jump;

	n_sprite chick_sprite;
	n_sprite kuina_sprite;
	n_sprite rocks_sprite;

	n_sprite bird1_sprite;
	n_sprite bird2_sprite;
	n_sprite bird3_sprite;

	n_sprite fish1_sprite;
	n_sprite fish2_sprite;
	n_sprite fish3_sprite;

} n_nn2_share;




#define N_NN2_STAGE_MAX       ( 9 )
#define N_NN2_DOKAN_LABEL_MAX ( N_NN2_STAGE_MAX + 1 )

static n_nn2_stage n_nn2_stage_0;
static n_nn2_stage n_nn2_stage_1;
static n_nn2_stage n_nn2_stage_2;
static n_nn2_stage n_nn2_stage_3;
static n_nn2_stage n_nn2_stage_4;
static n_nn2_stage n_nn2_stage_5;
static n_nn2_stage n_nn2_stage_6;
static n_nn2_stage n_nn2_stage_7;
static n_nn2_stage n_nn2_stage_8;




typedef struct {

	n_type_gfx walk_interval;
	n_type_gfx walk_step;

	n_type_gfx dash_value_max;
	n_type_gfx dash_interval;
	n_type_gfx dash_step;

	n_type_gfx jump_interval;
	n_type_gfx jump_up;
	n_type_gfx jump_fall;
	n_type_gfx jump_lr_walk;
	n_type_gfx jump_lr_dash;
	n_type_gfx jump_boost;

	n_type_gfx swim_interval;
	n_type_gfx swim_up;
	n_type_gfx swim_lr_walk;

} n_powerup;


void
n_nn2_powerup_init_blue( n_powerup* p )
{

	p->walk_interval  = 25;
	p->walk_step      = 1;

	p->dash_value_max = 15;
	p->dash_interval  = 100;
	p->dash_step      = 1;

	p->jump_interval  = 5;
	p->jump_up        = 256;
	p->jump_fall      = 12;
	p->jump_lr_walk   = ( 10 / 2 ) * sqrt( 2 );
	p->jump_lr_dash   = p->jump_lr_walk;
	p->jump_boost     = 15;

	p->swim_interval  = 2;
	p->swim_up        = 256;
	p->swim_lr_walk   = 1;


	return;
}

void
n_nn2_powerup_init_pink( n_powerup* p )
{

	p->walk_interval  = 25  * 0.5;
	p->walk_step      = 1   * 1.5;

	p->dash_value_max = 15  * 1.5;
	p->dash_interval  = 100 * 0.5;
	p->dash_step      = 1   * 1.5;

	p->jump_interval  = 5   * 1.5;
	p->jump_up        = 256 * 1.5;
	p->jump_fall      = 12;
	p->jump_lr_walk   = ( 10 / 2 ) * sqrt( 2 );
	p->jump_lr_dash   = p->jump_lr_walk;
	p->jump_boost     = 15  * 0.5;

	p->swim_interval  = 2;
	p->swim_up        = 256 * 5.0;
	p->swim_lr_walk   = 2;


	return;
}




typedef struct {

	BOOL        onoff;
	n_type_gfx  x, y;
	n_type_gfx  ox, oy;
	CGFloat     flowy;
	u32         timer;

} n_splash;




typedef struct {

	NonnonGame *self;

	BOOL        redraw_onoff;
	BOOL        game_onoff;

	CGFloat     interval_frame;
	CGFloat     interval_display;

	NSWindow   *window;
	NSWindow   *sheet;
	BOOL        sheet_onoff;
	BOOL        sheet_escape_key;

	n_type_gfx  sx;
	n_type_gfx  sy;

	n_bmp      *canvas;
	n_bmp       canvas_main;

	n_type_gfx  scaler;
	n_type_gfx  zoom;

	int         input;

	NSOperationQueue *queue;
	int               cores;

	GCExtendedGamepad *gamepad;

	BOOL        debug_pause;
	n_sprite   *debug_sprite;
	NSRect      debug_rect;
	n_type_gfx  debug_chip_x;
	n_type_gfx  debug_chip_y;

	int         draw_center;


	// [!] : Transition Engine

	BOOL        transition_lock;
	int         transition_phase;
	int         transition_type;
	n_type_real transition_percent;
	n_bmp       transition_bmp_old;
	n_bmp       transition_bmp_mid;
	n_bmp       transition_bmp_new;


	// [!] : time limit

	u32         timeup;
	int         timeup_mode;


	// [!] : Map Chips

	n_nn2_stage *stage;
	n_nn2_stage *stage_next;

	n_nn2_share  share_main;
	n_nn2_share *share;

	n_type_gfx   mapchip_unit;

	int          brick_animation_index;
	n_sprite    *brick_animation_sprite;

	n_type_gfx   map_sx_prv;


	// [!] : Character

	n_bmp       bmp_shared;

	int         direction;

	BOOL        costume;

	int         wall_collision;

	BOOL          idle_onoff;
	BOOL          duck_onoff;
	BOOL        unduck_onoff;

	BOOL        suck_stop;
	int         suck_phase;
	int         suck_direction;

	BOOL        lr_lock;

	n_powerup  *powerup;
	n_powerup   powerup_blue;
	n_powerup   powerup_pink;

	n_type_gfx  nina_walk_step;

	BOOL        dash_onoff;
	CGFloat     dash_float;
	u32         dash_timer;
	int         dash_direction;
	int         dash_phase;
	n_type_gfx  dash_value;
	int         dash_timeout_phase;
	u32         dash_timeout_timer;

	int         brake_phase;
	u32         brake_press;
	u32         brake_timer;
	int         brake_direction;

	BOOL        jump_onoff;
	int         jump_state;
	int         jump_state_prv;
	CGFloat     jump_float;
	int         jump_flow;
	n_type_gfx  jump_vertical_threshold;
	n_type_gfx  jump_vertical_start_x;
	BOOL        jump_vertical_onoff;
	BOOL        jump_reverse_onoff;
	int         jump_direction;

	int         fly_phase;
	BOOL        fly_down;

	BOOL        land_onoff;

	BOOL        turn_onoff;
	int         turn_direction;

	int         hipdrop_state;
	u32         hipdrop_timer;
	BOOL        hipdrop_high_jump_lock;
	BOOL        hipdrop_cliff_lock;
	int         hipdrop_cliff_direction;

	BOOL        wave_onoff;
	BOOL        swim_onoff;

	n_type_gfx  alignment_adjuster;

	int         wink;
	int         wink_frame;
	u32         wink_timer;

	CGFloat     global_blend;
	CGFloat       chip_blend;
	CGFloat      dokan_blend;
	CGFloat      dokan_blend_stop;

	n_sprite   *sprite_cur;

	n_type_gfx  nina_sx, nina_sy;

	n_type_gfx  nina_margin_head;
	n_type_gfx  nina_margin_hair;
	n_type_gfx  nina_margin_duck;
	n_type_gfx  nina_margin_foot;
	n_type_gfx  nina_margin_land;
	n_type_gfx  nina_margin_fwrd;
	n_type_gfx  nina_margin_rear;
	n_type_gfx  nina_margin_swim;
	//n_type_gfx  nina_margin_fmax;

	n_bmp       nina_stub;

	n_bmp       nina_idle_leg1;
	n_bmp       nina_idle_leg2;
	n_bmp       nina_idle_cliff;

	n_bmp       nina_swim_blue_body;
	n_bmp       nina_swim_blue_boob;
	n_bmp       nina_swim_blue_turn;
	n_bmp       nina_swim_pink_body;
	n_bmp       nina_swim_pink_boob;
	n_bmp       nina_swim_pink_turn;

	n_bmp       nina_walk_hair_r;
	n_bmp       nina_walk_hair_s;
	n_bmp       nina_walk_hair_f;
	n_bmp       nina_walk_face;
	n_bmp       nina_walk_ear;
	n_bmp       nina_walk_head;

	n_bmp       nina_walk_wink_1;
	n_bmp       nina_walk_wink_2;
	n_bmp       nina_walk_wink_3;
	n_bmp       nina_walk_neko;

	n_bmp       nina_stub_body;
	n_bmp       nina_stub_boob;
	n_bmp       nina_stub_hip;
	n_bmp       nina_stub_turn;
	n_bmp       nina_stub_sleeve_l;
	n_bmp       nina_stub_sleeve_m;
	n_bmp       nina_stub_sleeve_r;

	n_bmp       nina_blue_body;
	n_bmp       nina_blue_boob;
	n_bmp       nina_blue_hip;
	n_bmp       nina_blue_turn;
	n_bmp       nina_blue_sleeve_l;
	n_bmp       nina_blue_sleeve_m;
	n_bmp       nina_blue_sleeve_r;

	n_bmp       nina_pink_body;
	n_bmp       nina_pink_boob;
	n_bmp       nina_pink_hip;
	n_bmp       nina_pink_turn;
	n_bmp       nina_pink_sleeve_l;
	n_bmp       nina_pink_sleeve_m;
	n_bmp       nina_pink_sleeve_r;

	n_bmp       nina_walk_arm_f_1;
	n_bmp       nina_walk_arm_f_2;
	//n_bmp       nina_walk_arm_f_3;
	n_bmp       nina_walk_arm_n;
	n_bmp       nina_walk_arm_r_1;
	n_bmp       nina_walk_arm_r_2;
	//n_bmp       nina_walk_arm_r_3;

	//n_bmp       nina_walk_leg_f_1;
	n_bmp       nina_walk_leg_f_2;
	n_bmp       nina_walk_leg_fd2;
	n_bmp       nina_walk_leg_f_3;
	n_bmp       nina_walk_leg_n;
	//n_bmp       nina_walk_leg_r_1;
	n_bmp       nina_walk_leg_r_2;
	n_bmp       nina_walk_leg_r_3;

	//n_bmp       nina_walk_lgS_f_1;
	n_bmp       nina_walk_lgS_f_2;
	n_bmp       nina_walk_lgS_fd2;
	n_bmp       nina_walk_lgS_f_3;
	n_bmp       nina_walk_lgS_n;
	//n_bmp       nina_walk_lgS_r_1;
	n_bmp       nina_walk_lgS_r_2;
	n_bmp       nina_walk_lgS_r_3;

	n_bmp       nina_walk_amS_f_1;
	n_bmp       nina_walk_amS_f_2;
	//n_bmp       nina_walk_amS_f_3;
	//n_bmp       nina_walk_amS_n;
	n_bmp       nina_walk_amS_r_1;
	n_bmp       nina_walk_amS_r_2;
	//n_bmp       nina_walk_amS_r_3;

	n_bmp       nina_dash_arm_n;
	n_bmp       nina_dash_arm_f_1;
	n_bmp       nina_dash_amS_f_1;
	n_bmp       nina_dash_arm_f_2;
	n_bmp       nina_dash_amS_f_2;
	n_bmp       nina_dash_arm_r_1;
	n_bmp       nina_dash_amS_r_1;
	n_bmp       nina_dash_arm_r_2;
	n_bmp       nina_dash_amS_r_2;

	n_bmp       nina_dash_leg_f_1;
	n_bmp       nina_dash_lgS_f_1;
	n_bmp       nina_dash_leg_f_2;
	n_bmp       nina_dash_lgS_f_2;
	n_bmp       nina_dash_leg_r_1;
	n_bmp       nina_dash_lgS_r_1;
	n_bmp       nina_dash_leg_r_2;
	n_bmp       nina_dash_lgS_r_2;

	n_bmp       nina_duck_full_body;
	n_bmp       nina_duck_full_head;
	n_bmp       nina_duck_full_hair_rear;
	n_bmp       nina_duck_full_hair_side;
	n_bmp       nina_duck_full_hair_front;
	n_bmp       nina_duck_full_wink_1;
	n_bmp       nina_duck_full_wink_2;
	n_bmp       nina_duck_full_wink_3;
	n_bmp       nina_duck_full_neko;

	n_bmp       nina_duck_half_body;
	n_bmp       nina_duck_half_head;
	n_bmp       nina_duck_half_hair_rear;
	n_bmp       nina_duck_half_hair_side;
	n_bmp       nina_duck_half_hair_front;
	n_bmp       nina_duck_half_wink_1;
	n_bmp       nina_duck_half_wink_2;
	n_bmp       nina_duck_half_wink_3;
	n_bmp       nina_duck_half_neko;

	n_bmp       nina_jump_leg_1;
	n_bmp       nina_jump_leg_2;
	n_bmp       nina_jump_leg_3;
	n_bmp       nina_jump_lgS_1;
	n_bmp       nina_jump_lgS_2;
	n_bmp       nina_jump_lgS_3;
	n_bmp       nina_jump_leg_t;
	n_bmp       nina_jump_arm_1;
	n_bmp       nina_jump_arm_2;

	n_bmp       nina_suck_arm_1;
	n_bmp       nina_suck_leg_1;

	n_bmp       nina_indicator;

	n_bmp       nina_cliff_sweat;


	BOOL        object_handheld_ready;
	BOOL        object_handheld_onoff;
	BOOL        object_handheld_lock;
	BOOL        object_handheld_skim_ud;
	BOOL        object_handheld_skim_lr;
	n_sprite   *object_handheld_sprite;

	n_sprite   *object_dokan_sprite;

	n_bmp       chick_body;
	n_bmp       chick_wink;
	n_bmp       chick_wing_n;
	n_bmp       chick_wing_1;
	n_bmp       chick_wing_2;
	n_bmp       chick_foot_n;
	n_bmp       chick_foot_1;
	n_bmp       chick_foot_2;

	int         chick_stage_number_phase;
	n_bmp       chick_stage_number_indicator;
	n_bmp       chick_stage_number_bmp;
	int         chick_stage_number_prv;
	n_type_gfx  chick_stage_number_remover;

	n_bmp       kuina_body;
	n_bmp       kuina_wink;
	n_bmp       kuina_wing_n;
	n_bmp       kuina_wing_1;
	n_bmp       kuina_wing_2;
	n_bmp       kuina_foot_n;
	n_bmp       kuina_foot_1;

	n_bmp       bird_body;
	n_bmp       bird_wink_1;
	n_bmp       bird_wink_2;
	n_bmp       bird_wing_n;
	n_bmp       bird_wing_1;
	n_bmp       bird_wing_2;

	n_bmp       fish_bmp;

	n_bmp       lift_bmp[ N_NN2_LIFT_MAX ];
	n_bmp       lift_bmp_face;

	n_bmp       mountain_1;
	n_bmp       mountain_2;

	BOOL        dokan_onoff;
	BOOL        dokan_ready;
	n_bmp       dokan_bmp_top;
	n_bmp       dokan_bmp_body;
	n_type_gfx  dokan_disappear_offset;
	int         dokan_stage_number_shuffle;
	n_bmp       dokan_label[ N_NN2_DOKAN_LABEL_MAX ];
	n_type_gfx  dokan_label_x;
	n_type_gfx  dokan_label_y;

	n_sprite    kirakira[ N_OBJECT_KIRAKIRA_MAX ];
	n_bmp       kirakira_bmp_n;
	n_bmp       kirakira_bmp_1;
	n_bmp       kirakira_bmp_2;

	u32         weather_gradient_color;
	u32         weather_gradient_upper;
	u32         weather_gradient_lower;
	BOOL        weather_kirakira_onoff;
	BOOL        weather_darkmode_onoff;
	int         weather_hour;
	BOOL        weather_transition;
	BOOL        weather_transition_ready;
	n_bmp       weather_bmp_lite;
	n_bmp       weather_bmp_dark;
	n_bmp      *weather_bmp;
	n_sprite   *weather_sprite;

	n_bmp       bmp_rainbow;

	n_sprite   *bounce_sprite_l;
	n_sprite   *bounce_sprite_m;
	n_sprite   *bounce_sprite_r;
	BOOL        bounce_landed;

	n_bmp       bmp_spotlight;
	n_bmp       bmp_spotlight_canvas;

	n_particle  bubble[ N_NN2_BUBBLE_MAX ];
	u32         bubble_timer;

	int         bell_count, bell_count_prv;
	n_bmp       bell_bmp_0;
	n_bmp       bell_bmp_1;
	n_bmp       bell_bmp_2;
	n_bmp       bell_bmp_info;
	BOOL        bell_onoff;
	BOOL        bell_reset_onoff;

	BOOL        bell_condition_onoff;
	n_bmp       bell_condition_bmp;
	n_type_gfx  bell_condition_dokan_x;
	n_type_gfx  bell_condition_dokan_y;

	n_bmp       treasure_bmp  [ N_NN2_TREASURE_BMP_MAX + 1 ];
	n_bmp       treasure_heart[ 5 ];
	n_bmp       treasure_key;
	int         costume_transition;

	n_bmp       pass_thru_bmp;

	int         title_phase;
	n_bmp       title_neko;

	CGFloat     fuwafuwa;

	n_splash    splash_dash;
	n_splash    splash_slip;
	n_splash    splash_hipdrop;
	n_splash    splash_cloud;
	n_splash    splash_water;
	n_bmp       splash_water_kirakira;

	n_type_gfx  debug_box_x, debug_box_y;

} n_nn2;

#define n_nn2_zero( p ) n_memory_zero( p, sizeof( n_nn2 ) )




int
n_nn2_stage_number_get( n_nn2_stage *s )
{

	int ret = 0;

	if ( s == &n_nn2_stage_0 )
	{
		ret = 0;
	} else
	if ( s == &n_nn2_stage_1 )
	{
		ret = 1;
	} else
	if ( s == &n_nn2_stage_2 )
	{
		ret = 2;
	} else
	if ( s == &n_nn2_stage_3 )
	{
		ret = 3;
	} else
	if ( s == &n_nn2_stage_4 )
	{
		ret = 4;
	} else
	if ( s == &n_nn2_stage_5 )
	{
		ret = 5;
	} else
	if ( s == &n_nn2_stage_6 )
	{
		ret = 6;
	} else
	if ( s == &n_nn2_stage_7 )
	{
		ret = 7;
	} else
	if ( s == &n_nn2_stage_8 )
	{
		ret = 8;
	}


	return ret;
}

void
n_nn2_stage_transfer( n_nn2 *p, n_sprite *s, n_nn2_stage *st )
{
//return;

	if ( ( p->object_handheld_onoff )&&( p->object_handheld_sprite == s ) )
	{
		s->throw_onoff  = FALSE;
		s->invisible    = FALSE;
		s->stage_number = n_nn2_stage_number_get( st );

//NSLog( @"handheld %d", s->stage_number );
		return;
	}
/*
if ( s == &p->share->kuina_sprite )
{
NSLog( @"Number %d %d", s->stage_number, n_nn2_stage_number_get( st ) );
}
*/
	if ( s->stage_number == n_nn2_stage_number_get( st ) )
	{
//NSLog( @"visible" );
		s->invisible = FALSE;

#ifdef N_NN2_KUINA_AUTO
		if ( s == &p->share->kuina_sprite )
		{
			if ( p->weather_darkmode_onoff == FALSE )
			{
				s->invisible = TRUE;
			}
		}
#endif
	} else {
//NSLog( @"invisible" );
		s->invisible = TRUE;
	}


	return;
}




#define N_NN2_OBJECT_COLLISION_NONE ( 0 )

#define N_NN2_OBJECT_COLLISION_OBJECT_NINA_CRASH ( 1 )
#define N_NN2_OBJECT_COLLISION_OBJECT_NINA_CHASE ( 2 )
#define N_NN2_OBJECT_COLLISION_NINA_OBJECT_CRASH ( 3 )
#define N_NN2_OBJECT_COLLISION_NINA_OBJECT_CHASE ( 4 )

#define N_NN2_OBJECT_COLLISION_OBJECT_NINA_STILL ( 5 )
#define N_NN2_OBJECT_COLLISION_NINA_OBJECT_STILL ( 6 )


#define N_NN2_OBJECT_COLLISION_BA_CRASH ( 1 )
#define N_NN2_OBJECT_COLLISION_BA_CHASE ( 2 )
#define N_NN2_OBJECT_COLLISION_AB_CRASH ( 3 )
#define N_NN2_OBJECT_COLLISION_AB_CHASE ( 4 )

#define N_NN2_OBJECT_COLLISION_OVERLAP  ( 7 )


void
n_object_collision_debug( n_posix_char *label, n_sprite *p )
{

	n_posix_char *ud = "";
	n_posix_char *lr = "";

	if ( p->collision_ud_detail == N_NN2_OBJECT_COLLISION_BA_CRASH )
	{
		ud = "BA_CRASH";
	} else
	if ( p->collision_ud_detail == N_NN2_OBJECT_COLLISION_BA_CHASE )
	{
		ud = "BA_CHASE";
	} else
	if ( p->collision_ud_detail == N_NN2_OBJECT_COLLISION_AB_CRASH )
	{
		ud = "AB_CRASH";
	} else
	if ( p->collision_ud_detail == N_NN2_OBJECT_COLLISION_AB_CHASE )
	{
		ud = "AB_CHASE";
	} else
	if ( p->collision_ud_detail == N_NN2_OBJECT_COLLISION_OVERLAP )
	{
		ud = "OVERLAP";
	} else
	//
	{
		ud = "NONE";
	}


	if ( p->collision_lr_detail == N_NN2_OBJECT_COLLISION_BA_CRASH )
	{
		lr = "BA_CRASH";
	} else
	if ( p->collision_lr_detail == N_NN2_OBJECT_COLLISION_BA_CHASE )
	{
		lr = "BA_CHASE";
	} else
	if ( p->collision_lr_detail == N_NN2_OBJECT_COLLISION_AB_CRASH )
	{
		lr = "AB_CRASH";
	} else
	if ( p->collision_lr_detail == N_NN2_OBJECT_COLLISION_AB_CHASE )
	{
		lr = "AB_CHASE";
	} else
	if ( p->collision_lr_detail == N_NN2_OBJECT_COLLISION_OVERLAP )
	{
		lr = "OVERLAP";
	} else
	//
	{
		lr = "NONE";;
	}


	NSLog( @"%s : UD %s : LR %s", label, ud, lr );


	return;
}




void
n_nn2_bmp_flush_mixer_multithread( n_nn2 *p, n_bmp *bmp, u32 color_mix, n_type_real blend )
{
//return;

	if ( n_bmp_error( bmp ) ) { return; }


	n_type_gfx count = N_BMP_SX( bmp ) * N_BMP_SY( bmp );
	int        cores = p->cores;
	n_type_gfx slice = count / cores;


	n_posix_bool prv = n_bmp_is_multithread;
	n_bmp_is_multithread = n_posix_true;


	int offset = 0;
	n_posix_loop
	{

		NSOperation *o = [NSBlockOperation blockOperationWithBlock:^{

		n_type_gfx i = 0;
		n_posix_loop
		{

			u32 color_f = N_BMP_PTR( bmp )[ ( offset * slice ) + i ];
			if ( 0 != n_bmp_a( color_f ) )
			{
				u32 color_t = n_bmp_blend_pixel( color_f, color_mix, blend );
				if ( color_f != color_t )
				{
					N_BMP_PTR( bmp )[ ( offset * slice ) + i ] = color_t;
				}
			}

			i++;
			if ( i >= slice ) { break; }
		}

		}];
		[p->queue addOperation:o];

		offset++;
		if ( offset >= cores ) { break; }
	}


	[p->queue waitUntilAllOperationsAreFinished];


	n_bmp_is_multithread = prv;


	return;
}




#define N_CAMERAWORK_NONE ( 0 )
#define N_CAMERAWORK_STOP ( 1 )
#define N_CAMERAWORK_WARP ( 2 )
#define N_CAMERAWORK_LOOP ( 3 )

#define n_nn2_bmp_draw(        p, bmp, x, y ) n_nn2_bmp_draw_internal( p, bmp, x, y, YES )
#define n_nn2_bmp_draw_simple( p, bmp, x, y ) n_nn2_bmp_draw_internal( p, bmp, x, y,  NO )

void
n_nn2_bmp_draw_internal( n_nn2 *p, n_bmp *bmp, n_type_gfx x, n_type_gfx y, BOOL dark_onoff )
{

	if ( dark_onoff )
	{
		if ( p->stage == &n_nn2_stage_7 )
		{
			//
		} else
		if ( p->weather_darkmode_onoff )
		{
			CGFloat blend = 0.25;
			if ( p->stage == &n_nn2_stage_3 ) { blend = 0.125; }

			n_nn2_bmp_flush_mixer_multithread( p, bmp, n_bmp_black, blend );
		}
	}

	//n_bmp_transcopy( bmp, p->canvas, 0, 0, N_BMP_SX( bmp ), N_BMP_SY( bmp ), x, y );

	extern void n_chara_bmp_copy( n_nn2 *p, n_bmp *f, n_bmp *t, n_type_gfx ox, n_type_gfx oy, int direction );
	n_chara_bmp_copy( p, bmp, p->canvas, x, y, 0 );

	if ( p->stage->camera_mode == N_CAMERAWORK_LOOP )
	{
		n_chara_bmp_copy( p, bmp, p->canvas, x + p->stage->map_sx, y, 0 );
		n_chara_bmp_copy( p, bmp, p->canvas, x - p->stage->map_sx, y, 0 );
	}


	return;
}




n_bmp*
n_nn2_shared_bmp_internal( n_sprite *s, u32 color_bg )
{

	if ( NULL == N_BMP_PTR( &s->bmp_shared ) )
	{
		n_nn2_bmp_new( &s->bmp_shared, s->sx, s->sy );
	} else {
		n_bmp_flush( &s->bmp_shared, color_bg );
	}


	return &s->bmp_shared;
}

n_bmp*
n_nn2_shared_bmp( n_sprite *s )
{
	return n_nn2_shared_bmp_internal( s, n_bmp_black_invisible );
}




extern BOOL n_object_birds_is_caught( n_nn2 *p );




#include "./_sound.c"

#include "./_weather.c"

#include "./_debug.c"
#include "./_camerawork.c"

#include "./_brick_move.c"

#include "./_splash.c"

#include "./chip.c"


static n_sprite n_chara_sprite_idle;
static n_sprite n_chara_sprite_idle_cliff;
static n_sprite n_chara_sprite_walk;
static n_sprite n_chara_sprite_dash;
static n_sprite n_chara_sprite_slip;
static n_sprite n_chara_sprite_duck;
static n_sprite n_chara_sprite_unduck;
static n_sprite n_chara_sprite_jump;
static n_sprite n_chara_sprite_land;
static n_sprite n_chara_sprite_land_swim;
static n_sprite n_chara_sprite_suck;
static n_sprite n_chara_sprite_turn;

#include "./chara_collision.c"

#include "./chara.c"
#include "./chara_object.c"


#include "./wave.c"

#include "./object.c"
#include "./object_bell.c"
#include "./object_bgcloud.c"
#include "./object_birds.c"
#include "./object_chick.c"
#include "./object_lift.c"
#include "./object_rocks.c"
#include "./object_handheld.c"
#include "./object_kirakira.c"
#include "./object_kuina.c"


#include "./interobject.c"

#include "./jam_resolver.c"


#include "./maker.c"


#include "./object_treasure.c"




void
n_nn2_debug_grid_reset( n_nn2 *p, n_nn2_stage *s )
{

#ifdef DEBUG

	return;

	n_bmp_new_fast( &s->canvas_debug, p->mapchip_unit, p->mapchip_unit );
	n_bmp_flush( &s->canvas_debug, n_bmp_argb_mac( 255, 128,128,128 ) );

	n_bmp_box( &s->canvas_debug, 1,1,p->mapchip_unit-2,p->mapchip_unit-2, n_bmp_white_invisible );
	n_bmp_resizer( &s->canvas_debug, s->map_sx, s->map_sy, 0, N_BMP_RESIZER_TILE );

#endif // #ifdef DEBUG


	return;
}

void
n_nn2_nina_starting_position( n_nn2 *p, n_nn2_stage *stg )
{

	if ( p->wave_onoff )
	{
		stg->nina_x = p->stage->center_x;
		stg->nina_y = N_NN2_CAMERA_OCEAN - p->nina_sy;
	} else {
		stg->nina_x = p->stage->center_x;
		stg->nina_y = stg->map_max_sy - p->nina_sy;
	}

	static BOOL is_first = TRUE;
	if ( is_first )
	{
		is_first = FALSE;

		if ( stg == N_NN2_STAGE_START )
		{
			n_object_map_queue( p, &stg->map_queue, stg, N_OBJECT_MAP_QUEUE_NINA );
		}
	}


	return;
}

void
n_nn2_npc_starting_position( n_nn2 *ptr, n_nn2_stage *stg, n_sprite *spr )
{
//return;

	if ( ptr == NULL ) { return; }
	if ( stg == NULL ) { return; }
	if ( spr == NULL ) { return; }


//NSLog( @"%d %d", s->x, s->y );

	// [!] : chick and kuina use this. see n_nn2_stage_throw_go()

	spr->x = spr->starting_x;
	spr->y = N_OBJECT_BIRDS_Y - 150;
//NSLog( @"%d %d", spr->x, spr->y );
//NSLog( @"%d %d", stg->map_sy, stg->map_max_sy );

	if ( spr == &ptr->share->bird1_sprite ) { spr->y += spr->sy * 1; } else
	if ( spr == &ptr->share->bird2_sprite ) { spr->y += spr->sy * 2; } else
	if ( spr == &ptr->share->bird3_sprite ) { spr->y += spr->sy * 3; }


	return;
}

void
n_nn2_stage_init_center( n_nn2_stage *stg )
{

	n_bmp_new_fast( &stg->map_pass_thru, N_BMP_SX( &stg->map ), N_BMP_SY( &stg->map ) );
	n_bmp_flush( &stg->map_pass_thru, 0 );

	return;
}

void
n_nn2_map_metrics_reset_mapping( n_nn2 *ptr, n_nn2_stage *stg, n_sprite *spr )
{

	n_type_real a = (n_type_real) spr->x / ptr->map_sx_prv;
	n_type_real b = (n_type_real) stg->map_sx * a;

//NSLog( @"%d %d : %f %f", ptr->map_sx_prv, stg->map_sx, a, b );

	spr->x = (n_type_gfx) b;


	return;
}

void
n_nn2_map_metrics_reset( n_nn2 *p, n_nn2_stage *stg, BOOL is_init )
{
//NSLog( @"n_nn2_map_metrics_reset()" );

	// [!] : "s" is not "p->stage"


	stg->map_sx = N_BMP_SX( &stg->map ) * p->mapchip_unit;
	stg->map_sy = N_BMP_SY( &stg->map ) * p->mapchip_unit;

	stg->map_max_sx = stg->map_sx - p->sx;
	stg->map_max_sy = stg->map_sy - p->sy;


	if ( is_init == NO )
	{
		if ( p->map_sx_prv == 0 )
		{
			p->map_sx_prv = stg->map_sx;
		}
//NSLog( @"is_init == NO : %d %d", p->map_sx_prv, s->map_sx );

		n_nn2_map_metrics_reset_mapping( p, stg, &p->share->bird1_sprite );
		n_nn2_map_metrics_reset_mapping( p, stg, &p->share->bird2_sprite );
		n_nn2_map_metrics_reset_mapping( p, stg, &p->share->bird3_sprite );

		p->map_sx_prv = stg->map_sx;
	}


	n_object_kirakira_reset( p, stg );


	if ( stg == &n_nn2_stage_6 )
	{
		p->wave_onoff = TRUE;
		//p->swim_onoff = TRUE; // [!] : n_chara_collision_swim() does
		n_sprite_interval_change( &n_chara_sprite_jump, N_NN2_SWIM_INTERVAL );
	} else {
		p->wave_onoff = FALSE;
		p->swim_onoff = FALSE;
		n_sprite_interval_change( &n_chara_sprite_jump, N_NN2_JUMP_INTERVAL );
	}

	stg->camera_reset = TRUE;


	p->chick_stage_number_prv = -1;


	if ( is_init )
	{
		n_nn2_bgcloud_init( p, stg );
	}


#ifdef N_NN2_DOKAN_LABEL_SHOW

	n_bmp_flush( &stg->map_label, n_bmp_white );

#endif


	p->jump_state_prv = -1;


	return;
}




#include "./rc/0/stage_0.c"
#include "./rc/1/stage_1.c"
#include "./rc/2/stage_2.c"
#include "./rc/3/stage_3.c"
#include "./rc/4/stage_4.c"
#include "./rc/5/stage_5.c"
#include "./rc/6/stage_6.c"
#include "./rc/7/stage_7.c"
#include "./rc/8/stage_8.c"

#include "./stage.c"




void
n_nn2_init( n_nn2 *p )
{

	// Global

	n_random_shuffle();

	n_bmp_safemode = n_posix_false;

	n_bmp_transparent_onoff_default = n_posix_false;

	n_bmp_flip_onoff = n_posix_true;


	n_game_transition_percent_smooth = n_posix_true;


	n_random_shuffle();


	p->queue = [[NSOperationQueue alloc] init];
	p->cores = n_posix_cpu_count();


	n_nn2_weather_init( p, -1 );


	return;
}

void
n_nn2_init_canvas( n_nn2 *p )
{

	p->scaler = 2;
	p->zoom   = 1;

	// [!] : big mode
	//p->scaler = 1;
	//p->zoom   = 2;


	p->sx = 700;
	p->sy = 700;

	n_bmp_new_fast( &p->canvas_main, p->sx, p->sy );

	n_bmp_new_fast( &p->bmp_spotlight_canvas, p->sx, p->sy );


#ifdef N_NN2_TITLE_ON

	p->title_phase = 1;

#else

	n_bmp_flush( &p->canvas_main, n_bmp_black );

	p->bell_onoff = TRUE;

#endif

	p->canvas = &p->canvas_main;


	return;
}

void
n_nn2_init_firstscreen( n_nn2 *p )
{

	n_nn2_stage_transition( p, p->stage, p->stage );

	return;
}

void
n_nn2_init_rc( n_nn2 *p )
{

	n_nn2_sound_rc_init();


	p->share = &p->share_main;


	p->mapchip_unit = N_NN2_CHIP_UNIT;


	p->nina_sx = 256 * p->zoom;
	p->nina_sy = 256 * p->zoom;


	n_nn2_bmp_new( &p->bmp_shared, p->nina_sx, p->nina_sy );
	n_nn2_bmp_new( &p->nina_stub , p->nina_sx, p->nina_sy );


	n_posix_bool prv = n_bmp_is_multithread;
	n_bmp_is_multithread = n_posix_true;

	{
		NSOperation *o = [NSBlockOperation blockOperationWithBlock:^{

		n_nn2_bmp_new( &p->nina_stub_body    , p->nina_sx, p->nina_sy );
		n_nn2_bmp_new( &p->nina_stub_boob    , p->nina_sx, p->nina_sy );
		n_nn2_bmp_new( &p->nina_stub_hip     , p->nina_sx, p->nina_sy );
		n_nn2_bmp_new( &p->nina_stub_turn    , p->nina_sx, p->nina_sy );
		n_nn2_bmp_new( &p->nina_stub_sleeve_l, p->nina_sx, p->nina_sy );
		n_nn2_bmp_new( &p->nina_stub_sleeve_m, p->nina_sx, p->nina_sy );
		n_nn2_bmp_new( &p->nina_stub_sleeve_r, p->nina_sx, p->nina_sy );

		}];
		[p->queue addOperation:o];
	}

	{
		NSOperation *o = [NSBlockOperation blockOperationWithBlock:^{

		n_nn2_rc_load( @"rc/nina/10", &p->nina_walk_hair_r, p->scaler );
		n_nn2_rc_load( @"rc/nina/11", &p->nina_walk_hair_s, p->scaler );
		n_nn2_rc_load( @"rc/nina/24", &p->nina_walk_hair_f, p->scaler );
		n_nn2_rc_load( @"rc/nina/9" , &p->nina_walk_face  , p->scaler );
		n_nn2_rc_load( @"rc/nina/28", &p->nina_walk_ear   , p->scaler );

		n_nn2_rc_load( @"rc/nina/6" , &p->nina_blue_body    , p->scaler );
		n_nn2_rc_load( @"rc/nina/8" , &p->nina_blue_boob    , p->scaler );
		n_nn2_rc_load( @"rc/nina/7" , &p->nina_blue_hip     , p->scaler );
		n_nn2_rc_load( @"rc/nina/51", &p->nina_blue_turn    , p->scaler );
		n_nn2_rc_load( @"rc/nina/52", &p->nina_blue_sleeve_l, p->scaler );
		n_nn2_rc_load( @"rc/nina/53", &p->nina_blue_sleeve_m, p->scaler );
		n_nn2_rc_load( @"rc/nina/54", &p->nina_blue_sleeve_r, p->scaler );

		n_nn2_rc_load( @"rc/nina/61", &p->nina_pink_body    , p->scaler );
		n_nn2_rc_load( @"rc/nina/8" , &p->nina_pink_boob    , p->scaler );
		n_nn2_rc_load( @"rc/nina/62", &p->nina_pink_hip     , p->scaler );
		n_nn2_rc_load( @"rc/nina/63", &p->nina_pink_turn    , p->scaler );
		n_nn2_rc_load( @"rc/nina/64", &p->nina_pink_sleeve_l, p->scaler );
		n_nn2_rc_load( @"rc/nina/65", &p->nina_pink_sleeve_m, p->scaler );
		n_nn2_rc_load( @"rc/nina/66", &p->nina_pink_sleeve_r, p->scaler );

		n_nn2_rc_load( @"rc/nina/55", &p->nina_swim_blue_body, p->scaler );
		n_nn2_rc_load( @"rc/nina/56", &p->nina_swim_blue_boob, p->scaler );
		n_nn2_rc_load( @"rc/nina/57", &p->nina_swim_blue_turn, p->scaler );

		n_nn2_rc_load( @"rc/nina/58", &p->nina_swim_pink_body, p->scaler );
		n_nn2_rc_load( @"rc/nina/59", &p->nina_swim_pink_boob, p->scaler );
		n_nn2_rc_load( @"rc/nina/60", &p->nina_swim_pink_turn, p->scaler );

		// [!] : face maker

		n_bmp_flush_transcopy( &p->nina_walk_ear, &p->nina_walk_face );

		n_bmp_carboncopy( &p->nina_walk_face, &p->nina_walk_wink_1 );
		n_bmp_carboncopy( &p->nina_walk_face, &p->nina_walk_wink_2 );
		n_bmp_carboncopy( &p->nina_walk_face, &p->nina_walk_wink_3 );

		n_bmp wink_1; n_bmp_zero( &wink_1 ); n_nn2_rc_load( @"rc/nina/21", &wink_1, p->scaler );
		n_bmp wink_2; n_bmp_zero( &wink_2 ); n_nn2_rc_load( @"rc/nina/22", &wink_2, p->scaler );
		n_bmp wink_3; n_bmp_zero( &wink_3 ); n_nn2_rc_load( @"rc/nina/23", &wink_3, p->scaler );

		n_bmp_flush_transcopy( &wink_1, &p->nina_walk_wink_1 );
		n_bmp_flush_transcopy( &wink_2, &p->nina_walk_wink_2 );
		n_bmp_flush_transcopy( &wink_3, &p->nina_walk_wink_3 );

		n_bmp_free_fast( &wink_1 );
		n_bmp_free_fast( &wink_2 );
		n_bmp_free_fast( &wink_3 );

		n_bmp_carboncopy( &p->nina_walk_wink_1, &p->nina_walk_head );

		n_nn2_rc_load( @"rc/nina/48", &p->nina_walk_neko, p->scaler );


		// [!] : duck maker

		{

		n_nn2_rc_load( @"rc/nina/38", &p->nina_duck_full_body, p->scaler );

		n_bmp h_r; n_bmp_carboncopy( &p->nina_walk_hair_r, &h_r );
		n_bmp h_s; n_bmp_carboncopy( &p->nina_walk_hair_s, &h_s );
		n_bmp h_f; n_bmp_carboncopy( &p->nina_walk_hair_f, &h_f );
		n_bmp w_1; n_bmp_carboncopy( &p->nina_walk_wink_1, &w_1 );
		n_bmp w_2; n_bmp_carboncopy( &p->nina_walk_wink_2, &w_2 );
		n_bmp w_3; n_bmp_carboncopy( &p->nina_walk_wink_3, &w_3 );
		n_bmp nmm; n_bmp_carboncopy( &p->nina_walk_neko  , &nmm );

		n_nn2_bmp_new( &p->nina_duck_full_hair_rear , p->nina_sx,p->nina_sy );
		n_nn2_bmp_new( &p->nina_duck_full_hair_side , p->nina_sx,p->nina_sy );
		n_nn2_bmp_new( &p->nina_duck_full_hair_front, p->nina_sx,p->nina_sy );
		n_nn2_bmp_new( &p->nina_duck_full_wink_1    , p->nina_sx,p->nina_sy );
		n_nn2_bmp_new( &p->nina_duck_full_wink_2    , p->nina_sx,p->nina_sy );
		n_nn2_bmp_new( &p->nina_duck_full_wink_3    , p->nina_sx,p->nina_sy );
		n_nn2_bmp_new( &p->nina_duck_full_neko      , p->nina_sx,p->nina_sy );

		n_type_gfx ox =  18 / p->scaler;
		n_type_gfx oy = 100 / p->scaler;
		n_bmp_fastcopy( &h_r, &p->nina_duck_full_hair_rear , 0,0,p->nina_sx,p->nina_sy, ox,oy );
		n_bmp_fastcopy( &h_s, &p->nina_duck_full_hair_side , 0,0,p->nina_sx,p->nina_sy, ox,oy );
		n_bmp_fastcopy( &h_f, &p->nina_duck_full_hair_front, 0,0,p->nina_sx,p->nina_sy, ox,oy );
		n_bmp_fastcopy( &w_1, &p->nina_duck_full_wink_1    , 0,0,p->nina_sx,p->nina_sy, ox,oy );
		n_bmp_fastcopy( &w_2, &p->nina_duck_full_wink_2    , 0,0,p->nina_sx,p->nina_sy, ox,oy );
		n_bmp_fastcopy( &w_3, &p->nina_duck_full_wink_3    , 0,0,p->nina_sx,p->nina_sy, ox,oy );
		n_bmp_fastcopy( &nmm, &p->nina_duck_full_neko      , 0,0,p->nina_sx,p->nina_sy, ox,oy );

		n_bmp_carboncopy( &p->nina_duck_full_wink_1, &p->nina_duck_full_head );

		n_bmp_free_fast( &h_r );
		n_bmp_free_fast( &h_s );
		n_bmp_free_fast( &h_f );
		n_bmp_free_fast( &w_1 );
		n_bmp_free_fast( &w_2 );
		n_bmp_free_fast( &w_3 );
		n_bmp_free_fast( &nmm );

		}


		{

		n_nn2_rc_load( @"rc/nina/37", &p->nina_duck_half_body, p->scaler );

		n_bmp h_r; n_bmp_carboncopy( &p->nina_walk_hair_r, &h_r );
		n_bmp h_s; n_bmp_carboncopy( &p->nina_walk_hair_s, &h_s );
		n_bmp h_f; n_bmp_carboncopy( &p->nina_walk_hair_f, &h_f );
		n_bmp w_1; n_bmp_carboncopy( &p->nina_walk_wink_1, &w_1 );
		n_bmp w_2; n_bmp_carboncopy( &p->nina_walk_wink_2, &w_2 );
		n_bmp w_3; n_bmp_carboncopy( &p->nina_walk_wink_3, &w_3 );
		n_bmp nmm; n_bmp_carboncopy( &p->nina_walk_neko  , &nmm );

		n_nn2_bmp_new( &p->nina_duck_half_hair_rear , p->nina_sx,p->nina_sy );
		n_nn2_bmp_new( &p->nina_duck_half_hair_side , p->nina_sx,p->nina_sy );
		n_nn2_bmp_new( &p->nina_duck_half_hair_front, p->nina_sx,p->nina_sy );
		n_nn2_bmp_new( &p->nina_duck_half_wink_1    , p->nina_sx,p->nina_sy );
		n_nn2_bmp_new( &p->nina_duck_half_wink_2    , p->nina_sx,p->nina_sy );
		n_nn2_bmp_new( &p->nina_duck_half_wink_3    , p->nina_sx,p->nina_sy );
		n_nn2_bmp_new( &p->nina_duck_half_neko      , p->nina_sx,p->nina_sy );

		n_type_gfx ox = 0;
		n_type_gfx oy = 25 * p->zoom;
		n_bmp_fastcopy( &h_r, &p->nina_duck_half_hair_rear , 0,0,p->nina_sx,p->nina_sy, ox,oy );
		n_bmp_fastcopy( &h_s, &p->nina_duck_half_hair_side , 0,0,p->nina_sx,p->nina_sy, ox,oy );
		n_bmp_fastcopy( &h_f, &p->nina_duck_half_hair_front, 0,0,p->nina_sx,p->nina_sy, ox,oy );
		n_bmp_fastcopy( &w_1, &p->nina_duck_half_wink_1    , 0,0,p->nina_sx,p->nina_sy, ox,oy );
		n_bmp_fastcopy( &w_2, &p->nina_duck_half_wink_2    , 0,0,p->nina_sx,p->nina_sy, ox,oy );
		n_bmp_fastcopy( &w_3, &p->nina_duck_half_wink_3    , 0,0,p->nina_sx,p->nina_sy, ox,oy );
		n_bmp_fastcopy( &nmm, &p->nina_duck_half_neko      , 0,0,p->nina_sx,p->nina_sy, ox,oy );

		n_bmp_carboncopy( &p->nina_duck_half_wink_1, &p->nina_duck_half_head );

		n_bmp_free_fast( &h_r );
		n_bmp_free_fast( &h_s );
		n_bmp_free_fast( &h_f );
		n_bmp_free_fast( &w_1 );
		n_bmp_free_fast( &w_2 );
		n_bmp_free_fast( &w_3 );
		n_bmp_free_fast( &nmm );

		}

		}];
		[p->queue addOperation:o];
	}

	{
		NSOperation *o = [NSBlockOperation blockOperationWithBlock:^{

		n_nn2_rc_load( @"rc/nina/3" , &p->nina_walk_leg_f_2, p->scaler );
		n_nn2_rc_load( @"rc/nina/4" , &p->nina_walk_leg_fd2, p->scaler );
		n_nn2_rc_load( @"rc/nina/2" , &p->nina_walk_leg_f_3, p->scaler );
		n_nn2_rc_load( @"rc/nina/15", &p->nina_walk_leg_n  , p->scaler );
		n_nn2_rc_load( @"rc/nina/13", &p->nina_walk_leg_r_2, p->scaler );
		n_nn2_rc_load( @"rc/nina/12", &p->nina_walk_leg_r_3, p->scaler );


		n_nn2_rc_load( @"rc/nina/19", &p->nina_walk_arm_r_1, p->scaler );
		n_nn2_rc_load( @"rc/nina/18", &p->nina_walk_arm_r_2, p->scaler );
		n_nn2_rc_load( @"rc/nina/20", &p->nina_walk_arm_n  , p->scaler );
		n_nn2_rc_load( @"rc/nina/17", &p->nina_walk_arm_f_1, p->scaler );
		n_nn2_rc_load( @"rc/nina/16", &p->nina_walk_arm_f_2, p->scaler );


		n_nn2_rc_load( @"rc/nina/31", &p->nina_dash_arm_n  , p->scaler );
		n_nn2_rc_load( @"rc/nina/33", &p->nina_dash_arm_r_1, p->scaler );
		n_nn2_rc_load( @"rc/nina/32", &p->nina_dash_arm_r_2, p->scaler );
		n_nn2_rc_load( @"rc/nina/30", &p->nina_dash_arm_f_1, p->scaler );
		n_nn2_rc_load( @"rc/nina/29", &p->nina_dash_arm_f_2, p->scaler );

		n_nn2_rc_load( @"rc/nina/34", &p->nina_dash_leg_f_1, p->scaler );
		n_nn2_rc_load( @"rc/nina/35", &p->nina_dash_leg_f_2, p->scaler );
		n_nn2_rc_load( @"rc/nina/12", &p->nina_dash_leg_r_1, p->scaler );
		n_nn2_rc_load( @"rc/nina/36", &p->nina_dash_leg_r_2, p->scaler );


		// [!] : shadow maker

		n_nn2_walk_shadow( &p->nina_walk_leg_n  , &p->nina_walk_lgS_n   );

		n_nn2_walk_shadow( &p->nina_walk_leg_f_2, &p->nina_walk_lgS_f_2 );
		n_nn2_walk_shadow( &p->nina_walk_leg_fd2, &p->nina_walk_lgS_fd2 );
		n_nn2_walk_shadow( &p->nina_walk_leg_f_3, &p->nina_walk_lgS_f_3 );

		n_nn2_walk_shadow( &p->nina_walk_leg_r_2, &p->nina_walk_lgS_r_2 );
		n_nn2_walk_shadow( &p->nina_walk_leg_r_3, &p->nina_walk_lgS_r_3 );

		n_nn2_walk_shadow( &p->nina_walk_arm_r_1, &p->nina_walk_amS_r_1 );
		n_nn2_walk_shadow( &p->nina_walk_arm_r_2, &p->nina_walk_amS_r_2 );

		n_nn2_walk_shadow( &p->nina_walk_arm_f_1, &p->nina_walk_amS_f_1 );
		n_nn2_walk_shadow( &p->nina_walk_arm_f_2, &p->nina_walk_amS_f_2 );

		n_nn2_walk_shadow( &p->nina_dash_arm_r_1, &p->nina_dash_amS_r_1 );
		n_nn2_walk_shadow( &p->nina_dash_arm_r_2, &p->nina_dash_amS_r_2 );
		n_nn2_walk_shadow( &p->nina_dash_arm_f_1, &p->nina_dash_amS_f_1 );
		n_nn2_walk_shadow( &p->nina_dash_arm_f_2, &p->nina_dash_amS_f_2 );

		n_nn2_walk_shadow( &p->nina_dash_leg_f_1, &p->nina_dash_lgS_f_1 );
		n_nn2_walk_shadow( &p->nina_dash_leg_f_2, &p->nina_dash_lgS_f_2 );
		n_nn2_walk_shadow( &p->nina_dash_leg_r_1, &p->nina_dash_lgS_r_1 );
		n_nn2_walk_shadow( &p->nina_dash_leg_r_2, &p->nina_dash_lgS_r_2 );


		}];
		[p->queue addOperation:o];
	}


	{
		NSOperation *o = [NSBlockOperation blockOperationWithBlock:^{

		n_nn2_rc_load( @"rc/nina/39", &p->nina_jump_leg_1, p->scaler );
		n_nn2_rc_load( @"rc/nina/40", &p->nina_jump_leg_2, p->scaler );
		n_nn2_rc_load( @"rc/nina/41", &p->nina_jump_leg_3, p->scaler );
		n_nn2_rc_load( @"rc/nina/42", &p->nina_jump_leg_t, p->scaler );
		n_nn2_rc_load( @"rc/nina/43", &p->nina_jump_arm_1, p->scaler );
		n_nn2_rc_load( @"rc/nina/44", &p->nina_jump_arm_2, p->scaler );
		n_nn2_rc_load( @"rc/nina/45", &p->nina_suck_arm_1, p->scaler );
		n_nn2_rc_load( @"rc/nina/46", &p->nina_suck_leg_1, p->scaler );

		n_nn2_walk_shadow( &p->nina_jump_leg_1, &p->nina_jump_lgS_1 );
		n_nn2_walk_shadow( &p->nina_jump_leg_2, &p->nina_jump_lgS_2 );
		n_nn2_walk_shadow( &p->nina_jump_leg_3, &p->nina_jump_lgS_3 );

		}];
		[p->queue addOperation:o];
	}

	{
		NSOperation *o = [NSBlockOperation blockOperationWithBlock:^{

		n_nn2_rc_load( @"rc/nina/49", &p->nina_idle_leg1 , p->scaler );
		n_nn2_rc_load( @"rc/nina/50", &p->nina_idle_leg2 , p->scaler );
		n_nn2_rc_load( @"rc/nina/14", &p->nina_idle_cliff, p->scaler );

		n_nn2_rc_load( @"rc/nina/67", &p->nina_cliff_sweat, p->scaler );

		}];
		[p->queue addOperation:o];
	}


	{
		NSOperation *o = [NSBlockOperation blockOperationWithBlock:^{

		p->nina_walk_step = N_NN2_STEP_INIT;
		n_chara_walk_init( p, 2 );

		n_chara_idle_init( p, 5 );
		n_chara_dash_init( p, 2 );

		n_chara_idle_cliff_init( p, 2 );

		n_chara_suck_init( p, 5 );

		n_chara_duck_init  ( p, 3 );
		n_chara_unduck_init( p, 3 );

		n_chara_jump_init( p, N_NN2_JUMP_INTERVAL );

		n_chara_land_init( p, 3 );
		n_chara_land_swim_init( p, 3 );

		n_chara_slip_init( p, 5 );

		n_chara_turn_init( p, 3 );

		}];
		[p->queue addOperation:o];
	}

	{
		NSOperation *o = [NSBlockOperation blockOperationWithBlock:^{

		n_nn2_rc_load( @"rc/object/chick/2" , &p->chick_body  , p->scaler );
		n_nn2_rc_load( @"rc/object/chick/15", &p->chick_wink  , p->scaler );
		n_nn2_rc_load( @"rc/object/chick/7" , &p->chick_wing_n, p->scaler );
		n_nn2_rc_load( @"rc/object/chick/8" , &p->chick_wing_1, p->scaler );
		n_nn2_rc_load( @"rc/object/chick/9" , &p->chick_wing_2, p->scaler );
		n_nn2_rc_load( @"rc/object/chick/11", &p->chick_foot_n, p->scaler );
		n_nn2_rc_load( @"rc/object/chick/12", &p->chick_foot_1, p->scaler );
		n_nn2_rc_load( @"rc/object/chick/13", &p->chick_foot_2, p->scaler );

		}];
		[p->queue addOperation:o];
	}

	{
		NSOperation *o = [NSBlockOperation blockOperationWithBlock:^{

		n_nn2_rc_load( @"rc/object/kuina/2" , &p->kuina_body  , p->scaler );
		n_nn2_rc_load( @"rc/object/kuina/15", &p->kuina_wink  , p->scaler );
		n_nn2_rc_load( @"rc/object/kuina/7" , &p->kuina_wing_n, p->scaler );
		n_nn2_rc_load( @"rc/object/kuina/8" , &p->kuina_wing_1, p->scaler );
		n_nn2_rc_load( @"rc/object/kuina/9" , &p->kuina_wing_2, p->scaler );
		n_nn2_rc_load( @"rc/object/kuina/11", &p->kuina_foot_n, p->scaler );
		n_nn2_rc_load( @"rc/object/kuina/12", &p->kuina_foot_1, p->scaler );

		}];
		[p->queue addOperation:o];
	}

	{
		NSOperation *o = [NSBlockOperation blockOperationWithBlock:^{

		n_nn2_rc_load( @"rc/object/bird/2" , &p->bird_body  , p->scaler );
		n_nn2_rc_load( @"rc/object/bird/4" , &p->bird_wink_1, p->scaler );
		n_nn2_rc_load( @"rc/object/bird/5" , &p->bird_wink_2, p->scaler );
		n_nn2_rc_load( @"rc/object/bird/7" , &p->bird_wing_n, p->scaler );
		n_nn2_rc_load( @"rc/object/bird/8" , &p->bird_wing_1, p->scaler );
		n_nn2_rc_load( @"rc/object/bird/9" , &p->bird_wing_2, p->scaler );

		}];
		[p->queue addOperation:o];
	}

	{
		NSOperation *o = [NSBlockOperation blockOperationWithBlock:^{

		n_nn2_stage_mountain_maker( p );

		n_nn2_rc_load( @"rc/object/dokan/top" , &p->dokan_bmp_top , p->scaler );
		n_nn2_rc_load( @"rc/object/dokan/body", &p->dokan_bmp_body, p->scaler );

		n_nn2_rc_load( @"rc/object/lift/face", &p->lift_bmp_face , p->scaler );
//n_nn2_debug_bmp_save( p, &p->lift_bmp_face );

		n_object_lift_load_single( p, &p->lift_bmp[ 0 ], 1.000 );
		n_object_lift_load_single( p, &p->lift_bmp[ 1 ], 0.975 );
		n_object_lift_load_single( p, &p->lift_bmp[ 2 ], 0.955 );
		n_object_lift_load_single( p, &p->lift_bmp[ 3 ], 0.975 );

		n_object_kirakira_init( p );

		n_nn2_rc_load( @"rc/object/fish/0", &p->fish_bmp , p->scaler );

		n_object_bell_init( p );

		n_nn2_rc_load( @"rc/indicator/nina", &p->nina_indicator, p->scaler );
		n_bmp_flush_antialias( &p->nina_indicator, 1.0 );
		n_bmp_scaler_lil( &p->nina_indicator, 2 );

		n_nn2_stage_pass_thru_maker( p );

		n_nn2_dokan_label_maker( p );

		}];
		[p->queue addOperation:o];
	}

	{
		NSOperation *o = [NSBlockOperation blockOperationWithBlock:^{

		n_object_treasure_init( p );

		}];
		[p->queue addOperation:o];
	}


	[p->queue waitUntilAllOperationsAreFinished];

	n_bmp_is_multithread = prv;


	// Init

	p->sprite_cur = &n_chara_sprite_idle;

	p->nina_margin_head = n_chara_margin_detect( p, &p->nina_walk_neko     , N_CHARA_MARGIN_DETECT_HEAD );
	p->nina_margin_hair = n_chara_margin_detect( p, &p->nina_walk_neko     , N_CHARA_MARGIN_DETECT_HEAD );
	p->nina_margin_duck = n_chara_margin_detect( p, &p->nina_duck_full_neko, N_CHARA_MARGIN_DETECT_HEAD );
	p->nina_margin_foot = n_chara_margin_detect( p, &p->nina_walk_leg_n    , N_CHARA_MARGIN_DETECT_FOOT );
	p->nina_margin_land = n_chara_margin_detect( p, &p->nina_jump_leg_t    , N_CHARA_MARGIN_DETECT_FOOT );
	p->nina_margin_fwrd = n_chara_margin_detect( p, &p->nina_walk_leg_n    , N_CHARA_MARGIN_DETECT_FWRD );
	p->nina_margin_rear = n_chara_margin_detect( p, &p->nina_walk_leg_n    , N_CHARA_MARGIN_DETECT_REAR );
	p->nina_margin_swim = n_chara_margin_detect( p, &p->nina_jump_leg_3    , N_CHARA_MARGIN_DETECT_REAR );
	//p->nina_margin_fmax = n_chara_margin_detect( p, &p->nina_walk_leg_f_3  , N_CHARA_MARGIN_DETECT_FWRD );

	//p->nina_margin_fmax = ( p->nina_margin_fwrd + p->nina_margin_fmax ) / 2;

//NSLog( @"%d", p->nina_margin_head );
//NSLog( @"%d", p->nina_margin_foot );
//NSLog( @"%d %d", p->nina_margin_rear, p->nina_margin_swim );


	p->jump_vertical_threshold = 128 * p->zoom;
	p->jump_reverse_onoff      = TRUE;


	n_nn2_stage_init( p );

	p->stage = N_NN2_STAGE_START;

	n_nn2_stage_share_init( p );

	{
		n_nn2_stage *s = &n_nn2_stage_0;
		n_object_map_queue( p, &s->map_queue, s, N_OBJECT_MAP_QUEUE_CLOUD_H );
		n_object_map_queue( p, &s->map_queue, s, N_OBJECT_MAP_QUEUE_CLOUD_V );
	}

	{
		n_nn2_stage *s = &n_nn2_stage_4;
		n_object_map_queue( p, &s->map_queue, s, N_OBJECT_MAP_QUEUE_CLD_H_0 );
		n_object_map_queue( p, &s->map_queue, s, N_OBJECT_MAP_QUEUE_CLD_V_0 );
		n_object_map_queue( p, &s->map_queue, s, N_OBJECT_MAP_QUEUE_CLD_V_1 );
		n_object_map_queue( p, &s->map_queue, s, N_OBJECT_MAP_QUEUE_CLD_V_2 );
		n_object_map_queue( p, &s->map_queue, s, N_OBJECT_MAP_QUEUE_CLD_V_3 );
	}

	{
		n_nn2_stage *s = n_nn2_stage_number2ptr( N_NN2_STAGE_NUMBER_CHICK );
		n_object_map_queue( p, &s->map_queue, s, N_OBJECT_MAP_QUEUE_CHICK );
	}

	{
		n_nn2_stage *s = n_nn2_stage_number2ptr( N_NN2_STAGE_NUMBER_KUINA );
		n_object_map_queue( p, &s->map_queue, s, N_OBJECT_MAP_QUEUE_KUINA );
	}

	{
		n_nn2_stage *s = n_nn2_stage_number2ptr( N_NN2_STAGE_NUMBER_ROCKS );
		n_object_map_queue( p, &s->map_queue, s, N_OBJECT_MAP_QUEUE_ROCKS );
//p->share->rocks_sprite.x = s->map_sx - 64;
	}

	{
		n_nn2_stage *s = N_NN2_STAGE_START;
		n_nn2_npc_starting_position( p, s, &p->share->bird1_sprite );
		n_nn2_npc_starting_position( p, s, &p->share->bird2_sprite );
		n_nn2_npc_starting_position( p, s, &p->share->bird3_sprite );
	}


#ifdef N_NN2_DEBUG_WEATHER_BUTTON

	p->weather_sprite->invisible = FALSE;

#endif


	n_nn2_powerup_init_blue( &p->powerup_blue );
	n_nn2_powerup_init_pink( &p->powerup_pink );

	p->powerup = &p->powerup_blue;


	return;
}

void
n_nn2_draw( n_nn2 *p )
{

	// [!] : fluffy on clouds

	if (
		( p->stage == &n_nn2_stage_0 )
		||
		( p->stage == &n_nn2_stage_4 )
	)
	{
		extern BOOL n_nn2_stage_stand_onoff_cloud( n_nn2 *p );

		if ( n_nn2_stage_stand_onoff_cloud( p ) )
		{
			static u32 timer = 0;
			if ( n_game_timer( &timer, 12 ) )
			{
				static CGFloat value = 0;

				p->fuwafuwa = n_nn2_flowy_effect( 8, value );

				value += 0.015;
			}
		} else {
			p->fuwafuwa = 0;
		}
	} else {
		p->fuwafuwa = 0;
	}


	// [!] : main

	n_chip_draw( p );

//return;
//p->draw_center = 0;


	if ( p->stage == &n_nn2_stage_4 )
	{
		n_object_lift_action_turn_guideline( p, &p->stage->cld_h_1_sprite );
	} else
	if ( p->stage == &n_nn2_stage_7 )
	{
		n_nn2_stage_7_timeup_ui( p );
	}


#ifdef N_NN2_DRAW_DOKAN

	n_chip_draw_dokan( p );

#else

	p->global_blend = 0.25;

	n_chip_draw_dokan( p );

	p->global_blend = 0.0;

#endif


	n_object_treasure_draw_key( p );


	// [!] : before rocks_sprite
	if ( p->draw_center == N_NN2_DRAW_DASH )
	{
		n_chara_dash_draw( p, YES );
	}

	n_object_rocks_draw( p, &p->share->rocks_sprite );

	n_object_share_draw( p, &p->share->debug_move );
	n_object_share_draw( p, &p->share->debug_jump );


	n_object_birds_draw( p, &p->share->bird1_sprite );
	n_object_birds_draw( p, &p->share->bird2_sprite );
	n_object_birds_draw( p, &p->share->bird3_sprite );
	n_object_chick_draw( p, &p->share->chick_sprite );
	n_object_kuina_draw( p, &p->share->kuina_sprite );


	if ( p->stage == &n_nn2_stage_3 )
	{
		n_type_gfx x = 0;
		n_type_gfx y = 0;

		n_camerawork_offset_get( p, &x, &y );
//NSLog( @"%d %d", x, y );

		n_type_gfx size = N_NN2_SPOTLIGHT_SIZE;

		x += p->nina_sx / 2; x -= ( size / 2 );
		y += p->nina_sy / 2; y -= ( size / 2 );
//NSLog( @"%d %d", x, y );

		//n_bmp_transcopy( &p->bmp_spotlight, p->canvas, 0,0,size,size, x,y );

		n_bmp_flush( &p->bmp_spotlight_canvas, n_bmp_argb_mac( 64,0,0,0 ) );

		if ( p->stage->camera_mode == N_CAMERAWORK_LOOP )
		{
			n_type_gfx lx = x - p->stage->map_sx;
			n_type_gfx rx = x + p->stage->map_sx;
			n_type_gfx mx = x;

			n_bmp_circle( &p->bmp_spotlight_canvas, lx,y,size,size, n_bmp_argb_mac( 16,255,255,255 ) );
			n_bmp_circle( &p->bmp_spotlight_canvas, rx,y,size,size, n_bmp_argb_mac( 16,255,255,255 ) );
			n_bmp_circle( &p->bmp_spotlight_canvas, mx,y,size,size, n_bmp_argb_mac( 16,255,255,255 ) );
		} else {
			n_bmp_circle( &p->bmp_spotlight_canvas, x,y,size,size, n_bmp_argb_mac( 16,255,255,255 ) );
		}

		n_bmp_flush_transcopy( &p->bmp_spotlight_canvas, p->canvas );
	} else
	if ( p->stage == &n_nn2_stage_6 )
	{
		n_object_share_draw( p, &p->share->fish1_sprite );
		n_object_share_draw( p, &p->share->fish2_sprite );
		n_object_share_draw( p, &p->share->fish3_sprite );
	}


	if ( p->draw_center == N_NN2_DRAW_IDLE )
	{
		n_chara_idle_draw( p );
	} else
	if ( p->draw_center == N_NN2_DRAW_WALK )
	{
		n_chara_walk_draw( p );
	} else
	if ( p->draw_center == N_NN2_DRAW_DASH )
	{
		n_chara_dash_draw( p, NO );
		n_splash_init( p, &p->splash_dash, p->stage->nina_x, p->stage->nina_y, 333 );
	} else
	if ( p->draw_center == N_NN2_DRAW_JUMP )
	{
		n_chara_jump_draw( p );
	} else
	if ( p->draw_center == N_NN2_DRAW_SUCK )
	{
		n_chara_suck_draw( p );
	} else
	if ( p->draw_center == N_NN2_DRAW_SLIP )
	{
		n_chara_slip_draw( p );
		n_splash_init( p, &p->splash_slip, p->stage->nina_x, p->stage->nina_y, 333 );
	} else
	if ( p->draw_center == N_NN2_DRAW_DUCK )
	{
		n_chara_duck_draw( p );
	} else
	if ( p->draw_center == N_NN2_DRAW_LAND )
	{
		if ( p->swim_onoff )
		{
			n_chara_land_swim_draw( p );
		} else {
			n_chara_land_draw( p );
		}
	} else
	if ( p->draw_center == N_NN2_DRAW_TURN )
	{
		n_chara_turn_draw( p );
	} //else


//NSLog( @"%d", p->transition_phase );


	n_splash_dash_draw( p, &p->splash_dash );
	n_splash_slip_draw( p, &p->splash_slip );

	n_splash_cloud_draw( p, &p->splash_hipdrop );
	n_splash_cloud_draw( p, &p->splash_cloud   );

	n_splash_water_draw( p, &p->splash_water );


	n_nn2_bgcloud_draw( p );


	n_object_birds_draw_overlay( p, &p->share->bird1_sprite );
	n_object_birds_draw_overlay( p, &p->share->bird2_sprite );
	n_object_birds_draw_overlay( p, &p->share->bird3_sprite );
	n_object_chick_draw_overlay( p, &p->share->chick_sprite );
	n_object_kuina_draw_overlay( p, &p->share->kuina_sprite );

	if ( p->stage == &n_nn2_stage_0 )
	{
		n_object_lift_draw( p, &p->stage->cld_h_0_sprite );
		n_object_lift_draw( p, &p->stage->cld_v_0_sprite );
	} else
	if ( p->stage == &n_nn2_stage_4 )
	{
		n_object_lift_draw( p, &p->stage->cld_h_0_sprite );
		n_object_lift_draw( p, &p->stage->cld_h_1_sprite );
		n_object_lift_draw( p, &p->stage->cld_v_0_sprite );
		n_object_lift_draw( p, &p->stage->cld_v_1_sprite );
		//n_object_lift_draw( p, &p->stage->cld_v_2_sprite );
		//n_object_lift_draw( p, &p->stage->cld_v_3_sprite );
	}


	n_nn2_wave_draw( p );

	n_nn2_wave_bubble_draw( p );


	n_object_bell_draw( p );

	n_object_bell_condition_draw( p );


	n_chip_pass_thru_fade_out( p );


	n_chip_draw_treasure( p );


#ifdef DEBUG

	//n_chip_debug_draw( p );

	//n_nn2_debug_rect( p );

	{
		n_sprite *s = p->debug_sprite;
		if ( s != NULL )
		{
			n_type_gfx ox = s->x - n_camerawork_x( p );
			n_type_gfx oy = s->y - n_camerawork_y( p );
//NSLog( @"%d %d %d %d", s->x, s->y, s->sx, s->sy );
//NSLog( @"%d %d", ox, oy );

			n_bmp_box( p->canvas, ox, oy, s->sx, s->sy, n_bmp_rgb_mac( 255,0,0 ) );
		}
	}

	//n_object_treasure_heart_collision( p, 0, 0 );

#endif // #ifdef DEBUG


	if ( p->stage->camera_oy != 0 )
	{
		n_bmp_box( p->canvas, 0,                        0,p->sx,p->stage->camera_oy, n_bmp_black );
		n_bmp_box( p->canvas, 0,p->sy-p->stage->camera_oy,p->sx,p->stage->camera_oy, n_bmp_black );
	}


	return;
}

void
n_nn2_loop_minimal( n_nn2 *p )
{

	p->sprite_cur = &n_chara_sprite_idle;
	n_sprite_rewind( p->sprite_cur );

	n_chara_collision_gravity( p );

	p->draw_center = N_NN2_DRAW_IDLE;


	n_chip_draw_dokan( p );


	n_camerawork_go( p );


	return;
}

void
n_nn2_loop( n_nn2 *p )
{

	n_object_action_move ( p, &p->share->debug_move   );
	n_object_action_jump ( p, &p->share->debug_jump   );
	n_object_birds_action( p, &p->share->bird1_sprite );
	n_object_birds_action( p, &p->share->bird2_sprite );
	n_object_birds_action( p, &p->share->bird3_sprite );
	n_object_action_jump ( p, &p->share->chick_sprite );
	n_object_kuina_action( p, &p->share->kuina_sprite );
	n_object_rocks_action( p, &p->share->rocks_sprite );

	if ( p->stage == &n_nn2_stage_0 )
	{
		n_object_lift_action_horz( p, &p->stage->cld_h_0_sprite );
		n_object_lift_action_vert( p, &p->stage->cld_v_0_sprite );
	} else
	if ( p->stage == &n_nn2_stage_4 )
	{
		n_object_lift_action_horz( p, &p->stage->cld_h_0_sprite );
		n_object_lift_action_turn( p, &p->stage->cld_h_1_sprite );
		n_object_lift_action_vert( p, &p->stage->cld_v_0_sprite );
		n_object_lift_action_vert( p, &p->stage->cld_v_1_sprite );
		//n_object_lift_action_vert( p, &p->stage->cld_v_2_sprite );
		//n_object_lift_action_vert( p, &p->stage->cld_v_3_sprite );
	} else
	if ( p->stage == &n_nn2_stage_6 )
	{
		n_object_fish_move( p, &p->share->fish1_sprite );
		n_object_fish_move( p, &p->share->fish2_sprite );
		n_object_fish_move( p, &p->share->fish3_sprite );
	}

	// [Patch]
	if ( n_nn2_stage_stand_onoff_rock( p ) )
	{
		// [x] : this causes surge
		//p->stage->nina_y = p->share->rocks_sprite.y - p->nina_sy + p->nina_margin_foot - 1;
		if ( p->jump_onoff )
		{
			n_nn2_stage_stand_all_off( p );
		}
	}

	n_interobject_collision_lift_clamp( p, &p->share->rocks_sprite );


	static BOOL freefall = FALSE;
	if ( p->jump_state_prv != p->jump_state )
	{
//NSLog( @"%d %d", p->jump_state_prv, p->jump_state );

		if ( p->jump_state_prv == -1 ) { p->jump_state_prv = 1; }

		if ( ( p->jump_state_prv == 0 )&&( p->jump_state == 2 ) )
		{
			freefall = FALSE;
		} else {
			freefall = TRUE;
		}
//NSLog( @"%d", freefall );

		if ( freefall == FALSE )
		{

			n_type_gfx tx = p->stage->nina_x + ( p->nina_sx / 2 );
			n_type_gfx ty = p->stage->nina_y + p->nina_sy - p->nina_margin_foot + 1;

			tx /= p->mapchip_unit;
			ty /= p->mapchip_unit;
//p->debug_chip_x = tx;
//p->debug_chip_y = ty;
			int ret = n_chara_collision_fall_condition( p, tx, ty, 4 );
//NSLog( @"%d", ret );
			if ( ret >= 3 ) { freefall = TRUE; }
		}

		p->jump_state_prv = p->jump_state;
	}


	if ( p->costume_transition )
	{
		//
	} else
	if ( n_chara_collision_cliff_loop( p ) )
	{

		static u32 timer = 0;

		p->sprite_cur  = &n_chara_sprite_idle_cliff;
		p->draw_center = N_NN2_DRAW_IDLE;

		if ( p->hipdrop_cliff_lock == FALSE ) { timer = n_posix_tickcount() + 333; }

		p->hipdrop_cliff_lock      = TRUE;
		p->hipdrop_cliff_direction = p->direction;

		p->duck_onoff = FALSE;
		p->turn_onoff = FALSE;

		if ( timer < n_posix_tickcount() )
		{
			if ( p->direction == N_NN2_DIRECTION_LEFT )
			{
				if ( p->input & N_NN2_INPUT_L ) { goto n_goto_input; }
			} else
			if ( p->direction == N_NN2_DIRECTION_RIGHT )
			{
				if ( p->input & N_NN2_INPUT_R ) { goto n_goto_input; }
			}
		}

	} else
	if ( p->dokan_onoff )
	{
//NSLog( @"dokan_onoff" );
//p->debug_pause = TRUE;

		n_chara_action_idle( p, YES );


		n_type_gfx x = p->stage->nina_x + ( p->nina_sx / 2 );
		n_type_gfx y = p->stage->nina_y + p->nina_margin_hair;

		x /= p->mapchip_unit;
		y /= p->mapchip_unit;

		u32 data = 0; n_bmp_ptr_get( &p->stage->map_dokan, x, y, &data );
//NSLog( @"%d", n_chip_data_kind( data ) );

		if ( N_CHIP_DOKAN_WARP == n_chip_data_kind( data ) )
		{
			if ( p->transition_lock == FALSE )
			{
				p->transition_lock = TRUE;

				p->dokan_label_x = x;
				p->dokan_label_y = y;

				u32 data = 0; n_bmp_ptr_get( &p->stage->map_dokan, x, y, &data );

				int stage_number = n_nn2_map_dokan_data_kind( data );
//NSLog( @"Stage Number : %d", stage_number );

				n_nn2_stage_transition_go( p, stage_number );

				p->jump_state = N_NN2_JUMP_STATE_FALL;
				p->sprite_cur = &n_chara_sprite_jump;
			}
		}

	} else
	if ( p->suck_phase == 1 )
	{
//NSLog( @"%d", p->input );

		if ( p->suck_direction != p->direction )
		{
			p->suck_phase = 2;

			goto n_goto_fall;
		} else
		if ( p->input == 0 )
		{
			p->suck_phase = 2;

			goto n_goto_fall;
		} else
		if ( p->jump_onoff )
		{
			p->suck_phase = 0;
			p->jump_float = 0;

			goto n_goto_jump;
		}

	} else
	if ( p->land_onoff )
	{

		n_goto_land:


		p->duck_onoff = p->unduck_onoff = FALSE;

		if ( p->land_onoff == FALSE )
		{
			p->land_onoff = TRUE;

			if ( p->swim_onoff )
			{
				p->sprite_cur = &n_chara_sprite_land_swim;
			} else {
				p->sprite_cur = &n_chara_sprite_land;
			}

			if ( n_nn2_stage_stand_onoff_cloud( p ) )
			{
				//
			} else
			if ( p->swim_onoff == FALSE )
			{
				n_nn2_sound_effect_play( N_NN2_SOUND_LAND );
			}

			n_sprite_rewind( p->sprite_cur );
		} else {
			if ( p->sprite_cur->once )
			{
				p->land_onoff = FALSE;

				goto n_goto_idle;
			}
		}

		p->draw_center = N_NN2_DRAW_LAND;

	} else
	if ( p->brake_phase )
	{

		p->turn_onoff = FALSE;

		if ( p->swim_onoff )
		{

			p->brake_phase = N_NN2_DASH_BRAKE_NONE;

		} else
		if ( p->brake_phase == N_NN2_DASH_BRAKE_INIT )
		{

			p->brake_phase = N_NN2_DASH_BRAKE_DONE;
			p->brake_timer = n_posix_tickcount();


			p->sprite_cur = &n_chara_sprite_slip;

			p->direction = p->brake_direction;

			//n_chara_walk_engine( p, p->direction, YES );
			//p->draw_center = N_NN2_DRAW_SLIP;

			goto n_goto_brake_done;

		} else
		if ( p->brake_phase == N_NN2_DASH_BRAKE_DONE )
		{

			n_goto_brake_done:

			if ( n_chara_collision_gravity( p ) )
			{
				//
			} else {
				goto n_goto_fall;
			}

			if ( n_game_timer( &p->brake_timer, N_NN2_DASH_BRAKE_MSEC ) )
			{
				p->brake_phase = N_NN2_DASH_BRAKE_NONE;

				p->dash_phase = N_NN2_DASH_NONE;
				p->dash_value = 0;

				n_chara_action_idle( p, YES );
			} else {
				p->sprite_cur = &n_chara_sprite_slip;

				p->direction = p->brake_direction;

				n_chara_collision_lr( p, p->direction );

				p->draw_center = N_NN2_DRAW_SLIP;

				n_nn2_sound_effect_play( N_NN2_SOUND_BRAKE );
			}

		}

	} else
	if ( p->turn_onoff )
	{

		if ( n_chara_collision_gravity( p ) )
		{
			n_chara_collision_lr( p, p->direction );

			p->sprite_cur = &n_chara_sprite_turn;

			p->draw_center = N_NN2_DRAW_TURN;

			if ( p->sprite_cur->once )
			{
				p->turn_onoff = FALSE;
			}
		}

	} else
	if ( ( p->jump_onoff )&&( p->jump_state == N_NN2_JUMP_STATE_NONE ) )
	{
//static int i = 0; NSLog( @"Jump : %d", i ); i++;


		if ( n_object_treasure_is_running( p ) ) { p->jump_onoff = FALSE; goto n_goto_idle; }


		n_goto_jump:


		p->jump_state = N_NN2_JUMP_STATE_RISE;

		p->jump_vertical_onoff   = TRUE;
		p->jump_vertical_start_x = p->stage->nina_x;


		p->stage->nina_y -= p->nina_margin_foot;


		p->  turn_onoff = FALSE;
		p->  duck_onoff = FALSE;
		p->unduck_onoff = FALSE;


		if ( p->swim_onoff == FALSE )
		{
			n_nn2_sound_effect_play( N_NN2_SOUND_JUMP );
		}


		p->sprite_cur = &n_chara_sprite_jump;

		// [!] : for fast response
		goto n_goto_rise;

	} else
	if ( p->jump_state == N_NN2_JUMP_STATE_RISE )
	{

		n_goto_rise:
//NSLog( @"N_NN2_JUMP_STATE_RISE" );

		{
			CGFloat boost = p->powerup->jump_boost;
			if ( p->fly_phase ) { boost /= 2; }

			if ( n_nn2_flowy_effect_inc( p->nina_sy, &p->jump_float, boost ) )
			{
				p->jump_state = N_NN2_JUMP_STATE_FALL;
			}
		}


		if ( p->jump_onoff == FALSE )
		{
			p->jump_state = N_NN2_JUMP_STATE_FALL;
		}


		if ( p->jump_state == N_NN2_JUMP_STATE_FALL )
		{
			goto n_goto_fall;
		}


		n_chara_action_jump_lr( p );

		n_chara_collision_heading( p );

		p->draw_center = N_NN2_DRAW_JUMP;

	} else
	if ( p->jump_state == N_NN2_JUMP_STATE_FALL )
	{
//NSLog( @"N_NN2_JUMP_STATE_FALL" );

		n_goto_fall:


		if ( p->input == 0 )
		{
			p->jump_vertical_onoff   = TRUE;
			p->jump_vertical_start_x = p->stage->nina_x;
		}


		if ( p->jump_flow )
		{
			//
		} else
		if ( p->swim_onoff )
		{
			//
		} else
		if ( p->input & N_NN2_INPUT_D )
		{
			if ( p->hipdrop_high_jump_lock )
			{
				//
			} else
			if ( p->hipdrop_cliff_lock )
			{
				//
			} else
			if ( p->hipdrop_state == 0 )
			{
				p->hipdrop_state = 1;
			}
		}


		if ( p->fly_phase == 1 )
		{
			p->fly_phase = 2;

			p->jump_state = N_NN2_JUMP_STATE_RISE;
			p->jump_float = 0;

			p->jump_vertical_onoff = FALSE;

			goto n_goto_rise;
		} else
		if ( p->swim_onoff )
		{
			p->jump_state = N_NN2_JUMP_STATE_RISE;
			p->jump_float = 0;

			p->jump_vertical_onoff = FALSE;
		}


		if ( n_chara_collision_gravity( p ) )
		{
			p->jump_state = N_NN2_JUMP_STATE_DONE;
			p->jump_float = 0;
			p->suck_phase = 0;

			p->hipdrop_cliff_lock = FALSE;

			if ( p->dash_onoff == FALSE )
			{
				p->nina_walk_step = N_NN2_STEP_INIT;
			}

			if ( p->hipdrop_high_jump_lock )
			{
				p->hipdrop_high_jump_lock = FALSE;

				if ( p->input & N_NN2_INPUT_D )
				{
					n_nn2_sound_effect_play( N_NN2_SOUND_LAND );
					goto n_goto_duck;
				} else
				//
				{
					goto n_goto_land;
				}
			} else
			if ( p->hipdrop_state == 1 )
			{
				p->hipdrop_state = 2;
				p->hipdrop_timer = n_posix_tickcount() + 300;

				n_splash_init( p, &p->splash_hipdrop, p->stage->nina_x, p->stage->nina_y + p->nina_sy - p->nina_margin_foot - N_NN2_SPLASH_COUNT, 333 );

				n_nn2_sound_effect_play( N_NN2_SOUND_HIPDROP );
			} else
			//
			{
				p->turn_direction = p->direction;

				if ( p->input & N_NN2_INPUT_D )
				{
					goto n_goto_duck;
				} else
				if ( freefall )
				{
					goto n_goto_land;
				} else {
					if ( p->input )
					{
//NSLog( @"input" );
						goto n_goto_input;
					} else {
//NSLog( @"idle" );
						p->jump_vertical_onoff = TRUE;

						p->sprite_cur = &n_chara_sprite_jump;
						n_sprite_rewind( p->sprite_cur );

						p->draw_center = N_NN2_DRAW_JUMP;

						goto n_goto_idle;
					}
				}
			}
		} else {
			n_chara_action_jump_lr( p );

			p->draw_center = N_NN2_DRAW_JUMP;

//NSLog( @"%d", p->wall_collision );

			if ( p->suck_stop )
			{
				//
			} else
			if ( p->object_handheld_onoff )
			{
				//
			} else
			if ( p->swim_onoff )
			{
				//
			} else
			if ( FALSE == ( p->wall_collision & N_CHARA_COLLISION_LR_SLIP ) )
			{
				//
			} else
			if ( p->suck_phase == 0 )
			{
				p->suck_phase     = 1;
				p->suck_direction = p->direction;

				n_nn2_sound_effect_play( N_NN2_SOUND_SUCK );

				p->sprite_cur = &n_chara_sprite_suck;
				p->draw_center = N_NN2_DRAW_SUCK;
			} else
			if ( p->suck_phase == 2 )
			{
				if (
					(
						( p->input & N_NN2_INPUT_L )
						&&
						( p->direction == N_NN2_DIRECTION_LEFT )
					)
					||
					(
						( p->input & N_NN2_INPUT_R )
						&&
						( p->direction == N_NN2_DIRECTION_RIGHT )
					)
				)
				{
					p->suck_phase     = 1;
					p->suck_direction = p->direction;

					n_nn2_sound_effect_play( N_NN2_SOUND_SUCK );

					p->sprite_cur = &n_chara_sprite_suck;
					p->draw_center = N_NN2_DRAW_SUCK;
				}
			}

		}

	} else
	if ( p->duck_onoff )
	{
//NSLog( @"duck" );

		n_goto_duck:


		if ( p->dokan_onoff )
		{
			//
		} else {
			p->sprite_cur  = &n_chara_sprite_duck;
			p->draw_center = N_NN2_DRAW_DUCK;
		}

	} else
	if ( p->unduck_onoff )
	{
//NSLog( @"unduck" );

		if ( p->dokan_onoff )
		{
			//
		} else {
			p->sprite_cur  = &n_chara_sprite_unduck;
			p->draw_center = N_NN2_DRAW_DUCK;

			if ( p->sprite_cur->once )
			{
				p->unduck_onoff = FALSE;

				p->sprite_cur = &n_chara_sprite_idle;
				n_sprite_rewind( &n_chara_sprite_idle );
			}
		}

	} else
	if ( p->input )
	{
//return;
		if ( n_object_treasure_is_running( p ) ) { p->input = 0; goto n_goto_idle; }


		n_goto_input:


		p->idle_onoff = FALSE;

		int direction = N_NN2_DIRECTION_NONE;

		if ( p->input & N_NN2_INPUT_L )
		{
//NSLog( @"L : %d", p->idle_onoff );
			direction = N_NN2_DIRECTION_LEFT;
		} else
		if ( p->input & N_NN2_INPUT_R )
		{
//NSLog( @"R : %d", p->idle_onoff );
			direction = N_NN2_DIRECTION_RIGHT;
		}

		n_sprite_rewind( &n_chara_sprite_idle );

		n_chara_action_move( p, direction, p->dash_onoff );

		n_chara_collision_gravity( p );

	} else {

		n_goto_idle:


		p->idle_onoff = TRUE;

		p->object_handheld_lock = FALSE;

		p->hipdrop_state = 0;

		if ( ( p->sprite_cur->index == 0 )||( p->sprite_cur->index == 4 )||( p->object_handheld_onoff ) )
		{
			BOOL rewind = ( p->fuwafuwa != 0 );
			n_chara_action_idle( p, rewind );
		} else {
			n_chara_collision_gravity( p );
			p->draw_center = N_NN2_DRAW_IDLE;
		}

	}


	// [!] : Stage 7 : timeup while suck is used

//NSLog( @"%f", p->chip_blend );
	if ( p->chip_blend >= 1.0 )
	{
		p->suck_stop  = TRUE;
		p->suck_phase = 0;
	}


	if ( p->hipdrop_cliff_lock )
	{
		if ( p->hipdrop_cliff_direction != p->direction )
		{
			p->hipdrop_cliff_lock = FALSE;
		}
	}


	if ( p->transition_lock )
	{

		//

	} else
	if ( p->stage->nina_y > p->stage->map_sy )
	{
		p->transition_lock = TRUE;

		if ( p->stage == &n_nn2_stage_0 )
		{
			p->bell_reset_onoff = TRUE;
		}

		if ( n_object_birds_is_caught( p ) )
		{
			n_object_handheld_off( p );
		}

		n_type_gfx x = p->stage->nina_x + ( p->nina_sx / 2 ); x /= p->mapchip_unit;
		n_type_gfx y = N_BMP_SY( &p->stage->map_dokan ) - 1;

		u32 data = 0; n_bmp_ptr_get( &p->stage->map_dokan, x, y, &data );

		int stage_number = n_nn2_map_dokan_data_kind( data );
//NSLog( @"Warp to %d", stage_number );

		extern void n_nn2_stage_transition_go( n_nn2 *p, int stage_number );
		n_nn2_stage_transition_go( p, stage_number );
	}


	if ( p->bounce_landed == FALSE )
	{
		if  ( p->bounce_sprite_l != NULL )
		{
			p->bounce_sprite_l->bounce_offset--;
			if ( p->bounce_sprite_l->bounce_offset < 0 )
			{
				p->bounce_sprite_l->bounce_offset = 0;
				p->bounce_sprite_l = NULL;
			}
		}

		if  ( p->bounce_sprite_m != NULL )
		{
			p->bounce_sprite_m->bounce_offset--;
			if ( p->bounce_sprite_m->bounce_offset < 0 )
			{
				p->bounce_sprite_m->bounce_offset = 0;
				p->bounce_sprite_m = NULL;
			}
		}

		if  ( p->bounce_sprite_r != NULL )
		{
			p->bounce_sprite_r->bounce_offset--;
			if ( p->bounce_sprite_r->bounce_offset < 0 )
			{
				p->bounce_sprite_r->bounce_offset = 0;
				p->bounce_sprite_r = NULL;
			}
		}
	}

	p->bounce_landed = FALSE;


	n_nn2_bgcloud_action( p );


	// [Needed] : this position is important

	n_object_handheld_go ( p );
	n_object_handheld_set( p );

	n_chara_action_dash( p );
	n_nn2_jam_resolver( p );


	n_chara_collision_bell_autogetter_loop( p );


	if ( p->stage == &n_nn2_stage_7 )
	{
//NSLog( @"%d %d", p->stage->timeup, n_posix_tickcount() );

		if ( p->timeup < n_posix_tickcount() )
		{
//NSLog( @"Timeup : %d %d", p->timeup, n_posix_tickcount() );

			// [!] : set an empty map to falling down

			p-> chip_blend = 0;
			p->dokan_blend = 0;

			n_bmp_flush_fastcopy( &p->stage->map_timeup, &p->stage->map         );
			n_bmp_flush_fastcopy( &p->stage->map_timeup, &p->stage->map_dokan   );

			n_chip_init( p, p->stage );
		}
	}


	n_object_treasure_loop( p );


	// [Needed] : this position is important

	n_camerawork_go( p );


//NSLog( @"%d %d", p->camera_x, p->camera_y );

//NSLog( @"%d %d", p->stage->nina_x, p->stage->nina_y );

//n_sprite_cur_debug( p );

//NSLog( @"Rock : %d : %d %d",  p->share->rocks_sprite.stage_number, p->share->rocks_sprite.x, p->share->rocks_sprite.y );
//NSLog( @"Rock : %d",  p->share->rocks_sprite.x % p->mapchip_unit );

//NSLog( @"%d", p->stage->camera_first );

//NSLog( @"%d : %d %d", p->share->kuina_sprite.invisible, p->share->kuina_sprite.x, p->share->kuina_sprite.y );

//NSLog( @"%d", p->wall_collision );

//NSLog( @"%d", p->jump_state );


	return;
}

