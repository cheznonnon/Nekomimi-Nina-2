// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




BOOL
n_nn2_keystate_get( n_nn2 *p, int id )
{

	BOOL ret = n_mac_keystate_get( id );

	switch( id ) {

	case N_MAC_KEYCODE_ARROW_UP :

		if ( p->gamepad != nil )
		{
//NSLog( @"%f", p->gamepad.leftThumbstick.yAxis.value );
			ret |= p->gamepad.dpad.down.pressed;
			if ( p->gamepad.leftThumbstick.yAxis.value == 1.0 )
			{
				ret |= TRUE;
			}
		}

	break;

	case N_MAC_KEYCODE_ARROW_DOWN :

		if ( p->gamepad != nil )
		{
//NSLog( @"%f", p->gamepad.leftThumbstick.yAxis.value );
			ret |= p->gamepad.dpad.down.pressed;
			if ( p->gamepad.leftThumbstick.yAxis.value == -1.0 )
			{
				ret |= TRUE;
			}
		}

	break;

	case N_MAC_KEYCODE_ARROW_LEFT :

		if ( p->gamepad != nil )
		{
			ret |= p->gamepad.dpad.left.pressed;
			if ( p->gamepad.leftThumbstick.xAxis.value <= -1.0 )
			{
				ret |= TRUE;
			}
		}

	break;

	case N_MAC_KEYCODE_ARROW_RIGHT :

		if ( p->gamepad != nil )
		{
			ret |= p->gamepad.dpad.right.pressed;
			if ( p->gamepad.leftThumbstick.xAxis.value >=  1.0 )
			{
				ret |= TRUE;
			}
		}

	break;

	case N_MAC_KEYCODE_NN2_DASH :

		if ( p->gamepad != nil )
		{
			ret |= p->gamepad.buttonX.pressed;
			ret |= p->gamepad.buttonY.pressed;
		}

	break;

	case N_MAC_KEYCODE_NN2_JUMP :

		if ( p->gamepad != nil )
		{
			ret |= p->gamepad.buttonA.pressed;
			ret |= p->gamepad.buttonB.pressed;
		}

	break;

	} // switch


	return ret;
}




BOOL
n_nn2_input_event_lr( n_nn2 *p )
{
//return FALSE;

	if ( p->lr_lock ) { return FALSE; }

	if ( n_nn2_keystate_get( p, N_MAC_KEYCODE_ARROW_LEFT ) )
	{
//NSLog( @"Left" );
		p->input |=  N_NN2_INPUT_L;
	} else {
		p->input &= ~N_NN2_INPUT_L;
	}

	if ( n_nn2_keystate_get( p, N_MAC_KEYCODE_ARROW_RIGHT ) )
	{
//NSLog( @"Right" );
		p->input |=  N_NN2_INPUT_R;
	} else {
		p->input &= ~N_NN2_INPUT_R;
	}


	return p->input;
}

void
n_nn2_input_event_brake( n_nn2 *p )
{
//return;

	if ( p->jump_state )
	{
		//
	} else
	if ( p->brake_phase == N_NN2_DASH_BRAKE_NONE )
	{
		p->brake_phase     = N_NN2_DASH_BRAKE_INIT;
		p->brake_direction = p->dash_direction;
	}


	return;
}

void
n_nn2_input_event( n_nn2 *p )
{
//return;

	// [Mechanism]
	//
	//	1 : gamepad  : p->input gets zero for a while when L/R is changed
	//	2 : keyboard : L/R at the same time is possible


	static u32 last_input = 0;


#ifdef DEBUG

	// [!] : Numeric Keypad aka Ten Key

	if ( n_nn2_keystate_get( p, 91 ) )
	{
		p->stage->nina_y -= 16;
	} else
	if ( n_nn2_keystate_get( p, 84 ) )
	{
		p->stage->nina_y += 16;
	} else
	if ( n_nn2_keystate_get( p, 86 ) )
	{
		p->stage->nina_x -= 16;
	} else
	if ( n_nn2_keystate_get( p, 88 ) )
	{
		p->stage->nina_x += 16;
	}

	if ( n_mac_keystate_get( N_MAC_KEYCODE_SPACE ) )
	{
		p->debug_pause = TRUE;
	}

	if ( n_mac_keystate_get( N_MAC_KEYCODE_RETURN ) )
	{
		n_posix_sleep( 200 );
		n_nn2_weather_transition( p, p->weather_hour + 3 );
	}

#endif


	if ( p->brake_phase )
	{

		//

	} else
	if ( n_nn2_input_event_lr( p ) )
	{
//NSLog( @"%d", p->input );

		// [Needed] : skip zero input : important for brake


		// [!] : gamepads cannot be input by the both L/R at the same time

		if ( ( p->input & N_NN2_INPUT_L )&&( p->input & N_NN2_INPUT_R ) )
		{
//NSLog( @"Simul" );
			// [!] : keyboard only : use previous one
		} else
		if ( p->input & N_NN2_INPUT_L )
		{
//NSLog( @"Left : %d : %d", p->direction, p->dash_direction );

			if ( ( p->jump_reverse_onoff == FALSE )&&( p->jump_onoff ) )
			{
//NSLog( @"L : jump" );
				if ( p->jump_direction == N_NN2_DIRECTION_NONE ) { p->jump_direction = N_NN2_DIRECTION_LEFT; }
				if ( p->jump_direction != N_NN2_DIRECTION_LEFT ) { p->input = 0; }
			} else
			if ( p->dash_onoff == FALSE )
			{
				p->direction = N_NN2_DIRECTION_LEFT;
			} else
			if ( p->dash_phase == N_NN2_DASH_DONE )
			{
				if ( p->dash_direction == N_NN2_DIRECTION_RIGHT )
				{
//NSLog( @"Left Brake" );
					if ( ( last_input + 200 ) > n_posix_tickcount() )
					{
						n_nn2_input_event_brake( p );
					}
				}

				p->direction = p->dash_direction = N_NN2_DIRECTION_LEFT;
			} else {
				p->direction = N_NN2_DIRECTION_LEFT;
			}
		} else
		if ( p->input & N_NN2_INPUT_R )
		{
//NSLog( @"Right" );

			if ( ( p->jump_reverse_onoff == FALSE )&&( p->jump_onoff ) )
			{
//NSLog( @"R : jump" );
				if ( p->jump_direction == N_NN2_DIRECTION_NONE  ) { p->jump_direction = N_NN2_DIRECTION_RIGHT; }
				if ( p->jump_direction != N_NN2_DIRECTION_RIGHT ) { p->input = 0; }
			} else
			if ( p->dash_onoff == FALSE )
			{
				p->direction = N_NN2_DIRECTION_RIGHT;
			} else
			if ( p->dash_phase == N_NN2_DASH_DONE )
			{
				if ( p->dash_direction == N_NN2_DIRECTION_LEFT )
				{
//NSLog( @"Right Brake" );
					if ( ( last_input + 200 ) > n_posix_tickcount() )
					{
						n_nn2_input_event_brake( p );
					}
				} 

				p->direction = p->dash_direction = N_NN2_DIRECTION_RIGHT;
			} else {
				p->direction = N_NN2_DIRECTION_RIGHT;
			}
		}

	}


	// [Needed] : important : for brake

	if ( p->input ) { last_input = n_posix_tickcount(); }


	if ( n_nn2_keystate_get( p, N_MAC_KEYCODE_NN2_JUMP ) )
	{
		p->jump_onoff = TRUE;

		if ( p->jump_state == N_NN2_JUMP_STATE_FALL )
		{
			if ( p->object_handheld_onoff == FALSE )
			{
				p->jump_flow = TRUE;
			}
		}

		if ( p->object_handheld_sprite == &p->share->chick_sprite )
		{
			p->fly_phase = 1;
		}
	} else {
		p->jump_onoff = FALSE;
		p->jump_flow  = FALSE;

		p->jump_direction = N_NN2_DIRECTION_NONE;

		if ( p->object_handheld_sprite != &p->share->chick_sprite )
		{
			p->fly_phase = 0;
		}

		if ( p->jump_state == N_NN2_JUMP_STATE_DONE )
		{
			p->jump_state = N_NN2_JUMP_STATE_NONE;
		}
	}


	if ( n_nn2_keystate_get( p, N_MAC_KEYCODE_NN2_DASH ) )
	{
//n_nn2_debug_dash( p );

		if ( p->dash_onoff == FALSE )
		{

			p->dash_phase     = N_NN2_DASH_INIT;
			p->dash_direction = p->direction;
			p->dash_value     = 0;

			if ( p->swim_onoff )
			{
				p->nina_walk_step = p->powerup->swim_lr_walk;
			} else
			if ( p->jump_state )
			{
				p->nina_walk_step = p->powerup->jump_lr_dash;
			} else {
				// [!] : inherit current value
				p->nina_walk_step += N_NN2_STEP_INIT;
			}

		} else {

			if ( p->dash_direction != p->direction )
			{
				p->dash_phase     = N_NN2_DASH_INIT;
				p->dash_direction = p->direction;
				p->dash_value     = 0;
			}

			if ( p->dash_phase == N_NN2_DASH_INIT )
			{
				if ( p->dash_value > p->powerup->dash_value_max )
				{
					p->dash_phase = N_NN2_DASH_DONE;
				} else {
					p->dash_phase = N_NN2_DASH_INIT;
				}
			}

		}

		p->dash_onoff = TRUE;

	} else {
//NSLog( @"Dash Off" );

		if ( p->dash_onoff )
		{
			p->dash_onoff = FALSE;
			p->dash_phase = N_NN2_DASH_NONE;
			p->dash_value = 0;

			p->nina_walk_step = N_NN2_STEP_INIT;
		}

	}


	if ( n_nn2_keystate_get( p, N_MAC_KEYCODE_ARROW_DOWN ) )
	{
		p->input |=  N_NN2_INPUT_D;
	} else {
		p->input &= ~N_NN2_INPUT_D;
	}

	p->fly_down = ( p->input & N_NN2_INPUT_D );

	if ( p->dokan_ready )
	{
		//
	} else
	if ( p->object_handheld_sprite != NULL )
	{
		p->input &= ~N_NN2_INPUT_D;
	} else
	if ( p->input & N_NN2_INPUT_D )
	{
		if ( p->jump_state )
		{
			//
		} else
		if ( p->duck_onoff == FALSE )
		{
			p->duck_onoff = TRUE;
			n_sprite_rewind( &n_chara_sprite_duck );
		}
	} else {
		if ( p->duck_onoff )
		{
			p->duck_onoff = FALSE;

			p->unduck_onoff = TRUE;

			//p->sprite_cur = &n_chara_sprite_idle;
			//n_sprite_rewind( &n_chara_sprite_idle );

			p->sprite_cur = &n_chara_sprite_unduck;
			n_sprite_rewind( &n_chara_sprite_unduck );
		}
	}


	if ( p->input & N_NN2_INPUT_D )
	{
		//n_object_handheld_release( p );
	} else {
		if ( n_nn2_keystate_get( p, N_MAC_KEYCODE_NN2_DASH ) )
		{
			p->object_handheld_ready = TRUE;
		} else {
			if ( p->object_handheld_ready )
			{
				n_object_handheld_release( p );
			}
			p->object_handheld_ready = FALSE;
		}
	}


	//if ( 0 )
	{
//NSLog( @"%d", p->direction );

		if ( p->jump_state )
		{
			p->turn_onoff = FALSE;
			p->turn_direction = p->direction;
		} else
		//
		{
			if ( p->turn_direction != p->direction )
			{
//NSLog( @"Turned" );
				p->turn_onoff = TRUE;
				n_sprite_rewind( &n_chara_sprite_turn );
			}

			p->turn_direction = p->direction;
		}
	}


//NSLog( @"%d", p->input );
	return;
}

