#ifndef __MENU_CALLBACK_H__
#define  __MENU_CALLBACK_H__

#include "ff.h"
#include "spiffs.h"

void audio_only(void);
void audio_rhythm(void);
void drums_only(void);
void audio_drums(void);
void download_rhythm(void);
void record_rhythm(void);
void select_bars(void);
void select_channel(TM_KEYPAD_Button_t key);
void select_loops(void);
void select_rhythm();
void readRhythmFromSD(char *filename);
void writeSRAMtoSD(uint32_t bts,char *filename);
void saveLoopToSD(uint32_t n,char *filename);
void saveAllLoopsToSD(char *filename);
void readLoopFromSD(uint32_t n,char *filename);
void saveAllLoops();
void get_string(char *outstr);
BOOL get_file_sd(char *outstr);
void get_file_sf3(char *outstr);

typedef struct FileEntry{
	char filename[33];
	uint16_t number;
	uint16_t list_pos;
	struct FileEntry *prev;
	struct FileEntry *next;
} FileEntry;
#endif
