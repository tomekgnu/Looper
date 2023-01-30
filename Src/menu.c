#include "stdio.h"
#include "main.h"
#include "menu.h"
#include "tm_stm32f4_ili9341.h"
#include "tm_stm32_hd44780.h"
#include "string.h"
#include "menu_callback.h"
#include "stdarg.h"
#include "joystick.h"
#include "drums.h"
#include "tm_stm32f4_keypad.h"

static char filename[13];
static menuNodeType menu_nodes[TOTAL_MENU_NODES];
static uint8_t current_node_index;	// current option
extern TM_KEYPAD_Button_t Keypad_Button;

void menuShowOptions(){
	TM_KEYPAD_Button_t option_index;
	NODE_TYPE node_index;
	uint16_t offset = 10;
	char *tit;

	TM_ILI9341_Fill(ILI9341_COLOR_MAGENTA);

	for(option_index = TM_KEYPAD_Button_0; option_index < MAX_NUM_OPTS; option_index++){
		node_index = menu_nodes[current_node_index].options[option_index];
		if(node_index == NODE_EMPTY)
			continue;
		if(option_index == TM_KEYPAD_Button_0)
			tit = "Go back";
		else
			tit = menu_nodes[node_index].title;

		sprintf(lcdline,"[%c] %s",TM_KEYPAD_GetChar(option_index),tit);
		TM_ILI9341_Puts(option_index == TM_KEYPAD_Button_0?10:30, offset, lcdline, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
		offset += 20;
	}

}
/**
 * which node,number of options,title,return node
 */
static void initParentNode(uint8_t node_index,char * tit,void (*fun)()){
	if(node_index >= TOTAL_MENU_NODES)
		return;
	menu_nodes[node_index].title = tit;
	menu_nodes[node_index].options[TM_KEYPAD_Button_0] = node_index;	// return to itself by default
	menu_nodes[node_index].callback = fun;
}

/**
 * parent node,which parent option,child node
 */
void connectChildNode(uint8_t parent,uint8_t opt_key,uint8_t child){
	if(child >= TOTAL_MENU_NODES)
		return;
	if(opt_key == TM_KEYPAD_Button_0)	// option "0" will be connected to par_node later
		return;
	if(opt_key > TM_KEYPAD_Button_D)
		return;
	menu_nodes[child].options[TM_KEYPAD_Button_0] = parent;
	menu_nodes[parent].options[opt_key] = child;

}

void setCurrentMenuNode(uint8_t node){
	current_node_index = node;
}

void menuInit(){
	menuInitMsg();
	menuInitFunctions();
	memset(menu_nodes,(int)NODE_EMPTY,sizeof(menu_nodes));
	current_node_index = MAIN_MENU;

	// top 3 nodes: AUDIO RHYTHM AUDIO+RHYTHM
	initParentNode(MAIN_MENU,messages[MAIN],NULL);
		initParentNode(AUDIO_NODE,"Audio",audio_only);									// [AUDIO]
			initParentNode(RECORD_SELECT_LOOPS_NODE,messages[REC_SEL_LOOPS],select_loops);
		initParentNode(RHYTHM_NODE,messages[AUDIO_RHYTHM],audio_rhythm);				// [AUDIO AND RHYTHM]
			initParentNode(DOWNLOAD_RHYTHM_NODE,messages[DOWNL_RTH],download_rhythm);		// [DOWNLOAD RHYTHM

				initParentNode(SELECT_BARS_NODE,"Select bars",select_bars);						// [SELECT BARS]
					initParentNode(MOVE_BAR_BACK_START_NODE,messages[ONE_BAR_BACK_START],NULL);		// [MOVE BAR BACK]
					initParentNode(MOVE_BAR_FORW_START_NODE,messages[ONE_BAR_FORW_START],NULL);		// [MOVE BAR FORW]
					initParentNode(START_RHYTHM_NODE,messages[START_RHYTHM],NULL);					// [START RHYTHM]
					initParentNode(MOVE_BAR_BACK_END_NODE,messages[ONE_BAR_BACK_END],NULL);			// [MOVE BAR BACK]
					initParentNode(MOVE_BAR_FORW_END_NODE,messages[ONE_BAR_FORW_END],NULL);			// [MOVE BAR FORW]
					initParentNode(SAVE_ALL_LOOPS_NODE,"Save all loops",NULL);
				initParentNode(RECORD_DRUMS,"Record rhythm",record_rhythm);
				initParentNode(LOAD_RHYTHM_FROM_SD,"Select rhythm",select_rhythm);
		connectChildNode(MAIN_MENU,TM_KEYPAD_Button_1,AUDIO_NODE);

			connectChildNode(AUDIO_NODE,TM_KEYPAD_Button_1,RECORD_SELECT_LOOPS_NODE);
		connectChildNode(MAIN_MENU,TM_KEYPAD_Button_2,RHYTHM_NODE);
			connectChildNode(RHYTHM_NODE,TM_KEYPAD_Button_1,DOWNLOAD_RHYTHM_NODE);
			connectChildNode(RHYTHM_NODE,TM_KEYPAD_Button_2,LOAD_RHYTHM_FROM_SD);
			connectChildNode(RHYTHM_NODE,TM_KEYPAD_Button_3,SELECT_BARS_NODE);
			connectChildNode(RHYTHM_NODE,TM_KEYPAD_Button_4,RECORD_DRUMS);

				connectChildNode(SELECT_BARS_NODE,TM_KEYPAD_Button_1,MOVE_BAR_BACK_START_NODE);
				connectChildNode(SELECT_BARS_NODE,TM_KEYPAD_Button_2,MOVE_BAR_FORW_START_NODE);
				connectChildNode(SELECT_BARS_NODE,TM_KEYPAD_Button_3,START_RHYTHM_NODE);
				connectChildNode(SELECT_BARS_NODE,TM_KEYPAD_Button_4,MOVE_BAR_BACK_END_NODE);
				connectChildNode(SELECT_BARS_NODE,TM_KEYPAD_Button_5,MOVE_BAR_FORW_END_NODE);
				connectChildNode(SELECT_BARS_NODE,TM_KEYPAD_Button_6,SAVE_ALL_LOOPS_NODE);

}

void menuShow(TM_KEYPAD_Button_t opt_key){

	if(opt_key > TM_KEYPAD_Button_D)
		return;
	if(menu_nodes[current_node_index].options[opt_key] == NODE_EMPTY)
		return;

	current_node_index = menu_nodes[current_node_index].options[opt_key];
	sprintf(lcdline,"%s",menu_nodes[current_node_index].title);
	TM_HD44780_Clear();
	TM_ILI9341_Fill(ILI9341_COLOR_MAGENTA);
	TM_HD44780_Puts(0,0,lcdline);

	menuShowOptions();

	// execute function handler
	if(menu_nodes[current_node_index].callback != NULL)
		menu_nodes[current_node_index].callback();

	return;
}

void menuShowStatus(){
	char *channelLabel = "";

	char tmp[31];
	if(looper.TwoChannels == TRUE){
		if(looper.ch1.Active == TRUE)
			channelLabel = " [A]B";
		else if(looper.ch2.Active == TRUE)
			channelLabel = " A[B]";
	}
	else{
		if(looper.ch1.Active == TRUE)
			channelLabel = " [A] ";
		else if(looper.ch2.Active == TRUE)
			channelLabel = " [B] ";
	}
	// display action
	if(looper.Recording == FALSE && looper.Playback == FALSE)
		sprintf(tmp,"Stopped   ");
	else if(looper.Recording == TRUE)
		sprintf(tmp,"Recording ");
	else if(looper.Playback == TRUE)
		sprintf(tmp,"Playback  ");
	// display channel(s)
	sprintf(tmp + strlen(tmp),channelLabel);

	if(looper.ch1.Overdub == TRUE || looper.ch2.Overdub == TRUE)
		sprintf(tmp + strlen(tmp),"Overdub");

	menuStatusLine(tmp);

	// display current function
	sprintf(lcdline,"%-16s",functions[looper.Function]);
	TM_HD44780_Puts(0,1,lcdline);
}


// write short message text at the bottom
void menuStatusLine(char *text){
	sprintf(lcdline,"%-29s",text);
	TM_ILI9341_Puts(0, 221,lcdline, &TM_Font_11x18, ILI9341_COLOR_YELLOW, ILI9341_COLOR_BLACK);
}


void menuClearLines(uint8_t lines,...){

	uint8_t i;
	int line;
	va_list ap;
	va_start(ap, lines);
	for(i = 0;lines > 0 && i < lines; i++){
		line = va_arg(ap,int);
		TM_ILI9341_DrawFilledRectangle(10,line * 20 + 10,320,line * 20 + 30,ILI9341_COLOR_MAGENTA);
	}

	va_end(ap);
}

void menuMultiLineSmall(uint8_t lines,uint8_t offset,...){
	uint8_t i;
	char *line;
	va_list ap;

	va_start(ap, offset);
	for(i = 0; i < lines; i++,offset += 12) {
		 line = va_arg(ap, char *);
		 TM_ILI9341_Puts(10, offset,line, &TM_Font_7x10, ILI9341_COLOR_RED, ILI9341_COLOR_BLUE2);
	}
	va_end(ap);
}

void menuTextBox(uint8_t lines,uint8_t xoff,uint8_t yoff,...){
	uint8_t i;
	char *line;
	va_list ap;

	va_start(ap, yoff);
	for(i = 0; i < lines; i++,yoff += 12) {
		 line = va_arg(ap, char *);
		 TM_ILI9341_Puts(xoff,yoff,line, &TM_Font_7x10, ILI9341_COLOR_RED, ILI9341_COLOR_BLUE2);
	}
	va_end(ap);

}

void menuMultiLine(uint8_t lines,uint8_t offset,...){
	uint8_t i;
	char *line;
	va_list ap;

	va_start(ap, offset);
	for(i = 0; i < lines; i++,offset += 20) {
	     line = va_arg(ap, char *);
	     TM_ILI9341_Puts(10, offset,line, &TM_Font_11x18, ILI9341_COLOR_RED, ILI9341_COLOR_BLUE2);
	}
	va_end(ap);
}

void menuWaitReturn(){
	while((Keypad_Button = TM_KEYPAD_Read() != TM_KEYPAD_Button_0))
		continue;
}

void menuShowTimers(){
	uint16_t seconds;
	uint8_t cs;	// centiseconds

	if(looper.Recording == TRUE && looper.SamplesWritten % 150 == 0){
		seconds = looper.SamplesWritten / 15000;
		cs = ((float)((looper.SamplesWritten % 15000) / 15000.00)) * 100;
		sprintf(lcdline,"Rec:  %02u:%02u:%02u sec.",(unsigned int)(seconds / 60),(unsigned int)seconds,(unsigned int)cs);
		TM_ILI9341_Puts(10, 10, lcdline, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
	}
	else if(looper.Playback == TRUE && looper.SamplesRead % 150 == 0){
		seconds = looper.SamplesRead / 15000;
		cs = ((float)((looper.SamplesRead % 15000) / 15000.00)) * 100;
		sprintf(lcdline,"Play: %02u:%02u:%02u sec.",(unsigned int)(seconds / 60),(unsigned int)seconds,(unsigned int)cs);
		TM_ILI9341_Puts(10, 10, lcdline, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
	}

}
static inline void forwardBar(BOOL start,uint32_t numOfPatterns){
	if(start == TRUE){
		if(looper.StartPattern < looper.EndPattern)
			looper.StartPattern++;
		}
		else if(looper.EndPattern < (numOfPatterns - 1))
			looper.EndPattern++;

}


static inline void backwardBar(BOOL start){
	if(start == TRUE){
		if(looper.StartPattern > 0)
			looper.StartPattern--;
		}
	else if(looper.EndPattern > looper.StartPattern)
		looper.EndPattern--;
}

void drumMenuInput(uint32_t numOfPatterns,BOOL *play){
	BOOL input = TRUE;
	BOOL startBar = TRUE;
	BOOL overdub = FALSE;
	while (TRUE){
		set_function(AUDIO_DRUMS);
		if((GET_ACTIVE_CHANNEL)->Overdub != overdub){
			overdub = (GET_ACTIVE_CHANNEL)->Overdub;
			menuShowStatus();
		}

		Keypad_Button = TM_KEYPAD_Read();
		if(Keypad_Button != TM_KEYPAD_Button_NOPRESSED){
				input = TRUE;
				switch(Keypad_Button){

					case TM_KEYPAD_Button_0:	*play = FALSE; return;
					case TM_KEYPAD_Button_1:	startBar = TRUE;
												backwardBar(startBar);
												break;
					case TM_KEYPAD_Button_2:	startBar = TRUE;
												forwardBar(startBar,numOfPatterns);
												break;
					case TM_KEYPAD_Button_3:	*play = TRUE;  return;
					case TM_KEYPAD_Button_4:	startBar = FALSE;
												backwardBar(startBar);
												break;
					case TM_KEYPAD_Button_5:	startBar = FALSE;
												forwardBar(startBar,numOfPatterns); break;
					case TM_KEYPAD_Button_6:	if(looper.Playback == TRUE || looper.Recording == TRUE || looper.SamplesWritten == 0)
													break;
												get_string(filename);
												saveAllLoopsToSD(filename);
												menuShowOptions();
												break;
					case TM_KEYPAD_Button_A:
					case TM_KEYPAD_Button_B:
												select_channel(Keypad_Button);
												menuShowOptions();
												menuShowStatus();

				}

			}
			else if(looper.Recording == TRUE || looper.Playback == TRUE){
				*play = TRUE;
				break;
			}

			if(input == TRUE){
				if(startBar == TRUE){
					sprintf(lcdline,"*Start: %-4u",(unsigned int)(looper.StartPattern + 1));
					TM_ILI9341_Puts(10, 170, lcdline, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
					sprintf(lcdline," End: %-4u",(unsigned int)(looper.EndPattern + 1));
					TM_ILI9341_Puts(10, 190, lcdline, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
				}
				else{
					sprintf(lcdline," Start: %-4u",(unsigned int)(looper.StartPattern + 1));
					TM_ILI9341_Puts(10, 170, lcdline, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
					sprintf(lcdline,"*End: %-4u",(unsigned int)(looper.EndPattern + 1));
					TM_ILI9341_Puts(10, 190, lcdline, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);

				}

				input = FALSE;
			}

			//HAL_Delay(100);
		}


}
