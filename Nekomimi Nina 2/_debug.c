// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




void
n_nn2_debug_bmp_save( n_nn2 *p, n_bmp *bmp )
{

	BOOL prv = n_bmp_flip_onoff; n_bmp_flip_onoff = FALSE;


	n_bmp b; n_bmp_carboncopy( bmp, &b );

	n_bmp_mac( &b );

	n_bmp_save_literal( &b, "/Users/nonnon2/Desktop/ret.bmp" );

	n_bmp_free_fast( &b );


	n_bmp_flip_onoff = prv;


	return;
}

void
n_nn2_debug_rect( n_nn2 *p )
{

	n_type_gfx x,y,sx,sy; n_mac_rect_expand_size( p->debug_rect, &x, &y, &sx, &sy );

	x -= p->stage->camera_x;
	y -= p->stage->camera_y;

	n_bmp_box( p->canvas, x, y, sx, sy, n_bmp_rgb_mac( 255,0,0 ) );


	return;
}
/*
void
n_nn2_debug_ud( n_nn2 *p, n_type_gfx fy )
{

	n_type_gfx  x = 0;
	n_type_gfx  y = fy;
	n_type_gfx sx = p->sx;
	n_type_gfx sy = 0;

	x -= p->stage->camera_x;
	y -= p->stage->camera_y;

	n_bmp_line( p->canvas, x, y, x + sx, y + sy, n_bmp_black );


	return;
}

void
n_nn2_debug_lr( n_nn2 *p, n_type_gfx fx )
{

	extern n_type_gfx n_chara_margin_head( n_nn2 *p );

	n_type_gfx  x = fx;
	n_type_gfx  y = p->stage->nina_y + n_chara_margin_head( p );
	n_type_gfx sx = 0;
	n_type_gfx sy = p->nina_sy - p->nina_margin_foot - n_chara_margin_head( p );

	x -= p->stage->camera_x;
	y -= p->stage->camera_y;

	n_bmp_line( p->canvas, x, y, x + sx, y + sy, n_bmp_black );


	return;
}
*/
void
n_nn2_debug_dash( n_nn2 *p )
{

	n_posix_char str[ 1024 ];


	n_posix_sprintf_literal( str, "Dash : %d", p->dash_value );


	n_posix_strcat( str, " : " );

	if ( p->direction == N_NN2_DIRECTION_LEFT )
	{
		n_posix_strcat( str, "L" );
	} else
	if ( p->direction == N_NN2_DIRECTION_RIGHT )
	{
		n_posix_strcat( str, "R" );
	}


	n_posix_strcat( str, " : " );

	if ( p->dash_direction == N_NN2_DIRECTION_LEFT )
	{
		n_posix_strcat( str, "Dash L" );
	} else
	if ( p->dash_direction == N_NN2_DIRECTION_RIGHT )
	{
		n_posix_strcat( str, "Dash R" );
	}


	n_posix_strcat( str, " : " );

	if ( p->dash_phase == N_NN2_DASH_NONE )
	{
		n_posix_strcat( str, "NONE" );
	} else
	if ( p->dash_phase == N_NN2_DASH_INIT )
	{
		n_posix_strcat( str, "INIT" );
	} else
	if ( p->dash_phase == N_NN2_DASH_DONE )
	{
		n_posix_strcat( str, "DONE" );
	} //else


	n_posix_strcat( str, " : " );

	if ( p->brake_phase == N_NN2_DASH_BRAKE_NONE )
	{
		n_posix_strcat( str, "BRAKE_NONE" );
	} else
	if ( p->brake_phase == N_NN2_DASH_BRAKE_INIT )
	{
		n_posix_strcat( str, "BRAKE_INIT" );
	} else
	if ( p->brake_phase == N_NN2_DASH_BRAKE_DONE )
	{
		n_posix_strcat( str, "BRAKE_DONE" );
	} //else


	NSLog( @"%s", str );


	return;
}




#define N_CHARA_COLLISION_LR_NONE ( 0 << 0 )
#define N_CHARA_COLLISION_LR_HEAD ( 1 << 0 )
#define N_CHARA_COLLISION_LR_FOOT ( 1 << 1 )
#define N_CHARA_COLLISION_LR_SLIP ( 1 << 2 )
#define N_CHARA_COLLISION_LR_WALL ( 1 << 3 )
#define N_CHARA_COLLISION_LR_STOP ( 1 << 4 )
#define N_CHARA_COLLISION_LR_WL_L ( 1 << 5 )
#define N_CHARA_COLLISION_LR_WL_R ( 1 << 6 )
#define N_CHARA_COLLISION_STOP__L ( 1 << 7 )
#define N_CHARA_COLLISION_STOP__R ( 1 << 8 )

void
n_chara_collision_wall_debug( n_nn2 *p, int wall )
{

	if ( wall & N_CHARA_COLLISION_LR_HEAD )
	{
		NSLog( @"N_CHARA_COLLISION_LR_HEAD" );
	}

	if ( wall & N_CHARA_COLLISION_LR_FOOT )
	{
		NSLog( @"N_CHARA_COLLISION_LR_FOOT" );
	}

	if ( wall & N_CHARA_COLLISION_LR_SLIP )
	{
		NSLog( @"N_CHARA_COLLISION_LR_SLIP" );
	}

	if ( wall & N_CHARA_COLLISION_LR_WALL )
	{
		NSLog( @"N_CHARA_COLLISION_LR_WALL" );
	}

	if ( wall & N_CHARA_COLLISION_LR_STOP )
	{
		NSLog( @"N_CHARA_COLLISION_LR_STOP" );
	}

	if ( wall & N_CHARA_COLLISION_LR_WL_L )
	{
		NSLog( @"N_CHARA_COLLISION_LR_WL_L" );
	}

	if ( wall & N_CHARA_COLLISION_LR_WL_R )
	{
		NSLog( @"N_CHARA_COLLISION_LR_WL_R" );
	}

	if ( wall & N_CHARA_COLLISION_STOP__L )
	{
		NSLog( @"N_CHARA_COLLISION_STOP__L" );
	}

	if ( wall & N_CHARA_COLLISION_STOP__R )
	{
		NSLog( @"N_CHARA_COLLISION_STOP__R" );
	}


	return;
}

