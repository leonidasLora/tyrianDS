#ifndef _SOUND_IPC_H
#define _SOUND_IPC_H

#include <nds.h>
#include <stdio.h>

// structure that represents a particular sound channel
typedef struct
{
	const void *data;
	u32 len;
	u8 vol;
	u8 pan;
	u8 loop;
	u8 state;
	u16 sample_rate;
	u8 bit_rate;
	u8 priority;
} channel_t;

// shared structure between ARM7 and ARM9, representing the entire sound system
typedef struct
{
	channel_t channels[16];
	u8 master_volume;
	u8 PADDING[3];
} soundsys_t;

// the various possible states for each channel
#define STATE_NOT_PLAYING 0
#define STATE_NEW_SOUND 1
#define STATE_STILL_PLAYING 2
#define STATE_PLEASE_STOP 3
#define STATE_CHANGE_VOLUME 4
#define STATE_NEW_STREAM 6
#define STATE_STREAM_REBUFFER_TOP 7
#define STATE_STREAM_REBUFFER_BOTTOM 8

// commands used for communications between CPUs, sent via the FIFO
#define FIFO_CMD_SOUND_STRUCT_ADDRESS 1
#define FIFO_CMD_STREAM_NEEDS_REBUFFERING 2
#define FIFO_CMD_SLEEP_MODE 0x55443322
#define FIFO_CMD_WIFI_INIT 0x12345678
#define FIFO_CMD_WIFI_SYNC 0x87654321

// the DS only likes signed PCM data.  Sound usually comes in 16-bit signed, or 8-bit unsigned.
// This function will convert 8-bit unsigned to 8-bit signed, so the DS will like it better.
static inline void unsigned_to_signed(char *buf, int len)
{
	int i;
	for(i = 0; i < len; i++)
		buf[i] ^= 0x80;
}

#ifdef ARM9
// from sound.c
void enter_sleep_mode(void);
void FifoHandler(void);
int buffer_next_adpcm_block(void);
int start_streaming_music(const char *path);
int play_sound(const char *path, int priority);
void stop_sound(int channel);
void init_sound(void);
void sound_go(char *buf, int len, int freq, int bitrate, int chan);
void music_go(int freq, int bitrate);
void change_music_volume(int vol);
void queue_song_change(u8 song, u8 briefSong, int num_frames);
int check_for_song_change(void);

extern FILE *music_fp;

extern u8 prevSong;
extern u32 songChangeAt;

// from adpcm_decoder.c
int parse_wav(FILE *fp, int *sample_rate, int *data_offset);
int decode_adpcm(int bitrate, int start_of_pcm_data);
int decode_adpcm_block(void);
void music_enable_looping(int sec);

extern s16 *adpcm_buffer;
extern int adpcm_buffer_len;

extern int done_sleeping;
#endif

#endif
