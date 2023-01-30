#include "menu.h"

char *messages[30];
char *functions[7];

void menuInitFunctions(){
	functions[IDLE] = "Idle";
	functions[AUDIO_ONLY] = "Audio only";
	functions[AUDIO_DRUMS] = "Audio drums";
	functions[DRUMS_ONLY] = "Drums only";
	functions[DOWNLOAD_SRAM] = "Download SRAM";
	functions[PLAY_SONG] = "Play Song";
	functions[PLAY_SD] = "Play SD";

}

void menuInitMsg(){

	messages[MAIN] = "Main looper menu";
	messages[BACK] = "Go back";
	messages[DOWNL_RTH] = "Download rhythm";
	messages[PLAY_RTH] = "Play/Record rhythm";
	messages[ONE_BAR_BACK_START] = "Move start back";
	messages[ONE_BAR_FORW_START] = "Move start forward";
	messages[ONE_BAR_BACK_END] = "Move end back";
	messages[ONE_BAR_FORW_END] = "Move end forward";
	messages[START_RHYTHM] = "Start rhythm";
	messages[NO_PATTS] = "No patterns found!";
	messages[TOO_MANY_PATTS] = "Too many patterns!";
	messages[TOO_MANY_SUBB] = "Too many subbeats!";
	messages[CHANNEL_SELECT] = "Select channel";
	messages[AUDIO_RHYTHM] = "Audio and rhythm";
	messages[REC_SEL_LOOPS] = "Select loop";
}

