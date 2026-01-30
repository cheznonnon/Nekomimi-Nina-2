// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




void
n_nn2_stage_7_timeup_ui( n_nn2 *p )
{

	u32 bg = n_bmp_rgb_mac( 111,111,111 );
	u32 fg = n_bmp_rgb_mac(   0,200,255 );

	n_type_gfx size = N_NN2_TIMEUP_BAR_SIZE;

	n_type_gfx  x = size;
	n_type_gfx  y = size;
	n_type_gfx sx = p->sx - ( size * 2 );
	n_type_gfx sy = size;

	n_bmp_ui_progressbar_animation          = N_BMP_UI_PROGRESSBAR_ANIMATION_ON_DOWN;
	n_bmp_ui_progressbar_animation_interval = 20;

	CGFloat r;
	CGFloat d = p->timeup - n_posix_tickcount();

	if ( p->timeup_mode == 0 )
	{
		if ( d < N_NN2_STAGE_7_TIMEUP )
		{
			r = d / N_NN2_STAGE_7_TIMEUP;
		} else {
			r = 0.0;
		}
	} else
	if ( p->timeup_mode == 1 )
	{
		r = 0.0;
	} else {
		r = 1.0;
	}
//NSLog( @"%f", r );

	p->chip_blend = p->dokan_blend = 1.0 - sqrt( r );

	n_bmp_ui_progressbar_horz( p->canvas, x,y,sx,sy, fg,bg, r * 100, size * 2 );


	return;
}

void
n_nn2_stage_7_init( n_nn2 *p )
{

	n_nn2_stage *s = &n_nn2_stage_7;


	n_nn2_stage_zero( s );


	// [!] : Stage Number

	n_object_bell_stage_number_nina( p, s );


	// [!] : Map

	n_nn2_rc_load_map( @"rc/7/map/0" , &s->map        );
	n_nn2_rc_load_map( @"rc/7/map/1" , &s->map_queue  );
	n_nn2_rc_load_map( @"rc/7/map/2" , &s->map_dokan  );
	n_nn2_rc_load_map( @"rc/7/map/10", &s->map_timeup );
	n_nn2_rc_load_map( @"rc/7/map/15", &s->map_bell   );

	n_bmp_new( &s->map_label, N_BMP_SX( &s->map ), N_BMP_SY( &s->map ) );

	//n_object_bell_map_init( p, s ); // [x] : hard to implement

	n_nn2_map_metrics_reset( p, s, YES );


	// [!] : Chips

	n_nn2_rc_load( @"rc/chip/rainbow/rainbow_round", &p->bmp_rainbow, p->scaler );

	n_nn2_weather_change( p, s, p->weather_hour );


	n_chip_init( p, s );


	// [!] : Background

	n_nn2_weather_change_background( p, s );


	// [!] : Middleground

	s->middleground_onoff = TRUE;

	n_nn2_bmp_new( &s->canvas_middleground, s->map_sx, s->map_sy );

	n_type_gfx sx   = 1024 * 1.33;
	n_type_gfx sy   = 1024;
	n_type_gfx step = 22;

	n_bmp bmp; n_nn2_bmp_new( &bmp, sx,sy );

	n_type_gfx  o = 0;
	n_type_gfx oo = o * 2;

	n_bmp_circle( &bmp, o,o,sx-oo,sy-oo, n_bmp_rgb_mac( 97,187,70 ) );

	 o += step;
	oo  = o * 2;

	n_bmp_circle( &bmp, o,o,sx-oo,sy-oo, n_bmp_rgb_mac( 253,184,39 ) );

	 o += step;
	oo  = o * 2;

	n_bmp_circle( &bmp, o,o,sx-oo,sy-oo, n_bmp_rgb_mac( 245,130,31 ) );

	 o += step;
	oo  = o * 2;

	n_bmp_circle( &bmp, o,o,sx-oo,sy-oo, n_bmp_rgb_mac( 224,58,62 ) );

	 o += step;
	oo  = o * 2;

	n_bmp_circle( &bmp, o,o,sx-oo,sy-oo, n_bmp_rgb_mac( 150,61,151 ) );

	 o += step;
	oo  = o * 2;

	n_bmp_circle( &bmp, o,o,sx-oo,sy-oo, n_bmp_rgb_mac( 0,157,220 ) );

	 o += step;
	oo  = o * 2;

	n_bmp_circle( &bmp, o,o,sx-oo,sy-oo, n_bmp_black_invisible );

	n_type_gfx x = 100;
	n_type_gfx y = s->map_sy - ( sy / 2 );

	n_bmp_fastcopy( &bmp, &s->canvas_middleground, 0,0,sx,sy/2, x,y );


	n_bmp_free_fast( &bmp );


	n_nn2_stage_init_center( s );


	return;
}

void
n_nn2_stage_7_reset( n_nn2 *p )
{

	n_nn2_stage *s = &n_nn2_stage_7;


	// [!] : for timeout feature

	n_nn2_rc_load_map( @"rc/7/map/0", &s->map       );
	n_nn2_rc_load_map( @"rc/7/map/2", &s->map_dokan );

	n_nn2_map_metrics_reset( p, s, NO );

	n_chip_init( p, s );


	s->center_x = n_game_centering( p->sx, p->nina_sx );
	s->center_y = n_game_centering( p->sy, p->nina_sy );


	s->camera_mode = N_CAMERAWORK_STOP;


	s->camera_first = FALSE;
	s->camera_phase = N_NN2_CAMERA_PHASE_NONE;

	if ( p->object_handheld_sprite != NULL ) { s->camera_first = TRUE; }

	if ( p->sy > s->map_sy )
	{
		s->camera_oy = ( p->sy - s->map_sy ) / 2;
	}


	p->direction = p->turn_direction = N_NN2_DIRECTION_RIGHT;


	n_nn2_nina_starting_position( p, s );


	n_object_rock_reset( p, s );

	n_object_bird_reset( p, s );

	n_nn2_bgcloud_reset( p, s );


	n_nn2_stage_transfer( p, &p->share->debug_move  , s );
	n_nn2_stage_transfer( p, &p->share->debug_jump  , s );
	n_nn2_stage_transfer( p, &p->share->chick_sprite, s );
	n_nn2_stage_transfer( p, &p->share->kuina_sprite, s );
	n_nn2_stage_transfer( p, &p->share->rocks_sprite, s );


	n_nn2_debug_grid_reset( p, s );


	return;
}
