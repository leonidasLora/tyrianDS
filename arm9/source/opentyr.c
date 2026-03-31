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

#ifdef TARGET_NDS
#include <nds.h>
#include <fat.h>
#include "sound_ipc.h"
//#define DESMUME 1
#endif

#include "opentyr.h"

#include "config.h"
#include "episodes.h"
#include "error.h"
#include "fonthand.h"
#include "helptext.h"
#include "joystick.h"
#include "jukebox.h"
#include "keyboard.h"
#include "loudness.h"
#include "mainint.h"
#include "mtrand.h"
#include "musmast.h"
#include "network.h"
#include "newshape.h"
#include "nortsong.h"
#include "nortvars.h"
#include "pallib.h"
#include "params.h"
#include "picload.h"
#include "scroller.h"
#include "setup.h"
#include "shpmast.h"
#include "starfade.h"
#include "tyrian2.h"
#include "varz.h"
#include "vga256d.h"

#include "SDL.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#if TARGET_NDS
extern JE_boolean hate_scaling; /* tyrian2.c */
#endif

const JE_byte shapereorderlist[7] = {1, 2, 5, 0, 3, 4, 6};

const char *opentyrian_str = "OpenTyrian";
const char *opentyrian_menu_items[] =
{
	"About OpenTyrian",
#if TARGET_NDS
	"Toggle Horiz Scaling",
#else
	"Toggle Fullscreen",
#endif
	/* "Play Destruct", */
	"Jukebox",
#if TARGET_NDS
	"Play Destruct",
	"Super Arcade Mode",
#endif
	"Return to Main Menu"
};

/* zero-terminated strncpy */
char *strnztcpy( char *to, char *from, size_t count )
{
	to[count] = '\0';
	return strncpy(to, from, count);
}

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
/* endian-swapping fread */
size_t efread( void *buffer, size_t size, size_t num, FILE *stream )
{
	size_t i, f = fread(buffer, size, num, stream);

	switch (size)
	{
		case 2:
			for (i = 0; i < num; i++)
			{
				((Uint16 *)buffer)[i] = SDL_Swap16(((Uint16 *)buffer)[i]);
			}
			break;
		case 4:
			for (i = 0; i < num; i++)
			{
				((Uint32 *)buffer)[i] = SDL_Swap32(((Uint32 *)buffer)[i]);
			}
			break;
		case 8:
			for (i = 0; i < num; i++)
			{
				((Uint64 *)buffer)[i] = SDL_Swap64(((Uint64 *)buffer)[i]);
			}
			break;
		default:
			break;
	}

	return f;
}

/* endian-swapping fwrite */
size_t efwrite( void *buffer, size_t size, size_t num, FILE *stream )
{
	void *swap_buffer;
	size_t i, f;

	switch (size)
	{
		case 2:
			swap_buffer = malloc(size * num);
			for (i = 0; i < num; i++)
			{
				((Uint16 *)swap_buffer)[i] = SDL_SwapLE16(((Uint16 *)buffer)[i]);
			}
			break;
		case 4:
			swap_buffer = malloc(size * num);
			for (i = 0; i < num; i++)
			{
				((Uint32 *)swap_buffer)[i] = SDL_SwapLE32(((Uint32 *)buffer)[i]);
			}
			break;
		case 8:
			swap_buffer = malloc(size * num);
			for (i = 0; i < num; i++)
			{
				((Uint64 *)swap_buffer)[i] = SDL_SwapLE64(((Uint64 *)buffer)[i]);
			}
			break;
		default:
			swap_buffer = buffer;
			break;
	}

	f = fwrite(swap_buffer, size, num, stream);

	if (swap_buffer != buffer)
	{
		free(swap_buffer);
	}

	return f;
}
#endif

#if TARGET_NDS
void super_arcade_menu(void)
{
	int sel = 0;
	int maxSel = SA + 1;
	bool quit = false, fade_in = true;

	JE_fadeBlack(10);
	JE_loadPic(2, false); /* 2, 5, or 13? */

	JE_outTextAdjust(JE_fontCenter("Super Arcade Mode", FONT_SHAPES), 5, "Super Arcade mode", 15, -3, FONT_SHAPES, false);

	JE_showVGA();

	do
	{
		int i;
		for (i = 0; i <= maxSel; i++)
		{
			const char *text;
			if(i == 0)
				text = "Super Tyrian";
			else if(i == maxSel)
				text = "Return to OpenTyrian Menu";
			else
				text = superShips[i];
			
			JE_outTextAdjust(JE_fontCenter(text, SMALL_FONT_SHAPES),
			                 i * 16 + 32 + (i == maxSel ? 10 : (i > 0 ? 5 : 0)), text,
			                 15, (i != sel ? -4 : -2), SMALL_FONT_SHAPES, true);
		}
		
		JE_showVGA();
		
		if (fade_in)
		{
			fade_in = false;
			JE_fadeColor(20);
			wait_noinput(true, false, false);
		}

		tempW = 0;
		JE_textMenuWait(&tempW, false);

		if (newkey)
		{
			switch (lastkey_sym)
			{
				case SDLK_UP:
					sel--;
					if (sel < 0)
					{
						sel = maxSel;
					}
					JE_playSampleNum(CURSOR_MOVE);
					break;
				case SDLK_DOWN:
					sel++;
					if (sel > maxSel)
					{
						sel = 0;
					}
					JE_playSampleNum(CURSOR_MOVE);
					break;
				case SDLK_d: /* A button */
					if(sel == maxSel) { /* Return to main menu */
						quit = true;
						JE_playSampleNum(ESC);
					} else if(sel == 0) { /* Super Tyrian */
						JE_playSampleNum(37);
						JE_whoa();
						JE_clr256();
						JE_outText(10, 10, "Cheat codes have been disabled.", 15, 4);
						
						if (keysHeld() & KEY_R)
						{
							initialDifficulty = 6;
						} else {
							initialDifficulty = 8;
						}
						
						if (initialDifficulty == 8)
						{
							JE_outText(10, 20, "Difficulty level has been set to Lord of Game.", 15, 4);
						} else {
							JE_outText(10, 20, "Difficulty level has been set to Suicide.", 15, 4);
						}
						JE_outText(10, 30, "It is imperative that you discover the special codes.", 15, 4);
						if (initialDifficulty == 8)
						{
							JE_outText(10, 40, "(Next time, for an easier challenge hold down R.)", 15, 4);
						}
						JE_outText(10, 60, "Prepare to play...", 15, 4);
						
						char buf[10+1+15+1];
						snprintf(buf, sizeof(buf), "%s %s", miscTextB[4], pName[0]);
						JE_dString(JE_fontCenter(buf, FONT_SHAPES), 110, buf, FONT_SHAPES);
						JE_playSong(17);
						JE_playSampleNum(35);
						JE_showVGA();
						
						JE_wipeKey();
						
						wait_input(true, true, true);
						
						constantDie = false;
						JE_initEpisode(1);
						superTyrian = true;
						onePlayerAction = true;
						pItems[11] = 13;
						pItems[0] = 39;
						pItems[2] = 254;
						gameLoaded = true;
						difficultyLevel = initialDifficulty;
						score = 0;

						quit = true;
					} else { /* Super Arcade mode */
						int z = sel - 1;
						pItems[2] = z+1;
						pItems[11] = SAShip[z];
						JE_fadeBlack(10);
						if (JE_episodeSelect() && JE_difficultySelect())
						{
							/* Start special mode! */
							JE_fadeBlack(10);
							JE_loadPic(1, false);
							JE_clr256();
							JE_dString(JE_fontCenter(superShips[0], FONT_SHAPES), 30, superShips[0], FONT_SHAPES);
							JE_dString(JE_fontCenter(superShips[z+1], SMALL_FONT_SHAPES), 100, superShips[z+1], SMALL_FONT_SHAPES);
							tempW = ships[pItems[11]].shipgraphic;
							if (tempW != 1)
							{
								JE_drawShape2x2(148, 70, tempW, shapes9);
							}
							
							JE_showVGA();
							JE_fadeColor(50);
							
							wait_input(true, true, true);
							
							twoPlayerMode = false;
							onePlayerAction = true;
							superArcadeMode = z+1;
							gameLoaded = true;
							score = 0;
							pItems[0] = SAWeapon[z][0];
							pItems[10] = SASpecialWeapon[z];
							if (z+1 == SA)
							{
								pItems[3] = 24;
								pItems[4] = 24;
							}
							difficultyLevel++;
							initialDifficulty = difficultyLevel;
						}

						quit = true;
					}
					break;
				case SDLK_SPACE: /* Select */
				case SDLK_s: /* B button */
					quit = true;
					JE_playSampleNum(ESC);
					return;
				default:
					break;
			}
		}
	} while (!quit);

	JE_fadeBlack(10);
}
#endif

void opentyrian_menu( void )
{
	int sel = 0;
	int maxSel = COUNTOF(opentyrian_menu_items) - 1;
	bool quit = false, fade_in = true;

	JE_fadeBlack(10);
	JE_loadPic(13, false); /* 2, 5, or 13? */

	JE_outTextAdjust(JE_fontCenter(opentyrian_str, FONT_SHAPES), 5, opentyrian_str, 15, -3, FONT_SHAPES, false);

	memcpy(VGAScreen2->pixels, VGAScreen->pixels, VGAScreen2->pitch * VGAScreen2->h);

	JE_showVGA();

	if (currentJukeboxSong == 0) currentJukeboxSong = 37; /* A Field for Mag */
	JE_playSong(currentJukeboxSong);
	
	do
	{
		memcpy(VGAScreen->pixels, VGAScreen2->pixels, VGAScreen->pitch * VGAScreen->h);
		
		int i;
		for (i = 0; i <= maxSel; i++)
		{
			const char *text = opentyrian_menu_items[i];
			
			JE_outTextAdjust(JE_fontCenter(text, SMALL_FONT_SHAPES),
			                 (i != maxSel) ? (i * 16 + 32) : 118, text,
			                 15, (i != sel ? -4 : -2), SMALL_FONT_SHAPES, true);
		}
		
		JE_showVGA();
		
		if (fade_in)
		{
			fade_in = false;
			JE_fadeColor(20);
			wait_noinput(true, false, false);
		}

		tempW = 0;
		JE_textMenuWait(&tempW, false);

		if (newkey)
		{
			switch (lastkey_sym)
			{
				case SDLK_UP:
					sel--;
					if (sel < 0)
					{
						sel = maxSel;
					}
					JE_playSampleNum(CURSOR_MOVE);
					break;
				case SDLK_DOWN:
					sel++;
					if (sel > maxSel)
					{
						sel = 0;
					}
					JE_playSampleNum(CURSOR_MOVE);
					break;
				case SDLK_RETURN:
#if TARGET_NDS
				case SDLK_d: /* A button */
#endif
					switch (sel)
					{
						case 0: /* About */
							JE_playSampleNum(SELECT);
							scroller_sine(about_text);
							memcpy(VGAScreen->pixels, VGAScreen2->pixels, VGAScreen->pitch * VGAScreen->h);
							JE_showVGA();
							fade_in = true;
							break;
						case 1: /* Fullscreen */
#if TARGET_NDS
							hate_scaling = !hate_scaling;
							iprintf("Horizontal screen scaling now\n%s on most screens.\n", (hate_scaling ? "disabled" : "enabled"));
#else
							fullscreen_enabled = !fullscreen_enabled;
							JE_initVGA256();
							JE_playSampleNum(SELECT);
#endif
							break;
						case 2: /* Jukebox */
							JE_playSampleNum(SELECT);
							JE_jukeboxGo();
							memcpy(VGAScreen->pixels, VGAScreen2->pixels, VGAScreen->pitch * VGAScreen->h);
							JE_showVGA();
							fade_in = true;
							break;
#if TARGET_NDS
						case 3: /* Destruct */
							JE_playSampleNum(SELECT);
							loadDestruct = true;
							quit = true;
							break;
						case 4: /* Super Arcade mode */
							JE_playSampleNum(SELECT);
							super_arcade_menu();
							JE_loadPic(13, false); /* 2, 5, or 13? */
							if(gameLoaded) {
								quit = true;
							} else {
								memcpy(VGAScreen->pixels, VGAScreen2->pixels, VGAScreen->pitch * VGAScreen->h);
								JE_showVGA();
								fade_in = true;
							}
							break;
#endif
						default: /* Return to main menu */
							quit = true;
							JE_playSampleNum(ESC);
							break;
					}
					break;
				case SDLK_ESCAPE:
#if TARGET_NDS
				case SDLK_SPACE: /* Select */
				case SDLK_s: /* B button */
#endif
					quit = true;
					JE_playSampleNum(ESC);
					return;
				default:
					break;
			}
		}
	} while (!quit);
}

int main( int argc, char *argv[] )
{
#ifdef TARGET_NDS
	// setup the subscreen as a standard console
	videoSetModeSub(MODE_0_2D | DISPLAY_BG0_ACTIVE);
	vramSetBankH(VRAM_H_SUB_BG);
	SUB_BG0_CR = BG_MAP_BASE(8);
	BG_PALETTE_SUB[255] = RGB15(31,31,31);
	consoleInitDefault((u16*)SCREEN_BASE_BLOCK_SUB(8), (u16*)CHAR_BASE_BLOCK_SUB(0), 16);

	//lcdMainOnBottom();

	// setup the FIFO
	REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_SEND_CLEAR;
	//irqSet(IRQ_FIFO_NOT_EMPTY, FifoHandler);
	//irqEnable(IRQ_FIFO_NOT_EMPTY);
	//REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_RECV_IRQ;

#ifdef DESMUME
	// set this to non-zero before calling SDL_Init() to enable Desmume hacks within SDL
	sdl_desmume_hack = 1;
#endif

	// init sound
	init_sound();

	// init FAT
	if(!fatInitDefault()) {
		iprintf("Unable to initialize media device!");
		return -1;
	}

	// install the guru meditation handler
	defaultExceptionHandler();
#endif

	mt_srand(time(NULL));

	if (SDL_Init(0))
	{
		printf("Failed to initialize SDL: %s\n", SDL_GetError());
	}

	/* JE_detectCFG(); YKS: Removed */

	printf("\nWelcome to... >> OpenTyrian v0.1 <<\n\n");

	printf("Copyright (C) 2007 The OpenTyrian Development Team\n\n");

	printf("This program comes with ABSOLUTELY NO WARRANTY.\n");
	printf("This is free software, and you are welcome to redistribute it\n");
	printf("under certain conditions.  See the file GPL.txt for details.\n\n");

	JE_paramCheck(argc, argv);

	JE_scanForEpisodes();

	recordFileNum = 1;
	playDemoNum = 0;
	playDemo = false;

	JE_loadConfiguration();

	JE_initVGA256();
	init_keyboard();

	/* TODO: Tyrian originally checked availible memory here. */

	if (scanForJoystick)
	{
		JE_joystickInit();
		if (joystick_installed)
		{
			printf("Joystick detected. %d %d\n", jCenterX, jCenterY);
		} else {
			printf("No joystick found.\n");
		}
	} else {
		printf("Joystick override.\n");
		joystick_installed = false;
	}

	if (mouseInstalled)
	{
		printf("Mouse Detected.   ");
		if (mouse_threeButton)
		{
			printf("Mouse driver reports three buttons.");
		}
		printf("\n");
	} else {
		printf("No mouse found.\n");
	}

	if (tyrianXmas)
	{
		dont_die = true;
		if (JE_getFileSize("tyrianc.shp") <= 0)
		{
			tyrianXmas = false;
		}
		/*if (JE_getFileSize("voicesc.shp") == 0) tyrianXmas = false;*/
		dont_die = false;
#ifndef TARGET_GP2X
		if (tyrianXmas)
		{
			printf("*****************************\n"
			       "Christmas has been detected.\n"
			       "  Activate Christmas? (Y/N)\n"
			       "*****************************\n");
			wait_input(true, true, true);
#if TARGET_NDS
			if (lastkey_sym != SDLK_a) /* Y button */
#else
			if (lastkey_sym != SDLK_y)
#endif
			{
				tyrianXmas = false;
			}
		} else {
			printf("Christmas is missing.\n");
		}
#endif /*TARGET_GP2X*/
	}

	/* Default Options */
	youAreCheating = false;
	smoothScroll = true;
	showMemLeft = false;
	playerPasswordInput = true;

#if TARGET_NDS
	/* Cosmetic change, since the DS port doesn't use SDL for audio */
	printf("Loading...\n");
#else
	printf("Initializing SDL audio...\n");
#endif
	JE_loadSong(1);

	if (noSound) /* TODO: Check if sound is enabled, handle either appropriately */
	{
		/* TODO: Do we actually need to do anything here? */
		/* JE_initialize(0, 0, 0, 0, 0); */
	} else {
		/* SYN: This code block doesn't really resemble the original, because the
		    underlying sound code is very different. I've left out some stuff that
		    checked hardware values and stuff here. */

		JE_initialize(0, 0, 0, 0, 0); /* TODO: Fix arguments */

		soundEffects = true; /* TODO: find a real way to give this a value */
		if (soundEffects)
		{
			JE_multiSampleInit(0, 0, 0, 0); /* TODO: Fix arguments */

			/* I don't think these messages matter, but I'll replace them with more useful stuff if I can. */
			/*if (soundEffects == 2) printf("SoundBlaster active");
			printf ("DSP Version ***\n");
			printf ("SB port ***\n");
			printf ("Interrupt ***\n");*/

			JE_loadSndFile();
		}

	}

	if (recordDemo)
	{
		printf("Game will be recorded.\n");
	}

	megaData1 = malloc(sizeof(*megaData1));
	megaData2 = malloc(sizeof(*megaData2));
	megaData3 = malloc(sizeof(*megaData3));

	newshape_init();
	JE_loadMainShapeTables();
	/* TODO JE_loadExtraShapes;*/  /*Editship*/

	JE_loadHelpText();
	/*debuginfo("Help text complete");*/
	
	JE_loadPals();
	
	SDL_LockSurface(VGAScreen);
	
	if (isNetworkGame)
	{
		if (network_init())
		{
			network_tyrian_halt(3, false);
		}
	}
	
	loadDestruct = false;
	stoppedDemo = false;
	
#if TARGET_NDS
	// Somehow SDL manages to fux0r up the console after it gets done doing its thing.
	// I haven't figured out what the problem is yet, so for now let's just re-init the console.
	// Also set up a bitmap layer on the subscreen, but don't enable it yet.
	// XXX: The bitmap layer overlaps the text layer for the console in VRAM, because I couldn't
	// get them to co-exist peacefully.  Since showing console text on top of a background image
	// isn't terribly useful, I just make sure to only use one layer at a time.
	videoSetModeSub(MODE_5_2D | DISPLAY_BG0_ACTIVE);
	vramSetBankC(VRAM_C_SUB_BG);
	SUB_BG0_CR = BG_MAP_BASE(8) | BG_TILE_BASE(0);
	SUB_BG2_CR = BG_BMP8_256x256 | BG_BMP_BASE(0);
	SUB_BG2_XDX = 1 << 8;
	SUB_BG2_XDY = 0;
	SUB_BG2_YDX = 0;
	SUB_BG2_YDY = 1 << 8;
	SUB_BG2_CX = 0;
	SUB_BG2_CY = 0;
	BG_PALETTE_SUB[255] = RGB15(31,31,31);
	consoleInitDefault((u16*)SCREEN_BASE_BLOCK_SUB(8), (u16*)CHAR_BASE_BLOCK_SUB(0), 16);

	iprintf("OpenTyrian DS v%s by Dopefish\n", NDS_VERSION);
	iprintf("http://vespenegas.com\n\n");

	// re-init the FIFO for communicating with the ARM7 (SDL init removes our IRQ handler)
	REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_SEND_CLEAR;
	irqSet(IRQ_FIFO_NOT_EMPTY, FifoHandler);
	irqEnable(IRQ_FIFO_NOT_EMPTY);
	REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_RECV_IRQ;
#endif

	JE_main();
	
	SDL_UnlockSurface(VGAScreen);
	
	JE_closeVGA256();
	
	return 0;
}
