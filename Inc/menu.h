#ifndef __MENU_H__
#define  __MENU_H__

#include "main.h"
#include "audio.h"
#include "stm32f429xx.h"
#include "tm_stm32f4_keypad.h"
#include "stdarg.h"

extern char *messages[];
extern char *functions[];

#define TOTAL_MENU_NODES	20
#define MAX_NUM_OPTS		16

#define SHOW_STATUS_LINE()	if(show_status_line == TRUE){		\
								menuShowStatus();				\
									show_status_line = FALSE;	\
							}									\

typedef enum {
	NODE_EMPTY = -1,
	MAIN_MENU,
	AUDIO_NODE,
	RHYTHM_NODE,
	AUDIO_RHYTHM_NODE,
	SELECT_AUDIO_CHANNEL_NODE,
	SELECT_AUDIO_RHYTHM_CHANNEL_NODE,
	RECORD_SELECT_LOOPS_NODE,
	DOWNLOAD_RHYTHM_NODE,
	PLAY_RHYTHM_NODE,
	MOVE_BAR_BACK_START_NODE,
	MOVE_BAR_FORW_START_NODE,
	START_RHYTHM_NODE,
	MOVE_BAR_BACK_END_NODE,
	MOVE_BAR_FORW_END_NODE,
	SELECT_BARS_NODE,
	LOAD_RHYTHM_FROM_SD,
	SAVE_ALL_LOOPS_NODE,
	RECORD_DRUMS,
	NODE18,
	NODE19
} NODE_TYPE;

enum {
	MAIN = 0,
	BACK,
	DOWNL_RTH,
	PLAY_RTH,
	ONE_BAR_BACK_START,
	ONE_BAR_FORW_START,
	ONE_BAR_BACK_END,
	ONE_BAR_FORW_END,
	START_RHYTHM,
	NO_PATTS,
	TOO_MANY_PATTS,
	TOO_MANY_SUBB,
	CHANNEL_SELECT,
	AUDIO_RHYTHM,
	REC_SEL_LOOPS
};

typedef struct {
	char *title;
	NODE_TYPE options[MAX_NUM_OPTS];
	void (*before)(void);	// call before showing options
	void (*callback)(void);	// call after showing options.
} menuNodeType;

void menuShowTimers();
void menuShowOptions();
void menuShow(TM_KEYPAD_Button_t);
void menuInit();
void menuWaitReturn();
void menuTextBox(uint8_t lines,uint8_t xoff,uint8_t yoff,...);
void menuMultiLine(uint8_t lines, uint8_t offset, ...);
void menuMultiLineSmall(uint8_t lines, uint8_t offset, ...);
void menuClearLines(uint8_t lines,...);
void menuInitMsg();
void menuInitFunctions();
void menuStatusLine(char *text);
void menuShowStatus();
void drumMenuInput(uint32_t numOfPatterns, BOOL *play);
void setCurrentMenuNode(uint8_t node);
void connectChildNode(uint8_t parent,uint8_t opt_key,uint8_t child);
// menu macros
#define MESSG(X)			messages[X]

#endif
