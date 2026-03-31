#include <nds.h>
#include <stdlib.h>
#include <dswifi7.h>
#include "sound_ipc.h"

int old_master_volume;
int streaming_toggler = 0;
soundsys_t *sound_structure = NULL;
touchPosition first,tempPos;
static u32 wifi_init_pass = 0;
static u32 sound_init_pass = 0;

//---------------------------------------------------------------------------------
void enter_sleep_mode()
{
	u32 oldIE = REG_IE; // Save the current interrupt sate.
	swiChangeSoundBias(0, 0x400); // Turn the speaker down.

	int power = readPowerManagement(PM_CONTROL_REG); // Save current power state.
	writePowerManagement(PM_CONTROL_REG, PM_LED_CONTROL(1)); // Set sleep LED.

	REG_IE = IRQ_LID; // Register for the lid interrupt, and only that IRQ.

	// Power down till we get our interrupt.
	swiSleep();

	// Restore the interrupt state.
	REG_IF = ~0;
	REG_IE = oldIE;

	writePowerManagement(PM_CONTROL_REG, power); // Restore power state.

	swiChangeSoundBias(1, 0x400); // Turn the speaker up.

	// Tell the ARM9 to wake up (make sure it gets the message; dupe won't do anything)
	REG_IPC_FIFO_TX = FIFO_CMD_SLEEP_MODE;
	REG_IPC_FIFO_TX = FIFO_CMD_SLEEP_MODE;
}

//---------------------------------------------------------------------------------
void VcountHandler()
{
	static int lastbut = -1;
	
	uint16 but=0, x=0, y=0, xpx=0, ypx=0, z1=0, z2=0;

	but = REG_KEYXY;

	if (!( (but ^ lastbut) & (1<<6))) {
 
		tempPos = touchReadXY();

		if ( tempPos.x == 0 || tempPos.y == 0 ) {
			but |= (1 <<6);
			lastbut = but;
		} else {
			x = tempPos.x;
			y = tempPos.y;
			xpx = tempPos.px;
			ypx = tempPos.py;
			z1 = tempPos.z1;
			z2 = tempPos.z2;
		}
		
	} else {
		lastbut = but;
		but |= (1 <<6);
	}

	IPC->touchX			= x;
	IPC->touchY			= y;
	IPC->touchXpx		= xpx;
	IPC->touchYpx		= ypx;
	IPC->touchZ1		= z1;
	IPC->touchZ2		= z2;
	IPC->buttons		= but;
}

//---------------------------------------------------------------------------------
void VblankHandler(void)
{
	soundsys_t *sndsys = sound_structure;
	channel_t *c;
	u32 cr, i;

	if(wifi_init_pass > 1)
		Wifi_Update();

	if(sndsys != NULL) {
		if(sndsys->master_volume != old_master_volume) {
			old_master_volume = sndsys->master_volume;
			SOUND_MASTER_VOL = sndsys->master_volume;
		}

		for(i = 0, c = sndsys->channels; i < 16; i++, c++) {
			switch(c->state) {
				case STATE_NEW_SOUND: // new sample ready to begin playing
					SCHANNEL_TIMER(i) = SOUND_FREQ(c->sample_rate);
					SCHANNEL_SOURCE(i) = (u32) c->data;
					SCHANNEL_LENGTH(i) = c->len >> 2;
					cr = SCHANNEL_ENABLE | SOUND_VOL(c->vol) | SOUND_PAN(c->pan);

					if(c->bit_rate == 255)
						cr |= SOUND_FORMAT_ADPCM;
					else if(c->bit_rate == 16)
						cr |= SOUND_FORMAT_16BIT;
					else
						cr |= SOUND_FORMAT_8BIT;

					if(c->loop == 0)
						cr |= SOUND_ONE_SHOT;
					else
						cr |= SOUND_REPEAT;

					SCHANNEL_CR(i) = cr;
					c->state = STATE_STILL_PLAYING;
					break;

				case STATE_STILL_PLAYING: // still playing (check if done)
					if((SCHANNEL_CR(i) & SCHANNEL_ENABLE) == 0) {
						c->state = STATE_NOT_PLAYING;
						if(i == 15) {
							TIMER0_CR = 0;
							TIMER1_CR = 0;
						}
					}
					break;

				case STATE_PLEASE_STOP: // want to stop
					c->state = STATE_NOT_PLAYING;
					if(i == 15) {
						TIMER0_CR = 0;
						TIMER1_CR = 0;
					}
					SCHANNEL_CR(i) = 0;
					break;

				case STATE_CHANGE_VOLUME: // want to change volume, pan, or both
					SCHANNEL_VOL(i) = c->vol;
					SCHANNEL_PAN(i) = c->pan;
					c->state = STATE_STILL_PLAYING;
					break;

				case STATE_NEW_STREAM: // start playing streaming music
					SCHANNEL_TIMER(i) = SOUND_FREQ(c->sample_rate);
					SCHANNEL_SOURCE(i) = (u32) c->data;
					SCHANNEL_LENGTH(i) = (u32) c->len >> 2;
					SCHANNEL_REPEAT_POINT(i) = (u32) 0;
					cr = SCHANNEL_ENABLE | SOUND_VOL(c->vol) | SOUND_PAN(c->pan);

					// Note: streaming ADPCM doesn't really work, because the DS doesn't preserve the
					// decoding state when the buffer loops.  The only real way to solve this is to
					// software-decode the ADPCM, and send S8 or S16 data to the DS sound hardware.
					if(c->bit_rate == 255)
						//cr |= SOUND_FORMAT_ADPCM;
						cr |= SOUND_FORMAT_16BIT;
					else if(c->bit_rate == 16)
						cr |= SOUND_FORMAT_16BIT;
					else
						cr |= SOUND_FORMAT_8BIT;

					// even if it's a non-looping song, set this so streaming works properly
					cr |= SOUND_REPEAT;

					SCHANNEL_CR(i) = cr;

					TIMER0_DATA = SOUND_FREQ(c->sample_rate) * 2;
					TIMER0_CR = TIMER_DIV_1 | TIMER_ENABLE;

					TIMER1_DATA = 0x10000 - (c->len >> 1);
					TIMER1_CR = TIMER_CASCADE | TIMER_IRQ_REQ | TIMER_ENABLE;

					streaming_toggler = 0;

					c->state = STATE_STILL_PLAYING;
					break;
			}
		}
	}
}

//---------------------------------------------------------------------------------
void Timer1Handler(void)
{
	soundsys_t *sndsys = sound_structure;
	channel_t *ch;

	ch = &sndsys->channels[15];

	// alternate between filling either half of the buffer
	switch(streaming_toggler) {
		case 0:
			// we're halfway through the buffer; tell the ARM9 to refill the first half
			ch->state = STATE_STREAM_REBUFFER_TOP;
			REG_IPC_FIFO_TX = (int) FIFO_CMD_STREAM_NEEDS_REBUFFERING;
			REG_IPC_FIFO_TX = (int) 15;
			break;
		case 1:
			// we're all the way through the buffer; tell the ARM9 to refill the second half
			ch->state = STATE_STREAM_REBUFFER_BOTTOM;
			REG_IPC_FIFO_TX = (int) FIFO_CMD_STREAM_NEEDS_REBUFFERING;
			REG_IPC_FIFO_TX = (int) 15;
			break;
	}
	streaming_toggler = !streaming_toggler;
}

//---------------------------------------------------------------------------------
void FifoHandler(void) 
{
	u32 data;

	// handle WiFi stuff, and get address of sound structure from the ARM9 via the FIFO
	while ( !(REG_IPC_FIFO_CR & (IPC_FIFO_RECV_EMPTY)) ) {
		data = REG_IPC_FIFO_RX;
		if(wifi_init_pass == 1) {
			wifi_init_pass = data;
		} else if(sound_init_pass == 1) {
			sound_structure = (soundsys_t *) data;
			sound_init_pass = 2;
		} else if(data == FIFO_CMD_WIFI_INIT) {
			wifi_init_pass = 1;
		} else if(data == FIFO_CMD_WIFI_SYNC) {
			Wifi_Sync();
		} else if(data == FIFO_CMD_SLEEP_MODE) {
			enter_sleep_mode();
		} else if(data == FIFO_CMD_SOUND_STRUCT_ADDRESS) {
			sound_init_pass = 1;
		}
	}
}

//---------------------------------------------------------------------------------
void arm7_synctoarm9()
{
	REG_IPC_FIFO_TX = FIFO_CMD_WIFI_SYNC;
}

//---------------------------------------------------------------------------------
int main(int argc, char ** argv)
{
	readUserSettings();

	// Enable sound
	powerON(POWER_SOUND);
	writePowerManagement(PM_CONTROL_REG, ( readPowerManagement(PM_CONTROL_REG) & ~PM_SOUND_MUTE ) | PM_SOUND_AMP );
	SOUND_CR = SOUND_ENABLE | SOUND_VOL(0x7F);
	IPC->soundData = 0;

	// enable IRQs and FIFO
	irqInit();
	initClockIRQ();
	SetYtrigger(80);
	irqSet(IRQ_VBLANK, VblankHandler);
	irqSet(IRQ_VCOUNT, VcountHandler);
	irqSet(IRQ_TIMER1, Timer1Handler);
	irqSet(IRQ_WIFI, Wifi_Interrupt);
	irqSet(IRQ_FIFO_NOT_EMPTY, FifoHandler);
	REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_SEND_CLEAR;
	irqEnable(IRQ_VBLANK | IRQ_VCOUNT | IRQ_TIMER1 | IRQ_FIFO_NOT_EMPTY | IRQ_WIFI);
	REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_RECV_IRQ;

	// Keep the ARM7 idle, and wait for the signal to initialize WiFi
	while(1) {
		if(wifi_init_pass > 1) {
			Wifi_Init(wifi_init_pass);
			Wifi_SetSyncHandler(arm7_synctoarm9);
			break;
		}

		swiWaitForVBlank();
	}

	// infinite loop after initializing WiFi (assuming we ever do)
	while(1) {
		swiWaitForVBlank();
	}

	return 0;
}

