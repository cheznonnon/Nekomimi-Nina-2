// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




void
n_splash_smoke_kernel( n_nn2 *p, n_type_gfx x, n_type_gfx y )
{

	u32 color = n_bmp_rgb_mac( 255,255,255 );
	u32 edge  = n_bmp_rgb_mac( 200,200,200 );

	n_bmp bmp; n_bmp_zero( &bmp );

	int i = 0;
	n_posix_loop
	{
		n_type_gfx sz = n_game_random( N_NN2_SPLASH_SIZE );
		n_type_gfx hf = sz / 2;

		n_type_gfx rx = n_game_random( sz );
		n_type_gfx ry = n_game_random( sz );

		//n_bmp_circle( p->canvas, x + rx - hf, y + ry - hf, sz,sz,    color );
		//n_bmp_hoop  ( p->canvas, x + rx - hf, y + ry - hf, sz,sz, 2, edge  );

		n_bmp_new_fast( &bmp, sz, sz ); n_bmp_flush( &bmp, 0 );

		n_bmp_circle( &bmp, 0, 0, sz,sz,    color );
		n_bmp_hoop  ( &bmp, 0, 0, sz,sz, 2, edge  );

		n_type_real blend = (n_type_real) sz / ( N_NN2_SPLASH_SIZE - 1 );

		n_bmp_blendcopy( &bmp, p->canvas, 0, 0, sz,sz, x + rx - hf, y + ry - hf, 1.0 - blend );

		i++;
		if ( i >= N_NN2_SPLASH_COUNT ) { break; }
	}

	n_bmp_free_fast( &bmp );


	return;
}

void
n_splash_water_kernel( n_nn2 *p, n_type_gfx x, n_type_gfx y )
{

	u32 color = n_bmp_rgb_mac( 0,0,255 );

	int i = 0;
	n_posix_loop
	{

		if ( 0 == n_game_random( 4 ) )
		{
			n_type_gfx size = 12 + n_game_random( 12 );
			n_type_gfx half = size / 2;

			n_type_gfx rx = n_game_random( size * 2 );
			n_type_gfx ry = n_game_random( size * 2 );

			extern void n_object_kirakira_bmp( n_bmp *bmp, n_type_gfx sx, n_type_gfx sy, u32 bg, u32 fg );
			n_object_kirakira_bmp( &p->splash_water_kirakira, size,size, 0, n_bmp_white );

			n_bmp_transcopy( &p->splash_water_kirakira, p->canvas, 0,0,size,size, x + rx - half, y + ry - half );
		} else {
			n_type_gfx size = 8 + n_game_random( 8 );
			n_type_gfx half = size / 2;

			n_type_gfx rx = n_game_random( size * 2 );
			n_type_gfx ry = n_game_random( size * 2 );

			n_type_real blend = (n_type_real) n_game_random( 255 ) / 255;

			u32 c = n_bmp_blend_pixel( color, n_bmp_white, blend );

			n_bmp_circle( p->canvas, x + rx - half, y + ry - half, size,size,              c );
			n_bmp_hoop  ( p->canvas, x + rx - half, y + ry - half, size,size, 1, n_bmp_white );
		}

		i++;
		if ( i >= 4 ) { break; }
	}


	return;
}




void
n_splash_init( n_nn2 *p, n_splash *s, n_type_gfx x, n_type_gfx y, u32 timeout )
{
//return;

	s->onoff = TRUE;

	s->x = x;
	s->y = y;

	s->ox = 0;
	s->oy = 0;

	s->flowy = 0.0;

	s->timer = n_posix_tickcount() + timeout;


	return;
}

void
n_splash_init_water( n_nn2 *p )
{

	n_type_gfx x = p->stage->nina_x;
	n_type_gfx y = p->stage->nina_y + p->nina_sy - p->nina_margin_foot - N_NN2_SPLASH_SIZE;

	if ( p->direction == N_NN2_DIRECTION_LEFT )
	{
		x += N_NN2_SPLASH_SIZE;
	} else
	if ( p->direction == N_NN2_DIRECTION_RIGHT )
	{
		x -= N_NN2_SPLASH_SIZE;
	}

	n_splash_init( p, &p->splash_water, x, y, 666 );


	return;
}




void
n_splash_cloud_draw( n_nn2 *p, n_splash *s )
{

	if ( s->onoff == FALSE ) { return; }

	if ( s->timer < n_posix_tickcount() ) { s->onoff = FALSE; return; }


	n_type_gfx lx,rx,y;

	lx = s->x + p->nina_margin_rear;
	rx = s->x + p->nina_sx - p->nina_margin_rear;

	 y = s->y;


	const n_type_gfx offset = N_NN2_SPLASH_SIZE + 8;

	lx -= offset;
	rx += offset / 2;


	lx -= s->ox;
	rx += s->ox;
	 y -= s->oy;

	s->ox += p->zoom;
	s->oy  = n_nn2_flowy_effect( N_NN2_SPLASH_SIZE, s->flowy );

	s->flowy += 0.05;


	lx -= n_camerawork_x( p );
	rx -= n_camerawork_x( p );
	 y -= n_camerawork_y( p );


	n_splash_smoke_kernel( p, rx, y );
	n_splash_smoke_kernel( p, lx, y );


	return;
}




void
n_splash_dash_draw( n_nn2 *p, n_splash *s )
{

	if ( p->swim_onoff ) { return; }


	if ( s->onoff == FALSE ) { return; }

	if ( s->timer < n_posix_tickcount() ) { s->onoff = FALSE; return; }


	n_type_gfx lx,rx,y;

	lx = s->x + p->nina_sx - p->nina_margin_rear;
	rx = s->x + p->nina_margin_rear;

	 y = s->y + p->nina_sy - p->nina_margin_foot - N_NN2_SPLASH_SIZE;


	const n_type_gfx offset = N_NN2_SPLASH_SIZE * 3;

	lx += offset;
	rx -= offset;


	lx += s->ox;
	rx -= s->ox;
	 y -= s->oy;

	s->ox += p->zoom * 2;
	s->oy  = n_nn2_flowy_effect( N_NN2_SPLASH_SIZE, s->flowy );

	s->flowy += 0.025;


	lx -= n_camerawork_x( p );
	rx -= n_camerawork_x( p );
	 y -= n_camerawork_y( p );


	if ( p->direction == N_NN2_DIRECTION_LEFT )
	{
		n_splash_smoke_kernel( p, lx, y );
	} else
	if ( p->direction == N_NN2_DIRECTION_RIGHT )
	{
		n_splash_smoke_kernel( p, rx, y );
	}


	return;
}

void
n_splash_slip_draw( n_nn2 *p, n_splash *s )
{

	if ( p->swim_onoff ) { return; }


	if ( s->onoff == FALSE ) { return; }

	if ( s->timer < n_posix_tickcount() ) { s->onoff = FALSE; return; }


	n_type_gfx lx,rx,y;

	lx = s->x + ( p->nina_sx / 2 );
	rx = s->x + ( p->nina_sx / 2 );

	 y = s->y + p->nina_sy - p->nina_margin_foot - N_NN2_SPLASH_SIZE;


	lx += s->ox;
	rx -= s->ox;
	 y -= s->oy;

	s->ox += p->zoom * 2;
	s->oy  = n_nn2_flowy_effect( N_NN2_SPLASH_SIZE, s->flowy );

	s->flowy += 0.025;


	lx -= n_camerawork_x( p );
	rx -= n_camerawork_x( p );
	 y -= n_camerawork_y( p );


	if ( p->direction == N_NN2_DIRECTION_LEFT )
	{
		n_splash_smoke_kernel( p, lx, y );
	} else
	if ( p->direction == N_NN2_DIRECTION_RIGHT )
	{
		n_splash_smoke_kernel( p, rx, y );
	}


	return;
}




void
n_splash_water_draw( n_nn2 *p, n_splash *s )
{

	if ( s->onoff == FALSE ) { return; }

	if ( s->timer < n_posix_tickcount() ) { s->onoff = FALSE; return; }


	n_type_gfx lx,rx,y;

	lx = s->x + ( p->nina_sx / 2 );
	rx = s->x + ( p->nina_sx / 2 );

	 y = s->y;

//n_bmp_box( p->canvas, lx,0, 2,p->sy, n_bmp_rgb_mac( 255,0,0 ) );


	lx -= s->ox;
	rx += s->ox;
	 y -= s->oy;

	s->ox += p->zoom * 2;
	s->oy  = n_nn2_flowy_effect( N_NN2_SPLASH_SIZE * 2, s->flowy );

	s->flowy += 0.025;


	lx -= n_camerawork_x( p );
	rx -= n_camerawork_x( p );
	 y -= n_camerawork_y( p );


	n_splash_water_kernel( p, rx, y );
	n_splash_water_kernel( p, lx, y );


	return;
}


