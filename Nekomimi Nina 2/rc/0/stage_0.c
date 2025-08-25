// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




void
n_nn2_stage_0_init( n_nn2 *p )
{

	n_nn2_stage *s = &n_nn2_stage_0;


	n_nn2_stage_zero( s );


	// [!] : Stage Number

	n_object_bell_stage_number_nina( p, s );


	// [!] : Map

	n_nn2_rc_load_map( @"rc/0/map/0" , &s->map );
	n_nn2_rc_load_map( @"rc/0/map/1" , &s->map_queue  );
	n_nn2_rc_load_map( @"rc/0/map/2" , &s->map_dokan  );
	n_nn2_rc_load_map( @"rc/0/map/3" , &s->map_move_h );
	n_nn2_rc_load_map( @"rc/0/map/4" , &s->map_move_v );
	n_nn2_rc_load_map( @"rc/0/map/15", &s->map_bell   );

	n_bmp_new( &s->map_label, N_BMP_SX( &s->map ), N_BMP_SY( &s->map ) );

	n_object_bell_map_init( p, s );

	n_nn2_map_metrics_reset( p, s, YES );


	// [!] : Chips

	n_nn2_weather_change( p, s, p->weather_hour );

	n_bmp_new_fast( &s->chip_debug_1, p->mapchip_unit, p->mapchip_unit );
	n_bmp_new_fast( &s->chip_debug_2, p->mapchip_unit, p->mapchip_unit );
	n_bmp_new_fast( &s->chip_debug_3, p->mapchip_unit, p->mapchip_unit );

	n_bmp_flush( &s->chip_debug_1, n_bmp_blend_pixel( n_bmp_rgb_mac( 0,200,255 ), n_bmp_black, 0.1 ) );
	n_bmp_flush( &s->chip_debug_2, n_bmp_blend_pixel( n_bmp_rgb_mac( 0,200,255 ), n_bmp_black, 0.2 ) );
	n_bmp_flush( &s->chip_debug_3, n_bmp_blend_pixel( n_bmp_rgb_mac( 0,200,255 ), n_bmp_black, 0.3 ) );

	n_chip_init( p, s );


	// [!] : Background

	n_nn2_weather_change_background( p, s );


	// [!] : Mountain

	s->middleground_onoff = TRUE;

	n_nn2_bmp_new( &s->canvas_middleground, s->map_sx / 2, s->map_sy );

	n_nn2_stage_mountain_set( p, s, s->map_sx / 2 / 8 * 2 );
	n_nn2_stage_mountain_set( p, s, s->map_sx / 2 / 8 * 6 );


	// [!] : Misc

	n_object_lift_init( p, &s->cld_h_0_sprite );

	n_object_lift_init( p, &s->cld_v_0_sprite );
	s->cld_v_0_sprite.jump_state = N_NN2_JUMP_STATE_FALL;


	n_nn2_bgcloud_init( p, s );


	n_nn2_stage_init_center( s );


	return;
}

void
n_nn2_stage_0_reset( n_nn2 *p )
{

	n_nn2_stage *s = &n_nn2_stage_0;


	n_nn2_map_metrics_reset( p, s, NO );


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

	n_camerawork_go( p );


	p->direction = p->turn_direction = N_NN2_DIRECTION_RIGHT;


	n_nn2_nina_starting_position( p, s );


	n_object_lift_reset( p, &s->cld_h_0_sprite );
	n_object_lift_reset( p, &s->cld_v_0_sprite );

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
