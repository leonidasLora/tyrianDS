/* vim: set noet:
 *
 * OpenTyrian Classic: A modern cross-platform port of Tyrian
 * Copyright (C) 2007  The OpenTyrian Development Team
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include "opentyr.h"
#include "network.h"

#include "episodes.h"
#include "fonthand.h"
#include "helptext.h"
#include "joystick.h"
#include "keyboard.h"
#include "mainint.h"
#include "newshape.h"
#include "starfade.h"
#include "picload.h"
#include "varz.h"
#include "vga256d.h"

#include "SDL.h"

#include <assert.h>


#define NETWORK_VERSION       1            // increment whenever networking changes might create incompatability
#define NETWORK_PORT          1333         // UDP

#define NETWORK_PACKET_SIZE   256
#define NETWORK_PACKET_QUEUE  16

#define NETWORK_RETRY         640          // ticks to wait for packet acknowledgement before resending
#define NETWORK_RESEND        320          // ticks to wait before requesting unreceived game packet
#define NETWORK_KEEP_ALIVE    1600         // ticks to wait between keep-alive packets
#define NETWORK_TIME_OUT      16000        // ticks to wait before considering connection dead

int network_delay = 1 + 1;  // minimum is 1

char *network_opponent_host = 0;

Uint16 network_player_port = NETWORK_PORT,
       network_opponent_port = NETWORK_PORT;

char *network_player_name = "",
     *network_opponent_name = "";

UDPsocket socket;
IPaddress ip;

UDPpacket *packet_in, *packet_out, *packet_ack, *packet_temp;

Uint16 last_in_sync = 0, last_out_sync = 0, last_ack_sync = 0;
Uint32 last_in_tick = 0, last_out_tick = 0, last_ack_tick = 0;

UDPpacket *packet_state_in[NETWORK_PACKET_QUEUE] = {0}, *packet_state_out[NETWORK_PACKET_QUEUE] = {0};

Uint16 last_state_in_sync = 0, last_state_out_sync = 0;
Uint32 last_state_in_tick = 0, last_state_out_tick = 0;

bool quit = false;


JE_integer thisPlayerNum = 0;  /* Player number on this PC (1 or 2) */

JE_boolean haltGame = false;
JE_boolean netQuit = false;

JE_boolean moveOk;

/* Special Requests */
JE_boolean pauseRequest, skipLevelRequest, helpRequest, nortShipRequest;
JE_boolean yourInGameMenuRequest, inGameMenuRequest;

JE_boolean portConfigChange, portConfigDone;


int network_check( void )
{
	//STUB();
	return 0;
}

int network_acknowledge( Uint16 sync )
{
	//STUB();
	return 0;
}

void network_prepare( Uint16 type )
{
	//STUB();
}

int network_send( int len )
{
	//STUB();
	return 0;
}

bool network_is_sync( void )
{
	//STUB();
	return false;
}

bool network_is_alive( void )
{
	//STUB();
	return false;
}

void network_reset_keep_alive( void )
{
	//STUB();
}

bool network_keep_alive( void )
{
	//STUB();
	return false;
}


void network_state_prepare( void )
{
	//STUB();
}

int network_state_send( void )
{
	//STUB();
	return 0;
}

bool network_state_update( void )
{
	//STUB();
	return false;
}

bool network_state_is_latest( void )
{
	//STUB();
	return false;
}

bool network_state_is_reset( void )
{
	//STUB();
	return false;
}

void network_state_reset( void )
{
	//STUB();
}


int network_connect( void )
{
	//STUB();
	return 0;
}

void network_tyrian_halt( int err, bool attempt_sync )
{
	//STUB();
}

int network_init( void )
{
	//STUB();
	return 0;
}

void network_packet_copy( UDPpacket *dst, UDPpacket *src )
{
	//STUB();
}


void JE_clearSpecialRequests( void )
{
	//STUB();
}

