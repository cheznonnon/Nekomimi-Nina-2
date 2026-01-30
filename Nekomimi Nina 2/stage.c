// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




void
n_nn2_stage_stand_register( n_nn2 *p, n_sprite *s, BOOL reg_or_unreg )
{

	if ( reg_or_unreg )
	{

		if ( s == &p->stage->cld_h_0_sprite )
		{
			if ( p->stage->cld_v_0_sprite.sprite_stand == NULL )
			{
				p->stage->cld_h_0_sprite.sprite_stand = s;
			}
		} else
		if ( s == &p->stage->cld_h_1_sprite )
		{
			p->stage->cld_h_1_sprite.sprite_stand = s;
			p->stage->cld_h_1_sprite.stand_offset = p->stage->nina_x - s->x;
		} else
		if ( s == &p->stage->cld_v_0_sprite )
		{
			if ( p->stage->cld_h_0_sprite.sprite_stand == NULL )
			{
				p->stage->cld_v_0_sprite.sprite_stand = s;
			}
		} else
		if ( s == &p->share->rocks_sprite )
		{
			p->share->rocks_sprite.sprite_stand = s;
		} else
		if ( s == &p->stage->cld_v_1_sprite )
		{
			p->stage->cld_v_1_sprite.sprite_stand = s;
		}

	} else {

		if ( s == &p->share->rocks_sprite )
		{
//NSLog( @"rocks_sprite : OFF" );
			p->share->rocks_sprite.sprite_stand = NULL;
		} else
		if ( s == &p->stage->cld_h_0_sprite )
		{
			p->stage->cld_h_0_sprite.sprite_stand = NULL;
		} else
		if ( s == &p->stage->cld_h_1_sprite )
		{
			p->stage->cld_h_1_sprite.sprite_stand = NULL;
		} else
		if ( s == &p->stage->cld_v_0_sprite )
		{
			p->stage->cld_v_0_sprite.sprite_stand = NULL;
		} else
		if ( s == &p->stage->cld_v_1_sprite )
		{
			p->stage->cld_v_1_sprite.sprite_stand = NULL;
		}

	}


	return;
}

void
n_nn2_stage_stand_all_off( n_nn2 *p )
{

	p->share->  rocks_sprite.sprite_stand = NULL;

	p->stage->cld_h_0_sprite.sprite_stand = NULL;
	p->stage->cld_h_1_sprite.sprite_stand = NULL;
	p->stage->cld_v_0_sprite.sprite_stand = NULL;
	p->stage->cld_v_1_sprite.sprite_stand = NULL;

	return;
}

BOOL
n_nn2_stage_stand_onoff_horz( n_nn2 *p, n_sprite *s )
{

	BOOL ret = FALSE;

	if ( p->stage->cld_h_0_sprite.sprite_stand == s ) { ret = TRUE; }


	return ret;
}

BOOL
n_nn2_stage_stand_onoff_vert( n_nn2 *p, n_sprite *s )
{

	BOOL ret = FALSE;

	if ( p->stage->cld_v_0_sprite.sprite_stand == s ) { ret = TRUE; } else
	if ( p->stage->cld_v_1_sprite.sprite_stand == s ) { ret = TRUE; }


	return ret;
}

BOOL
n_nn2_stage_stand_onoff_turn( n_nn2 *p, n_sprite *s )
{

	BOOL ret = FALSE;

	if ( p->stage->cld_h_1_sprite.sprite_stand == s ) { ret = TRUE; }


	return ret;
}

BOOL
n_nn2_stage_stand_onoff_rock( n_nn2 *p )
{

	BOOL ret = FALSE;

	if ( p->share->rocks_sprite.sprite_stand != NULL ) { ret = TRUE; }


	return ret;
}

BOOL
n_nn2_stage_stand_onoff( n_nn2 *p )
{

	BOOL ret = FALSE;

	if ( p->share->rocks_sprite.sprite_stand != NULL )
	{
		if ( p->share->rocks_sprite.sprite_stand->is_caught == FALSE )
		{
			ret = TRUE;
		}
	} else
	if ( p->stage->cld_h_0_sprite.sprite_stand != NULL ) { ret = TRUE; } else
	if ( p->stage->cld_h_1_sprite.sprite_stand != NULL ) { ret = TRUE; } else
	if ( p->stage->cld_v_0_sprite.sprite_stand != NULL ) { ret = TRUE; } else
	if ( p->stage->cld_v_1_sprite.sprite_stand != NULL ) { ret = TRUE; }


	return ret;
}

BOOL
n_nn2_stage_stand_onoff_cloud( n_nn2 *p )
{

	BOOL ret = FALSE;

	if ( p->stage->cld_h_0_sprite.sprite_stand != NULL ) { ret = TRUE; } else
	if ( p->stage->cld_h_1_sprite.sprite_stand != NULL ) { ret = TRUE; } else
	if ( p->stage->cld_v_0_sprite.sprite_stand != NULL ) { ret = TRUE; } else
	if ( p->stage->cld_v_1_sprite.sprite_stand != NULL ) { ret = TRUE; }


	return ret;
}




n_nn2_stage*
n_nn2_stage_number2ptr( int number )
{

	n_nn2_stage *ret = NULL;

	if ( number == 0 ) { ret = &n_nn2_stage_0; } else
	if ( number == 1 ) { ret = &n_nn2_stage_1; } else
	if ( number == 2 ) { ret = &n_nn2_stage_2; } else
	if ( number == 3 ) { ret = &n_nn2_stage_3; } else
	if ( number == 4 ) { ret = &n_nn2_stage_4; } else
	if ( number == 5 ) { ret = &n_nn2_stage_5; } else
	if ( number == 6 ) { ret = &n_nn2_stage_6; } else
	if ( number == 7 ) { ret = &n_nn2_stage_7; } else
	if ( number == 8 ) { ret = &n_nn2_stage_8; }


	return ret;
}




void
n_nn2_stage_transition( n_nn2 *p, n_nn2_stage *from, n_nn2_stage *to )
{

	BOOL is_first = ( from == to );


	n_bmp_new_fast( &p->transition_bmp_old, p->sx, p->sy ); //n_bmp_flush( &p->transition_bmp_old, n_bmp_black );
	n_bmp_new_fast( &p->transition_bmp_mid, p->sx, p->sy ); n_bmp_flush( &p->transition_bmp_mid, n_bmp_black );
	n_bmp_new_fast( &p->transition_bmp_new, p->sx, p->sy );

	{
		extern void n_nn2_loop_minimal( n_nn2 *p );
		extern void n_nn2_draw( n_nn2 *p );

		n_bmp_flush( &p->canvas_base, n_bmp_black );


		p->stage  = from;
		p->canvas = &p->transition_bmp_old;

		n_nn2_loop_minimal( p );
		n_nn2_draw( p );

		from->camera_lock = FALSE;

		if ( to == &n_nn2_stage_0 ) { n_nn2_stage_0_reset( p ); } else
		if ( to == &n_nn2_stage_1 ) { n_nn2_stage_1_reset( p ); } else
		if ( to == &n_nn2_stage_2 ) { n_nn2_stage_2_reset( p ); } else
		if ( to == &n_nn2_stage_3 ) { n_nn2_stage_3_reset( p ); } else
		if ( to == &n_nn2_stage_4 ) { n_nn2_stage_4_reset( p ); } else
		if ( to == &n_nn2_stage_5 ) { n_nn2_stage_5_reset( p ); } else
		if ( to == &n_nn2_stage_6 ) { n_nn2_stage_6_reset( p ); } else
		if ( to == &n_nn2_stage_7 ) { n_nn2_stage_7_reset( p ); } else
		if ( to == &n_nn2_stage_8 ) { n_nn2_stage_8_reset( p ); }

		n_chip_dokan_label_set( p, p->dokan_label_x, p->dokan_label_y );

		p-> chip_blend = 0;
		p->dokan_blend = 0;
		p->suck_stop   = FALSE;

		p->splash_dash   .onoff = FALSE;
		p->splash_slip   .onoff = FALSE;
		p->splash_hipdrop.onoff = FALSE;
		p->splash_cloud  .onoff = FALSE;
		p->splash_water  .onoff = FALSE;


		p->stage  = to;
		p->canvas = &p->transition_bmp_new;

		if ( p->stage == &n_nn2_stage_7 )
		{
			p->timeup_mode = 2;
		} else {
			p->timeup_mode = 0;
		}

		if ( p->bell_reset_onoff )
		{
			p->bell_reset_onoff = FALSE;
			n_object_bell_reset( p );
		}

		p->bell_onoff = TRUE;

		n_nn2_loop_minimal( p );
		n_nn2_draw( p );

		p->bell_onoff = FALSE;


		p->timeup_mode = 0;

		p->stage      = from;
		p->stage_next = to;

		p->canvas = &p->canvas_main;
	}


	if ( is_first )
	{
		p->transition_type = N_BMP_UI_TRANSITION_WIPE_Y;
	} else  {
		//p->transition_type++;
		//if ( p->transition_type > N_GAME_TRANSITION_LAST ) { p->transition_type = N_GAME_TRANSITION_WIPE_X; }

		p->transition_type = N_BMP_UI_TRANSITION_FADE;
	}

//p->transition_type = N_GAME_TRANSITION_CIRCLE;

#ifdef N_NN2_TITLE_ON

	p->transition_phase = 1;

#else

	if ( is_first )
	{
		p->transition_phase = 2;
	} else {
		p->transition_phase = 1;
	}

#endif

	p->transition_percent = 0;


	n_bmp_ui_transition_circle_x = p->sx / 2;
	n_bmp_ui_transition_circle_y = p->sy / 2;


//NSLog( @"%d", p->stage->nina_x );
	return;
}

void
n_nn2_stage_transition_go( n_nn2 *p, int stage_number )
{
//NSLog( @"n_nn2_stage_transition_go() : %d", stage_number );

	n_nn2_stage *next = &n_nn2_stage_0;

	stage_number = n_nn2_map_dokan_stage_number( p, stage_number );

	if ( 0 == stage_number )
	{
		next = &n_nn2_stage_0;
	} else
	if ( 1 == stage_number )
	{
		next = &n_nn2_stage_1;
	} else
	if ( 2 == stage_number )
	{
		next = &n_nn2_stage_2;
	} else
	if ( 3 == stage_number )
	{
		next = &n_nn2_stage_3;
	} else
	if ( 4 == stage_number )
	{
		next = &n_nn2_stage_4;
	} else
	if ( 5 == stage_number )
	{
		next = &n_nn2_stage_5;
	} else
	if ( 6 == stage_number )
	{
		next = &n_nn2_stage_6;
	} else
	if ( 7 == stage_number )
	{
		next = &n_nn2_stage_7;
	} else
	if ( 8 == stage_number )
	{
		next = &n_nn2_stage_8;
	}


	if ( p->stage == &n_nn2_stage_0 )
	{
//NSLog( @"0" );
		n_nn2_stage_transition( p, &n_nn2_stage_0, next );
	} else
	if ( p->stage == &n_nn2_stage_1 )
	{
//NSLog( @"1" );
		n_nn2_stage_transition( p, &n_nn2_stage_1, next );
	} else
	if ( p->stage == &n_nn2_stage_2 )
	{
//NSLog( @"2" );
		n_nn2_stage_transition( p, &n_nn2_stage_2, next );
	} else
	if ( p->stage == &n_nn2_stage_3 )
	{
//NSLog( @"3" );
		n_nn2_stage_transition( p, &n_nn2_stage_3, next );
	} else
	if ( p->stage == &n_nn2_stage_4 )
	{
		n_nn2_stage_transition( p, &n_nn2_stage_4, next );
	} else
	if ( p->stage == &n_nn2_stage_5 )
	{
		n_nn2_stage_transition( p, &n_nn2_stage_5, next );
	} else
	if ( p->stage == &n_nn2_stage_6 )
	{
		n_nn2_stage_transition( p, &n_nn2_stage_6, next );
	} else
	if ( p->stage == &n_nn2_stage_7 )
	{
		n_nn2_stage_transition( p, &n_nn2_stage_7, next );
	} else
	if ( p->stage == &n_nn2_stage_8 )
	{
		n_nn2_stage_transition( p, &n_nn2_stage_8, next );
	}


	return;
}

void
n_nn2_stage_throw_go( n_nn2 *p, n_sprite *s, n_type_gfx x, n_type_gfx y )
{
//return;

	if ( ( p->object_handheld_onoff )&&( p->object_handheld_sprite == s ) ) { return; }


	if ( ( x == -1 )||( y == -1 ) )
	{
		x = s->x + ( s->sx / 2 );
		y = p->stage->map_sy - 1;

		x /= p->mapchip_unit;
		y /= p->mapchip_unit;
	}


	int stage_number = n_nn2_map_dokan_stage_number_get_by_position( p, x, y );
//NSLog( @"%d", stage_number );


	s->stage_number = stage_number;
	s->throw_onoff  = TRUE;
	s->is_landed    = FALSE;
	s->invisible    = TRUE;

	if ( s == &p->share->kuina_sprite )
	{
		s->fall_step = N_OBJ_STEP_FALL;
	}


	n_nn2_npc_starting_position( p, n_nn2_stage_number2ptr( s->stage_number ), s );


//NSLog( @"Object Warp : X %d Y %d : %d %d %d(No.)", x,y, n_bmp_r( data ), n_bmp_g( data ), n_bmp_b( data ) );


	return;
}




void
n_nn2_stage_animation( n_nn2 *p )
{

	if ( p->stage == &n_nn2_stage_0 )
	{
		n_sprite_animation( &p->stage->cld_h_0_sprite );
		n_sprite_animation( &p->stage->cld_v_0_sprite );
	} else
	if ( p->stage == &n_nn2_stage_4 )
	{
		n_sprite_animation( &p->stage->cld_h_0_sprite );
		n_sprite_animation( &p->stage->cld_h_1_sprite );
		n_sprite_animation( &p->stage->cld_v_0_sprite );
		n_sprite_animation( &p->stage->cld_v_1_sprite );
		//n_sprite_animation( &p->stage->cld_v_2_sprite );
		//n_sprite_animation( &p->stage->cld_v_3_sprite );
	} //else

	n_nn2_bgcloud_animation( p );

	n_sprite_animation( &p->share->bird1_sprite );
	n_sprite_animation( &p->share->bird2_sprite );
	n_sprite_animation( &p->share->bird3_sprite );
	n_sprite_animation( &p->share->chick_sprite );
	n_sprite_animation( &p->share->kuina_sprite );

	//n_sprite_animation( &p->share->rocks_sprite );
	p->share->rocks_sprite.index = p->brick_animation_index;
//NSLog( @"%d %d", p->brick_animation_index, p->share->rocks_sprite.frame );


	return;
}




void
n_nn2_stage_share_init( n_nn2 *p )
{

	{

		n_bmp bmp; n_bmp_zero( &bmp );
		n_nn2_rc_load( @"rc/1/chip/weather/0", &bmp, p->scaler );

		n_type_gfx sz = p->mapchip_unit;

		n_nn2_bmp_new( &p->weather_bmp_lite, sz, sz );
		n_nn2_bmp_new( &p->weather_bmp_dark, sz, sz );

		{
			u32 outer = n_bmp_rgb_mac( 128,128,128 );
			u32 inner = n_bmp_rgb_mac( 255,255,255 );

			n_bmp_ui_roundframe( &p->weather_bmp_lite, 0, 0, sz, sz, 4, 2, outer, inner );
		}

		{
			u32 outer = n_bmp_rgb_mac( 128,128,128 );
			u32 inner = n_bmp_rgb_mac(  55, 55, 55 );

			n_bmp_ui_roundframe( &p->weather_bmp_dark, 0, 0, sz, sz, 4, 2, outer, inner );
		}

		n_bmp_flush_transcopy( &bmp, &p->weather_bmp_lite );
		n_bmp_flush_transcopy( &bmp, &p->weather_bmp_dark );

		n_bmp_free_fast( &bmp );
	}


	n_object_birds_init( p, &p->share->bird1_sprite );
	n_object_birds_init( p, &p->share->bird2_sprite );
	n_object_birds_init( p, &p->share->bird3_sprite );
	n_object_chick_init( p, &p->share->chick_sprite );
	n_object_kuina_init( p, &p->share->kuina_sprite );
	n_object_rocks_init( p, &p->share->rocks_sprite );

	p->share->chick_sprite.stage_number = N_NN2_STAGE_NUMBER_CHICK;
	p->share->kuina_sprite.stage_number = N_NN2_STAGE_NUMBER_KUINA;
	p->share->rocks_sprite.stage_number = N_NN2_STAGE_NUMBER_ROCKS;


	n_object_init( p, &p->share->debug_move, &p->stage->chip_brick_1 );

	p->share->debug_move.option     = N_SPRITE_OPTION_CREEP;
	p->share->debug_move.jump_state = N_NN2_JUMP_STATE_FALL;

	p->share->debug_move.move_step += 1;


	n_object_init( p, &p->share->debug_jump, &p->stage->chip_brick_1 );

	p->share->debug_jump.option     = N_SPRITE_OPTION_JUMP;
	p->share->debug_jump.jump_state = N_NN2_JUMP_STATE_FALL;
	p->share->debug_jump.jump_max   = N_OBJ_JUMP_MAX;


	// [!] : these positions will be overwritten

	n_type_gfx ox = p->mapchip_unit;

	p->share->debug_move  .starting_x = ox *  1;
	p->share->debug_jump  .starting_x = ox *  2;

	p->share->chick_sprite.starting_x = ox *  3;
	p->share->kuina_sprite.starting_x = ox *  4;
	p->share->rocks_sprite.starting_x = ox *  6;

	p->share->bird1_sprite.starting_x = ox *  7;
	p->share->bird2_sprite.starting_x = ox *  9;
	p->share->bird3_sprite.starting_x = ox * 11;


	return;
}

void
n_nn2_stage_init( n_nn2 *p )
{

	BOOL prv = n_bmp_is_multithread;
	n_bmp_is_multithread = TRUE;


	NSOperation *o;


	o = [NSBlockOperation blockOperationWithBlock:^{

		n_nn2_stage_0_init( p );

	}];
	[p->queue addOperation:o];


	o = [NSBlockOperation blockOperationWithBlock:^{

		n_nn2_stage_1_init( p );

	}];
	[p->queue addOperation:o];


	o = [NSBlockOperation blockOperationWithBlock:^{

		n_nn2_stage_2_init( p );

	}];
	[p->queue addOperation:o];


	o = [NSBlockOperation blockOperationWithBlock:^{

		n_nn2_stage_3_init( p );

	}];
	[p->queue addOperation:o];


	o = [NSBlockOperation blockOperationWithBlock:^{

		n_nn2_stage_4_init( p );

	}];
	[p->queue addOperation:o];


	o = [NSBlockOperation blockOperationWithBlock:^{

		n_nn2_stage_5_init( p );

	}];
	[p->queue addOperation:o];


	o = [NSBlockOperation blockOperationWithBlock:^{

		n_nn2_stage_6_init( p );

	}];
	[p->queue addOperation:o];


	o = [NSBlockOperation blockOperationWithBlock:^{

		n_nn2_stage_7_init( p );

	}];
	[p->queue addOperation:o];


	o = [NSBlockOperation blockOperationWithBlock:^{

		n_nn2_stage_8_init( p );

	}];
	[p->queue addOperation:o];


	[p->queue waitUntilAllOperationsAreFinished];

	n_bmp_is_multithread = prv;


	return;
}

