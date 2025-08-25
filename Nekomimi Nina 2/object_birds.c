// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#define N_OBJECT_BIRDS_WINK_MAX ( 300 )

#define N_OBJECT_BIRDS_Y        ( 300 )




n_type_gfx
n_object_birds_y( n_nn2 *p )
{

	n_type_gfx ret = N_OBJECT_BIRDS_Y;

	if ( p->stage == &n_nn2_stage_6 )
	{
		ret -= 200;
	}


	return ret;
}




BOOL
n_object_birds_is_caught( n_nn2 *p )
{

	BOOL ret = FALSE;

	if (
		( p->object_handheld_sprite == &p->share->bird1_sprite )
		||
		( p->object_handheld_sprite == &p->share->bird2_sprite )
		||
		( p->object_handheld_sprite == &p->share->bird3_sprite )
	)
	{
		ret = TRUE;
	}


	return ret;
}

#define n_object_bird_reset n_object_birds_reset

void
n_object_birds_reset( n_nn2 *p, n_nn2_stage *s )
{

	BOOL invisible;

	if (
		( s == &n_nn2_stage_3 )
		//||
		//( s == &n_nn2_stage_6 )
		||
		( s == &n_nn2_stage_7 )
	)
	{
		invisible = TRUE;
	} else {
		invisible = FALSE;
	}

	p->share->bird1_sprite.invisible = invisible;
	p->share->bird2_sprite.invisible = invisible;
	p->share->bird3_sprite.invisible = invisible;


	n_type_gfx y = n_object_birds_y( p );

	if ( p->share->bird1_sprite.y > ( y + 100 ) )
	{
		p->share->bird1_sprite.y = ( y + 100 );
	}

	if ( p->share->bird2_sprite.y > ( y + 100 ) )
	{
		p->share->bird2_sprite.y = ( y + 100 );
	}

	if ( p->share->bird3_sprite.y > ( y + 100 ) )
	{
		p->share->bird3_sprite.y = ( y + 100 );
	}


	return;
}

void
n_object_birds_init( n_nn2 *p, n_sprite *s )
{

	n_sprite_zero( s );

	int i = 0;
	n_posix_loop
	{

		n_bmp *bmp[ N_SPRITE_MAX ];
		int     ox[ N_SPRITE_MAX ];
		int     oy[ N_SPRITE_MAX ];

		int interval = 1;

		if ( i == 0 )
		{
			int n = 0;
			bmp[ n ] = &p->bird_body  ; ox[ n ] = 0; oy[ n ] = 0; n++;
			bmp[ n ] = &p->bird_wing_n; ox[ n ] = 0; oy[ n ] = 0; n++;
			bmp[ n ] = NULL;
		} else
		if ( i == 1 )
		{
			int n = 0;
			bmp[ n ] = &p->bird_body  ; ox[ n ] = 0; oy[ n ] = 0; n++;
			bmp[ n ] = &p->bird_wing_1; ox[ n ] = 0; oy[ n ] = 0; n++;
			bmp[ n ] = NULL;
		} else
		if ( i == 2 )
		{
			int n = 0;
			bmp[ n ] = &p->bird_body  ; ox[ n ] = 0; oy[ n ] = 0; n++;
			bmp[ n ] = &p->bird_wing_2; ox[ n ] = 0; oy[ n ] = 0; n++;
			bmp[ n ] = NULL;
		} else
		if ( i == 3 )
		{
			int n = 0;
			bmp[ n ] = &p->bird_body  ; ox[ n ] = 0; oy[ n ] = 0; n++;
			bmp[ n ] = &p->bird_wing_1; ox[ n ] = 0; oy[ n ] = 0; n++;
			bmp[ n ] = NULL;
		} else
		//
		{
			break;
		}

		n_sprite_set( s, i, bmp, ox, oy, 0, interval );

		i++;
	}


	s->option     = N_SPRITE_OPTION_BIRD;

	s->jump_state = N_NN2_JUMP_STATE_FALL;

	s->move_step  = N_OBJ_STEP_MOVE;

	s->is_landed  = TRUE;


	s->sx = N_BMP_SX( s->obj[ 0 ].bmp[ 0 ] );
	s->sy = N_BMP_SY( s->obj[ 0 ].bmp[ 0 ] );

	s->margin_sx = 12 * p->zoom;
	s->margin_sy = 12 / p->zoom;


	if ( 0 == n_game_random( 2 ) )
	{
		s->direction  = N_NN2_DIRECTION_LEFT;
	} else {
		s->direction  = N_NN2_DIRECTION_RIGHT;
	}


	s->wink_count = n_game_random( N_OBJECT_BIRDS_WINK_MAX );


	return;
}

void
n_object_birds_draw( n_nn2 *p, n_sprite *s )
{

	if ( s->invisible ) { return; }


	n_bmp *b = n_nn2_shared_bmp( s );


	n_sprite_single *obj = n_object_get( s );


	if ( s->is_caught ) { s->direction = p->direction; }


	int i = 0;
	n_posix_loop
	{

		if ( obj->bmp[ i ] == NULL ) { break; }

		n_bmp *bmp = obj->bmp[ i ];
		int    ox  = obj-> ox[ i ];
		int    oy  = obj-> oy[ i ];

		n_chara_bmp_copy_grabbable( p, bmp, b, ox,oy, s->direction );

		if ( s->wink_count <= 15 )
		{
			n_chara_bmp_copy_grabbable( p, &p->bird_wink_2, b, ox,oy, s->direction );
		} else {
			n_chara_bmp_copy_grabbable( p, &p->bird_wink_1, b, ox,oy, s->direction );
		}

		i++;
		if ( i >= N_SPRITE_MAX ) { break; }
	}


	s->wink_count++;
	if ( s->wink_count > N_OBJECT_BIRDS_WINK_MAX ) { s->wink_count = 0; }


	n_type_gfx x = s->x + s->ox - n_camerawork_x( p );
	n_type_gfx y = s->y + s->oy - n_camerawork_y( p );

	if ( s->is_caught )
	{
		if  ( p->bounce_sprite_m != NULL ) { y += N_NN2_BRICK_MOVE_MAX; }
	}

	n_nn2_bmp_draw( p, b, x, y );


	return;
}

void
n_object_birds_draw_overlay( n_nn2 *p, n_sprite *s )
{

	if ( s->invisible ) { return; }


	if ( s->is_caught == FALSE ) { return; }


	n_bmp *b = n_nn2_shared_bmp( s );


	n_sprite_single *obj = n_object_get( s );


	s->direction = p->direction;


	int i = 0;
	n_posix_loop
	{

		if ( obj->bmp[ i ] == NULL ) { break; }

		n_bmp *bmp = obj->bmp[ i ];
		int    ox  = obj-> ox[ i ];
		int    oy  = obj-> oy[ i ];

		if ( ( bmp == &p->bird_wing_1 )||( bmp == &p->bird_wing_2 ) )
		{
			n_chara_bmp_copy_grabbable( p, bmp, b, ox,oy, s->direction );
		}

		i++;
		if ( i >= N_SPRITE_MAX ) { break; }
	}


	n_type_gfx x = s->x + s->ox - n_camerawork_x( p );
	n_type_gfx y = s->y + s->oy - n_camerawork_y( p );

	if ( s->is_caught )
	{
		if  ( p->bounce_sprite_m != NULL ) { y += N_NN2_BRICK_MOVE_MAX; }
	}

	n_nn2_bmp_draw( p, b, x, y );


	return;
}

void
n_object_birds_action( n_nn2 *p, n_sprite *s )
{

	if ( s->invisible ) { return; }


	if ( s->y > ( n_object_birds_y( p ) + 100 ) )
	{
		s->y -= 4;
	} else
	if ( s->y < ( n_object_birds_y( p ) - 100 ) )
	{
		s->y += 4;
	} else {
		s->y += 1 - n_game_random( 3 );
	}


	//n_object_collision_gravity( p, s );

	extern BOOL n_interobject_collision_birds( n_nn2 *p, n_sprite *s );
	n_interobject_collision_birds( p, s );

	n_object_collision_lr( p, s );


	return;
}

