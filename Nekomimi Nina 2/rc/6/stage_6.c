// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




void
n_nn2_stage_6_init( n_nn2 *p )
{

	n_nn2_stage *s = &n_nn2_stage_6;


	n_nn2_stage_zero( s );


	// [!] : Stage Number

	n_object_bell_stage_number_nina( p, s );


	// [!] : Map

	n_nn2_rc_load_map( @"rc/6/map/0" , &s->map );
	n_nn2_rc_load_map( @"rc/6/map/1" , &s->map_queue );
	n_nn2_rc_load_map( @"rc/6/map/2" , &s->map_dokan );
	n_nn2_rc_load_map( @"rc/6/map/15", &s->map_bell  );

	n_bmp_new( &s->map_label, N_BMP_SX( &s->map ), N_BMP_SY( &s->map ) );

	n_object_bell_map_init( p, s );

	n_nn2_map_metrics_reset( p, s, YES );


	// [!] : Chips

	n_nn2_weather_change( p, s, p->weather_hour );

	{
		//u32 color; n_bmp_ptr_get( &s->chip_block, p->mapchip_unit / 2, p->mapchip_unit / 2, &color );
		//n_nn2_stage_hidden_maker( p, s, &s->chip_hidden, n_bmp_black_invisible, n_bmp_black, color );
	}

	n_chip_init( p, s );


	// [!] : Background

	n_nn2_weather_change_background( p, s );


	// [!] : Mountain

	s->middleground_onoff = FALSE;

	//n_nn2_bmp_new( &s->canvas_middleground, s->map_sx / 2, s->map_sy );


	// [!] : Misc

	n_nn2_bgcloud_init( p, s );


	n_object_init( p, &p->share->fish1_sprite, &p->fish_bmp );

	n_object_map_queue( p, &s->map_queue, s, N_OBJECT_MAP_QUEUE_FISH_1 );

	p->share->fish1_sprite.x = n_random_range( s->map_sx );

	p->share->fish1_sprite.direction = 1 + n_random_range( 2 );


	n_object_init( p, &p->share->fish2_sprite, &p->fish_bmp );

	n_object_map_queue( p, &s->map_queue, s, N_OBJECT_MAP_QUEUE_FISH_2 );

	p->share->fish2_sprite.x = n_random_range( s->map_sx );

	p->share->fish2_sprite.direction = 1 + n_random_range( 2 );


	n_object_init( p, &p->share->fish3_sprite, &p->fish_bmp );

	n_object_map_queue( p, &s->map_queue, s, N_OBJECT_MAP_QUEUE_FISH_3 );

	p->share->fish3_sprite.x = n_random_range( s->map_sx );

	p->share->fish3_sprite.direction = 1 + n_random_range( 2 );


	n_nn2_stage_init_center( s );


	return;
}

void
n_nn2_stage_6_reset( n_nn2 *p )
{
//NSLog( @"n_nn2_stage_6_reset()" );

	n_nn2_stage *s = &n_nn2_stage_6;


	n_nn2_map_metrics_reset( p, s, NO );


	s->center_x = n_game_centering( p->sx, p->nina_sx );
	s->center_y = n_game_centering( p->sy, p->nina_sy );


	s->camera_mode = N_CAMERAWORK_LOOP;


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
