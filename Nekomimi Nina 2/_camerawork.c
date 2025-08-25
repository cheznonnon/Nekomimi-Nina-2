// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#define N_NN2_CAMERA_STEP       ( 8 * p->zoom )

#define N_NN2_CAMERA_PHASE_NONE ( 0 )
#define N_NN2_CAMERA_PHASE_MOVE ( 1 )
#define N_NN2_CAMERA_PHASE_FREE ( 2 )
#define N_NN2_CAMERA_PHASE_BACK ( 3 )


#define N_NN2_CAMERA_OCEAN      ( p->mapchip_unit * 12 )

#define N_NN2_CAMERA_PHASE_NONE_UPPER ( 0 )
#define N_NN2_CAMERA_PHASE_MOVE_UPPER ( 1 )
#define N_NN2_CAMERA_PHASE_FREE_UPPER ( 2 )
#define N_NN2_CAMERA_PHASE_BACK_UPPER ( 3 )

#define N_NN2_CAMERA_PHASE_NONE_LOWER ( 0 )
#define N_NN2_CAMERA_PHASE_MOVE_LOWER ( 5 )
#define N_NN2_CAMERA_PHASE_FREE_LOWER ( 6 )
#define N_NN2_CAMERA_PHASE_BACK_LOWER ( 7 )




void
n_camerwork_clamp( n_nn2 *p, n_type_gfx *x, n_type_gfx *y )
{

	if (
		( p->stage->camera_mode == N_CAMERAWORK_STOP )
		||
		( p->stage->camera_mode == N_CAMERAWORK_WARP )
	)
	{
		if ( (*x) < 0 ) { (*x) = 0; }
		if ( (*y) < 0 ) { (*y) = 0; }

		if ( (*x) >= p->stage->map_max_sx ) { (*x) = p->stage->map_max_sx; }
		if ( (*y) >= p->stage->map_max_sy ) { (*y) = p->stage->map_max_sy; }
	} else
	if ( p->stage->camera_mode == N_CAMERAWORK_LOOP )
	{
		if ( (*y) <                     0 ) { (*y) =                    0; }
		if ( (*y) >= p->stage->map_max_sy ) { (*y) = p->stage->map_max_sy; }
	}


	return;
}

n_type_gfx
n_camerawork_x( n_nn2 *p )
{
	return p->stage->camera_x;
}

n_type_gfx
n_camerawork_y( n_nn2 *p )
{
	return p->stage->camera_y;
}

void
n_camerawork_autofocus_normal( n_nn2 *p )
{
//return;

//NSLog( @"%d %d", p->nina_y, p->map_max_sy );

	if ( p->stage->camera_phase == N_NN2_CAMERA_PHASE_NONE )
	{
		p->stage->camera_y = p->stage->map_max_sy;

		if ( p->stage->camera_first )
		{
			if ( p->stage->nina_y < p->stage->map_max_sy )
			{
				p->stage->camera_phase = N_NN2_CAMERA_PHASE_MOVE;
				p->stage->camera_step  = 0;
			}
		}
	} else
	if ( p->stage->camera_phase == N_NN2_CAMERA_PHASE_MOVE )
	{
		n_type_gfx prv_step = p->stage->camera_step;

		p->stage->camera_step += N_NN2_CAMERA_STEP;

		n_type_gfx cy = p->stage->map_max_sy - p->stage->camera_step;
		if ( cy <= p->stage->camera_y )
		{
			p->stage->camera_step = prv_step;

			cy = p->stage->camera_y;
			p->stage->camera_phase = N_NN2_CAMERA_PHASE_FREE;
		}

		p->stage->camera_y = cy;
	} else
	if ( p->stage->camera_phase == N_NN2_CAMERA_PHASE_FREE )
	{
		if ( p->stage->camera_first )
		{
			if ( p->stage->nina_y > ( p->stage->map_max_sy + ( p->sy / 3 ) ) )
			{
				p->stage->camera_phase = N_NN2_CAMERA_PHASE_BACK;
				p->stage->camera_step  = 0;
			}
		}
	} else
	if ( p->stage->camera_phase == N_NN2_CAMERA_PHASE_BACK )
	{
		n_type_gfx prv_step = p->stage->camera_step;

		p->stage->camera_step += N_NN2_CAMERA_STEP;

		n_type_gfx cy = p->stage->camera_y + p->stage->camera_step;
		if ( cy > p->stage->map_max_sy )
		{
			p->stage->camera_step = prv_step;

			cy = p->stage->map_max_sy;
			p->stage->camera_phase = N_NN2_CAMERA_PHASE_NONE;
		}

		p->stage->camera_y = cy;
	}

//NSLog( @"%d", p->camera_phase );


	return;
}

void
n_camerawork_autofocus_ocean_lower( n_nn2 *p )
{
//return;

	n_type_gfx base = N_NN2_CAMERA_OCEAN;
	n_type_gfx step = N_NN2_CAMERA_STEP * 2;


	if ( p->stage->camera_phase == N_NN2_CAMERA_PHASE_NONE_LOWER )
	{
		if ( p->stage->nina_y > ( p->mapchip_unit * 32 ) )
		{
			p->stage->camera_phase = N_NN2_CAMERA_PHASE_MOVE_LOWER;
		}
	} else
	if ( p->stage->camera_phase == N_NN2_CAMERA_PHASE_MOVE_LOWER )
	{
		p->stage->camera_y += step;
		if ( p->stage->camera_y >= p->stage->map_max_sy )
		{
			p->stage->camera_y = p->stage->map_max_sy;
			p->stage->camera_phase = N_NN2_CAMERA_PHASE_FREE_LOWER;
		}

		p->stage->camera_wave_offset += step;
	} else
	if ( p->stage->camera_phase == N_NN2_CAMERA_PHASE_FREE_LOWER )
	{
		p->stage->camera_y = p->stage->map_max_sy;

		if ( p->stage->nina_y < p->stage->map_max_sy )
		{
			p->stage->camera_phase = N_NN2_CAMERA_PHASE_BACK_LOWER;
		}
	} else
	if ( p->stage->camera_phase == N_NN2_CAMERA_PHASE_BACK_LOWER )
	{
		p->stage->camera_y -= step;
		if ( p->stage->camera_y <= base )
		{
			p->stage->camera_y = base;
			p->stage->camera_phase = N_NN2_CAMERA_PHASE_NONE_LOWER;
		}

		p->stage->camera_wave_offset -= step;
	}


	return;
}

void
n_camerawork_autofocus_ocean_upper( n_nn2 *p )
{
//return;

	if ( p->stage->camera_wave_under == FALSE ) { return; }


	n_type_gfx base = N_NN2_CAMERA_OCEAN;
	n_type_gfx step = N_NN2_CAMERA_STEP * 2;


	if ( p->stage->camera_phase == N_NN2_CAMERA_PHASE_NONE_UPPER )
	{
		if ( p->stage->nina_y < base )
		{
			p->stage->camera_phase = N_NN2_CAMERA_PHASE_MOVE_UPPER;
		}
	} else
	if ( p->stage->camera_phase == N_NN2_CAMERA_PHASE_MOVE_UPPER )
	{
		p->stage->camera_y -= step;
		if ( p->stage->camera_y < 0 )
		{
			p->stage->camera_y = 0;
			p->stage->camera_phase = N_NN2_CAMERA_PHASE_FREE_UPPER;
		}

		p->stage->camera_wave_offset -= step;
	} else
	if ( p->stage->camera_phase == N_NN2_CAMERA_PHASE_FREE_UPPER )
	{
		p->stage->camera_y = 0;

		if ( p->stage->nina_y > base )
		{
			p->stage->camera_phase = N_NN2_CAMERA_PHASE_BACK_UPPER;
		}
	} else
	if ( p->stage->camera_phase == N_NN2_CAMERA_PHASE_BACK_UPPER )
	{
		p->stage->camera_y += step;
		if ( p->stage->camera_y > base )
		{
			p->stage->camera_y = base;
			p->stage->camera_phase = N_NN2_CAMERA_PHASE_NONE_UPPER;
		}

		p->stage->camera_wave_offset += step;
	}


	return;
}

void
n_camerawork_go( n_nn2 *p )
{
//return;

	if ( p->stage->camera_lock ) { return; }

/*
	n_type_gfx step = 8;

	if ( n_mac_keystate_get( N_MAC_KEYCODE_U ) )
	{
		p->stage->camera_y -= step;
	} else
	if ( n_mac_keystate_get( N_MAC_KEYCODE_N ) )
	{
		p->stage->camera_y += step;
	} else
	if ( n_mac_keystate_get( N_MAC_KEYCODE_H ) )
	{
		p->stage->camera_x -= step;
	} else
	if ( n_mac_keystate_get( N_MAC_KEYCODE_J ) )
	{
		p->stage->camera_x += step;
	}

	n_camerwork_clamp( p, &p->stage->camera_x, &p->stage->camera_y );

	return;
*/

	if ( p->wave_onoff )
	{
		if ( p->stage->camera_reset )
		{
			p->stage->camera_reset       = FALSE;
			p->stage->camera_y           = N_NN2_CAMERA_OCEAN;
			p->stage->camera_wave_offset = 0;
			p->stage->camera_wave_under  = FALSE;
		}

		p->stage->camera_x = p->stage->nina_x - p->stage->center_x;
		//p->stage->camera_y = p->stage->nina_y - p->stage->center_y;

		n_camerwork_clamp( p, &p->stage->camera_x, &p->stage->camera_y );

		n_camerawork_autofocus_ocean_upper( p );
		n_camerawork_autofocus_ocean_lower( p );
	} else {
		p->stage->camera_x = p->stage->nina_x - p->stage->center_x;
		p->stage->camera_y = p->stage->nina_y - p->stage->center_y;

		n_camerwork_clamp( p, &p->stage->camera_x, &p->stage->camera_y );

		n_camerawork_autofocus_normal( p );
	}


	return;
}

void
n_camerawork_offset_get( n_nn2 *p, int *ox, int *oy )
{

	(*ox) = p->stage->nina_x - p->stage->camera_x;
	(*oy) = p->stage->nina_y - p->stage->camera_y;


	return;
}
