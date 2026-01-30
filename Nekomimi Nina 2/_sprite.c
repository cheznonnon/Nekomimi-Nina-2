// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




// [!] : crash prevention : when zero-cleared : work fine silently

// [!] : higher value causes higher memory consumption : 32 will use 1 GB

#define N_SPRITE_MAX ( 16 )




#define N_SPRITE_OPTION_NONE  ( 0 )
#define N_SPRITE_OPTION_CREEP ( 1 )
#define N_SPRITE_OPTION_JUMP  ( 2 )
#define N_SPRITE_OPTION_ROCK  ( 3 )
#define N_SPRITE_OPTION_BIRD  ( 4 )




typedef struct {

	n_bmp *bmp[ N_SPRITE_MAX ];
	int     ox[ N_SPRITE_MAX ];
	int     oy[ N_SPRITE_MAX ];

	int    sound_id;

	int    frame_interval;

} n_sprite_single;


typedef struct n_sprite_struct {

	n_sprite_single obj[ N_SPRITE_MAX ];

	int             frame;
	int             index;
	BOOL            once;


	// [!] : for non-player objects

	int             option;


	n_type_gfx      x, y, sx, sy;
	int             direction;

	n_bmp           bmp_shared;

	n_bmp          *override_bmp;
	n_type_gfx      override_x;
	n_type_gfx      override_y;

	n_type_gfx      starting_x;

	n_type_gfx      ox, oy;

	n_type_gfx      margin_sx;
	n_type_gfx      margin_sy;

	BOOL            is_landed;

	int             jump_state;
	CGFloat         jump_max;
	CGFloat         jump_float;
	CGFloat         jump_pos_prv;

	n_type_gfx      move_step;
	n_type_gfx      fall_step;

	int             wink_count;

	BOOL            is_caught;

	int             collision_ud_detail;
	int             collision_lr_detail;
	BOOL            collision_lift_onoff;

	struct n_sprite_struct *sprite_lift;
	struct n_sprite_struct *sprite_stand;

	int             brick_move_type;
	int             brick_move_phase;
	n_type_gfx      brick_move_step;
	n_type_gfx      brick_move_max;

	n_type_gfx      bounce_offset;

	BOOL            throw_onoff;
	BOOL            invisible;

	BOOL            dokan_onoff;
	n_type_gfx      dokan_tx;
	n_type_gfx      dokan_ty;
	n_type_gfx      dokan_center;
	n_type_gfx      dokan_disappear_offset;

	u32             fish_timer;
	int             fish_phase;
	int             fish_move;

	n_type_gfx      stand_offset;

	int             stage_number;

} n_sprite;




#define n_sprite_zero( p ) n_memory_zero( p, sizeof( n_sprite ) )

void
n_sprite_debug( n_sprite *r )
{

NSLog( @"option %d", r->option );
NSLog( @"pos %d %d %d %d", r->x, r->y, r->sx, r->sy );
NSLog( @"override %d %d", r->override_x, r->override_y );
NSLog( @"direction %d", r->direction );
NSLog( @"margin %d %d", r->margin_sx, r->margin_sy );
NSLog( @"landed %d", r->is_landed );
NSLog( @"pos %d %f %f %f", r->jump_state, r->jump_max, r->jump_float, r->jump_pos_prv );
NSLog( @"step %d %d", r->move_step, r->fall_step );
NSLog( @"is_caught %d", r->is_caught );
NSLog( @"collision %d %d %d", r->collision_ud_detail, r->collision_lr_detail, r->collision_lift_onoff );
NSLog( @"throw_onoff %d", r->throw_onoff );
NSLog( @"invisible %d", r->invisible );
NSLog( @"stage_number %d", r->stage_number );


	return;
}

void
n_sprite_rewind( n_sprite *p )
{

	p->frame = 0;
	p->index = 0;
	p->once  = FALSE;


	return;
}

void
n_sprite_animation( n_sprite *p )
{

	p->frame++;

	return;
}
/*
void
n_sprite_offset( n_sprite *p, int ox, int oy )
{

	int i = 0;
	n_posix_loop
	{

		int j = 0;
		n_posix_loop
		{
			p->obj[ i ].ox[ j ] += ox;
			p->obj[ i ].oy[ j ] += oy;

			i++;
			if ( i >= N_SPRITE_MAX ) { break; }
		}

		i++;
		if ( i >= N_SPRITE_MAX ) { break; }
	}


	return;
}
*/
void
n_sprite_interval_change( n_sprite *s, int interval )
{

	int i = 0;
	n_posix_loop
	{

		s->obj[ i ].frame_interval = interval;

		i++;
		if ( i >= N_SPRITE_MAX ) { break; }
	}


	return;
}

void
n_sprite_set
(
	n_sprite *p,
	int       index,
	n_bmp    *bmp[ N_SPRITE_MAX ],
	int        ox[ N_SPRITE_MAX ],
	int        oy[ N_SPRITE_MAX ],
	int       sound_id,
	int       frame_interval
)
{

	if ( p == NULL ) { return; }


	int i = 0;
	n_posix_loop
	{

		if ( bmp[ i ] == NULL ) { break; }

		p->obj[ index ].bmp[ i ] = bmp[ i ];
		p->obj[ index ]. ox[ i ] =  ox[ i ];
		p->obj[ index ]. oy[ i ] =  oy[ i ];

		i++;
		if ( i >= N_SPRITE_MAX ) { break; }
	}


	p->obj[ index ].sound_id       = sound_id;
	p->obj[ index ].frame_interval = frame_interval;


	return;
}

