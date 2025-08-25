// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




void
n_nn2_stage_4_init( n_nn2 *p )
{

	n_nn2_stage *s = &n_nn2_stage_4;


	n_nn2_stage_zero( s );


	// [!] : Stage Number

	n_object_bell_stage_number_nina( p, s );


	// [!] : Map

	n_nn2_rc_load_map( @"rc/4/map/0" , &s->map );
	n_nn2_rc_load_map( @"rc/4/map/1" , &s->map_queue  );
	n_nn2_rc_load_map( @"rc/4/map/2" , &s->map_dokan  );
	n_nn2_rc_load_map( @"rc/4/map/3" , &s->map_move_h );
	n_nn2_rc_load_map( @"rc/4/map/4" , &s->map_move_v );
	n_nn2_rc_load_map( @"rc/4/map/5" , &s->map_lift_guideline );
	n_nn2_rc_load_map( @"rc/4/map/15", &s->map_bell   );

	n_bmp_new( &s->map_label, N_BMP_SX( &s->map ), N_BMP_SY( &s->map ) );

	n_object_bell_map_init( p, s );

	n_nn2_map_metrics_reset( p, s, YES );


	// [!] : Chips

	n_nn2_weather_change( p, s, p->weather_hour );

	n_nn2_rc_load( @"rc/4/chip/special/0", &s->chip_special_lite, p->scaler );
	n_nn2_rc_load( @"rc/4/chip/special/1", &s->chip_special_dark, p->scaler );

	n_chip_init( p, s );


	// [!] : Background

	n_nn2_weather_change_background( p, s );


	// [!] : Mountain

	s->middleground_onoff = FALSE;

	//n_nn2_bmp_new( &s->canvas_middleground, s->map_sx / 2, s->map_sy );


	// [!] : Misc

	n_object_lift_init( p, &s->cld_h_0_sprite );

	n_object_lift_init( p, &s->cld_h_1_sprite );
	s->cld_h_1_sprite.direction = N_NN2_DIRECTION_RIGHT;

	n_object_lift_init( p, &s->cld_v_0_sprite );
	s->cld_v_0_sprite.jump_state = N_NN2_JUMP_STATE_FALL;

	n_object_lift_init( p, &s->cld_v_1_sprite );
	s->cld_v_1_sprite.jump_state = N_NN2_JUMP_STATE_FALL;

	//n_object_lift_init( p, &s->cld_v_2_sprite );
	//s->cld_v_2_sprite.jump_state = N_NN2_JUMP_STATE_FALL;

	//n_object_lift_init( p, &s->cld_v_3_sprite );
	//s->cld_v_3_sprite.jump_state = N_NN2_JUMP_STATE_FALL;


	n_nn2_bgcloud_init( p, s );


	n_nn2_stage_init_center( s );


	return;
}

void
n_nn2_stage_4_reset( n_nn2 *p )
{

	n_nn2_stage *s = &n_nn2_stage_4;


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


	n_object_lift_reset( p, &s->cld_h_0_sprite ); //s->cld_h_0_sprite.invisible = TRUE;
	n_object_lift_reset( p, &s->cld_h_1_sprite ); //s->cld_h_1_sprite.invisible = TRUE;
	n_object_lift_reset( p, &s->cld_v_0_sprite ); //s->cld_v_0_sprite.invisible = TRUE;
	n_object_lift_reset( p, &s->cld_v_1_sprite ); //s->cld_v_1_sprite.invisible = TRUE;
	//n_object_lift_reset( p, &s->cld_v_2_sprite ); //s->cld_v_2_sprite.invisible = TRUE;
	//n_object_lift_reset( p, &s->cld_v_3_sprite ); //s->cld_v_3_sprite.invisible = TRUE;

	n_object_rock_reset( p, s );

	n_object_bird_reset( p, s );

	n_nn2_bgcloud_reset( p, s );


	n_nn2_stage_transfer( p, &p->share->debug_move  , s );
	n_nn2_stage_transfer( p, &p->share->debug_jump  , s );
	n_nn2_stage_transfer( p, &p->share->chick_sprite, s );
	n_nn2_stage_transfer( p, &p->share->kuina_sprite, s );
	n_nn2_stage_transfer( p, &p->share->rocks_sprite, s );


	// [Needed] : passing through without this

	p->share->rocks_sprite.is_landed = TRUE;


	n_nn2_debug_grid_reset( p, s );


	return;
}
