// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




void
n_nn2_title_gdi( n_bmp *bmp, n_posix_char *str, n_type_gfx sz )
{

	n_gdi gdi; n_gdi_zero( &gdi );


	gdi.sx                  = sz;
	gdi.sy                  = sz;
	gdi.scale               = N_GDI_SCALE_AUTO;
	gdi.style               = N_GDI_DEFAULT;
	gdi.layout              = N_GDI_LAYOUT_HORIZONTAL;
	gdi.align               = N_GDI_ALIGN_CENTER;

	gdi.base                = n_posix_literal( "" );
	gdi.base_index          = 0;
	gdi.base_color_bg       = n_bmp_white_invisible;
	gdi.base_color_fg       = n_bmp_white_invisible;
	gdi.base_style          = N_GDI_BASE_SOLID;
	gdi.base_unit           = 0;

	//gdi.frame_style         = N_GDI_FRAME_SIMPLE;

	gdi.text                = str;
	gdi.text_font           = N_NN2_FONT_NAME_TITLE;
	gdi.text_size           = sz;
	gdi.text_style          = N_GDI_TEXT_CONTOUR;
	gdi.text_color_main     = n_bmp_rgb_mac( 255,255,255 );
	gdi.text_color_contour  = n_bmp_rgb_mac(   0,150,200 );
	gdi.text_color_shadow   = n_bmp_rgb_mac(   0,150,200 );
	gdi.text_fxsize1        = 1;
	gdi.text_fxsize2        = 1;


	n_bmp_free( bmp );
	n_gdi_bmp( &gdi, bmp );


	return;
}

void
n_nn2_title_gdi_press_A_button( n_bmp *bmp, n_posix_char *str )
{

	n_gdi gdi; n_gdi_zero( &gdi );


	gdi.sx                  = 0;
	gdi.sy                  = 0;
	gdi.scale               = N_GDI_SCALE_AUTO;
	gdi.style               = N_GDI_DEFAULT;
	gdi.layout              = N_GDI_LAYOUT_HORIZONTAL;
	gdi.align               = N_GDI_ALIGN_CENTER;

	gdi.base                = n_posix_literal( "" );
	gdi.base_index          = 0;
	gdi.base_color_bg       = n_bmp_white_invisible;
	gdi.base_color_fg       = n_bmp_white_invisible;
	gdi.base_style          = N_GDI_BASE_SOLID;
	gdi.base_unit           = 0;

	//gdi.frame_style         = N_GDI_FRAME_SIMPLE;

	gdi.text                = str;
	gdi.text_font           = N_NN2_FONT_NAME_TITLE;
	gdi.text_size           = 16;
	gdi.text_style          = N_GDI_TEXT_CONTOUR;
	gdi.text_color_main     = n_bmp_rgb_mac( 255,255,255 );
	gdi.text_color_contour  = n_bmp_rgb_mac(   0,150,200 );
	gdi.text_color_shadow   = n_bmp_rgb_mac(   0,150,200 );
	gdi.text_fxsize1        = 1;
	gdi.text_fxsize2        = 1;


	n_bmp_free( bmp );
	n_gdi_bmp( &gdi, bmp );


	return;
}

void
n_nn2_title_start( n_nn2 *p )
{

	if ( p->game_onoff == FALSE )
	{
		p->game_onoff = TRUE;
		n_nn2_sound_effect_play( N_NN2_SOUND_START );
	}


	p->title_phase = 0;

	n_sprite_rewind( p->sprite_cur );

	n_bmp_free( &p->transition_bmp_old );
	n_bmp_carboncopy( p->canvas, &p->transition_bmp_old );


	return;
}

void
n_nn2_title_go( n_nn2 *p )
{

	const  n_posix_char *str = "Nekomimi Nina 2";
	static n_bmp         bmp[ 32 ];
	static n_bmp         press_A_button;
	static n_bmp         bmp_scanline;


	typedef struct {

		CGFloat value;

	} n_nn2_title_pop;

	static n_nn2_title_pop pop[ 32 ];
	static u32             pop_timer;


	if ( p->title_phase == 1 )
	{

		p->title_phase = 2;

		n_type_gfx chars = (n_type_gfx) strlen( str );
		n_type_gfx    sz = p->sx / chars;

		n_nn2_rc_load( @"rc/title/neko", &p->title_neko, 1 );
//NSLog( @"%d %d", sz, N_BMP_SX( &p->title_neko ) );

		n_type_gfx sx = N_BMP_SX( &p->title_neko );
		//n_type_gfx sy = N_BMP_SY( &p->title_neko );

		int i = 0;
		n_posix_loop
		{
			if ( str[ i ] != N_STRING_CHAR_SPACE )
			{
				n_bmp_zero( &bmp[ i ] );

				n_posix_char s[ 2 ] = { str[ i ], 0 };
				n_nn2_title_gdi( &bmp[ i ], s, sx / 2 );

				n_bmp_resizer( &bmp[ i ], sz+16,sz+16, n_bmp_white_invisible, N_BMP_RESIZER_CENTER );
			}

			pop[ i ].value = 0;
			pop_timer      = n_posix_tickcount();

			i++;
			if ( i >= chars ) { break; }
		}


		n_bmp_zero( &press_A_button );
		n_nn2_title_gdi_press_A_button( &press_A_button, "Press 🅐 button" );


		{
			n_bmp_zero( &bmp_scanline );
			n_bmp_new_fast( &bmp_scanline, 1,4 );
			n_bmp_flush( &bmp_scanline, n_bmp_white_invisible );

			n_bmp_box( &bmp_scanline, 0,2, 1,2, n_bmp_black );

			n_bmp_resizer( &bmp_scanline, p->sx, p->sy, 0, N_BMP_RESIZER_TILE );
		}

	}

	if ( p->title_phase == 2 )
	{

		extern void n_chip_draw( n_nn2 *p );
		n_chip_draw( p );

		extern void n_chip_draw_dokan( n_nn2 *p );
		n_chip_draw_dokan( p );


		n_type_gfx chars = (n_type_gfx) strlen( str );

		n_type_gfx sx = N_BMP_SX( &p->title_neko );
		n_type_gfx sy = N_BMP_SY( &p->title_neko );

		n_type_gfx x = ( p->sx % chars ) / 2;
		n_type_gfx y = ( p->sy - sy ) / 3 * 1;

		int i;


		i = 0;
		x = ( p->sx % chars ) / 2;
		n_posix_loop
		{
			if ( str[ i ] != N_STRING_CHAR_SPACE )
			{
				n_bmp_rasterizer( &p->title_neko, p->canvas, x,y, p->weather_gradient_color, FALSE );
			}

			x += sx;

			i++;
			if ( i >= chars ) { break; }
		}


		n_bmp_flush_blendcopy( &bmp_scanline, p->canvas, 0.9 );


		i = 0;
		x = ( p->sx % chars ) / 2;
		n_posix_loop
		{
			if ( str[ i ] != N_STRING_CHAR_SPACE )
			{
				CGFloat pos = n_nn2_flowy_effect( 7, pop[ i ].value );

				n_bmp_transcopy( &bmp[ i ], p->canvas, 0,0,sx,sy, x,y-pos );
//n_bmp_box( p->canvas, x,y,sx,sy, n_bmp_black );
			}

			x += sx;

			i++;
			if ( i >= chars ) { break; }
		}


		if ( n_game_timer( &pop_timer, 27 ) )
		{
			static int scan = 0;

			i = 0;
			n_posix_loop
			{
				pop[ i ].value += 0.015;

				if ( i == scan )
				{
					scan++;
					break;
				}

				i++;
				if ( i >= chars ) { break; }
			}
		}


		static BOOL start = FALSE;
		static int  phase = -1;

		{
			n_type_gfx sx = N_BMP_SX( &press_A_button );
			n_type_gfx sy = N_BMP_SY( &press_A_button );

			n_type_gfx x = ( p->sx - sx ) / 2;
			n_type_gfx y = ( p->sy - sy ) / 3 * 2;

			static u32     timer = 0;
			static CGFloat blend = 0.0;

			if ( phase == -1 )
			{
				phase = 0;
				timer = n_posix_tickcount();
			} else
			if ( phase == 0 )
			{
				if ( n_game_timer( &timer, 2222 ) )
				{
					phase = 1;
				}
			} else
			if ( phase == 1 )
			{
				if ( n_game_timer( &timer, 20 ) )
				{
					blend += 0.015;
					if ( blend > 0.25 )
					{
						blend = 0.25;
						if ( start )
						{
							n_nn2_title_start( p );
						} else {
							phase = 2;
						}
					}
				}
			} else
			if ( phase == 2 )
			{
				if ( n_game_timer( &timer, 20 ) )
				{
					blend -= 0.015;
					if ( blend < 0.00 ) { blend = 0.00; phase = 0; }
				}
			}

			CGFloat pos = n_nn2_flowy_effect( 100, blend ) * 0.01;

			n_bmp_blendcopy( &press_A_button, p->canvas, 0,0,sx,sy, x,y, pos );
		}


		extern BOOL n_nn2_keystate_get( n_nn2 *p, int id );

		if (
			( n_nn2_keystate_get( p, N_MAC_KEYCODE_NN2_DASH ) )
			||
			( n_nn2_keystate_get( p, N_MAC_KEYCODE_NN2_JUMP ) )
		)
		{
			start = TRUE;
			phase = 1;
		}

	}


	return;
}

