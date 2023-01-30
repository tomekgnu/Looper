#ifndef __DRUMS_H
#define __DRUMS_H

#include "main.h"
#include "stm32f429xx.h"


typedef struct{
	uint32_t sram_position;			// pattern offset in sram in bytes
	uint32_t sample_position;		// pattern sample offset
	BOOL channel_recorded[2];		// index 0: channel one, index 1: channel two
} PatternData;


typedef struct{
	uint32_t beats;		// number of beats in pattern
	uint32_t division; 	// number of smaller parts in single beat
	uint32_t beattime;	// beat time e.g. 60..120 (beats per minute)
	uint32_t repeat;	// not used yet
}PatternBeats;

typedef struct{
	uint32_t subbeats;			// number of beats * division
	uint32_t barDuration;		// pattern duration in milliseconds
	uint32_t remainder;			// remaining milliseconds of bar duration % subbeats
	uint32_t subBeatDuration;	// bar duration / subbeats

}PatternTimes;



void readDrums(uint32_t *numOfPatterns,uint32_t *numOfBytes,uint32_t *maxResolution);
void drumLoop();
void updateMetronome();
void drumAudioSync();
void resetDrums();
void startDrums();
void stopDrums();
void playDrums();
TM_KEYPAD_Button_t readDrumKeyboard(BOOL record);
TM_KEYPAD_Button_t readBassKeyboard(BOOL record);
void midiDrumHandler();
void initDrumBeats();
void setBarBeats(uint16_t bar,uint16_t beats);
void setPatternTime(__IO PatternBeats *p,__IO PatternTimes *t);
void updatePatternTime(__IO PatternBeats *p,__IO PatternTimes *t);
void record_drums();
void preview_drums();
void play_drums();
void select_rhythm_params();
void clear_drums();
void save_first(uint8_t *ptr);
void save_next(uint8_t *ptr);
void getRhythmParams(uint32_t *beats,uint32_t *division,uint32_t *beattime);

#define BEAT_MILLIS(x)	(60000 / (x))	// convert beat time signature (e.g. 60,120) to milliseconds
#define NUM_DRUM_TRACKS	4
#define NUM_ALL_TRACKS		5
#define DRUM_TIM_HDR_SIZE	16	// beats,division,beattime,repeat: each 4 bytes
#define DRUM_INSTR			5	// 4 drums + bass
#define L_HAND	0
#define R_HAND	1
#define L_FOOT	2
#define R_FOOT	3
#define BASS	4

// index constants for drum file header
#define HEADER_NUM_BYTES		0
#define HEADER_NUM_PATTS		1
#define HEADER_MAX_BEATS		2
#define MAX_PATTERNS			200
#define MAX_SUBBEATS			48


extern PatternData pattern_audio_map[MAX_PATTERNS + 1];
extern __IO PatternTimes *timptr;
extern uint8_t key_to_drum_part[][2];
extern uint8_t drumBuffA[MAX_SUBBEATS * NUM_ALL_TRACKS];
#endif
