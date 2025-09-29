// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#define N_NN2_BRICK_MOVE_NONE   ( 0 )
#define N_NN2_BRICK_MOVE_TYPE_U ( 1 )
#define N_NN2_BRICK_MOVE_TYPE_D ( 2 )




BOOL
n_brick_move_animation( n_nn2 *p, n_type_gfx tx, n_type_gfx ty )
{

	if ( p->weather_transition ) { return FALSE; }


	extern n_sprite* n_chip_get_auto( n_nn2 *p, n_type_gfx x, n_type_gfx y );

	n_sprite *s = n_chip_get_auto( p, tx, ty );
	if ( s == NULL ) { return FALSE; }

	if ( s->brick_move_type == N_NN2_BRICK_MOVE_TYPE_U )
	{
		// [!] : heading

		if ( s->brick_move_phase == 1 )
		{
			s->override_y -= s->brick_move_step;

			if ( s->override_y < -s->brick_move_max )
			{
//NSLog( @"phase 1 : %d", p->weather_transition_ready );

				s->brick_move_phase = 2;

				return TRUE;
			}
		} else
		if ( s->brick_move_phase == 2 )
		{
			s->override_y += s->brick_move_step;

			if ( s->override_y > 0 )
			{
//NSLog( @"phase 2 : %d", p->weather_transition_ready );

				s->override_y       = 0;
				s->brick_move_phase = 0;

				p->brick_move_onoff = FALSE;

				p->weather_transition_ready = FALSE;
			}
		}
	} else
	if ( s->brick_move_type == N_NN2_BRICK_MOVE_TYPE_D )
	{
		// [!] : hipdrop

		if ( s->brick_move_phase == 1 )
		{
			s->override_y += s->brick_move_step;

			p->stage->nina_y += s->brick_move_step;

			if ( s->override_y >= s->brick_move_max )
			{
				s->brick_move_phase = 2;
			}
		} else
		if ( s->brick_move_phase == 2 )
		{
			s->override_y -= s->brick_move_step;

			p->stage->nina_y -= s->brick_move_step;

			if ( s->override_y <= 0 )
			{
				s->brick_move_phase = 0;
			}
		}
	}// else


	return FALSE;
}

void
n_brick_move_action_on( n_nn2 *p, n_type_gfx tx, n_type_gfx ty, BOOL is_heading )
{
//NSLog( @"n_brick_move_action_on()" );

	extern n_sprite* n_chip_get_auto( n_nn2 *p, n_type_gfx x, n_type_gfx y );

	n_sprite *s = n_chip_get_auto( p, tx, ty );
	if ( s == NULL ) { return; }

	if ( s->brick_move_phase == 0 )
	{
		p->brick_move_onoff = TRUE;

		s->brick_move_phase = 1;
		s->brick_move_step  = N_NN2_BRICK_MOVE_STEP;
		s->brick_move_max   = N_NN2_BRICK_MOVE_MAX;

		if ( is_heading )
		{
			s->brick_move_type = N_NN2_BRICK_MOVE_TYPE_U;
		} else {
			s->brick_move_type = N_NN2_BRICK_MOVE_TYPE_D;
		}
	}


	return;
}

