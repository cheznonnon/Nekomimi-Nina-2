// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




void
n_object_treasure_open( n_nn2 *p )
{
#ifndef N_NN2_TREASURE_ONOFF
	return;
#endif
//return;

	n_type_gfx x = p->stage->treasure_x;
	n_type_gfx y = p->stage->treasure_y; y += 2;

	n_bmp_ptr_set( &p->stage->map, x+0, y+0, n_bmp_black );
	n_bmp_ptr_set( &p->stage->map, x+1, y+0, n_bmp_black );
	n_bmp_ptr_set( &p->stage->map, x+2, y+0, n_bmp_black );
	n_bmp_ptr_set( &p->stage->map, x+3, y+0, n_bmp_black );

	n_bmp_ptr_set( &p->stage->map, x+0, y+1, n_bmp_black );
	n_bmp_ptr_set( &p->stage->map, x+1, y+1, n_bmp_black );
	n_bmp_ptr_set( &p->stage->map, x+2, y+1, n_bmp_black );
	n_bmp_ptr_set( &p->stage->map, x+3, y+1, n_bmp_black );


	u32 event = n_bmp_rgb_mac( 255, 255, N_CHIP_TREASURE_X  );
	u32 slip  = n_bmp_rgb_mac( 255, 255, N_CHIP_TREASURE_SR );

	n_bmp_ptr_set( &p->stage->map, x+4, y-0, event );
	n_bmp_ptr_set( &p->stage->map, x+4, y-1, event );
	n_bmp_ptr_set( &p->stage->map, x+4, y-2, event );

	n_bmp_ptr_set( &p->stage->map, x+5, y-0, event );
	n_bmp_ptr_set( &p->stage->map, x+5, y-1, event );
	n_bmp_ptr_set( &p->stage->map, x+5, y-2, slip  );


	return;
}




BOOL
n_object_treasure_heart_collision( n_nn2 *p, n_type_gfx tx, n_type_gfx ty )
{

	// [!] : how to debug : call this from n_nn2_draw()


	BOOL ret = FALSE;

/*
	u32 data = 0; n_bmp_ptr_get( &p->stage->map_dokan, tx, ty, &data );

	if ( N_CHIP_TREASURE_HT == n_chip_data_kind( data ) )
	{
		ret = TRUE;
	}
*/

	n_type_gfx heart_fx, heart_fy, heart_tx, heart_ty;
	n_type_gfx  nina_fx,  nina_fy,  nina_tx,  nina_ty;

	{
		n_type_gfx threshold = 16;

		n_type_gfx size = N_BMP_SX( &p->treasure_heart[ 0 ] );

		n_type_gfx hx = ( p->stage->treasure_x * p->mapchip_unit ) + ( size / 2 );
		n_type_gfx hy = ( p->stage->treasure_y * p->mapchip_unit ) + ( size / 2 );

		n_type_gfx target_x = hx - threshold - n_camerawork_x( p );
		n_type_gfx target_y = hy - threshold - n_camerawork_y( p );
		n_type_gfx sx = threshold * 2;
		n_type_gfx sy = threshold * 2;

		target_x -= p->mapchip_unit;

		//n_bmp_box( p->canvas, target_x, target_y, sx, sy, n_bmp_rgb_mac( 255,0,0 ) );

		heart_fx = target_x;
		heart_fy = target_y;
		heart_tx = target_x + sx;
		heart_ty = target_y + sy;
	}

	{
		n_type_gfx threshold = 8;

		n_type_gfx target_x = p->stage->nina_x + ( p->nina_sx / 2 ) - threshold - n_camerawork_x( p );
		n_type_gfx target_y = p->stage->nina_y + ( p->nina_sy / 2 ) - threshold - n_camerawork_y( p );
		n_type_gfx sx = threshold * 2;
		n_type_gfx sy = threshold * 2;

		target_y += p->mapchip_unit * 2;

		//n_bmp_box( p->canvas, target_x, target_y, sx, sy, n_bmp_rgb_mac( 0,0,255 ) );

		nina_fx = target_x;
		nina_fy = target_y;
		nina_tx = target_x + sx;
		nina_ty = target_y + sy;
	}


	if (
		( ( nina_fx > heart_fx )&&( nina_tx < heart_tx ) )
		&&
		( ( nina_fy > heart_fy )&&( nina_ty < heart_ty ) )
	)
	{
		ret = TRUE;
	}


	return ret;
}

BOOL
n_object_treasure_heart_detect( n_nn2 *p, n_type_gfx tx, n_type_gfx ty )
{
//return FALSE;

	if ( p->costume_transition ) { return FALSE; }

	if ( p->costume == N_NN2_COSTUME_PINK ) { return FALSE; }

	if ( p->stage->treasure_state != ( N_NN2_TREASURE_BMP_MAX + 1 - 1 ) ) { return FALSE; }


	BOOL ret = FALSE;


	if ( n_object_treasure_heart_collision( p, tx, ty ) )
	{
//NSLog( @"!" );
		ret = TRUE;


		n_bmp_ptr_set( &p->stage->map_dokan, tx-1, ty, 0 );
		n_bmp_ptr_set( &p->stage->map_dokan, tx-0, ty, 0 );
		n_bmp_ptr_set( &p->stage->map_dokan, tx+1, ty, 0 );


		n_bmp_free( &p->transition_bmp_old );
		n_bmp_carboncopy( p->canvas, &p->transition_bmp_old );


		n_bmp_new( &p->transition_bmp_mid, p->sx, p->sy ); // [!] : black screen


		n_bmp_new_fast( &p->transition_bmp_new, p->sx, p->sy );

		extern void n_nn2_loop( n_nn2 *p );
		extern void n_nn2_draw( n_nn2 *p );

		p->canvas = &p->transition_bmp_new;


		p->costume = N_NN2_COSTUME_PINK;

		p->powerup = &p->powerup_pink;
		//n_chara_collision_force_landing( p );


		n_nn2_loop( p );
		n_nn2_draw( p );

		p->canvas = &p->canvas_main;


		p->transition_percent = 0;
		p->costume_transition = 1;


		n_nn2_sound_effect_play( N_NN2_SOUND_HEART );
	}


	return ret;
}

BOOL
n_object_treasure_detect( n_nn2 *p, n_type_gfx x, n_type_gfx y )
{
#ifndef N_NN2_TREASURE_ONOFF
	return FALSE;
#endif

	u32 data; n_bmp_ptr_get( &p->stage->map, x, y, &data );

	if (
		( N_CHIP_TREASURE_X  == n_chip_data_kind( data ) )
		||
		( N_CHIP_TREASURE_SL == n_chip_data_kind( data ) )
		||
		( N_CHIP_TREASURE_SR == n_chip_data_kind( data ) )
	)
	{
		return TRUE;
	}


	return FALSE;
}

int
n_object_treasure_collision_wall_lr_loop( n_nn2 *p, n_type_gfx lx, n_type_gfx rx, n_type_gfx yy )
{
#ifndef N_NN2_TREASURE_ONOFF
	return 0;
#endif

	if ( p->jump_state ) { return 0; }


	int ret = 0;


	n_type_gfx treasure_fx, treasure_tx;
	if (0)//( p->stage->treasure_direction == N_NN2_DIRECTION_LEFT )
	{
		treasure_fx = lx;
		treasure_tx = rx;
	} else {
		treasure_fx = rx;
		treasure_tx = lx;
	}

	if ( n_object_treasure_detect( p, treasure_fx, yy ) )
	{
		ret |= N_CHARA_COLLISION_LR_WALL;
	}

	if ( n_object_treasure_detect( p, treasure_tx, yy ) )
	{
		ret |= N_CHARA_COLLISION_LR_WALL;
	}


//p->debug_chip_x = treasure_fx + 1;
//p->debug_chip_y = yy;

	if ( n_object_treasure_detect( p, treasure_fx + 1, yy ) )
	{
		if ( p->direction == N_NN2_DIRECTION_RIGHT )
		{
			p->stage->treasure_ready = TRUE;
		} else {
			p->stage->treasure_ready = FALSE;
		}
	} else {
		p->stage->treasure_ready = FALSE;
	}


	return ret;
}




void
n_object_treasure_draw_heart( n_nn2 *p )
{
#ifndef N_NN2_TREASURE_ONOFF
	return;
#endif

	if ( p->costume != N_NN2_COSTUME_PINK )
	{
		static int i = 0;
		static u32 t = 0;

		if ( n_bmp_ui_timer( &t, 100 ) )
		{
			i++;
			if ( i >= 4 ) { i = 0; }
		}

		n_type_gfx x = p->stage->treasure_x;
		n_type_gfx y = p->stage->treasure_y;
//NSLog( @"%d %d", x, y );

		n_chip_draw_dokan_single( p, &p->treasure_heart[ i ], x, y );
	}


	return;
}

void
n_object_treasure_draw_key( n_nn2 *p )
{
#ifndef N_NN2_TREASURE_ONOFF
	return;
#endif

	// [!] : this is a special module

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


	if ( p->stage->treasure_state == 0 )
	{
		n_type_gfx ox = -1;
		n_type_gfx oy =  4;

		n_type_gfx x = p->stage->treasure_x;
		n_type_gfx y = p->stage->treasure_y;

		n_chip_draw_dokan_single( p, &p->treasure_key, x + ox, y + oy );
	}


	return;
}

void
n_object_treasure_draw( n_nn2 *p )
{
#ifndef N_NN2_TREASURE_ONOFF
	return;
#endif

	n_type_gfx x = p->stage->treasure_x;
	n_type_gfx y = p->stage->treasure_y;
//NSLog( @"%d %d", x, y );

	n_chip_draw_dokan_single( p, &p->treasure_bmp[ p->stage->treasure_state ], x, y );


	return;
}




void
n_object_treasure_init( n_nn2 *p )
{
//NSLog( @"n_object_treasure_init()" );

#ifndef N_NN2_TREASURE_ONOFF
	return;
#endif

	n_nn2_treasure_maker( p );

	n_nn2_rc_load( @"rc/object/treasure/heart", &p->treasure_heart[ 0 ], p->scaler );
	n_nn2_rc_load( @"rc/object/treasure/heart", &p->treasure_heart[ 1 ], p->scaler );
	n_nn2_rc_load( @"rc/object/treasure/heart", &p->treasure_heart[ 2 ], p->scaler );
	n_nn2_rc_load( @"rc/object/treasure/heart", &p->treasure_heart[ 3 ], p->scaler );

	n_bmp_resampler( &p->treasure_heart[ 1 ], 1.05, 1.05 );
	n_bmp_resampler( &p->treasure_heart[ 2 ], 1.10, 1.10 );
	n_bmp_resampler( &p->treasure_heart[ 3 ], 1.05, 1.05 );

	n_type_gfx sx = N_BMP_SX( &p->treasure_heart[ 0 ] );
	n_type_gfx sy = N_BMP_SY( &p->treasure_heart[ 0 ] );

	n_bmp_resizer( &p->treasure_heart[ 1 ], sx, sy, 0, N_BMP_RESIZER_CENTER );
	n_bmp_resizer( &p->treasure_heart[ 2 ], sx, sy, 0, N_BMP_RESIZER_CENTER );
	n_bmp_resizer( &p->treasure_heart[ 3 ], sx, sy, 0, N_BMP_RESIZER_CENTER );

	n_nn2_rc_load( @"rc/object/treasure/key", &p->treasure_key, p->scaler );

//n_nn2_debug_bmp_save( p, &p->treasure_bmp[ 0 ] );
//n_nn2_debug_bmp_save( p, &p->treasure_key );


	return;
}

BOOL
n_object_treasure_is_running( n_nn2 *p )
{
	return (
		( p->stage->treasure_state >= 1 )
		&&
		( p->stage->treasure_state <= ( N_NN2_TREASURE_BMP_MAX + 1 - 3 ) )
	);
}

void
n_object_treasure_loop( n_nn2 *p )
{
//NSLog( @"n_object_treasure_loop()" );

#ifndef N_NN2_TREASURE_ONOFF
	return;
#endif

	static u32 timer = 0;
	static u32 msec  = 0;

	if ( p->stage->treasure_state == 0 )
	{
		if ( p->stage->treasure_ready )
		{
			if ( p->dash_onoff == FALSE )
			{
				p->stage->treasure_phase = 1;
			} else
			if ( p->stage->treasure_phase == 1 )
			{
				p->stage->treasure_phase = 2;
			}

			if ( p->stage->treasure_phase == 2 )
			{
				p->lr_lock = TRUE;

				p->stage->treasure_state++;
				timer = n_posix_tickcount();
				msec  = 200;

				n_nn2_sound_effect_play( N_NN2_SOUND_OPEN );
			}
		}
	} else
	if ( n_object_treasure_is_running( p ) )
	{
		if ( n_bmp_ui_timer( &timer, msec ) )
		{
			p->stage->treasure_state++;
			timer = n_posix_tickcount();
			msec  = msec / 3;
		}
	} else
	if ( p->stage->treasure_state == ( N_NN2_TREASURE_BMP_MAX + 1 - 2 ) )
	{
		if ( n_bmp_ui_timer( &timer, msec ) )
		{
			p->stage->treasure_state++;
			//timer = n_posix_tickcount();

			n_object_treasure_open( p );
			//p->stage->treasure_is_open = FALSE;

			p->lr_lock = FALSE;
		}
	}


	return;
}

