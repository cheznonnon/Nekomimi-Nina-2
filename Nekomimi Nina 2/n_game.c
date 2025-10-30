// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// Template File


// [!] : Usage
//
//	1 : replace your NSView (Custom View) to NonnonGame
//	2 : IB : right pane : "Custom Class", "Class", set "NonnonGame"
//	3 : modify behavior


// [!] : trouble shooter
//
//	[ drawRect is not called at redraw ]
//
//		a : call [_n_game display];
//		b : see your @property and connect(DnD) to UI on the Xib canvas
//		c : layer may cause




#ifndef _H_NONNON_MAC_NONNON_GAME
#define _H_NONNON_MAC_NONNON_GAME




#import <Cocoa/Cocoa.h>


#include "../nonnon/mac/_mac.c"
#include "../nonnon/mac/draw.c"
#include "../nonnon/mac/image.c"
#include "../nonnon/mac/sound.c"
#include "../nonnon/mac/window.c"

#include "../nonnon/mac/gamepad.c"


#include "../nonnon/neutral/time.c"
#include "../nonnon/neutral/bmp/ui/rectframe.c"
#include "../nonnon/neutral/bmp/ui/roundframe.c"


#include "../nonnon/game/helper.c"
#include "../nonnon/game/progressbar.c"
#include "../nonnon/game/transition.c"


#include "../nonnon/win32/gdi.c"




@interface NonnonGame : NSView
@end




static NonnonGame *n_nn2_global = NULL;




#include "nn2.c"




#define N_MAC_KEYCODE_NN2_DASH N_MAC_KEYCODE_UNDO // "Z"
#define N_MAC_KEYCODE_NN2_JUMP N_MAC_KEYCODE_CUT  // "X"


#define N_MAC_KEYCODE_U ( 32 )
#define N_MAC_KEYCODE_N ( 45 )
#define N_MAC_KEYCODE_H (  4 )
#define N_MAC_KEYCODE_J ( 38 )




#include "./input.c"
#include "./title.c"







#define n_type_timer n_type_real

n_posix_inline n_posix_bool
n_nn2_game_timer( n_type_timer *prv, n_type_real interval )
{

	n_type_timer  cur = CACurrentMediaTime() * 1000;
	n_type_timer msec = cur - (*prv);


	if ( msec >= interval )
	{
		(*prv) = cur;

		return n_posix_true;
	}


	return n_posix_false;
}




@interface NonnonGame ()

@end


@implementation NonnonGame {

	n_nn2  nn2;
	NSRect n_rect;

}




- (n_nn2*) n_nn2_global_instance_get
{
	return &nn2;
}




- (instancetype)initWithCoder:(NSCoder *)coder
{
//NSLog( @"initWithCoder : 1" );

	self = [super initWithCoder:coder];
	if ( self )
	{

		n_nn2_global = self;

		[[NSNotificationCenter defaultCenter]
			addObserver: self
			   selector: @selector( n_DidConnectNotification: )
			       name: GCControllerDidConnectNotification
			     object: nil
		];

		[[NSNotificationCenter defaultCenter]
			addObserver: self
			   selector: @selector( n_DidDisconnectNotification: )
			       name: GCControllerDidDisconnectNotification
			     object: nil
		];

		// [x] : needed : n_mac_image_window.backingScaleFactor : see awakeFromNib

	}


//NSLog( @"initWithCoder : 2" );

	return self;
}




- (void) n_launch
{

	n_nn2 *p = &nn2;

	p->interval_frame   = 1000 / 30;
	p->interval_display = 1000 / 60;

	// [x] : Sonoma : glitch prevention
	n_mac_timer_init_once( self, @selector( n_timer_method_launch ), 500 );

}

- (void) n_timer_method_launch
{
//NSLog( @"n_timer_method_launch" );

	n_mac_timer_init( self, @selector( n_timer_method1 ), 1 );
	n_mac_timer_init( self, @selector( n_timer_method2 ), 1 );

}




- (void) n_DidConnectNotification:(NSNotification *)notification
{
//NSLog( @"n_DidConnectNotification" );

	nn2.gamepad = n_mac_gamepad_init();

}

- (void) n_DidDisconnectNotification:(NSNotification *)notification
{
//NSLog( @"n_DidDisconnectNotification" );

	nn2.gamepad = nil;

}


- (void) n_mac_game_canvas_resize:(NSWindow*)window width:(n_type_gfx)sx height:(n_type_gfx)sy
{

	if ( sx == -1 ) { sx = nn2.sx; }
	if ( sy == -1 ) { sy = nn2.sy; }

	NSSize size = NSMakeSize( sx,sy );

	[window setContentMinSize:size];
	[window setContentMaxSize:size];

	n_rect = NSMakeRect( 0,0,sx,sy );

	[self setFrame:n_rect];

	[window setContentSize:size];

}




- (void) n_game_start
{

	// [!] : macOS Sonoma launching sequence
	//
	//	initWithCoder
	//	awakeFromNib
	//	applicationWillFinishLaunching
	//	drawRect
	//	applicationDidFinishLaunching
	//	n_timer_method

}




- (BOOL) isFlipped
{
	return NO;//YES;
}

- (void) n_timer_method1
{
//NSLog( @"n_timer_method" );


	n_nn2 *p = &nn2;
	//n_mac_image_imagerep_alias( p->rep, &p->canvas_main );
	//N_BMP_PTR( &p->canvas_main ) = (void*) [p->rep bitmapData];


//NSLog( @"%d", p->title_phase );


	// [!] : n_timer_method is called while inactive window

	static BOOL bgm_onoff = TRUE;

	if ( [n_mac_image_window isKeyWindow] == FALSE )
	{

		if ( bgm_onoff )
		{
//NSLog( @"1" );
			bgm_onoff = FALSE;
			n_nn2_sound_bgm_volume( p, 0 );
		}

		return;
	} else {

		if ( bgm_onoff == FALSE )
		{
			bgm_onoff = TRUE;
			if ( p->title_phase == 0 )
			{
//NSLog( @"2" );
				n_nn2_sound_bgm_volume( p, 0.5 );
			}
		}

	}


	if ( p->title_phase )
	{
		n_nn2_title_go( p );

		[self display];

		return;
	}


	if ( p->weather_transition )
	{
		static BOOL ret = FALSE;
		ret = n_game_transition
		(
			 p->canvas,
			&p->transition_bmp_old,
			&p->transition_bmp_new,
			 1000,
			&p->transition_percent,
			 N_GAME_TRANSITION_MOSAIC
		);
		if ( ret )
		{
			p->weather_transition = FALSE;

			n_bmp_free( &p->transition_bmp_old );
			n_bmp_free( &p->transition_bmp_new );
		}

		[self display];

		return;
	}


	if ( p->costume_transition == 1 )
	{
		BOOL ret = n_game_transition
		(
			 p->canvas,
			&p->transition_bmp_old,
			&p->transition_bmp_mid,
			 1000,
			&p->transition_percent,
			 N_GAME_TRANSITION_CIRCLE
		);
		if ( ret )
		{
			p->costume_transition = 2;
		}

		[self display];

		return;
	} else
	if ( p->costume_transition == 2 )
	{
		BOOL ret = n_game_transition
		(
			 p->canvas,
			&p->transition_bmp_mid,
			&p->transition_bmp_new,
			 1000,
			&p->transition_percent,
			 N_GAME_TRANSITION_CIRCLE
		);
		if ( ret )
		{
			p->costume_transition = 0;

			n_bmp_free( &p->transition_bmp_old );
			n_bmp_free( &p->transition_bmp_mid );
			n_bmp_free( &p->transition_bmp_new );
		}

		[self display];

		return;
	}


	const u32 transition_msec = 1000;// * 5;

	if ( p->transition_phase == 1 )
	{
		BOOL ret = n_game_transition
		(
			 p->canvas,
			&p->transition_bmp_old,
			&p->transition_bmp_mid,
			    transition_msec,
			&p->transition_percent,
			 p->transition_type
		);
		if ( ret )
		{
//NSLog( @"phase 2" );
			p->transition_phase = 2;

			n_nn2_sound_bgm_stop( p );
		} else {
			CGFloat volume = 0.5 - ( 0.5 * ( p->transition_percent / 100 ) );
			n_nn2_sound_bgm_volume( p, volume );
		}

		[self display];

//if ( n_mac_keystate_get( N_MAC_KEYCODE_SPACE ) ) { p->debug_pause = TRUE; }

		return;
	} else
	if ( p->transition_phase == 2 )
	{
		BOOL ret = n_game_transition
		(
			 p->canvas,
			&p->transition_bmp_mid,
			&p->transition_bmp_new,
			    transition_msec,
			&p->transition_percent,
			 p->transition_type
		);
		if ( ret )
		{
//NSLog( @"phase done" );
			n_bmp_free( &p->transition_bmp_old );
			n_bmp_free( &p->transition_bmp_mid );
			n_bmp_free( &p->transition_bmp_new );

			p->stage = p->stage_next;

			n_nn2_sound_bgm_volume( p, 0.5 );
			n_nn2_sound_bgm_play( p );

			p->transition_phase = 0;
			p->transition_lock  = FALSE;

			p->dokan_onoff            = FALSE;
			p->dokan_disappear_offset = 0;

			p->bell_onoff = TRUE;

			p->hipdrop_state = 0;

			p->dokan_stage_number_shuffle = 2 + n_game_random( 4 );

			p->timeup = n_posix_tickcount() + N_NN2_STAGE_7_TIMEUP;

			p->dokan_blend_stop = -1;
		}

		[self display];

		return;
	}


	static CGFloat timer = 0;
	if ( n_nn2_game_timer( &timer, 12 ) )
	{
#ifdef DEBUG
		if ( p->debug_pause ) { return; }
#endif // #ifdef DEBUG

		n_nn2_input_event( p );

		n_nn2_loop( p );

		p->redraw_onoff = TRUE;
	}


	// [!] : for slow motion

	//n_posix_sleep( 100 );


}

- (void) n_timer_method2
{

	n_nn2 *p = &nn2;
	//n_mac_image_imagerep_alias( p->rep, &p->canvas_main );
	//N_BMP_PTR( &p->canvas_main ) = (void*) [p->rep bitmapData];


	static n_type_timer timer_frame = 0;
	if ( n_nn2_game_timer( &timer_frame, p->interval_frame ) )
	{
		n_sprite_animation( p->sprite_cur );
		n_chip_animation( p );
		n_nn2_stage_animation( p );

		n_object_kirakira_animation( p );
	}


	// [Needed] : this position is important : objects blink at transition

	if ( p->transition_phase   ) { return; }
	if ( p->weather_transition ) { return; }
	if ( p->costume_transition ) { return; }


	static n_type_timer timer = 0;
	if ( n_nn2_game_timer( &timer, p->interval_display ) )
	{
		if ( p->redraw_onoff )
		{
			n_nn2_draw( p );
			[self display];

			p->redraw_onoff = FALSE;
		}
	}

}

- (void)drawRect:(NSRect)rect
{
//NSLog( @"drawRect" );

//NSLog( @"%f %f", rect.size.width, rect.size.height );

//n_mac_draw_box( [NSColor blackColor], n_rect );

	n_nn2 *p = &nn2;

	//n_mac_image_nbmp_direct_draw_fast( p->canvas, &n_rect, n_posix_false );

	//n_mac_image_imagerep_sync( p->rep, p->canvas );

	// [Needed] : set every time
	n_mac_image_imagerep_alias_fast( p->rep, &p->canvas_main );

	n_mac_image_nbmp_direct_draw_faster( p->rep, &n_rect, n_posix_false );

}




- (void) n_sheet_init
{
	n_nn2 *p = &nn2;

	p->sheet_onoff = TRUE;

	[p->window beginSheet:p->sheet completionHandler:nil];
}

- (void) n_sheet_exit
{
	n_nn2 *p = &nn2;

	p->sheet_onoff      = FALSE;
	p->sheet_escape_key = FALSE;

	[p->window endSheet:p->sheet];
}




- (BOOL)acceptsFirstResponder
{
//NSLog(@"acceptsFirstResponder");

	return YES;
}

- (BOOL)becomeFirstResponder
{
//NSLog(@"becomeFirstResponder");

        return YES;
}

- (void) keyDown : (NSEvent*) event
{
//NSLog( @"Key Code = %d : Chars %@", event.keyCode, event.characters );

	// [x] : you cannot combine between alphabet keys and arrow keys like X + Left

	n_nn2 *p = &nn2;

	if ( p->title_phase )
	{
		if ( event.keyCode == N_MAC_KEYCODE_NN2_DASH )
		{
			// [!] : see title.c
		} else
		if ( event.keyCode == N_MAC_KEYCODE_NN2_JUMP )
		{
			// [!] : see title.c
		} else {
			p->sheet_escape_key = FALSE;
			[self n_sheet_init];
		}
	} else {
		if ( event.keyCode == N_MAC_KEYCODE_ESCAPE )
		{
			p->sheet_escape_key = TRUE;
			[self n_sheet_init];
		} 
	}

}

- (void) keyUp : (NSEvent*) event
{

	// [x] : you cannot combine between alphabet keys and arrow keys like X + Left

}



@end


#endif // _H_NONNON_MAC_NONNON_GAME


