// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




extern void n_nn2_stage_stand_register( n_nn2 *p, n_sprite *s, BOOL reg_or_unreg );




// internal
BOOL
n_interobject_collision_ud_detail( n_nn2 *p, n_sprite *obj_a, n_sprite *obj_b, BOOL is_inner )
{
//return TRUE;


	if ( obj_a == obj_b ) { return FALSE; }


	if ( obj_a->is_landed == FALSE ) { return FALSE; }
	if ( obj_b->is_landed == FALSE ) { return FALSE; }


	if ( obj_a->invisible ) { return FALSE; }
	if ( obj_b->invisible ) { return FALSE; }


	n_type_gfx a_f, a_t;
	n_type_gfx b_f, b_t;

	a_f = obj_a->y;
	a_t = obj_a->y + obj_a->sy; if ( is_inner ) { a_t--; }

	a_f += obj_a->margin_sy;
	a_t -= obj_a->margin_sy;


	b_f = obj_b->y;
	b_t = obj_b->y + obj_b->sy; if ( is_inner ) { b_t--; }

	b_f += obj_b->margin_sy;
	b_t -= obj_b->margin_sy;


	obj_a->collision_ud_detail = N_NN2_OBJECT_COLLISION_NONE;

	if ( ( a_f >= b_f )&&( a_f <= b_t )&&( a_t >= b_f )&&( a_t <= b_t ) )
	{
		obj_a->collision_ud_detail = N_NN2_OBJECT_COLLISION_OVERLAP;
		//obj_a->collision_ud_detail = N_NN2_OBJECT_COLLISION_BA_CRASH;
	} else
	if ( ( a_f >= b_f )&&( a_f <= b_t ) )
	{
		// [!] : Object B (Upper)
		// [!] : Object A (Lower)

		obj_a->collision_ud_detail = N_NN2_OBJECT_COLLISION_BA_CRASH;
	} else
	if ( ( a_t >= b_f )&&( a_t <= b_t ) )
	{
		// [!] : Object A (Upper)
		// [!] : Object B (Lower)

		obj_a->collision_ud_detail = N_NN2_OBJECT_COLLISION_AB_CRASH;
	}


	obj_a->collision_lift_onoff = FALSE;

	if ( ( a_t >= b_f )&&( a_t <= ( b_f + 24 ) ) )
	{
		// [!] : A Object (Upper)
		// [!] : B Cloud  (Lower)

		obj_a->collision_lift_onoff = TRUE;
	}


	return ( obj_a->collision_ud_detail != N_NN2_OBJECT_COLLISION_NONE );
}

// internal
void
n_interobject_collision_lr_detail_engine( n_nn2 *p, n_sprite *obj_a, n_sprite *obj_b, n_type_gfx a_f, n_type_gfx a_t, n_type_gfx b_f, n_type_gfx b_t )
{

	if ( ( a_f >= b_f )&&( a_f <= b_t )&&( a_t >= b_f )&&( a_t <= b_t ) )
	{
		obj_a->collision_lr_detail = N_NN2_OBJECT_COLLISION_OVERLAP;
		//obj_a->collision_lr_detail = N_NN2_OBJECT_COLLISION_BA_CRASH; 
	} else
	if ( ( a_f >= b_f )&&( a_f <= b_t ) )
	{
		// [!] : Object B (L) : Object A (R)

		if ( obj_a->direction == obj_b->direction )
		{
			obj_a->collision_lr_detail = N_NN2_OBJECT_COLLISION_BA_CHASE; 
		} else {
			obj_a->collision_lr_detail = N_NN2_OBJECT_COLLISION_BA_CRASH; 
		}
	} else
	if ( ( a_t >= b_f )&&( a_t <= b_t ) )
	{
		// [!] : Object A (L) : Object B (R)
	
		if ( obj_a->direction == obj_b->direction )
		{
			obj_a->collision_lr_detail = N_NN2_OBJECT_COLLISION_AB_CHASE; 
		} else {
			obj_a->collision_lr_detail = N_NN2_OBJECT_COLLISION_AB_CRASH; 
		}
	}


	return;
}

// internal
BOOL
n_interobject_collision_lr_detail( n_nn2 *p, n_sprite *obj_a, n_sprite *obj_b, BOOL is_inner )
{
//return TRUE;


	if ( obj_a == obj_b ) { return FALSE; }


	if ( obj_a->is_landed == FALSE ) { return FALSE; }
	if ( obj_b->is_landed == FALSE ) { return FALSE; }


	if ( obj_a->invisible ) { return FALSE; }
	if ( obj_b->invisible ) { return FALSE; }


//if ( obj_a->option == N_SPRITE_OPTION_ROCK ) { NSLog( @"n_interobject_collision_lr_detail()" ); }


	n_type_gfx a_f, a_t;
	n_type_gfx b_f, b_t;

	a_f = obj_a->x;
	a_t = obj_a->x + obj_a->sx; if ( is_inner ) { a_t--; }

	a_f += obj_a->margin_sx;
	a_t -= obj_a->margin_sx;

	b_f = obj_b->x;
	b_t = obj_b->x + obj_b->sx; if ( is_inner ) { b_t--; }

	b_f += obj_b->margin_sx;
	b_t -= obj_b->margin_sx;

	obj_a->collision_lr_detail = N_NN2_OBJECT_COLLISION_NONE;

	n_interobject_collision_lr_detail_engine( p, obj_a, obj_b, a_f, a_t, b_f, b_t );

	if (
		( p->stage->camera_mode == N_CAMERAWORK_WARP )
		||
		( p->stage->camera_mode == N_CAMERAWORK_LOOP )
	)
	{
		n_type_gfx wx = p->stage->map_sx;

		n_interobject_collision_lr_detail_engine( p, obj_a, obj_b, a_f, a_t, b_f-wx, b_t-wx );
		n_interobject_collision_lr_detail_engine( p, obj_a, obj_b, a_f, a_t, b_f+wx, b_t+wx );
	}


	return ( obj_a->collision_lr_detail != N_NN2_OBJECT_COLLISION_NONE );
}

// internal
BOOL
n_interobject_collision_detail( n_nn2 *p, n_sprite *a, n_sprite *b )
{
//return FALSE;


	BOOL ret = FALSE;


	if ( a == b ) { return ret; }

	if ( a->invisible ) { return ret; }
	if ( b->invisible ) { return ret; }

	if ( p->object_handheld_sprite == a ) { return ret; }
	if ( p->object_handheld_sprite == b ) { return ret; }


	BOOL ud = n_interobject_collision_ud_detail( p, a, b, NO );
	BOOL lr = n_interobject_collision_lr_detail( p, a, b, NO );

	if ( ( ud )&&( lr ) )
	{
//if ( a == &p->share->debug_move ) { n_object_collision_debug( "!", a ); }

		ret = TRUE;
	}

//NSLog( @"%d %d : %d", ud, lr, ret );


	return ret;
}

BOOL
n_interobject_collision_lift_condition( n_nn2 *p )
{
//return FALSE;

	n_sprite *t[ 32 ];

	int n = 0;

	if ( p->stage == &n_nn2_stage_0 )
	{
		t[ n ] = &p->stage->cld_h_0_sprite; n++;
		t[ n ] = &p->stage->cld_v_0_sprite; n++;
	} else
	if ( p->stage == &n_nn2_stage_4 )
	{
		t[ n ] = &p->stage->cld_h_0_sprite; n++;
		t[ n ] = &p->stage->cld_h_1_sprite; n++;
		t[ n ] = &p->stage->cld_v_0_sprite; n++;
		t[ n ] = &p->stage->cld_v_1_sprite; n++;
		//t[ n ] = &p->stage->cld_v_2_sprite; n++;
		//t[ n ] = &p->stage->cld_v_3_sprite; n++;
	} else
	//
	{
		return FALSE;
	}

	t[ n ] = NULL;



	BOOL ret = FALSE;


	int i = 0;
	n_posix_loop
	{

		n_sprite *u = t[ i ];
		if ( u == NULL ) { break; }

		if ( u->sprite_lift != NULL )
		{
			ret = TRUE;
			break;
		}

		i++;
	}


	return ret;
}

BOOL
n_interobject_collision_lift( n_nn2 *p, n_sprite *s )
{
//return FALSE;

	n_sprite *t[ 32 ];

	int n = 0;

	if ( p->stage == &n_nn2_stage_0 )
	{
		t[ n ] = &p->stage->cld_h_0_sprite; n++;
		t[ n ] = &p->stage->cld_v_0_sprite; n++;
	} else
	if ( p->stage == &n_nn2_stage_4 )
	{
		t[ n ] = &p->stage->cld_h_0_sprite; n++;
		t[ n ] = &p->stage->cld_h_1_sprite; n++;
		t[ n ] = &p->stage->cld_v_0_sprite; n++;
		t[ n ] = &p->stage->cld_v_1_sprite; n++;
		//t[ n ] = &p->stage->cld_v_2_sprite; n++;
		//t[ n ] = &p->stage->cld_v_3_sprite; n++;
	} else
	//
	{
		return FALSE;
	}

	t[ n ] = NULL;



	BOOL ret = FALSE;


	int i = 0;
	n_posix_loop
	{

		n_sprite *u = t[ i ];
		if ( u == NULL ) { break; }

		n_sprite *a = s;
		n_sprite *b = u;

		int ret = n_interobject_collision_detail( p, a, b );
		if ( ( ret )&&( s->collision_lift_onoff ) )
		{
			if ( FALSE == n_interobject_collision_lift_condition( p ) )
			{
				ret = TRUE;
				u->sprite_lift = s;
			}

			break;
		} else {
			u->sprite_lift = NULL;
		}

		i++;
	}


	return ret;
}

void
n_interobject_collision_lift_clamp( n_nn2 *p, n_sprite *s )
{
//return;

	n_sprite *t[ 32 ];

	int n = 0;

	if ( p->stage == &n_nn2_stage_0 )
	{
		t[ n ] = &p->stage->cld_h_0_sprite; n++;
		t[ n ] = &p->stage->cld_v_0_sprite; n++;
	} else
	if ( p->stage == &n_nn2_stage_4 )
	{
		t[ n ] = &p->stage->cld_h_0_sprite; n++;
		t[ n ] = &p->stage->cld_h_1_sprite; n++;
		t[ n ] = &p->stage->cld_v_0_sprite; n++;
		t[ n ] = &p->stage->cld_v_1_sprite; n++;
		//t[ n ] = &p->stage->cld_v_2_sprite; n++;
		//t[ n ] = &p->stage->cld_v_3_sprite; n++;
	} else
	//
	{
		return;
	}

	t[ n ] = NULL;


	extern BOOL n_nn2_stage_stand_onoff_rock( n_nn2 *p );

	int i = 0;
	n_posix_loop
	{

		n_sprite *u = t[ i ];
		if ( u == NULL ) { break; }

		if ( u->sprite_lift != NULL )
		{
			s->y = ( u->y + u->margin_sy ) - s->sy;
			if ( n_nn2_stage_stand_onoff_rock( p ) )
			{
				p->stage->nina_y = s->y - p->nina_sy + p->nina_margin_foot - 1;
			}
			break;
		}

		i++;
	}


	return;
}

BOOL
n_interobject_collision_birds( n_nn2 *p, n_sprite *s )
{
//return FALSE;

	n_sprite *t[ 32 ];

	int n = 0;
	t[ n ] = &p->share->bird1_sprite; n++;
	t[ n ] = &p->share->bird2_sprite; n++;
	t[ n ] = &p->share->bird3_sprite; n++;
	t[ n ] = NULL;


	BOOL ret = FALSE;


	int i = 0;
	n_posix_loop
	{

		n_sprite *u = t[ i ];
		if ( u == NULL ) { break; }

		n_sprite *a = s;
		n_sprite *b = u;

		if ( n_interobject_collision_detail( p, a, b ) )
		{
			if ( a->direction == N_NN2_DIRECTION_LEFT )
			{
				a->direction = N_NN2_DIRECTION_RIGHT;
				b->direction = N_NN2_DIRECTION_LEFT;
			} else {
				a->direction = N_NN2_DIRECTION_LEFT;
				b->direction = N_NN2_DIRECTION_RIGHT;
			}
			break;
		}

		i++;
	}


	return ret;
}




n_sprite*
n_interobject_collision_gravity_lift( n_nn2 *p )
{
//return FALSE;

	n_sprite *t[ 32 ];

	int n = 0;

	if ( p->stage == &n_nn2_stage_0 )
	{
		t[ n ] = &p->stage->cld_h_0_sprite; n++;
		t[ n ] = &p->stage->cld_v_0_sprite; n++;
	} else
	if ( p->stage == &n_nn2_stage_4 )
	{
		t[ n ] = &p->stage->cld_h_0_sprite; n++;
		t[ n ] = &p->stage->cld_h_1_sprite; n++;
		t[ n ] = &p->stage->cld_v_0_sprite; n++;
		t[ n ] = &p->stage->cld_v_1_sprite; n++;
		//t[ n ] = &p->stage->cld_v_2_sprite; n++;
		//t[ n ] = &p->stage->cld_v_3_sprite; n++;
	}

	t[ n ] = &p->share->debug_move  ; n++;
	t[ n ] = &p->share->debug_jump  ; n++;
	t[ n ] = &p->share->chick_sprite; n++;
	t[ n ] = &p->share->kuina_sprite; n++;
	t[ n ] = &p->share->rocks_sprite; n++;
	t[ n ] = NULL;


	n_sprite *ret = NULL;


	int i = 0;
	n_posix_loop
	{

		n_sprite *s = t[ i ];
		if ( s == NULL ) { break; }

		if ( n_object_lift_collision_is_hit( p, s, YES ) )
		{
//if ( s == &p->stage->rocks_sprite ) { NSLog( @"On" ); }
			ret = s;
			n_nn2_stage_stand_register( p, s, YES );
		} else {
//if ( s == &p->stage->rocks_sprite ) { NSLog( @"Off" ); }
			n_nn2_stage_stand_register( p, s,  NO );
		}

		i++;
	}


	return ret;
}

