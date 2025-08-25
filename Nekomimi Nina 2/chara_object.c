// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#define N_NN2_DEBUG_OBJECT_CHARA FALSE
//#define N_NN2_DEBUG_OBJECT_CHARA TRUE




void
n_chara_object_turn( n_nn2 *p, n_sprite *s )
{

	if ( s == NULL ) { return; }

	if ( s->invisible ) { return; }


	if ( p->direction == N_NN2_DIRECTION_LEFT )
	{
		s->direction = N_NN2_DIRECTION_LEFT;
	} else {
		s->direction = N_NN2_DIRECTION_RIGHT;
	}


	return;
}




BOOL
n_chara_object_collision_ud_detail( n_nn2 *p, n_sprite *s, BOOL is_inner )
{
//return FALSE;;

	if ( N_NN2_DEBUG_OBJECT_CHARA ) { return FALSE; }

	if ( p->object_handheld_sprite == s ) { return FALSE; }

	if ( s->invisible ) { return FALSE; }


	n_type_gfx f = s->y;
	n_type_gfx t = s->y + s->sy - s->margin_sy; if ( is_inner ) { t--; }

	f += s->margin_sy;
	t -= s->margin_sy;


	n_type_gfx u = p->stage->nina_y + n_chara_margin_head( p );
	n_type_gfx d = p->stage->nina_y + p->nina_sy - p->nina_margin_foot; if ( is_inner ) { d--; }


	s->collision_ud_detail = N_NN2_OBJECT_COLLISION_NONE;

	BOOL ret_f = ( ( t >= u )&&( t <= d ) );
	BOOL ret_t = ( ( f >= u )&&( f <= d ) );

//if ( s == &p->share->debug_move ) { NSLog( @"%d %d : %d : %d %d", ret_f, ret_t, t, u, d ); }

	if ( ( ret_f )&&( ret_t ) )
	{
		s->collision_ud_detail = N_NN2_OBJECT_COLLISION_OVERLAP;
	} else
	if ( ret_f )
	{
		// [!] : Object : Nina
		if ( s->jump_state == N_NN2_JUMP_STATE_RISE )
		{
			s->collision_ud_detail = N_NN2_OBJECT_COLLISION_OBJECT_NINA_CRASH;
		} else
		if ( s->jump_state == N_NN2_JUMP_STATE_FALL )
		{
			s->collision_ud_detail = N_NN2_OBJECT_COLLISION_OBJECT_NINA_CHASE;
		} else {
			s->collision_ud_detail = N_NN2_OBJECT_COLLISION_OBJECT_NINA_STILL;
		}
	} else
	if ( ret_t )
	{
		// [!] : Nina : Object
		if ( s->jump_state == N_NN2_JUMP_STATE_RISE )
		{
			s->collision_ud_detail = N_NN2_OBJECT_COLLISION_NINA_OBJECT_CRASH;
		} else
		if ( s->jump_state == N_NN2_JUMP_STATE_FALL )
		{
			s->collision_ud_detail = N_NN2_OBJECT_COLLISION_NINA_OBJECT_CHASE;
		} else {
			s->collision_ud_detail = N_NN2_OBJECT_COLLISION_NINA_OBJECT_STILL;
		}
	}


	p->object_handheld_skim_ud = ( ( f >= u )&&( f <= ( d + 8 ) ) );


	return ( 0 != s->collision_ud_detail );
}

// internal
int
n_chara_object_collision_lr_detail_engine( n_nn2 *p, n_sprite *s, n_type_gfx f, n_type_gfx t, n_type_gfx l, n_type_gfx r )
{

	int ret = N_NN2_OBJECT_COLLISION_NONE;

	if ( p->direction == N_NN2_DIRECTION_LEFT )
	{
		if ( ( l >= f )&&( l <= t ) )
		{
			ret = N_NN2_OBJECT_COLLISION_OBJECT_NINA_CRASH; 
		} else
		if ( ( r >= f )&&( r <= t ) )
		{
			ret = N_NN2_OBJECT_COLLISION_NINA_OBJECT_CHASE; 
		}
	} else
	if ( p->direction == N_NN2_DIRECTION_RIGHT )
	{
		if ( ( l >= f )&&( l <= t ) )
		{
			ret = N_NN2_OBJECT_COLLISION_OBJECT_NINA_CHASE; 
		} else
		if ( ( r >= f )&&( r <= t ) )
		{
			ret = N_NN2_OBJECT_COLLISION_NINA_OBJECT_CRASH; 
		}
	}


	return ret;
}

BOOL
n_chara_object_collision_lr_detail( n_nn2 *p, n_sprite *s, BOOL is_inner, BOOL is_outer )
{
//return FALSE;

	if ( N_NN2_DEBUG_OBJECT_CHARA ) { return FALSE; }


	if ( p->object_handheld_sprite == s ) { return FALSE; }


	if ( s->invisible ) { return FALSE; }


	n_type_gfx f = s->x;
	n_type_gfx t = s->x + s->sx - s->margin_sx;

	if ( is_inner ) { t--; }
	if ( is_outer ) { f--; t++; }

	f += s->margin_sx;
	t -= s->margin_sx;


	n_type_gfx l = p->stage->nina_x;
	n_type_gfx r = p->stage->nina_x + p->nina_sx;

	if ( is_inner ) { r--; }
	if ( is_outer ) { l--; r++; }

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

//NSLog( @"%d : Obj %d :%d : Nina %d %d", p->stage->map_sx, f, t, l, r );


	s->collision_lr_detail = n_chara_object_collision_lr_detail_engine( p, s, f, t, l, r );

	if ( p->stage->camera_mode == N_CAMERAWORK_WARP )
	{
		n_type_gfx wx = p->stage->map_sx;

		int r1 = n_chara_object_collision_lr_detail_engine( p, s, f, t, l-wx, r-wx );
		int r2 = n_chara_object_collision_lr_detail_engine( p, s, f, t, l+wx, r+wx );

		if ( ( r1 )||( r2 ) ) { n_chara_object_turn( p, s ); }
	} else
	if ( p->stage->camera_mode == N_CAMERAWORK_LOOP )
	{
		n_type_gfx wx = p->stage->map_sx;

		s->collision_lr_detail |= n_chara_object_collision_lr_detail_engine( p, s, f, t, l-wx, r-wx );
		s->collision_lr_detail |= n_chara_object_collision_lr_detail_engine( p, s, f, t, l+wx, r+wx );
	}

//n_object_collision_debug( "!", s );


	p->object_handheld_skim_lr = ( s->collision_lr_detail != N_NN2_OBJECT_COLLISION_NONE );


	return ( s->collision_lr_detail != N_NN2_OBJECT_COLLISION_NONE );
}

BOOL
n_chara_object_collision_detail( n_nn2 *p, n_sprite *s )
{
//return FALSE;

	if ( N_NN2_DEBUG_OBJECT_CHARA ) { return FALSE; }


	if ( p->object_handheld_sprite == s ) { return FALSE; }


	if ( s == NULL ) { return FALSE; }

	if ( s->invisible ) { return FALSE; }

	if ( s->is_caught ) { return FALSE; }


	BOOL ret = FALSE;

	BOOL ud = n_chara_object_collision_ud_detail( p, s, NO      );// if ( ud == FALSE ) { return FALSE; }
	BOOL lr = n_chara_object_collision_lr_detail( p, s, NO, YES );

//NSLog( @"%d %d", ud, lr );

	if ( ( ud )&&( lr ) )
	{
		ret = TRUE;
	}


	return ret;
}

