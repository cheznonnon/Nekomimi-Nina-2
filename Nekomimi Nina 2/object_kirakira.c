// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




void
n_object_kirakira_bmp( n_bmp *bmp, n_type_gfx sx, n_type_gfx sy, u32 bg, u32 fg )
{

	n_bmp_new_fast( bmp, sx, sy ); n_bmp_flush( bmp, fg );

	n_bmp_squircle( bmp, 0, 0, sx * 2, sy * 2, bg, 6 );


	n_type_gfx half = sx / 2;

	n_bmp ul; n_bmp_zero( &ul ); n_bmp_new_fast( &ul, half, half );
	n_bmp_fastcopy( bmp, &ul, 0,0,half,half, 0,0 );
	n_bmp_rotate( &ul, N_BMP_ROTATE_LEFT );
	n_bmp_rotate( &ul, N_BMP_ROTATE_LEFT );

	n_bmp ur; n_bmp_zero( &ur ); n_bmp_new_fast( &ur, half, half );
	n_bmp_fastcopy( bmp, &ur, 0,0,half,half, 0,0 );
	n_bmp_rotate( &ur, N_BMP_ROTATE_LEFT );

	n_bmp dl; n_bmp_zero( &dl ); n_bmp_new_fast( &dl, half, half );
	n_bmp_fastcopy( bmp, &dl, 0,0,half,half, 0,0 );
	n_bmp_rotate( &dl, N_BMP_ROTATE_RIGHT );

	n_bmp dr; n_bmp_zero( &dr ); n_bmp_new_fast( &dr, half, half );
	n_bmp_fastcopy( bmp, &dr, 0,0,half,half, 0,0 );


	n_bmp_fastcopy( &ul, bmp, 0,0,half,half,    0,   0 );
	n_bmp_fastcopy( &ur, bmp, 0,0,half,half, half,   0 );
	n_bmp_fastcopy( &dl, bmp, 0,0,half,half,    0,half );
	n_bmp_fastcopy( &dr, bmp, 0,0,half,half, half,half );


	n_bmp_free_fast( &ul );
	n_bmp_free_fast( &ur );
	n_bmp_free_fast( &dl );
	n_bmp_free_fast( &dr );


	return;
}

// internal
void
n_object_kirakira_init_single( n_nn2 *p, n_sprite *s )
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
			interval = n_game_random( 100 );

			int n = 0;
			bmp[ n ] = &p->kirakira_bmp_n; ox[ n ] = 0; oy[ n ] = 0; n++;
			bmp[ n ] = NULL;
		} else
		if ( i == 1 )
		{
			int n = 0;
			bmp[ n ] = &p->kirakira_bmp_1; ox[ n ] = 0; oy[ n ] = 0; n++;
			bmp[ n ] = NULL;
		} else
		if ( i == 2 )
		{
			int n = 0;
			bmp[ n ] = &p->kirakira_bmp_2; ox[ n ] = 0; oy[ n ] = 0; n++;
			bmp[ n ] = NULL;
		} else
		if ( i == 3 )
		{
			int n = 0;
			bmp[ n ] = &p->kirakira_bmp_1; ox[ n ] = 0; oy[ n ] = 0; n++;
			bmp[ n ] = NULL;
		} else
		//
		{
			break;
		}

		n_sprite_set( s, i, bmp, ox, oy, 0, interval );

		i++;
	}


	return;
}

// internal
void
n_object_kirakira_reset( n_nn2 *p, n_nn2_stage *st )
{

	int i = 0;
	n_posix_loop
	{

		n_sprite *s = &p->kirakira[ i ];

		s->x = n_game_random( st->map_sx );
		s->y = n_game_random( st->map_sy );

		s->sx = N_BMP_SX( s->obj[ 0 ].bmp[ 0 ] );
		s->sy = N_BMP_SY( s->obj[ 0 ].bmp[ 0 ] );

		s->frame = n_game_random( 4 );

		i++;
		if ( i >= N_OBJECT_KIRAKIRA_MAX ) { break; }
	}


	return;
}

// internal
void
n_object_kirakira_init( n_nn2 *p )
{

	n_type_gfx sx = 32;//p->mapchip_unit;
	n_type_gfx sy = 32;//p->mapchip_unit;
//NSLog( @"%d %d", sx, sy );

	n_object_kirakira_bmp( &p->kirakira_bmp_1, sx / 2, sy / 2, n_bmp_white_invisible, n_bmp_white );
	n_object_kirakira_bmp( &p->kirakira_bmp_2, sx    , sy    , n_bmp_white_invisible, n_bmp_white );

	n_bmp_resizer( &p->kirakira_bmp_1, sx, sy, n_bmp_white_invisible, N_BMP_RESIZER_CENTER );


	//n_type_gfx sx = N_BMP_SX( &p->kirakira_bmp_1 );
	//n_type_gfx sy = N_BMP_SY( &p->kirakira_bmp_1 );

	n_nn2_bmp_new( &p->kirakira_bmp_n, sx,sy );


	int i = 0;
	n_posix_loop
	{
		n_sprite *s = &p->kirakira[ i ];

		n_object_kirakira_init_single( p, s );

		i++;
		if ( i >= N_OBJECT_KIRAKIRA_MAX ) { break; }
	}


	return;
}

// internal
void
n_object_kirakira_animation( n_nn2 *p )
{

	int i = 0;
	n_posix_loop
	{

		n_sprite *s = &p->kirakira[ i ];

		n_sprite_animation( s );

		i++;
		if ( i >= N_OBJECT_KIRAKIRA_MAX ) { break; }
	}


	return;
}

// internal
void
n_object_kirakira_draw( n_nn2 *p )
{
//NSLog( @"n_object_kirakira_draw()" );

	// [!] : don't call already-threaded module like n_chara_bmp_copy() here


	if (
		( p->stage != &n_nn2_stage_3 )
		&&
		( p->stage != &n_nn2_stage_6 )
		&&
		( p->stage != &n_nn2_stage_7 )
	)
	{
		if ( p->weather_kirakira_onoff == NO ) { return; }
	}


	n_type_gfx count = N_OBJECT_KIRAKIRA_MAX;
	if ( p->stage == &n_nn2_stage_1 ) { count /= 3; }

	int        cores = p->cores;
	n_type_gfx slice = count / cores;


	n_posix_bool prv = n_bmp_is_multithread;
	n_bmp_is_multithread = n_posix_true;

//n_bmp_box( p->canvas, 400, 400, 50, 50, n_bmp_rgb_mac( 0,200,255 ) );

	int offset = 0;
	n_posix_loop
	{

		NSOperation *o = [NSBlockOperation blockOperationWithBlock:^{

		int n = 0;
		n_posix_loop
		{

			n_sprite *s = &p->kirakira[ ( offset * slice ) + n ];

			n_sprite_single *obj = n_object_get( s );
			n_bmp           *bmp = obj->bmp[ 0 ];

			//if ( bmp == &p->kirakira_bmp_n )
			{
//n_bmp_box( p->canvas, 400, 400, 50, 50, n_bmp_rgb_mac( 0,200,255 ) );
//bmp = &p->kirakira_bmp_1;
			}

			{
				n_type_gfx x = s->x + s->ox - n_camerawork_x( p );
				n_type_gfx y = s->y + s->oy - n_camerawork_y( p );

				n_bmp_transcopy( bmp, p->canvas, 0, 0, N_BMP_SX( bmp ), N_BMP_SY( bmp ), x, y );
			}

			n++;
			if ( n >= slice ) { break; }
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


