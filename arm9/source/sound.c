#include <nds.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sound_ipc.h"

// global variables
#define MAX_BUFFER_SIZE (0x20000 * 2)
char music_buffer[MAX_BUFFER_SIZE];
int music_buffer_len;
FILE *music_fp = NULL;
int adpcm_bytes_done, adpcm_bytes_needed, start_of_pcm_data;
char *adpcm_decode_ptr;

u8 prevSong = 0, briefSong = 0;
u32 songChangeAt = 0;

// from nortsong.h
extern u8 currentSong, music_volume, sample_volume;
void JE_playSong (u16 songnum);

// local variables
static soundsys_t cache_sndsys;
static soundsys_t *sndsys = (soundsys_t *) ((int)&cache_sndsys + 0x400000); // point at the uncached mirror of RAM
static char *snd_buf[16] = {0};

// prototypes
static int find_free_channel(int priority);
static void channel_start_sound(int c, char *data, int len, int volume, int pan, int loop, int sample_rate, int bit_rate, int priority);
static void channel_stop(int c);
static int channel_done_playing(int c);
static void channel_change_volume(int c, int volume);
static void channel_change_pan(int c, int pan);
static void stop_all_sounds(void);
static int sound_init(void);
static void buffer_more_music(int channel);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Primary sound functions
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// puts the DS into low-power sleep mode until the ARM7 tells us to wake up
int done_sleeping;
void enter_sleep_mode(void)
{
	/* save the current IRQs, and turn off everything except the FIFO */
	unsigned long oldIE = REG_IE;
	REG_IE = IRQ_FIFO_NOT_EMPTY;

   	/* tell ARM7 to sleep */
	REG_IPC_FIFO_TX = FIFO_CMD_SLEEP_MODE;

	/* wait until the ARM7 tells us to wake up */
	done_sleeping = 0;
	while(!done_sleeping)
		swiWaitForIRQ();

	/* wait a bit longer until returning power */
	while(REG_VCOUNT != 0);
	while(REG_VCOUNT == 0);
	while(REG_VCOUNT != 0);

	/* restore power, and restore IRQs */
	powerON(POWER_LCD);
	REG_IE = oldIE;
}

// register this function as the handler for a FIFO interrupt
void FifoHandler(void)
{
	static int mode = 0;
	int data;

	while ( !(REG_IPC_FIFO_CR & (IPC_FIFO_RECV_EMPTY)) ) {
		data = (int) REG_IPC_FIFO_RX;

		if(mode > 0) {
			switch(mode) {
				case 1:
					if(data >= 0 && data <= 15 && sndsys->channels[data].state != 0) {
						buffer_more_music(data);
					} else {
						// invalid channel, or else the channel is not currently playing; ignore it.
					}
					break;
			}
			mode = 0;
		} else {
			switch(data) {
				case FIFO_CMD_STREAM_NEEDS_REBUFFERING:
					// the ARM7 is requesting we fill the streaming sound buffer with more data
					mode = 1;
					break;
				case FIFO_CMD_SLEEP_MODE:
					// the ARM7 is requesting that we wake up from sleep mode
					done_sleeping = 1;
					break;
#if 0
				case FIFO_CMD_WIFI_SYNC:
					// the ARM7 is requesting that we let dswifi do its business
					Wifi_Sync();
					break;
#endif
			}
		}
	}
}

// Decodes the next ADPCM block in the stream (if necessary), and adds it to the music buffer.
// This should be called at regular intervals:  preferably once per main loop, possibly at vblank.
int buffer_next_adpcm_block(void)
{
	extern s16 *adpcm_buffer;
	extern int adpcm_buffer_len;
	int i = 0;

	// if we're not currently streaming music, or we're streaming something besides ADPCM, this is a no-op.
	if(sndsys->channels[15].state != STATE_NOT_PLAYING && sndsys->channels[15].bit_rate == 255) {
		i = (adpcm_buffer_len < (adpcm_bytes_needed - adpcm_bytes_done) ? adpcm_buffer_len : (adpcm_bytes_needed - adpcm_bytes_done));
		if(i > 0) {
			if(decode_adpcm_block()) {
				//dmaCopy(adpcm_buffer, adpcm_decode_ptr, i);
				memcpy(adpcm_decode_ptr, adpcm_buffer, i);
				//iprintf("buf %d (0x%x)\n", i, adpcm_decode_ptr);
			}

			adpcm_decode_ptr += i;
			adpcm_bytes_done += i;
		}
	}

	return i;
}

// start streaming music from a wave file
int start_streaming_music(const char *path)
{
	char *start_of_music_buffer = (char*)((int)&music_buffer + 0x400000);
	int block_size, sample_rate, bitrate;

	// close old music file, if any
	if(music_fp) {
		fclose(music_fp);
		music_fp = NULL;
	}

	music_fp = fopen(path, "rb");
	if(music_fp) {
		bitrate = parse_wav(music_fp, &sample_rate, &start_of_pcm_data);
		if(bitrate == 8 || bitrate == 16) {
			// this is an ordinary PCM wave file; we can stream it without doing any fancy decoding
			music_buffer_len = MAX_BUFFER_SIZE;
		} else if(bitrate == 255) {
			// this is an ADPCM wave file; we can software decode it and stream from there
			block_size = decode_adpcm(bitrate, start_of_pcm_data);

			// special case for Camanis: the looping point is not at the beginning of the song.
			if(strcmp(path, "/tyrian/song_4.wav") == 0) {
				music_enable_looping(64);
			}

			music_buffer_len = 0;
			while(music_buffer_len + block_size * 2 < MAX_BUFFER_SIZE)
				music_buffer_len += block_size * 2; // make our buffer size an even multiple of the ADPCM block size
		} else {
			// this is not in any sound format we can handle
			fclose(music_fp);
			music_fp = NULL;
			start_of_pcm_data = 0;
			return -1;
		}

		// Ug. the channel's bitrate doesn't get set until we call channel_start_sound().
		// it needs to be set before we call buffer_more_music() in order to differentiate ADPCM from normal WAV.
		// but we don't want the ARM7 to start playing on the channel until we've put something in the buffer.
		channel_t *ch = &sndsys->channels[15];
		ch->bit_rate = bitrate;
		ch->state = STATE_NOT_PLAYING;

		buffer_more_music(15);
		buffer_next_adpcm_block();

		channel_start_sound(15, start_of_music_buffer, music_buffer_len, music_volume >> 1, 64, 1, sample_rate, bitrate, -10);
		return 15;
	} else {
		// unable to open file
		return -1;
	}
}

// plays a sound effect from a wave file
int play_sound(const char *path, int priority)
{
	int channel, sample_rate, bitrate, offset, length;
	FILE *fp;
	int i, done = 0;

	channel = find_free_channel(priority);
	if(channel > -1) {
		// free previous buffer, if any
		if(snd_buf[channel] != NULL) {
			free(snd_buf[channel]);
			snd_buf[channel] = NULL;
		}

		fp = fopen(path, "rb");
		if(fp) {
			bitrate = parse_wav(fp, &sample_rate, &offset);
			if(!(bitrate == 8 || bitrate == 16 || bitrate == 255)) {
				// not a format we understand
				fclose(fp);
				return -1;
			}

			if(bitrate == 8 || bitrate == 16) {
				// the next word contains the length; remaining data is the raw PCM sound
				fread(&length, 4, 1, fp);
				offset += 4;
			} else {
				// assume the rest of the file is ADPCM data blocks
				fseek(fp, 0, SEEK_END);
				length = ftell(fp) - offset;
				fseek(fp, offset, SEEK_SET);
			}

			snd_buf[channel] = malloc(length);
			if(!snd_buf[channel]) {
				// malloc failed
				fclose(fp);
				return -1;
			}

			// fill the buffer
			do {
				if(bitrate == 255) {
					i = fread(snd_buf[channel] + done, 1, length - done, fp);
					if(i > 0) done += i;
				} else {
					// a little fancy, so that we read in 2-byte chunks for 16-bit data (no particular reason)
					i = fread(snd_buf[channel] + done, bitrate, (length - done) * (bitrate / 8), fp);
					if(i > 0) done += i * (bitrate / 8);
				}
			} while(i > 0 && done < length);
			length = done;

			fclose(fp);

			if(bitrate == 8)
				unsigned_to_signed(snd_buf[channel], length);

			channel_start_sound(channel, snd_buf[channel], length, 127, 64, 0, sample_rate, bitrate, priority);
			return channel;
		} else {
			// unable to open file
			return -1;
		}
	} else {
		// no sound channels available
		return -1;
	}
}

// stops playing a sound on the specified channel
void stop_sound(int channel)
{
	channel_stop(channel);

	// free previous buffer, if any
	if(snd_buf[channel] != NULL) {
		free(snd_buf[channel]);
		snd_buf[channel] = NULL;
	}
}

// initializes the sound system
void init_sound(void)
{
	sound_init();
}

void sound_go(char *buf, int len, int freq, int bitrate, int chan)
{
	return channel_start_sound(chan, buf, len, sample_volume >> 1, 64, 0, freq, bitrate, 1);
}

void change_music_volume(int vol)
{
	channel_change_volume(15, vol);
}

void queue_song_change(u8 song, u8 newSong, int num_frames)
{
	extern u32 nds_frame_counter; /* from SDL */

	prevSong = song;
	briefSong = newSong;
	songChangeAt = nds_frame_counter + num_frames;
}

int check_for_song_change(void)
{
	extern u32 nds_frame_counter; /* from SDL */

	if(songChangeAt != 0 && nds_frame_counter >= songChangeAt) {
		songChangeAt = 0;
		if(currentSong == briefSong) {
			JE_playSong(prevSong);
			return prevSong;
		}
	}
	return -1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Internal sound functions
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// finds an available sound channel
static int find_free_channel(int priority)
{
	static int channel_to_steal = 0;
	int i;
	channel_t *c;

	// see if there's a free channel we can use
	for(i = 0, c = sndsys->channels; i < 15; i++, c++) {
		if(c->state != STATE_NOT_PLAYING) {
			//iprintf("Using channel %d\n", i);
			return i;
		}
	}

	// all channels are in use; try to find one at a lower priority than us, that we can steal
	// rotate around the list, so we don't always snipe the same channel (poor guy)
	for(i = channel_to_steal, c = sndsys->channels; i < 15; i++, c++) {
		if(c->priority <= priority) {
			//iprintf("Bumping channel %d\n", i);
			channel_to_steal++;
			channel_to_steal = (channel_to_steal == 14 ? 0 : channel_to_steal + 1);
			return i;
		}
	}
	for(i = channel_to_steal - 1, c = sndsys->channels; i >= 0; i++, c++) {
		if(c->priority <= priority) {
			//iprintf("Bumping channel %d\n", i);
			channel_to_steal++;
			channel_to_steal = (channel_to_steal == 14 ? 0 : channel_to_steal + 1);
			return i;
		}
	}

	// no channels are available; give up
	//iprintf("No channel available\n");
	return -1;
}

// starts playing a sample on the specified channel
static void channel_start_sound(int c, char *data, int len, int volume, int pan, int loop, int sample_rate, int bit_rate, int priority)
{
	channel_t *ch;

	if(c < 0 || c > 15)
		return;

	ch = &sndsys->channels[c];
	ch->data = data;
	ch->len = len;
	ch->pan = (pan > 127 ? 127 : pan);
	ch->vol = (volume > 127 ? 127 : volume);
	ch->loop = loop;
	ch->sample_rate = sample_rate;
	ch->bit_rate = bit_rate;
	ch->priority = abs(priority);
	ch->state = (priority < 0 ? STATE_NEW_STREAM : STATE_NEW_SOUND); // if priority == -1, consider this a streaming music channel

	DC_FlushRange(data, len);
}

// stops playing sound on the specified channel
static void channel_stop(int c)
{
	if(c < 0 || c > 15)
		return;

	sndsys->channels[c].state = STATE_PLEASE_STOP;
}

// tests if the specified channel is done playing its sample
static int channel_done_playing(int c)
{
	if(c < 0 || c > 15)
		return 1;

	//return sndsys->channels[c].state == STATE_PLEASE_STOP || sndsys->channels[c].state == STATE_NOT_PLAYING;
	return sndsys->channels[c].state == STATE_NOT_PLAYING;
}

// change the volume on a sound channel
static void channel_change_volume(int c, int volume)
{
	channel_t *ch;

	if(c < 0 || c > 15)
		return;

	ch = &sndsys->channels[c];
	if(ch->state == STATE_STILL_PLAYING || ch->state == STATE_CHANGE_VOLUME) {
		ch->vol = volume;
		ch->state = STATE_CHANGE_VOLUME;
	}
}

// change the panning on a sound channel
static void channel_change_pan(int c, int pan)
{
	channel_t *ch;

	if(c < 0 || c > 15)
		return;

	ch = &sndsys->channels[c];
	if(ch->state == STATE_STILL_PLAYING || ch->state == STATE_CHANGE_VOLUME) {
		ch->pan = pan;
		ch->state = STATE_CHANGE_VOLUME;
	}
}

// stop playing sound on all channels
static void stop_all_sounds(void)
{
	int i;
	channel_t *c;

	for(i = 0, c = sndsys->channels; i < 16; i++, c++) {
		if(c->state != STATE_NOT_PLAYING)
			c->state = STATE_PLEASE_STOP;
	}
}

// initialize sound system
static int sound_init(void)
{
	sndsys->master_volume = 127;

	// pass the address of our sound structure to the ARM7 via the FIFO
	REG_IPC_FIFO_TX = (int) FIFO_CMD_SOUND_STRUCT_ADDRESS;
	REG_IPC_FIFO_TX = (int) sndsys;
	return 0;
}

// buffers some music data
static void buffer_more_music(int channel)
{
	channel_t *ch;
	int i = 0;

	if(music_fp == NULL) {
		return;
	}

	ch = &sndsys->channels[channel];

	switch(ch->state) {
		case STATE_NOT_PLAYING: // initial load; fill entire buffer
		case STATE_PLEASE_STOP:
			adpcm_decode_ptr = (char *) ((int)&music_buffer[0] + 0x400000);
			adpcm_bytes_needed = music_buffer_len;
			break;
		case STATE_STREAM_REBUFFER_TOP: // fill top half of buffer
			adpcm_decode_ptr = (char *) ((int)&music_buffer[0] + 0x400000);
			adpcm_bytes_needed = music_buffer_len / 2;
			break;
		case STATE_STREAM_REBUFFER_BOTTOM: // fill bottom half of buffer
			adpcm_decode_ptr = (char *) ((int)&music_buffer[music_buffer_len / 2] + 0x400000);
			adpcm_bytes_needed = music_buffer_len / 2;
			break;
		default:
			return;
	};

	ch->state = STATE_STILL_PLAYING;

	//memset(adpcm_decode_ptr, 0, adpcm_bytes_needed);
	adpcm_bytes_done = 0;

	// if this is not an ADPCM stream, refill the buffer straight from the file.
	// if it is an ADPCM stream, filling the buffer is taken care of by buffer_next_adpcm_block()
	if(ch->bit_rate != 255) {
		// fill in the buffer
		do {
			if(feof(music_fp))
				fseek(music_fp, start_of_pcm_data, SEEK_SET);

			i = fread(adpcm_decode_ptr + adpcm_bytes_done, 1, adpcm_bytes_needed - adpcm_bytes_done, music_fp);
			if(i > 0)
				adpcm_bytes_done += i;
		} while(i > 0 && adpcm_bytes_done < adpcm_bytes_needed);

		if(ch->bit_rate == 8)
			unsigned_to_signed(adpcm_decode_ptr, adpcm_bytes_done);
	}
}

