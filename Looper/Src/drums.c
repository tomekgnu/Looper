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

uint32_t drumBeatIndex;
__IO BOOL switch_buff;
__IO BOOL first_beat;

uint8_t key_to_drum[16] = {
		Acoustic_Bass_Drum,
		Side_Stick,
		Acoustic_Snare,
		Cowbell,
		Low_Floor_Tom,
		High_Floor_Tom,
		Low_Mid_Tom,
		Hi_Mid_Tom,
		High_Tom,
		Closed_Hi_Hat,
		Open_Hi_Hat,
		Pedal_Hi_Hat,
		Crash_Cymbal_1,
		Ride_Cymbal_2,
		Splash_Cymbal,
		Chinese_Cymbal
};

uint8_t drumBuffA[MAX_SUBBEATS * NUM_ALL_TRACKS];
uint8_t drumBuffB[MAX_SUBBEATS * NUM_ALL_TRACKS];
uint8_t * drumBuffPtr;

__IO Pattern pat1;
__IO Pattern pat2;
__IO DrumTimes tim1;
__IO DrumTimes tim2;
__IO DrumTimes *timptr;
__IO Pattern *patptr;


extern TM_KEYPAD_Button_t Keypad_Button;

static void seekPattern(uint32_t (*map)[2],uint32_t ind){
	switch_buff = FALSE;
	SRAM_seekRead(map[ind][0],SRAM_SET);
	readSRAM((uint8_t *)&pat1,sizeof(Pattern));
	setPatternTime(&pat1,&tim1);
	readSRAM((uint8_t *)drumBuffA,tim1.subbeats * 5);
}

static uint32_t drumMenu(uint32_t (*map)[2],uint32_t currPat,uint32_t numOfPatterns,TM_KEYPAD_Button_t *key){
	BOOL input = FALSE;

	while (1){
			Keypad_Button = TM_KEYPAD_Read();
			if(Keypad_Button != TM_KEYPAD_Button_NOPRESSED){
				input = TRUE;
				*key = Keypad_Button;
				switch(Keypad_Button){
					case TM_KEYPAD_Button_5:	return currPat;
					case TM_KEYPAD_Button_0:	return 0;
					case TM_KEYPAD_Button_6:	if(currPat < (numOfPatterns - 1))
													currPat++;
												break;
					case TM_KEYPAD_Button_4:	if(currPat > 0)
													currPat--;
												break;
				}

			}
			else if(Active_Joystick() == TRUE){
				input = TRUE;
				JOYSTICK js = Read_Joystick();
				if(js.ypos > CENTER && currPat < (numOfPatterns - 1))
					currPat++;
				else if(js.ypos < CENTER && currPat > 0)
					currPat--;

				if(js.but == TRUE){
					if(looper.DrumState == DRUMS_READY){
						*key = TM_KEYPAD_Button_5;
						break;
					}
					if(looper.DrumState == DRUMS_STOPPED){
						looper.DrumState = DRUMS_READY;
						Wait_Joystick();
					}
				}

				Wait_Joystick();

			}

			if(input == TRUE){
				sprintf(lcdline,"%u bar ",(unsigned int)(currPat + 1));
				TM_ILI9341_Puts(10, 100, lcdline, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
				input = FALSE;
			}
		}


	return currPat;
}

static uint32_t drumLoop(uint32_t (*map)[2],uint32_t currPat,uint32_t numOfPatterns){
	seekPattern(map,currPat);
	switch_buff = FALSE;
	resetDrums();
	HAL_TIM_Base_Start_IT(&htim2);

	TM_ILI9341_DrawFilledRectangle(10,10,480,48,ILI9341_COLOR_MAGENTA);
	TM_ILI9341_Puts(10, 10,"[User button] Stop", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
	looper.DrumState = DRUMS_STARTED;


		while(looper.DrumState == DRUMS_STARTED  && currPat < numOfPatterns){

			if(switch_buff == FALSE){
					updatePatternTime(&pat1,&tim1);
					timptr = &tim1;
					patptr = &pat1;
					drumBuffPtr = drumBuffA;
					if(currPat == (numOfPatterns - 1))
						goto wait_first_beat;
					readSRAM((uint8_t *)&pat2,sizeof(Pattern));
					setPatternTime(&pat2,&tim2);
					readSRAM((uint8_t *)drumBuffB,tim2.subbeats * 5);
					switch_buff = TRUE;
				}
				else{
					updatePatternTime(&pat2,&tim2);
					timptr = &tim2;
					patptr = &pat2;
					drumBuffPtr = drumBuffB;
					if(currPat == (numOfPatterns - 1))
						goto wait_first_beat;
					readSRAM((uint8_t *)&pat1,sizeof(Pattern));
					setPatternTime(&pat1,&tim1);
					readSRAM((uint8_t *)drumBuffA,tim1.subbeats * 5);
					switch_buff = FALSE;
				}

				wait_first_beat:
				sprintf(lcdline,"%u bar ",(unsigned int)(currPat + 1));
				TM_ILI9341_Puts(10, 100, lcdline, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);

				while(first_beat == FALSE && looper.DrumState == DRUMS_STARTED){
					continue;
				}
				if(looper.DrumState == DRUMS_STOPPED || looper.DrumState == DRUMS_PAUSED){
					goto end_drum_loop;
				}

				first_beat = FALSE;
				currPat++;
				if(currPat == numOfPatterns){
					currPat = 0;
					switch_buff = FALSE;
					first_beat = FALSE;
					SRAM_seekRead(map[currPat][0],SRAM_SET);
					readSRAM((uint8_t *)&pat1,sizeof(Pattern));
					setPatternTime(&pat1,&tim1);
					readSRAM((uint8_t *)drumBuffA,tim1.subbeats * 5);
				}

		}


		end_drum_loop:
		stopDrums();

		looper.StartLooper = FALSE;
		looper.Playback = FALSE;
		looper.Recording = FALSE;

		return currPat;

}

void playDrums(FIL *fil){

	uint32_t numOfPatterns = 0;
	uint32_t numOfBytes = 0;
	uint32_t maxResolution = 0;
	uint32_t header[3];		// number of patterns, number of bytes, max. resolution
	uint32_t (*map)[2];
	uint8_t choice;
	uint32_t currPat;
	switch_buff = FALSE;
	first_beat = FALSE;

	//f_read(fil,header,sizeof(header),&bytesRead);
	SRAM_seekRead(0,SRAM_SET);
	readSRAM((uint8_t *)header,sizeof(header));
	numOfBytes = header[HEADER_NUM_BYTES];
	numOfPatterns = header[HEADER_NUM_PATTS];
	maxResolution = header[HEADER_MAX_BEATS];

	if(numOfPatterns == 0){
		TM_ILI9341_Puts(10, 100,"No patterns found!", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
		return;
	}
	if(maxResolution > MAX_SUBBEATS){
		TM_ILI9341_Puts(10, 100,"Max. number of subbeats exceeded!", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
		return;
	}
	// create memory map
	map = malloc(numOfPatterns * 2);
	if(map == NULL)
		return;

	for(currPat = 0; currPat < numOfPatterns; currPat++){
		map[currPat][0] = SRAM_readerPosition();
		readSRAM((uint8_t *)&pat1,sizeof(Pattern));
		readSRAM((uint8_t *)drumBuffA,pat1.beats * pat1.division * 5);
	}


	currPat = 0;

	do{
		// return pattern from which to play and use it as parameter to drum loop
		currPat = drumMenu(map,currPat,numOfPatterns,&choice);
		switch(choice){
			case TM_KEYPAD_Button_0: break;

			case TM_KEYPAD_Button_5: // return last played pattern and use it as parameter to menu
									 currPat = drumLoop(map,currPat,numOfPatterns);
							 	 	 break;
			default:				 break;
		}
	}
	while(choice != TM_KEYPAD_Button_0);

	free(map);


}


void stopDrums(){
	looper.DrumState = DRUMS_STOPPED;
	HAL_TIM_Base_Stop_IT(&htim2);
	HAL_Delay(100);

}

void setPatternTime(__IO Pattern *p,__IO DrumTimes *t){
	uint32_t millis = BEAT_MILLIS(p->beattime);
	t->subbeats = p->beats * p->division;
	t->barDuration = p->beats * millis;
	t->remainder = t->barDuration % t->subbeats;
	t->subBeatDuration = t->barDuration / t->subbeats;
}

void updatePatternTime(__IO Pattern *p,__IO DrumTimes *t){
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


	if(midiDrumClock < timptr->barDuration){
		if(midiDrumClock % ((timptr->remainder > 0 && drumBeatIndex == NUM_ALL_TRACKS)?(timptr->subBeatDuration + timptr->remainder):timptr->subBeatDuration) == 0){
			for(i = drumBeatIndex; i < drumBeatIndex + NUM_DRUM_TRACKS; i++){
				if(drumBuffPtr[i] != 0 && looper.DrumState == DRUMS_STARTED)
					playPercussion(NOTEON,drumBuffPtr[i]);
			}

			if(drumBuffPtr[i] != 0 && looper.DrumState == DRUMS_STARTED)
				playBass(NOTEON,drumBuffPtr[i]);

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



void resetDrums(){
	midiDrumClock = 0;
	drumBeatIndex = 0;
}

