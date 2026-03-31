#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sound_ipc.h"

struct WaveHeader {
	char		riff[4];		// 'RIFF'
	u32			size;			// Size of the file
	char		wave[4];		// 'WAVE'

	// fmt chunk
	char		fmt[4];			// 'fmt '
	u32			fmtSize;		// Chunk size
	u16			fmtFormatTag;	// Format of this file
	u16			fmtChannels;	// Num channels
	u32			fmtSamPerSec;	// Samples per second
	u32			fmtBytesPerSec; // Bytes per second
	u16			fmtBlockAlign;	// Block alignment
	u16			fmtBitsPerSam;	// Bits per sample

	u16			fmtExtraData;	// Number of extra fmt bytes
	u16			fmtExtra;		// Samples per block (only for IMA-ADPCM files)
} __attribute__ ((packed));

struct ChunkHeader {
	char 		name[4];	
	u32			size;
} __attribute__ ((packed));

struct BlockHeader {
	s16 		firstSample;
	char		stepTableIndex;
	char		reserved;
} __attribute__ ((packed));

const int stepTab[ 89 ] = {
7, 8, 9, 10, 11, 12, 13, 14,
16, 17, 19, 21, 23, 25, 28, 31,
34, 37, 41, 45, 50, 55, 60, 66,
73, 80, 88, 97, 107, 118, 130, 143,
157, 173, 190, 209, 230, 253, 279, 307,
337, 371, 408, 449, 494, 544, 598, 658,
724, 796, 876, 963, 1060, 1166, 1282, 1411,
1552, 1707, 1878, 2066, 2272, 2499, 2749, 3024,
3327, 3660, 4026, 4428, 4871, 5358, 5894, 6484,
7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899,
15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794,
32767 };

const int indexTab[ 16 ] = {
-1, -1, -1, -1, 2, 4, 6, 8,
-1, -1, -1, -1, 2, 4, 6, 8 };

static int stepTableIndex = 0;
static int prevSample = 0;
int wave_actual_data_start = 0;
int orig_wave_actual_data_start = 0;

static struct WaveHeader waveHeader;

s16 *adpcm_buffer = NULL;
int adpcm_buffer_len = 0;

// Parses a wave file.
// Returns the bitrate (8 for unsigned 8-bit, 16 for signed 16-bit, 255 for IMA-ADPCM, 0 on error)
// If sample_rate and data_offset are not NULL, they will be filled with the sample rate (in Hz),
// and the start of the actual sound data (offset in bytes from beginning of file).
int parse_wav(FILE *fp, int *sample_rate, int *data_offset)
{
	static struct ChunkHeader chunkHeader;

	if(!fp) {
		return 0;
	}

	if(fread(&waveHeader, sizeof(struct WaveHeader), 1, fp)) {
		// return the sample rate & bitrate if the caller passed in pointers to store them in
		if(sample_rate != NULL)
			*sample_rate = waveHeader.fmtSamPerSec;

		// skip all chunks until we find the DATA chunk
		if(fread(&chunkHeader, sizeof(struct ChunkHeader), 1, fp)) {
			while(strncmp(chunkHeader.name, "data", 4) != 0) {
				if(chunkHeader.size < 0 || chunkHeader.size > 10000) // sanity check
					return 0;

				fseek(fp, chunkHeader.size, SEEK_CUR);
				if(!fread(&chunkHeader, sizeof(struct ChunkHeader), 1, fp)) {
					// not enough input data to read the entire chunk, plus the next chunk header
					return 0;
				}
			}

			// this is the start of the actual sound data (offset from beginning of file)
			if(data_offset != NULL)
				*data_offset = ftell(fp);

			if(waveHeader.fmtFormatTag == 1) {
				// we've got ordinary PCM data
				return waveHeader.fmtBitsPerSam;
			} else if(waveHeader.fmtFormatTag == 0x11) {
				// we've got IMA-ADPCM data
				return 255;
			} else {
				// we've got some type of compression other than IMA-ADPCM
				return 0;
			}
		} else {
			// not enough input data to even read the first chunk header
			return 0;
		}
	} else {
		// not enough input data to even read the wave header
		return 0;
	}
}

// Begins software-decoding an ADPCM file for streaming.
int decode_adpcm(int bitrate, int start_of_pcm_data)
{
	if(!music_fp) {
		// need a file handle
		return 0;
	}

	stepTableIndex = 0;

	// if we still have an old decoding buffer, get rid of it
	if(adpcm_buffer != NULL) {
		free(adpcm_buffer);
		adpcm_buffer = NULL;
	}

	// make sure it's really ADPCM
	if(bitrate == 255) {
		wave_actual_data_start = start_of_pcm_data;
		orig_wave_actual_data_start = wave_actual_data_start;

		// allocate a new buffer to hold the decoded sound data
		adpcm_buffer_len = (waveHeader.fmtExtra) * 2; // 1 extra sample, just in case of buffer overflow
		adpcm_buffer = malloc(adpcm_buffer_len);
		if(adpcm_buffer == NULL) {
			// owned by malloc
			adpcm_buffer_len = 0;
			return 0;
		} else {
			// yay
			memset(adpcm_buffer, 0, adpcm_buffer_len);
			return adpcm_buffer_len;
		}
	} else {
		// not a valid ADPCM file
		return 0;
	}
}

// Decodes the next data block from an ADPCM file.
int decode_adpcm_block(void)
{
	static struct BlockHeader blockHeader;
	static int block[2048];
	int bytes_read;
	int i;

	if(!music_fp || !adpcm_buffer) return 0;

music_loop_around:
	if(feof(music_fp)) {
		fseek(music_fp, wave_actual_data_start, SEEK_SET);
	}

	fread(&blockHeader, sizeof(struct BlockHeader), 1, music_fp);

	stepTableIndex = blockHeader.stepTableIndex;
	prevSample = blockHeader.firstSample;

	adpcm_buffer[0] = (s16) prevSample;

	bytes_read = fread(block, 1, waveHeader.fmtBlockAlign - sizeof(struct BlockHeader), music_fp);

	if(bytes_read * 2 < waveHeader.fmtExtra - 1) /* if we read less than a full block of data, dump it and loop */
	   	goto music_loop_around;

	for(i = 0; i < bytes_read * 2; i++) {
		int word = block[i >> 3];
		int offset = 0;

		switch(7 - (i & 0x0007)) {
			case 0: offset = (word & 0xF0000000) >> 28; break;
			case 1: offset = (word & 0x0F000000) >> 24; break;
			case 2: offset = (word & 0x00F00000) >> 20; break;
			case 3: offset = (word & 0x000F0000) >> 16; break;
			case 4: offset = (word & 0x0000F000) >> 12; break;
			case 5: offset = (word & 0x00000F00) >> 8; break;
			case 6: offset = (word & 0x000000F0) >> 4; break;
			case 7: offset = (word & 0x0000000F); break;
		}

		int diff = 0;
		if(offset & 4)
			diff += stepTab[stepTableIndex];
		if(offset & 2)
			diff += stepTab[stepTableIndex] >> 1;
		if(offset & 1)
			diff += stepTab[stepTableIndex] >> 2;
		diff += stepTab[stepTableIndex] >> 3;

		if(offset & 8)
			diff = -diff;

		int newSample = prevSample + diff;
		if(newSample > 32767) newSample = 32767;
		if(newSample < -32768) newSample = -32768;

		prevSample = newSample;

		stepTableIndex += indexTab[offset];
		if(stepTableIndex > 88) stepTableIndex = 88;
		if(stepTableIndex < 0) stepTableIndex = 0;

		// FIXME: off-by-one error?
		adpcm_buffer[i + 1] = (s16) newSample;
	}

	return i;
}

// Sets the loop point to 'sec' seconds into the song, instead of the beginning of the song.
void music_enable_looping(int sec)
{
	int i;

	if(!music_fp || !adpcm_buffer) return;

	wave_actual_data_start = orig_wave_actual_data_start;

	i = (sec * waveHeader.fmtSamPerSec) / waveHeader.fmtExtra;
	wave_actual_data_start += waveHeader.fmtBlockAlign * i;

	i = (sec * waveHeader.fmtSamPerSec) % waveHeader.fmtExtra;
	if(i > waveHeader.fmtExtra / 2)
		wave_actual_data_start += waveHeader.fmtBlockAlign;
}
