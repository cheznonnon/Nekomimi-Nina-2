//
//  AppDelegate.m
//  Nekomimi Nina 2
//
//  Created by のんのん２ on 2023/12/17.
//


#import "AppDelegate.h"


#include "n_game.c"




@interface NonnonSheetSubclass : NSWindow

@end


@implementation NonnonSheetSubclass


- (void) keyDown : (NSEvent*) event
{
//NSLog( @"Subclass : Key Code = %d : Chars %@", event.keyCode, event.characters );

	if ( event.keyCode == N_MAC_KEYCODE_RETURN )
	{
		[super keyDown:event];
	} else
	if ( event.keyCode == N_MAC_KEYCODE_SPACE )
	{
		[super keyDown:event];
	} else
	//if ( event.keyCode )
	{
		[n_nn2_global n_sheet_exit];
	}// else

}


@end




@interface AppDelegate ()

@property (strong) IBOutlet NSWindow *window;

@property (weak) IBOutlet NonnonGame *n_game;

@property (weak) IBOutlet NSWindow *n_sheet;

@end




@implementation AppDelegate


- (void)awakeFromNib
{
//NSLog( @"awakeFromNib 1" );

	n_mac_image_window = _window;
	n_gdi_scale_factor = n_mac_image_window.backingScaleFactor;

//if ( _window == nil ) { NSLog( @"nil" ); }


	[[NSNotificationCenter defaultCenter]
	      addObserver: self
		 selector: @selector( windowWillClose: )
		     name: NSWindowWillCloseNotification
		   object: nil
	];


	// [Needed] : see initWithCoder @ n_game.c

	[_window makeKeyWindow];


	n_nn2 *nn2 = [n_nn2_global n_nn2_global_instance_get];


	// [!] : trick : glitch at first time
	n_mac_keystate_get( 0 );

	n_nn2_zero( nn2 );
	n_nn2_init( nn2 );

	nn2->self = n_nn2_global;

	nn2->window = _window;
	nn2->sheet  = _n_sheet;


	n_nn2_init_canvas( nn2 );

	n_nn2_init_rc( nn2 );

	n_nn2_init_firstscreen( nn2 );

	if ( nn2->title_phase )
	{
		n_nn2_title_go( nn2 );
		[n_nn2_global display];
	}


	[_n_game n_mac_game_canvas_resize:_window width:-1 height:-1];


	[_n_game n_launch];


//NSLog( @"awakeFromNib 2" );
}




- (void) windowWillClose:(NSNotification *)notification
{
//NSLog( @"windowWillClose" );

	NSWindow *window = notification.object;
	if ( window == self.window )
	{
		[NSApp terminate:self];
	}
}




- (void)applicationWillFinishLaunching:(NSNotification *)aNotification
{
//NSLog( @"applicationWillFinishLaunching" );
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
//NSLog( @"applicationDidFinishLaunching" );

	[_n_game n_game_start];

}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
	// Insert code here to tear down your application
}


- (BOOL)applicationSupportsSecureRestorableState:(NSApplication *)app {
	return YES;
}


- (IBAction)n_nn2_menu_readme:(id)sender {
//NSLog( @"n_nn2_menu_readme" );

	NSString *helpFilePath = [[NSBundle mainBundle] pathForResource:@"nn2" ofType:@"html"];
	NSURL    *helpFileURL  = [NSURL fileURLWithPath:helpFilePath];

	[[NSWorkspace sharedWorkspace] openURL:helpFileURL];

}


- (IBAction)n_nn2_sheet_ok:(id)sender {

	[n_nn2_global n_sheet_exit];

	n_nn2 *p = [n_nn2_global n_nn2_global_instance_get];

	if ( p->game_onoff == FALSE )
	{
		n_nn2_title_start( p );
	}
}


@end
