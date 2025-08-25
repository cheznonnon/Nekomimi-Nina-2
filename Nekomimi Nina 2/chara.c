// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




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


	n_posix_bool prv = n_bmp_is_multithread;
	n_bmp_is_multithread = n_posix_true;

	int i = 0;
	n_posix_loop
	{

		NSOperation *o = [NSBlockOperation blockOperationWithBlock:^{

		n_type_gfx fx = 0; if ( direction == N_NN2_DIRECTION_RIGHT ) { fx = N_BMP_SX( f ) - 1; }
		n_type_gfx fy = i;
		n_type_gfx tx = 0;
		n_posix_loop
		{

			n_posix_bool write_needed = n_posix_false;

			if (
				( n_bmp_ptr_is_accessible( f,      fx,      fy ) )
				&&
				( n_bmp_ptr_is_accessible( t, ox + tx, oy + fy ) )
			)
			{
				u32 color = n_bmp_composite_pixel_fast
				(
					f, t,
					fx, fy, ox + tx, oy + fy,
					n_posix_false,
					n_posix_false,
					n_posix_true,
					n_posix_false,
					p->global_blend,
					&write_needed
				);

				if ( write_needed )
				{
					n_bmp_ptr_set_fast( t, ox + tx, oy + fy, color );
				}
			}


			if ( direction == N_NN2_DIRECTION_RIGHT )
			{
				fx--;
				tx++;
				if ( fx < 0 )
				{
					fx = N_BMP_SX( f ) - 1;
					tx = 0;

					fy += p->cores;
					if ( fy >= bmpsy ) { break; }
				}
			} else {
				fx++;
				tx++;
				if ( fx >= bmpsx )
				{
					fx = 0;
					tx = 0;

					fy += p->cores;
					if ( fy >= bmpsy ) { break; }
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
			p->sprite_cur->once = n_posix_true;
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
		if ( p->wink_timer == 0 ) { n_game_timer( &p->wink_timer, 0 ); }

		p->wink       = n_game_timer( &p->wink_timer, 2000 );
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
n_chara_idle_init( n_nn2 *p, int interval )
{

	n_sprite_zero( &n_chara_sprite_idle );

	int i = 0;
	n_posix_loop
	{

		n_bmp *bmp[ N_SPRITE_MAX ];
		int     ox[ N_SPRITE_MAX ];
		int     oy[ N_SPRITE_MAX ];

		if ( i == 0 )
		{
			interval = 200;

			int n = 0;
			bmp[ n ] = &p->nina_stub_body    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_hip     ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_boob    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_head    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_hair_r  ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_neko    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_hair_s  ; ox[ n ] =  0; oy[ n ] =  0; n++; // Hair Side
			bmp[ n ] = &p->nina_walk_hair_f  ; ox[ n ] =  0; oy[ n ] =  0; n++; // Hair 1
			bmp[ n ] = &p->nina_walk_leg_n   ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_arm_n   ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_sleeve_m; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = NULL;
		} else
		if ( i == 1 )
		{
			interval = 2;

			int n = 0;
			bmp[ n ] = &p->nina_stub_body    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_hip     ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_boob    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_head    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_hair_r  ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_neko    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_hair_s  ; ox[ n ] =  0; oy[ n ] =  0; n++; // Hair Side
			bmp[ n ] = &p->nina_walk_hair_f  ; ox[ n ] =  0; oy[ n ] =  0; n++; // Hair 1
			bmp[ n ] = &p->nina_walk_leg_n   ; ox[ n ] = -1; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_arm_n   ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_sleeve_m; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = NULL;
		} else
		if ( i == 2 )
		{
			interval = 2;

			int n = 0;
			bmp[ n ] = &p->nina_stub_body    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_hip     ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_boob    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_head    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_hair_r  ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_neko    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_hair_s  ; ox[ n ] =  0; oy[ n ] =  0; n++; // Hair Side
			bmp[ n ] = &p->nina_walk_hair_f  ; ox[ n ] =  2; oy[ n ] =  0; n++; // Hair 2
			bmp[ n ] = &p->nina_walk_lgS_n   ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_idle_leg1    ; ox[ n ] = -2; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_arm_n   ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_sleeve_m; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = NULL;
		} else
		if ( i == 3 )
		{
			interval = 2;

			int n = 0;
			bmp[ n ] = &p->nina_stub_body    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_hip     ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_boob    ; ox[ n ] =  0; oy[ n ] = -2; n++;
			bmp[ n ] = &p->nina_walk_head    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_hair_r  ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_neko    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_hair_s  ; ox[ n ] =  0; oy[ n ] =  0; n++; // Hair Side
			bmp[ n ] = &p->nina_walk_hair_f  ; ox[ n ] =  2; oy[ n ] =  0; n++; // Hair 2
			bmp[ n ] = &p->nina_walk_lgS_n   ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_idle_leg2    ; ox[ n ] = -3; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_arm_n   ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_sleeve_m; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = NULL;
		} else
		if ( i == 4 )
		{
			interval = 2;

			int n = 0;
			bmp[ n ] = &p->nina_stub_body    ; ox[ n ] =  0; oy[ n ] =  2; n++;
			bmp[ n ] = &p->nina_stub_hip     ; ox[ n ] =  0; oy[ n ] =  3; n++;
			bmp[ n ] = &p->nina_stub_boob    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_head    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_hair_r  ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_neko    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_hair_s  ; ox[ n ] =  0; oy[ n ] =  0; n++; // Hair Side
			bmp[ n ] = &p->nina_walk_hair_f  ; ox[ n ] =  0; oy[ n ] = -2; n++; // Hair 3
			bmp[ n ] = &p->nina_walk_lgS_n   ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_idle_leg1    ; ox[ n ] = -2; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_arm_n   ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_sleeve_m; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = NULL;
		} else
		if ( i == 5 )
		{
			interval = 2;

			int n = 0;
			bmp[ n ] = &p->nina_stub_body    ; ox[ n ] =  0; oy[ n ] =  2; n++;
			bmp[ n ] = &p->nina_stub_hip     ; ox[ n ] =  0; oy[ n ] =  3; n++;
			bmp[ n ] = &p->nina_stub_boob    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_head    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_hair_r  ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_neko    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_hair_s  ; ox[ n ] =  0; oy[ n ] =  0; n++; // Hair Side
			bmp[ n ] = &p->nina_walk_hair_f  ; ox[ n ] =  0; oy[ n ] = -2; n++; // Hair 3
			bmp[ n ] = &p->nina_walk_lgS_n   ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_idle_leg1    ; ox[ n ] = -2; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_arm_n   ; ox[ n ] =  0; oy[ n ] = -2; n++;
			bmp[ n ] = &p->nina_stub_sleeve_m; ox[ n ] =  0; oy[ n ] = -1; n++;
			bmp[ n ] = NULL;
		} else
		if ( i == 6 )
		{
			interval = 2;

			int n = 0;
			bmp[ n ] = &p->nina_stub_body    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_hip     ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_boob    ; ox[ n ] =  0; oy[ n ] = -2; n++;
			bmp[ n ] = &p->nina_walk_head    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_hair_r  ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_neko    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_hair_s  ; ox[ n ] =  0; oy[ n ] =  0; n++; // Hair Side
			bmp[ n ] = &p->nina_walk_hair_f  ; ox[ n ] =  2; oy[ n ] =  0; n++; // Hair 2
			bmp[ n ] = &p->nina_walk_lgS_n   ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_idle_leg2    ; ox[ n ] = -3; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_arm_n   ; ox[ n ] =  0; oy[ n ] = -2; n++;
			bmp[ n ] = &p->nina_stub_sleeve_m; ox[ n ] =  0; oy[ n ] = -1; n++;
			bmp[ n ] = NULL;
		} else
		if ( i == 7 )
		{
			interval = 2;

			int n = 0;
			bmp[ n ] = &p->nina_stub_body    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_hip     ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_boob    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_head    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_hair_r  ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_neko    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_hair_s  ; ox[ n ] =  0; oy[ n ] =  0; n++; // Hair Side
			bmp[ n ] = &p->nina_walk_hair_f  ; ox[ n ] =  2; oy[ n ] =  0; n++; // Hair 2
			bmp[ n ] = &p->nina_walk_lgS_n   ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_idle_leg1    ; ox[ n ] = -2; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_arm_n   ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_sleeve_m; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = NULL;
		} else {
			break;
		}

		n_sprite_set( &n_chara_sprite_idle, i, bmp, ox, oy, 0, interval );

		i++;
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
				bmp = &p->nina_walk_arm_r_2;
			} else
			if ( bmp == &p->nina_jump_arm_1 )
			{
				bmp = &p->nina_walk_arm_r_2;
			} else
			if ( bmp == &p->nina_jump_arm_2 )
			{
				bmp = &p->nina_walk_arm_r_2;
			}
		}

		if ( n_object_treasure_is_running( p ) )
		{
			if ( bmp == &p->nina_walk_arm_n )
			{
				bmp = &p->nina_walk_arm_r_2;
			} else
			if ( bmp == &p->nina_jump_arm_1 )
			{
				bmp = &p->nina_walk_arm_r_2;
			} else
			if ( bmp == &p->nina_jump_arm_2 )
			{
				bmp = &p->nina_walk_arm_r_2;
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
n_chara_idle_cliff_init( n_nn2 *p, int interval )
{

	n_sprite_zero( &n_chara_sprite_idle_cliff );

	int i = 0;
	n_posix_loop
	{

		n_bmp *bmp[ N_SPRITE_MAX ];
		int     ox[ N_SPRITE_MAX ];
		int     oy[ N_SPRITE_MAX ];

		if ( i == 0 )
		{
			int n = 0;
			bmp[ n ] = &p->nina_stub_body    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_hip     ; ox[ n ] =  1; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_boob    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_head    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_hair_r  ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_neko    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_hair_s  ; ox[ n ] =  0; oy[ n ] =  0; n++; // Hair Side
			bmp[ n ] = &p->nina_walk_hair_f  ; ox[ n ] =  0; oy[ n ] =  0; n++; // Hair 1
			bmp[ n ] = &p->nina_idle_cliff   ; ox[ n ] =  1; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_jump_arm_1   ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_sleeve_m; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_cliff_sweat  ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = NULL;
		} else
		if ( i == 1 )
		{
			int n = 0;
			bmp[ n ] = &p->nina_stub_body    ; ox[ n ] =  0; oy[ n ] = -1; n++;
			bmp[ n ] = &p->nina_stub_hip     ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_boob    ; ox[ n ] =  0; oy[ n ] = -1; n++;
			bmp[ n ] = &p->nina_walk_head    ; ox[ n ] =  1; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_hair_r  ; ox[ n ] =  1; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_neko    ; ox[ n ] =  1; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_hair_s  ; ox[ n ] =  1; oy[ n ] =  0; n++; // Hair Side
			bmp[ n ] = &p->nina_walk_hair_f  ; ox[ n ] =  1; oy[ n ] =  0; n++; // Hair 1
			bmp[ n ] = &p->nina_idle_cliff   ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_jump_arm_2   ; ox[ n ] =  1; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_sleeve_m; ox[ n ] =  1; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_cliff_sweat  ; ox[ n ] = -6; oy[ n ] = -6; n++;
			bmp[ n ] = NULL;
		} else
		if ( i == 2 )
		{
			int n = 0;
			bmp[ n ] = &p->nina_stub_body    ; ox[ n ] =  0; oy[ n ] = -1; n++;
			bmp[ n ] = &p->nina_stub_hip     ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_boob    ; ox[ n ] =  0; oy[ n ] = -1; n++;
			bmp[ n ] = &p->nina_walk_head    ; ox[ n ] =  1; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_hair_r  ; ox[ n ] =  1; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_neko    ; ox[ n ] =  1; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_hair_s  ; ox[ n ] =  1; oy[ n ] =  0; n++; // Hair Side
			bmp[ n ] = &p->nina_walk_hair_f  ; ox[ n ] =  1; oy[ n ] =  0; n++; // Hair 1
			bmp[ n ] = &p->nina_idle_cliff   ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_jump_arm_1   ; ox[ n ] =  1; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_sleeve_m; ox[ n ] =  1; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_cliff_sweat  ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = NULL;
		} else
		//
		{
			break;
		}

		n_sprite_set( &n_chara_sprite_idle_cliff, i, bmp, ox, oy, 0, interval );

		i++;
	}


	return;
}




void
n_chara_walk_init( n_nn2 *p, int interval )
{

	n_sprite_zero( &n_chara_sprite_walk );


	int i = 0;
	n_posix_loop
	{

		n_bmp *bmp[ N_SPRITE_MAX ];
		int     ox[ N_SPRITE_MAX ];
		int     oy[ N_SPRITE_MAX ];
		int    snd = 0;

		if ( i == 0 )
		{
			int n = 0;
			bmp[ n ] = &p->nina_stub_body    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_hip     ; ox[ n ] =  0; oy[ n ] = -1; n++;
			bmp[ n ] = &p->nina_stub_boob    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_head    ; ox[ n ] =  0; oy[ n ] =  2; n++;
			bmp[ n ] = &p->nina_walk_hair_r  ; ox[ n ] =  0; oy[ n ] =  2; n++;
			bmp[ n ] = &p->nina_walk_neko    ; ox[ n ] =  0; oy[ n ] =  2; n++;
			bmp[ n ] = &p->nina_walk_hair_s  ; ox[ n ] =  0; oy[ n ] =  2; n++; // Hair Side
			bmp[ n ] = &p->nina_walk_hair_f  ; ox[ n ] =  0; oy[ n ] =  1; n++; // Hair 1
			bmp[ n ] = &p->nina_walk_leg_n   ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_arm_n   ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_sleeve_m; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = NULL;
		} else
		if ( i == 1 )
		{
			int n = 0;
			bmp[ n ] = &p->nina_walk_amS_r_1 ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_lgS_r_2 ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_body    ; ox[ n ] =  0; oy[ n ] = -1; n++;
			bmp[ n ] = &p->nina_stub_hip     ; ox[ n ] =  0; oy[ n ] = -2; n++;
			bmp[ n ] = &p->nina_stub_boob    ; ox[ n ] =  0; oy[ n ] =  1; n++;
			bmp[ n ] = &p->nina_walk_head    ; ox[ n ] =  0; oy[ n ] =  2; n++;
			bmp[ n ] = &p->nina_walk_hair_r  ; ox[ n ] =  0; oy[ n ] =  2; n++;
			bmp[ n ] = &p->nina_walk_neko    ; ox[ n ] =  0; oy[ n ] =  2; n++;
			bmp[ n ] = &p->nina_walk_hair_s  ; ox[ n ] =  0; oy[ n ] =  2; n++; // Hair Side
			bmp[ n ] = &p->nina_walk_hair_f  ; ox[ n ] =  0; oy[ n ] =  1; n++; // Hair 1
			bmp[ n ] = &p->nina_walk_leg_f_2 ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_arm_f_1 ; ox[ n ] =  0; oy[ n ] = -1; n++;
			bmp[ n ] = &p->nina_stub_sleeve_r; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = NULL;
			snd = N_NN2_SOUND_WALK;
		} else
		if ( i == 2 )
		{
			int n = 0;
			bmp[ n ] = &p->nina_walk_amS_r_2 ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_lgS_r_3 ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_body    ; ox[ n ] =  0; oy[ n ] = -1; n++;
			bmp[ n ] = &p->nina_stub_hip     ; ox[ n ] =  0; oy[ n ] = -2; n++;
			bmp[ n ] = &p->nina_stub_boob    ; ox[ n ] =  0; oy[ n ] =  2; n++;
			bmp[ n ] = &p->nina_walk_head    ; ox[ n ] =  0; oy[ n ] =  1; n++;
			bmp[ n ] = &p->nina_walk_hair_r  ; ox[ n ] =  0; oy[ n ] =  1; n++;
			bmp[ n ] = &p->nina_walk_neko    ; ox[ n ] =  0; oy[ n ] =  1; n++;
			bmp[ n ] = &p->nina_walk_hair_s  ; ox[ n ] =  0; oy[ n ] =  1; n++; // Hair Side
			bmp[ n ] = &p->nina_walk_hair_f  ; ox[ n ] =  0; oy[ n ] =  1; n++; // Hair 1
			bmp[ n ] = &p->nina_walk_leg_f_3 ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_arm_f_2 ; ox[ n ] =  0; oy[ n ] = -1; n++;
			bmp[ n ] = &p->nina_stub_sleeve_r; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = NULL;
		} else
		if ( i == 3 )
		{
			int n = 0;
			bmp[ n ] = &p->nina_walk_amS_r_1 ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_lgS_r_2 ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_body    ; ox[ n ] =  0; oy[ n ] = -2; n++;
			bmp[ n ] = &p->nina_stub_hip     ; ox[ n ] =  0; oy[ n ] = -3; n++;
			bmp[ n ] = &p->nina_stub_boob    ; ox[ n ] =  0; oy[ n ] =  1; n++;
			bmp[ n ] = &p->nina_walk_head    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_hair_r  ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_neko    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_hair_s  ; ox[ n ] =  0; oy[ n ] =  0; n++; // Hair Side
			bmp[ n ] = &p->nina_walk_hair_f  ; ox[ n ] =  0; oy[ n ] =  0; n++; // Hair 1
			bmp[ n ] = &p->nina_walk_leg_fd2 ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_arm_f_1 ; ox[ n ] =  0; oy[ n ] = -2; n++;
			bmp[ n ] = &p->nina_stub_sleeve_r; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = NULL;
		} else
		if ( i == 4 )
		{
			int n = 0;
			bmp[ n ] = &p->nina_stub_body    ; ox[ n ] =  0; oy[ n ] = -2; n++;
			bmp[ n ] = &p->nina_stub_hip     ; ox[ n ] =  0; oy[ n ] = -3; n++;
			bmp[ n ] = &p->nina_stub_boob    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_head    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_hair_r  ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_neko    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_hair_s  ; ox[ n ] =  0; oy[ n ] =  0; n++; // Hair Side
			bmp[ n ] = &p->nina_walk_hair_f  ; ox[ n ] =  0; oy[ n ] =  0; n++; // Hair 1
			bmp[ n ] = &p->nina_walk_leg_n   ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_arm_n   ; ox[ n ] =  0; oy[ n ] = -2; n++;
			bmp[ n ] = &p->nina_stub_sleeve_m; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = NULL;
		} else
		if ( i == 5 )
		{
			int n = 0;
			bmp[ n ] = &p->nina_walk_amS_f_1 ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_lgS_f_2 ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_body    ; ox[ n ] =  0; oy[ n ] = -1; n++;
			bmp[ n ] = &p->nina_stub_hip     ; ox[ n ] =  0; oy[ n ] = -2; n++;
			bmp[ n ] = &p->nina_stub_boob    ; ox[ n ] =  0; oy[ n ] =  1; n++;
			bmp[ n ] = &p->nina_walk_head    ; ox[ n ] =  0; oy[ n ] =  1; n++;
			bmp[ n ] = &p->nina_walk_hair_r  ; ox[ n ] =  0; oy[ n ] =  1; n++;
			bmp[ n ] = &p->nina_walk_neko    ; ox[ n ] =  0; oy[ n ] =  1; n++;
			bmp[ n ] = &p->nina_walk_hair_s  ; ox[ n ] =  0; oy[ n ] =  1; n++; // Hair Side
			bmp[ n ] = &p->nina_walk_hair_f  ; ox[ n ] =  0; oy[ n ] =  1; n++; // Hair 1
			bmp[ n ] = &p->nina_walk_leg_r_2 ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_arm_r_1 ; ox[ n ] =  0; oy[ n ] = -1; n++;
			bmp[ n ] = &p->nina_stub_sleeve_l; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = NULL;
			snd = N_NN2_SOUND_WALK;
		} else
		if ( i == 6 )
		{
			int n = 0;
			bmp[ n ] = &p->nina_walk_amS_f_2 ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_lgS_f_3 ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_body    ; ox[ n ] =  0; oy[ n ] = -1; n++;
			bmp[ n ] = &p->nina_stub_hip     ; ox[ n ] =  0; oy[ n ] = -2; n++;
			bmp[ n ] = &p->nina_stub_boob    ; ox[ n ] =  0; oy[ n ] =  2; n++;
			bmp[ n ] = &p->nina_walk_head    ; ox[ n ] =  0; oy[ n ] =  2; n++;
			bmp[ n ] = &p->nina_walk_hair_r  ; ox[ n ] =  0; oy[ n ] =  2; n++;
			bmp[ n ] = &p->nina_walk_neko    ; ox[ n ] =  0; oy[ n ] =  2; n++;
			bmp[ n ] = &p->nina_walk_hair_s  ; ox[ n ] =  0; oy[ n ] =  2; n++; // Hair Side
			bmp[ n ] = &p->nina_walk_hair_f  ; ox[ n ] =  0; oy[ n ] =  1; n++; // Hair 1
			bmp[ n ] = &p->nina_walk_leg_r_3 ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_arm_r_2 ; ox[ n ] =  0; oy[ n ] = -1; n++;
			bmp[ n ] = &p->nina_stub_sleeve_l; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = NULL;
		} else
		if ( i == 7 )
		{
			int n = 0;
			bmp[ n ] = &p->nina_walk_amS_f_1 ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_lgS_fd2 ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_body    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_hip     ; ox[ n ] =  0; oy[ n ] = -1; n++;
			bmp[ n ] = &p->nina_stub_boob    ; ox[ n ] =  0; oy[ n ] =  1; n++;
			bmp[ n ] = &p->nina_walk_head    ; ox[ n ] =  0; oy[ n ] =  2; n++;
			bmp[ n ] = &p->nina_walk_hair_r  ; ox[ n ] =  0; oy[ n ] =  2; n++;
			bmp[ n ] = &p->nina_walk_neko    ; ox[ n ] =  0; oy[ n ] =  2; n++;
			bmp[ n ] = &p->nina_walk_hair_s  ; ox[ n ] =  0; oy[ n ] =  2; n++; // Hair Side
			bmp[ n ] = &p->nina_walk_hair_f  ; ox[ n ] =  0; oy[ n ] =  1; n++; // Hair 1
			bmp[ n ] = &p->nina_walk_leg_r_2 ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_arm_r_1 ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_sleeve_l; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = NULL;
		} else { break; }

		n_sprite_set( &n_chara_sprite_walk, i, bmp, ox, oy, snd, interval );

		i++;
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
				bmp = &p->nina_walk_arm_r_2;
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
n_chara_dash_init( n_nn2 *p, int interval )
{
//n_chara_sprite_dash = n_chara_sprite_walk; return;


	n_sprite_zero( &n_chara_sprite_dash );


	// [!] : slow motion for debug

	//interval = 20;


	int i = 0;
	n_posix_loop
	{

		n_bmp *bmp[ N_SPRITE_MAX ];
		int     ox[ N_SPRITE_MAX ];
		int     oy[ N_SPRITE_MAX ];
		int    snd = 0;

		if ( i == 0 )
		{
			int n = 0;
			bmp[ n ] = &p->nina_stub         ; ox[ n ] =  0; oy[ n ] =  0; n++; // arm
			bmp[ n ] = &p->nina_stub         ; ox[ n ] =  0; oy[ n ] =  0; n++; // leg
			bmp[ n ] = &p->nina_stub_body    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_hip     ; ox[ n ] = -2; oy[ n ] = -1; n++;
			bmp[ n ] = &p->nina_stub_boob    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_head    ; ox[ n ] =  0; oy[ n ] =  2; n++;
			bmp[ n ] = &p->nina_walk_hair_r  ; ox[ n ] =  0; oy[ n ] =  2; n++;
			bmp[ n ] = &p->nina_walk_neko    ; ox[ n ] =  0; oy[ n ] =  2; n++;
			bmp[ n ] = &p->nina_walk_hair_s  ; ox[ n ] =  0; oy[ n ] =  2; n++; // Hair Side
			bmp[ n ] = &p->nina_walk_hair_f  ; ox[ n ] =  0; oy[ n ] =  0; n++; // Hair 1
			bmp[ n ] = &p->nina_walk_leg_n   ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_dash_arm_n   ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_sleeve_m; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = NULL;
		} else
		if ( i == 1 )
		{
			int n = 0;
			bmp[ n ] = &p->nina_dash_amS_r_1 ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_dash_lgS_f_2 ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_body    ; ox[ n ] =  0; oy[ n ] = -1; n++;
			bmp[ n ] = &p->nina_stub_hip     ; ox[ n ] = -2; oy[ n ] = -2; n++;
			bmp[ n ] = &p->nina_stub_boob    ; ox[ n ] =  0; oy[ n ] =  1; n++;
			bmp[ n ] = &p->nina_walk_head    ; ox[ n ] =  0; oy[ n ] =  2; n++;
			bmp[ n ] = &p->nina_walk_hair_r  ; ox[ n ] =  0; oy[ n ] =  2; n++;
			bmp[ n ] = &p->nina_walk_neko    ; ox[ n ] =  0; oy[ n ] =  2; n++;
			bmp[ n ] = &p->nina_walk_hair_s  ; ox[ n ] =  0; oy[ n ] =  2; n++; // Hair Side
			bmp[ n ] = &p->nina_walk_hair_f  ; ox[ n ] = -1; oy[ n ] =  1; n++; // Hair 1
			bmp[ n ] = &p->nina_dash_arm_f_1 ; ox[ n ] =  0; oy[ n ] = -1; n++;
			bmp[ n ] = &p->nina_dash_leg_r_1 ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_sleeve_l; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = NULL;
			snd = N_NN2_SOUND_WALK;
		} else
		if ( i == 2 )
		{
			int n = 0;
			bmp[ n ] = &p->nina_dash_amS_r_2 ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_dash_lgS_f_1 ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_body    ; ox[ n ] =  0; oy[ n ] = -1; n++;
			bmp[ n ] = &p->nina_stub_hip     ; ox[ n ] = -2; oy[ n ] = -2; n++;
			bmp[ n ] = &p->nina_stub_boob    ; ox[ n ] =  0; oy[ n ] =  2; n++;
			bmp[ n ] = &p->nina_walk_head    ; ox[ n ] =  0; oy[ n ] =  1; n++;
			bmp[ n ] = &p->nina_walk_hair_r  ; ox[ n ] =  0; oy[ n ] =  1; n++;
			bmp[ n ] = &p->nina_walk_neko    ; ox[ n ] =  0; oy[ n ] =  1; n++;
			bmp[ n ] = &p->nina_walk_hair_s  ; ox[ n ] =  0; oy[ n ] =  1; n++; // Hair Side
			bmp[ n ] = &p->nina_walk_hair_f  ; ox[ n ] = -1; oy[ n ] =  2; n++; // Hair 1
			bmp[ n ] = &p->nina_dash_arm_f_2 ; ox[ n ] =  0; oy[ n ] = -1; n++;
			bmp[ n ] = &p->nina_dash_leg_r_2 ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_sleeve_l; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = NULL;
		} else
		if ( i == 3 )
		{
			int n = 0;
			bmp[ n ] = &p->nina_dash_amS_r_1 ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_dash_lgS_f_2 ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_body    ; ox[ n ] =  0; oy[ n ] = -2; n++;
			bmp[ n ] = &p->nina_stub_hip     ; ox[ n ] = -2; oy[ n ] = -3; n++;
			bmp[ n ] = &p->nina_stub_boob    ; ox[ n ] =  0; oy[ n ] =  1; n++;
			bmp[ n ] = &p->nina_walk_head    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_hair_r  ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_neko    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_hair_s  ; ox[ n ] =  0; oy[ n ] =  0; n++; // Hair Side
			bmp[ n ] = &p->nina_walk_hair_f  ; ox[ n ] = -1; oy[ n ] =  3; n++; // Hair 1
			bmp[ n ] = &p->nina_dash_arm_f_1 ; ox[ n ] =  0; oy[ n ] = -2; n++;
			bmp[ n ] = &p->nina_dash_leg_r_1 ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_sleeve_l; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = NULL;
		} else
		if ( i == 4 )
		{
			int n = 0;
			bmp[ n ] = &p->nina_stub         ; ox[ n ] =  0; oy[ n ] =  0; n++; // arm
			bmp[ n ] = &p->nina_stub         ; ox[ n ] =  0; oy[ n ] =  0; n++; // leg
			bmp[ n ] = &p->nina_stub_body    ; ox[ n ] =  0; oy[ n ] = -2; n++;
			bmp[ n ] = &p->nina_stub_hip     ; ox[ n ] = -2; oy[ n ] = -3; n++;
			bmp[ n ] = &p->nina_stub_boob    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_head    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_hair_r  ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_neko    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_hair_s  ; ox[ n ] =  0; oy[ n ] =  0; n++; // Hair Side
			bmp[ n ] = &p->nina_walk_hair_f  ; ox[ n ] = -0; oy[ n ] =  4; n++; // Hair 1
			bmp[ n ] = &p->nina_walk_leg_n   ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_dash_arm_n   ; ox[ n ] =  0; oy[ n ] = -2; n++;
			bmp[ n ] = &p->nina_stub_sleeve_m; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = NULL;
		} else
		if ( i == 5 )
		{
			int n = 0;
			bmp[ n ] = &p->nina_dash_amS_f_1 ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_dash_lgS_r_1 ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_body    ; ox[ n ] =  0; oy[ n ] = -2; n++;
			bmp[ n ] = &p->nina_stub_hip     ; ox[ n ] = -2; oy[ n ] = -2; n++;
			bmp[ n ] = &p->nina_stub_boob    ; ox[ n ] =  0; oy[ n ] =  1; n++;
			bmp[ n ] = &p->nina_walk_head    ; ox[ n ] =  0; oy[ n ] =  1; n++;
			bmp[ n ] = &p->nina_walk_hair_r  ; ox[ n ] =  0; oy[ n ] =  1; n++;
			bmp[ n ] = &p->nina_walk_neko    ; ox[ n ] =  0; oy[ n ] =  1; n++;
			bmp[ n ] = &p->nina_walk_hair_s  ; ox[ n ] =  0; oy[ n ] =  1; n++; // Hair Side
			bmp[ n ] = &p->nina_walk_hair_f  ; ox[ n ] = -1; oy[ n ] =  3; n++; // Hair 1
			bmp[ n ] = &p->nina_dash_arm_r_1 ; ox[ n ] =  0; oy[ n ] = -1; n++;
			bmp[ n ] = &p->nina_dash_leg_f_2 ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_sleeve_r; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = NULL;
			snd = N_NN2_SOUND_WALK;
		} else
		if ( i == 6 )
		{
			int n = 0;
			bmp[ n ] = &p->nina_dash_amS_f_2 ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_dash_lgS_r_2 ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_body    ; ox[ n ] =  0; oy[ n ] = -1; n++;
			bmp[ n ] = &p->nina_stub_hip     ; ox[ n ] = -2; oy[ n ] = -2; n++;
			bmp[ n ] = &p->nina_stub_boob    ; ox[ n ] =  0; oy[ n ] =  2; n++;
			bmp[ n ] = &p->nina_walk_head    ; ox[ n ] =  0; oy[ n ] =  2; n++;
			bmp[ n ] = &p->nina_walk_hair_r  ; ox[ n ] =  0; oy[ n ] =  2; n++;
			bmp[ n ] = &p->nina_walk_neko    ; ox[ n ] =  0; oy[ n ] =  2; n++;
			bmp[ n ] = &p->nina_walk_hair_s  ; ox[ n ] =  0; oy[ n ] =  2; n++; // Hair Side
			bmp[ n ] = &p->nina_walk_hair_f  ; ox[ n ] = -1; oy[ n ] =  2; n++; // Hair 1
			bmp[ n ] = &p->nina_dash_arm_r_2 ; ox[ n ] =  0; oy[ n ] = -1; n++;
			bmp[ n ] = &p->nina_dash_leg_f_1 ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_sleeve_r; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = NULL;
		} else
		if ( i == 7 )
		{
			int n = 0;
			bmp[ n ] = &p->nina_dash_amS_f_1 ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_dash_lgS_r_1 ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_body    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_hip     ; ox[ n ] = -2; oy[ n ] = -1; n++;
			bmp[ n ] = &p->nina_stub_boob    ; ox[ n ] =  0; oy[ n ] =  1; n++;
			bmp[ n ] = &p->nina_walk_head    ; ox[ n ] =  0; oy[ n ] =  2; n++;
			bmp[ n ] = &p->nina_walk_hair_r  ; ox[ n ] =  0; oy[ n ] =  2; n++;
			bmp[ n ] = &p->nina_walk_neko    ; ox[ n ] =  0; oy[ n ] =  2; n++;
			bmp[ n ] = &p->nina_walk_hair_s  ; ox[ n ] =  0; oy[ n ] =  2; n++; // Hair Side
			bmp[ n ] = &p->nina_walk_hair_f  ; ox[ n ] = -1; oy[ n ] =  1; n++; // Hair 1
			bmp[ n ] = &p->nina_dash_arm_r_1 ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_dash_leg_f_2 ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_sleeve_r; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = NULL;
		} else { break; }

		n_sprite_set( &n_chara_sprite_dash, i, bmp, ox, oy, snd, interval );
//if ( i == 5 ) { break; }

		i++;
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
		int    ox  = obj-> ox[ i ] / p->scaler;
		int    oy  = obj-> oy[ i ] / p->scaler;

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
				bmp = &p->nina_walk_arm_r_2;
			}
		}

		bmp = n_chara_swim_loop( p, bmp );
		bmp = n_chara_wink_loop( p, bmp );

		oy *= 2;

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
n_chara_slip_init( n_nn2 *p, int interval )
{

	n_sprite_zero( &n_chara_sprite_slip );

	int i = 0;
	n_posix_loop
	{

		n_bmp *bmp[ N_SPRITE_MAX ];
		int     ox[ N_SPRITE_MAX ];
		int     oy[ N_SPRITE_MAX ];
		int    snd = 0;

		if ( i == 0 )
		{
			int n = 0;
			bmp[ n ] = &p->nina_stub_body    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_hip     ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_boob    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_head    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_hair_r  ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_neko    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_hair_s  ; ox[ n ] =  0; oy[ n ] =  0; n++; // Hair Side
			bmp[ n ] = &p->nina_walk_hair_f  ; ox[ n ] =  0; oy[ n ] =  0; n++; // Hair 1
			bmp[ n ] = &p->nina_walk_leg_f_2 ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_arm_f_2 ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_sleeve_l; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = NULL;
			//snd = N_NN2_SOUND_BRAKE;
		} else {
			break;
		}

		n_sprite_set( &n_chara_sprite_slip, i, bmp, ox, oy, snd, interval );

		i++;
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
			if ( bmp == &p->nina_walk_arm_f_2 )
			{
				bmp = &p->nina_walk_arm_r_2;
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
n_chara_duck_init( n_nn2 *p, int interval )
{

	n_sprite_zero( &n_chara_sprite_duck );

	n_type_gfx hf = N_NN2_STEP_DUCK_HALF;
	n_type_gfx fl = N_NN2_STEP_DUCK_FULL;

	int i = 0;
	n_posix_loop
	{

		n_bmp *bmp[ N_SPRITE_MAX ];
		int     ox[ N_SPRITE_MAX ];
		int     oy[ N_SPRITE_MAX ];

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
		} else {
			break;
		}

		n_sprite_set( &n_chara_sprite_duck, i, bmp, ox, oy, 0, interval );

		i++;
	}


	return;
}

void
n_chara_unduck_init( n_nn2 *p, int interval )
{

	n_sprite_zero( &n_chara_sprite_unduck );

	n_type_gfx hf = N_NN2_STEP_DUCK_HALF;
	n_type_gfx fl = N_NN2_STEP_DUCK_FULL;

	int i = 0;
	n_posix_loop
	{

		n_bmp *bmp[ N_SPRITE_MAX ];
		int     ox[ N_SPRITE_MAX ];
		int     oy[ N_SPRITE_MAX ];

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
		} else
		//
		{
			break;
		}

		n_sprite_set( &n_chara_sprite_unduck, i, bmp, ox, oy, 0, interval );

		i++;
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
n_chara_jump_init( n_nn2 *p, int interval )
{

	n_sprite_zero( &n_chara_sprite_jump );

	int i = 0;
	n_posix_loop
	{

		n_bmp *bmp[ N_SPRITE_MAX ];
		int     ox[ N_SPRITE_MAX ];
		int     oy[ N_SPRITE_MAX ];
		int    snd = 0;

		if ( i == 0 )
		{
			int n = 0;
			bmp[ n ] = &p->nina_stub_body    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_hip     ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_boob    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_head    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_ear     ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_hair_r  ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_neko    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_hair_s  ; ox[ n ] =  0; oy[ n ] =  0; n++; // Hair Side
			bmp[ n ] = &p->nina_walk_hair_f  ; ox[ n ] =  0; oy[ n ] =  0; n++; // Hair 1
			bmp[ n ] = &p->nina_jump_lgS_1   ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_jump_leg_3   ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_jump_arm_1   ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_sleeve_r; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = NULL;
		} else
		if ( i == 1 )
		{
			int n = 0;
			bmp[ n ] = &p->nina_stub_body    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_hip     ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_boob    ; ox[ n ] =  0; oy[ n ] =  1; n++;
			bmp[ n ] = &p->nina_walk_head    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_ear     ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_hair_r  ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_neko    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_hair_s  ; ox[ n ] =  0; oy[ n ] =  0; n++; // Hair Side
			bmp[ n ] = &p->nina_walk_hair_f  ; ox[ n ] =  0; oy[ n ] =  0; n++; // Hair 2
			bmp[ n ] = &p->nina_jump_leg_2   ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_jump_arm_1   ; ox[ n ] =  1; oy[ n ] =  1; n++;
			bmp[ n ] = &p->nina_stub_sleeve_r; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = NULL;
		} else
		if ( i == 2 )
		{
			int n = 0;
			bmp[ n ] = &p->nina_stub_body    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_hip     ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_boob    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_head    ; ox[ n ] =  0; oy[ n ] =  1; n++;
			bmp[ n ] = &p->nina_walk_ear     ; ox[ n ] =  0; oy[ n ] =  1; n++;
			bmp[ n ] = &p->nina_walk_hair_r  ; ox[ n ] =  0; oy[ n ] =  1; n++;
			bmp[ n ] = &p->nina_walk_neko    ; ox[ n ] =  0; oy[ n ] =  1; n++;
			bmp[ n ] = &p->nina_walk_hair_s  ; ox[ n ] =  0; oy[ n ] =  1; n++; // Hair Side
			bmp[ n ] = &p->nina_walk_hair_f  ; ox[ n ] =  0; oy[ n ] =  1; n++; // Hair 3
			bmp[ n ] = &p->nina_jump_lgS_3   ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_jump_leg_1   ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_jump_arm_1   ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_sleeve_r; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = NULL;
		} else
		if ( i == 3 )
		{
			int n = 0;
			bmp[ n ] = &p->nina_stub_body    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_hip     ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_boob    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_head    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_ear     ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_hair_r  ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_neko    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_hair_s  ; ox[ n ] =  0; oy[ n ] =  0; n++; // Hair Side
			bmp[ n ] = &p->nina_walk_hair_f  ; ox[ n ] =  0; oy[ n ] =  0; n++; // Hair 2
			bmp[ n ] = &p->nina_jump_leg_2   ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_jump_arm_1   ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_sleeve_r; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = NULL;
		} else {
			break;
		}

		n_sprite_set( &n_chara_sprite_jump, i, bmp, ox, oy, snd, interval );

		i++;
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
				bmp = &p->nina_walk_arm_r_2;
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
n_chara_land_init( n_nn2 *p, int interval )
{

	n_sprite_zero( &n_chara_sprite_land );
	
	n_type_gfx hf = N_NN2_STEP_DUCK_HALF;

	int i = 0;
	n_posix_loop
	{

		n_bmp *bmp[ N_SPRITE_MAX ];
		int     ox[ N_SPRITE_MAX ];
		int     oy[ N_SPRITE_MAX ];
		int    snd = 0;

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
		} else {
			break;
		}

		n_sprite_set( &n_chara_sprite_land, i, bmp, ox, oy, snd, interval );

		i++;
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
n_chara_land_swim_init( n_nn2 *p, int interval )
{

	n_sprite_zero( &n_chara_sprite_land_swim );

	int i = 0;
	n_posix_loop
	{

		n_bmp *bmp[ N_SPRITE_MAX ];
		int     ox[ N_SPRITE_MAX ];
		int     oy[ N_SPRITE_MAX ];
		int    snd = 0;

		if ( i == 0 )
		{
			int n = 0;
			bmp[ n ] = &p->nina_stub_body    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_hip     ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_boob    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_head    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_ear     ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_hair_r  ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_neko    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_hair_s  ; ox[ n ] =  0; oy[ n ] =  0; n++; // Hair Side
			bmp[ n ] = &p->nina_walk_hair_f  ; ox[ n ] =  0; oy[ n ] =  0; n++; // Hair 1
			bmp[ n ] = &p->nina_jump_lgS_1   ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_leg_n   ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_jump_arm_2   ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_sleeve_r; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = NULL;
		} else
		//
		{
			break;
		}

		n_sprite_set( &n_chara_sprite_land_swim, i, bmp, ox, oy, snd, interval );

		i++;
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
n_chara_suck_init( n_nn2 *p, int interval )
{

	n_sprite_zero( &n_chara_sprite_suck );

	int i = 0;
	n_posix_loop
	{

		n_bmp *bmp[ N_SPRITE_MAX ];
		int     ox[ N_SPRITE_MAX ];
		int     oy[ N_SPRITE_MAX ];
		int    snd = 0;

		if ( i == 0 )
		{
			int n = 0;
			bmp[ n ] = &p->nina_stub_body    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_hip     ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_boob    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_head    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_ear     ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_hair_r  ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_neko    ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_walk_hair_s  ; ox[ n ] =  0; oy[ n ] =  0; n++; // Hair Side
			bmp[ n ] = &p->nina_walk_hair_f  ; ox[ n ] =  0; oy[ n ] =  0; n++; // Hair 1
			bmp[ n ] = &p->nina_suck_arm_1   ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_stub_sleeve_r; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = &p->nina_suck_leg_1   ; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = NULL;
		} else {
			break;
		}

		n_sprite_set( &n_chara_sprite_suck, i, bmp, ox, oy, snd, interval );

		i++;
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
n_chara_turn_init( n_nn2 *p, int interval )
{

	n_sprite_zero( &n_chara_sprite_turn );

	int i = 0;
	n_posix_loop
	{

		n_bmp *bmp[ N_SPRITE_MAX ];
		int     ox[ N_SPRITE_MAX ];
		int     oy[ N_SPRITE_MAX ];
		int    snd = 0;

		if ( i == 0 )
		{
			int n = 0;
			bmp[ n ] = &p->nina_stub_turn; ox[ n ] =  0; oy[ n ] =  0; n++;
			bmp[ n ] = NULL;
		} else {
			break;
		}

		n_sprite_set( &n_chara_sprite_turn, i, bmp, ox, oy, snd, interval );

		i++;
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
		if ( n_game_timer( &timer, 100 ) )
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

		if ( n_game_timer( &p->dash_timer, p->powerup->dash_interval ) )
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

		if ( n_game_timer( &p->dash_timer, p->powerup->walk_interval / p->nina_walk_step ) )
		{

			static u32 axl_timer = 0;
			if ( n_game_timer( &axl_timer, 500 ) )
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


