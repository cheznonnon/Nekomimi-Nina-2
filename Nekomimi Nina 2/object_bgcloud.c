// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




// internal
void
n_nn2_bgcloud_load_single( n_nn2 *p, n_bmp *bmp, CGFloat ratio )
{

	n_nn2_rc_load( @"rc/object/bgcloud/0", bmp, p->scaler );

	n_type_gfx sx = N_BMP_SX( bmp );
	n_type_gfx sy = N_BMP_SY( bmp );

	n_bmp_resampler( bmp, ratio, ratio );
	n_bmp_resizer( bmp, sx,sy, n_bmp_white_invisible, N_BMP_RESIZER_CENTER );


	return;
}

// internal
void
n_nn2_bgcloud_init_single( n_nn2 *p, n_sprite *spr, n_nn2_stage *stg )
{

	n_sprite_zero( spr );


	int i = 0;
	n_posix_loop
	{

		n_bmp *bmp[ N_SPRITE_MAX ];
		int     ox[ N_SPRITE_MAX ];
		int     oy[ N_SPRITE_MAX ];

		int interval = 2;

		if ( i == 0 )
		{
			interval = 8;

			int n = 0;
			bmp[ n ] = &stg->bgcloud_bmp[ 0 ]; ox[ n ] = 0; oy[ n ] = 0; n++;
			bmp[ n ] = NULL;
		} else
		if ( i == 1 )
		{
			int n = 0;
			bmp[ n ] = &stg->bgcloud_bmp[ 1 ]; ox[ n ] = 0; oy[ n ] = 0; n++;
			bmp[ n ] = NULL;
		} else
		if ( i == 2 )
		{
			int n = 0;
			bmp[ n ] = &stg->bgcloud_bmp[ 2 ]; ox[ n ] = 0; oy[ n ] = 0; n++;
			bmp[ n ] = NULL;
		} else
		if ( i == 3 )
		{
			int n = 0;
			bmp[ n ] = &stg->bgcloud_bmp[ 3 ]; ox[ n ] = 0; oy[ n ] = 0; n++;
			bmp[ n ] = NULL;
		} else
		//
		{
			break;
		}

		n_sprite_set( spr, i, bmp, ox, oy, 0, interval );

		i++;
	}


	spr->option    = N_SPRITE_OPTION_NONE;
	spr->direction = N_NN2_DIRECTION_LEFT;

	spr->sx = N_BMP_SX( spr->obj[ 0 ].bmp[ 0 ] );
	spr->sy = N_BMP_SY( spr->obj[ 0 ].bmp[ 0 ] );


	return;
}

void
n_nn2_bgcloud_init( n_nn2 *p, n_nn2_stage *stg )
{

	n_nn2_bgcloud_load_single( p, &stg->bgcloud_bmp[ 0 ], 1.000 );
	n_nn2_bgcloud_load_single( p, &stg->bgcloud_bmp[ 1 ], 0.975 );
	n_nn2_bgcloud_load_single( p, &stg->bgcloud_bmp[ 2 ], 0.955 );
	n_nn2_bgcloud_load_single( p, &stg->bgcloud_bmp[ 3 ], 0.975 );


	int i = 0;
	n_posix_loop
	{

		n_nn2_bgcloud_init_single( p, &stg->bgcloud_sprite[ i ], stg );

		i++;
		if ( i >= N_NN2_BGCLOUD_MAX ) { break; }
	}


	return;
}

void
n_nn2_bgcloud_reset( n_nn2 *p, n_nn2_stage *stg )
{

	int step[] = { 25, 50, 100, 125 };

	int j = 0;
	n_posix_loop
	{
		int r1 = n_game_random( N_NN2_BGCLOUD_MAX );
		int r2 = n_game_random( N_NN2_BGCLOUD_MAX );

		int   temp = step[ r1 ];
		step[ r1 ] = step[ r2 ];
		step[ r2 ] = temp;

		j++;
		if ( j >= 50 ) { break; }
	}


	BOOL invisible;

	if (
		( stg == &n_nn2_stage_3 )
		//||
		//( stg == &n_nn2_stage_6 )
		||
		( stg == &n_nn2_stage_7 )
	)
	{
		invisible = TRUE;
	} else {
		invisible = FALSE;
	}


	int i = 0;
	n_posix_loop
	{
		stg->bgcloud_sprite[ i ].x = n_game_random( stg->map_sx );
		stg->bgcloud_sprite[ i ].y = stg->map_sy / 10 * ( i + 1 );

		if ( stg == &n_nn2_stage_6 ) { stg->bgcloud_sprite[ i ].y -= 400; }

		stg->bgcloud_interval[ i ] = step[ i ];

		stg->bgcloud_sprite[ i ].invisible = invisible;

		i++;
		if ( i >= N_NN2_BGCLOUD_MAX ) { break; }
	}


	return;
}

// internal
void
n_nn2_bgcloud_draw_single( n_nn2 *p, n_sprite *spr )
{

	if ( spr->invisible ) { return; }


	CGFloat prev = p->global_blend;
	p->global_blend = 0.1;


	n_bmp b; n_nn2_bmp_new_white( &b, spr->sx, spr->sy );


	n_sprite_single *obj = n_object_get( spr );

	int i = 0;
	n_posix_loop
	{

		if ( obj->bmp[ i ] == NULL ) { break; }

		n_bmp *bmp = obj->bmp[ i ];
		int    ox  = obj-> ox[ i ];
		int    oy  = obj-> oy[ i ];

		n_chara_bmp_copy( p, bmp, &b, ox,oy, spr->direction );

		i++;
		if ( i >= N_SPRITE_MAX ) { break; }
	}


	n_type_gfx x = spr->x + spr->ox - n_camerawork_x( p );
	n_type_gfx y = spr->y + spr->oy - n_camerawork_y( p );

	n_nn2_bmp_draw( p, &b, x, y );

	n_bmp_free_fast( &b );


	p->global_blend = prev;


	return;
}

void
n_nn2_bgcloud_animation( n_nn2 *p )
{

	int i = 0;
	n_posix_loop
	{

		n_sprite_animation( &p->stage->bgcloud_sprite[ i ] );

		i++;
		if ( i >= N_NN2_BGCLOUD_MAX ) { break; }
	}

	return;
}

void
n_nn2_bgcloud_draw( n_nn2 *p )
{

	int i = 0;
	n_posix_loop
	{

		n_nn2_bgcloud_draw_single( p, &p->stage->bgcloud_sprite[ i ] );

		i++;
		if ( i >= N_NN2_BGCLOUD_MAX ) { break; }
	}

	return;
}

void
n_nn2_bgcloud_action( n_nn2 *p )
{

	int i = 0;
	n_posix_loop
	{

		n_sprite *s = &p->stage->bgcloud_sprite[ i ];

		if ( n_game_timer( &p->stage->bgcloud_timer[ i ], p->stage->bgcloud_interval[ i ] ) )
		{
			s->x--;
			if ( s->x < -s->sx ) { s->x = p->stage->map_sx; }
		}

		i++;
		if ( i >= N_NN2_BGCLOUD_MAX ) { break; }
	}


	return;
}

