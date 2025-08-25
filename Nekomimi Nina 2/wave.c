// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




void
n_nn2_wave_draw_vertical_line( n_nn2 *p, n_type_gfx x, n_type_gfx y )
{

	if ( ( x < 0 )||( x >= p->sx ) ) { return; }


	n_bmp_ptr_set( p->canvas, x, y, n_bmp_white );


	n_posix_loop
	{
		y++;
		if ( y >= p->sy ) { break; }


		u32 color;
		n_bmp_ptr_get( p->canvas, x, y, &color );

		color = n_bmp_blend_pixel( color, n_bmp_rgb_mac( 0,0,255 ), 0.2 );

		n_bmp_ptr_set( p->canvas, x, y, color );
	}


	return;
}

void
n_nn2_wave_draw( n_nn2 *p )
{
//return;

	if ( p->wave_onoff == FALSE ) { return; }


	// [!] : trochoidal wave : thanx DeepSeek

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

	n_type_gfx  prev = 0;
	int         byte = p->sx * sizeof( int );
	int        *line = n_memory_new( byte ); n_memory_zero( line, byte );

	static n_type_real updown_v = 1.0;
	static BOOL        updown   = FALSE;

	static n_type_real phi = 0;

	n_type_gfx x = 0;
	n_posix_loop
	{

		n_type_real len = x / length;
		n_type_real th  = pi2 * len;

		n_type_gfx xx = ( amp_a * th ) - ( amp_b * sin( th + phi ) * updown_v );
		n_type_gfx yy = ( amp_a      ) - ( amp_b * cos( th + phi ) * updown_v );


		yy -= p->stage->camera_wave_offset;
		yy += amp_a;

		n_type_gfx xxx = prev;
		n_posix_loop
		{//break;
			if ( ( xxx < 0 )||( xxx >= p->sx ) )
			{
				//break;
			} else
			if ( line[ xxx ] == 0 )
			{
				line[ xxx ] = 1;
				n_nn2_wave_draw_vertical_line( p, xxx, yy );
			}

			if ( xx == xxx ) { prev = xxx; break; } else
			if ( xx >  xxx ) { xxx++; } else { xxx--; }
		}


		x++;
		if ( x >= p->sx ) { break; }
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
	} else {
		updown_v -= step;
		if ( updown_v <= 0.5 )
		{
			updown_v = 0.5;
			updown   = TRUE;
		}
	}


	n_memory_free( line );


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

