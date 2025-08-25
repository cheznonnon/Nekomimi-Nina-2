// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




void
n_nn2_weather_init( n_nn2 *p, int hour )
{

	if ( hour == -1 )
	{
		n_time_today( NULL, NULL, NULL, &hour, NULL, NULL );
	}

	p->weather_hour = hour % 24;

//NSLog( @"%d", hour );
//hour = 0;
//hour = 4;
//hour = 8;
//hour = 14;
//hour = 18;
//hour = 23;

	u32 color_1 = n_bmp_rgb_mac( 0, 50,100 );
	u32 color_2 = n_bmp_rgb_mac( 0,100,200 );
	u32 color_3 = n_bmp_rgb_mac( 0,200,255 );

	n_type_real v = (n_type_real) hour / 24;
//NSLog( @"%f", coeff );

	if ( v < 0.25 )
	{
		n_type_real blend = v / 0.25;
		p->weather_gradient_upper = n_bmp_black;
		p->weather_gradient_lower = n_bmp_blend_pixel( color_1, color_2, blend );
		p->weather_gradient_color = p->weather_gradient_lower;
		p->weather_kirakira_onoff = YES;
		p->weather_darkmode_onoff = YES;
	} else
	if ( v < 0.50 )
	{
		n_type_real blend = ( v - 0.25 ) / 0.25;
		p->weather_gradient_upper = n_bmp_blend_pixel( color_2, color_3, blend );
		p->weather_gradient_lower = n_bmp_white;
		p->weather_gradient_color = p->weather_gradient_upper;
		p->weather_kirakira_onoff = NO;
		p->weather_darkmode_onoff = NO;
	} else
	if ( v < 0.75 )
	{
		n_type_real blend = ( v - 0.50 ) / 0.25;
		p->weather_gradient_upper = n_bmp_blend_pixel( color_3, color_2, blend );
		p->weather_gradient_lower = n_bmp_white;
		p->weather_gradient_color = p->weather_gradient_upper;
		p->weather_kirakira_onoff = NO;
		p->weather_darkmode_onoff = NO;
	} else
	//
	{
		n_type_real blend = ( v - 0.75 ) / 0.25;
		p->weather_gradient_upper = n_bmp_black;
		p->weather_gradient_lower = n_bmp_blend_pixel( color_2, color_1, blend );
		p->weather_gradient_color = p->weather_gradient_lower;
		p->weather_kirakira_onoff = YES;
		p->weather_darkmode_onoff = YES;
	}


	return;
}

void
n_nn2_weather_change( n_nn2 *p, n_nn2_stage *s, int hour )
{

	n_nn2_weather_init( p, hour );


	int darkmode_onoff = p->weather_darkmode_onoff;
	u32 gradient_upper = p->weather_gradient_upper;
	u32 gradient_lower = p->weather_gradient_lower;

	if ( s == &n_nn2_stage_3 )
	{
		darkmode_onoff = 2;
		gradient_upper = n_bmp_black;
		gradient_lower = n_bmp_rgb_mac( 50,50,50 );
	}


	// [!] : Objects

	extern void n_nn2_stage_earth_maker( n_nn2 *p, n_nn2_stage *s, int dark );

	n_nn2_stage_earth_maker( p, s, darkmode_onoff );


	extern void n_nn2_stage_brick_maker( n_nn2 *p, n_nn2_stage *s, n_bmp *bmp, CGFloat blend, int dark );

	n_nn2_stage_brick_maker( p, s, &s->chip_brick_1, 0.55, darkmode_onoff );
	n_nn2_stage_brick_maker( p, s, &s->chip_brick_2, 0.77, darkmode_onoff );
	n_nn2_stage_brick_maker( p, s, &s->chip_brick_3, 0.99, darkmode_onoff );


	extern void n_nn2_stage_block_maker( n_nn2 *p, n_nn2_stage *s, n_bmp *bmp, int dark );

	n_nn2_stage_block_maker( p, s, &s->chip_block, darkmode_onoff );


	extern void n_nn2_stage_rocks_maker( n_nn2 *p, n_nn2_stage *s, int dark );

	n_nn2_stage_rocks_maker( p, s, darkmode_onoff );


	if ( darkmode_onoff )
	{
		p->weather_bmp = &p->weather_bmp_lite;
	} else {
		p->weather_bmp = &p->weather_bmp_dark;
	}


	if ( p->object_handheld_sprite == &p->share->kuina_sprite )
	{
		//
	} else
	if ( darkmode_onoff )
	{
		p->share->kuina_sprite.invisible = FALSE;
	} else {
		p->share->kuina_sprite.invisible = TRUE;
	}


	return;
}

void
n_nn2_weather_change_background( n_nn2 *p, n_nn2_stage *s )
{

	// [Needed] : call after n_chip_init()

	BOOL darkmode_onoff = p->weather_darkmode_onoff;
	u32  gradient_upper = p->weather_gradient_upper;
	u32  gradient_lower = p->weather_gradient_lower;

	if ( s == &n_nn2_stage_3 )
	{
		darkmode_onoff = YES;
		gradient_upper = n_bmp_black;
		gradient_lower = n_bmp_rgb_mac(  50, 50, 50 );

		n_bmp_new_fast( &s->canvas_background, p->sx, p->sy/2 );
		n_bmp_flush_gradient( &s->canvas_background, gradient_lower, gradient_upper, N_BMP_GRADIENT_VERTICAL );

		n_bmp_resizer( &s->canvas_background, p->sx, s->map_sy, n_bmp_black, N_BMP_RESIZER_NORMAL );

		n_bmp_flush_mirror( &s->canvas_background, N_BMP_MIRROR_ROTATE180 );
	} else
	if ( s == &n_nn2_stage_6 )
	{
		gradient_lower = n_bmp_rgb_mac( 0,0,255 );

		n_bmp_new( &s->canvas_background, p->sx, s->map_sy );
		n_bmp_flush_gradient( &s->canvas_background, gradient_upper, gradient_lower, N_BMP_GRADIENT_VERTICAL );
	} else
	if ( s == &n_nn2_stage_7 )
	{
		// [!] : black
		n_bmp_new( &s->canvas_background, p->sx, s->map_sy );
		//n_bmp_flush( &s->canvas_background, n_bmp_white );
	} else
	//
	{
		n_bmp_new_fast( &s->canvas_background, p->sx, s->map_sy );
		n_bmp_flush_gradient( &s->canvas_background, gradient_upper, gradient_lower, N_BMP_GRADIENT_VERTICAL );
	}


	return;
}

void
n_nn2_weather_transition( n_nn2 *p, int hour )
{

	if ( p->weather_transition ) { return; }


	n_bmp_free( &p->transition_bmp_old );
	n_bmp_carboncopy( p->canvas, &p->transition_bmp_old );


	n_nn2_weather_change( p, &n_nn2_stage_0, hour );
	n_nn2_weather_change( p, &n_nn2_stage_1, hour );
	n_nn2_weather_change( p, &n_nn2_stage_2, hour );
	n_nn2_weather_change( p, &n_nn2_stage_3, hour );
	n_nn2_weather_change( p, &n_nn2_stage_4, hour );
	n_nn2_weather_change( p, &n_nn2_stage_5, hour );
	n_nn2_weather_change( p, &n_nn2_stage_6, hour );
	n_nn2_weather_change( p, &n_nn2_stage_7, hour );
	n_nn2_weather_change( p, &n_nn2_stage_8, hour );

	n_nn2_weather_change_background( p, &n_nn2_stage_0 );
	n_nn2_weather_change_background( p, &n_nn2_stage_1 );
	n_nn2_weather_change_background( p, &n_nn2_stage_2 );
	n_nn2_weather_change_background( p, &n_nn2_stage_3 );
	n_nn2_weather_change_background( p, &n_nn2_stage_4 );
	n_nn2_weather_change_background( p, &n_nn2_stage_5 );
	n_nn2_weather_change_background( p, &n_nn2_stage_6 );
	n_nn2_weather_change_background( p, &n_nn2_stage_7 );
	n_nn2_weather_change_background( p, &n_nn2_stage_8 );


	n_bmp_new_fast( &p->transition_bmp_new, p->sx, p->sy );


	extern void n_nn2_loop( n_nn2 *p );
	extern void n_nn2_draw( n_nn2 *p );

	p->canvas = &p->transition_bmp_new;

	n_nn2_loop( p );
	n_nn2_draw( p );

	p->canvas = &p->canvas_main;


	p->transition_percent = 0;
	p->weather_transition = TRUE;


	return;
}

