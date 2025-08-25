// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




extern BOOL n_nn2_stage_stand_onoff_horz( n_nn2 *p, n_sprite *s );
extern BOOL n_nn2_stage_stand_onoff_vert( n_nn2 *p, n_sprite *s );
extern BOOL n_nn2_stage_stand_onoff_turn( n_nn2 *p, n_sprite *s );

extern BOOL n_object_handheld_go( n_nn2 *p );




#define n_nn2_map_move_data_kind n_bmp_r_mac




void
n_object_lift_reset( n_nn2 *p, n_sprite *s )
{

	s->direction = N_NN2_DIRECTION_LEFT;
	s->is_landed = TRUE;

	s->jump_state = N_NN2_JUMP_STATE_RISE;
	s->jump_max   = 400;

	s->sx = N_BMP_SX( s->obj[ 0 ].bmp[ 0 ] );
	s->sy = N_BMP_SY( s->obj[ 0 ].bmp[ 0 ] );


	return;
}




// internal
void
n_object_lift_load_single( n_nn2 *p, n_bmp *bmp, CGFloat ratio )
{

	n_nn2_rc_load( @"rc/object/lift/body", bmp, p->scaler );
	n_bmp_flush_transcopy( &p->lift_bmp_face, bmp );

	n_type_gfx sx = N_BMP_SX( bmp );
	n_type_gfx sy = N_BMP_SY( bmp );

	n_bmp_resampler( bmp, ratio, ratio );
	n_bmp_resizer( bmp, sx,sy, n_bmp_white_invisible, N_BMP_RESIZER_CENTER );


	return;
}

// internal
void
n_object_lift_init( n_nn2 *p, n_sprite *s )
{

	n_sprite_zero( s );


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
			bmp[ n ] = &p->lift_bmp[ 0 ]; ox[ n ] = 0; oy[ n ] = 0; n++;
			bmp[ n ] = NULL;
		} else
		if ( i == 1 )
		{
			int n = 0;
			bmp[ n ] = &p->lift_bmp[ 1 ]; ox[ n ] = 0; oy[ n ] = 0; n++;
			bmp[ n ] = NULL;
		} else
		if ( i == 2 )
		{
			int n = 0;
			bmp[ n ] = &p->lift_bmp[ 2 ]; ox[ n ] = 0; oy[ n ] = 0; n++;
			bmp[ n ] = NULL;
		} else
		if ( i == 3 )
		{
			int n = 0;
			bmp[ n ] = &p->lift_bmp[ 3 ]; ox[ n ] = 0; oy[ n ] = 0; n++;
			bmp[ n ] = NULL;
		} else
		//
		{
			break;
		}

		n_sprite_set( s, i, bmp, ox, oy, 0, interval );

		i++;
	}


	s->option    = N_SPRITE_OPTION_NONE;
	s->direction = N_NN2_DIRECTION_LEFT;

	s->sx = N_BMP_SX( s->obj[ 0 ].bmp[ 0 ] );
	s->sy = N_BMP_SY( s->obj[ 0 ].bmp[ 0 ] );


	s->margin_sx = s->margin_sy = 24 * p->zoom;


	return;
}




void
n_object_lift_draw( n_nn2 *p, n_sprite *s )
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

		n_chara_bmp_copy( p, bmp, b, ox,oy, s->direction );

		i++;
		if ( i >= N_SPRITE_MAX ) { break; }
	}


	n_type_gfx x = s->x + s->ox - n_camerawork_x( p );
	n_type_gfx y = s->y + s->oy - n_camerawork_y( p );

	n_nn2_bmp_draw( p, b, x, y );


	return;
}




void
n_object_lift_action_horz( n_nn2 *p, n_sprite *s )
{
//return;

	if ( s->invisible ) { return; }


	const int step = N_OBJ_STEP_LIFT * 2;


	n_type_gfx add_x = 0;

	n_posix_loop
	{
		n_type_gfx nina = p->stage->nina_x;
		n_type_gfx obj  = 0;

		if ( s->sprite_lift != NULL )
		{
			obj = s->sprite_lift->x;
		}

		if ( s->direction == N_NN2_DIRECTION_LEFT )
		{
			s->x--;
			if ( n_nn2_stage_stand_onoff_horz( p, s ) ) { p->stage->nina_x--; }
			if ( s->sprite_lift != NULL ) { s->sprite_lift->x--; }
		} else
		if ( s->direction == N_NN2_DIRECTION_RIGHT )
		{
			s->x++;
			if ( n_nn2_stage_stand_onoff_horz( p, s ) ) { p->stage->nina_x++; }
			if ( s->sprite_lift != NULL ) { s->sprite_lift->x++; }
		} else {
			break;
		}

		u32 data = 0; n_bmp_ptr_get( &p->stage->map_move_h, s->x / p->mapchip_unit, s->y / p->mapchip_unit, &data );
//NSLog( @"%d %d %d", n_bmp_r( data ), n_bmp_g( data ), n_bmp_b( data ) );
		if ( 255 == n_nn2_map_move_data_kind( data ) )
		{
			if ( s->direction == N_NN2_DIRECTION_LEFT )
			{
				s->direction = N_NN2_DIRECTION_RIGHT;
			} else {
				s->direction = N_NN2_DIRECTION_LEFT;
			}
		}

		if (
			( n_nn2_stage_stand_onoff_horz( p, s ) )
			&&
			( n_chara_collision_wall_lr_loop( p ) & N_CHARA_COLLISION_LR_WALL )
		)
		{
			p->stage->nina_x = nina;
			break;
		} else
		//
		{
			add_x++;
			if ( add_x >= step )
			{
				break;
			}
		}
	}


	return;
}

void
n_object_lift_action_vert( n_nn2 *p, n_sprite *s )
{
//return;
//NSLog( @"%d", p->jump_state );

	if ( s->invisible ) { return; }


	const int step = N_OBJ_STEP_LIFT;


	if ( s->jump_state == N_NN2_JUMP_STATE_NONE )
	{
		s->jump_state = N_NN2_JUMP_STATE_FALL;
	}


	if ( s->jump_state == N_NN2_JUMP_STATE_RISE )
	{

		n_type_gfx add_y = 0;

		n_posix_loop
		{
			n_type_gfx prev_y = s->y;
			n_type_gfx nina_y = p->stage->nina_y;

			s->y--;
			if ( n_nn2_stage_stand_onoff_vert( p, s ) ) { p->stage->nina_y--; }

			if ( n_object_collision_heading_loop( p, s ) )
			{
				s->            y = prev_y;
				p->stage->nina_y = nina_y;

				s->jump_state = N_NN2_JUMP_STATE_FALL;

				break;
			} else {
				u32 data = 0; n_bmp_ptr_get( &p->stage->map_move_v, s->x / p->mapchip_unit, s->y / p->mapchip_unit, &data );
//NSLog( @"%d %d %d", n_bmp_r( data ), n_bmp_g( data ), n_bmp_b( data ) );
				if ( 255 == n_nn2_map_move_data_kind( data ) )
				{
//NSLog( @"Stop" );
					s->jump_state = N_NN2_JUMP_STATE_FALL;

					break;
				}

				add_y++;
				if ( add_y >= step ) { break; }
			}
		}

	} else
	if ( s->jump_state == N_NN2_JUMP_STATE_FALL )
	{

		n_type_gfx add_y = 0;
		n_posix_loop
		{
			n_type_gfx prev_y = s->y;
			n_type_gfx nina_y = p->stage->nina_y;

			s->y++;
			if ( n_nn2_stage_stand_onoff_vert( p, s ) ) { p->stage->nina_y++; }

			if ( n_object_collision_gravity_loop( p, s ) )
			{
				s->            y = prev_y;
				p->stage->nina_y = nina_y;

				s->jump_state = N_NN2_JUMP_STATE_RISE;

				break;
			} else {
				u32 data = 0; n_bmp_ptr_get( &p->stage->map_move_v, s->x / p->mapchip_unit, s->y / p->mapchip_unit, &data );
//NSLog( @"%d %d %d", n_bmp_r( data ), n_bmp_g( data ), n_bmp_b( data ) );
				if ( 255 == n_nn2_map_move_data_kind( data ) )
				{
					s->jump_state = N_NN2_JUMP_STATE_RISE;

					break;
				}

				add_y++;
				if ( add_y >= step ) { break; }
			}
		}

	}


	return;
}

void
n_object_lift_action_turn_guideline( n_nn2 *p, n_sprite *s )
{

	n_type_gfx size = 300;

	n_type_gfx x = 1500;
	n_type_gfx y =  500;

	x -= n_camerawork_x( p );
	y -= n_camerawork_y( p );

	n_bmp_hoop( p->canvas, x-size,y-size,size*2,size*2, 2, n_chip_guidline_color( p ) );


	return;
}

void
n_object_lift_action_turn( n_nn2 *p, n_sprite *s )
{

	if ( s->invisible ) { return; }


	static CGFloat step = 0;

	n_type_gfx size = 300;

	n_type_gfx x = 1500;
	n_type_gfx y =  500;

	CGFloat _s = sin( M_PI * 2 * step );
	CGFloat _c = cos( M_PI * 2 * step );

	if ( _s >= 0 )
	{
		s->direction = N_NN2_DIRECTION_RIGHT;
	} else {
		s->direction = N_NN2_DIRECTION_LEFT;
	}

	s->x = x + ( size * _s ) - ( s->sx / 2 );
	s->y = y + ( size * _c ) - ( s->sy / 2 );

	step -= 0.0015;

	if ( p->jump_state )
	{
		extern void n_nn2_stage_stand_all_off( n_nn2 *p );
		n_nn2_stage_stand_all_off( p );
	}

	if ( n_nn2_stage_stand_onoff_turn( p, s ) )
	{
		p->stage->nina_x = s->x + s->stand_offset;
		p->stage->nina_y = s->y + s->margin_sy - p->nina_sy + p->nina_margin_foot;
	}


	return;
}

