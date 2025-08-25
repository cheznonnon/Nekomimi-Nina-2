// Project Nonnon
// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




void
n_object_chick_indicator_maker( n_nn2 *p )
{

	n_type_gfx sx = N_BMP_SX( &p->chick_body );
	n_type_gfx sy = N_BMP_SY( &p->chick_body );

	n_nn2_bmp_new( &p->chick_stage_number_indicator, sx, sy );

	n_bmp_flush_transcopy( &p->chick_body  , &p->chick_stage_number_indicator );
	n_bmp_flush_transcopy( &p->chick_wing_2, &p->chick_stage_number_indicator );

	n_bmp_flush_antialias( &p->chick_stage_number_indicator, 1.0 );
	n_bmp_scaler_lil( &p->chick_stage_number_indicator, 2 );


	return;
}

void
n_object_chick_init( n_nn2 *p, n_sprite *s )
{

	n_sprite_zero( s );

	int i = 0;
	n_posix_loop
	{

		n_bmp *bmp[ N_SPRITE_MAX ];
		int     ox[ N_SPRITE_MAX ];
		int     oy[ N_SPRITE_MAX ];

		int interval = 3;

		if ( i == 0 )
		{
			int n = 0;
			bmp[ n ] = &p->chick_body  ; ox[ n ] = 0; oy[ n ] = 0; n++;
			bmp[ n ] = &p->chick_wing_n; ox[ n ] = 0; oy[ n ] = 0; n++;
			//bmp[ n ] = &p->chick_foot_n; ox[ n ] = 0; oy[ n ] = 0; n++;
			bmp[ n ] = NULL;
		} else
		if ( i == 1 )
		{
			int n = 0;
			bmp[ n ] = &p->chick_body  ; ox[ n ] = 0; oy[ n ] = 0; n++;
			bmp[ n ] = &p->chick_wing_1; ox[ n ] = 0; oy[ n ] = 0; n++;
			//bmp[ n ] = &p->chick_foot_1; ox[ n ] = 0; oy[ n ] = 0; n++;
			bmp[ n ] = NULL;
		} else
		if ( i == 2 )
		{
			int n = 0;
			bmp[ n ] = &p->chick_body  ; ox[ n ] = 0; oy[ n ] = 0; n++;
			bmp[ n ] = &p->chick_wing_2; ox[ n ] = 0; oy[ n ] = 0; n++;
			//bmp[ n ] = &p->chick_foot_n; ox[ n ] = 0; oy[ n ] = 0; n++;
			bmp[ n ] = NULL;
		} else
		if ( i == 3 )
		{
			int n = 0;
			bmp[ n ] = &p->chick_body  ; ox[ n ] = 0; oy[ n ] = 0; n++;
			bmp[ n ] = &p->chick_wing_1; ox[ n ] = 0; oy[ n ] = 0; n++;
			//bmp[ n ] = &p->chick_foot_2; ox[ n ] = 0; oy[ n ] = 0; n++;
			bmp[ n ] = NULL;
		} else
		//
		{
			break;
		}

		n_sprite_set( s, i, bmp, ox, oy, 0, interval );

		i++;
	}


	s->option     = N_SPRITE_OPTION_JUMP;

	s->jump_state = N_NN2_JUMP_STATE_FALL;
	s->jump_max   = N_OBJ_JUMP_MAX * 2;

	s->move_step  = N_OBJ_STEP_MOVE;
	s->fall_step  = N_OBJ_STEP_FALL;


	s->sx = N_BMP_SX( s->obj[ 0 ].bmp[ 0 ] );
	s->sy = N_BMP_SY( s->obj[ 0 ].bmp[ 0 ] );


	s->margin_sx = 12 * p->zoom;
	s->margin_sy = 12 * p->zoom;


	s->direction  = N_NN2_DIRECTION_LEFT;


	n_object_chick_indicator_maker( p );


	return;
}

void
n_object_chick_draw( n_nn2 *p, n_sprite *s )
{

	if ( s->invisible ) { return; }


	n_bmp *b = n_nn2_shared_bmp( s );


	n_sprite_single *obj = n_object_get( s );


	if ( s->is_caught ) { s->direction = p->direction; }


	const int wink_max = 100;


	int i = 0;
	n_posix_loop
	{

		if ( obj->bmp[ i ] == NULL ) { break; }

		n_bmp *bmp = obj->bmp[ i ];
		int    ox  = obj-> ox[ i ];
		int    oy  = obj-> oy[ i ];

		if ( s->is_caught ) 
		{
			if ( bmp == &p->chick_wing_1 ) { i++; continue; } else
			if ( bmp == &p->chick_wing_2 ) { i++; continue; } else
			if ( bmp == &p->chick_foot_1 ) { bmp = &p->chick_foot_n; } else
			if ( bmp == &p->chick_foot_2 ) { bmp = &p->chick_foot_n; }
		}

		n_sprite *spr = NULL;
		if ( s == p->object_dokan_sprite ) { spr = s; }

		n_chara_bmp_copy_all_grabbable( p, bmp, b, ox,oy, s->direction, NO, spr );

		if ( s->wink_count <= 5 )
		{
			n_chara_bmp_copy_all_grabbable( p, &p->chick_wink, b, ox,oy, s->direction, NO, spr );
		}

		i++;
		if ( i >= N_SPRITE_MAX ) { break; }
	}


	s->wink_count++;
	if ( s->wink_count > wink_max ) { s->wink_count = 0; }


	n_type_gfx x = s->x + s->ox - n_camerawork_x( p );
	n_type_gfx y = s->y + s->oy - n_camerawork_y( p );

	n_nn2_bmp_draw( p, b, x, y );


	return;
}

void
n_object_chick_draw_overlay( n_nn2 *p, n_sprite *s )
{

	if ( s->invisible ) { return; }


	if ( s->is_caught == FALSE ) { return; }


	n_bmp *b = n_nn2_shared_bmp_internal( s, n_bmp_argb_mac( 0,255,222,0 ) );


	n_sprite_single *obj = n_object_get( s );


	s->direction = p->direction;


	int i = 0;
	n_posix_loop
	{

		if ( obj->bmp[ i ] == NULL ) { break; }

		n_bmp *bmp = obj->bmp[ i ];
		int    ox  = obj-> ox[ i ];
		int    oy  = obj-> oy[ i ];

		if (
			( bmp == &p->chick_wing_n )
			||
			( bmp == &p->chick_wing_1 )
			||
			( bmp == &p->chick_wing_2 )
		)
		{
			n_sprite *spr = NULL;
			if ( s == p->object_dokan_sprite ) { spr = s; }

			n_chara_bmp_copy_all_grabbable( p, bmp, b, ox,oy, s->direction, NO, spr );
		}

		i++;
		if ( i >= N_SPRITE_MAX ) { break; }
	}


	n_type_gfx x = s->x + s->ox - n_camerawork_x( p );
	n_type_gfx y = s->y + s->oy - n_camerawork_y( p );

	n_nn2_bmp_draw( p, b, x, y );


	return;
}


