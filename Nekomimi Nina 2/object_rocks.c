// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




extern BOOL n_interobject_collision_lift( n_nn2 *p, n_sprite *s );




BOOL
n_object_rocks_action_is_hit( n_nn2 *p, n_sprite *s )
{
//return FALSE;

	if ( s->invisible ) { return FALSE; }

	if ( p->object_handheld_sprite == s ) { return FALSE; }


	BOOL ud = n_chara_object_collision_ud_detail( p, s, YES );
	BOOL lr = n_chara_object_collision_lr_detail( p, s, YES, YES );
//NSLog( @"UD %d : LR %d", ud, lr );


	return ( ( ud ) && ( lr ) );
}

BOOL
n_object_rocks_action_mutex( n_nn2 *p )
{
//return FALSE;

	// [!] : stop when left/right side is touched

	BOOL ret = FALSE;

	if ( n_object_rocks_action_is_hit( p, &p->share->rocks_sprite ) )
	{
		if ( n_nn2_stage_stand_onoff( p ) )
		{
			if (
				( p->stage->cld_h_0_sprite.sprite_stand != NULL )
				||
				( p->stage->cld_h_1_sprite.sprite_stand != NULL )
				||
				( p->stage->cld_v_0_sprite.sprite_stand != NULL )
				||
				( p->stage->cld_v_1_sprite.sprite_stand != NULL )
				//||
				//( p->stage->cld_v_2_sprite.sprite_stand != NULL )
				//||
				//( p->stage->cld_v_3_sprite.sprite_stand != NULL )
			)
			{
				ret = TRUE;
			}
		} else {
			ret = TRUE;
		}
	}


	return ret;
}

void
n_object_rocks_action( n_nn2 *p, n_sprite *s )
{

	if ( s->invisible ) { return; }


	if ( n_interobject_collision_lift( p, s ) )
	{
		//
	} else
	if ( n_object_collision_gravity( p, s ) )
	{
		s->jump_state = N_NN2_JUMP_STATE_NONE;
		s->is_landed  = TRUE;
	}


	return;
}

void
n_object_rocks_draw( n_nn2 *p, n_sprite *s )
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

		n_chara_bmp_copy_all_grabbable( p, bmp, b, ox,oy, N_NN2_DIRECTION_LEFT, NO, spr );

		i++;
		if ( i >= N_SPRITE_MAX ) { break; }
	}


	n_type_gfx x = s->x + s->ox - n_camerawork_x( p );
	n_type_gfx y = s->y + s->oy - n_camerawork_y( p );

	n_nn2_bmp_draw_simple( p, b, x, y );

	if ( p->stage->camera_mode == N_CAMERAWORK_WARP )
	{
		n_nn2_bmp_draw_simple( p, b, x + p->stage->map_sx, y );
		n_nn2_bmp_draw_simple( p, b, x - p->stage->map_sx, y );
	}


	return;
}

