// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#define N_NN2_BELL_MAX ( 5 )

void
n_object_bell_map_init( n_nn2 *p, n_nn2_stage *s )
{

	int count = 0;

	n_type_gfx x = 0;
	n_type_gfx y = 0;
	n_posix_loop
	{//break;
		u32 data = 0; n_bmp_ptr_get( &s->map_bell, x,y, &data );
//NSLog( @"%d", data );

		if ( 100 == n_bmp_r_mac( data ) )
		{
//NSLog( @"found : %d %d", x, y );
			count++;
		}

		x++;
		if ( x >= N_BMP_SX( &s->map_bell ) )
		{
			x = 0;

			y++;
			if ( y >= N_BMP_SY( &s->map_bell ) ) { break; }
		}
	}

//NSLog( @"stage %d : count = %d", n_nn2_stage_number_get( s ), count ); return;

	if ( count == 0 ) { return; }


	typedef struct {

		n_type_gfx x,y;

	} n_object_bell_map;


	int                byte = sizeof( n_object_bell_map ) * count;
	n_object_bell_map *map  = n_memory_new( byte ); n_memory_zero( map, byte );


	int i = 0;

	x = 0;
	y = 0;
	n_posix_loop
	{//break;
		u32 data = 0; n_bmp_ptr_get( &s->map_bell, x,y, &data );
//NSLog( @"%d", data );

		if ( 100 == n_bmp_r_mac( data ) )
		{
//NSLog( @"found : %d %d", x, y );

			map[ i ].x = x;
			map[ i ].y = y;

			i++;
			if ( i >= count ) { break; }

		}

		x++;
		if ( x >= N_BMP_SX( &s->map_bell ) )
		{
			x = 0;

			y++;
			if ( y >= N_BMP_SY( &s->map_bell ) ) { break; }
		}
	}
//return;

	{
		int i = 0;
		n_posix_loop
		{
			int f = n_game_random( count );
			int t = n_game_random( count );

			n_object_bell_map m        = map[ f ];
			                  map[ f ] = map[ t ];
			                  map[ t ] = m;

			i++;
			if ( i >= 100 ) { break; }
		}

		i = 0;
		n_posix_loop
		{
			n_bmp_ptr_set( &s->map, map[ i ].x, map[ i ].y, n_bmp_rgb_mac( 0,0,N_CHIP_BELL ) );

			i++;
			if ( i >= N_NN2_BELL_MAX ) { break; }
		}
	}


	return;
}

void
n_object_bell_init( n_nn2 *p )
{

	n_nn2_rc_load( @"rc/chip/bell/0", &p->bell_bmp_0 , p->scaler );
	n_nn2_rc_load( @"rc/chip/bell/1", &p->bell_bmp_1 , p->scaler );
	n_nn2_rc_load( @"rc/chip/bell/2", &p->bell_bmp_2 , p->scaler );

	p->bell_count     = N_NN2_BELL_DEFAULT;
	p->bell_count_prv = -1;


	return;
}

void
n_object_bell_reset( n_nn2 *p )
{

	p->bell_count     =  N_NN2_BELL_DEFAULT;
	p->bell_count_prv = -1;

	n_nn2_rc_load_map( @"rc/1/map/0", &n_nn2_stage_1.map );
	n_nn2_rc_load_map( @"rc/2/map/0", &n_nn2_stage_2.map );
	n_nn2_rc_load_map( @"rc/3/map/0", &n_nn2_stage_3.map );
	n_nn2_rc_load_map( @"rc/4/map/0", &n_nn2_stage_4.map );
	n_nn2_rc_load_map( @"rc/5/map/0", &n_nn2_stage_5.map );
	n_nn2_rc_load_map( @"rc/6/map/0", &n_nn2_stage_6.map );

	n_object_bell_map_init( p, &n_nn2_stage_1 );
	n_object_bell_map_init( p, &n_nn2_stage_2 );
	n_object_bell_map_init( p, &n_nn2_stage_3 );
	n_object_bell_map_init( p, &n_nn2_stage_4 );
	n_object_bell_map_init( p, &n_nn2_stage_5 );
	n_object_bell_map_init( p, &n_nn2_stage_6 );

	n_chip_init( p, &n_nn2_stage_1 );
	n_chip_init( p, &n_nn2_stage_2 );
	n_chip_init( p, &n_nn2_stage_3 );
	n_chip_init( p, &n_nn2_stage_4 );
	n_chip_init( p, &n_nn2_stage_5 );
	n_chip_init( p, &n_nn2_stage_6 );


	return;
}

void
n_object_bell_info( n_nn2 *p, n_bmp *bmp, int count, int fxsize )
{
//NSLog( @"n_object_bell_info()" );

	n_gdi gdi; n_gdi_zero( &gdi );


	n_posix_char str[ 100 ]; n_posix_sprintf( str, " x %d", count );

	gdi.base_color_bg       = n_bmp_white_invisible;

	gdi.text                = str;
	gdi.text_font           = N_NN2_FONT_NAME_INDICATOR;
	gdi.text_size           = 20;
	gdi.text_style          = N_GDI_TEXT_SMOOTH | N_GDI_TEXT_CONTOUR;
	gdi.text_color_main     = n_bmp_rgb_mac( 255,255,255 );
	gdi.text_color_contour  = n_bmp_rgb_mac(   0,150,200 );
	gdi.text_color_shadow   = n_bmp_rgb_mac(   0,150,200 );
	gdi.text_fxsize1        = fxsize;
	gdi.text_fxsize2        = fxsize;


	n_bmp_free( bmp );
	n_gdi_bmp( &gdi, bmp );


	return;
}

void
n_object_bell_stage_number( n_nn2 *p, n_bmp *bmp, int number, n_bmp *bmp_indicator )
{

	// [!] : shared

	n_type_gfx gap_a;
	n_type_gfx gap_b;
	n_type_gfx gap_c;

	n_posix_char str[ 100 ];

	if ( bmp_indicator == &p->bell_bmp_0 )
	{
		gap_a = 4;
		gap_b = 0;
		gap_c = 8;
		n_posix_sprintf( str, "x%d", number );
	} else {
		gap_a = 4;
		gap_b = 8;
		gap_c = 8;
		n_posix_sprintf( str, "%d", number );
	}


	// [!] : Number

	n_gdi gdi; n_gdi_zero( &gdi );

	gdi.text                = str;
	gdi.text_font           = N_NN2_FONT_NAME_INDICATOR;
	gdi.text_size           = 20;
	gdi.text_style          = N_GDI_TEXT_SMOOTH | N_GDI_TEXT_MAC_NO_CROP;

	gdi.base_color_bg       = n_bmp_black;
	gdi.text_color_main     = n_bmp_white;

	n_bmp bmp_no; n_bmp_zero( &bmp_no ); n_gdi_bmp( &gdi, &bmp_no );

	n_type_gfx sx = N_BMP_SX( &p->bell_bmp_0 ) + N_BMP_SX( &bmp_no ) + gap_a + gap_b + gap_c;
	n_type_gfx sy = N_BMP_SY( &p->bell_bmp_0 );


	// [!] : background

	n_bmp bmp_bg; n_bmp_zero( &bmp_bg ); n_bmp_new_fast( &bmp_bg, sx, sy );
	n_bmp_flush( &bmp_bg, n_bmp_white_invisible );

	n_bmp_roundrect( &bmp_bg, 0, 0, sx, sy, n_bmp_argb_mac( 250,255,255,255 ), sy );


	// [!] : Maker

	n_bmp_free( bmp ); n_bmp_new_fast( bmp, sx, sy );
	n_bmp_flush( bmp, n_bmp_white_invisible );


	n_bmp_flush_transcopy( &bmp_bg, bmp );

	n_bmp_transcopy
	(
		bmp_indicator, bmp,
		0,0,
		N_BMP_SX( bmp_indicator ),
		N_BMP_SY( bmp_indicator ),
		gap_a, 0
	);

	n_type_gfx x = gap_a + N_BMP_SX( bmp_indicator ) + gap_b;
	n_type_gfx y = n_game_centering( sy, N_BMP_SY( &bmp_no ) );

	n_bmp_rasterizer( &bmp_no, bmp, x,y, n_bmp_white_invisible, FALSE );


	n_bmp_free_fast( &bmp_bg );
	n_bmp_free_fast( &bmp_no );


	return;
}

void
n_object_bell_stage_number_nina( n_nn2 *p, n_nn2_stage *stg )
{

	n_object_bell_stage_number
	(
		p,
		&stg->bmp_stage_number,
		n_nn2_stage_number_get( stg ),
		&p->nina_indicator
	);


	return;
}

void
n_object_bell_draw( n_nn2 *p )
{
//return;

	if ( p->bell_onoff == FALSE ) { return; }


	if ( p->bell_count_prv != p->bell_count )
	{
		n_object_bell_info( p, &p->bell_bmp_info, p->bell_count, 1 );

		p->bell_count_prv = p->bell_count;
	}


	n_type_gfx gap = 10;

	n_type_gfx  x = gap;
	n_type_gfx  y = gap;
	n_type_gfx sx = N_BMP_SX( &p->bell_bmp_0 );
	n_type_gfx sy = N_BMP_SY( &p->bell_bmp_0 );

	if ( p->stage == &n_nn2_stage_7 )
	{
		y += gap + N_NN2_TIMEUP_BAR_SIZE;
	}

	{
		n_bmp *b = &p->stage->bmp_stage_number;

		n_bmp_transcopy
		(
			b, p->canvas,
			0, 0, N_BMP_SX( b ), N_BMP_SY( b ),
			x, y
		);

		x += N_BMP_SX( b ) + gap;
	}


	if ( p->chick_stage_number_prv != p->share->chick_sprite.stage_number )
	{
		n_object_bell_stage_number
		(
			p,
			&p->chick_stage_number_bmp,
			 p->share->chick_sprite.stage_number,
			&p->chick_stage_number_indicator
		);
	}

	n_type_gfx offset = N_BMP_SX( &p->chick_stage_number_bmp ) + gap;

	if ( p->chick_stage_number_phase == 1 )
	{
		p->chick_stage_number_remover += 4;

		if ( p->chick_stage_number_remover >= offset )
		{
			p->chick_stage_number_phase   = 2;
			p->chick_stage_number_remover = offset;
		}

		n_bmp_blendcopy
		(
			&p->chick_stage_number_bmp, p->canvas,
			0, 0,
			N_BMP_SX( &p->chick_stage_number_bmp ),
			N_BMP_SY( &p->chick_stage_number_bmp ),
			x, y,
			1.0 - ( (CGFloat) p->chick_stage_number_remover / offset )
		);

		x += p->chick_stage_number_remover;
	} else
	if ( p->chick_stage_number_phase == 2 )
	{
		n_bmp_transcopy
		(
			&p->chick_stage_number_bmp, p->canvas,
			0, 0,
			N_BMP_SX( &p->chick_stage_number_bmp ),
			N_BMP_SY( &p->chick_stage_number_bmp ),
			x, y
		);

		x += N_BMP_SX( &p->chick_stage_number_bmp ) + gap;
	}


	n_bmp_transcopy
	(
		&p->bell_bmp_0, p->canvas,
		0, 0, sx, sy,
		x, y
	);

	x += sx;
	y += ( N_BMP_SY( &p->bell_bmp_0 ) - N_BMP_SY( &p->bell_bmp_info ) ) / 2;

	sx = N_BMP_SX( &p->bell_bmp_info );
	sy = N_BMP_SY( &p->bell_bmp_info );

	n_bmp_transcopy
	(
		&p->bell_bmp_info, p->canvas,
		0, 0, sx, sy,
		x, y
	);


	return;
}

BOOL
n_object_bell_detect( n_nn2 *p, n_type_gfx tx, n_type_gfx ty )
{
//return FALSE;

	if ( p->bell_onoff == FALSE ) { return FALSE; }


	BOOL ret = FALSE;

	u32 data = 0; n_bmp_ptr_get( &p->stage->map, tx, ty, &data );

	if ( N_CHIP_BELL == n_chip_data_kind( data ) )
	{
		ret = TRUE;

		p->bell_count++;

		n_bmp_ptr_set( &p->stage->map, tx, ty, n_bmp_black );

		n_sprite *spr = n_chip_get_auto( p, tx,ty );
		n_sprite_zero( spr );

		n_nn2_sound_effect_play( N_NN2_SOUND_BELL );
	}


	return ret;
}

BOOL
n_object_bell_condition_detect( n_nn2 *p, n_type_gfx x, n_type_gfx y )
{
//return TRUE;

	u32 data = 0; n_bmp_ptr_get( &p->stage->map_dokan, x, y, &data );
//NSLog( @"%d %d %d", n_bmp_r_mac( data ), n_bmp_g_mac( data ), n_bmp_b_mac( data ) );

	data = n_nn2_map_enter_data_kind( data ) - 150;
//data = 1;

	static u32 prev = 0xffff;
	if ( prev != data )
	{
		prev = data;

		n_object_bell_stage_number
		(
			p,
			&p->bell_condition_bmp,
			data,
			&p->bell_bmp_0
		);
	}

//NSLog( @"%d %d", data, p->bell_count );

	return ( data <= p->bell_count );
}

void
n_object_bell_condition_draw( n_nn2 *p )
{
//return;

	// [!] : fade-out animation

	static BOOL enabled = FALSE;

	static BOOL disappear_animation = FALSE;

	static n_type_real blend = 0.0;

	if (
		( p->bell_condition_onoff == FALSE )
		||
		( p->jump_state )
	)
	{
//return;
		if ( enabled )
		{
			disappear_animation = TRUE;
		} else {
			return;
		}
	} else {
		enabled = TRUE;
	}

	if ( disappear_animation )
	{
		static u32 timer = 0;
		if ( n_game_timer( &timer, 33 ) )
		{
			blend += 0.1;
			if ( blend >= 1.0 )
			{
				enabled = disappear_animation = FALSE;
				blend = 0.0;
				return;
			}
		}
	}


	// [!] : animation for duck

	static n_type_gfx margin_head_prv = 0;
	       n_type_gfx margin_head_cur = n_chara_margin_head( p );

	if ( margin_head_prv == 0 )
	{
		margin_head_prv = margin_head_cur;
	}

	if ( margin_head_prv != margin_head_cur )
	{
		const n_type_gfx step = 4;

		if ( margin_head_prv < margin_head_cur )
		{
			margin_head_prv += step;
			if ( margin_head_prv > margin_head_cur ) { margin_head_prv = margin_head_cur; }
		} else {
			margin_head_prv -= step;
			if ( margin_head_prv < margin_head_cur ) { margin_head_prv = margin_head_cur; }
		}
	}


	// [!] : main

	n_type_gfx gap = 24;

	n_type_gfx sx = N_BMP_SX( &p->bell_condition_bmp );
	n_type_gfx sy = N_BMP_SY( &p->bell_condition_bmp );

	n_type_gfx tx = p->bell_condition_dokan_x - n_camerawork_x( p ) - ( sx / 2 );
	n_type_gfx ty = p->bell_condition_dokan_y - n_camerawork_y( p ) + margin_head_prv - gap - sy;

	n_bmp_blendcopy
	(
		&p->bell_condition_bmp, p->canvas,
		0, 0, sx, sy,
		tx, ty,
		blend
	);


	return;
}

