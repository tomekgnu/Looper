/*
 * menu_callback.c
 *
 *  Created on: 29.07.2018
 *      Author: Tomek
 */
#include "stdio.h"
#include "ff.h"
#include "main.h"
#include "menu.h"
#include "memops.h"
#include "drums.h"
#include "tm_stm32f4_keypad.h"
#include "tm_stm32f4_ili9341.h"
#include "tm_stm32_hd44780.h"
#include "menu_callback.h"
#include "stdlib.h"
#include "ads1256_test.h"
#include "audio.h"
#include "stm32f429i_discovery.h"
#include "tm_stm32f4_fatfs.h"
#include "tim.h"

extern TM_KEYPAD_Button_t Keypad_Button;
extern BOOL Skip_Read_Button;

static char filename[13];

void audio_rhythm(){
	//looper.Function = AUDIO_DRUMS;

}

void audio_only(){
	//looper.Function = AUDIO_ONLY;

}

void drums_only(void){
	//looper.Function = DRUMS_ONLY;
}

void audio_drums(void){
	//looper.Function = AUDIO_DRUMS;
}

static void highlight_on(FileEntry *current){
	TM_ILI9341_Puts(10, 10 + (current->list_pos * 11), current->filename, &TM_Font_7x10, ILI9341_COLOR_YELLOW, ILI9341_COLOR_CYAN);
}

static void highlight_off(FileEntry *current){
	TM_ILI9341_Puts(10, 10 + (current->list_pos * 11), current->filename, &TM_Font_7x10, ILI9341_COLOR_YELLOW, ILI9341_COLOR_MAGENTA);
}

static void display_list_desc(FileEntry *head,uint16_t size){
	FileEntry *current = head;
	TM_ILI9341_Fill(ILI9341_COLOR_MAGENTA);
	current = head;
	do{
		if(current->list_pos == (size - 1))
			TM_ILI9341_Puts(10, 10 + (current->list_pos * 11), current->filename, &TM_Font_7x10, ILI9341_COLOR_YELLOW, ILI9341_COLOR_CYAN);
		else
			TM_ILI9341_Puts(10, 10 + (current->list_pos * 11), current->filename, &TM_Font_7x10, ILI9341_COLOR_YELLOW, ILI9341_COLOR_MAGENTA);

		current = current->prev;
	}
	while(current != NULL && current->list_pos != (size - 1));

}

static void display_list_asc(FileEntry *head,uint16_t size){
	FileEntry *current = head;
	TM_ILI9341_Fill(ILI9341_COLOR_MAGENTA);
	current = head;
	do{
		if(current->list_pos == 0)
			TM_ILI9341_Puts(10, 10 + (current->list_pos * 11), current->filename, &TM_Font_7x10, ILI9341_COLOR_YELLOW, ILI9341_COLOR_CYAN);
		else
			TM_ILI9341_Puts(10, 10 + (current->list_pos * 11), current->filename, &TM_Font_7x10, ILI9341_COLOR_YELLOW, ILI9341_COLOR_MAGENTA);
		current = current->next;
	}
	while(current != NULL && current->list_pos != 0);

}


BOOL get_file_sd(char *outstr){
	uint32_t idx = 0;
	uint32_t list_size = 10;
	KeyDir keydir = KEY_NONE;
	TCHAR path[8];
	FILINFO fno;
	DIR dir;
	FIL fil;
	FileEntry *current, *head, *node, *tail;
	current = NULL;
	head = NULL;
	BOOL delete = FALSE;
	FRESULT res = f_getcwd(path,8);


	f_opendir(&dir,path);
	if(res != FR_OK){
			TM_ILI9341_Puts(10, 10, "Error reading SD card", &TM_Font_7x10,	ILI9341_COLOR_YELLOW, ILI9341_COLOR_MAGENTA);
			return FALSE;
	}

	while((res = f_readdir(&dir, &fno)) == FR_OK && fno.fname[0] != 0){
		if((fno.fattrib & AM_DIR) || (fno.fsize == 0))	// directory
			continue;
		 node = (FileEntry *)malloc(sizeof(FileEntry));
		 strcpy(node->filename,fno.fname);
		 node->number = idx++;
		 node->list_pos = node->number % list_size;
		 node->next = node->prev = NULL;
		 if(head == NULL){
			current = head = tail = node;
		 }
		 else{
			 current = current->next = node;
			 current->prev = tail;
			 tail = current;
		 }
	}

	f_closedir(&dir);
	if(head == NULL){
		TM_ILI9341_Fill(ILI9341_COLOR_MAGENTA);
		menuMultiLine(2,10,"No files","Press [0] key");
		menuWaitReturn();
		return FALSE;
	}

	current = head;
	display_list_asc(current,list_size);
	menuMultiLine(3,180,"[2]Up [8]Down [5]Select","[3]Listen [U]ser button stop","[6]Delete [0]Cancel");

	// read keys and display file list and cursor
	while ((Keypad_Button = TM_KEYPAD_Read())) {
		switch (Keypad_Button) {
			case TM_KEYPAD_Button_2:	keydir = KEY_UP;
										if(current->prev != NULL)
											current = current->prev;
										break;
			case TM_KEYPAD_Button_3:	if(looper.Function == AUDIO_ONLY){
											f_open(&fil,current->filename,FA_OPEN_ALWAYS | FA_READ);
											looper.Function = PLAY_SD;
											SD_readSingleTrack(&fil);
											f_close(&fil);
										}
										else if(looper.Function == AUDIO_DRUMS){
											uint32_t numOfPatterns;
											uint32_t numOfBytes;
											uint32_t maxResolution;
											readRhythmFromSD(current->filename);
											memset(pattern_audio_map,0,sizeof(pattern_audio_map));
											readDrums(&numOfPatterns,&numOfBytes,&maxResolution);
											looper.StartPattern = 0;
											looper.EndPattern = (numOfPatterns > 4?3:(numOfPatterns - 1));
											drumLoop();
											looper.Function = AUDIO_DRUMS;
										}
										break;
			case TM_KEYPAD_Button_5:	strcpy(outstr,current->filename);
										goto end_get_file;
			case TM_KEYPAD_Button_6:	if(f_unlink(current->filename) == FR_OK){
											delete = TRUE;
											goto end_get_file;
										}
										break;
			case TM_KEYPAD_Button_8:	keydir = KEY_DOWN;
										if(current->next != NULL)
											current = current->next;
										break;
			case TM_KEYPAD_Button_0:	goto end_get_file;

		}

		if(Keypad_Button != TM_KEYPAD_Button_NOPRESSED){
			if(keydir == KEY_DOWN){
				if(current->list_pos == 0)
					display_list_asc(current,list_size);
				else{
					highlight_on(current);
					highlight_off(current->prev);
				}
			}
			else if(keydir == KEY_UP){
				if(current->list_pos == (list_size - 1))
					display_list_desc(current,list_size);
				else{
					highlight_on(current);
					highlight_off(current->next);
				}
			}

			menuMultiLine(3,180,"[2]Up [8]Down [5]Select","[3]Listen [U]ser button stop","[6]Delete [0]Cancel");
		}
	}	// end of while


	end_get_file:
	// free linked list memory
	for(current = head; current;current=current->next)
		free(current);

	return delete;
}


void get_string(char *outstr) {
	uint8_t nameIndex = 0;
	TM_KEYPAD_Button_t tmpKey = TM_KEYPAD_Button_NOPRESSED;
	uint8_t charIndex = 0;
	BOOL keyChar = FALSE;
	BOOL keyBack = FALSE;
	BOOL keyForw = FALSE;
	BOOL keyDel = FALSE;

	// array indexed by TM_KEYPAD_Button_t
	static char* keychars[10] = { "0", "1._", "2ABCabc", "3DEFdef", "4GHIghi",
			"5JKLjkl", "6MNOmno", "7PRSprs", "8TUVtuv", "9WXYZwxyz" };
	memset(outstr, '\0', 26);
	TM_ILI9341_Fill(ILI9341_COLOR_MAGENTA);
	TM_ILI9341_Puts(10, 10, "Type filename and/or press [#]", &TM_Font_7x10,
			ILI9341_COLOR_YELLOW, ILI9341_COLOR_MAGENTA);
	while (TM_KEYPAD_Read() == TM_KEYPAD_Button_4)
		continue;
	TM_ILI9341_Putc(10 + (nameIndex * 11), 30, ' ', &TM_Font_11x18,
			ILI9341_COLOR_BLACK, ILI9341_COLOR_YELLOW);
	while ((Keypad_Button = TM_KEYPAD_Read()) != TM_KEYPAD_Button_HASH) {
		switch (Keypad_Button) {
			case TM_KEYPAD_Button_0:
			case TM_KEYPAD_Button_1:
			case TM_KEYPAD_Button_2:
			case TM_KEYPAD_Button_3:
			case TM_KEYPAD_Button_4:
			case TM_KEYPAD_Button_5:
			case TM_KEYPAD_Button_6:
			case TM_KEYPAD_Button_7:
			case TM_KEYPAD_Button_8:
			case TM_KEYPAD_Button_9:
				keyChar = TRUE;
				if (Keypad_Button != tmpKey) {
					tmpKey = Keypad_Button;
					charIndex = 0;
				} else if (keychars[Keypad_Button][charIndex + 1] != '\0')
					charIndex++;
				else
					charIndex = 0;

				outstr[nameIndex] = keychars[Keypad_Button][charIndex];
				break;
			case TM_KEYPAD_Button_A:
				keyForw = TRUE;
				break;
			case TM_KEYPAD_Button_B:
				keyBack = TRUE;
				break;
			case TM_KEYPAD_Button_C:
				keyDel = TRUE;
				break;
		}
		if (Keypad_Button != TM_KEYPAD_Button_NOPRESSED) {
			TM_ILI9341_Puts(10, 30, outstr, &TM_Font_11x18,
					ILI9341_COLOR_BLACK, ILI9341_COLOR_CYAN);
			if (keyChar == TRUE) {
				keyChar = FALSE;
				TM_ILI9341_Putc(10 + (nameIndex * 11), 30, outstr[nameIndex],
						&TM_Font_11x18, ILI9341_COLOR_BLACK,
						ILI9341_COLOR_YELLOW);
			} else if (keyForw == TRUE) {
				keyForw = FALSE;
				if (nameIndex < 11 && outstr[nameIndex] != '\0')
					nameIndex++;
				else
					TM_ILI9341_Putc(10 + (nameIndex * 11), 30, ' ',
							&TM_Font_11x18, ILI9341_COLOR_BLACK,
							ILI9341_COLOR_YELLOW);
			} else if (keyBack == TRUE) {
				keyBack = FALSE;
				if (outstr[nameIndex] == '\0')
					TM_ILI9341_Putc(10 + (nameIndex * 11), 30, ' ',
							&TM_Font_11x18, ILI9341_COLOR_MAGENTA,
							ILI9341_COLOR_MAGENTA);

				if (nameIndex > 0)
					nameIndex--;

				TM_ILI9341_Putc(10 + (nameIndex * 11), 30, outstr[nameIndex],
						&TM_Font_11x18, ILI9341_COLOR_BLACK,
						ILI9341_COLOR_YELLOW);
			} else if (keyDel == TRUE) {
				keyDel = FALSE;
				strcpy(outstr + nameIndex, outstr + nameIndex + 1);
				TM_ILI9341_Puts(10, 30, outstr, &TM_Font_11x18,
						ILI9341_COLOR_BLACK, ILI9341_COLOR_CYAN);
				TM_ILI9341_Putc(10 + (strlen(outstr) * 11), 30, ' ',
						&TM_Font_11x18, ILI9341_COLOR_MAGENTA,
						ILI9341_COLOR_MAGENTA);
			}

			if (outstr[nameIndex] != '\0')
				TM_ILI9341_Putc(10 + (nameIndex * 11), 30, outstr[nameIndex],
						&TM_Font_11x18, ILI9341_COLOR_BLACK,
						ILI9341_COLOR_YELLOW);
			else
				TM_ILI9341_Putc(10 + (nameIndex * 11), 30, ' ', &TM_Font_11x18,
						ILI9341_COLOR_BLACK, ILI9341_COLOR_YELLOW);
		}
	}
}


void saveAllLoops()
{

}

void download_rhythm() {
	looper.Function = DOWNLOAD_SRAM;
	uint32_t bytes_written = 0;

	while(TRUE){
		begin_download:
			filename[0] = '\0';
			menuMultiLine(2,110,"Press [Send via USB] button","in Rhythm application.");
			if((bytes_written = SRAM_download_rhythm()) > 0){
				TM_ILI9341_DrawFilledRectangle(10,110,320,150,ILI9341_COLOR_MAGENTA);
				menuMultiLine(2,110,"[1] Save rhythm to SD card","[2] Repeat download");
				while(TRUE){
					filename[0] = '\0';
					Keypad_Button = TM_KEYPAD_Read();

					switch(Keypad_Button){
						case TM_KEYPAD_Button_0:	goto end_download;
						case TM_KEYPAD_Button_1:	get_string(filename);
													writeSRAMtoSD(bytes_written,filename);
													menuShowOptions();
													goto end_download;
													goto begin_download;
						case TM_KEYPAD_Button_2:	goto begin_download;
					}
				}

			}
			else goto end_download;


	}

	end_download:
	looper.Function = IDLE;
	Skip_Read_Button = TRUE;
}

void select_rhythm(){
	set_function(IDLE);
	menuMultiLine(1,30,"[1] Select file");
	while(TRUE){
			filename[0] = '\0';
			Keypad_Button = TM_KEYPAD_Read();
			checkSD();
			switch(Keypad_Button){
				case TM_KEYPAD_Button_0: 	goto end_select_loop;
				case TM_KEYPAD_Button_1:	while(get_file_sd(filename) == TRUE)
												continue;
											readRhythmFromSD(filename);
											menuShowOptions();
											break;
			}
			if(Keypad_Button != TM_KEYPAD_Button_NOPRESSED){
				menuMultiLine(1,30,"[1] Select file");
			}

	}

	end_select_loop:
	Skip_Read_Button = TRUE;
}

void select_loops(){

	looper.StartPattern = 0;
	looper.EndPattern = 0;

	while(TM_KEYPAD_Read() != TM_KEYPAD_Button_NOPRESSED)
		continue;
	show_status_line = TRUE;
	SHOW_STATUS_LINE();

	menuMultiLine(7,30,"[1] Skip loop backward","[2] Skip loop forward","[3] Pause/Resume loop","[4] Save current loop","[5] Read loop from SD","[6] Save all loops","[AB] Select channels");
	sprintf(lcdline, "Current loop: %u", (unsigned int)(looper.StartPattern + 1));
	menuMultiLine(1,200,lcdline);

	while(TRUE){
		filename[0] = '\0';
		set_function(AUDIO_ONLY);
		Keypad_Button = TM_KEYPAD_Read();
		checkSD();

		switch(Keypad_Button){
			case TM_KEYPAD_Button_0: goto end_select_loop;
			case TM_KEYPAD_Button_1:
				 looper.StartLooper = FALSE;
				 looper.Playback = FALSE;
				 looper.Recording = FALSE;
				 if(pattern_audio_map[looper.StartPattern].sample_position > 0){
					 looper.StartPattern--;
					 looper.EndPattern--;
				 }

				 break;
			case TM_KEYPAD_Button_2:
				looper.StartLooper = FALSE;
				looper.Playback = FALSE;
				looper.Recording = FALSE;
				if(pattern_audio_map[looper.EndPattern + 1].sample_position > 0){
					 looper.StartPattern++;
					 looper.EndPattern++;
				 }

				break;
			case TM_KEYPAD_Button_A:
			case TM_KEYPAD_Button_B:	select_channel(Keypad_Button);
										break;

			case TM_KEYPAD_Button_3:	pauseLoop();
										break;
			case TM_KEYPAD_Button_4:	if(looper.Playback == TRUE || looper.Recording == TRUE || looper.SamplesWritten == 0)
											break;
										get_string(filename);
										saveLoopToSD(looper.StartPattern,filename);
										menuShowOptions();
										break;
			case TM_KEYPAD_Button_5:	if(looper.Playback == TRUE || looper.Recording == TRUE)
											break;
										while(get_file_sd(filename) == TRUE)
											continue;
										readLoopFromSD(looper.StartPattern,filename);
										setStartEndPatterns(looper.StartPattern,looper.EndPattern);
										menuShowOptions();
										break;

		}

		if(Keypad_Button != TM_KEYPAD_Button_NOPRESSED){
			menuMultiLine(7,30,"[1] Skip loop backward","[2] Skip loop forward","[3] Pause/Resume loop","[4] Save current loop","[5] Read loop from SD","[6] Save all loops","[AB] Select channels");
			sprintf(lcdline, "Current loop: %u", (unsigned int)(looper.StartPattern + 1));
			menuMultiLine(1,200,lcdline);
			show_status_line = TRUE;
		}


		SHOW_STATUS_LINE();

	}


	end_select_loop:
	stopAll();
	Skip_Read_Button = TRUE;
}


void select_channel(TM_KEYPAD_Button_t key){
	if(looper.Playback == TRUE || looper.Recording == TRUE)
		pauseLoop();
	switch(key){
		case TM_KEYPAD_Button_A:
			if(ACTIVE_CHANNEL_2){
				setActiveChannelOne();
				break;
			}
			if(ACTIVE_CHANNEL_1){
				 if(looper.TwoChannels == FALSE)
					 toggleActiveBothChannels(TRUE);
				 else
					 toggleActiveBothChannels(FALSE);
			}
			break;

		case TM_KEYPAD_Button_B:
			if(ACTIVE_CHANNEL_1){
				setActiveChannelTwo();
				break;
			}
			if(ACTIVE_CHANNEL_2){
				 if(looper.TwoChannels == FALSE)
					 toggleActiveBothChannels(TRUE);
				 else
					 toggleActiveBothChannels(FALSE);
			}
			break;
		}

}


void select_bars() {
	uint32_t numOfPatterns;
	uint32_t numOfBytes;
	uint32_t maxResolution;
	BOOL play = FALSE;
	//looper.startPattern = 0;
	sdram_pointer = 0;
	//map = (uint32_t (*)[])
	memset(pattern_audio_map,0,sizeof(pattern_audio_map));
	readDrums(&numOfPatterns,&numOfBytes,&maxResolution);
	// restore previous values of start/end bars
	getStartEndPatterns(&looper.StartPattern,&looper.EndPattern);
	//looper.endPattern = numOfPatterns - 1;

	if(numOfPatterns == 0){
		menuMultiLine(1,130,messages[NO_PATTS]);
		menuWaitReturn();
		goto end_play_rhythm;
	}
	if(numOfPatterns > MAX_PATTERNS){
		menuMultiLine(1,150,messages[TOO_MANY_PATTS]);
		menuWaitReturn();
		goto end_play_rhythm;
	}
	if(maxResolution > MAX_SUBBEATS){
		menuMultiLine(1,150,messages[TOO_MANY_SUBB]);
		menuWaitReturn();
		goto end_play_rhythm;
	}

	do{
		looper.DrumState = DRUMS_READY;
		// return star and end patterns and use them as parameters to drum loop
		// waits until play == TRUE (button "3" or joystick)
		menuShowStatus();
		drumMenuInput(numOfPatterns,&play);
		if(play == FALSE)
			goto end_play_rhythm;
		drumLoop();
		// end playing on pressing user button or joystick


	}while(play != FALSE);

	end_play_rhythm:
	//free(map);
	looper.DrumState = DRUMS_STOPPED;
	Skip_Read_Button = TRUE;
}

void record_rhythm(){
	uint32_t beats,division,beattime;

	menuMultiLine(7,30,"[1] Preview sounds",
						"[2] Press to record",
						"[3] Play current",
						"[4] Clear current",
						"[5] Save as first",
						"[6] Save as next",
						"[7] Set parameters");
	getRhythmParams(&beats,&division,&beattime);
	sprintf(lcdline,"Beats:%2u Subbeats:%2u Time:%3u",(unsigned int)beats,(unsigned int)division,(unsigned int)beattime);
	menuStatusLine(lcdline);

	while(TRUE){
		Keypad_Button = TM_KEYPAD_Read();
		looper.DrumState = DRUMS_PAUSED;
		switch(Keypad_Button){
			case TM_KEYPAD_Button_0:	return;
			case TM_KEYPAD_Button_1:	preview_drums();
										break;
			case TM_KEYPAD_Button_2:	record_drums();
										break;
			case TM_KEYPAD_Button_3:	play_drums();
										break;
			case TM_KEYPAD_Button_4:	clear_drums();
										break;
			case TM_KEYPAD_Button_5:	save_first(drumBuffA);
										break;
			case TM_KEYPAD_Button_6:	save_next(drumBuffA);
										break;
			case TM_KEYPAD_Button_7:	select_rhythm_params();
										break;


		}

		if(Keypad_Button != TM_KEYPAD_Button_NOPRESSED){
			menuClearLines(2,3,5);
			menuMultiLine(7,30,"[1] Preview sounds",
								"[2] Press to record",
								"[3] Play current",
								"[4] Clear current",
								"[5] Save as first",
								"[6] Save as next",
								"[7] Set parameters");
			getRhythmParams(&beats,&division,&beattime);
			sprintf(lcdline,"Beats:%2u Subbeats:%2u Time:%3u",(unsigned int)beats,(unsigned int)division,(unsigned int)beattime);
			menuStatusLine(lcdline);
		}


	}

}

