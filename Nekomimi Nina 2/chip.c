// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html



extern void n_chara_bmp_copy( n_nn2 *p, n_bmp *f, n_bmp *t, n_type_gfx x, n_type_gfx y, int direction );




#define n_chip_data_kind          n_bmp_b_mac
#define n_nn2_map_dokan_data_kind n_bmp_r_mac
#define n_nn2_map_enter_data_kind n_bmp_g_mac

int
n_nn2_map_dokan_stage_number( n_nn2 *p, int stage_number )
{

	if ( 255 == stage_number )
	{
#ifdef N_NN2_STAGE_SHUFFLE
		//stage_number = 2 + n_random_range( 4 );
		stage_number = p->dokan_stage_number_shuffle;
//NSLog( @"Shuffle" );
#else
		stage_number = 2;
#endif
//NSLog( @"Stage Random : %d", stage_number );
	}


	return stage_number;
}

int
n_nn2_map_dokan_stage_number_get_by_position( n_nn2 *p, n_type_gfx x, n_type_gfx y )
{

	u32 data = 0; n_bmp_ptr_get( &p->stage->map_dokan, x, y, &data );

	int stage_number = n_nn2_map_dokan_stage_number( p, n_nn2_map_dokan_data_kind( data ) );
//NSLog( @"%d", stage_number );

	return stage_number;
}




#define N_CHIP_SKY         ( 0 )
#define N_CHIP_EARTH       ( 1 )
#define N_CHIP_EARTH_L     ( 2 )
#define N_CHIP_EARTH_R     ( 3 )
#define N_CHIP_BLOCK       ( 4 )
#define N_CHIP_BRICK       ( 5 )
#define N_CHIP_DOKAN_T     ( 6 )
#define N_CHIP_DOKAN_B     ( 7 )
#define N_CHIP_BLOCK_INV   ( 8 )
#define N_CHIP_PASS_THRU   ( 9 )
#define N_CHIP_SPECIAL     ( 10 )
#define N_CHIP_HIDDEN      ( 11 )
#define N_CHIP_WEATHER     ( 12 )
#define N_CHIP_RAINBOW     ( 13 )
#define N_CHIP_RAINBOW_BG  ( 14 )
#define N_CHIP_BELL        ( 15 )
#define N_CHIP_TREASURE_B  ( 16 )
#define N_CHIP_TREASURE_L  ( 17 )
#define N_CHIP_TREASURE_X  ( 18 )
#define N_CHIP_TREASURE_SL ( 19 )
#define N_CHIP_TREASURE_SR ( 20 )
#define N_CHIP_TREASURE_HT ( 21 )

#define N_CHIP_DOKAN_Q_L   ( 100 )
#define N_CHIP_DOKAN_Q_R   ( 101 )
#define N_CHIP_DOKAN_WARP  ( 102 )
#define N_CHIP_DOKAN_OBJ   ( 103 )


#define N_CHIP_HILL_0_0    ( 200 ) // left   earth
#define N_CHIP_HILL_1_0    ( 201 ) // middle earth
#define N_CHIP_HILL_2_0    ( 202 ) // right  earth
#define N_CHIP_HILL_1_1    ( 203 ) // left   wall edge
#define N_CHIP_HILL_1_2    ( 204 ) // middle wall
#define N_CHIP_HILL_1_3    ( 205 ) // right  wall edge
#define N_CHIP_HILL_0_0_SP ( 206 ) // left  earth covered by another hill
#define N_CHIP_HILL_2_0_SP ( 207 ) // right earth covered by another hill

#define N_CHIP_LAND_0_0    ( 210 ) // left   earth
#define N_CHIP_LAND_1_0    ( 211 ) // middle earth
#define N_CHIP_LAND_2_0    ( 212 ) // right  earth
#define N_CHIP_LAND_1_1    ( 213 ) // left   wall edge
#define N_CHIP_LAND_1_2    ( 214 ) // middle wall
#define N_CHIP_LAND_1_3    ( 215 ) // right  wall edge

#define N_CHIP_DEBUG       ( 255 )




// internal
void
n_chip_new( n_nn2_stage *s, n_type_gfx sx, n_type_gfx sy )
{

	int byte = sx * sy * sizeof( n_sprite );

	n_memory_free( s->chip_map );
	s->chip_map = n_memory_new( byte );

	n_memory_zero( s->chip_map, byte );


	return;
}

BOOL
n_chip_is_accessible( n_nn2_stage *s, n_type_gfx x, n_type_gfx y )
{
	return n_bmp_ptr_is_accessible( &s->map, x, y );
}

n_sprite*
n_chip_get_raw( n_nn2_stage *s, n_type_gfx x, n_type_gfx y, n_type_gfx sx )
{
	return &s->chip_map[ x + ( y * sx ) ];
}

n_sprite*
n_chip_get( n_nn2 *p, n_type_gfx x, n_type_gfx y, n_type_gfx sx )
{
	return n_chip_get_raw( p->stage, x, y, sx );
}

n_sprite*
n_chip_get_auto( n_nn2 *p, n_type_gfx x, n_type_gfx y )
{
	return n_chip_get_raw( p->stage, x, y, N_BMP_SX( &p->stage->map ) );
}

void
n_chip_init( n_nn2 *p, n_nn2_stage *s )
{

	n_type_gfx msx = N_BMP_SX( &s->map );
	n_type_gfx msy = N_BMP_SY( &s->map );

	n_chip_new( s, msx, msy );
//NSLog( @"%d %d", msx, msy );


	n_type_gfx x = 0;
	n_type_gfx y = 0;
	n_posix_loop
	{

		n_sprite *spr = n_chip_get_raw( s, x,y, msx );
		//n_sprite_zero( spr );

		u32 data; n_bmp_ptr_get( &s->map, x,y, &data );

		if ( N_CHIP_SKY == n_chip_data_kind( data ) )
		{

			int i = 0;
			n_posix_loop
			{

				n_bmp *bmp[ N_SPRITE_MAX ];
				int     ox[ N_SPRITE_MAX ];
				int     oy[ N_SPRITE_MAX ];

				int interval = 0;
				if ( i == 0 )
				{
					int n = 0;
					bmp[ n ] = NULL;
				} else { break; }

				n_sprite_set( spr, i, bmp, ox, oy, 0, interval );

				i++;
			}

		} else
		if ( N_CHIP_EARTH == n_chip_data_kind( data ) )
		{

			int i = 0;
			n_posix_loop
			{

				n_bmp *bmp[ N_SPRITE_MAX ];
				int     ox[ N_SPRITE_MAX ];
				int     oy[ N_SPRITE_MAX ];

				int interval = 0;
				if ( i == 0 )
				{
					int n = 0;
					bmp[ n ] = &s->chip_earth[ 1 ]; ox[ n ] = 0; oy[ n ] = 0; n++;
					bmp[ n ] = NULL;
				} else { break; }

				n_sprite_set( spr, i, bmp, ox, oy, 0, interval );

				i++;
			}

		} else
		if ( N_CHIP_EARTH_L == n_chip_data_kind( data ) )
		{

			int i = 0;
			n_posix_loop
			{

				n_bmp *bmp[ N_SPRITE_MAX ];
				int     ox[ N_SPRITE_MAX ];
				int     oy[ N_SPRITE_MAX ];

				int interval = 0;
				if ( i == 0 )
				{
					int n = 0;
					bmp[ n ] = &s->chip_earth[ 0 ]; ox[ n ] = 0; oy[ n ] = 0; n++;
					bmp[ n ] = NULL;
				} else { break; }

				n_sprite_set( spr, i, bmp, ox, oy, 0, interval );

				i++;
			}

		} else
		if ( N_CHIP_EARTH_R == n_chip_data_kind( data ) )
		{

			int i = 0;
			n_posix_loop
			{

				n_bmp *bmp[ N_SPRITE_MAX ];
				int     ox[ N_SPRITE_MAX ];
				int     oy[ N_SPRITE_MAX ];

				int interval = 0;
				if ( i == 0 )
				{
					int n = 0;
					bmp[ n ] = &s->chip_earth[ 2 ]; ox[ n ] = 0; oy[ n ] = 0; n++;
					bmp[ n ] = NULL;
				} else { break; }

				n_sprite_set( spr, i, bmp, ox, oy, 0, interval );

				i++;
			}

		} else
		if ( N_CHIP_BLOCK == n_chip_data_kind( data ) )
		{

			int i = 0;
			n_posix_loop
			{

				n_bmp *bmp[ N_SPRITE_MAX ];
				int     ox[ N_SPRITE_MAX ];
				int     oy[ N_SPRITE_MAX ];

				int interval = 0;
				if ( i == 0 )
				{
					int n = 0;
					bmp[ n ] = &s->chip_block; ox[ n ] = 0; oy[ n ] = 0; n++;
					bmp[ n ] = NULL;
				} else { break; }

				n_sprite_set( spr, i, bmp, ox, oy, 0, interval );

				i++;
			}

		} else
		if ( N_CHIP_BLOCK_INV == n_chip_data_kind( data ) )
		{

			int i = 0;
			n_posix_loop
			{

				n_bmp *bmp[ N_SPRITE_MAX ];
				int     ox[ N_SPRITE_MAX ];
				int     oy[ N_SPRITE_MAX ];

				int interval = 0;
				if ( i == 0 )
				{
					int n = 0;
					bmp[ n ] = NULL;
				} else { break; }

				n_sprite_set( spr, i, bmp, ox, oy, 0, interval );

				i++;
			}

		} else
		if ( N_CHIP_BRICK == n_chip_data_kind( data ) )
		{

			int i = 0;
			n_posix_loop
			{

				n_bmp *bmp[ N_SPRITE_MAX ];
				int     ox[ N_SPRITE_MAX ];
				int     oy[ N_SPRITE_MAX ];

				int interval = 2;
				if ( i == 0 )
				{
					interval = 33;

					int n = 0;
					bmp[ n ] = &s->chip_brick_1; ox[ n ] = 0; oy[ n ] = 0; n++;
					bmp[ n ] = NULL;
				} else
				if ( i == 1 )
				{
					int n = 0;
					bmp[ n ] = &s->chip_brick_2; ox[ n ] = 0; oy[ n ] = 0; n++;
					bmp[ n ] = NULL;
				} else
				if ( i == 2 )
				{
					int n = 0;
					bmp[ n ] = &s->chip_brick_3; ox[ n ] = 0; oy[ n ] = 0; n++;
					bmp[ n ] = NULL;
				} else
				if ( i == 3 )
				{
					int n = 0;
					bmp[ n ] = &s->chip_brick_2; ox[ n ] = 0; oy[ n ] = 0; n++;
					bmp[ n ] = NULL;
				} else { break; }

				n_sprite_set( spr, i, bmp, ox, oy, 0, interval );

				if ( p->brick_animation_sprite == NULL ) { p->brick_animation_sprite = spr; }

				i++;
			}

		} else
		if ( N_CHIP_BELL == n_chip_data_kind( data ) )
		{

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
					bmp[ n ] = &p->bell_bmp_0; ox[ n ] = 0; oy[ n ] =-1; n++;
					bmp[ n ] = NULL;
				} else
				if ( i == 1 )
				{
					int n = 0;
					bmp[ n ] = &p->bell_bmp_1; ox[ n ] = 1; oy[ n ] =-1; n++;
					bmp[ n ] = NULL;
				} else
				if ( i == 2 )
				{
					int n = 0;
					bmp[ n ] = &p->bell_bmp_2; ox[ n ] = 2; oy[ n ] = 0; n++;
					bmp[ n ] = NULL;
				} else
				if ( i == 3 )
				{
					int n = 0;
					bmp[ n ] = &p->bell_bmp_1; ox[ n ] = 1; oy[ n ] =-1; n++;
					bmp[ n ] = NULL;
				} else
				//
				{
					break;
				}

				n_sprite_set( spr, i, bmp, ox, oy, 0, interval );

				i++;
			}

		} else
		if ( N_CHIP_PASS_THRU == n_chip_data_kind( data ) )
		{

			int i = 0;
			n_posix_loop
			{//break;

				n_bmp *bmp[ N_SPRITE_MAX ];
				int     ox[ N_SPRITE_MAX ];
				int     oy[ N_SPRITE_MAX ];

				int interval = 0;
				if ( i == 0 )
				{
					int n = 0;
					bmp[ n ] = &p->pass_thru_bmp;
				} else
				if ( i == 1 )
				{
					int n = 0;
					bmp[ n ] = NULL;
				} else
				//
				{
					break;
				}

				n_sprite_set( spr, i, bmp, ox, oy, 0, interval );

				i++;
			}

		} else
		if ( N_CHIP_SPECIAL == n_chip_data_kind( data ) )
		{

			int i = 0;
			n_posix_loop
			{

				n_bmp *bmp[ N_SPRITE_MAX ];
				int     ox[ N_SPRITE_MAX ];
				int     oy[ N_SPRITE_MAX ];

				int interval = 0;
				if ( i == 0 )
				{
					int n = 0;
					bmp[ n ] = &s->chip_special_lite; ox[ n ] = 0; oy[ n ] = 0; n++;
					bmp[ n ] = NULL;
				} else
				//
				{
					break;
				}

				n_sprite_set( spr, i, bmp, ox, oy, 0, interval );

				i++;
			}

		} else
		if ( N_CHIP_WEATHER == n_chip_data_kind( data ) )
		{

			int i = 0;
			n_posix_loop
			{

				n_bmp *bmp[ N_SPRITE_MAX ];
				int     ox[ N_SPRITE_MAX ];
				int     oy[ N_SPRITE_MAX ];

				int interval = 0;
				if ( i == 0 )
				{
					int n = 0;
					bmp[ n ] = &p->weather_bmp_lite; ox[ n ] = 0; oy[ n ] = 0; n++;
					bmp[ n ] = NULL;
				} else
				//
				{
					break;
				}

				n_sprite_set( spr, i, bmp, ox, oy, 0, interval );

				if ( p->weather_sprite == NULL ) { p->weather_sprite = spr; }

				spr->invisible = TRUE;

				i++;
			}

		} else
		if (
			( N_CHIP_RAINBOW    == n_chip_data_kind( data ) )
			||
			( N_CHIP_RAINBOW_BG == n_chip_data_kind( data ) )
		)
		{

			int i = 0;
			n_posix_loop
			{

				n_bmp *bmp[ N_SPRITE_MAX ];
				int     ox[ N_SPRITE_MAX ];
				int     oy[ N_SPRITE_MAX ];

				int interval = 0;
				if ( i == 0 )
				{
					int n = 0;
					bmp[ n ] = &p->bmp_rainbow; ox[ n ] = 0; oy[ n ] = 0; n++;
					bmp[ n ] = NULL;
				} else
				//
				{
					break;
				}

				n_sprite_set( spr, i, bmp, ox, oy, 0, interval );

				i++;
			}

		} else
		if (
			( N_CHIP_HILL_0_0    <= n_chip_data_kind( data ) )
			&&
			( N_CHIP_HILL_2_0_SP >= n_chip_data_kind( data ) )
		)
		{

			int target = n_chip_data_kind( data ) - 200;

			int i = 0;
			n_posix_loop
			{

				n_bmp *bmp[ N_SPRITE_MAX ];
				int     ox[ N_SPRITE_MAX ];
				int     oy[ N_SPRITE_MAX ];

				int interval = 0;
				if ( i == 0 )
				{
					int n = 0;
					bmp[ n ] = &s->chip_earth[ target ]; ox[ n ] = 0; oy[ n ] = 0; n++;
					bmp[ n ] = NULL;
				} else
				//
				{
					break;
				}

				n_sprite_set( spr, i, bmp, ox, oy, 0, interval );

				i++;
			}

		} else
		if (
			( N_CHIP_LAND_0_0 <= n_chip_data_kind( data ) )
			&&
			( N_CHIP_LAND_1_3 >= n_chip_data_kind( data ) )
		)
		{

			int target = n_chip_data_kind( data ) - 210;

			int i = 0;
			n_posix_loop
			{

				n_bmp *bmp[ N_SPRITE_MAX ];
				int     ox[ N_SPRITE_MAX ];
				int     oy[ N_SPRITE_MAX ];

				int interval = 0;
				if ( i == 0 )
				{
					int n = 0;
					bmp[ n ] = &s->chip_earth[ target ]; ox[ n ] = 0; oy[ n ] = 0; n++;
					bmp[ n ] = NULL;
				} else
				//
				{
					break;
				}

				n_sprite_set( spr, i, bmp, ox, oy, 0, interval );

				i++;
			}

		} else
		if ( N_CHIP_DEBUG == n_chip_data_kind( data ) )
		{

			int i = 0;
			n_posix_loop
			{

				n_bmp *bmp[ N_SPRITE_MAX ];
				int     ox[ N_SPRITE_MAX ];
				int     oy[ N_SPRITE_MAX ];

				int interval = 5;
				if ( i == 0 )
				{	
					int n = 0;
					bmp[ n ] = &s->chip_debug_1; ox[ n ] = 0; oy[ n ] = 0; n++;
					bmp[ n ] = NULL;
				} else
				if ( i == 1 )
				{
					int n = 0;
					bmp[ n ] = &s->chip_debug_2; ox[ n ] = 0; oy[ n ] = 0; n++;
					bmp[ n ] = NULL;
				} else
				if ( i == 2 )
				{
					int n = 0;
					bmp[ n ] = &s->chip_debug_3; ox[ n ] = 0; oy[ n ] = 0; n++;
					bmp[ n ] = NULL;
				} else
				if ( i == 3 )
				{
					int n = 0;
					bmp[ n ] = &s->chip_debug_2; ox[ n ] = 0; oy[ n ] = 0; n++;
					bmp[ n ] = NULL;
				} else
				//
				{
					break;
				}

				n_sprite_set( spr, i, bmp, ox, oy, 0, interval );

				i++;
			}

		}


		x++;
		if ( x >= msx )
		{
			x = 0;

			y++;
			if ( y >= msy ) { break; }
		}
	}


	return;
}

void
n_chip_animation( n_nn2 *p )
{
//return;

	n_type_gfx msx = N_BMP_SX( &p->stage->map );
	n_type_gfx msy = N_BMP_SY( &p->stage->map );

	n_type_gfx x = 0;
	n_type_gfx y = 0;
	n_posix_loop
	{

		n_sprite *spr = n_chip_get( p, x, y, msx );
		if ( spr == NULL ) { break; }

		spr->frame++;
		if ( spr->frame >= spr->obj[ spr->index ].frame_interval )
		{
			spr->frame = 0;

			spr->index++;
			if ( NULL == spr->obj[ spr->index ].bmp[ 0 ] )
			{
				spr->index = 0;
			}
		}


		u32 data; n_bmp_ptr_get( &p->stage->map, x,y, &data );

		if ( N_CHIP_BRICK == n_chip_data_kind( data ) )
		{
			p->brick_animation_index = spr->index;
		}

		if ( n_brick_move_animation( p, x, y ) )
		{
			if ( N_CHIP_WEATHER == n_chip_data_kind( data ) )
			{
				if ( p->weather_transition_ready == FALSE )
				{
					p->weather_transition_ready = TRUE;
					n_nn2_weather_transition( p, p->weather_hour + 6 );
				}
			}
		}

		x++;
		if ( x >= msx )
		{
			x = 0;

			y++;
			if ( y >= msy ) { break; }
		}
	}


	return;
}

BOOL
n_chip_detect_block( n_nn2 *p, u32 data )
{

	BOOL ret = FALSE;

	if (
		( N_CHIP_EARTH      == n_chip_data_kind( data ) )
		||
		( N_CHIP_EARTH_L    == n_chip_data_kind( data ) )
		||
		( N_CHIP_EARTH_R    == n_chip_data_kind( data ) )
		||
		( N_CHIP_BLOCK      == n_chip_data_kind( data ) )
		||
		( N_CHIP_BLOCK_INV  == n_chip_data_kind( data ) )
		||
		( N_CHIP_BRICK      == n_chip_data_kind( data ) )
		||
		( N_CHIP_RAINBOW    == n_chip_data_kind( data ) )
		||
		( N_CHIP_TREASURE_X == n_chip_data_kind( data ) )
		||
		(
			( p->weather_sprite != NULL )
			&&
			( p->weather_sprite->invisible == FALSE )
			&&
			( N_CHIP_WEATHER == n_chip_data_kind( data ) )
		)
		||
		(
			( N_CHIP_LAND_0_0 <= n_chip_data_kind( data ) )
			&&
			( N_CHIP_LAND_1_3 >= n_chip_data_kind( data ) )
		)
	)
	{
		ret = TRUE;
	} else
	if ( N_CHIP_TREASURE_SL == n_chip_data_kind( data ) )
	{
		p->stage->nina_x -= N_NN2_STEP_FORCE;
		ret = TRUE;
	} else
	if ( N_CHIP_TREASURE_SR == n_chip_data_kind( data ) )
	{
		p->stage->nina_x += N_NN2_STEP_FORCE;
		ret = TRUE;
	}


	return ret;
}

BOOL
n_chip_detect_pass_thru( n_nn2 *p, u32 data )
{

	if ( N_CHIP_PASS_THRU == n_chip_data_kind( data ) )
	{
		return TRUE;
	}


	return FALSE;
}

BOOL
n_chip_detect_hill( n_nn2 *p, u32 data )
{

	if (
		( N_CHIP_HILL_0_0    == n_chip_data_kind( data ) )
		||
		( N_CHIP_HILL_1_0    == n_chip_data_kind( data ) )
		||
		( N_CHIP_HILL_2_0    == n_chip_data_kind( data ) )
		||
		( N_CHIP_HILL_0_0_SP == n_chip_data_kind( data ) )
		||
		( N_CHIP_HILL_2_0_SP == n_chip_data_kind( data ) )
	)
	{
		return TRUE;
	}


	return FALSE;
}

BOOL
n_chip_detect_special( n_nn2 *p, u32 data )
{

	if ( N_CHIP_SPECIAL == n_chip_data_kind( data ) )
	{
		return TRUE;
	}


	return FALSE;
}

BOOL
n_chip_detect_hidden( n_nn2 *p, u32 data )
{

	if ( N_CHIP_HIDDEN == n_chip_data_kind( data ) )
	{
		return TRUE;
	}


	return FALSE;
}

void
n_chip_hidden_show( n_nn2 *p, n_type_gfx x, n_type_gfx y )
{

	n_nn2_stage *s = p->stage;


	u32 d; n_bmp_ptr_get( &p->stage->map, x, y, &d );

	int a = n_bmp_a_mac( d );
	int r = n_bmp_r_mac( d );
	int g = n_bmp_g_mac( d );
	int b = N_CHIP_BRICK;//n_bmp_b_mac( d );

	d = n_bmp_argb_mac( a,r,g,b );

	n_bmp_ptr_set( &p->stage->map, x, y,  d );


	int i = 0;
	n_posix_loop
	{

		n_bmp *bmp[ N_SPRITE_MAX ];
		int     ox[ N_SPRITE_MAX ];
		int     oy[ N_SPRITE_MAX ];

		int interval = 2;
		if ( i == 0 )
		{
			interval = 33;

			int n = 0;
			bmp[ n ] = &s->chip_brick_1; ox[ n ] = 0; oy[ n ] = 0; n++;
			bmp[ n ] = NULL;
		} else
		if ( i == 1 )
		{
			int n = 0;
			bmp[ n ] = &s->chip_brick_2; ox[ n ] = 0; oy[ n ] = 0; n++;
			bmp[ n ] = NULL;
		} else
		if ( i == 2 )
		{
			int n = 0;
			bmp[ n ] = &s->chip_brick_3; ox[ n ] = 0; oy[ n ] = 0; n++;
			bmp[ n ] = NULL;
		} else
		if ( i == 3 )
		{
			int n = 0;
			bmp[ n ] = &s->chip_brick_2; ox[ n ] = 0; oy[ n ] = 0; n++;
			bmp[ n ] = NULL;
		} else { break; }

		n_sprite *spr = n_chip_get_auto( p, x, y );

		n_sprite_set( spr, i, bmp, ox, oy, 0, interval );

		spr->frame = p->brick_animation_sprite->frame;
		spr->index = p->brick_animation_sprite->index;


		i++;
	}


	return;
}

BOOL
n_chip_detect( n_nn2 *p, n_type_gfx tx, n_type_gfx ty )
{

	u32 data = 0; n_bmp_ptr_get( &p->stage->map, tx, ty, &data );

	if ( n_chip_detect_block( p, data ) )
	{
		return TRUE;
	} else
	if ( n_chip_detect_special( p, data ) )
	{
		if ( n_object_birds_is_caught( p ) )
		{
			return TRUE;
		}
	}


	return FALSE;
}

BOOL
n_chip_detect_slip_single( n_nn2 *p, n_type_gfx tx, n_type_gfx ty )
{

	BOOL ret = FALSE;

	u32 data = 0; n_bmp_ptr_get( &p->stage->map, tx, ty, &data );

	if (
		( N_CHIP_BLOCK      == n_chip_data_kind( data ) )
		||
		( N_CHIP_BRICK      == n_chip_data_kind( data ) )
		||
		( N_CHIP_RAINBOW    == n_chip_data_kind( data ) )
	)
	{
		ret = TRUE;
	}


	return ret;
}

BOOL
n_chip_detect_slip( n_nn2 *p, n_type_gfx tx, n_type_gfx ty )
{
//return FALSE;


	int i = 0;

	if ( n_chip_detect_slip_single( p, tx, ty-2 ) ) { i++; }
	if ( n_chip_detect_slip_single( p, tx, ty-1 ) ) { i++; }
	if ( n_chip_detect_slip_single( p, tx, ty-0 ) ) { i++; }
	if ( n_chip_detect_slip_single( p, tx, ty+1 ) ) { i++; }
	if ( n_chip_detect_slip_single( p, tx, ty+2 ) ) { i++; }


	return ( i == 5 );
}

BOOL
n_chip_dokan_label_get( n_nn2 *p, n_type_gfx mx, n_type_gfx my )
{
//return TRUE;

	u32 d1 = n_bmp_black; n_bmp_ptr_get( &p->stage->map_label, mx+1, my, &d1 );
	u32 d2 = n_bmp_black; n_bmp_ptr_get( &p->stage->map_label, mx+2, my, &d2 );
//NSLog( @"%x %x", d1, d2 );

//n_bmp_ptr_set( &p->stage->map_label, mx+1, my, n_bmp_rgb_mac( 255,0,0 ) );
//n_bmp_ptr_set( &p->stage->map_label, mx+2, my, n_bmp_rgb_mac( 255,0,0 ) );

//n_nn2_debug_bmp_save( p, &p->stage->map_label );

	return ( ( d1 == n_bmp_white )||( d2 == n_bmp_white ) );
}

void
n_chip_dokan_label_set( n_nn2 *p, n_type_gfx mx, n_type_gfx my )
{
//return;

	n_bmp_ptr_set( &p->stage->map_label, mx+0, my, n_bmp_white );
	n_bmp_ptr_set( &p->stage->map_label, mx+1, my, n_bmp_white );

//n_nn2_debug_bmp_save( p, &p->stage->map_label );
//n_nn2_debug_bmp_save( p, &p->stage->map );

	return;
}

void
n_chip_draw_dokan_single( n_nn2 *p, n_bmp *bmp, n_type_gfx x, n_type_gfx y )
{

	n_type_gfx sx = N_BMP_SX( bmp );
	n_type_gfx sy = N_BMP_SY( bmp );

	n_bmp b; n_nn2_bmp_new( &b, sx, sy );

	n_chara_bmp_copy( p, bmp, &b, 0,0, 0 );


	n_type_gfx cx = ( x * p->mapchip_unit ) - n_camerawork_x( p );
	n_type_gfx cy = ( y * p->mapchip_unit ) - n_camerawork_y( p );

	CGFloat blend = p->global_blend;

	if ( p->dokan_blend_stop == -1 )
	{
		p->global_blend = p->dokan_blend;
	} else {
		p->global_blend = p->dokan_blend_stop;
	}

	n_nn2_bmp_draw( p, &b, cx, cy + 1 );

	p->global_blend = blend;


	n_bmp_free_fast( &b );


	return;
}

void
n_chip_pass_thru_go( n_nn2 *p, n_type_gfx mx, n_type_gfx my )
{
//return;

	n_bmp *map = &p->stage->map_pass_thru;


	u32 d = 0; n_bmp_ptr_get( map, mx, my, &d );

	u32 e = d;

	//static u32 timer = 0;
	//if ( n_bmp_ui_timer( &timer, 33 ) )
	{
		d += 4;
		if ( d > 255 ) { d = 255; }
	}

	if ( e != d )
	{
		n_bmp_ptr_set( map, mx, my, d );
	}


	return;
}

CGFloat
n_chip_pass_thru_blend( n_nn2 *p, n_type_gfx mx, n_type_gfx my )
{

	n_bmp *map = &p->stage->map_pass_thru;


	u32 d = 0; n_bmp_ptr_get( map, mx, my, &d );

	u32 e = d;

	if ( d != 0 )
	{
		d += 4;
		if ( d > 255 ) { d = 255; }
	}

	if ( e != d )
	{
		n_bmp_ptr_set( map, mx, my, d );
	}


	return 1.0 - ( (CGFloat) d / 255 );
}

void
n_chip_pass_thru_fade_out( n_nn2 *p )
{
return;

	// [x] : hard to implement


	n_bmp *map = &p->stage->map_pass_thru;


	n_type_gfx tx = p->stage->nina_x + ( p->nina_sx / 2 );
	n_type_gfx ty = p->stage->nina_y + p->nina_sy - p->nina_margin_foot;

	tx = tx / p->mapchip_unit;
	ty = ty / p->mapchip_unit;


	n_type_gfx mx = 0;
	n_type_gfx my = 0;
	n_posix_loop
	{

		u32 data_midl = 0; n_bmp_ptr_get( &p->stage->map, tx, ty, &data_midl );
		if ( FALSE == n_chip_detect_pass_thru( p, data_midl ) )
		{
			u32 d = 0; n_bmp_ptr_get( map, mx, my, &d );

			if ( d > 8 ) { d -= 8; }

			n_bmp_ptr_set( map, mx, my, d );
		}

		mx++;
		if ( mx >= N_BMP_SX( map ) )
		{
			mx = 0;
			my++;
			if ( my >= N_BMP_SY( map ) ) { break; }
		}
	}


	return;
}

void
n_chip_draw_dokan( n_nn2 *p )
{
//return;

	n_type_gfx msx = N_BMP_SX( &p->stage->map );
	n_type_gfx msy = N_BMP_SY( &p->stage->map );

	n_type_gfx x = 0;
	n_type_gfx y = 0;
	n_posix_loop
	{

		u32 data = 0; n_bmp_ptr_get( &p->stage->map_dokan, x, y, &data );

		if ( N_CHIP_DOKAN_T == n_chip_data_kind( data ) )
		{
			n_chip_draw_dokan_single( p, &p->dokan_bmp_top, x, y );

			if ( n_chip_dokan_label_get( p, x, y ) )
			{
				int stage_number = n_nn2_map_dokan_stage_number_get_by_position( p, x+1, y );
//NSLog( @"%d", stage_number );
//CGFloat u = p->mapchip_unit; p->debug_rect = NSMakeRect( (x+1)*u, y*u, u, u ); n_nn2_debug_rect( p );

				if ( p->stage == &n_nn2_stage_1 )
				{
					stage_number = N_NN2_STAGE_MAX;
				}

				n_chip_draw_dokan_single( p, &p->dokan_label[ stage_number ], x, y );
			}
		} else
		if ( N_CHIP_DOKAN_B == n_chip_data_kind( data ) )
		{
			n_chip_draw_dokan_single( p, &p->dokan_bmp_body, x, y );
		}


		x++;
		if ( x >= msx )
		{
			x = 0;

			y++;
			if ( y >= msy ) { break; }
		}
	}


	return;
}

void
n_chip_draw_treasure( n_nn2 *p )
{
//return;

	if ( p->stage == &n_nn2_stage_6 )
	{
		//
	} else
	if ( p->stage == &n_nn2_stage_8 )
	{
		//
	} else
	//
	{
		return;
	}


	n_type_gfx msx = N_BMP_SX( &p->stage->map );
	n_type_gfx msy = N_BMP_SY( &p->stage->map );

	if ( p->stage->treasure_state != 0 )
	{
		n_type_gfx x = 0;
		n_type_gfx y = 0;
		n_posix_loop
		{

			u32 data = 0; n_bmp_ptr_get( &p->stage->map_dokan, x, y, &data );

			if ( N_CHIP_TREASURE_L == n_chip_data_kind( data ) )
			{
				extern void n_object_treasure_draw_heart( n_nn2 *p );
				n_object_treasure_draw_heart( p );
				break;
			}

			x++;
			if ( x >= msx )
			{
				x = 0;

				y++;
				if ( y >= msy ) { break; }
			}
		}
	}


	n_type_gfx x = 0;
	n_type_gfx y = 0;
	n_posix_loop
	{

		u32 data = 0; n_bmp_ptr_get( &p->stage->map_dokan, x, y, &data );

		if ( N_CHIP_TREASURE_B == n_chip_data_kind( data ) )
		{
			p->stage->treasure_x = x;
			p->stage->treasure_y = y - 4;
		} else
		if ( N_CHIP_TREASURE_L == n_chip_data_kind( data ) )
		{
			extern void n_object_treasure_draw( n_nn2 *p );
			n_object_treasure_draw( p );
		}

		x++;
		if ( x >= msx )
		{
			x = 0;

			y++;
			if ( y >= msy ) { break; }
		}
	}


	return;
}

u32
n_chip_guidline_color( n_nn2 *p )
{

	u32 line_color_grad = p->weather_gradient_color;
	u32 line_color_main;

	if ( p->weather_darkmode_onoff )
	{
		line_color_main = n_bmp_black;
	} else {
		line_color_main = n_bmp_white;
	}

	line_color_grad = n_bmp_blend_pixel( line_color_main, line_color_grad, 0.5 );


	return line_color_grad;
}

void
n_chip_draw( n_nn2 *p )
{

	n_type_gfx camera_x = n_camerawork_x( p );
	n_type_gfx camera_y = n_camerawork_y( p );


	//n_bmp_flush_fastcopy( &p->stage->canvas_background, p->canvas );
/*
	n_bmp_fastcopy
	(
		&p->stage->canvas_background,
		 p->canvas,
		 camera_x, camera_y, p->sx, p->sy,
		 0, 0
	);
*/
	n_bmp_fastcopy
	(
		&p->stage->canvas_background,
		 p->canvas,
		 0, camera_y, p->sx, p->sy,
		 0, 0
	);


	extern void n_object_kirakira_draw( n_nn2 *p );
	n_object_kirakira_draw( p );


	if ( p->stage->middleground_onoff )
	{
		n_type_gfx sx = N_BMP_SX( &p->stage->canvas_middleground );
		n_type_gfx sy = N_BMP_SY( &p->stage->canvas_middleground );

		if ( p->stage->camera_mode == N_CAMERAWORK_LOOP )
		{

			BOOL prv = n_bmp_is_multithread;
			n_bmp_is_multithread = TRUE;

			n_type_real blend = 0.5;
			if ( p->stage != &n_nn2_stage_7 ) { blend = 0.0; }

			{
				NSOperation *o = [NSBlockOperation blockOperationWithBlock:^{

				n_type_gfx x = camera_x;
				n_type_gfx y = camera_y;

				if ( p->stage != &n_nn2_stage_1 )
				{
					x /= 2;
				}

				n_bmp_blendcopy( &p->stage->canvas_middleground, p->canvas, x, y, sx, sy, 0, 0, blend );

				}];
				[p->queue addOperation:o];
			}

			{
				NSOperation *o = [NSBlockOperation blockOperationWithBlock:^{

				n_type_gfx x = ( camera_x + p->stage->map_sx );
				n_type_gfx y = camera_y;

				if ( p->stage != &n_nn2_stage_1 )
				{
					x /= 2;
				}

				n_bmp_blendcopy( &p->stage->canvas_middleground, p->canvas, x, y, sx, sy, 0, 0, blend );

				}];
				[p->queue addOperation:o];
			}

			{
				NSOperation *o = [NSBlockOperation blockOperationWithBlock:^{

				n_type_gfx x = ( camera_x - p->stage->map_sx );
				n_type_gfx y = camera_y;

				if ( p->stage != &n_nn2_stage_1 )
				{
					x /= 2;
				}

				n_bmp_blendcopy( &p->stage->canvas_middleground, p->canvas, x, y, sx, sy, 0, 0, blend );

				}];
				[p->queue addOperation:o];
			}


			[p->queue waitUntilAllOperationsAreFinished];

			n_bmp_is_multithread = prv;

		} else {

			n_type_gfx x = camera_x;
			n_type_gfx y = camera_y;

			if ( p->stage != &n_nn2_stage_1 )
			{
				x /= 2;
			}

			n_bmp_blendcopy( &p->stage->canvas_middleground, p->canvas, x, y, sx, sy, 0, 0, 0.5 );

		}
	}


	BOOL prv = n_bmp_is_multithread;
	n_bmp_is_multithread = TRUE;


	n_type_gfx msx = N_BMP_SX( &p->stage->map );
	n_type_gfx msy = N_BMP_SY( &p->stage->map );

	n_type_gfx fx = camera_x / p->mapchip_unit;
	n_type_gfx tx = fx + ( p->sx / p->mapchip_unit ) + ( 0 != ( p->sx % p->mapchip_unit ) ); if ( tx < msx ) { tx++; }

	n_type_gfx fy = camera_y / p->mapchip_unit;
	n_type_gfx ty = fy + ( p->sy / p->mapchip_unit ) + ( 0 != ( p->sy % p->mapchip_unit ) ); if ( ty < msy ) { ty++; }

	if ( p->stage->camera_mode == N_CAMERAWORK_LOOP ) { fx -= 5; tx += 5; }

	int i = 0;
	n_posix_loop
	{
//break;

		NSOperation *o = [NSBlockOperation blockOperationWithBlock:^{

		n_type_gfx x = fx - 1;
		n_type_gfx y = ty - i;
		n_posix_loop
		{
			n_sprite *spr = NULL;

			n_type_gfx lx = x;
			if ( p->stage->camera_mode == N_CAMERAWORK_LOOP )
			{
				if ( x <    0 ) { lx = x + msx; } else
				if ( x >= msx ) { lx = x - msx; }
			}

			if ( n_chip_is_accessible( p->stage, lx, y ) )
			{
				spr = n_chip_get( p, lx, y, msx );
			}


			u32 data = 0; n_bmp_ptr_get( &p->stage->map_lift_guideline, lx, y, &data );
			if ( 255 != n_bmp_b_mac( data ) )
			{
				n_type_gfx xx = x * p->mapchip_unit;
				n_type_gfx yy = y * p->mapchip_unit;

				n_type_gfx cx = xx - camera_x;
				n_type_gfx cy = yy - camera_y;

				cy -= p->stage->camera_oy;

/*
				u32 line_color_grad = p->weather_gradient_color;
				u32 line_color_main;

				if ( p->weather_darkmode_onoff )
				{
					line_color_main = n_bmp_black;
				} else {
					line_color_main = n_bmp_white;
				}

				line_color_grad = n_bmp_blend_pixel( line_color_main, line_color_grad, 0.5 );
*/
				u32 line_color_grad = n_chip_guidline_color( p );


				const n_type_gfx line_sz = 2;

				if ( 1 == n_bmp_b_mac( data ) )
				{
					n_bmp_box( p->canvas, cx+p->mapchip_unit,cy,line_sz,p->mapchip_unit, line_color_grad );
				} else
				if ( 2 == n_bmp_b_mac( data ) )
				{
					n_bmp_box( p->canvas, cx,cy+p->mapchip_unit,p->mapchip_unit,line_sz, line_color_grad );
				}
			}


			n_bmp *bmp = NULL;

			n_type_gfx ox = 0;
			n_type_gfx oy = 0;

			CGFloat blend = 0.0;

			if ( spr == NULL )
			{
				//
			} else
			if ( spr->invisible )
			{
				//
			} else
			if ( spr->override_bmp != NULL )
			{
				bmp = spr->override_bmp; spr->override_bmp = NULL;
			} else
			//
			{
				n_sprite_single *obj = &spr->obj[ spr->index ];

				bmp = obj->bmp[ 0 ];
				 ox = obj-> ox[ 0 ] + spr->override_x;
				 oy = obj-> oy[ 0 ] + spr->override_y;

				if ( bmp == &p->stage->chip_special_lite )
				{
					if ( p->weather_darkmode_onoff )
					{
						bmp = &p->stage->chip_special_dark;
					}

					if ( FALSE == n_object_birds_is_caught( p ) )
					{
						bmp = NULL;
					}
					blend = p->chip_blend;
				} else
				if ( bmp == &p->weather_bmp_lite )
				{
					if ( p->weather_darkmode_onoff )
					{
						bmp = &p->weather_bmp_dark;
					}
					blend = p->chip_blend;
				} else
				if ( bmp == &p->stage->chip_brick_1 )
				{
					p->brick_animation_sprite = spr;
					blend = p->chip_blend;
				} else
				if ( bmp == &p->pass_thru_bmp )
				{
					blend = n_chip_pass_thru_blend( p, lx, y );
				} else
				//
				{
					blend = p->chip_blend;
				}
			}

			if ( p->bell_onoff == FALSE )
			{
				if ( bmp == &p->bell_bmp_0 ) { bmp = NULL; } else
				if ( bmp == &p->bell_bmp_1 ) { bmp = NULL; } else
				if ( bmp == &p->bell_bmp_2 ) { bmp = NULL; }//
			}
/*
#ifdef DEBUG
			if (
				( p->debug_box_x != -1 )
				&&
				( p->debug_box_y != -1 )
			)
			{
				n_type_gfx xx = p->debug_box_x * p->mapchip_unit;
				n_type_gfx yy = p->debug_box_y * p->mapchip_unit;

				n_type_gfx cx = ox + xx - camera_x;
				n_type_gfx cy = oy + yy - camera_y;

				cy -= p->stage->camera_oy;

				n_bmp_box( p->canvas, cx,cy, p->mapchip_unit, p->mapchip_unit, n_bmp_rgb_mac( 255,0,0 ) );

				p->debug_box_x = p->debug_box_y = -1;
			} else
#endif
*/
			if ( bmp != NULL )
			{
				n_type_gfx xx = x * p->mapchip_unit;
				n_type_gfx yy = y * p->mapchip_unit;

				n_type_gfx cx = ox + xx - camera_x;
				n_type_gfx cy = oy + yy - camera_y;

				cy -= p->stage->camera_oy;

				if  ( ( p->bounce_sprite_l == spr )||( p->bounce_sprite_m == spr )||( p->bounce_sprite_r == spr ) )
				{
					cy += spr->bounce_offset;
				}

				n_bmp_blendcopy( bmp, p->canvas, 0,0,p->mapchip_unit,p->mapchip_unit, cx,cy, blend );
			}

			x++;
			if ( x >= tx )
			{
				x = fx;

				y -= p->cores;
				if ( y < fy ) { break; }
			}
		}


		}];

		[p->queue addOperation:o];


		i++;
		if ( i >= p->cores ) { break; }
	}


	[p->queue waitUntilAllOperationsAreFinished];

	n_bmp_is_multithread = prv;


#ifdef DEBUG

//n_bmp_box( p->canvas, 400, 400, 50, 50, n_bmp_rgb_mac( 0,200,255 ) );

	n_bmp_transcopy
	(
		&p->stage->canvas_debug,
		 p->canvas,
		 camera_x, camera_y, p->sx, p->sy,
		 0, 0
	);

#endif


	return;
}

void
n_chip_debug_draw( n_nn2 *p )
{
//NSLog( @"n_chip_debug_draw()" );

	n_type_gfx camera_x = n_camerawork_x( p );
	n_type_gfx camera_y = n_camerawork_y( p );


	n_type_gfx msx = N_BMP_SX( &p->stage->map );
	n_type_gfx msy = N_BMP_SY( &p->stage->map );

	n_type_gfx fx = camera_x / p->mapchip_unit;
	n_type_gfx tx = fx + ( p->sx / p->mapchip_unit ) + ( 0 != ( p->sx % p->mapchip_unit ) ); if ( tx < msx ) { tx++; }

	n_type_gfx fy = camera_y / p->mapchip_unit;
	n_type_gfx ty = fy + ( p->sy / p->mapchip_unit ) + ( 0 != ( p->sy % p->mapchip_unit ) ); if ( ty < msy ) { ty++; }

	if ( p->stage->camera_mode == N_CAMERAWORK_LOOP ) { fx -= 5; tx += 5; }


	n_bmp b; n_bmp_zero( &b ); n_bmp_new_fast( &b, p->mapchip_unit, p->mapchip_unit );
	n_bmp_flush( &b, n_bmp_rgb_mac( 255,0,0 ) );


	int i = 0;
	n_posix_loop
	{
//break;

		n_type_gfx x = fx - 1;
		n_type_gfx y = ty - i;
		n_posix_loop
		{

			{
				n_type_gfx xx = x * p->mapchip_unit;
				n_type_gfx yy = y * p->mapchip_unit;

				n_type_gfx cx = xx - camera_x;
				n_type_gfx cy = yy - camera_y;

				cy -= p->stage->camera_oy;

				if ( ( x == p->debug_chip_x )&&( y == p->debug_chip_y ) )
				{
					n_bmp_fastcopy( &b, p->canvas, 0,0,N_BMP_SX( &b ),N_BMP_SY( &b ), cx,cy );
				}
			}

			x++;
			if ( x >= tx )
			{
				x = fx;

				y -= p->cores;
				if ( y < fy ) { break; }
			}
		}

		i++;
		if ( i >= p->cores ) { break; }
	}


	n_bmp_free_fast( &b );


	return;
}

