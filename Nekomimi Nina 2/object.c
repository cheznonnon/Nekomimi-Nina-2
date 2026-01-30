// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




extern BOOL n_object_collision_gravity_loop( n_nn2 *p, n_sprite *s );
extern BOOL n_object_collision_heading_loop( n_nn2 *p, n_sprite *s );

extern BOOL n_nn2_stage_stand_onoff( n_nn2 *p );

extern void n_object_handheld_on( n_nn2 *p, n_sprite *s );

extern void n_nn2_stage_throw_go( n_nn2 *p, n_sprite *s, n_type_gfx x, n_type_gfx y );




n_sprite_single*
n_object_get( n_sprite *s )
{

	if ( s->frame >= s->obj[ s->index ].frame_interval )
	{
		s->frame = 0;

		s->index++;
		if ( NULL == s->obj[ s->index ].bmp[ 0 ] )
		{
			s->index = 0;
		}
	}


	return &s->obj[ s->index ];
}

n_sprite_single*
n_object_get_with_interval( n_sprite *s, int interval )
{

	if ( s->frame >= interval )
	{
		s->frame = 0;

		s->index++;
		if ( NULL == s->obj[ s->index ].bmp[ 0 ] )
		{
			s->index = 0;
		}
	}


	return &s->obj[ s->index ];
}




void
n_object_turn( n_nn2 *p, n_sprite *s )
{

	if ( s == NULL ) { return; }

	if ( s->invisible ) { return; }


	if ( s->direction == N_NN2_DIRECTION_LEFT )
	{
		s->direction = N_NN2_DIRECTION_RIGHT;
	} else {
		s->direction = N_NN2_DIRECTION_LEFT;
	}


	return;
}




int
n_object_clamp( n_nn2 *p, n_sprite *s )
{

	int ret = N_CHARA_COLLISION_LR_NONE;

	if ( p->stage->camera_mode == N_CAMERAWORK_STOP )
	{
		if ( s->x < 0 )
		{
			ret = N_CHARA_COLLISION_LR_WALL | N_CHARA_COLLISION_STOP__L | N_CHARA_COLLISION_LR_STOP;
		}
		if ( ( s->x + s->sx ) > ( p->stage->map_max_sx + p->sx ) )
		{
			ret = N_CHARA_COLLISION_LR_WALL | N_CHARA_COLLISION_STOP__R | N_CHARA_COLLISION_LR_STOP;
		}
	} else
	if ( p->stage->camera_mode == N_CAMERAWORK_WARP )
	{
		if ( s->x < -s->sx )
		{
			s->x += p->stage->map_sx;
		} else
		if ( s->x >= p->stage->map_sx )
		{
			s->x -= p->stage->map_sx + s->sx - 1;
		}
	} else
	if ( p->stage->camera_mode == N_CAMERAWORK_LOOP )
	{
		if ( s->x < 0 )
		{
			s->x += p->stage->map_sx;
		} else
		if ( s->x >= p->stage->map_sx )
		{
			s->x -= p->stage->map_sx;
		}
	}


	return ret;
}




int
n_object_collision_wall_lr_loop( n_nn2 *p, n_sprite *s )
{
//return N_CHARA_COLLISION_LR_NONE;

	if ( s == NULL ) { return N_CHARA_COLLISION_LR_NONE; }

	if ( s->invisible ) { return N_CHARA_COLLISION_LR_NONE; }

	if ( s->dokan_onoff ) { return N_CHARA_COLLISION_LR_NONE; }


	if ( N_NN2_DEBUG_LOOP ) { n_nn2_debug_hangup_detector( @"n_object_collision_wall_lr_loop_main()" ); }


	int clamp = n_object_clamp( p, s );
	if ( clamp ) { return clamp; }


	int ret = N_CHARA_COLLISION_LR_NONE;

	if ( s->direction == N_NN2_DIRECTION_LEFT )
	{

		n_type_gfx tx = s->x - 1;

		tx += s->margin_sx;

		tx = tx / p->mapchip_unit;


		n_type_gfx ty_u = s->y;
		n_type_gfx ty_d = s->y + s->sy;

		ty_u += s->margin_sy;
		ty_d -= s->margin_sy;


		ty_u = ty_u / p->mapchip_unit;
		ty_d = ty_d / p->mapchip_unit;


		n_posix_loop
		{

			if ( n_chip_detect( p, tx, ty_u ) )
			{
				ret |= N_CHARA_COLLISION_LR_WALL;
				ret |= N_CHARA_COLLISION_LR_WL_L;

				break;
			}

			ty_u++;
			if ( ty_u >= ty_d ) { break; }
		}

	} else
	if ( s->direction == N_NN2_DIRECTION_RIGHT )
	{

		n_type_gfx tx = s->x + s->sx;

		tx += s->margin_sx;

		tx = tx / p->mapchip_unit;


		n_type_gfx ty_u = s->y;
		n_type_gfx ty_d = s->y + s->sy;

		ty_u += s->margin_sy;
		ty_d -= s->margin_sy;

		ty_u = ty_u / p->mapchip_unit;
		ty_d = ty_d / p->mapchip_unit;


		n_posix_loop
		{

			if ( n_chip_detect( p, tx, ty_u ) )
			{
				ret |= N_CHARA_COLLISION_LR_WALL;
				ret |= N_CHARA_COLLISION_LR_WL_R;

				break;
			}

			ty_u++;
			if ( ty_u >= ty_d ) { break; }
		}

	}


	return ret;
}

BOOL
n_object_collision_lr( n_nn2 *p, n_sprite *s )
{
//return FALSE;

	if ( s == NULL ) { return FALSE; }

	if ( s->invisible ) { return FALSE; }

	if ( s->dokan_onoff ) { return FALSE; }


	if ( N_NN2_DEBUG_LOOP ) { n_nn2_debug_hangup_detector( @"n_object_collision_lr()" ); }


	BOOL ret = FALSE;

	int step = s->move_step;

	n_type_gfx add = 0;

	n_posix_loop
	{
		n_type_gfx prev = s->x;

		if ( s->direction == N_NN2_DIRECTION_LEFT )
		{
			s->x--;
		} else
		if ( s->direction == N_NN2_DIRECTION_RIGHT )
		{
			s->x++;
		} else {
			break;
		}

		int wall = n_object_collision_wall_lr_loop( p, s );
		if ( wall & N_CHARA_COLLISION_LR_WALL )
		{
			ret  = TRUE;
			s->x = prev;

			n_object_turn( p, s );

			break;
		} else
		//
		{
			add++;
			if ( add >= step )
			{
				break;
			}
		}
	}


	return ret;
}




void
n_object_collision_dokan( n_nn2 *p, n_sprite *s, n_type_gfx tx, n_type_gfx ty, u32 data )
{

	if ( p->object_handheld_sprite == s )
	{
		//
	} else
	if ( s->dokan_onoff == FALSE )
	{
		s->dokan_onoff            = TRUE;
		s->dokan_disappear_offset = 0;
		s->dokan_tx               = tx;
		s->dokan_ty               = ty;

		p->object_dokan_sprite = s;

		if ( N_CHIP_DOKAN_Q_L == n_chip_data_kind( data ) )
		{
			s->dokan_center = ( tx * p->mapchip_unit ) + p->mapchip_unit;
		} else
		if ( N_CHIP_DOKAN_Q_R == n_chip_data_kind( data ) )
		{
			s->dokan_center = ( tx * p->mapchip_unit );
		}
	}


	return;
}

BOOL
n_object_collision_ud_loop( n_nn2 *p, n_sprite *s, BOOL is_gravity )
{
 //return FALSE;

	if ( s == NULL ) { return FALSE; }

	if ( s->invisible ) { return FALSE; }


	if ( N_NN2_DEBUG_LOOP ) { n_nn2_debug_hangup_detector( @"n_object_collision_ud_loop()" ); }


	n_type_gfx tx_l = s->x;
	n_type_gfx tx_m = s->x + ( s->sx / 2 );
	n_type_gfx tx_r = s->x + s->sx;

	tx_l += s->margin_sx;
	tx_r -= s->margin_sx;


	n_type_gfx ty;

	if ( is_gravity )
	{
		ty = s->y + s->sy - 1; // [!] : -1 : rock use this
		ty -= s->margin_sy;
	} else {
		ty = s->y;
		ty += s->margin_sy;
	}


	tx_l = tx_l / p->mapchip_unit;
	tx_m = tx_m / p->mapchip_unit;
	tx_r = tx_r / p->mapchip_unit;
	ty   = ty   / p->mapchip_unit;

	if ( tx_l < 0 ) { tx_l = 0; }
	if ( tx_r < 0 ) { tx_l = 0; }

	if ( tx_l >= N_BMP_SX( &p->stage->map ) ) { tx_l = N_BMP_SX( &p->stage->map ) - 1; }
	if ( tx_r >= N_BMP_SX( &p->stage->map ) ) { tx_r = N_BMP_SX( &p->stage->map ) - 1; }


	if ( is_gravity )
	{
		u32 dat_l = 0; n_bmp_ptr_get( &p->stage->map, tx_l, ty, &dat_l );
		u32 dat_m = 0; n_bmp_ptr_get( &p->stage->map, tx_m, ty, &dat_m );
		u32 dat_r = 0; n_bmp_ptr_get( &p->stage->map, tx_r, ty, &dat_r );

		if (
			( N_CHIP_DOKAN_Q_L == n_chip_data_kind( dat_m ) )
			||
			( N_CHIP_DOKAN_Q_R == n_chip_data_kind( dat_m ) )
		)
		{
			n_object_collision_dokan( p, s, tx_m, ty, dat_m );
		} else
		if ( N_CHIP_DOKAN_Q_R == n_chip_data_kind( dat_l ) )
		{
			n_object_collision_dokan( p, s, tx_l, ty, dat_l );
		} else
		if ( N_CHIP_DOKAN_Q_L == n_chip_data_kind( dat_r ) )
		{
			n_object_collision_dokan( p, s, tx_r, ty, dat_r );
		}
	}


	BOOL ret = FALSE;

//NSLog( @"%d %d", tx_l, tx_r );

//p->debug_chip_x = tx_r;
//p->debug_chip_y = ty;

	n_posix_loop
	{

		u32 data = 0; n_bmp_ptr_get( &p->stage->map, tx_l, ty, &data );

		if ( n_chip_detect_block( p, data ) )
		{
			ret = TRUE;
//p->debug_chip_x = tx_l;
//p->debug_chip_y = ty;

			break;
		} else
		if ( n_chip_detect_hill( p, data ) )
		{
			if ( is_gravity ) { ret = TRUE; break; }
		} else
		if (
			( N_CHIP_DOKAN_Q_L == n_chip_data_kind( data ) )
			||
			( N_CHIP_DOKAN_Q_R == n_chip_data_kind( data ) )
		)
		{
			if ( is_gravity ) { ret = TRUE; break; }
		}

		tx_l++;
		if ( tx_l > tx_r ) { break; } // [!] : don't use ">=" : left edge uses "tx_r"
	}


	return ret;
}

BOOL
n_object_collision_gravity_loop( n_nn2 *p, n_sprite *s )
{
	return n_object_collision_ud_loop( p, s, YES );
}

BOOL
n_object_collision_heading_loop( n_nn2 *p, n_sprite *s )
{
	return n_object_collision_ud_loop( p, s,  NO );
}




BOOL
n_object_collision_gravity( n_nn2 *p, n_sprite *s )
{

	if ( s == NULL ) { return FALSE; }

	if ( s->invisible ) { return FALSE; }

	if ( s->dokan_onoff )
	{
		//return FALSE;

		const n_type_gfx step = N_NN2_STEP_FALL / 3; // [!] : same as Nina

		n_type_gfx py_1 = s->dokan_disappear_offset;
		n_type_gfx py_2 = s->y;

		s->dokan_disappear_offset += step;
		s->y                      += step;
		if ( s->dokan_disappear_offset >= s->sy )
		{
			s->dokan_disappear_offset = py_1;
			s->y                      = py_2;

			s->dokan_onoff = FALSE;

			p->object_dokan_sprite = NULL;

			n_nn2_sound_effect_play( N_NN2_SOUND_DOKAN );

			n_nn2_stage_throw_go( p, s, s->dokan_tx, s->dokan_ty );
		}

		if ( ( s->x + ( s->sx / 2 ) ) < s->dokan_center )
		{
			s->x += step;
		} else
		if ( ( s->x + ( s->sx / 2 ) ) > s->dokan_center )
		{
			s->x -= step;
		}

		return FALSE;
	}


	if ( N_NN2_DEBUG_LOOP ) { n_nn2_debug_hangup_detector( @"n_object_collision_gravity()" ); }


	n_type_gfx step = s->fall_step;

	if ( p->swim_onoff )
	{
		if ( s->dokan_onoff )
		{
			step /= 3;
		} else {
			step /= 4;
		}
	}


	BOOL ret = FALSE;


	n_type_gfx add_y = 0;
	n_posix_loop
	{
		n_type_gfx py = s->y;

		s->y++;

		if ( n_object_collision_gravity_loop( p, s ) )
		{
			s->y = py;
			ret = TRUE;
			break;
		} else
		//
		{
			add_y++;
			if ( add_y >= step ) { break; }
		}
	}


	if ( s->y > p->stage->map_sy )
	{
//if ( s == &p->share->chick_sprite ) { NSLog( @"chick" ); }
//if ( s == &p->share->rocks_sprite ) { NSLog( @"rocks" ); }

		n_nn2_stage_throw_go( p, s, -1, -1 );
	}


	return ret;
}




BOOL
n_object_collision_heading( n_nn2 *p, n_sprite *s )
{
//return FALSE;

	if ( s == NULL ) { return FALSE; }

	if ( s->invisible ) { return FALSE; }

	if ( s->dokan_onoff ) { return FALSE; }


	if ( N_NN2_DEBUG_LOOP ) { n_nn2_debug_hangup_detector( @"n_object_collision_heading()" ); }


	CGFloat pos = n_nn2_flowy_effect( s->jump_max, s->jump_float );

	n_type_gfx step = pos - s->jump_pos_prv;

	s->jump_pos_prv = pos;


	BOOL ret = FALSE;

	n_type_gfx add_y = 0;

	n_posix_loop
	{
		n_type_gfx py = s->y;

		s->y--;
		if ( n_object_collision_heading_loop( p, s ) )
		{
			s->y = py;
			ret = TRUE;
			break;
		} else
		//
		{
			add_y++;
			if ( add_y >= step ) { break; }
		}
	}


	return ret;
}




BOOL
n_object_lift_collision_is_hit( n_nn2 *p, n_sprite *s, BOOL on_lift )
{
//return FALSE;

	// [!] : this for Nina : n_interobject_collision_lift() is used for objects


	if ( s->invisible ) { return FALSE; }


	BOOL ret_ud = FALSE;
	BOOL ret_lr = FALSE;

	{
		n_type_gfx f = s->y + s->margin_sy;
		n_type_gfx t;

		if ( on_lift )
		{
			t = f + N_OBJ_ON_LIFT;
		} else {
			t = s->y + s->sy - s->margin_sy;
		}

		n_type_gfx d = p->stage->nina_y + p->nina_sy - p->nina_margin_foot;

		if ( ( d >= f )&&( d <= t ) ) { ret_ud = TRUE; }

//if ( s == &p->stage->cld_h_0_sprite ) { NSLog( @"%d : %d %d : %d : %d", p->nina_y, f, t, d, ret_ud ); }
//if ( s == &p->stage->cld_v_0_sprite ) { NSLog( @"%d : %d %d : %d : %d : %d : %d ", p->nina_y, f, t, d, ret_ud, s->jump_state, N_OBJ_ON_CLOUD ); }
//if ( s == &p->share->rocks_sprite ) { NSLog( @"%d : %d %d %d", ret_ud, f, d, t ); }
	}

	{
		n_type_gfx f = s->x + s->margin_sx;
		n_type_gfx t = s->x + s->sx - s->margin_sx;

		n_type_gfx l = p->stage->nina_x;
		n_type_gfx r = p->stage->nina_x + p->nina_sx;

		if ( p->direction == N_NN2_DIRECTION_LEFT )
		{
			l += p->nina_margin_fwrd;
			r -= p->nina_margin_rear;
		} else
		if ( p->direction == N_NN2_DIRECTION_RIGHT )
		{
			l += p->nina_margin_rear;
			r -= p->nina_margin_fwrd;
		}

		if ( p->direction == N_NN2_DIRECTION_LEFT )
		{
			if ( n_nn2_stage_stand_onoff( p ) )
			{
				if ( ( l >= f )&&( l <= t ) ) { ret_lr = TRUE; }
			}

			if ( ( r >= f )&&( r <= t ) ) { ret_lr = TRUE; }
		} else
		if ( p->direction == N_NN2_DIRECTION_RIGHT )
		{
			if ( n_nn2_stage_stand_onoff( p ) )
			{
				if ( ( r >= f )&&( r <= t ) ) { ret_lr = TRUE; }
			}

			if ( ( l >= f )&&( l <= t ) ) { ret_lr = TRUE; }
		}
	}

//if ( s == &p->share->rocks_sprite ) { NSLog( @"%d %d", ret_ud, ret_lr ); }


	return ( ( ret_ud )&&( ret_lr ) );
}




#define n_nn2_map_queue_data_kind n_bmp_r_mac

#define N_OBJECT_MAP_QUEUE_NINA    ( 1 )
#define N_OBJECT_MAP_QUEUE_CLOUD_H ( 2 )
#define N_OBJECT_MAP_QUEUE_CLOUD_V ( 3 )
#define N_OBJECT_MAP_QUEUE_CHICK   ( 4 )
#define N_OBJECT_MAP_QUEUE_ROCKS   ( 5 )
#define N_OBJECT_MAP_QUEUE_KUINA   ( 6 )
#define N_OBJECT_MAP_QUEUE_CLD_H_0 ( 7 )
#define N_OBJECT_MAP_QUEUE_CLD_V_0 ( 8 )
#define N_OBJECT_MAP_QUEUE_CLD_V_1 ( 9 )
#define N_OBJECT_MAP_QUEUE_CLD_V_2 ( 10 )
#define N_OBJECT_MAP_QUEUE_CLD_V_3 ( 11 )
#define N_OBJECT_MAP_QUEUE_FISH_1  ( 100 )
#define N_OBJECT_MAP_QUEUE_FISH_2  ( 101 )
#define N_OBJECT_MAP_QUEUE_FISH_3  ( 102 )

void
n_object_map_queue( n_nn2 *p, n_bmp *queue, n_nn2_stage *stage, int target )
{

	if ( n_bmp_error( queue ) ) { return; }


	if ( N_NN2_DEBUG_LOOP ) { n_nn2_debug_hangup_detector( @"n_object_map_queue()" ); }


	n_type_gfx x = 0;
	n_type_gfx y = 0;
	n_posix_loop
	{

		u32 data = 0; n_bmp_ptr_get( queue, x, y, &data );

		if ( target == n_nn2_map_queue_data_kind( data ) )
		{
			if ( target == N_OBJECT_MAP_QUEUE_NINA )
			{
				stage->nina_x = x * p->mapchip_unit;
				stage->nina_y = y * p->mapchip_unit;
//NSLog( @"Found : %d %d", stage->nina_x, stage->nina_y );

				n_chara_collision_force_landing( p );

				p->jump_state = 0;
				p->idle_onoff = TRUE;

				break;
			} else
			if ( target == N_OBJECT_MAP_QUEUE_CLOUD_H )
			{
				stage->cld_h_0_sprite.x = x * p->mapchip_unit;
				stage->cld_h_0_sprite.y = y * p->mapchip_unit;
//NSLog( @"Found : %d %d", stage->cld_h_0_sprite.x, stage->cld_h_0_sprite.y );

				break;
			} else
			if ( target == N_OBJECT_MAP_QUEUE_CLOUD_V )
			{
				stage->cld_v_0_sprite.x = x * p->mapchip_unit;
				stage->cld_v_0_sprite.y = y * p->mapchip_unit;

				break;
			} else
			if ( target == N_OBJECT_MAP_QUEUE_CHICK )
			{
				p->share->chick_sprite.x = x * p->mapchip_unit;
				p->share->chick_sprite.y = y * p->mapchip_unit;

				break;
			} else
			if ( target == N_OBJECT_MAP_QUEUE_KUINA )
			{
				p->share->kuina_sprite.x = x * p->mapchip_unit;
				p->share->kuina_sprite.y = y * p->mapchip_unit;

				break;
			} else
			if ( target == N_OBJECT_MAP_QUEUE_ROCKS )
			{
				p->share->rocks_sprite.x = x * p->mapchip_unit;
				p->share->rocks_sprite.y = y * p->mapchip_unit;

//NSLog( @"Found : %d %d", p->share->rocks_sprite.x, p->share->rocks_sprite.y );

				break;
			} else
			if ( target == N_OBJECT_MAP_QUEUE_CLD_H_0 )
			{
				stage->cld_h_0_sprite.x = x * p->mapchip_unit;
				stage->cld_h_0_sprite.y = y * p->mapchip_unit;

				break;
			} else
			if ( target == N_OBJECT_MAP_QUEUE_CLD_V_0 )
			{
				stage->cld_v_0_sprite.x = x * p->mapchip_unit;
				stage->cld_v_0_sprite.y = y * p->mapchip_unit;

				break;
			} else
			if ( target == N_OBJECT_MAP_QUEUE_CLD_V_1 )
			{
				stage->cld_v_1_sprite.x = x * p->mapchip_unit;
				stage->cld_v_1_sprite.y = y * p->mapchip_unit;

				break;
			} else
			if ( target == N_OBJECT_MAP_QUEUE_CLD_V_2 )
			{
				//stage->cld_v_2_sprite.x = x * p->mapchip_unit;
				//stage->cld_v_2_sprite.y = y * p->mapchip_unit;

				break;
			} else
			if ( target == N_OBJECT_MAP_QUEUE_CLD_V_3 )
			{
				//stage->cld_v_3_sprite.x = x * p->mapchip_unit;
				//stage->cld_v_3_sprite.y = y * p->mapchip_unit;

				break;
			} else
			if ( target == N_OBJECT_MAP_QUEUE_FISH_1 )
			{
				p->share->fish1_sprite.x = x * p->mapchip_unit;
				p->share->fish1_sprite.y = y * p->mapchip_unit;

				break;
			} else
			if ( target == N_OBJECT_MAP_QUEUE_FISH_2 )
			{
				p->share->fish2_sprite.x = x * p->mapchip_unit;
				p->share->fish2_sprite.y = y * p->mapchip_unit;

				break;
			} else
			if ( target == N_OBJECT_MAP_QUEUE_FISH_3 )
			{
				p->share->fish3_sprite.x = x * p->mapchip_unit;
				p->share->fish3_sprite.y = y * p->mapchip_unit;

				break;
			}
		}


		x++;
		if ( x >= N_BMP_SX( queue ) )
		{
			x = 0;

			y++;
			if ( y >= N_BMP_SY( queue ) ) { break; }
		}
	}

//NSLog( @"Cloud Horz : %d %d", stage->cld_h_0_sprite.x, stage->cld_h_0_sprite.y );


	return;
}

void
n_object_init( n_nn2 *p, n_sprite *s, n_bmp *bmp_obj )
{

	n_sprite_zero( s );

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
			bmp[ n ] = bmp_obj; ox[ n ] = 0; oy[ n ] = 0; n++;
			bmp[ n ] = NULL;
		} else {
			break;
		}

		n_sprite_set( s, i, bmp, ox, oy, 0, interval );

		i++;
	}


	s->sx = N_BMP_SX( bmp_obj );
	s->sy = N_BMP_SY( bmp_obj );


	s->direction = N_NN2_DIRECTION_LEFT;

	s->move_step = N_OBJ_STEP_MOVE;
	s->fall_step = N_OBJ_STEP_FALL;


	return;
}




void
n_object_share_draw( n_nn2 *p, n_sprite *s )
{

	if ( s->invisible ) { return; }


	n_bmp *b = n_nn2_shared_bmp( s );


	n_sprite_single *obj = n_object_get( s );

	int i = 0;
	n_posix_loop
	{

		if ( obj->bmp[ i ] == NULL ) { break; }

		n_bmp *bmp = obj->bmp[ i ];
		int    ox  = obj-> ox[ i ];
		int    oy  = obj-> oy[ i ];

		n_sprite *spr = NULL;
		if ( s == p->object_dokan_sprite ) { spr = s; }

		n_chara_bmp_copy_all( p, bmp, b, ox,oy, s->direction, NO, spr );

		i++;
		if ( i >= N_SPRITE_MAX ) { break; }
	}


	n_type_gfx x = s->x + s->ox - n_camerawork_x( p );
	n_type_gfx y = s->y + s->oy - n_camerawork_y( p );

	n_nn2_bmp_draw_simple( p, b, x, y );


	return;
}




void
n_object_action_move( n_nn2 *p, n_sprite *s )
{

	if ( s->invisible ) { return; }


	if ( n_object_collision_gravity( p, s ) )
	{
//NSLog( @"landing" );
		s->jump_state = N_NN2_JUMP_STATE_NONE;
		s->is_landed  = TRUE;
	}

	if ( s->is_landed )
	{
		n_object_collision_lr( p, s );
	}


	return;
}




void
n_object_action_jump( n_nn2 *p, n_sprite *s )
{

	if ( s->invisible ) { return; }


	if ( s->jump_state == N_NN2_JUMP_STATE_FALL )
	{

		BOOL ret = n_object_collision_gravity( p, s );
		if ( ret )
		{
//NSLog( @"landing" );
			s->jump_state = N_NN2_JUMP_STATE_NONE;
			s->is_landed  = TRUE;
		}

	}

	if ( s->jump_state == N_NN2_JUMP_STATE_NONE )
	{

		s->jump_state = N_NN2_JUMP_STATE_RISE;
		s->jump_float = 0.0;

	}

	if ( s->jump_state == N_NN2_JUMP_STATE_RISE )
	{
		if ( n_nn2_flowy_effect_inc( s->jump_max, &s->jump_float, 10 ) )
		{
			s->jump_state = N_NN2_JUMP_STATE_FALL;
		}

		n_object_collision_heading( p, s );
	}

	// [!] : this position is important to resolve jamming
	if ( s->is_landed )
	{
		n_object_collision_lr( p, s );
	}


	return;
}




void
n_object_rock_reset( n_nn2 *p, n_nn2_stage *s )
{

	n_sprite *t = &p->share->rocks_sprite;


	int i = 0;
	n_posix_loop
	{

		n_bmp *bmp[ N_SPRITE_MAX ];
		int     ox[ N_SPRITE_MAX ];
		int     oy[ N_SPRITE_MAX ];

		// [!] : sync with mapchip, see n_nn2_stage_animation() at stage.c
		int interval = 2;
		if ( i == 0 )
		{
			interval = 33;

			int n = 0;
			bmp[ n ] = &s->chip_rocks_1; ox[ n ] = 0; oy[ n ] = 0; n++;
			bmp[ n ] = NULL;
		} else
		if ( i == 1 )
		{
			int n = 0;
			bmp[ n ] = &s->chip_rocks_2; ox[ n ] = 0; oy[ n ] = 0; n++;
			bmp[ n ] = NULL;
		} else
		if ( i == 2 )
		{
			int n = 0;
			bmp[ n ] = &s->chip_rocks_3; ox[ n ] = 0; oy[ n ] = 0; n++;
			bmp[ n ] = NULL;
		} else
		if ( i == 3 )
		{
			int n = 0;
			bmp[ n ] = &s->chip_rocks_2; ox[ n ] = 0; oy[ n ] = 0; n++;
			bmp[ n ] = NULL;
		} else
		//
		{
			break;
		}

		n_sprite_set( t, i, bmp, ox, oy, 0, interval );

		i++;
	}


	t->sx = N_BMP_SX( &s->chip_rocks_1 );
	t->sy = N_BMP_SY( &s->chip_rocks_1 );


	return;
}

void
n_object_rocks_init( n_nn2 *p, n_sprite *s )
{

	n_sprite_zero( s );


	n_object_rock_reset( p, p->stage );


	s->option     = N_SPRITE_OPTION_ROCK;
	s->jump_state = N_NN2_JUMP_STATE_FALL;
	s->fall_step  = N_OBJ_STEP_FALL * 2;


	return;
}




void
n_object_fish_move( n_nn2 *p, n_sprite *s )
{

	if ( n_bmp_ui_timer( &s->fish_timer, n_random_range( 100 ) ) )
	{
		if ( s->fish_phase == 0 )
		{
			s->fish_move += 2;
			if ( s->fish_move >= ( N_OBJ_STEP_MOVE * 5 ) )
			{
				s->fish_phase = 1;
			}
		} else {
			s->fish_move -= 2;
			if ( s->fish_move < N_OBJ_STEP_MOVE )
			{
				s->fish_phase = 0;
			}
		}
	}


	s->move_step = s->fish_move;

	n_type_gfx delta = abs( s->x - p->stage->nina_x );
	if ( delta < 333 )
	{
		if ( s->x < p->stage->nina_x )
		{
			s->direction = N_NN2_DIRECTION_LEFT;
		} else {
			s->direction = N_NN2_DIRECTION_RIGHT;
		}
	}

	n_object_collision_lr( p, s );


	return;
}

