// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// [!] : Copilot Optimization Done




extern BOOL n_object_treasure_is_running( n_nn2 *p );




void
n_sprite_cur_debug( n_nn2 *p )
{

	if ( p->sprite_cur == &n_chara_sprite_idle )
	{
		NSLog( @"idle" );
	} else
	if ( p->sprite_cur == &n_chara_sprite_walk )
	{
		NSLog( @"walk" );
	} else
	if ( p->sprite_cur == &n_chara_sprite_dash )
	{
		NSLog( @"dash" );
	} else
	if ( p->sprite_cur == &n_chara_sprite_slip )
	{
		NSLog( @"slip" );
	} else
	if ( p->sprite_cur == &n_chara_sprite_duck )
	{
		NSLog( @"duck" );
	} else
	if ( p->sprite_cur == &n_chara_sprite_unduck )
	{
		NSLog( @"unduck" );
	} else
	if ( p->sprite_cur == &n_chara_sprite_jump )
	{
		NSLog( @"jump" );
	} else
	if ( p->sprite_cur == &n_chara_sprite_land )
	{
		NSLog( @"land" );
	} else
	if ( p->sprite_cur == &n_chara_sprite_suck )
	{
		NSLog( @"suck" );
	} //else


	return;
}




void
n_chara_bmp_copy_all( n_nn2 *p, n_bmp *f, n_bmp *t, n_type_gfx ox, n_type_gfx oy, int direction, BOOL nina_dokan_onoff, n_sprite *object_dokan_sprite )
{

	n_type_gfx bmpsx = N_BMP_SX( f );
	n_type_gfx bmpsy = N_BMP_SY( f );


	if ( nina_dokan_onoff )
	{
//NSLog( @"%d %d %d", bmpsy, oy, p->dokan_disappear_offset );

		bmpsy -= p->dokan_disappear_offset;
	}

	if ( object_dokan_sprite != NULL )
	{
		bmpsy -= p->object_dokan_sprite->dokan_disappear_offset;
		oy    -= p->object_dokan_sprite->margin_sy - 2;
	}


	oy -= p->stage->camera_oy;


	BOOL prv = n_bmp_is_multithread;
	n_bmp_is_multithread = TRUE;

	int i = 0;
	n_posix_loop
	{

		NSOperation *o = [NSBlockOperation blockOperationWithBlock:^{

			/* cache hot fields locally to reduce repeated lookups */
			n_bmp *lf = f;
			n_bmp *lt = t;
			n_type_gfx local_bmpsx = bmpsx;
			n_type_gfx local_bmpsy = bmpsy;
			int      local_cores = p->cores;
			u32      local_blend = p->global_blend;

			n_type_gfx fx = (direction == N_NN2_DIRECTION_RIGHT) ? (local_bmpsx - 1) : 0;
			n_type_gfx fy = i;
			int tx = 0;

			n_posix_loop
			{
				BOOL write_needed = FALSE;

				/* check source and destination pixel accessibility once per pixel */
				if ( ( n_bmp_ptr_is_accessible( lf, fx, fy ) ) && ( n_bmp_ptr_is_accessible( lt, ox + tx, oy + fy ) ) )
				{
					u32 color = n_bmp_composite_pixel_fast(
						lf, lt,
						fx, fy, ox + tx, oy + fy,
						FALSE,
						FALSE,
						TRUE,
						FALSE,
						local_blend,
						&write_needed
					);

					if ( write_needed )
					{
						n_bmp_ptr_set_fast( lt, ox + tx, oy + fy, color );
					}
				}

				if ( direction == N_NN2_DIRECTION_RIGHT )
				{
					if ( fx == 0 )
					{
						fx = local_bmpsx - 1;
						tx = 0;

						fy += local_cores;
						if ( fy >= local_bmpsy ) { break; }
					}
					else
					{
						fx--;
						tx++;
					}
				}
				else
				{
					if ( fx + 1 >= local_bmpsx )
					{
						fx = 0;
						tx = 0;

						fy += local_cores;
						if ( fy >= local_bmpsy ) { break; }
					}
					else
					{
						fx++;
						tx++;
					}
				}
			}

		}];

		[p->queue addOperation:o];


		i++;
		if ( i >= p->cores ) { break; }
	}


	[p->queue waitUntilAllOperationsAreFinished];

	n_bmp_is_multithread = prv;


	return;
}

void
n_chara_bmp_copy( n_nn2 *p, n_bmp *f, n_bmp *t, n_type_gfx ox, n_type_gfx oy, int direction )
{

	n_chara_bmp_copy_all( p, f, t, ox, oy, direction, NO, NULL );

	return;
}

void
n_chara_bmp_copy_nina( n_nn2 *p, n_bmp *f, n_bmp *t, n_type_gfx ox, n_type_gfx oy, int direction )
{

	if  ( p->bounce_sprite_m != NULL )
	{
		oy += N_NN2_BRICK_MOVE_MAX;
	}

	oy += p->fuwafuwa;

	n_chara_bmp_copy_all( p, f, t, ox, oy, direction, NO, NULL );

	return;
}

void
n_chara_bmp_copy_all_grabbable( n_nn2 *p, n_bmp *f, n_bmp *t, n_type_gfx ox, n_type_gfx oy, int direction, BOOL nina_dokan_onoff, n_sprite *object_dokan_sprite )
{

	oy += p->fuwafuwa;

	n_chara_bmp_copy_all( p, f, t, ox, oy, direction, nina_dokan_onoff, object_dokan_sprite );

	return;
}

void
n_chara_bmp_copy_grabbable( n_nn2 *p, n_bmp *f, n_bmp *t, n_type_gfx ox, n_type_gfx oy, int direction )
{

	oy += p->fuwafuwa;

	n_chara_bmp_copy_all( p, f, t, ox, oy, direction, NO, NULL );

	return;
}




n_sprite_single*
n_chara_get( n_nn2 *p )
{
//NSLog( @"%d", p->sprite_cur->frame );

	if ( p->sprite_cur->frame >= p->sprite_cur->obj[ p->sprite_cur->index ].frame_interval )
	{
		p->sprite_cur->frame = 0;

		p->sprite_cur->index++;
		if ( NULL == p->sprite_cur->obj[ p->sprite_cur->index ].bmp[ 0 ] )
		{
			p->sprite_cur->index = 0;
		}
	}


	return &p->sprite_cur->obj[ p->sprite_cur->index ];
}

n_sprite_single*
n_chara_get_with_interval( n_nn2 *p, int interval )
{
//NSLog( @"%d", p->sprite_cur->frame );

	if ( p->sprite_cur->frame >= interval )
	{
		p->sprite_cur->frame = 0;

		p->sprite_cur->index++;
		if ( NULL == p->sprite_cur->obj[ p->sprite_cur->index ].bmp[ 0 ] )
		{
			p->sprite_cur->index = 0;
		}
	}


	return &p->sprite_cur->obj[ p->sprite_cur->index ];
}

n_sprite_single*
n_chara_get_once( n_nn2 *p )
{
//NSLog( @"%d", p->sprite_cur->frame );

	if ( p->sprite_cur->frame >= p->sprite_cur->obj[ p->sprite_cur->index ].frame_interval )
	{
		p->sprite_cur->frame = 0;

		p->sprite_cur->index++;
		if ( NULL == p->sprite_cur->obj[ p->sprite_cur->index ].bmp[ 0 ] )
		{
			p->sprite_cur->index--;
			p->sprite_cur->once = TRUE;
		}
	}


	return &p->sprite_cur->obj[ p->sprite_cur->index ];
}




void
n_chara_draw( n_nn2 *p, n_bmp *bmp )
{

	n_type_gfx x = 0;
	n_type_gfx y = 0;

	n_camerawork_offset_get( p, &x, &y );


	n_nn2_bmp_draw( p, bmp, x, y );


	return;
}

void
n_chara_wink_init( n_nn2 *p )
{

	if ( p->wink == 0 ) 
	{
		if ( p->wink_timer == 0 ) { n_bmp_ui_timer( &p->wink_timer, 0 ); }

		p->wink       = n_bmp_ui_timer( &p->wink_timer, 2000 );
		p->wink_frame = 0;
	}


	return;
}

n_bmp*
n_chara_wink_loop( n_nn2 *p, n_bmp *bmp )
{

	if ( p->wink )
	{

		if ( bmp == &p->nina_walk_head )
		{
			if ( p->wink == 1 )
			{
				bmp = &p->nina_walk_wink_2;
			} else
			if ( p->wink == 2 )
			{
				bmp = &p->nina_walk_wink_3;
			} else {
				bmp = &p->nina_walk_wink_2;
			}
		}

		if ( bmp == &p->nina_duck_full_head )
		{
			if ( p->wink == 1 )
			{
				bmp = &p->nina_duck_full_wink_2;
			} else
			if ( p->wink == 2 )
			{
				bmp = &p->nina_duck_full_wink_3;
			} else {
				bmp = &p->nina_duck_full_wink_2;
			}
		}

	}


	return bmp;
}

void
n_chara_wink_exit( n_nn2 *p )
{

	if ( p->wink )
	{
		p->wink_frame++;
		if ( p->wink_frame >= 4 )
		{
			p->wink_frame = 0;

			if ( p->wink == 1 )
			{
				p->wink++;
			} else
			if ( p->wink == 2 )
			{
				p->wink++;
			} else {
				p->wink = 0;
			}
		}
	}


	return;
}




n_bmp*
n_chara_swim_loop( n_nn2 *p, n_bmp *bmp )
{

	if ( p->wave_onoff )
	{
		if ( p->costume == N_NN2_COSTUME_PINK )
		{
			if ( bmp == &p->nina_stub_body )
			{
				bmp = &p->nina_swim_pink_body;
			} else
			if ( bmp == &p->nina_stub_boob )
			{
				bmp = &p->nina_swim_pink_boob;
			} else
			if ( bmp == &p->nina_stub_hip )
			{
				//
			} else
			if ( bmp == &p->nina_stub_turn )
			{
				bmp = &p->nina_swim_pink_turn;
			}
		} else {
			if ( bmp == &p->nina_stub_body )
			{
				bmp = &p->nina_swim_blue_body;
			} else
			if ( bmp == &p->nina_stub_boob )
			{
				bmp = &p->nina_swim_blue_boob;
			} else
			if ( bmp == &p->nina_stub_hip )
			{
				//
			} else
			if ( bmp == &p->nina_stub_turn )
			{
				bmp = &p->nina_swim_blue_turn;
			}
		}
	} else {
		if ( p->costume == N_NN2_COSTUME_PINK )
		{
			if ( bmp == &p->nina_stub_body )
			{
				bmp = &p->nina_pink_body;
			} else
			if ( bmp == &p->nina_stub_boob )
			{
				bmp = &p->nina_pink_boob;
			} else
			if ( bmp == &p->nina_stub_hip )
			{
				bmp = &p->nina_pink_hip;
			} else
			if ( bmp == &p->nina_stub_turn )
			{
				bmp = &p->nina_pink_turn;
			} else
			if ( bmp == &p->nina_stub_sleeve_l )
			{
				bmp = &p->nina_pink_sleeve_l;
			} else
			if ( bmp == &p->nina_stub_sleeve_m )
			{
				bmp = &p->nina_pink_sleeve_m;
			} else
			if ( bmp == &p->nina_stub_sleeve_r )
			{
				bmp = &p->nina_pink_sleeve_r;
			}
		} else {
			if ( bmp == &p->nina_stub_body )
			{
				bmp = &p->nina_blue_body;
			} else
			if ( bmp == &p->nina_stub_boob )
			{
				bmp = &p->nina_blue_boob;
			} else
			if ( bmp == &p->nina_stub_hip )
			{
				bmp = &p->nina_blue_hip;
			} else
			if ( bmp == &p->nina_stub_turn )
			{
				bmp = &p->nina_blue_turn;
			} else
			if ( bmp == &p->nina_stub_sleeve_l )
			{
				bmp = &p->nina_blue_sleeve_l;
			} else
			if ( bmp == &p->nina_stub_sleeve_m )
			{
				bmp = &p->nina_blue_sleeve_m;
			} else
			if ( bmp == &p->nina_stub_sleeve_r )
			{
				bmp = &p->nina_blue_sleeve_r;
			}
		}
	}


	return bmp;
}




void
n_chara_idle_init( n_nn2 *p )
{

	n_sprite *s = &n_chara_sprite_idle;


	const int max_frames = 6;


	n_sprite_zero( s );


	int ox[ N_SPRITE_MAX ]; n_memory_zero( ox, N_SPRITE_MAX * sizeof( int ) );
	int oy[ N_SPRITE_MAX ]; n_memory_zero( oy, N_SPRITE_MAX * sizeof( int ) );


	int i = 0;
	n_posix_loop
	{
		int interval = 2;

		if ( i == 0 )
		{
			interval = 200;
		}


		n_bmp *bmp[ N_SPRITE_MAX ];

		bmp[  0 ] = &p->nina_stub_body    ;
		bmp[  1 ] = &p->nina_stub_hip     ;
		bmp[  2 ] = &p->nina_stub_boob    ;
		bmp[  3 ] = &p->nina_walk_head    ;
		bmp[  4 ] = &p->nina_walk_hair_r  ;
		bmp[  5 ] = &p->nina_walk_neko    ;
		bmp[  6 ] = &p->nina_walk_hair_s  ;
		bmp[  7 ] = &p->nina_walk_hair_f  ;
		bmp[  8 ] = &p->nina_walk_lgS_n   ;
		bmp[  9 ] = &p->nina_walk_leg_n   ;
		bmp[ 10 ] = &p->nina_walk_arm_n   ;
		bmp[ 11 ] = &p->nina_stub_sleeve_m;
		bmp[ 12 ] = NULL;


		n_sprite_set( s, i, bmp, ox,oy, 0, interval );


		i++;
		if ( i >= max_frames ) { break; }
	}


	i = 0;
	n_posix_loop
	{//break;

		n_sprite_single *obj = &s->obj[ i ];

		if ( i == 0 )
		{
			//
		} else
		if ( i == 1 )
		{
			obj->bmp[ 9 ] = &p->nina_idle_leg1; 

			obj->oy[ 1 ] = -1; // nina_stub_hip
			obj->oy[ 2 ] = -1; // nina_stub_boob
			obj->oy[ 4 ] = -1; // nina_walk_hair_r
			obj->oy[ 6 ] = -1; // nina_walk_hair_s
			obj->oy[ 7 ] = -1; // nina_walk_hair_f
		} else
		if ( i == 2 )
		{
			obj->bmp[ 9 ] = &p->nina_idle_leg2; 

			obj->oy[ 1 ] = -2; // nina_stub_hip
			obj->oy[ 2 ] = -2; // nina_stub_boob
			obj->oy[ 4 ] = -2; // nina_walk_hair_r
			obj->oy[ 6 ] = -2; // nina_walk_hair_s
			obj->oy[ 7 ] = -2; // nina_walk_hair_f
		} else
		if ( i == 3 )
		{
			obj->bmp[ 9 ] = &p->nina_idle_leg1; 

			obj->oy[ 1 ] = -1; // nina_stub_hip
			obj->oy[ 2 ] = -1; // nina_stub_boob
			obj->oy[ 4 ] = -1; // nina_walk_hair_r
			obj->oy[ 6 ] = -1; // nina_walk_hair_s
			obj->oy[ 7 ] = -1; // nina_walk_hair_f
		} else
		if ( i == 4 )
		{
			obj->bmp[ 9 ] = &p->nina_idle_leg2; 

			obj->oy[ 1 ] = -2; // nina_stub_hip
			obj->oy[ 2 ] = -2; // nina_stub_boob
			obj->oy[ 4 ] = -2; // nina_walk_hair_r
			obj->oy[ 6 ] = -2; // nina_walk_hair_s
			obj->oy[ 7 ] = -2; // nina_walk_hair_f
		} else
		if ( i == 5 )
		{
			obj->bmp[ 9 ] = &p->nina_idle_leg1; 

			obj->oy[ 1 ] = -1; // nina_stub_hip
			obj->oy[ 2 ] = -1; // nina_stub_boob
			obj->oy[ 4 ] = -1; // nina_walk_hair_r
			obj->oy[ 6 ] = -1; // nina_walk_hair_s
			obj->oy[ 7 ] = -1; // nina_walk_hair_f
		}


		i++;
		if ( i >= max_frames ) { break; }
	}


	return;
}

void
n_chara_idle_draw( n_nn2 *p )
{
//return;

	n_bmp *b = &p->bmp_shared; n_bmp_flush( b, n_bmp_black_invisible );


	n_sprite_single *obj = n_chara_get( p );

	n_chara_wink_init( p );

	int i = 0;
	n_posix_loop
	{

		if ( obj->bmp[ i ] == NULL ) { break; }

		n_bmp *bmp = obj->bmp[ i ];
		int    ox  = obj-> ox[ i ];
		int    oy  = obj-> oy[ i ];

//ox += n_game_centering( p->sx, N_BMP_SX( bmp ) );
//oy += n_game_centering( p->sy, N_BMP_SY( bmp ) );

		if ( p->direction == N_NN2_DIRECTION_RIGHT ) { ox *= -1; }

		if ( p->object_handheld_onoff )
		{
			if ( bmp == &p->nina_walk_arm_n )
			{
				bmp = &p->nina_walk_arm_f_2;
			} else
			if ( bmp == &p->nina_jump_arm_1 )
			{
				bmp = &p->nina_walk_arm_f_2;
			} else
			if ( bmp == &p->nina_jump_arm_2 )
			{
				bmp = &p->nina_walk_arm_f_2;
			}
		}

		if ( n_object_treasure_is_running( p ) )
		{
			if ( bmp == &p->nina_walk_arm_n )
			{
				bmp = &p->nina_walk_arm_f_2;
			} else
			if ( bmp == &p->nina_jump_arm_1 )
			{
				bmp = &p->nina_walk_arm_f_2;
			} else
			if ( bmp == &p->nina_jump_arm_2 )
			{
				bmp = &p->nina_walk_arm_f_2;
			}
		}

		bmp = n_chara_swim_loop( p, bmp );
		bmp = n_chara_wink_loop( p, bmp );

		if ( p->dokan_onoff )
		{
			n_chara_bmp_copy_all ( p, bmp, b, ox,oy, p->direction, p->dokan_onoff, NULL );
		} else {
			n_chara_bmp_copy_nina( p, bmp, b, ox,oy, p->direction );
		}

		i++;
		if ( i >= N_SPRITE_MAX ) { break; }
	}

	n_chara_wink_exit( p );


	n_chara_draw( p, b );


	return;
}




void
n_chara_idle_cliff_init( n_nn2 *p )
{

	n_sprite *s = &n_chara_sprite_idle_cliff;


	const int max_frames = 2;


	n_sprite_zero( s );


	int ox[ N_SPRITE_MAX ]; n_memory_zero( ox, N_SPRITE_MAX * sizeof( int ) );
	int oy[ N_SPRITE_MAX ]; n_memory_zero( oy, N_SPRITE_MAX * sizeof( int ) );
	
	
	int interval = 3;


	int i = 0;
	n_posix_loop
	{

		n_bmp *bmp[ N_SPRITE_MAX ];

		bmp[  0 ] = &p->nina_stub_body    ;
		bmp[  1 ] = &p->nina_stub_hip     ;
		bmp[  2 ] = &p->nina_stub_boob    ;
		bmp[  3 ] = &p->nina_walk_head    ;
		bmp[  4 ] = &p->nina_walk_hair_r  ;
		bmp[  5 ] = &p->nina_walk_neko    ;
		bmp[  6 ] = &p->nina_walk_hair_s  ;
		bmp[  7 ] = &p->nina_walk_hair_f  ;
		bmp[  8 ] = &p->nina_idle_cliff   ;
		bmp[  9 ] = &p->nina_jump_arm_1   ;
		bmp[ 10 ] = &p->nina_stub_sleeve_m;
		bmp[ 11 ] = &p->nina_cliff_sweat  ;
		bmp[ 12 ] = NULL;

		n_sprite_set( s, i, bmp, ox, oy, 0, interval );

		i++;
		if ( i >= max_frames ) { break; }
	}


	int n;

	i = 0;
	n_posix_loop
	{

		n_sprite_single *obj = &s->obj[ i ];

		if ( i == 0 )
		{
			//
		} else
		if ( i == 1 )
		{
			n =  0; obj->oy[ n ] =  0; obj->oy[ n ] = -1; // nina_stub_body
			n =  1; obj->oy[ n ] =  0; obj->oy[ n ] = -1; // nina_stub_hip
			n =  2; obj->oy[ n ] = -1; obj->oy[ n ] = -1; // nina_stub_boob
			n =  3; obj->ox[ n ] =  1; obj->oy[ n ] =  0; // nina_walk_head
			n =  4; obj->ox[ n ] =  1; obj->oy[ n ] = -1; // nina_walk_hair_r
			n =  5; obj->ox[ n ] =  1; obj->oy[ n ] =  0; // nina_walk_neko
			n =  6; obj->ox[ n ] =  1; obj->oy[ n ] =  0; // nina_walk_hair_s
			n =  7; obj->ox[ n ] =  1; obj->oy[ n ] = -1; // nina_walk_hair_f
			n =  9; obj->ox[ n ] =  1; obj->oy[ n ] =  0; // nina_jump_arm_1
			n = 10; obj->ox[ n ] =  1; obj->oy[ n ] =  0; // nina_stub_sleeve_m
			n = 11; obj->ox[ n ] = -6; obj->oy[ n ] = -6; // nina_cliff_sweat

			obj->bmp[ 9 ] = &p->nina_jump_arm_2;
		}

		i++;
		if ( i >= max_frames ) { break; }
	}


	return;
}




void
n_chara_walk_init( n_nn2 *p )
{

	n_sprite *s = &n_chara_sprite_walk;


	const int max_frames = 8;


	n_sprite_zero( s );

	int ox[ N_SPRITE_MAX ]; n_memory_zero( ox, N_SPRITE_MAX * sizeof( int ) );
	int oy[ N_SPRITE_MAX ]; n_memory_zero( oy, N_SPRITE_MAX * sizeof( int ) );


	int interval = 2;

	// [!] : slow motion for debug
	//interval = 20;


	int i = 0;
	n_posix_loop
	{

		n_bmp *bmp[ N_SPRITE_MAX ];
		int    snd = 0;

		bmp[  0 ] = &p->nina_stub         ; // Arm
		bmp[  1 ] = &p->nina_stub         ; // Leg
		bmp[  2 ] = &p->nina_stub_body    ;
		bmp[  3 ] = &p->nina_stub_hip     ;
		bmp[  4 ] = &p->nina_stub_boob    ;
		bmp[  5 ] = &p->nina_walk_head    ;
		bmp[  6 ] = &p->nina_walk_hair_r  ; // Hair Rear
		bmp[  7 ] = &p->nina_walk_neko    ;
		bmp[  8 ] = &p->nina_walk_hair_s  ; // Hair Side
		bmp[  9 ] = &p->nina_walk_hair_f  ; // Hair Front
		bmp[ 10 ] = &p->nina_walk_leg_n   ;
		bmp[ 11 ] = &p->nina_walk_arm_n   ;
		bmp[ 12 ] = &p->nina_stub_sleeve_m;
		bmp[ 13 ] = NULL;

		if ( ( i == 1 )||( i == 5 ) )
		{
			snd = N_NN2_SOUND_WALK;
		}

		n_sprite_set( s, i, bmp, ox, oy, snd, interval );

		i++;
		if ( i >= max_frames ) { break; }
	}

	int n;

	i = 0;
	n_posix_loop
	{
		n_sprite_single *obj = &s->obj[ i ];


		if ( i <= 3 )
		{
			obj->bmp[ 12 ] = &p->nina_stub_sleeve_l;
		} else
		if ( i >= 5 )
		{
			obj->bmp[ 12 ] = &p->nina_stub_sleeve_r;
		}


		if ( ( i == 1 )||( i == 5 ) )
		{
			n =  2; obj->ox[ n ] =  0; obj->oy[ n ] = -1; // nina_stub_body
			n =  3; obj->ox[ n ] = -1; obj->oy[ n ] = -1; // nina_stub_hip
			n =  4; obj->ox[ n ] =  0; obj->oy[ n ] = -1; // nina_stub_boob
			n =  5; obj->ox[ n ] =  0; obj->oy[ n ] =  1; // nina_walk_head
			n =  6; obj->ox[ n ] =  0; obj->oy[ n ] =  0; // nina_walk_hair_r
			n =  7; obj->ox[ n ] =  0; obj->oy[ n ] =  1; // nina_walk_neko
			n =  8; obj->ox[ n ] =  0; obj->oy[ n ] =  0; // nina_walk_hair_s
			n =  9; obj->ox[ n ] =  0; obj->oy[ n ] = -1; // nina_walk_hair_f
			n = 12; obj->ox[ n ] =  0; obj->oy[ n ] = -1; // nina_stub_sleeve
		} else
		if ( ( i == 2 )||( i == 6 ) )
		{
			n =  2; obj->ox[ n ] =  0; obj->oy[ n ] = -2; // nina_stub_body
			n =  3; obj->ox[ n ] = -2; obj->oy[ n ] = -2; // nina_stub_hip
			n =  4; obj->ox[ n ] =  0; obj->oy[ n ] = -2; // nina_stub_boob
			n =  5; obj->ox[ n ] =  0; obj->oy[ n ] =  2; // nina_walk_head
			n =  6; obj->ox[ n ] =  0; obj->oy[ n ] = -1; // nina_walk_hair_r
			n =  7; obj->ox[ n ] =  0; obj->oy[ n ] =  2; // nina_walk_neko
			n =  8; obj->ox[ n ] =  0; obj->oy[ n ] =  1; // nina_walk_hair_s
			n =  9; obj->ox[ n ] = -1; obj->oy[ n ] = -2; // nina_walk_hair_f
			n = 12; obj->ox[ n ] =  0; obj->oy[ n ] = -2; // nina_stub_sleeve
		} else
		if ( ( i == 3 )||( i == 7 ) )
		{
			n =  2; obj->ox[ n ] =  0; obj->oy[ n ] = -1; // nina_stub_body
			n =  3; obj->ox[ n ] = -1; obj->oy[ n ] = -1; // nina_stub_hip
			n =  4; obj->ox[ n ] =  0; obj->oy[ n ] = -1; // nina_stub_boob
			n =  5; obj->ox[ n ] =  0; obj->oy[ n ] =  1; // nina_walk_head
			n =  6; obj->ox[ n ] =  0; obj->oy[ n ] =  0; // nina_walk_hair_r
			n =  7; obj->ox[ n ] =  0; obj->oy[ n ] =  1; // nina_walk_neko
			n =  8; obj->ox[ n ] =  0; obj->oy[ n ] =  0; // nina_walk_hair_s
			n =  9; obj->ox[ n ] =  0; obj->oy[ n ] = -1; // nina_walk_hair_f
			n = 12; obj->ox[ n ] =  0; obj->oy[ n ] = -1; // nina_stub_sleeve
		}


		if ( i == 0 )
		{
			// neutral
		} else
		if ( i == 1 )
		{
			obj->bmp[  0 ] = &p->nina_walk_amS_r_1;
			obj->bmp[  1 ] = &p->nina_walk_lgS_f_1;
			obj->bmp[ 10 ] = &p->nina_walk_leg_r_1;
			obj->bmp[ 11 ] = &p->nina_walk_arm_f_1;
		} else
		if ( i == 2 )
		{
			obj->bmp[  0 ] = &p->nina_walk_amS_r_2;
			obj->bmp[  1 ] = &p->nina_walk_lgS_f_2;
			obj->bmp[ 10 ] = &p->nina_walk_leg_r_2;
			obj->bmp[ 11 ] = &p->nina_walk_arm_f_2;
		} else
		if ( i == 3 )
		{
			obj->bmp[  0 ] = &p->nina_walk_amS_r_1;
			obj->bmp[  1 ] = &p->nina_walk_lgS_fd1;
			obj->bmp[ 10 ] = &p->nina_walk_leg_r_1;
			obj->bmp[ 11 ] = &p->nina_walk_arm_f_1;
		} else
		if ( i == 4 )
		{
			// neutral
		} else
		if ( i == 5 )
		{
			obj->bmp[  0 ] = &p->nina_walk_amS_f_1;
			obj->bmp[  1 ] = &p->nina_walk_lgS_r_1;
			obj->bmp[ 10 ] = &p->nina_walk_leg_f_1;
			obj->bmp[ 11 ] = &p->nina_walk_arm_r_1;
		} else
		if ( i == 6 )
		{
			obj->bmp[  0 ] = &p->nina_walk_amS_f_2;
			obj->bmp[  1 ] = &p->nina_walk_lgS_r_2;
			obj->bmp[ 10 ] = &p->nina_walk_leg_f_2;
			obj->bmp[ 11 ] = &p->nina_walk_arm_r_2;
		} else
		if ( i == 7 )
		{
			obj->bmp[  0 ] = &p->nina_walk_amS_f_1;
			obj->bmp[  1 ] = &p->nina_walk_lgS_r_1;
			obj->bmp[ 10 ] = &p->nina_walk_leg_fd1;
			obj->bmp[ 11 ] = &p->nina_walk_arm_r_1;
		}


		i++;
		if ( i >= max_frames ) { break; }
	}


	return;
}

void
n_chara_walk_draw( n_nn2 *p )
{

	n_bmp *b = &p->bmp_shared; n_bmp_flush( b, n_bmp_black_invisible );


	n_sprite_single *obj = n_chara_get( p );

	n_chara_wink_init( p );

	int i = 0;
	n_posix_loop
	{

		if ( obj->bmp[ i ] == NULL ) { break; }

		n_bmp *bmp = obj->bmp[ i ];
		int    ox  = obj-> ox[ i ] * p->zoom;
		int    oy  = obj-> oy[ i ] * p->zoom;

//ox += n_game_centering( p->sx, N_BMP_SX( bmp ) );
//oy += n_game_centering( p->sy, N_BMP_SY( bmp ) );

		if ( p->object_handheld_onoff )
		{
			if (
				( bmp == &p->nina_walk_arm_n )

				||
				( bmp == &p->nina_walk_arm_f_1 )
				||
				( bmp == &p->nina_walk_arm_f_2 )
				||
				( bmp == &p->nina_walk_arm_r_1 )
				||
				( bmp == &p->nina_walk_arm_r_2 )

				||
				( bmp == &p->nina_walk_amS_f_1 )
				||
				( bmp == &p->nina_walk_amS_f_2 )
				||
				( bmp == &p->nina_walk_amS_r_1 )
				||
				( bmp == &p->nina_walk_amS_r_2 )
			)
			{
				bmp = &p->nina_walk_arm_f_2;
			}
		}

		bmp = n_chara_swim_loop( p, bmp );
		bmp = n_chara_wink_loop( p, bmp );

		n_chara_bmp_copy_nina( p, bmp, b, ox,oy, p->direction );

		if ( p->swim_onoff == FALSE )
		{
			n_nn2_sound_effect_play( obj->sound_id );
		}

		i++;
		if ( i >= N_SPRITE_MAX ) { break; }
	}

	n_chara_wink_exit( p );


	n_chara_draw( p, b );


	return;
}




void
n_chara_dash_init( n_nn2 *p )
{
//n_chara_sprite_dash = n_chara_sprite_walk; return;


	n_sprite *s = &n_chara_sprite_dash;


	const int max_frames = 8;


	n_sprite_zero( s );

	int ox[ N_SPRITE_MAX ]; n_memory_zero( ox, N_SPRITE_MAX * sizeof( int ) );
	int oy[ N_SPRITE_MAX ]; n_memory_zero( oy, N_SPRITE_MAX * sizeof( int ) );


	int interval = 2;

	// [!] : slow motion for debug
	//interval = 20;


	int i = 0;
	n_posix_loop
	{

		n_bmp *bmp[ N_SPRITE_MAX ];
		int    snd = 0;

		bmp[  0 ] = &p->nina_stub         ; // Arm
		bmp[  1 ] = &p->nina_stub         ; // Leg
		bmp[  2 ] = &p->nina_stub_body    ;
		bmp[  3 ] = &p->nina_stub_hip     ;
		bmp[  4 ] = &p->nina_stub_boob    ;
		bmp[  5 ] = &p->nina_walk_head    ;
		bmp[  6 ] = &p->nina_walk_hair_r  ; // Hair Rear
		bmp[  7 ] = &p->nina_walk_neko    ;
		bmp[  8 ] = &p->nina_walk_hair_s  ; // Hair Side
		bmp[  9 ] = &p->nina_walk_hair_f  ; // Hair Front
		bmp[ 10 ] = &p->nina_walk_leg_n   ;
		bmp[ 11 ] = &p->nina_dash_arm_n   ;
		bmp[ 12 ] = &p->nina_stub_sleeve_m;
		bmp[ 13 ] = NULL;

		if ( ( i == 1 )||( i == 5 ) )
		{
			snd = N_NN2_SOUND_WALK;
		}

		n_sprite_set( s, i, bmp, ox, oy, snd, interval );

		i++;
		if ( i >= max_frames ) { break; }
	}


	int n;

	i = 0;
	n_posix_loop
	{
		n_sprite_single *obj = &s->obj[ i ];


		if ( i <= 3 )
		{
			obj->bmp[ 12 ] = &p->nina_stub_sleeve_l;
		} else
		if ( i >= 5 )
		{
			obj->bmp[ 12 ] = &p->nina_stub_sleeve_r;
		}


		// [!] : currently, parameters are the same as walking

		if ( ( i == 1 )||( i == 5 ) )
		{
			n =  2; obj->ox[ n ] =  0; obj->oy[ n ] = -1; // nina_stub_body
			n =  3; obj->ox[ n ] = -1; obj->oy[ n ] = -1; // nina_stub_hip
			n =  4; obj->ox[ n ] =  0; obj->oy[ n ] = -1; // nina_stub_boob
			n =  5; obj->ox[ n ] =  0; obj->oy[ n ] =  1; // nina_walk_head
			n =  6; obj->ox[ n ] =  0; obj->oy[ n ] =  0; // nina_walk_hair_r
			n =  7; obj->ox[ n ] =  0; obj->oy[ n ] =  1; // nina_walk_neko
			n =  8; obj->ox[ n ] =  0; obj->oy[ n ] =  0; // nina_walk_hair_s
			n =  9; obj->ox[ n ] =  0; obj->oy[ n ] = -1; // nina_walk_hair_f
			n = 12; obj->ox[ n ] =  0; obj->oy[ n ] = -1; // nina_stub_sleeve
		} else
		if ( ( i == 2 )||( i == 6 ) )
		{
			n =  2; obj->ox[ n ] =  0; obj->oy[ n ] = -2; // nina_stub_body
			n =  3; obj->ox[ n ] = -2; obj->oy[ n ] = -2; // nina_stub_hip
			n =  4; obj->ox[ n ] =  0; obj->oy[ n ] = -2; // nina_stub_boob
			n =  5; obj->ox[ n ] =  0; obj->oy[ n ] =  2; // nina_walk_head
			n =  6; obj->ox[ n ] =  0; obj->oy[ n ] = -1; // nina_walk_hair_r
			n =  7; obj->ox[ n ] =  0; obj->oy[ n ] =  2; // nina_walk_neko
			n =  8; obj->ox[ n ] =  0; obj->oy[ n ] =  1; // nina_walk_hair_s
			n =  9; obj->ox[ n ] = -1; obj->oy[ n ] = -2; // nina_walk_hair_f
			n = 12; obj->ox[ n ] =  0; obj->oy[ n ] = -2; // nina_stub_sleeve
		} else
		if ( ( i == 3 )||( i == 7 ) )
		{
			n =  2; obj->ox[ n ] =  0; obj->oy[ n ] = -1; // nina_stub_body
			n =  3; obj->ox[ n ] = -1; obj->oy[ n ] = -1; // nina_stub_hip
			n =  4; obj->ox[ n ] =  0; obj->oy[ n ] = -1; // nina_stub_boob
			n =  5; obj->ox[ n ] =  0; obj->oy[ n ] =  1; // nina_walk_head
			n =  6; obj->ox[ n ] =  0; obj->oy[ n ] =  0; // nina_walk_hair_r
			n =  7; obj->ox[ n ] =  0; obj->oy[ n ] =  1; // nina_walk_neko
			n =  8; obj->ox[ n ] =  0; obj->oy[ n ] =  0; // nina_walk_hair_s
			n =  9; obj->ox[ n ] =  0; obj->oy[ n ] = -1; // nina_walk_hair_f
			n = 12; obj->ox[ n ] =  0; obj->oy[ n ] = -1; // nina_stub_sleeve
		}


		if ( i == 0 )
		{
			// neutral
		} else
		if ( i == 1 )
		{
			obj->bmp[  0 ] = &p->nina_dash_amS_r_1;
			obj->bmp[  1 ] = &p->nina_dash_lgS_f_1;
			obj->bmp[ 10 ] = &p->nina_dash_leg_r_1;
			obj->bmp[ 11 ] = &p->nina_dash_arm_f_1;
		} else
		if ( i == 2 )
		{
			obj->bmp[  0 ] = &p->nina_dash_amS_r_2;
			obj->bmp[  1 ] = &p->nina_dash_lgS_f_2;
			obj->bmp[ 10 ] = &p->nina_dash_leg_r_2;
			obj->bmp[ 11 ] = &p->nina_dash_arm_f_2;
		} else
		if ( i == 3 )
		{
			obj->bmp[  0 ] = &p->nina_dash_amS_r_1;
			obj->bmp[  1 ] = &p->nina_dash_lgS_f_1;
			obj->bmp[ 10 ] = &p->nina_dash_leg_r_1;
			obj->bmp[ 11 ] = &p->nina_dash_arm_f_1;
		} else
		if ( i == 4 )
		{
			// neutral
		} else
		if ( i == 5 )
		{
			obj->bmp[  0 ] = &p->nina_dash_amS_f_1;
			obj->bmp[  1 ] = &p->nina_dash_lgS_r_1;
			obj->bmp[ 10 ] = &p->nina_dash_leg_f_1;
			obj->bmp[ 11 ] = &p->nina_dash_arm_r_1;
		} else
		if ( i == 6 )
		{
			obj->bmp[  0 ] = &p->nina_dash_amS_f_2;
			obj->bmp[  1 ] = &p->nina_dash_lgS_r_2;
			obj->bmp[ 10 ] = &p->nina_dash_leg_f_2;
			obj->bmp[ 11 ] = &p->nina_dash_arm_r_2;
		} else
		if ( i == 7 )
		{
			obj->bmp[  0 ] = &p->nina_dash_amS_f_1;
			obj->bmp[  1 ] = &p->nina_dash_lgS_r_1;
			obj->bmp[ 10 ] = &p->nina_dash_leg_f_1;
			obj->bmp[ 11 ] = &p->nina_dash_arm_r_1;
		}


		i++;
		if ( i >= max_frames ) { break; }
	}


	return;
}

void
n_chara_dash_draw( n_nn2 *p, BOOL is_pre )
{
//return;

	n_bmp *b = &p->bmp_shared; n_bmp_flush( b, n_bmp_black_invisible );


	n_sprite_single *obj = n_chara_get( p );

	n_chara_wink_init( p );

	int i = 0;
	if ( is_pre == FALSE ) { i = 2; }

	n_posix_loop
	{

		if ( obj->bmp[ i ] == NULL ) { break; }

		n_bmp *bmp = obj->bmp[ i ];
		int    ox  = obj-> ox[ i ] * p->zoom;
		int    oy  = obj-> oy[ i ] * p->zoom;

//ox += n_game_centering( p->sx, N_BMP_SX( bmp ) );
//oy += n_game_centering( p->sy, N_BMP_SY( bmp ) );


		if ( p->object_handheld_onoff )
		{
			p->object_handheld_sprite->ox = ox;
			p->object_handheld_sprite->oy = oy;

			if ( bmp == &p->nina_stub_sleeve_r )
			{
				bmp = &p->nina_stub_sleeve_l;
			} else
			if (
				( bmp == &p->nina_dash_arm_n   )

				||
				( bmp == &p->nina_dash_arm_f_1 )
				||
				( bmp == &p->nina_dash_arm_f_2 )
				||
				( bmp == &p->nina_dash_arm_r_1 )
				||
				( bmp == &p->nina_dash_arm_r_2 )

				||
				( bmp == &p->nina_dash_amS_f_1 )
				||
				( bmp == &p->nina_dash_amS_f_2 )
				||
				( bmp == &p->nina_dash_amS_r_1 )
				||
				( bmp == &p->nina_dash_amS_r_2 )
			)
			{
				bmp = &p->nina_walk_arm_f_2;
			}
		}

		bmp = n_chara_swim_loop( p, bmp );
		bmp = n_chara_wink_loop( p, bmp );

		n_chara_bmp_copy_nina( p, bmp, b, ox,oy, p->direction );


		if ( p->swim_onoff == FALSE )
		{
			n_nn2_sound_effect_play( obj->sound_id );
		}


		i++;
		if ( i >= N_SPRITE_MAX ) { break; }

		if ( ( is_pre )&&( i >= 2 ) ) { break; }
	}

	n_chara_wink_exit( p );


	n_chara_draw( p, b );


	return;
}




void
n_chara_slip_init( n_nn2 *p )
{

	n_sprite *s = &n_chara_sprite_slip;


	const int max_frames = 1;


	n_sprite_zero( s );

	int ox[ N_SPRITE_MAX ]; n_memory_zero( ox, N_SPRITE_MAX * sizeof( int ) );
	int oy[ N_SPRITE_MAX ]; n_memory_zero( oy, N_SPRITE_MAX * sizeof( int ) );


	int interval = 5;


	int i = 0;
	n_posix_loop
	{

		n_bmp *bmp[ N_SPRITE_MAX ];

		bmp[  0 ] = &p->nina_stub_body    ;
		bmp[  1 ] = &p->nina_stub_hip     ;
		bmp[  2 ] = &p->nina_stub_boob    ;
		bmp[  3 ] = &p->nina_walk_head    ;
		bmp[  4 ] = &p->nina_walk_hair_r  ;
		bmp[  5 ] = &p->nina_walk_neko    ;
		bmp[  6 ] = &p->nina_walk_hair_s  ;
		bmp[  7 ] = &p->nina_walk_hair_f  ;
		bmp[  8 ] = &p->nina_walk_leg_f_2 ;
		bmp[  9 ] = &p->nina_walk_arm_r_2 ;
		bmp[ 10 ] = &p->nina_stub_sleeve_l;
		bmp[ 11 ] = NULL;

		n_sprite_set( s, i, bmp, ox, oy, 0, interval );

		i++;
		if ( i >= max_frames ) { break; }
	}


	return;
}

void
n_chara_slip_draw( n_nn2 *p )
{

	n_bmp *b = &p->bmp_shared; n_bmp_flush( b, n_bmp_black_invisible );


	n_sprite_single *obj = n_chara_get_once( p );

	int i = 0;
	n_posix_loop
	{

		if ( obj->bmp[ i ] == NULL ) { break; }

		n_bmp *bmp = obj->bmp[ i ];
		int    ox  = obj-> ox[ i ];
		int    oy  = obj-> oy[ i ];

//ox += n_game_centering( p->sx, N_BMP_SX( bmp ) );
//oy += n_game_centering( p->sy, N_BMP_SY( bmp ) );


		if ( p->object_handheld_onoff )
		{
			if ( bmp == &p->nina_walk_arm_r_2 )
			{
				bmp = &p->nina_walk_arm_f_2;
			}
		}


		bmp = n_chara_swim_loop( p, bmp );


		n_chara_bmp_copy_nina( p, bmp, b, ox,oy, p->direction );


		//n_nn2_sound_effect_play( obj->sound_id );

		i++;
		if ( i >= N_SPRITE_MAX ) { break; }
	}


	n_chara_draw( p, b );


	return;
}




void
n_chara_duck_init( n_nn2 *p )
{

	n_sprite *s = &n_chara_sprite_duck;


	const int max_frames = 2;


	n_sprite_zero( s );

	int ox[ N_SPRITE_MAX ]; n_memory_zero( ox, N_SPRITE_MAX * sizeof( int ) );
	int oy[ N_SPRITE_MAX ]; n_memory_zero( oy, N_SPRITE_MAX * sizeof( int ) );


	int interval = 2;


	n_type_gfx hf = N_NN2_STEP_DUCK_HALF;
	n_type_gfx fl = N_NN2_STEP_DUCK_FULL;

	int i = 0;
	n_posix_loop
	{

		n_bmp *bmp[ N_SPRITE_MAX ];

		if ( i == 0 )
		{
			int n = 0;
			bmp[ n ] = &p->nina_stub_body           ; ox[ n ] =  0; oy[ n ] = hf; n++;
			bmp[ n ] = &p->nina_stub_hip            ; ox[ n ] =  0; oy[ n ] = hf; n++;
			bmp[ n ] = &p->nina_stub_boob           ; ox[ n ] =  0; oy[ n ] = hf; n++;
			bmp[ n ] = &p->nina_duck_half_body      ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_sleeve_m       ; ox[ n ] =  0; oy[ n ] = hf; n++;
			bmp[ n ] = &p->nina_duck_half_head      ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_duck_half_hair_rear ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_duck_half_neko      ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_duck_half_hair_side ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_duck_half_hair_front; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = NULL;
		} else
		if ( i == 1 )
		{//break;
			int n = 0;
			bmp[ n ] = &p->nina_stub_body           ; ox[ n ] =  0; oy[ n ] = fl; n++;
			bmp[ n ] = &p->nina_stub_hip            ; ox[ n ] =  0; oy[ n ] = fl; n++;
			bmp[ n ] = &p->nina_stub_boob           ; ox[ n ] =  0; oy[ n ] = fl; n++;
			bmp[ n ] = &p->nina_duck_full_body      ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_sleeve_m       ; ox[ n ] =  0; oy[ n ] = fl; n++;
			bmp[ n ] = &p->nina_duck_full_head      ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_duck_full_hair_rear ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_duck_full_neko      ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_duck_full_hair_side ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_duck_full_hair_front; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = NULL;
		}

		n_sprite_set( s, i, bmp, ox, oy, 0, interval );

		i++;
		if ( i >= max_frames ) { break; }
	}


	return;
}

void
n_chara_unduck_init( n_nn2 *p )
{

	n_sprite *s = &n_chara_sprite_unduck;


	const int max_frames = 2;


	n_sprite_zero( s );

	int ox[ N_SPRITE_MAX ]; n_memory_zero( ox, N_SPRITE_MAX * sizeof( int ) );
	int oy[ N_SPRITE_MAX ]; n_memory_zero( oy, N_SPRITE_MAX * sizeof( int ) );


	int interval = 2;


	n_type_gfx hf = N_NN2_STEP_DUCK_HALF;
	n_type_gfx fl = N_NN2_STEP_DUCK_FULL;

	int i = 0;
	n_posix_loop
	{

		n_bmp *bmp[ N_SPRITE_MAX ];

		if ( i == 0 )
		{
			int n = 0;
			bmp[ n ] = &p->nina_stub_body           ; ox[ n ] =  0; oy[ n ] = fl; n++;
			bmp[ n ] = &p->nina_stub_hip            ; ox[ n ] =  0; oy[ n ] = fl; n++;
			bmp[ n ] = &p->nina_stub_boob           ; ox[ n ] =  0; oy[ n ] = fl; n++;
			bmp[ n ] = &p->nina_duck_full_body      ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_sleeve_m       ; ox[ n ] =  0; oy[ n ] = fl; n++;
			bmp[ n ] = &p->nina_duck_full_head      ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_duck_full_hair_rear ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_duck_full_neko      ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_duck_full_hair_side ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_duck_full_hair_front; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = NULL;
		} else
		if ( i == 1 )
		{//break;
			int n = 0;
			bmp[ n ] = &p->nina_stub_body           ; ox[ n ] =  0; oy[ n ] = hf; n++;
			bmp[ n ] = &p->nina_stub_hip            ; ox[ n ] =  0; oy[ n ] = hf; n++;
			bmp[ n ] = &p->nina_stub_boob           ; ox[ n ] =  0; oy[ n ] = hf; n++;
			bmp[ n ] = &p->nina_duck_half_body      ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_sleeve_m       ; ox[ n ] =  0; oy[ n ] = hf; n++;
			bmp[ n ] = &p->nina_duck_half_head      ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_duck_half_hair_rear ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_duck_half_neko      ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_duck_half_hair_side ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_duck_half_hair_front; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = NULL;
		}

		n_sprite_set( s, i, bmp, ox, oy, 0, interval );

		i++;
		if ( i >= max_frames ) { break; }
	}


	return;
}

void
n_chara_duck_draw( n_nn2 *p )
{

	n_bmp *b = &p->bmp_shared; n_bmp_flush( b, n_bmp_black_invisible );


	n_sprite_single *obj = n_chara_get_once( p );

	n_chara_wink_init( p );

	int i = 0;
	n_posix_loop
	{

		if ( obj->bmp[ i ] == NULL ) { break; }

		n_bmp *bmp = obj->bmp[ i ];
		int    ox  = obj-> ox[ i ];
		int    oy  = obj-> oy[ i ];

//ox += n_game_centering( p->sx, N_BMP_SX( bmp ) );
//oy += n_game_centering( p->sy, N_BMP_SY( bmp ) );

		if ( oy == N_NN2_STEP_DUCK_FULL )
		{
			if (
				( bmp == &p->nina_stub_body )
				||
				( bmp == &p->nina_stub_boob )
				||
				( bmp == &p->nina_stub_hip )
				||
				( bmp == &p->nina_stub_sleeve_m )
			)
			{
				n_type_gfx tweaker = 9 * p->zoom;

				if ( p->direction == N_NN2_DIRECTION_LEFT )
				{
					ox += tweaker;
				} else {
					ox -= tweaker;
				}
			}
		}

		bmp = n_chara_swim_loop( p, bmp );
		bmp = n_chara_wink_loop( p, bmp );

		n_chara_bmp_copy_nina( p, bmp, b, ox,oy, p->direction );

		n_nn2_sound_effect_play( obj->sound_id );

		i++;
		if ( i >= N_SPRITE_MAX ) { break; }
	}

	n_chara_wink_exit( p );


	n_chara_draw( p, b );


	return;
}




void
n_chara_jump_init( n_nn2 *p )
{

	n_sprite *s = &n_chara_sprite_jump;


	const int max_frames = 4;


	n_sprite_zero( s );

	int ox[ N_SPRITE_MAX ]; n_memory_zero( ox, N_SPRITE_MAX * sizeof( int ) );
	int oy[ N_SPRITE_MAX ]; n_memory_zero( oy, N_SPRITE_MAX * sizeof( int ) );


	int interval = 1;


	int i = 0;
	n_posix_loop
	{

		n_bmp *bmp[ N_SPRITE_MAX ];
		int    snd = 0;

		bmp[  0 ] = &p->nina_stub_body    ;
		bmp[  1 ] = &p->nina_stub_hip     ;
		bmp[  2 ] = &p->nina_stub_boob    ;
		bmp[  3 ] = &p->nina_walk_head    ;
		bmp[  4 ] = &p->nina_walk_ear     ;
		bmp[  5 ] = &p->nina_walk_hair_r  ;
		bmp[  6 ] = &p->nina_walk_neko    ;
		bmp[  7 ] = &p->nina_walk_hair_s  ;
		bmp[  8 ] = &p->nina_walk_hair_f  ;
		bmp[  9 ] = &p->nina_jump_lgS_1   ;
		bmp[ 10 ] = &p->nina_jump_leg_3   ;
		bmp[ 11 ] = &p->nina_jump_arm_1   ;
		bmp[ 12 ] = &p->nina_stub_sleeve_r;
		bmp[ 13 ] = NULL;

		n_sprite_set( s, i, bmp, ox, oy, snd, interval );

		i++;
		if ( i >= max_frames ) { break; }
	}

	int n = 0;

	i = 0;
	n_posix_loop
	{

		n_sprite_single *obj = &s->obj[ i ];

		if ( i == 0 )
		{
			//
		} else
		if ( i == 1 )
		{
			n =  2; obj->ox[ n ] =  0; obj->oy[ n ] =  1; // nina_stub_boob
			n =  5; obj->ox[ n ] =  0; obj->oy[ n ] =  1; // nina_walk_hair_r
			n =  8; obj->ox[ n ] = -1; obj->oy[ n ] =  1; // nina_walk_hair_f
			n = 11; obj->ox[ n ] =  1; obj->oy[ n ] =  1; // nina_jump_arm_1

			obj->bmp[  9 ] = &p->nina_stub; // [!]: unused
			obj->bmp[ 10 ] = &p->nina_jump_leg_2;
		} else
		if ( i == 2 )
		{
			n =  2; obj->ox[ n ] =  0; obj->oy[ n ] =  2; // nina_stub_boob
			n =  3; obj->ox[ n ] =  0; obj->oy[ n ] =  1; // nina_walk_head
			n =  4; obj->ox[ n ] =  0; obj->oy[ n ] =  1; // nina_walk_ear
			n =  5; obj->ox[ n ] = -1; obj->oy[ n ] =  2; // nina_walk_hair_r
			n =  6; obj->ox[ n ] =  0; obj->oy[ n ] =  1; // nina_walk_neko
			n =  7; obj->ox[ n ] =  0; obj->oy[ n ] =  1; // nina_walk_hair_s
			n =  8; obj->ox[ n ] = -2; obj->oy[ n ] =  2; // nina_walk_hair_f
			n = 11; obj->ox[ n ] =  1; obj->oy[ n ] =  1; // nina_jump_arm_1

			obj->bmp[  9 ] = &p->nina_jump_lgS_3;
			obj->bmp[ 10 ] = &p->nina_jump_leg_1;
		} else
		if ( i == 3 )
		{
			n =  2; obj->ox[ n ] =  0; obj->oy[ n ] =  1; // nina_stub_boob
			n =  5; obj->ox[ n ] =  0; obj->oy[ n ] =  1; // nina_walk_hair_r
			n =  8; obj->ox[ n ] = -1; obj->oy[ n ] =  1; // nina_walk_hair_f
			n = 11; obj->ox[ n ] =  1; obj->oy[ n ] =  1; // nina_jump_arm_1

			obj->bmp[  9 ] = &p->nina_stub; // [!]: unused
			obj->bmp[ 10 ] = &p->nina_jump_leg_2;
		}

		i++;
		if ( i >= max_frames ) { break; }
	}


	return;
}

void
n_chara_jump_draw( n_nn2 *p )
{
//NSLog( @"%d", p->object_handheld_onoff );


	n_bmp *b = &p->bmp_shared; n_bmp_flush( b, n_bmp_black_invisible );


	int interval;

	if ( p->swim_onoff )
	{
		interval = p->powerup->swim_interval;
	} else {
		interval = p->powerup->jump_interval;
	}

	n_sprite_single *obj = n_chara_get_with_interval( p, interval );


	if ( p->jump_vertical_threshold < abs( p->jump_vertical_start_x - p->stage->nina_x ) )
	{
		p->jump_vertical_onoff = FALSE;
	}


	n_chara_wink_init( p );

	int i = 0;
	n_posix_loop
	{

		if ( obj->bmp[ i ] == NULL ) { break; }

		n_bmp *bmp = obj->bmp[ i ];
		int    ox  = obj-> ox[ i ];
		int    oy  = obj-> oy[ i ];

//ox += n_game_centering( p->sx, N_BMP_SX( bmp ) );
//oy += n_game_centering( p->sy, N_BMP_SY( bmp ) );

		if ( p->object_handheld_onoff )
		{
			if ( bmp == &p->nina_jump_arm_1 )
			{
				bmp = &p->nina_walk_arm_f_2;
			}
		} else
		if ( p->swim_onoff )
		{
			if ( bmp == &p->nina_jump_arm_1 )
			{
				ox = oy = 0;
				bmp = &p->nina_jump_arm_2;
			}
		} else
		if ( p->jump_flow )
		{
			if ( bmp == &p->nina_jump_arm_1 )
			{
				if ( p->sprite_cur->frame & 1 ) { bmp = &p->nina_jump_arm_2; }
			}
		}

		if ( p->jump_vertical_onoff )
		{
			if (
				( bmp == &p->nina_jump_leg_1 )
				||
				( bmp == &p->nina_jump_leg_2 )
				||
				( bmp == &p->nina_jump_leg_3 )
				||
				( bmp == &p->nina_jump_lgS_1 )
				||
				( bmp == &p->nina_jump_lgS_2 )
				||
				( bmp == &p->nina_jump_lgS_3 )
			)
			{
				bmp = &p->nina_jump_leg_t;
			}
		}

		if ( p->swim_onoff )
		{
			//
		} else
		if ( p->fly_phase == 0 )
		{
			if (
				( bmp == &p->nina_jump_leg_1 )
				||
				( bmp == &p->nina_jump_leg_2 )
				||
				( bmp == &p->nina_jump_leg_3 )
				||
				( bmp == &p->nina_jump_lgS_1 )
				||
				( bmp == &p->nina_jump_lgS_2 )
				||
				( bmp == &p->nina_jump_lgS_3 )
			)
			{
				bmp = &p->nina_jump_leg_1;
			}
		}

		bmp = n_chara_swim_loop( p, bmp );
		bmp = n_chara_wink_loop( p, bmp );
	
		n_chara_bmp_copy_nina( p, bmp, b, ox,oy, p->direction );

		n_nn2_sound_effect_play( obj->sound_id );


		i++;
		if ( i >= N_SPRITE_MAX ) { break; }
	}

	n_chara_wink_exit( p );


	n_chara_draw( p, b );


	return;
}




void
n_chara_land_init( n_nn2 *p )
{

	n_sprite *s = &n_chara_sprite_land;


	const int max_frames = 1;


	n_sprite_zero( s );

	int ox[ N_SPRITE_MAX ]; n_memory_zero( ox, N_SPRITE_MAX * sizeof( int ) );
	int oy[ N_SPRITE_MAX ]; n_memory_zero( oy, N_SPRITE_MAX * sizeof( int ) );


	int interval = 3;


	n_type_gfx hf = N_NN2_STEP_DUCK_HALF;

	oy[ 0 ] = hf;
	oy[ 1 ] = hf;
	oy[ 2 ] = hf;
	oy[ 4 ] = hf;

	int i = 0;
	n_posix_loop
	{

		n_bmp *bmp[ N_SPRITE_MAX ];

		bmp[  0 ] = &p->nina_stub_body           ;
		bmp[  1 ] = &p->nina_stub_hip            ;
		bmp[  2 ] = &p->nina_stub_boob           ;
		bmp[  3 ] = &p->nina_duck_half_body      ;
		bmp[  4 ] = &p->nina_stub_sleeve_m       ;
		bmp[  5 ] = &p->nina_duck_half_head      ;
		bmp[  6 ] = &p->nina_duck_half_hair_rear ;
		bmp[  7 ] = &p->nina_duck_half_neko      ;
		bmp[  8 ] = &p->nina_duck_half_hair_side ;
		bmp[  9 ] = &p->nina_duck_half_hair_front;
		bmp[ 10 ] = NULL;

		n_sprite_set( s, i, bmp, ox, oy, 0, interval );

		i++;
		if ( i >= max_frames ) { break; }
	}

	return;
}

void
n_chara_land_draw( n_nn2 *p )
{

	n_bmp *b = &p->bmp_shared; n_bmp_flush( b, n_bmp_black_invisible );


	n_sprite_single *obj = n_chara_get_once( p );


	int i = 0;
	n_posix_loop
	{

		if ( obj->bmp[ i ] == NULL ) { break; }

		n_bmp *bmp = obj->bmp[ i ];
		int    ox  = obj-> ox[ i ];
		int    oy  = obj-> oy[ i ];

//ox += n_game_centering( p->sx, N_BMP_SX( bmp ) );
//oy += n_game_centering( p->sy, N_BMP_SY( bmp ) );

		bmp = n_chara_swim_loop( p, bmp );

		n_chara_bmp_copy_nina( p, bmp, b, ox,oy, p->direction );

		i++;
		if ( i >= N_SPRITE_MAX ) { break; }
	}

//p->debug_pause = TRUE;


	n_chara_draw( p, b );


	return;
}




void
n_chara_land_swim_init( n_nn2 *p )
{

	n_sprite *s = &n_chara_sprite_land_swim;


	const int max_frames = 1;


	n_sprite_zero( s );

	int ox[ N_SPRITE_MAX ]; n_memory_zero( ox, N_SPRITE_MAX * sizeof( int ) );
	int oy[ N_SPRITE_MAX ]; n_memory_zero( oy, N_SPRITE_MAX * sizeof( int ) );


	int interval = 3;


	int i = 0;
	n_posix_loop
	{

		n_bmp *bmp[ N_SPRITE_MAX ];
		int    snd = 0;

		bmp[  0 ] = &p->nina_stub_body    ;
		bmp[  1 ] = &p->nina_stub_hip     ;
		bmp[  2 ] = &p->nina_stub_boob    ;
		bmp[  3 ] = &p->nina_walk_head    ;
		bmp[  4 ] = &p->nina_walk_ear     ;
		bmp[  5 ] = &p->nina_walk_hair_r  ;
		bmp[  6 ] = &p->nina_walk_neko    ;
		bmp[  7 ] = &p->nina_walk_hair_s  ;
		bmp[  8 ] = &p->nina_walk_hair_f  ;
		bmp[  9 ] = &p->nina_jump_lgS_1   ;
		bmp[ 10 ] = &p->nina_walk_leg_n   ;
		bmp[ 11 ] = &p->nina_jump_arm_2   ;
		bmp[ 12 ] = &p->nina_stub_sleeve_r;
		bmp[ 13 ] = NULL;

		n_sprite_set( s, i, bmp, ox, oy, snd, interval );

		i++;
		if ( i >= max_frames ) { break; }
	}


	return;
}

void
n_chara_land_swim_draw( n_nn2 *p )
{

	n_bmp *b = &p->bmp_shared; n_bmp_flush( b, n_bmp_black_invisible );


	n_sprite_single *obj = n_chara_get_once( p );


	int i = 0;
	n_posix_loop
	{

		if ( obj->bmp[ i ] == NULL ) { break; }

		n_bmp *bmp = obj->bmp[ i ];
		int    ox  = obj-> ox[ i ];
		int    oy  = obj-> oy[ i ];

		bmp = n_chara_swim_loop( p, bmp );

		n_chara_bmp_copy_nina( p, bmp, b, ox,oy, p->direction );

		i++;
		if ( i >= N_SPRITE_MAX ) { break; }
	}


	n_chara_draw( p, b );


	return;
}




void
n_chara_suck_init( n_nn2 *p )
{

	n_sprite *s = &n_chara_sprite_suck;


	const int max_frames = 1;


	n_sprite_zero( s );

	int ox[ N_SPRITE_MAX ]; n_memory_zero( ox, N_SPRITE_MAX * sizeof( int ) );
	int oy[ N_SPRITE_MAX ]; n_memory_zero( oy, N_SPRITE_MAX * sizeof( int ) );


	int interval = 5;


	int i = 0;
	n_posix_loop
	{

		n_bmp *bmp[ N_SPRITE_MAX ];

		bmp[  0 ] = &p->nina_stub_body    ;
		bmp[  1 ] = &p->nina_stub_hip     ;
		bmp[  2 ] = &p->nina_stub_boob    ;
		bmp[  3 ] = &p->nina_walk_head    ;
		bmp[  4 ] = &p->nina_walk_ear     ;
		bmp[  5 ] = &p->nina_walk_hair_r  ;
		bmp[  6 ] = &p->nina_walk_neko    ;
		bmp[  7 ] = &p->nina_walk_hair_s  ;
		bmp[  8 ] = &p->nina_walk_hair_f  ;
		bmp[  9 ] = &p->nina_suck_arm_1   ;
		bmp[ 10 ] = &p->nina_stub_sleeve_r;
		bmp[ 11 ] = &p->nina_suck_leg_1   ;
		bmp[ 12 ] = NULL;

		n_sprite_set( s, i, bmp, ox, oy, 0, interval );

		i++;
		if ( i >= max_frames ) { break; }
	}


	return;
}

void
n_chara_suck_draw( n_nn2 *p )
{

	n_bmp *b = &p->bmp_shared; n_bmp_flush( b, n_bmp_black_invisible );


	n_sprite_single *obj = n_chara_get_once( p );


	n_type_gfx offset = 0;//p->nina_margin_fwrd - p->nina_margin_fmax; 


	n_chara_wink_init( p );

	int i = 0;
	n_posix_loop
	{

		if ( obj->bmp[ i ] == NULL ) { break; }

		n_bmp *bmp = obj->bmp[ i ];
		int    ox  = obj-> ox[ i ];
		int    oy  = obj-> oy[ i ];

		bmp = n_chara_swim_loop( p, bmp );
		bmp = n_chara_wink_loop( p, bmp );

		if ( p->suck_direction == N_NN2_DIRECTION_LEFT )
		{
			ox -= offset;
		} else
		if ( p->suck_direction == N_NN2_DIRECTION_RIGHT )
		{
			ox += offset;
		}

		n_chara_bmp_copy_nina( p, bmp, b, ox,oy, p->direction );

		//n_nn2_sound_effect_play( obj->sound_id );

		i++;
		if ( i >= N_SPRITE_MAX ) { break; }
	}

	n_chara_wink_exit( p );


	n_chara_draw( p, b );


	return;
}




void
n_chara_turn_init( n_nn2 *p )
{

	n_sprite *s = &n_chara_sprite_turn;


	const int max_frames = 1;


	n_sprite_zero( s );

	int ox[ N_SPRITE_MAX ]; n_memory_zero( ox, N_SPRITE_MAX * sizeof( int ) );
	int oy[ N_SPRITE_MAX ]; n_memory_zero( oy, N_SPRITE_MAX * sizeof( int ) );


	int interval = 3;


	int i = 0;
	n_posix_loop
	{

		n_bmp *bmp[ N_SPRITE_MAX ];
		int    snd = 0;

		bmp[ 0 ] = &p->nina_stub_turn;
		bmp[ 1 ] = NULL;

		n_sprite_set( s, i, bmp, ox, oy, snd, interval );

		i++;
		if ( i >= max_frames ) { break; }
	}


	return;
}

void
n_chara_turn_draw( n_nn2 *p )
{

	n_bmp *b = &p->bmp_shared; n_bmp_flush( b, n_bmp_black_invisible );


	n_sprite_single *obj = n_chara_get_once( p );


	int i = 0;
	n_posix_loop
	{

		if ( obj->bmp[ i ] == NULL ) { break; }

		n_bmp *bmp = obj->bmp[ i ];
		int    ox  = obj-> ox[ i ];
		int    oy  = obj-> oy[ i ];

		bmp = n_chara_swim_loop( p, bmp );

		n_chara_bmp_copy_nina( p, bmp, b, ox,oy, p->direction );

		i++;
		if ( i >= N_SPRITE_MAX ) { break; }
	}


	n_chara_draw( p, b );


	return;
}




void
n_chara_action_idle( n_nn2 *p, BOOL rewind )
{

	p->dash_float = 0.0;


	p->sprite_cur = &n_chara_sprite_idle;
	if ( rewind ) { n_sprite_rewind( p->sprite_cur ); }


	n_chara_collision_gravity( p );


	p->draw_center = N_NN2_DRAW_IDLE;


	return;
}

void
n_chara_action_dash( n_nn2 *p )
{

	if ( p->input == 0 )
	{
		// [!] : Walk/Dash speed reducer

		static u32 timer = 0;
		if ( n_bmp_ui_timer( &timer, 100 ) )
		{
			if ( p->nina_walk_step > N_NN2_STEP_INIT )
			{
				p->nina_walk_step--;
			}

			if ( p->dash_value > 0 )
			{
				p->dash_value--;
			}
		}
	} else
	if ( p->dash_onoff )
	{
		p->dash_value++;

		if ( n_bmp_ui_timer( &p->dash_timer, p->powerup->dash_interval ) )
		{
			if ( p->nina_walk_step < N_NN2_STEP_DASH )
			{
				p->nina_walk_step += p->powerup->dash_step;
			}
		}
	}


	return;
}

void
n_chara_action_move( n_nn2 *p, int direction, BOOL is_dash )
{

	BOOL skip = FALSE;
	if ( p->idle_onoff ) { skip = TRUE; }

	if ( is_dash )
	{

		p->sprite_cur = &n_chara_sprite_dash;
		if ( skip )
		{
			static int alt = 0;
			if ( alt & 1 )
			{
				p->sprite_cur->index = 1;
			} else {
				p->sprite_cur->index = 5;
			}
			alt++;
		}

		n_chara_collision_lr( p, p->direction );

		p->draw_center = N_NN2_DRAW_DASH;

	} else {

		if ( n_bmp_ui_timer( &p->dash_timer, p->powerup->walk_interval / p->nina_walk_step ) )
		{

			static u32 axl_timer = 0;
			if ( n_bmp_ui_timer( &axl_timer, 500 ) )
			{
				if ( p->nina_walk_step < N_NN2_STEP_WALK )
				{
					p->nina_walk_step += p->powerup->walk_step;
				}
			}

			p->sprite_cur = &n_chara_sprite_walk;
			if ( skip )
			{
				static int alt = 0;
				if ( alt & 1 )
				{
					p->sprite_cur->index = 1;
				} else {
					p->sprite_cur->index = 5;
				}
				alt++;
			}

			n_chara_collision_lr( p, p->direction );

			p->draw_center = N_NN2_DRAW_WALK;
		}

	}


	return;
}

void
n_chara_action_jump_lr( n_nn2 *p )
{
//NSLog( @"%d", p->input );
//return;

	if ( ( p->fly_phase )||( p->swim_onoff ) )
	{
//n_chara_action_move( p, p->direction, YES ); return;
//return;
		if ( ( p->input & N_NN2_INPUT_L )||( p->input & N_NN2_INPUT_R ) )
		{
			n_chara_collision_lr( p, p->direction );
		}
	} else
	if ( p->input & N_NN2_INPUT_D )
	{
		if ( ( p->input & N_NN2_INPUT_L )||( p->input & N_NN2_INPUT_R ) )
		{
			goto n_goto_input;
		}
	} else
	if ( p->input )
	{
//return;
		n_goto_input:
		{

			n_type_gfx prv = p->nina_walk_step;

//NSLog( @"Dash : %d", p->dash_onoff );

			if ( p->dash_onoff == FALSE )
			{
//NSLog( @"%f", sqrt( 2 ) );
				p->nina_walk_step = p->powerup->jump_lr_walk;
			}

			n_chara_collision_lr( p, p->direction );

			p->nina_walk_step = prv;

		}

	}


	return;
}


