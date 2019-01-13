#include "drums.h"
#include "main.h"
#include "midi.h"
#include "ff.h"
#include "stdlib.h"
#include "ads1256_test.h"
#include "tim.h"
#include "string.h"
#include "audio.h"
#include "SRAMDriver.h"
#include "tm_stm32f4_ili9341.h"
#include "joystick.h"
#include "menu.h"
#include "stm32f429i_discovery.h"

uint32_t drumBeatIndex;
__IO BOOL switch_buff;
__IO BOOL first_beat;

uint8_t key_to_drum_part[16][2] = {
		{Acoustic_Bass_Drum,L_FOOT},
		{Side_Stick,L_HAND},
		{Acoustic_Snare,L_HAND},
		{Cowbell,L_HAND},
		{Low_Floor_Tom,L_HAND},
		{High_Floor_Tom,L_HAND},
		{Low_Mid_Tom,L_HAND},
		{Hi_Mid_Tom,L_HAND},
		{High_Tom,L_HAND},
		{Closed_Hi_Hat,L_HAND},
		{Open_Hi_Hat,L_HAND},
		{Pedal_Hi_Hat,L_HAND},
		{Crash_Cymbal_1,L_HAND},
		{Ride_Cymbal_2,L_HAND},
		{Splash_Cymbal,L_HAND},
		{Chinese_Cymbal,L_HAND}
};


PatternData pattern_audio_map[MAX_PATTERNS + 1];
uint8_t drumBuffA[MAX_SUBBEATS * NUM_ALL_TRACKS];
uint8_t drumBuffB[MAX_SUBBEATS * NUM_ALL_TRACKS];
uint32_t drumEventTimes[MAX_SUBBEATS];
uint8_t * drumBuffReadPtr;
uint8_t * drumBuffWritePtr;

__IO PatternBeats pat1;
__IO PatternBeats pat2;
__IO PatternTimes tim1;
__IO PatternTimes tim2;
__IO PatternTimes *timptr;
__IO PatternBeats *patptr;

static void seekPattern(PatternData *pattern_audio_map,uint32_t ind){
	switch_buff = FALSE;
	SRAM_seekRead(pattern_audio_map[ind].sram_position,SRAM_SET);
	readSRAM((uint8_t *)&pat1,sizeof(PatternBeats));
	setPatternTime(&pat1,&tim1);
	readSRAM((uint8_t *)drumBuffA,tim1.subbeats * 5);

}

void drumAudioSync(){
	//looper.SampleOffset = pattern_audio_map[looper.StartPattern].sample_position;
	//sdram_pointer = looper.SampleOffset * 2;
	//looper.SampleCount = pattern_audio_map[looper.EndPattern + 1].sample_position;
}

void drumLoop(){
	uint32_t tmp = looper.StartPattern;
	switch_buff = FALSE;
	first_beat = FALSE;
	seekPattern(pattern_audio_map,looper.StartPattern);
	resetDrums();
	HAL_TIM_Base_Start_IT(&htim2);
	looper.Function = AUDIO_DRUMS;
	looper.DrumState = DRUMS_STARTED;
	//setStartEndPatterns(looper.StartPattern,looper.EndPattern);

	while(looper.DrumState == DRUMS_STARTED && looper.StartPattern < (looper.EndPattern + 1)){

			looper.StartLooper = TRUE;

			if(switch_buff == FALSE){
					updatePatternTime(&pat1,&tim1);
					timptr = &tim1;
					patptr = &pat1;
					drumBuffReadPtr = drumBuffA;
					menuShowStatus();
					if(looper.StartPattern == looper.EndPattern)
						goto wait_first_beat;
					readSRAM((uint8_t *)&pat2,sizeof(PatternBeats));
					setPatternTime(&pat2,&tim2);
					readSRAM((uint8_t *)drumBuffB,tim2.subbeats * NUM_ALL_TRACKS);
					switch_buff = TRUE;
				}
				else{
					updatePatternTime(&pat2,&tim2);
					timptr = &tim2;
					patptr = &pat2;
					drumBuffReadPtr = drumBuffB;
					if(looper.StartPattern == looper.EndPattern)
						goto wait_first_beat;
					readSRAM((uint8_t *)&pat1,sizeof(PatternBeats));
					setPatternTime(&pat1,&tim1);
					readSRAM((uint8_t *)drumBuffA,tim1.subbeats * NUM_ALL_TRACKS);
					switch_buff = FALSE;
				}

				wait_first_beat:
				sprintf(lcdline," Playing bar: %-4u",(unsigned int)(looper.StartPattern + 1));
				TM_ILI9341_Puts(10, 150, lcdline, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);

				while(first_beat == FALSE){
					if(looper.DrumState == DRUMS_PAUSED || looper.DrumState == DRUMS_STOPPED)
						goto end_drum_loop;
					continue;
				}

				first_beat = FALSE;
				if(looper.Recording == TRUE)
					pattern_audio_map[looper.StartPattern].channel_recorded[ACTIVE_CHANNEL_INDEX] = TRUE;
				looper.StartPattern++;

				if(looper.StartPattern == (looper.EndPattern + 1)){
					if(looper.Recording == TRUE){
						looper.Recording = FALSE;
						goto end_drum_loop;
					}

					looper.StartPattern = tmp;	// restore original start pattern number
					// setStartEndPatterns in read_sample and read_samples
					switch_buff = FALSE;
					first_beat = FALSE;
					seekPattern(pattern_audio_map,looper.StartPattern);

				}

		}


		end_drum_loop:
		stopDrums();
		looper.StartPattern = tmp;
		looper.StartLooper = FALSE;
		looper.Playback = FALSE;
		looper.Recording = FALSE;
		BSP_LED_Off(LED_GREEN);
		BSP_LED_Off(LED_RED);
}

void readDrums(uint32_t *numOfPatterns,uint32_t *numOfBytes,uint32_t *maxResolution){
	PatternBeats tmp;
	uint32_t header[3];		// number of patterns, number of bytes, max. resolution
	//uint32_t (*map)[2] = NULL;
	uint32_t currPat = 0;
	switch_buff = FALSE;
	first_beat = FALSE;

	//f_read(fil,header,sizeof(header),&bytesRead);
	SRAM_seekRead(0,SRAM_SET);
	readSRAM((uint8_t *)header,sizeof(header));
	*numOfBytes = header[HEADER_NUM_BYTES];
	*numOfPatterns = header[HEADER_NUM_PATTS];
	*maxResolution = header[HEADER_MAX_BEATS];

	if(*numOfPatterns == 0 || *maxResolution > MAX_SUBBEATS)
		return;

	// create memory map
	pattern_audio_map[0].sample_position = 0;
	pattern_audio_map[0].channel_recorded[_CH1] = FALSE;
	pattern_audio_map[0].channel_recorded[_CH2] = FALSE;
	for(currPat = 0; currPat < *numOfPatterns; currPat++){
		pattern_audio_map[currPat].channel_recorded[_CH1] = FALSE;
		pattern_audio_map[currPat].channel_recorded[_CH2] = FALSE;
		pattern_audio_map[currPat].sram_position = SRAM_readerPosition();
		readSRAM((uint8_t *)&tmp,sizeof(PatternBeats));
		readSRAM((uint8_t *)drumBuffA,tmp.beats * tmp.division * NUM_ALL_TRACKS);
		if(currPat < *numOfPatterns){
			uint32_t millis = BEAT_MILLIS(tmp.beattime);
			pattern_audio_map[currPat + 1].sample_position = pattern_audio_map[currPat].sample_position + (millis * tmp.beats * 15);
		}
	}

	//return (uint32_t *)map;

}

void startDrums(){
	HAL_TIM_Base_Start_IT(&htim2);
	looper.DrumState = DRUMS_STARTED;
}

void stopDrums(){
	looper.DrumState = DRUMS_STOPPED;
	HAL_TIM_Base_Stop_IT(&htim2);
	HAL_Delay(100);

}

void setPatternTime(__IO PatternBeats *p,__IO PatternTimes *t){
	uint32_t millis = BEAT_MILLIS(p->beattime);
	t->subbeats = p->beats * p->division;
	t->barDuration = p->beats * millis;
	t->remainder = t->barDuration % t->subbeats;
	t->subBeatDuration = t->barDuration / t->subbeats;
}

void updatePatternTime(__IO PatternBeats *p,__IO PatternTimes *t){
	uint32_t millis = BEAT_MILLIS(p->beattime + looper.timeIncrement) ;
	t->subbeats = p->beats * p->division;
	t->barDuration = p->beats * millis;
	t->remainder = t->barDuration % t->subbeats;
	t->subBeatDuration = t->barDuration / t->subbeats;
}



void midiDrumHandler(){
	uint32_t i;
	if(looper.DrumState != DRUMS_STARTED){
		return;
	}

	if(looper.Metronome == TRUE){
		if(midiDrumClock % 1000 == 0){
			playPercussion(NOTEON,Metronome_Click);
		}
	}

	if(midiDrumClock < timptr->barDuration){
		if(midiDrumClock % ((timptr->remainder > 0 && drumBeatIndex == NUM_ALL_TRACKS)?(timptr->subBeatDuration + timptr->remainder):timptr->subBeatDuration) == 0){
			for(i = drumBeatIndex; i < drumBeatIndex + NUM_DRUM_TRACKS; i++){
				if(drumBuffReadPtr[i] != 0 && looper.DrumState == DRUMS_STARTED)
					playPercussion(NOTEON,drumBuffReadPtr[i]);
			}

			if(drumBuffReadPtr[i] != 0 && looper.DrumState == DRUMS_STARTED)
				playBass(NOTEON,drumBuffReadPtr[i]);

			drumBeatIndex += NUM_ALL_TRACKS;

		}

		midiDrumClock++;
	}
	else{

		first_beat = TRUE;
		midiDrumClock = 0;
		drumBeatIndex = 0;
	}

}

void recordDrums(){
	PatternTimes pattim;
	uint32_t barMillis;
	midiMetronomePointer = 0;
	  pattim.barDuration = 4000;
	  pattim.subBeatDuration = 250;
	  pattim.subbeats = 16;
	  pattim.remainder = 0;
	  drumBuffReadPtr = drumBuffA;
	  drumBuffWritePtr = drumBuffB;
	  timptr = &pattim;
	  HAL_TIM_Base_Start_IT(&htim2);

	  while(TRUE){
		  while(looper.DrumState != DRUMS_STARTED)
		  	 continue;
		  BSP_LED_On(LED_GREEN);

		  looper.Metronome = TRUE;
		  while(midiDrumClock < 4000)
			  continue;
		  drumBuffB[midiMetronomePointer] = No_Event;
		  looper.DrumState = DRUMS_PAUSED;
		  BSP_LED_Off(LED_GREEN);
		  drumBeatIndex = 0;
		  midiMetronomePointer = 0;
		  while(drumBuffB[midiMetronomePointer] != No_Event){

			  for(barMillis = 0; barMillis < 4000; barMillis += 250){
				  if(drumEventTimes[midiMetronomePointer] >= barMillis && drumEventTimes[midiMetronomePointer] < (barMillis + 250)){
					  drumBeatIndex = barMillis / 250;
					  if(drumEventTimes[midiMetronomePointer] < (barMillis + 125))
						  drumBuffA[drumBeatIndex * 5] = drumBuffB[midiMetronomePointer];
					  else
						  drumBuffA[drumBeatIndex * 5 + 5] = drumBuffB[midiMetronomePointer];
				  }
			  }

			  midiMetronomePointer++;
		  }
		  drumBeatIndex = 0;
		  midiDrumClock = 0;
		  //while(TM_KEYPAD_Read() == TM_KEYPAD_Button_NOPRESSED)
		  	// continue;
		  looper.DrumState = DRUMS_STARTED;
	  }

}

void resetDrums(){
	midiDrumClock = 0;
	drumBeatIndex = 0;
}

