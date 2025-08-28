// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// [!] : Copilot Optimization Done




static void
n_nn2_wave_draw_vertical_line_fast( void *canvas, int sx, int sy, n_type_gfx x, n_type_gfx y, u32 blend_color )
{
	if ( ( x < 0 ) || ( x >= sx ) ) { return; }

	/* draw first pixel as white (same behavior as original) */
	n_bmp_ptr_set( canvas, x, y, n_bmp_white );

	while ( 1 )
	{
		y++;
		if ( y >= sy ) { break; }

		u32 color;
		n_bmp_ptr_get( canvas, x, y, &color );
		color = n_bmp_blend_pixel( color, blend_color, 0.2 );
		n_bmp_ptr_set( canvas, x, y, color );
	}

	return;
}

void
n_nn2_wave_draw( n_nn2 *p )
{
//return;

	if ( p->wave_onoff == FALSE ) { return; }

	/* [!] : trochoidal wave : thanx DeepSeek */

	const n_type_real pi2       = M_PI * 2.0;
	const n_type_real amp_a     = 12;
	const n_type_real amp_b     =  8;
	const n_type_real length    = amp_a * 5;
	const n_type_real threshold = amp_a * 3;

/*
	if ( n_mac_keystate_get( N_MAC_KEYCODE_U ) )
	{
		p->stage->camera_wave_offset -= 16;
	} else
	if ( n_mac_keystate_get( N_MAC_KEYCODE_N ) )
	{
		p->stage->camera_wave_offset += 16;
	}
*/
//NSLog( @"%d", p->stage->camera_wave_offset );

	if ( p->stage->camera_wave_offset >= threshold )
	{
		n_bmp_mixer( p->canvas, 0,0,p->sx,p->sy, n_bmp_rgb_mac( 0,0,255 ), 0.2 );
		return;
	}

	/* cache frequently used fields */
	int sx = p->sx;
	int sy = p->sy;
	void *canvas = p->canvas;
	int cam_off = p->stage->camera_wave_offset;

	n_type_gfx  prev = 0;

	/* reuse a static line buffer to avoid per-frame alloc/free */
	static int *line = NULL;
	static int  line_size = 0;
	if ( line_size < sx )
	{
		if ( line ) { n_memory_free( line ); }
		int bytes = sx * sizeof(int);
		line = n_memory_new( bytes );
		line_size = sx;
	}
	n_memory_zero( line, line_size * sizeof(int) );

	static n_type_real updown_v = 1.0;
	static BOOL        updown   = FALSE;
	static n_type_real phi = 0;

	/* precompute trig rotation for phi and incremental delta per x */
	const n_type_real delta = pi2 / length;
	n_type_real sin_phi = sin( phi );
	n_type_real cos_phi = cos( phi );

	/* incremental sin/cos for th starting at 0 */
	n_type_real sin_th = 0.0;
	n_type_real cos_th = 1.0;
	n_type_real cos_dt = cos( delta );
	n_type_real sin_dt = sin( delta );

	/* accumulative amp_a * th (so we don't multiply by th every time) */
	n_type_real accum_th_amp = 0.0;

	n_type_gfx x = 0;
	while ( 1 )
	{
		/* th = delta * x (represented inside accum_th_amp)
		   compute sin(th+phi) and cos(th+phi) via rotation by phi */
		n_type_real sin_thp = sin_th * cos_phi + cos_th * sin_phi;
		n_type_real cos_thp = cos_th * cos_phi - sin_th * sin_phi;

		n_type_gfx xx = ( n_type_gfx )( accum_th_amp - ( amp_b * sin_thp * updown_v ) );
		n_type_gfx yy = ( n_type_gfx )( amp_a - ( amp_b * cos_thp * updown_v ) );

		yy -= cam_off;
		yy += amp_a;

		n_type_gfx xxx = prev;
		while ( 1 )
		{
			if ( ( xxx < 0 ) || ( xxx >= sx ) )
			{
				/* out of bounds - do nothing */
			}
			else if ( line[ xxx ] == 0 )
			{
				line[ xxx ] = 1;
				n_nn2_wave_draw_vertical_line_fast( canvas, sx, sy, xxx, yy, n_bmp_rgb_mac( 0,0,255 ) );
			}

			if ( xx == xxx ) { prev = xxx; break; }
			if ( xx > xxx ) { xxx++; } else { xxx--; }
		}

		/* increment accumulators for next x */
		accum_th_amp += amp_a * delta;

		/* advance sin_th/cos_th by delta using rotation matrix to avoid sin()/cos() per x */
		n_type_real next_sin = sin_th * cos_dt + cos_th * sin_dt;
		n_type_real next_cos = cos_th * cos_dt - sin_th * sin_dt;
		sin_th = next_sin; cos_th = next_cos;

		x++;
		if ( x >= sx ) { break; }
	}


	const n_type_real step = 0.05;

	phi += step;

	if ( updown )
	{
		updown_v += step;
		if ( updown_v >= 1.0 )
		{
			updown_v = 1.0;
			updown   = FALSE;
		}
	}
	else
	{
		updown_v -= step;
		if ( updown_v <= 0.5 )
		{
			updown_v = 0.5;
			updown   = TRUE;
		}
	}

	return;
}

void
n_nn2_wave_bubble_draw( n_nn2 *p )
{
//return;

	if ( p->wave_onoff == FALSE ) { return; }


	int i = 0;
	n_posix_loop
	{//break;

		n_particle *b = &p->bubble[ i ];

		if ( b->onoff )
		{
			b->y -= 4;
			if ( b->y < ( ( p->mapchip_unit * 12 ) + 32 ) )
			{
				b->onoff = FALSE;
			} else {
				n_type_gfx x = b->x - p->stage->camera_x;
				n_type_gfx y = b->y - p->stage->camera_y;
				n_bmp_hoop( p->canvas, x, y, b->sx, b->sy, 1, n_bmp_white );
				b->x += 8 - n_game_random( 16 );
			}
		} else
		if ( n_game_timer( &p->bubble_timer, 1000 ) )
		{
			//n_type_gfx r = n_game_random( 16 );

			b->onoff = TRUE;
			b->x     = p->stage->nina_x + ( p->nina_sx / 2 );
			b->y     = p->stage->nina_y + n_chara_margin_head( p );
			b->sx    = 10;// + r;
			b->sy    = 10;// + r;
		}

		i++;
		if ( i >= N_NN2_BUBBLE_MAX ) { break; }
	}


	return;
}

