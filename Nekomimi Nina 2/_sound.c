// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#define N_NN2_SOUND_MUTE     (  0 )
#define N_NN2_SOUND_START    (  1 )
#define N_NN2_SOUND_WALK     (  2 )
#define N_NN2_SOUND_BRAKE    (  3 )
#define N_NN2_SOUND_HIPDROP  (  4 )
#define N_NN2_SOUND_JUMP     (  5 )
#define N_NN2_SOUND_SUCK     (  6 )
#define N_NN2_SOUND_LAND     (  7 )
#define N_NN2_SOUND_DOKAN    (  8 )
#define N_NN2_SOUND_BELL     (  9 )
#define N_NN2_SOUND_OPEN     ( 10 )
#define N_NN2_SOUND_HEART    ( 11 )
#define N_NN2_SOUND_WATER    ( 12 )
#define N_NN2_SOUND_WATER_O  ( 13 )

#define N_NN2_SOUND_DOKAN_COUNT ( 3 )
#define N_NN2_SOUND_BELL_COUNT  ( 5 )

#define N_NN2_SOUND_BGM_0   ( 100 )
#define N_NN2_SOUND_BGM_1   ( 101 )
#define N_NN2_SOUND_BGM_2   ( 102 )
#define N_NN2_SOUND_BGM_3   ( 103 )
#define N_NN2_SOUND_BGM_4   ( 104 )
#define N_NN2_SOUND_BGM_5   ( 105 )
#define N_NN2_SOUND_BGM_6   ( 106 )
#define N_NN2_SOUND_BGM_7   ( 107 )
#define N_NN2_SOUND_BGM_8   ( 108 )


// [x] : broken : not played normally
/*
#define n_nn2_sound          n_mac_sound2
#define n_nn2_sound_init     n_mac_sound2_init2
//#define n_nn2_sound_init     n_mac_sound2_effect_init2
#define n_nn2_sound_play     n_mac_sound2_play
*/
#define n_nn2_sound          AVAudioPlayer
#define n_nn2_sound_init     n_mac_sound_init
#define n_nn2_sound_play     n_mac_sound_play
#define n_nn2_sound_stop     n_mac_sound_stop
#define n_nn2_sound_rewind   n_mac_sound_rewind
//




#define n_nn2_bgm n_mac_sound4

n_nn2_bgm*
n_nn2_bgm_init( NSString *resource_name, NSString *ext, int64_t fr, int64_t to )
{

	if ( N_NN2_SOUND_ONOFF == FALSE ) { return NULL; }

	return n_mac_sound4_init( resource_name, ext, fr, to );
}

void
n_nn2_bgm_play_internal( n_nn2_bgm *p )
{

	if ( N_NN2_SOUND_ONOFF == FALSE ) { return; }

	n_mac_sound4_play( p );

	return;
}

void
n_nn2_bgm_stop( n_nn2_bgm *p )
{

	if ( N_NN2_SOUND_ONOFF == FALSE ) { return; }

	n_mac_sound4_stop( p );

	return;
}

void
n_nn2_bgm_rewind( n_nn2_bgm *p )
{

	if ( N_NN2_SOUND_ONOFF == FALSE ) { return; }

	n_mac_sound4_rewind( p );

	return;
}

void
n_nn2_bgm_volume( n_nn2_bgm *p, CGFloat zero_one )
{

	if ( N_NN2_SOUND_ONOFF == FALSE ) { return; }

	n_mac_sound4_volume( p, zero_one );

	return;
}


// [!] : this is important to play

static n_nn2_sound *n_nn2_sound_mute;
static n_nn2_sound *n_nn2_sound_start;
static n_nn2_sound *n_nn2_sound_walk;
static n_nn2_sound *n_nn2_sound_brake;
static n_nn2_sound *n_nn2_sound_hipdrop;
static n_nn2_sound *n_nn2_sound_jump;
static n_nn2_sound *n_nn2_sound_suck;
static n_nn2_sound *n_nn2_sound_land;
static n_nn2_sound *n_nn2_sound_dokan[ N_NN2_SOUND_DOKAN_COUNT ];
static n_nn2_sound *n_nn2_sound_bell[ N_NN2_SOUND_BELL_COUNT ];
static n_nn2_sound *n_nn2_sound_open;
static n_nn2_sound *n_nn2_sound_heart;
static n_nn2_sound *n_nn2_sound_water;
static n_nn2_sound *n_nn2_sound_water_o;

static n_nn2_bgm   *n_nn2_sound_bgm_stage_0;
static n_nn2_bgm   *n_nn2_sound_bgm_stage_1;
static n_nn2_bgm   *n_nn2_sound_bgm_stage_2;
static n_nn2_bgm   *n_nn2_sound_bgm_stage_3;
static n_nn2_bgm   *n_nn2_sound_bgm_stage_4;
static n_nn2_bgm   *n_nn2_sound_bgm_stage_5;
static n_nn2_bgm   *n_nn2_sound_bgm_stage_6;
static n_nn2_bgm   *n_nn2_sound_bgm_stage_7;
static n_nn2_bgm   *n_nn2_sound_bgm_stage_8;

void
n_nn2_sound_rc_init( void )
{

	if ( N_NN2_SOUND_ONOFF == FALSE ) { return; }


	n_nn2_sound_mute    = n_nn2_sound_init( @"rc/sound/mute"   , @"wav" );
	n_nn2_sound_start   = n_nn2_sound_init( @"rc/sound/ting"   , @"wav" );
	n_nn2_sound_walk    = n_nn2_sound_init( @"rc/sound/walk"   , @"wav" );
	n_nn2_sound_brake   = n_nn2_sound_init( @"rc/sound/brake"  , @"wav" );
	n_nn2_sound_hipdrop = n_nn2_sound_init( @"rc/sound/hipdrop", @"wav" );
	n_nn2_sound_jump    = n_nn2_sound_init( @"rc/sound/sh"     , @"wav" );
	n_nn2_sound_suck    = n_nn2_sound_jump;
	n_nn2_sound_land    = n_nn2_sound_walk;
	n_nn2_sound_open    = n_nn2_sound_init( @"rc/sound/open"   , @"wav" );
	n_nn2_sound_heart   = n_nn2_sound_init( @"rc/sound/heart"  , @"wav" );
	n_nn2_sound_water   = n_nn2_sound_init( @"rc/sound/water"  , @"mp3" );
	n_nn2_sound_water_o = n_nn2_sound_init( @"rc/sound/cork"   , @"wav" );

	{
		int i = 0;
		n_posix_loop
		{
			n_nn2_sound_dokan[ i ] = n_nn2_sound_init( @"rc/sound/dokan", @"wav" );
			[n_nn2_sound_dokan[ i ] setVolume:0.8];

			i++;
			if ( i >= N_NN2_SOUND_DOKAN_COUNT ) { break; }
		}
	}

	{
		int i = 0;
		n_posix_loop
		{
			n_nn2_sound_bell[ i ] = n_nn2_sound_init( @"rc/sound/ting", @"wav" );

			i++;
			if ( i >= N_NN2_SOUND_BELL_COUNT ) { break; }
		}
	}

	n_nn2_sound_bgm_stage_0 = n_nn2_bgm_init( @"rc/0/bgm_0", @"wav", 0,  2702 );
	n_nn2_sound_bgm_stage_1 = n_nn2_bgm_init( @"rc/1/bgm_1", @"wav", 0,  8727 );
	n_nn2_sound_bgm_stage_2 = n_nn2_bgm_init( @"rc/2/bgm_2", @"wav", 0,  2702 );
	n_nn2_sound_bgm_stage_3 = n_nn2_bgm_init( @"rc/3/bgm_3", @"wav", 0,  1065 );
	n_nn2_sound_bgm_stage_4 = n_nn2_bgm_init( @"rc/4/bgm_4", @"wav", 0,  5050 );
	n_nn2_sound_bgm_stage_5 = n_nn2_bgm_init( @"rc/5/bgm_5", @"wav", 0,  3000 );
	n_nn2_sound_bgm_stage_6 = n_nn2_bgm_init( @"rc/6/bgm_6", @"wav", 0,  7800 );
	n_nn2_sound_bgm_stage_7 = n_nn2_bgm_init( @"rc/7/bgm_7", @"wav", 0, 38000 );
	n_nn2_sound_bgm_stage_8 = n_nn2_bgm_init( @"rc/8/bgm_8", @"wav", 0,  8727 );


	// [x] : it seems not to work

	n_nn2_bgm_volume( n_nn2_sound_bgm_stage_0, 0.5 );
	n_nn2_bgm_volume( n_nn2_sound_bgm_stage_1, 0.5 );
	n_nn2_bgm_volume( n_nn2_sound_bgm_stage_2, 0.5 );
	n_nn2_bgm_volume( n_nn2_sound_bgm_stage_3, 0.5 );
	n_nn2_bgm_volume( n_nn2_sound_bgm_stage_4, 0.5 );
	n_nn2_bgm_volume( n_nn2_sound_bgm_stage_5, 0.5 );
	n_nn2_bgm_volume( n_nn2_sound_bgm_stage_6, 0.5 );
	n_nn2_bgm_volume( n_nn2_sound_bgm_stage_7, 0.5 );
	n_nn2_bgm_volume( n_nn2_sound_bgm_stage_8, 0.5 );


	n_nn2_sound_play( n_nn2_sound_mute );


	return;
}

void
n_nn2_sound_effect_play( int id )
{

	if ( N_NN2_SOUND_ONOFF == FALSE ) { return; }


	if ( id == N_NN2_SOUND_MUTE )
	{
		n_nn2_sound_play( n_nn2_sound_mute );
	} else
	if ( id == N_NN2_SOUND_START )
	{
		n_nn2_sound_play( n_nn2_sound_start );
	} else
	if ( id == N_NN2_SOUND_WALK )
	{
		n_nn2_sound_play( n_nn2_sound_walk );
	} else
	if ( id == N_NN2_SOUND_BRAKE )
	{
//NSLog( @"!" );
		n_nn2_sound_play( n_nn2_sound_brake );
	} else
	if ( id == N_NN2_SOUND_HIPDROP )
	{
		n_nn2_sound_rewind( n_nn2_sound_hipdrop );
		n_nn2_sound_play  ( n_nn2_sound_hipdrop );
	} else
	if ( id == N_NN2_SOUND_JUMP )
	{
		n_nn2_sound_play( n_nn2_sound_jump );
	} else
	if ( id == N_NN2_SOUND_SUCK )
	{
		n_nn2_sound_play( n_nn2_sound_suck );
	} else
	if ( id == N_NN2_SOUND_LAND )
	{
		n_nn2_sound_play( n_nn2_sound_land );
	} else
	if ( id == N_NN2_SOUND_DOKAN )
	{
		static int i = 0;
		n_nn2_sound_play( n_nn2_sound_dokan[ i % N_NN2_SOUND_DOKAN_COUNT ] );
		i++;
	} else
	if ( id == N_NN2_SOUND_BELL )
	{
		static int i = 0;
		n_nn2_sound_play( n_nn2_sound_bell[ i % N_NN2_SOUND_BELL_COUNT ] );
		i++;
	} else
	if ( id == N_NN2_SOUND_OPEN )
	{
		n_nn2_sound_play( n_nn2_sound_open );
	} else
	if ( id == N_NN2_SOUND_HEART )
	{
		n_nn2_sound_play( n_nn2_sound_heart );
	} else
	if ( id == N_NN2_SOUND_WATER )
	{
		n_nn2_sound_rewind( n_nn2_sound_water );
		n_nn2_sound_play( n_nn2_sound_water );
	} else
	if ( id == N_NN2_SOUND_WATER_O )
	{
		n_nn2_sound_play( n_nn2_sound_water_o );
	}


	return;
}




void
n_nn2_sound_bgm_stop( n_nn2 *p )
{

	if ( p->stage == &n_nn2_stage_0 )
	{
		n_nn2_bgm_stop( n_nn2_sound_bgm_stage_0 );
	} else
	if ( p->stage == &n_nn2_stage_1 )
	{
		n_nn2_bgm_stop( n_nn2_sound_bgm_stage_1 );
	} else
	if ( p->stage == &n_nn2_stage_2 )
	{
		n_nn2_bgm_stop( n_nn2_sound_bgm_stage_2 );
	} else
	if ( p->stage == &n_nn2_stage_3 )
	{
		n_nn2_bgm_stop( n_nn2_sound_bgm_stage_3 );
	} else
	if ( p->stage == &n_nn2_stage_4 )
	{
		n_nn2_bgm_stop( n_nn2_sound_bgm_stage_4 );
	} else
	if ( p->stage == &n_nn2_stage_5 )
	{
		n_nn2_bgm_stop( n_nn2_sound_bgm_stage_5 );
	} else
	if ( p->stage == &n_nn2_stage_6 )
	{
		n_nn2_bgm_stop( n_nn2_sound_bgm_stage_6 );
	} else
	if ( p->stage == &n_nn2_stage_7 )
	{
		n_nn2_bgm_stop( n_nn2_sound_bgm_stage_7 );
	} else
	if ( p->stage == &n_nn2_stage_8 )
	{
		n_nn2_bgm_stop( n_nn2_sound_bgm_stage_8 );
	}


	return;
}

void
n_nn2_sound_bgm_volume( n_nn2 *p, CGFloat volume )
{

	if ( p->stage == &n_nn2_stage_0 )
	{
		n_nn2_bgm_volume( n_nn2_sound_bgm_stage_0, volume );
	} else
	if ( p->stage == &n_nn2_stage_1 )
	{
		n_nn2_bgm_volume( n_nn2_sound_bgm_stage_1, volume );
	} else
	if ( p->stage == &n_nn2_stage_2 )
	{
		n_nn2_bgm_volume( n_nn2_sound_bgm_stage_2, volume );
	} else
	if ( p->stage == &n_nn2_stage_3 )
	{
		n_nn2_bgm_volume( n_nn2_sound_bgm_stage_3, volume );
	} else
	if ( p->stage == &n_nn2_stage_4 )
	{
		n_nn2_bgm_volume( n_nn2_sound_bgm_stage_4, volume );
	} else
	if ( p->stage == &n_nn2_stage_5 )
	{
		n_nn2_bgm_volume( n_nn2_sound_bgm_stage_5, volume );
	} else
	if ( p->stage == &n_nn2_stage_6 )
	{
		n_nn2_bgm_volume( n_nn2_sound_bgm_stage_6, volume );
	} else
	if ( p->stage == &n_nn2_stage_7 )
	{
		n_nn2_bgm_volume( n_nn2_sound_bgm_stage_7, volume );
	} else
	if ( p->stage == &n_nn2_stage_8 )
	{
		n_nn2_bgm_volume( n_nn2_sound_bgm_stage_8, volume );
	}


	return;
}

void
n_nn2_sound_bgm_play( n_nn2 *p )
{

	if ( p->stage == &n_nn2_stage_0 )
	{
		n_nn2_bgm_rewind( n_nn2_sound_bgm_stage_0 );
		n_nn2_bgm_play_internal( n_nn2_sound_bgm_stage_0 );
	} else
	if ( p->stage == &n_nn2_stage_1 )
	{
		n_nn2_bgm_rewind( n_nn2_sound_bgm_stage_1 );
		n_nn2_bgm_play_internal( n_nn2_sound_bgm_stage_1 );
	} else
	if ( p->stage == &n_nn2_stage_2 )
	{
		n_nn2_bgm_rewind( n_nn2_sound_bgm_stage_2 );
		n_nn2_bgm_play_internal( n_nn2_sound_bgm_stage_2 );
	} else
	if ( p->stage == &n_nn2_stage_3 )
	{
		n_nn2_bgm_rewind( n_nn2_sound_bgm_stage_3 );
		n_nn2_bgm_play_internal( n_nn2_sound_bgm_stage_3 );
	} else
	if ( p->stage == &n_nn2_stage_4 )
	{
		n_nn2_bgm_rewind( n_nn2_sound_bgm_stage_4 );
		n_nn2_bgm_play_internal( n_nn2_sound_bgm_stage_4 );
	} else
	if ( p->stage == &n_nn2_stage_5 )
	{
		n_nn2_bgm_rewind( n_nn2_sound_bgm_stage_5 );
		n_nn2_bgm_play_internal( n_nn2_sound_bgm_stage_5 );
	} else
	if ( p->stage == &n_nn2_stage_6 )
	{
		n_nn2_bgm_rewind( n_nn2_sound_bgm_stage_6 );
		n_nn2_bgm_play_internal( n_nn2_sound_bgm_stage_6 );
	} else
	if ( p->stage == &n_nn2_stage_7 )
	{
		n_nn2_bgm_rewind( n_nn2_sound_bgm_stage_7 );
		n_nn2_bgm_play_internal( n_nn2_sound_bgm_stage_7 );
	} else
	if ( p->stage == &n_nn2_stage_8 )
	{
		n_nn2_bgm_rewind( n_nn2_sound_bgm_stage_8 );
		n_nn2_bgm_play_internal( n_nn2_sound_bgm_stage_8 );
	}


	return;
}

