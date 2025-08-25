// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




extern void n_object_kuina_interval_change( n_sprite *s );




void
n_object_handheld_off( n_nn2 *p )
{

	p->object_handheld_ready  = FALSE;
	p->object_handheld_onoff  = FALSE;
	p->object_handheld_lock   = TRUE;
	p->object_handheld_sprite = NULL;

	p->turn_direction = p->direction;
	p->dash_onoff     = FALSE;
	p->dash_direction = p->direction;
	p->dash_value     = 0;
	p->nina_walk_step = N_NN2_STEP_INIT;

	p->weather_sprite->invisible = TRUE;


	return;
}

void
n_object_handheld_on( n_nn2 *p, n_sprite *s )
{

	if ( s == NULL ) { return; }

	if ( s->invisible ) { return; }

	if ( s->dokan_onoff ) { return; }

	if ( p->jump_state ) { return; }

	if ( p->object_handheld_lock ) { return; }

	if ( p->object_handheld_ready )
	{
		if ( p->object_handheld_onoff == FALSE )
		{
			p->object_handheld_onoff  = TRUE;
			p->object_handheld_sprite = s;
			s->is_caught              = TRUE;

			if (
				( s == &p->share->kuina_sprite )
				||
				( n_object_birds_is_caught( p ) )
			)
			{
				p->weather_sprite->invisible = FALSE;
			}

			if ( s == &p->share->chick_sprite )
			{
				if ( p->chick_stage_number_phase == 0 )
				{
					p->chick_stage_number_phase   = 1;
					p->chick_stage_number_remover = 0;
				}
			}
		}
	}


	return;
}

n_type_gfx
n_object_handheld_get_x( n_nn2 *p )
{

	n_sprite *s = p->object_handheld_sprite;
	if ( s == NULL ) { return 0; }

	if ( s->invisible ) { return 0; }


	n_type_gfx new_x = 0;

	if ( p->brake_phase )
	{
		if ( p->brake_direction == N_NN2_DIRECTION_LEFT )
		{
			new_x = p->stage->nina_x + p->nina_margin_fwrd - s->sx - 1;
		} else
		if ( p->brake_direction == N_NN2_DIRECTION_RIGHT )
		{
			new_x = p->stage->nina_x + p->nina_sx - p->nina_margin_fwrd + 1;
		}
	} else
	if ( ( p->turn_onoff )&&( p->brake_phase == 0 ) )
	{
		new_x = p->stage->nina_x + p->nina_margin_fwrd - ( s->sx / 2 );
	} else
	if ( p->direction == N_NN2_DIRECTION_LEFT )
	{
		new_x = p->stage->nina_x + p->nina_margin_fwrd - s->sx - 1;
	} else
	if ( p->direction == N_NN2_DIRECTION_RIGHT )
	{
		new_x = p->stage->nina_x + p->nina_sx - p->nina_margin_fwrd + 1;
	} else {
		//
	}


	return new_x;
}

n_type_gfx
n_object_handheld_get_y( n_nn2 *p )
{

	n_sprite *s = p->object_handheld_sprite;
	if ( s == NULL ) { return 0; }

	if ( s->invisible ) { return 0; }


	return p->stage->nina_y + ( p->nina_sy / 2 ) - ( s->sy / 2 );
}

void
n_object_handheld_set( n_nn2 *p )
{

	n_sprite *s = p->object_handheld_sprite;
	if ( s == NULL ) { return; }

	if ( s->invisible ) { return; }


	s->x = n_object_handheld_get_x( p );
	s->y = n_object_handheld_get_y( p );


	return;
}

BOOL
n_object_handheld_go( n_nn2 *p )
{
//return FALSE;

	if ( N_NN2_DEBUG_LOOP ) { n_nn2_debug_hangup_detector( @"n_object_handheld_go()" ); }


	if ( N_NN2_DEBUG_OBJECT_CHARA ) { return FALSE; }


	BOOL ret = FALSE;


	n_sprite *t[ 32 ];

	int n = 0;
	t[ n ] = &p->share->debug_move  ; n++;
	t[ n ] = &p->share->debug_jump  ; n++;
	t[ n ] = &p->share->chick_sprite; n++;
	t[ n ] = &p->share->kuina_sprite; n++;
	t[ n ] = &p->share->bird1_sprite; n++;
	t[ n ] = &p->share->bird2_sprite; n++;
	t[ n ] = &p->share->bird3_sprite; n++;
	t[ n ] = &p->share->rocks_sprite; n++;
	t[ n ] = NULL;


	int i = 0;
	n_posix_loop
	{
		n_sprite *s = t[ i ];
		if ( s == NULL ) { break; }

		BOOL b = n_chara_object_collision_detail( p, s );
//NSLog( @"%d %d", p->object_handheld_skim_ud, p->object_handheld_skim_lr );

		// [x] : conflict : not working : n_object_handheld_on()
		if ( ( p->object_handheld_skim_ud )&&( p->object_handheld_skim_lr ) )
		{
			//b = TRUE;
		}

		if ( b )
		{
			ret = TRUE;

			n_object_handheld_on( p, s );

			break;
		}

		i++;
	}


	return ret;
}

BOOL
n_object_handheld_wall_lr_loop( n_nn2 *p, n_sprite *s )
{
//return 0;

	n_type_gfx fy = s->y;
	n_type_gfx ty = s->y + s->sy;

	fy /= p->mapchip_unit;
	ty /= p->mapchip_unit;


	BOOL ret = FALSE;

	if ( p->direction == N_NN2_DIRECTION_LEFT )
	{

		n_type_gfx lx = s->x - 1;

		lx /= p->mapchip_unit;

		n_posix_loop
		{//break;

			if ( n_chip_detect( p, lx, fy ) )
			{
				ret = TRUE;
				break;
			}

			fy++;
			if ( fy >= ty ) { break; }
		}

	} else
	if ( p->direction == N_NN2_DIRECTION_RIGHT )
	{

		n_type_gfx rx = s->x + s->sx + 0; // [x] : I don't know this is accurate or not

		rx /= p->mapchip_unit;

		n_posix_loop
		{//break;

			if ( n_chip_detect( p, rx, fy ) )
			{
				ret = TRUE;
				break;
			}

			fy++;
			if ( fy >= ty ) { break; }
		}

	}


	return ret;
}

void
n_object_handheld_jam_resolver( n_nn2 *p, BOOL is_released )
{
//NSLog( @"n_interobject_handheld_jam_resolver()" );
//return;

	n_sprite *s = p->object_handheld_sprite;
	if ( s == NULL ) { return; }

	if ( is_released )
	{
		n_object_handheld_set( p );

		if ( s->option & N_SPRITE_OPTION_ROCK )
		{
			if ( p->jump_state == 0 )
			{
				s->y = s->y / p->mapchip_unit * p->mapchip_unit;
			}
		}
	}


	n_posix_loop
	{//break;

		n_type_gfx obj  = s->x;
		n_type_gfx nina = p->stage->nina_x;

		if ( p->direction == N_NN2_DIRECTION_LEFT )
		{
			s->x++;
			p->stage->nina_x++;
		} else
		if ( p->direction == N_NN2_DIRECTION_RIGHT )
		{
			s->x--;
			p->stage->nina_x--;
		} else { break; }

		if ( 0 == n_object_handheld_wall_lr_loop( p, s ) )
		{
			s->x             = obj;
			p->stage->nina_x = nina;

			break;
		}

	}


	return;
}

void
n_object_handheld_release( n_nn2 *p )
{
//return;

	n_sprite *t[ 32 ];

	int n = 0;
	t[ n ] = &p->share->debug_move  ; n++;
	t[ n ] = &p->share->debug_jump  ; n++;
	t[ n ] = &p->share->chick_sprite; n++;
	t[ n ] = &p->share->kuina_sprite; n++;
	t[ n ] = &p->share->bird1_sprite; n++;
	t[ n ] = &p->share->bird2_sprite; n++;
	t[ n ] = &p->share->bird3_sprite; n++;
	t[ n ] = &p->share->rocks_sprite; n++;
	t[ n ] = NULL;


	BOOL onoff = FALSE;

	int i = 0;
	n_posix_loop
	{

		n_sprite *s = t[ i ];
		if ( s == NULL ) { break; }

		if ( s->is_caught )
		{
			onoff = TRUE;

			s->jump_state = N_NN2_JUMP_STATE_FALL;
			s->is_caught  = FALSE;
			s->ox         = 0;
			s->oy         = 0;

			if ( s == &p->share->kuina_sprite )
			{
				s->fall_step = N_OBJ_STEP_FALL;
			}

			n_object_handheld_jam_resolver( p, YES );
		}

		i++;
	}


	if ( onoff )
	{
		n_object_handheld_off( p );
	}


	return;
}

