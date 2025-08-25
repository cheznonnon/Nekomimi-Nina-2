// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




extern BOOL n_object_collision_heading_loop( n_nn2 *p, n_sprite *s );
extern BOOL n_object_collision_gravity_loop( n_nn2 *p, n_sprite *s );
extern int  n_object_collision_wall_lr_loop( n_nn2 *p, n_sprite *s );




void
n_nn2_jam_resolver_debug( n_nn2 *p, n_sprite *a, n_sprite *b )
{

	n_posix_char *str_a;
	if ( a == &p->share->debug_move )
	{
		str_a = "A :  Move";
	} else
	if ( a == &p->share->debug_jump )
	{
		str_a = "A :  Jump";
	} else
	if ( a == &p->share->chick_sprite )
	{
		str_a = "A : Chick";
	} else
	if ( a == &p->share->kuina_sprite )
	{
		str_a = "A : Kuina";
	} else
	if ( a == &p->share->rocks_sprite )
	{
		str_a = "A : Rocks";
	} else
	//
	{
		str_a = "";
	}

	n_posix_char *str_b;
	if ( b == &p->share->debug_move )
	{
		str_b = "B :  Move";
	} else
	if ( b == &p->share->debug_jump )
	{
		str_b = "B :  Jump";
	} else
	if ( b == &p->share->chick_sprite )
	{
		str_b = "B : Chick";
	} else
	if ( b == &p->share->kuina_sprite )
	{
		str_b = "B : Kuina";
	} else
	if ( b == &p->share->kuina_sprite )
	{
		str_b = "B : Kuina";
	} else
	if ( b == &p->share->rocks_sprite )
	{
		str_b = "B : Rocks";
	} else
	//
	{
		str_b = "";
	}


	NSLog( @"%s / %s", str_a, str_b );


	return;
}

void
n_nn2_jam_resolver( n_nn2 *p )
{
//return;

	if ( N_NN2_DEBUG_LOOP ) { n_nn2_debug_hangup_detector( @"n_nn2_jam_resolver()" ); }


	if ( p->object_handheld_sprite != NULL )
	{

		n_sprite *s = p->object_handheld_sprite;

//n_nn2_debug_hangup_detector( @"n_nn2_jam_resolver() : n_object_collision_heading_loop()" );

		n_posix_loop
		{//break;
			BOOL ret_u = n_object_collision_heading_loop( p, s );
			if ( ret_u == FALSE ) { break; }

			BOOL ret_d = n_object_collision_gravity_loop( p, s );
			if ( ret_d != FALSE ) { break; }

			s->y++;
			p->stage->nina_y++;
		}

//n_nn2_debug_hangup_detector( @"n_nn2_jam_resolver() : n_object_collision_gravity_loop()" );

		if ( p->jump_state == N_NN2_JUMP_STATE_FALL )
		{
			BOOL release = FALSE;
			n_posix_loop
			{//break;

				BOOL ret_d = n_object_collision_gravity_loop( p, s );
				if ( ret_d == FALSE ) { break; }

				BOOL ret_u = n_object_collision_heading_loop( p, s );
				if ( ret_u != FALSE ) { break; }

				s->y--;
				p->stage->nina_y--;

				release = TRUE;
			}

			if ( release ) { n_object_handheld_release( p ); }
		}

		n_object_handheld_jam_resolver( p, NO );

		// [!] : when this is used, then other birds will pass through
		//return;
	}


//n_nn2_debug_hangup_detector( @"Main" );
//return;

	n_sprite *t[ 32 ];

	int n = 0;
	t[ n ] = &p->share->debug_move  ; n++;
	t[ n ] = &p->share->debug_jump  ; n++;
	t[ n ] = &p->share->chick_sprite; n++;
	t[ n ] = &p->share->kuina_sprite; n++;
	t[ n ] = &p->share->rocks_sprite; n++;
	t[ n ] = &p->share->bird1_sprite; n++;
	t[ n ] = &p->share->bird2_sprite; n++;
	t[ n ] = &p->share->bird3_sprite; n++;
	t[ n ] = NULL;


	int i = 0;
	n_posix_loop
	{//break;

		n_sprite *a = t[ i ];
		if ( a == NULL ) { break; }

		if ( a == &p->share->rocks_sprite )
		{
			i++;
			continue;
		} else
		if ( a == p->object_handheld_sprite )
		{
			i++;
			continue;
		}

		int j = 0;
		n_posix_loop
		{
			n_sprite *b = t[ j ];
			if ( b == NULL ) { break; }
//n_nn2_jam_resolver_debug( p, a, b );

			BOOL ud = n_interobject_collision_ud_detail( p, a, b, YES );
			BOOL lr = n_interobject_collision_lr_detail( p, a, b, YES );

			int          mode_ud = 0;
			n_type_gfx target_ud = 0;

			int          mode_lr = 0;
			n_type_gfx target_lr = 0;

			if ( ( ud )&&( lr ) )
			{
//n_object_collision_debug( "!", a );

				if (
					( a->collision_ud_detail == N_NN2_OBJECT_COLLISION_AB_CRASH )
					||
					( a->collision_ud_detail == N_NN2_OBJECT_COLLISION_AB_CHASE )
				)
				{
					  mode_ud = 1;
					target_ud = b->y - a->sy;

					a->jump_state = N_NN2_JUMP_STATE_RISE;
					a->jump_float = 0;
				} else
				if (
					( a->collision_ud_detail == N_NN2_OBJECT_COLLISION_BA_CRASH )
					||
					( a->collision_ud_detail == N_NN2_OBJECT_COLLISION_BA_CHASE )
				)
				{
					  mode_ud = 2;
					target_ud = b->y + b->sy;

					a->jump_state = N_NN2_JUMP_STATE_FALL;
				}

				if (
					( a->collision_lr_detail == N_NN2_OBJECT_COLLISION_AB_CRASH )
					||
					( a->collision_lr_detail == N_NN2_OBJECT_COLLISION_AB_CHASE )
				)
				{
					  mode_lr = 1;
					target_lr = b->x - a->sx;

					a->direction = N_NN2_DIRECTION_LEFT;
				} else
				if (
					( a->collision_lr_detail == N_NN2_OBJECT_COLLISION_BA_CRASH )
					||
					( a->collision_lr_detail == N_NN2_OBJECT_COLLISION_BA_CHASE )
				)
				{
					  mode_lr = 2;
					target_lr = b->x + b->sx;

					a->direction = N_NN2_DIRECTION_RIGHT;
				}

//if ( a == &p->share->kuina_sprite ) { n_object_collision_debug( "!", a ); }
			}

//mode_ud = 0;
//mode_lr = 0;

			// [!] : clamp
			n_type_gfx wx = p->stage->map_sx;

			if ( target_lr <   0 ) { target_lr += wx; } else
			if ( target_lr >= wx ) { target_lr -= wx; }


			if ( mode_ud == 1 )
			{
				n_posix_loop
				{
					BOOL ret  = n_object_collision_heading_loop( p, a );
					if ( ret )
					{
						break;
					} else {
						if ( a->y <= target_ud ) { break; }
						a->y--;
					}
				}
			} else
			if ( mode_ud == 2 )
			{
				n_posix_loop
				{
					BOOL ret = n_object_collision_gravity_loop( p, a );
					if ( ret )
					{
						break;
					} else {
						if ( a->y >= target_ud ) { break; }
						a->y++;
					}
				}
			} else
			if ( mode_lr == 1 )
			{
				n_posix_loop
				{
					BOOL ret = n_object_collision_wall_lr_loop( p, a );
					if ( ret )
					{
						break;
					} else {
						if ( a->x <= target_lr ) { break; }
						a->x--;
					}
				}
			} else
			if ( mode_lr == 2 )
			{
				n_posix_loop
				{
					BOOL ret = n_object_collision_wall_lr_loop( p, a );
					if ( ret )
					{
						break;
					} else {
						if ( a->x >= target_lr ) { break; }
						a->x++;
					}
				}
			}

			j++;
		}

		i++;
	}


	// [!] : heading will be possible

	i = 0;
	n_posix_loop
	{//break;
		n_sprite *b = t[ i ];
		if ( b == NULL ) { break; }

		if ( b == &p->share->rocks_sprite )
		{
			i++;
			continue;
		}

		BOOL ud = n_chara_object_collision_ud_detail( p, b, YES     );
		BOOL lr = n_chara_object_collision_lr_detail( p, b, YES, NO );

		if ( ( ud )&&( lr ) )
		{
			if (
				( b->collision_ud_detail == N_NN2_OBJECT_COLLISION_OBJECT_NINA_CRASH )
				||
				( b->collision_ud_detail == N_NN2_OBJECT_COLLISION_OBJECT_NINA_CHASE )
			)
			{
//NSLog( @"%d", b->collision_lr_detail );
				if ( b->option == N_SPRITE_OPTION_BIRD )
				{
					n_object_turn( p, b );
				} else {
					b->jump_state = N_NN2_JUMP_STATE_RISE;
					b->jump_float = 0.0;
					b->y = p->stage->nina_y - b->sy;
				}

				return;
			}
		}

		i++;
	}


	i = 0;
	n_posix_loop
	{
		n_sprite *b = t[ i ];
		if ( b == NULL ) { break; }

		if ( b->dokan_onoff )
		{
			i++;
			continue;
		}

		BOOL onoff = FALSE;

		BOOL ud = n_chara_object_collision_ud_detail( p, b, YES     );
		BOOL lr = n_chara_object_collision_lr_detail( p, b, YES, NO );

		if ( ( ud )&&( lr ) )
		{
//n_object_collision_debug( "!", b );

			if (
				( b->collision_lr_detail == N_NN2_OBJECT_COLLISION_OBJECT_NINA_CRASH )
				||
				( b->collision_lr_detail == N_NN2_OBJECT_COLLISION_OBJECT_NINA_CHASE )
				||
				( b->collision_lr_detail == N_NN2_OBJECT_COLLISION_OBJECT_NINA_STILL )
			)
			{
				onoff = TRUE;;

				b->direction = N_NN2_DIRECTION_LEFT;
			} else
			if (
				( b->collision_lr_detail == N_NN2_OBJECT_COLLISION_NINA_OBJECT_CRASH )
				||
				( b->collision_lr_detail == N_NN2_OBJECT_COLLISION_NINA_OBJECT_CHASE )
				||
				( b->collision_lr_detail == N_NN2_OBJECT_COLLISION_NINA_OBJECT_STILL )
			)
			{
				onoff = TRUE;;

				b->direction = N_NN2_DIRECTION_RIGHT;
			} else { break; }
//break;
		}


		if ( onoff )
		{
			if ( b->direction == N_NN2_DIRECTION_LEFT )
			{
//NSLog( @"N_NN2_DIRECTION_LEFT" );
				b->x = p->stage->nina_x + p->nina_margin_fwrd - b->sx;
			} else {
//NSLog( @"N_NN2_DIRECTION_RIGHT" );
				b->x = p->stage->nina_x + p->nina_sx - p->nina_margin_fwrd;
			}
	
			BOOL reflection = FALSE;

			n_posix_loop
			{
				if ( n_object_collision_wall_lr_loop( p, b ) )
				{
					reflection = TRUE;
					if ( b->direction == N_NN2_DIRECTION_LEFT ) { b->x++; } else { b->x--; }
				} else {
					break;
				}
			}

			if ( reflection )
			{
				if ( b->direction == N_NN2_DIRECTION_LEFT )
				{
					p->stage->nina_x = b->x + b->sx - ( b->margin_sx * 2 ) - p->nina_margin_fwrd;
				} else {
					p->stage->nina_x = b->x + b->margin_sx - p->nina_sx + p->nina_margin_fwrd;
				}
			}
		}

		i++;
	}


	n_chara_collision_jam_resolver( p, p->direction );


	return;
}

