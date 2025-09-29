// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




extern BOOL n_object_rocks_action_mutex( n_nn2 *p );

extern n_sprite* n_interobject_collision_gravity_lift( n_nn2 *p );
extern void n_interobject_collision_handheld_release( n_nn2 *p );

extern BOOL n_object_bell_detect( n_nn2 *p, n_type_gfx tx, n_type_gfx ty );
extern BOOL n_object_bell_condition_detect( n_nn2 *p, n_type_gfx x, n_type_gfx y );

extern int  n_object_treasure_collision_wall_lr_loop( n_nn2 *p, n_type_gfx lx, n_type_gfx rx, n_type_gfx yy );
extern BOOL n_object_treasure_heart_detect( n_nn2 *p, n_type_gfx tx, n_type_gfx ty );

extern void n_object_handheld_off( n_nn2 *p );

extern void n_splash_init( n_nn2 *p, n_splash *s, n_type_gfx x, n_type_gfx y, u32 timeout );




int
n_chara_collision_hole_detect( n_nn2 *p, n_type_gfx ty_u, n_type_gfx ty_d )
{

	int ret = 0;

	n_type_gfx max_y = N_BMP_SY( &p->stage->map );

	p->lr_lock = FALSE;

	if ( ty_u >= max_y ) { p->lr_lock = TRUE; ret |= N_CHARA_COLLISION_LR_HEAD; }
	//if ( ty_d >= max_y ) { p->lr_lock = TRUE; ret |= N_CHARA_COLLISION_LR_FOOT; }

	if ( p->lr_lock ) { p->suck_stop = TRUE; }


	return ret;
}

void
n_chara_collision_hole( n_nn2 *p )
{

	if ( p->transition_lock )
	{

		//

	} else
	if ( p->stage->nina_y > p->stage->map_sy )
	{
		p->transition_lock = TRUE;

		if ( p->stage == &n_nn2_stage_0 )
		{
			p->bell_reset_onoff = TRUE;
		}

		if ( n_object_birds_is_caught( p ) )
		{
			n_object_handheld_off( p );
		}

		n_type_gfx x = p->stage->nina_x + ( p->nina_sx / 2 ); x /= p->mapchip_unit;
		n_type_gfx y = N_BMP_SY( &p->stage->map_dokan ) - 1;

		u32 data = 0; n_bmp_ptr_get( &p->stage->map_dokan, x, y, &data );

		int stage_number = n_nn2_map_dokan_data_kind( data );
//NSLog( @"Warp to %d", stage_number );

		extern void n_nn2_stage_transition_go( n_nn2 *p, int stage_number );
		n_nn2_stage_transition_go( p, stage_number );
	}


	return;
}




#define N_CHARA_MARGIN_DETECT_NONE ( 0 )
#define N_CHARA_MARGIN_DETECT_HEAD ( 1 )
#define N_CHARA_MARGIN_DETECT_FOOT ( 2 )
#define N_CHARA_MARGIN_DETECT_FWRD ( 3 )
#define N_CHARA_MARGIN_DETECT_REAR ( 4 )

n_type_gfx
n_chara_margin_detect( n_nn2 *p, n_bmp *bmp, int udlr )
{

	if ( udlr == N_CHARA_MARGIN_DETECT_HEAD )
	{

		n_type_gfx x = 0;
		n_type_gfx y = 0;
		n_posix_loop
		{

			u32 color; n_bmp_ptr_get( bmp, x,y, &color );
			if ( 255 == n_bmp_a( color ) )
			{
				return y;
			}

			x++;
			if ( x >= N_BMP_SX( bmp ) )
			{
				x = 0;

				y++;
				if ( y >= N_BMP_SY( bmp ) ) { break; }
			}
		}

	} else
	if ( udlr == N_CHARA_MARGIN_DETECT_FOOT )
	{

		n_type_gfx x = 0;
		n_type_gfx y = N_BMP_SY( bmp ) - 1;
		n_posix_loop
		{

			u32 color; n_bmp_ptr_get( bmp, x,y, &color );
			if ( 255 == n_bmp_a( color ) )
			{
//NSLog( @"found %d %d", N_BMP_SY( bmp ), N_BMP_SY( bmp ) - y );
				return N_BMP_SY( bmp ) - y;
			}

			x++;
			if ( x >= N_BMP_SX( bmp ) )
			{
				x = 0;

				y--;
				if ( y < 0 ) { break; }
			}
		}

	} else
	if ( udlr == N_CHARA_MARGIN_DETECT_FWRD )
	{

		n_type_gfx x = 0;
		n_type_gfx y = 0;
		n_posix_loop
		{

			u32 color; n_bmp_ptr_get( bmp, x,y, &color );
			if ( 255 == n_bmp_a( color ) )
			{
				return x;
			}

			y++;
			if ( y >= N_BMP_SY( bmp ) )
			{
				y = 0;

				x++;
				if ( x >= N_BMP_SX( bmp ) ) { break; }
			}
		}

	} else
	if ( udlr == N_CHARA_MARGIN_DETECT_REAR )
	{

		n_type_gfx x = N_BMP_SX( bmp ) - 1;
		n_type_gfx y = 0;
		n_posix_loop
		{

			u32 color; n_bmp_ptr_get( bmp, x,y, &color );
			if ( 255 == n_bmp_a( color ) )
			{
				return N_BMP_SX( bmp ) - x;
			}

			y++;
			if ( y >= N_BMP_SY( bmp ) )
			{
				y = 0;

				x--;
				if ( x < 0 ) { break; }
			}
		}

	} //else


	return 0;
}

n_type_gfx
n_chara_margin_head( n_nn2 *p )
{
	if ( p->duck_onoff )
	{
		return p->nina_margin_duck;
	} else {
		return p->nina_margin_head;
	}
}




void
n_chara_collision_jam_resolver( n_nn2 *p, int direction )
{
//NSLog( @"n_chara_collision_jam_resolver()" );
//return;


	if ( p->hipdrop_state ) { return; }

	if ( p->dokan_onoff ) { return; }

	if ( p->brick_move_onoff ) { return; }

/*
	if ( n_mac_keystate_get( N_MAC_KEYCODE_H ) )
	{
		n_posix_sleep( 200 );
		p->stage->nina_x -= 16;
	} else
	if ( n_mac_keystate_get( N_MAC_KEYCODE_J ) )
	{
		n_posix_sleep( 200 );
		p->stage->nina_x += 16;
	}
//return;
*/

	n_type_gfx fwrd = p->nina_margin_fwrd;

	if ( direction == N_NN2_DIRECTION_LEFT )
	{
//NSLog( @"Left" );

		if ( p->jump_state == 0 )
		{
			n_type_gfx tx = p->stage->nina_x + ( p->nina_sx / 2 );
			n_type_gfx ty = p->stage->nina_y + n_chara_margin_head( p );
			n_type_gfx dx = p->stage->nina_x + fwrd;
			n_type_gfx dy = p->stage->nina_y + p->nina_sy - p->nina_margin_foot - 1;

			n_type_gfx start = ty;
			n_type_gfx stop  = -1;

			n_posix_loop
			{
				if ( n_chip_detect( p, tx / p->mapchip_unit, ty / p->mapchip_unit ) )
				{
//NSLog( @"Right : Hit" );
					stop = tx;
					break;
				}

				ty++;
				if ( ty >= dy )
				{
					ty = start;

					tx--;
					if ( tx <= dx ) { break; }
				}
			}

			if ( stop != -1 ) { p->stage->nina_x += N_NN2_STEP_FORCE; }
		}


		n_type_gfx tx = p->stage->nina_x + ( p->nina_sx / 2 );
		n_type_gfx ty = p->stage->nina_y + n_chara_margin_head( p );
		n_type_gfx dx = p->stage->nina_x + p->nina_sx - p->nina_margin_swim;
		n_type_gfx dy = p->stage->nina_y + p->nina_sy - p->nina_margin_foot - 1;

		n_type_gfx start = ty;
		n_type_gfx stop  = -1;

		n_posix_loop
		{
			if ( n_chip_detect( p, tx / p->mapchip_unit, ty / p->mapchip_unit ) )
			{
//NSLog( @"Left : Hit" );
				stop = tx;
				break;
			}

			ty++;
			if ( ty >= dy )
			{
				ty = start;

				tx++;
				if ( tx >= dx ) { break; }
			}
		}

		if ( stop != -1 ) { p->stage->nina_x -= N_NN2_STEP_FORCE; }

	} else
	if ( direction == N_NN2_DIRECTION_RIGHT )
	{
//NSLog( @"Right" );

		if ( p->jump_state == 0 )
		{
			n_type_gfx tx = p->stage->nina_x + ( p->nina_sx / 2 );
			n_type_gfx ty = p->stage->nina_y + n_chara_margin_head( p );
			n_type_gfx dx = p->stage->nina_x + p->nina_sx - fwrd;
			n_type_gfx dy = p->stage->nina_y + p->nina_sy - p->nina_margin_foot - 1;

			n_type_gfx start = ty;
			n_type_gfx stop  = -1;

			n_posix_loop
			{
				if ( n_chip_detect( p, tx / p->mapchip_unit, ty / p->mapchip_unit ) )
				{
//NSLog( @"Left : Hit" );
					stop = tx;
					break;
				}

				ty++;
				if ( ty >= dy )
				{
					ty = start;

					tx++;
					if ( tx >= dx ) { break; }
				}
			}

			if ( stop != -1 ) { p->stage->nina_x -= N_NN2_STEP_FORCE; }

			return;
		}


		n_type_gfx tx = p->stage->nina_x + ( p->nina_sx / 2 );
		n_type_gfx ty = p->stage->nina_y + n_chara_margin_head( p );
		n_type_gfx dx = p->stage->nina_x + p->nina_margin_swim;
		n_type_gfx dy = p->stage->nina_y + p->nina_sy - p->nina_margin_foot - 1;

		n_type_gfx start = ty;
		n_type_gfx stop  = -1;

		n_posix_loop
		{
			if ( n_chip_detect( p, tx / p->mapchip_unit, ty / p->mapchip_unit ) )
			{
//NSLog( @"Right : Hit" );
				stop = tx;
				break;
			}

			ty++;
			if ( ty >= dy )
			{
				ty = start;

				tx--;
				if ( tx <= dx ) { break; }
			}
		}

		if ( stop != -1 ) { p->stage->nina_x += N_NN2_STEP_FORCE; }

	}


	return;
}




int
n_chara_collision_wall_lr_loop( n_nn2 *p )
{
//return 0;

	if ( p->stage->camera_mode == N_CAMERAWORK_STOP )
	{
		n_type_gfx margin = p->nina_margin_fwrd;

		if ( p->stage->nina_x < -margin ) { return N_CHARA_COLLISION_LR_STOP | N_CHARA_COLLISION_STOP__L; }
		if ( ( p->stage->nina_x + p->nina_sx - margin ) > ( p->stage->map_max_sx + p->sx ) ) { return N_CHARA_COLLISION_LR_STOP | N_CHARA_COLLISION_STOP__R; }
	} else
	if ( p->stage->camera_mode == N_CAMERAWORK_WARP )
	{
		s32 min_x = -p->nina_sx + p->nina_margin_fwrd;
		s32 max_x = p->stage->map_sx - p->nina_margin_fwrd + ( p->share->kuina_sprite.sx / 2 );

		if ( p->direction == N_NN2_DIRECTION_LEFT )
		{
			if ( p->stage->nina_x < min_x ) { p->stage->nina_x = max_x; }
		} else
		if ( p->direction == N_NN2_DIRECTION_RIGHT )
		{
			if ( p->stage->nina_x > max_x ) { p->stage->nina_x = min_x; }
		}
	} else
	if ( p->stage->camera_mode == N_CAMERAWORK_LOOP )
	{
		if ( p->direction == N_NN2_DIRECTION_LEFT )
		{
			s32 min_x = -p->nina_margin_fwrd;
			if ( p->stage->nina_x < min_x ) { p->stage->nina_x += p->stage->map_sx; }
		} else
		if ( p->direction == N_NN2_DIRECTION_RIGHT )
		{
			s32 max_x = p->stage->map_sx - p->nina_margin_fwrd;
			if ( p->stage->nina_x > max_x ) { p->stage->nina_x -= p->stage->map_sx; }
		}
	}


	// [!] : stage 0 : rear is used on-lift process 

	n_type_gfx tx_l_base = p->stage->nina_x;
	n_type_gfx tx_r_base = p->stage->nina_x + p->nina_sx;

	tx_l_base += p->nina_margin_fwrd;
	tx_r_base -= p->nina_margin_fwrd;

/*
	static BOOL onoff = FALSE;
	static int  count = 0;
	if ( onoff == FALSE )
	{
		count++;
		if ( count >= 200 ) { onoff =  TRUE; }
	} else {
		count--;
		if ( count <=   0 ) { onoff = FALSE; }
	}

	tx_l_base += count;
	tx_r_base -= count;
*/

	n_type_gfx ty_u_base = p->stage->nina_y              + n_chara_margin_head( p );
	n_type_gfx ty_d_base = p->stage->nina_y + p->nina_sy - p->nina_margin_foot - 1;


	n_type_gfx tx_l = tx_l_base;
	n_type_gfx tx_r = tx_r_base;

	tx_l = tx_l / p->mapchip_unit;
	tx_r = tx_r / p->mapchip_unit;


	n_type_gfx ty_u = ty_u_base;
	n_type_gfx ty_d = ty_d_base;
	n_type_gfx ty_m = p->stage->nina_y + ( p->nina_sy / 2 ) - p->nina_margin_foot;

	ty_u = ty_u / p->mapchip_unit;
	ty_d = ty_d / p->mapchip_unit;
	ty_m = ty_m / p->mapchip_unit;


	int ret = N_CHARA_COLLISION_LR_NONE;
//return ret;


	ret |= n_chara_collision_hole_detect( p, ty_u, ty_d );
//p->debug_box_x = tx_r;
//p->debug_box_y = ty_u;


	if ( n_chip_detect( p, tx_l, ty_u ) )
	{
//NSLog( @"L : N_CHARA_COLLISION_LR_HEAD" );
		ret |= N_CHARA_COLLISION_LR_HEAD;
	}

	if ( n_chip_detect_slip( p, tx_l, ty_m ) )
	{
//NSLog( @"L : N_CHARA_COLLISION_LR_SLIP" );
		ret |= N_CHARA_COLLISION_LR_SLIP;
	}

	if ( n_chip_detect( p, tx_l, ty_d ) )
	{
//NSLog( @"L : N_CHARA_COLLISION_LR_FOOT" );
		ret |= N_CHARA_COLLISION_LR_FOOT;
	}


	if ( n_chip_detect( p, tx_r, ty_u ) )
	{
//NSLog( @"R : N_CHARA_COLLISION_LR_HEAD" );
		ret |= N_CHARA_COLLISION_LR_HEAD;
	}

	if ( n_chip_detect_slip( p, tx_r, ty_m ) )
	{
//NSLog( @"R : N_CHARA_COLLISION_LR_SLIP" );
		ret |= N_CHARA_COLLISION_LR_SLIP;
	}

	if ( n_chip_detect( p, tx_r, ty_d ) )
	{
//NSLog( @"R : N_CHARA_COLLISION_LR_FOOT" );
		ret |= N_CHARA_COLLISION_LR_FOOT;
	}


	n_type_gfx ty_f_u = ty_u_base;
	n_type_gfx ty_f_d = ty_d_base;

	n_type_gfx lx = tx_l_base;
	n_type_gfx rx = tx_r_base;
	n_type_gfx fy = ty_f_u;
	n_type_gfx ty = ty_f_d;

	lx /= p->mapchip_unit;
	rx /= p->mapchip_unit;
	fy /= p->mapchip_unit;
	ty /= p->mapchip_unit;

	n_posix_loop
	{//break;

		ret |= n_object_treasure_collision_wall_lr_loop( p, lx, rx, fy );

		if ( n_chip_detect( p, lx, fy ) )
		{
			ret |= N_CHARA_COLLISION_LR_WALL;
			break;
		}

		if ( n_chip_detect( p, rx, fy ) )
		{
			ret |= N_CHARA_COLLISION_LR_WALL;
			break;
		}

		fy++;
		if ( fy >= ty ) { break; }
	}


	return ret;
}

void
n_chara_collision_lr( n_nn2 *p, int direction )
{
//return;

	if ( N_NN2_DEBUG_LOOP ) { n_nn2_debug_hangup_detector( @"n_chara_collision_lr()" ); }


	if ( p->dokan_onoff ) { return; }


	n_type_gfx add = 0;

	n_posix_loop
	{
		n_type_gfx nina = p->stage->nina_x;

		if ( direction == N_NN2_DIRECTION_LEFT )
		{
			p->stage->nina_x--;
		} else
		if ( direction == N_NN2_DIRECTION_RIGHT )
		{
			p->stage->nina_x++;
		}

		p->wall_collision = n_chara_collision_wall_lr_loop( p );
		if ( p->wall_collision )
		{
			p->stage->nina_x = nina;
			break;
		} else
		if ( n_object_rocks_action_mutex( p ) )
		{
			p->stage->nina_x = nina;
			break;
		} else
		//
		{//break;
			add++;
			if ( add >= p->nina_walk_step )
			{
				break;
			}
		}
	}


	return;
}




// internal
void
n_chara_collision_swim( n_nn2 *p, n_type_gfx my )
{

	if ( p->wave_onoff == FALSE ) { return; }

//NSLog( @"%d", my );

	if ( my < 10 )
	{
		if ( p->swim_onoff )
		{
//NSLog( @"1" );
			n_nn2_sound_effect_play( N_NN2_SOUND_WATER );

			p->swim_onoff = FALSE;
			p->jump_onoff = FALSE;
			p->jump_float = 0;

			n_splash_init_water( p );
		}
	} else
	if ( my > 13 )
	{
		if ( p->swim_onoff == FALSE )
		{
//NSLog( @"2" );
			n_nn2_sound_effect_play( N_NN2_SOUND_WATER );

			p->swim_onoff = TRUE;
			p->jump_onoff = FALSE;
			p->jump_float = 0;

			n_splash_init_water( p );
		}

		if ( my > 18 )
		{
			p->stage->camera_wave_under = TRUE;
		}
	}


	return;
}

// internal
void
n_chara_collision_bell_autogetter_loop( n_nn2 *p )
{

	n_type_gfx sx = p->nina_sx - ( p->nina_margin_fwrd + p->nina_margin_rear );
	n_type_gfx sy = p->nina_sy - ( p->nina_margin_head + p->nina_margin_foot );

	n_type_gfx x = 0;
	n_type_gfx y = 0;
	n_posix_loop
	{

		n_type_gfx tx = ( p->stage->nina_x + p->nina_margin_fwrd + x ) / p->mapchip_unit;
		n_type_gfx ty = ( p->stage->nina_y + p->nina_margin_head + y ) / p->mapchip_unit;

		n_object_bell_detect( p, tx, ty );

		n_object_treasure_heart_detect( p, tx, ty );

		x++;
		if ( x >= sx )
		{
			x = 0;
			y++;
			if ( y >= sy ) { break; }
		}
	}


	return;
}

// internal
void
n_chara_collision_dokan( n_nn2 *p, n_type_gfx tx, n_type_gfx ty, u32 data )
{
	if ( n_object_bell_condition_detect( p, tx, ty ) )
	{
//NSLog( @"enter" );

		p->dokan_ready = TRUE;

		if ( ( p->input & N_NN2_INPUT_D )&&( p->dokan_onoff == FALSE ) )
		{
//NSLog( @"N_CHIP_DOKAN_Q : ON" );
//p->debug_pause = TRUE;

			p->dokan_onoff = TRUE;

			p->stage->camera_lock = TRUE;


			extern void n_object_handheld_off( n_nn2 *p );
			n_object_handheld_off( p );

			p->sprite_cur = &n_chara_sprite_idle;
			n_sprite_rewind( p->sprite_cur );

			p->draw_center = N_NN2_DRAW_IDLE;


			p->dokan_disappear_offset = p->nina_margin_foot - 2;


			p->jump_state = 0;

			p->dokan_blend_stop = p->dokan_blend;

			n_nn2_sound_effect_play( N_NN2_SOUND_DOKAN );
		}

	} else {

		p->bell_condition_onoff = TRUE;

		if ( N_CHIP_DOKAN_Q_L == n_chip_data_kind( data ) )
		{
			p->bell_condition_dokan_x = ( tx * p->mapchip_unit ) + p->mapchip_unit;
		} else
		if ( N_CHIP_DOKAN_Q_R == n_chip_data_kind( data ) )
		{
			p->bell_condition_dokan_x = ( tx * p->mapchip_unit );
		}

		p->bell_condition_dokan_y = p->stage->nina_y;

	}


	return;
}

// internal
BOOL
n_chara_collision_gravity_loop( n_nn2 *p, BOOL is_inner )
{

	n_sprite *spr = n_interobject_collision_gravity_lift( p );
	if ( spr != NULL )
	{
		// [!] : pass-thru when hipdrop

		if ( p->hipdrop_state == 1 )
		{
//static int i = 0; NSLog( @"%d", i ); i++;

			n_splash_init( p, &p->splash_cloud, p->stage->nina_x, spr->y, 333 );

			p->stage->nina_y += N_NN2_CHIP_UNIT;
			p->hipdrop_state  = 0;

			return FALSE;
		}

		return TRUE;
	}


	n_type_gfx tx = p->stage->nina_x + ( p->nina_sx / 2 );
	n_type_gfx ty = p->stage->nina_y + p->nina_sy - p->nina_margin_foot; if ( is_inner ) { ty--; }
//NSLog( @"%d %d", tx, ty );

	if ( p->dokan_onoff ) { ty -= p->dokan_disappear_offset; }

	tx = tx / p->mapchip_unit;
	ty = ty / p->mapchip_unit;


	n_chara_collision_swim( p, ty );


	n_type_gfx tx_fwrd = p->stage->nina_x + ( p->nina_sx / 2 );
	n_type_gfx tx_rear = p->stage->nina_x + ( p->nina_sx / 2 );

	if ( p->direction == N_NN2_DIRECTION_LEFT )
	{
		tx_fwrd = p->stage->nina_x +              p->nina_margin_fwrd;
		tx_rear = p->stage->nina_x + p->nina_sx - p->nina_margin_rear;
	} else
	if ( p->direction == N_NN2_DIRECTION_RIGHT )
	{
		tx_fwrd = p->stage->nina_x + p->nina_sx - p->nina_margin_fwrd;
		tx_rear = p->stage->nina_x +              p->nina_margin_rear;
	}

	tx_fwrd = tx_fwrd / p->mapchip_unit;
	tx_rear = tx_rear / p->mapchip_unit;


	u32 data_fwrd = 0; n_bmp_ptr_get( &p->stage->map, tx_fwrd, ty, &data_fwrd );
	u32 data_rear = 0; n_bmp_ptr_get( &p->stage->map, tx_rear, ty, &data_rear );
	u32 data_midl = 0; n_bmp_ptr_get( &p->stage->map, tx     , ty, &data_midl );

	if ( tx_fwrd < 0 ) { n_bmp_ptr_get( &p->stage->map, 0, ty, &data_fwrd ); }
	if ( tx_rear < 0 ) { n_bmp_ptr_get( &p->stage->map, 0, ty, &data_rear ); }
	if ( tx      < 0 ) { n_bmp_ptr_get( &p->stage->map, 0, ty, &data_midl ); }

	if ( tx_fwrd >= N_BMP_SX( &p->stage->map ) ) { n_bmp_ptr_get( &p->stage->map, N_BMP_SX( &p->stage->map ) - 1, ty, &data_fwrd ); }
	if ( tx_rear >= N_BMP_SX( &p->stage->map ) ) { n_bmp_ptr_get( &p->stage->map, N_BMP_SX( &p->stage->map ) - 1, ty, &data_rear ); }
	if ( tx      >= N_BMP_SX( &p->stage->map ) ) { n_bmp_ptr_get( &p->stage->map, N_BMP_SX( &p->stage->map ) - 1, ty, &data_midl ); }

//NSLog( @"%d %d", n_chip_data_kind( data_fwrd ), n_chip_data_kind( data_rear ) );


	p->alignment_adjuster = 0;

	p->bell_condition_onoff = FALSE;


	BOOL ret = FALSE;

	p->dokan_ready = FALSE;

	if (
		( N_CHIP_DOKAN_Q_L == n_chip_data_kind( data_midl ) )
		||
		( N_CHIP_DOKAN_Q_R == n_chip_data_kind( data_midl ) )
	)
	{
//NSLog( @"N_CHIP_DOKAN_Q : %d", p->input );

		// [!] : usability : before n_chip_detect_block()

		n_chara_collision_dokan( p, tx, ty, data_midl );

		ret = TRUE;

	} else
	if (
		( n_chip_detect_block( p, data_fwrd ) )
		||
		( n_chip_detect_block( p, data_rear ) )
		||
		( n_chip_detect_block( p, data_midl ) )
	)
	{
//NSLog( @"landing" );

		ret = TRUE;

		if ( p->hipdrop_state == 1 )
		{
			if ( N_CHIP_BRICK == n_chip_data_kind( data_fwrd ) )
			{
				n_brick_move_action_on( p, tx_fwrd, ty, NO );
			}

			if ( N_CHIP_BRICK == n_chip_data_kind( data_rear ) )
			{
				n_brick_move_action_on( p, tx_rear, ty, NO );
			}

			if ( N_CHIP_BRICK == n_chip_data_kind( data_midl ) )
			{
				n_brick_move_action_on( p, tx     , ty, NO );
			}
		}

	} else
	if ( n_chip_detect_pass_thru( p, data_midl ) )
	{
//NSLog( @"pass thru : %d %d", n_chip_detect_pass_thru( p, data_fwrd ), n_chip_detect_pass_thru( p, data_rear ) );
//NSLog( @"pass thru : %d %d %d",  tx_fwrd, tx_rear, ty );

		ret = TRUE;

		p->alignment_adjuster = ( ty * p->mapchip_unit ) - p->nina_sy + p->nina_margin_foot;

		n_chip_pass_thru_go( p, tx, ty );

	} else
	if ( n_chip_detect_pass_thru( p, data_fwrd ) )
	{
		n_chip_pass_thru_go( p, tx_fwrd, ty );
	} else
	if ( n_chip_detect_pass_thru( p, data_rear ) )
	{
		n_chip_pass_thru_go( p, tx_rear, ty );
	} else
	if (
		( n_chip_detect_hill( p, data_fwrd ) )
		||
		( n_chip_detect_hill( p, data_rear ) )
	)
	{
		ret = TRUE;

		p->alignment_adjuster = ( ty * p->mapchip_unit ) - p->nina_sy + p->nina_margin_foot;
	} else
	if (
		( n_chip_detect_special( p, data_fwrd ) )
		||
		( n_chip_detect_special( p, data_rear ) )
	)
	{
		if ( n_object_birds_is_caught( p ) )
		{
			ret = TRUE;

			p->bounce_landed = TRUE;

			p->bounce_sprite_m = n_chip_get_auto( p, tx, ty );
			if ( p->bounce_sprite_m->bounce_offset < N_NN2_BRICK_MOVE_MAX ) { p->bounce_sprite_m->bounce_offset++; }

			if ( n_chip_is_accessible( p->stage, tx-1, ty ) )
			{
				p->bounce_sprite_l = n_chip_get_auto( p, tx-1, ty );
				p->bounce_sprite_l->bounce_offset = p->bounce_sprite_m->bounce_offset / 2;
			} else {
				p->bounce_sprite_l = NULL;
			}

			if ( n_chip_is_accessible( p->stage, tx+1, ty ) )
			{
				p->bounce_sprite_r = n_chip_get_auto( p, tx+1, ty );
				p->bounce_sprite_r->bounce_offset = p->bounce_sprite_m->bounce_offset / 2;
			} else {
				p->bounce_sprite_r = NULL;
			}

		}
	} else
	//
	{

		if ( p->dokan_onoff )
		{
			//
		} else
		if ( p->jump_state != N_NN2_JUMP_STATE_RISE )
		{
//NSLog( @"free fall : %d", p->jump_state );

			p->jump_state = N_NN2_JUMP_STATE_FALL;
			p->sprite_cur = &n_chara_sprite_jump;
		}

		ret = FALSE;

	}


	return ret;
}

BOOL
n_chara_collision_gravity( n_nn2 *p )
{

	if ( N_NN2_DEBUG_LOOP ) { n_nn2_debug_hangup_detector( @"n_chara_collision_gravity()" ); }


	BOOL ret = FALSE;

	n_type_gfx add = 0;


	int step = p->powerup->jump_fall;

	if ( p->object_handheld_sprite == &p->share->chick_sprite )
	{
		if ( p->fly_down )
		{
			step /= 1;
		} else {
			step /= 4;
		}
	} else
	if ( p->swim_onoff )
	{
		if ( p->fly_down )
		{
			step /= 2;
		} else
		if ( p->dokan_onoff )
		{
			step /= 3;
		} else {
			step /= 4;
		}
	} else
	if ( p->dokan_onoff )
	{
		if ( p->dokan_onoff )
		{
			step /= 3;
		}
	} else
	if ( p->jump_flow )
	{
		step /= 5;
	} else
	if ( p->hipdrop_state == 1 )
	{
		step *= 4;
	}

	n_posix_loop
	{
		n_type_gfx nina = p->stage->nina_y;

		p->stage->nina_y++;
		if ( n_chara_collision_gravity_loop( p, NO ) )
		{
			p->stage->nina_y = nina;

			ret = TRUE;

			p->stage->camera_first = TRUE;

			if ( p->alignment_adjuster != 0 )
			{
				p->stage->nina_y = p->alignment_adjuster;
			}

			break;
		} else
		//
		{
			if ( p->dokan_onoff ) { p->dokan_disappear_offset++; }

			add++;
			if ( add >= step ) { break; }
		}

	}


	return ret;
}

void
n_chara_collision_force_landing( n_nn2 *p )
{

	while( !n_chara_collision_gravity( p ) ) {}


	return;
}




//static BOOL n_chara_collision_fall_condition_NSLog = FALSE;

// internal
n_type_gfx
n_chara_collision_fall_condition( n_nn2 *p, n_type_gfx tx, n_type_gfx ty, n_type_gfx maxim )
{

	n_type_gfx count = 0;

	n_type_gfx i = 0;
	n_posix_loop
	{

		u32 d = 0; n_bmp_ptr_get( &p->stage->map, tx, ty + i, &d );
		if ( d == n_bmp_black )
		{
			count++;
		}

		i++;
		if ( i >= maxim ) { break; }
	}

//if ( n_chara_collision_fall_condition_NSLog ) { NSLog( @"%d", count ); }
	return count;
}

// internal
BOOL
n_chara_collision_cliff_detect( n_nn2 *p, u32 data )
{

	if (
		( n_chip_detect_block( p, data ) )
		||
		//( n_chip_detect_pass_thru( p, data ) )
		//||
		( n_chip_detect_hill( p, data ) )
		||
		( N_CHIP_DOKAN_Q_L == n_chip_data_kind( data ) )
		||
		( N_CHIP_DOKAN_Q_R == n_chip_data_kind( data ) )
	)
	{
		return TRUE;
	}


	return FALSE;
}

BOOL
n_chara_collision_cliff_loop( n_nn2 *p )
{

	if ( p->jump_onoff ) { return FALSE; }

	if ( p->brake_phase ) { return FALSE; }

	if ( p->swim_onoff ) { return FALSE; }

	if ( p->dokan_onoff ) { return FALSE; }


	n_type_gfx ty = p->stage->nina_y + p->nina_sy - p->nina_margin_foot + 1;

	ty = ty / p->mapchip_unit;


	n_type_gfx tx_fwrd = p->stage->nina_x + ( p->nina_sx / 2 );
	n_type_gfx tx_midl = p->stage->nina_x + ( p->nina_sx / 2 );
	n_type_gfx tx_rear = p->stage->nina_x + ( p->nina_sx / 2 );

	if ( p->direction == N_NN2_DIRECTION_LEFT )
	{
		tx_fwrd = p->stage->nina_x +              p->nina_margin_fwrd;
		tx_rear = p->stage->nina_x + p->nina_sx - p->nina_margin_rear;
	} else
	if ( p->direction == N_NN2_DIRECTION_RIGHT )
	{
		tx_fwrd = p->stage->nina_x + p->nina_sx - p->nina_margin_fwrd;
		tx_rear = p->stage->nina_x +              p->nina_margin_rear;
	}


	u32 data_fwrd = 0; n_bmp_ptr_get( &p->stage->map, tx_fwrd / p->mapchip_unit, ty, &data_fwrd );
	u32 data_midl = 0; n_bmp_ptr_get( &p->stage->map, tx_midl / p->mapchip_unit, ty, &data_midl );
	u32 data_rear = 0; n_bmp_ptr_get( &p->stage->map, tx_rear / p->mapchip_unit, ty, &data_rear );


	BOOL ret = FALSE;


	BOOL fwrd = n_chara_collision_cliff_detect( p, data_fwrd );
	BOOL midl = n_chara_collision_cliff_detect( p, data_midl );
	BOOL rear = n_chara_collision_cliff_detect( p, data_rear );
//NSLog( @"%d %d %d", fwrd, midl, rear );

	if ( rear )
	{
		if (
			( fwrd == FALSE )
			&&
			( midl == FALSE )
			&&
			( 3 < n_chara_collision_fall_condition( p, tx_midl / p->mapchip_unit, ty, 4 ) )
		)
		{
			n_type_gfx threshold = tx_rear % p->mapchip_unit;
//NSLog( @"%d / %d", threshold, p->mapchip_unit );

			if ( p->direction == N_NN2_DIRECTION_LEFT )
			{
				threshold = p->mapchip_unit - threshold;
			} else
			if ( p->direction == N_NN2_DIRECTION_RIGHT )
			{
				//
			}

			if ( threshold >= 27 ) { ret = TRUE; }

			//ret = TRUE;
		}
	} else {
		ret = FALSE;
	}


	if ( ( ret )&&( n_object_birds_is_caught( p ) ) )
	{
		BOOL f = n_chip_detect_special( p, data_fwrd );
		BOOL m = n_chip_detect_special( p, data_midl );
		BOOL r = n_chip_detect_special( p, data_rear );
//NSLog( @"%d %d %d", f, m, r );

		if ( ( f )&&( m )&&( r == FALSE ) )
		{
			ret = FALSE;
		}
	}


	return ret;
}




BOOL
n_chara_collision_heading_loop( n_nn2 *p )
{

	//n_type_gfx tx = p->stage->nina_x + ( p->nina_sx / 2 );
	n_type_gfx ty = p->stage->nina_y + n_chara_margin_head( p );

	//tx = tx / p->mapchip_unit;
	ty = ty / p->mapchip_unit;


	n_chara_collision_swim( p, ty );


	n_type_gfx tx_fwrd;
	n_type_gfx tx_rear;

	if ( p->direction == N_NN2_DIRECTION_LEFT )
	{
		tx_fwrd = p->stage->nina_x +              p->nina_margin_fwrd;
		tx_rear = p->stage->nina_x + p->nina_sx - p->nina_margin_rear;
	} else
	//if ( p->direction == N_NN2_DIRECTION_RIGHT )
	{
		tx_fwrd = p->stage->nina_x + p->nina_sx - p->nina_margin_fwrd;
		tx_rear = p->stage->nina_x +              p->nina_margin_rear;
	}

	tx_fwrd /= p->mapchip_unit;
	tx_rear /= p->mapchip_unit;


	BOOL ret = FALSE;;

	n_posix_loop
	{
		n_type_gfx tx = tx_fwrd;


		u32 data = 0; n_bmp_ptr_get( &p->stage->map, tx, ty, &data );

		if ( n_chip_detect_block( p, data ) )
		{
			ret = TRUE;

			if ( N_CHIP_BRICK == n_chip_data_kind( data ) )
			{
				n_brick_move_action_on( p, tx, ty, YES );
			}

			if ( N_CHIP_WEATHER == n_chip_data_kind( data ) )
			{
				n_brick_move_action_on( p, tx, ty, YES );
			}
		} else
		if ( n_chip_detect_special( p, data ) )
		{
			if ( n_object_birds_is_caught( p ) )
			{
				ret = TRUE;
			}
		} else
		if ( n_chip_detect_hidden( p, data ) )
		{
			ret = TRUE;

			n_chip_hidden_show( p, tx, ty );
			n_brick_move_action_on( p, tx, ty, YES );
		}

		if ( p->direction == N_NN2_DIRECTION_LEFT )
		{
			tx_fwrd++;
			if ( tx_fwrd > tx_rear ) { break; }
		} else {
			tx_fwrd--;
			if ( tx_fwrd < tx_rear ) { break; }
		}
	}


	return ret;
}

BOOL
n_chara_collision_heading( n_nn2 *p )
{
//return FALSE;

	if ( N_NN2_DEBUG_LOOP ) { n_nn2_debug_hangup_detector( @"n_chara_collision_heading()" ); }


	static CGFloat p_pos = -1;

	CGFloat value;
	if ( p->swim_onoff )
	{
		value = p->powerup->swim_up;
	} else {
		value = p->powerup->jump_up;
	}

	CGFloat pos = n_nn2_flowy_effect( value, p->jump_float );
	if ( p_pos == -1 ) { p_pos = pos; }

	n_type_gfx step = pos - p_pos;
//NSLog( @"%d", step );

	p_pos = pos;


	if ( p->hipdrop_timer > n_posix_tickcount() )
	{
		step *= 2;

		p->hipdrop_high_jump_lock = TRUE;
	} else
	if ( p->swim_onoff )
	{
		step /= 6;
	}


	BOOL ret = FALSE;

	n_type_gfx add = 0;

	n_posix_loop
	{
		n_type_gfx nina = p->stage->nina_y;

		p->stage->nina_y--;

		if ( n_chara_collision_heading_loop( p ) )
		{
			p->stage->nina_y = nina;
			ret = TRUE;

			break;
		} else
		//
		{
			add++;
			if ( add >= step ) { break; }
		}
	}


	return ret;
}
