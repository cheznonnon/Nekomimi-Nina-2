// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




void
n_nn2_crop( n_bmp *bmp, u32 color_condition )
{

	if ( n_bmp_error( bmp ) ) { return; }


	n_type_gfx bmpsx = N_BMP_SX( bmp );
	n_type_gfx bmpsy = N_BMP_SY( bmp );


	n_type_gfx fx = 0;
	n_type_gfx tx = 0;
	n_type_gfx fy = bmpsx;
	n_type_gfx ty = bmpsy;


	n_type_gfx x,y;


	// [!] : Left
	
	x = y = 0;
	n_posix_loop
	{

		u32 color; n_bmp_ptr_get_fast( bmp, x,y, &color );
		if ( color != color_condition ) { fx = x; break; }

		y++;
		if ( y >= bmpsy )
		{
			y = 0;

			x++;
			if ( x >= bmpsx ) { break; }
		}
	}

	// [!] : Top
	
	x = y = 0;
	n_posix_loop
	{

		u32 color; n_bmp_ptr_get_fast( bmp, x,y, &color );
		if ( color != color_condition ) { fy = y; break; }

		x++;
		if ( x >= bmpsx )
		{
			x = 0;

			y++;
			if ( y >= bmpsy ) { break; }
		}
	}

	// [!] : Right
	
	x = bmpsx - 1;
	y = 0;
	n_posix_loop
	{

		u32 color; n_bmp_ptr_get_fast( bmp, x,y, &color );
		if ( color != color_condition ) { tx = x; break; }

		y++;
		if ( y >= bmpsy )
		{
			y = 0;

			x--;
			if ( x < 0 ) { break; }
		}
	}

	// [!] : Bottom
	
	x = 0;
	y = bmpsy - 1;
	n_posix_loop
	{

		u32 color; n_bmp_ptr_get_fast( bmp, x,y, &color );
		if ( color != color_condition ) { ty = y; break; }

		x++;
		if ( x >= bmpsx )
		{
			x = 0;

			y--;
			if ( y < 0 ) { break; }
		}
	}

	if ( fx >     0 ) { fx--; }
	if ( fy >     0 ) { fy--; }
	if ( tx < bmpsx ) { tx++; }
	if ( ty < bmpsx ) { ty++; }

	n_type_gfx fsx = tx - fx; if ( fsx < 0 ) { return; }
	n_type_gfx fsy = ty - fy; if ( fsy < 0 ) { return; }

	n_bmp ret; n_bmp_zero( &ret ); n_bmp_new( &ret, fsx, fsy );

	n_bmp_fastcopy( bmp, &ret, fx,fy,fsx,fsy, 0,0 );

	n_bmp_free( bmp );
	n_bmp_alias( &ret, bmp );


	return;
}

void
n_nn2_walk_shadow( n_bmp *bmp_f, n_bmp *bmp_t )
{

	n_bmp_carboncopy( bmp_f, bmp_t );


	u32         blend_color = n_bmp_black;
	n_type_real blend_ratio = 0.2;

	n_nn2_bmp_flush_mixer( bmp_t, blend_color, blend_ratio );


	return;
}

void
n_nn2_stage_dark_maker( n_nn2 *p, n_bmp *bmp, int dark )
{

	// [!] : make blue
	n_bmp_flush_hue_wheel( bmp, -127 );

	// [!] : make darker tone
	if ( dark == 1 )
	{
		n_nn2_bmp_flush_mixer( bmp, p->weather_gradient_color, 0.5 );
	}


	return;
}

void
n_nn2_stage_earth_maker_cornermask( n_nn2 *p, n_bmp *bmp, u32 bg_ul )
{

	if ( n_bmp_error( bmp ) ) { return; }


	n_type_gfx round_size = 4;


	n_bmp b; n_bmp_carboncopy( bmp, &b ); n_bmp_flush( &b, n_bmp_white );

	n_bmp_flush_roundrect( &b, n_bmp_black, round_size );
//n_nn2_debug_bmp_save( p, &b );


	n_type_gfx  x = 0;
	n_type_gfx  y = 0;
	n_type_gfx sx = N_BMP_SX( &b );
	n_type_gfx sy = N_BMP_SY( &b );


	round_size = n_bmp_roundrect_param( sx, sy, round_size );

	n_type_gfx sz = round_size;
	n_type_gfx ff = sz;
	//n_type_gfx tt = sz;

	n_bmp fragment; n_bmp_zero( &fragment ); n_bmp_1st_fast( &fragment, sz,sz );

	n_bmp_fastcopy( &b, &fragment, 0,0,sz,sz, 0,0 );
	n_bmp_rasterizer_cornermask( &fragment, bmp, 0,0,sz,ff, x      ,      y, bg_ul );

	//n_bmp_fastcopy( &b, &fragment, sx-sz,0,sz,sz, 0,0 );
	//n_bmp_rasterizer_cornermask( &fragment, bmp, 0,0,sz,ff, x+sx-sz,      y, bg_ur );

	//n_bmp_fastcopy( &b, &fragment, 0,sy-sz,sz,sz, 0,0 );
	//n_bmp_rasterizer_cornermask( &fragment, bmp, 0,0,sz,tt, x      ,y+sy-sz, bg_dl );

	//n_bmp_fastcopy( &b, &fragment, sx-sz,sy-sz,sz,sz, 0,0 );
	//n_bmp_rasterizer_cornermask( &fragment, bmp, 0,0,sz,tt, x+sx-sz,y+sy-sz, bg_dr );

	n_bmp_free_fast( &fragment );


	n_bmp_free_fast( &b );


	return;
}

void
n_nn2_stage_earth_maker( n_nn2 *p, n_nn2_stage *s, int dark )
{

	n_bmp bmp; n_bmp_zero( &bmp );

	if ( s == &n_nn2_stage_6 )
	{
		n_nn2_rc_load( @"rc/chip/sand/0" , &bmp, p->scaler );
	} else {
		n_nn2_rc_load( @"rc/chip/earth/0", &bmp, p->scaler );
	}

	if ( dark ) { n_nn2_stage_dark_maker( p, &bmp, dark ); }

	n_type_gfx sz = p->mapchip_unit;

	int i = 0;
	n_posix_loop
	{
		n_bmp_new_fast( &s->chip_earth[ i ], sz,sz );
		//n_bmp_flush( &s->chip_earth[ i ], n_bmp_white_invisible );

		i++;
		if ( i >= ( 6 + 2 ) ) { break; }
	}

	n_nn2_stage_earth_maker_cornermask( p, &bmp, n_bmp_alpha_invisible_pixel( p->weather_gradient_color ) );

	n_bmp_fastcopy( &bmp, &s->chip_earth[ 0 ], sz*0,sz*0,sz,sz, 0,0 );
	n_bmp_fastcopy( &bmp, &s->chip_earth[ 1 ], sz*1,sz*0,sz,sz, 0,0 );
	n_bmp_fastcopy( &bmp, &s->chip_earth[ 2 ], sz*0,sz*0,sz,sz, 0,0 );
	n_bmp_fastcopy( &bmp, &s->chip_earth[ 3 ], sz*0,sz*1,sz,sz, 0,0 );
	n_bmp_fastcopy( &bmp, &s->chip_earth[ 4 ], sz*1,sz*1,sz,sz, 0,0 );
	n_bmp_fastcopy( &bmp, &s->chip_earth[ 5 ], sz*0,sz*1,sz,sz, 0,0 );
//n_nn2_debug_bmp_save( p, &bmp );
//n_nn2_debug_bmp_save( p, &s->chip_earth[ 1 ] );

	n_bmp_flush_mirror( &s->chip_earth[ 2 ], N_BMP_MIRROR_LEFTSIDE_RIGHT );
	n_bmp_flush_mirror( &s->chip_earth[ 5 ], N_BMP_MIRROR_LEFTSIDE_RIGHT );

	{
		u32 color; n_bmp_ptr_get_fast( &s->chip_earth[ 4 ], 0,0, &color );

		n_bmp_flush( &s->chip_earth[ 6 ], color );
		n_bmp_flush( &s->chip_earth[ 7 ], color );

		n_bmp_flush_transcopy( &s->chip_earth[ 0 ], &s->chip_earth[ 6 ] );
		n_bmp_flush_transcopy( &s->chip_earth[ 2 ], &s->chip_earth[ 7 ] );
	}

	n_bmp_free_fast( &bmp );


	return;
}

void
n_nn2_stage_brick_maker( n_nn2 *p, n_nn2_stage *s, n_bmp *bmp, CGFloat blend, int dark )
{

	u32 color_light = n_bmp_rgb_mac( 150,100, 50 );
	u32 color_dark  = n_bmp_rgb_mac(  90, 40, 10 );


	u32 color_base;
	u32 color[ 8 ];

	color_base = color_light;

	color[ 0 ] = color_light;
	color[ 1 ] = color_dark;
	color[ 2 ] = color_light;
	color[ 3 ] = color_dark;
	color[ 4 ] = n_bmp_blend_pixel( color_light, n_bmp_white, blend );
	color[ 5 ] = color_dark;
	color[ 6 ] = n_bmp_blend_pixel( color_light, n_bmp_white, blend );
	color[ 7 ] = color_dark;


	n_bmp brick_n;
	n_bmp brick_m;

	n_type_gfx f  = 1;
	n_type_gfx ff = f * 2;

	n_type_gfx sz = p->mapchip_unit; sz -= ff;
//NSLog( @"%d", sz );

	n_type_gfx bsx = sz / 2;
	n_type_gfx bsy = sz / 3;
	n_type_gfx mod = sz % 3;

	n_nn2_bmp_new( &brick_n, bsx, bsy     );
	n_nn2_bmp_new( &brick_m, bsx, bsy+mod ); // [!] : modulo

	n_bmp_flush( &brick_n, color_base );
	n_bmp_flush( &brick_m, color_base );

	n_bmp_ui_rectframe_drawedge( &brick_n, 1, color );
	n_bmp_ui_rectframe_drawedge( &brick_m, 1, color );
//n_nn2_debug_bmp_save( p, &brick_n );


	n_nn2_bmp_new( bmp, sz + ff, sz + ff ); //n_bmp_flush( bmp, n_bmp_white );
//NSLog( @"%d", sz + ff );

	n_bmp_fastcopy( &brick_n, bmp, 0,0,bsx,bsy,   0+f,f );
	n_bmp_fastcopy( &brick_n, bmp, 0,0,bsx,bsy, bsx+f,f );

	n_bmp_fastcopy( &brick_m, bmp, 0,0,bsx,bsy+mod, -(bsx/2)    +f,(bsy*1)+f );
	n_bmp_fastcopy( &brick_m, bmp, 0,0,bsx,bsy+mod,  (bsx/2)    +f,(bsy*1)+f );
	n_bmp_fastcopy( &brick_m, bmp, 0,0,bsx,bsy+mod,  (bsx/2)+bsx+f,(bsy*1)+f );

	n_bmp_fastcopy( &brick_n, bmp, 0,0,bsx,bsy,   0+f,mod+(bsy*2)+f );
	n_bmp_fastcopy( &brick_n, bmp, 0,0,bsx,bsy, bsx+f,mod+(bsy*2)+f );


	color[ 0 ] = n_bmp_black;
	color[ 1 ] = n_bmp_black;
	color[ 2 ] = n_bmp_black;
	color[ 3 ] = n_bmp_black;
	color[ 4 ] = n_bmp_black;
	color[ 5 ] = n_bmp_black;
	color[ 6 ] = n_bmp_black;
	color[ 7 ] = n_bmp_black;

	n_bmp_ui_rectframe_drawedge( bmp, 1, color );


	if ( dark ) { n_nn2_stage_dark_maker( p, bmp, dark );}


	n_bmp_free_fast( &brick_n );
	n_bmp_free_fast( &brick_m );


	return;
}

void
n_nn2_stage_block_maker( n_nn2 *p, n_nn2_stage *s, n_bmp *bmp, int dark )
{

	n_type_gfx sz = p->mapchip_unit;

	n_nn2_bmp_new( bmp, sz, sz );


	const int ratio = 33;

	u32 color_light = n_bmp_rgb_mac( 150,100, 50 );
	u32 color_dark  = n_bmp_rgb_mac(  90, 40, 10 );


	n_type_gfx o, oo;

	 o = 0;
	oo = o * 2;
	n_bmp_roundrect_ratio( bmp, o,o, sz-oo,sz-oo, n_bmp_black, ratio );

	 o = o + 1;
	oo = o * 2;
	n_bmp_roundrect_ratio( bmp, o,o, sz-oo,sz-oo, color_light, ratio );

	 o = o + 4;
	oo = o * 2;
	n_bmp_roundrect_ratio( bmp, o,o, sz-oo,sz-oo, color_dark , ratio );

	 o = o + 4;
	oo = o * 2;
	n_bmp_roundrect_ratio( bmp, o,o, sz-oo,sz-oo, color_light, ratio );


	if ( dark ) { n_nn2_stage_dark_maker( p, bmp, dark ); }


	return;
}

void
n_nn2_stage_rocks_maker( n_nn2 *p, n_nn2_stage *s, int dark )
{

	// [!] : parameter "dark" is not used


	n_bmp_free( &s->chip_rocks_1 );

	n_bmp_carboncopy( &s->chip_brick_1, &s->chip_rocks_1 );
	n_bmp_resizer
	(
		&s->chip_rocks_1,
		N_BMP_SX( &s->chip_rocks_1 ) * 2, 
		N_BMP_SY( &s->chip_rocks_1 ) * 2,
		n_bmp_white_invisible,
		N_BMP_RESIZER_TILE
	);

//n_nn2_debug_bmp_save( p, &s->chip_rocks );


	n_bmp_free( &s->chip_rocks_2 );

	n_bmp_carboncopy( &s->chip_brick_2, &s->chip_rocks_2 );
	n_bmp_resizer
	(
		&s->chip_rocks_2,
		N_BMP_SX( &s->chip_rocks_2 ) * 2, 
		N_BMP_SY( &s->chip_rocks_2 ) * 2,
		n_bmp_white_invisible,
		N_BMP_RESIZER_TILE
	);


	n_bmp_free( &s->chip_rocks_3 );

	n_bmp_carboncopy( &s->chip_brick_3, &s->chip_rocks_3 );
	n_bmp_resizer
	(
		&s->chip_rocks_3,
		N_BMP_SX( &s->chip_rocks_3 ) * 2, 
		N_BMP_SY( &s->chip_rocks_3 ) * 2,
		n_bmp_white_invisible,
		N_BMP_RESIZER_TILE
	);


	return;
}

void
n_nn2_stage_pass_thru_maker( n_nn2 *p )
{

	n_bmp *bmp = &p->pass_thru_bmp;


	n_bmp_new_fast( bmp, p->mapchip_unit, p->mapchip_unit );

	n_bmp_flush( bmp, n_bmp_white_invisible );

	n_type_gfx sx = N_BMP_SX( bmp );
	n_type_gfx sy = N_BMP_SY( bmp );

	u32        color;
	n_type_gfx o, oo;

	color = n_bmp_argb_mac( 222, 255,255,255 );
	 o = 0;
	oo = o * 2;
	n_bmp_roundrect_ratio( bmp, o,o,sx-oo,sy-oo, color, 50 );

	color = n_bmp_argb_mac(   0, 255,255,255 );
	 o = 3;
	oo = o * 2;
	n_bmp_roundrect_ratio( bmp, o,o,sx-oo,sy-oo, color, 50 );


	return;
}
/*
void
n_nn2_stage_hidden_maker( n_nn2 *p, n_nn2_stage *s, n_bmp *bmp, u32 color_iv, u32 color_bg, u32 color_fg )
{

	n_bmp_new_fast( bmp, p->mapchip_unit, p->mapchip_unit );

	n_bmp_flush( bmp, color_iv );

	n_type_gfx sx = N_BMP_SX( bmp );
	n_type_gfx sy = N_BMP_SY( bmp );

	n_type_gfx o, oo;

	 o = 0;
	oo = o * 2;
	n_bmp_roundrect_ratio( bmp, o,o,sx-oo,sy-oo, color_bg, 25 );

	 o = 2;
	oo = o * 2;
	n_bmp_roundrect_ratio( bmp, o,o,sx-oo,sy-oo, color_fg, 25 );


	return;
}
*/

void
n_nn2_stage_mountain_maker( n_nn2 *p )
{

	u32 outer = n_bmp_rgb_mac( 0,  0,  0 );
	u32 inner = n_bmp_rgb_mac( 0,100,100 );

	{
		n_bmp *bmp = &p->mountain_1;

		n_type_gfx sx = 400 / 3 * 2;
		n_type_gfx sy = 400;

		n_nn2_bmp_new( bmp, sx, sy );

		n_bmp_ui_roundframe( bmp, 0, 0, sx, sy * 2, sx / 2, 4, outer, inner );

		n_bmp_flush_antialias( bmp, 1.0 );
//n_nn2_debug_bmp_save( p, bmp );
	}

	{
		n_bmp *bmp = &p->mountain_2;

		n_type_gfx sx = 200 / 3 * 2;
		n_type_gfx sy = 200;

		n_nn2_bmp_new( bmp, sx, sy );

		n_bmp_ui_roundframe( bmp, 0, 0, sx, sy * 2, sx / 2, 4, outer, inner );

		n_bmp_flush_antialias( bmp, 1.0 );
//n_nn2_debug_bmp_save( p, bmp );
	}


	return;
}

void
n_nn2_stage_mountain_set( n_nn2 *p, n_nn2_stage *s, n_type_gfx tx )
{

	{
		n_bmp *bmp = &p->mountain_1;

		n_type_gfx sx = N_BMP_SX( bmp );
		n_type_gfx sy = N_BMP_SY( bmp );

		n_bmp_transcopy( bmp, &s->canvas_middleground, 0,0,sx,sy, tx, s->map_sy - sy );

		tx += sx;
	}

	{
		n_bmp *bmp = &p->mountain_2;

		n_type_gfx sx = N_BMP_SX( bmp );
		n_type_gfx sy = N_BMP_SY( bmp );

		tx -= sx / 2;

		n_bmp_transcopy( bmp, &s->canvas_middleground, 0,0,sx,sy, tx, s->map_sy - sy );
	}


	return;
}

void
n_nn2_dokan_label_maker( n_nn2 *p )
{

	const n_type_gfx circle_size = 24;

	const u32 color_bg = n_bmp_rgb_mac( 100,100,100 );
	const u32 color_mg = n_bmp_rgb_mac(  80, 80, 80 );
	const u32 color_fg = n_bmp_rgb_mac( 255,255,255 );


	n_gdi gdi; n_gdi_zero( &gdi );


	gdi.sx                  = circle_size;
	gdi.sy                  = circle_size;
	gdi.scale               = N_GDI_SCALE_AUTO;
	gdi.style               = N_GDI_DEFAULT;
	gdi.layout              = N_GDI_LAYOUT_HORIZONTAL;
	gdi.align               = N_GDI_ALIGN_CENTER;

	gdi.base                = n_posix_literal( "" );
	gdi.base_index          = 0;
	gdi.base_color_bg       = n_bmp_alpha_invisible_pixel( color_mg );
	gdi.base_color_fg       = n_bmp_alpha_invisible_pixel( color_mg );
	gdi.base_style          = N_GDI_BASE_DEFAULT;
	gdi.base_unit           = 0;

	gdi.text_font           = N_NN2_FONT_NAME_INDICATOR;
	gdi.text_size           = 18;
	gdi.text_style          = N_GDI_TEXT_BOLD | N_GDI_TEXT_MAC_NO_CROP;
	gdi.text_color_main     = color_fg;
	gdi.text_color_contour  = color_fg;
	gdi.text_color_shadow   = color_fg;
	gdi.text_fxsize1        = 0;
	gdi.text_fxsize2        = 0;


	int i = 0;
	n_posix_loop
	{
		n_posix_char str[ 100 ]; gdi.text = str;

		if ( i >= N_NN2_STAGE_MAX )
		{
			n_posix_sprintf( str, "?" );
		} else {
			n_posix_sprintf( str, "%d", i );
		}


		u32 bg = n_bmp_alpha_invisible_pixel( color_bg );


		n_type_gfx sx = N_BMP_SX( &p->dokan_bmp_top );
		n_type_gfx sy = N_BMP_SY( &p->dokan_bmp_top );

		n_nn2_bmp_new_internal( &p->dokan_label[ i ], sx, sy, bg );


		n_bmp bmp_1;
		n_nn2_bmp_new_internal( &bmp_1, circle_size, circle_size, bg );

		n_bmp_circle( &bmp_1, 0, 0, circle_size, circle_size, color_mg );


		n_bmp bmp_2; n_bmp_zero( &bmp_2 ); n_gdi_bmp( &gdi, &bmp_2 );


		n_type_gfx cx = n_game_centering( sx, circle_size );
		n_type_gfx cy = n_game_centering( sy, circle_size ) - 12;

		n_bmp_transcopy( &bmp_1, &p->dokan_label[ i ], 0, 0, circle_size, circle_size, cx, cy );
		n_bmp_transcopy( &bmp_2, &p->dokan_label[ i ], 0, 0, circle_size, circle_size, cx, cy );


		n_bmp_free_fast( &bmp_1 );
		n_bmp_free_fast( &bmp_2 );


		i++;
		if ( i >= N_NN2_DOKAN_LABEL_MAX ) { break; }
	}


	return;
}

void
n_nn2_treasure_maker( n_nn2 *p )
{
//NSLog( @"n_nn2_treasure_maker()" );

	const int degree_base = 90 / N_NN2_TREASURE_BMP_MAX;


	n_bmp bmp_body; n_bmp_zero( &bmp_body ); n_nn2_rc_load( @"rc/object/treasure/body", &bmp_body, p->scaler );
	n_bmp bmp_lid ; n_bmp_zero( &bmp_lid  ); n_nn2_rc_load( @"rc/object/treasure/lid" , &bmp_lid , p->scaler );

//n_nn2_debug_bmp_save( p, &bmp_body );

	if ( n_bmp_error( &bmp_body ) ) { return; }
	if ( n_bmp_error( &bmp_lid  ) ) { return; }


	n_type_gfx body_sx = N_BMP_SX( &bmp_body );
	n_type_gfx body_sy = N_BMP_SY( &bmp_body );

	n_type_gfx  lid_sx = N_BMP_SX( &bmp_lid  );

	n_type_gfx sx = body_sx + lid_sx;
	n_type_gfx sy = body_sy * 3;

	int i = 0;
	n_posix_loop
	{
		n_nn2_bmp_new( &p->treasure_bmp[ i ], sx, sy );

		n_bmp_transcopy( &bmp_body, &p->treasure_bmp[ i ], 0,0,body_sx,body_sy, 0, body_sy * 2 );

		n_bmp bmp; n_bmp_carboncopy( &bmp_lid, &bmp );
		n_bmp_matrix_rotate( &bmp, degree_base * i, n_bmp_white_invisible, 0 );
		n_nn2_crop( &bmp, n_bmp_white_invisible );

		n_type_gfx tx = 0;
		n_type_gfx ty = N_BMP_SY( &bmp ) - 1;
		n_posix_loop
		{
			u32 color; n_bmp_ptr_get_fast( &bmp, tx,ty, &color );
			if ( 0 != n_bmp_a( color ) ) { break; }

			tx++;
			if ( tx >= lid_sx ) { break; }
		}

		n_type_gfx bmpsx = N_BMP_SX( &bmp );
		n_type_gfx bmpsy = N_BMP_SY( &bmp );

		n_type_gfx xx = body_sx - tx; if ( i == 0 ) { xx = 0; }
		n_type_gfx yy = sy - body_sy - bmpsy;

		n_bmp_transcopy( &bmp, &p->treasure_bmp[ i ], 0,0,bmpsx,bmpsy, xx,yy );

		n_bmp_free_fast( &bmp );

		i++;
		if ( i >= ( N_NN2_TREASURE_BMP_MAX + 1 ) ) { break; }
	}


	return;
}

void
n_nn2_sand_maker( n_nn2 *p, n_nn2_stage *s )
{

	// [!] : for making a resource


	n_type_gfx u = 256;


	n_bmp bmp; n_nn2_bmp_new( &bmp, u, u );


	u32 f = n_bmp_rgb_mac( 200,150, 50 );
	u32 t = n_bmp_rgb_mac( 111,111,111 );

	n_bmp_flush_gradient( &bmp, f, t, N_BMP_GRADIENT_VERTICAL );


	int i = 0;
	n_posix_loop
	{
		n_type_gfx x = n_random_range( u );
		n_type_gfx y = n_random_range( u );

		n_bmp_ptr_set( &bmp, x,y, n_bmp_black );

		i++;
		if ( i >= 3333 ) { break; }
	}


	n_bmp_box( &bmp, 0,0,u,8, n_bmp_black );
	n_bmp_box( &bmp, 0,0,8,u, n_bmp_black );

	n_nn2_debug_bmp_save( p, &bmp );


	n_bmp_free_fast( &bmp );


	return;
}

