/**	
 * |----------------------------------------------------------------------
 * | Copyright (C) Tilen Majerle, 2014
 * | 
 * | This program is free software: you can redistribute it and/or modify
 * | it under the terms of the GNU General Public License as published by
 * | the Free Software Foundation, either version 3 of the License, or
 * | any later version.
 * |  
 * | This program is distributed in the hope that it will be useful,
 * | but WITHOUT ANY WARRANTY; without even the implied warranty of
 * | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * | GNU General Public License for more details.
 * | 
 * | You should have received a copy of the GNU General Public License
 * | along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * |----------------------------------------------------------------------
 */
#include "tm_stm32f4_keypad.h"
#include "stm32f429xx.h"
#include "main.h"
#include "midi.h"
#include "spi.h"

/* Pins configuration, columns are outputs */
#define KEYPAD_COLUMN_1_HIGH		HAL_GPIO_WritePin(KEYPAD_COLUMN_1_GPIO_Port, KEYPAD_COLUMN_1_GPIO_Port,GPIO_PIN_SET)
#define KEYPAD_COLUMN_1_LOW			HAL_GPIO_WritePin(KEYPAD_COLUMN_1_GPIO_Port, KEYPAD_COLUMN_1_Pin,GPIO_PIN_RESET)
#define KEYPAD_COLUMN_2_HIGH		HAL_GPIO_WritePin(KEYPAD_COLUMN_2_GPIO_Port, KEYPAD_COLUMN_2_Pin,GPIO_PIN_SET)
#define KEYPAD_COLUMN_2_LOW			HAL_GPIO_WritePin(KEYPAD_COLUMN_2_GPIO_Port, KEYPAD_COLUMN_2_Pin,GPIO_PIN_RESET)
#define KEYPAD_COLUMN_3_HIGH		HAL_GPIO_WritePin(KEYPAD_COLUMN_3_GPIO_Port, KEYPAD_COLUMN_3_Pin,GPIO_PIN_SET)
#define KEYPAD_COLUMN_3_LOW			HAL_GPIO_WritePin(KEYPAD_COLUMN_3_GPIO_Port, KEYPAD_COLUMN_3_Pin,GPIO_PIN_RESET)
#define KEYPAD_COLUMN_4_HIGH		HAL_GPIO_WritePin(KEYPAD_COLUMN_4_GPIO_Port, KEYPAD_COLUMN_4_Pin,GPIO_PIN_SET)
#define KEYPAD_COLUMN_4_LOW			HAL_GPIO_WritePin(KEYPAD_COLUMN_4_GPIO_Port, KEYPAD_COLUMN_4_Pin,GPIO_PIN_RESET)

/* Read input pins */
#define KEYPAD_ROW_1_CHECK			(!HAL_GPIO_ReadPin(KEYPAD_ROW_1_GPIO_Port, KEYPAD_ROW_1_Pin))
#define KEYPAD_ROW_2_CHECK			(!HAL_GPIO_ReadPin(KEYPAD_ROW_2_GPIO_Port, KEYPAD_ROW_2_Pin))
#define KEYPAD_ROW_3_CHECK			(!HAL_GPIO_ReadPin(KEYPAD_ROW_3_GPIO_Port, KEYPAD_ROW_3_Pin))
#define KEYPAD_ROW_4_CHECK			(!HAL_GPIO_ReadPin(KEYPAD_ROW_4_GPIO_Port, KEYPAD_ROW_4_Pin))


uint8_t KEYPAD_INT_Buttons[4][4] = {
	{0x01, 0x02, 0x03, 0x0C},
	{0x04, 0x05, 0x06, 0x0D},
	{0x07, 0x08, 0x09, 0x0E},
	{0x0A, 0x00, 0x0B, 0x0F},
};

uint8_t KEYPAD_INT_Chars[16] = {'0','1', '2','3','4', '5','6','7','8','9','*','#','A','B','C','D'};

/* Private functions */
void TM_KEYPAD_INT_SetColumn(uint8_t column);
uint8_t TM_KEYPAD_INT_CheckRow(uint8_t column);
uint8_t TM_KEYPAD_INT_Read(void);

/* Private variables */
TM_KEYPAD_Type_t TM_KEYPAD_INT_KeypadType;
static TM_KEYPAD_Button_t KeypadStatus = TM_KEYPAD_Button_NOPRESSED;
static TM_KEYPAD_Button_t KeyCode = TM_KEYPAD_Button_NOPRESSED;
static uint8_t KeyPressCount = 0;
static uint16_t millis = 0;

void TM_KEYPAD_Init() {
	
	TM_KEYPAD_INT_SetColumn(0);
}

char TM_KEYPAD_GetChar(TM_KEYPAD_Button_t key){
	return KEYPAD_INT_Chars[key];
}

TM_KEYPAD_Button_t TM_KEYPAD_Read(void) {

	if(KeyCode != TM_KEYPAD_Button_NOPRESSED && KeypadStatus != TM_KEYPAD_Button_PRESSED){
		millis = 0;
		KeypadStatus = TM_KEYPAD_Button_PRESSED;
		return KeyCode;
	}
		return TM_KEYPAD_Button_NOPRESSED;

}

/* Private */
void TM_KEYPAD_INT_SetColumn(uint8_t column) {
	/* Set rows high */
	KEYPAD_COLUMN_1_HIGH;
	KEYPAD_COLUMN_2_HIGH;
	KEYPAD_COLUMN_3_HIGH;
	KEYPAD_COLUMN_4_HIGH;

	/* Set column low */
	if (column == 1) {
		KEYPAD_COLUMN_1_LOW;
	}
	if (column == 2) {
		KEYPAD_COLUMN_2_LOW;
	}
	if (column == 3) {
		KEYPAD_COLUMN_3_LOW;
	}
	if (column == 4) {
		KEYPAD_COLUMN_4_LOW;
	}
}



uint8_t TM_KEYPAD_INT_CheckRow(uint8_t column) {
	/* Read rows */
	
	/* Scan row 1 */
	if (KEYPAD_ROW_1_CHECK) {
		return KEYPAD_INT_Buttons[0][column - 1];	
	}
	/* Scan row 2 */
	if (KEYPAD_ROW_2_CHECK) {
		return KEYPAD_INT_Buttons[1][column - 1];
	}
	/* Scan row 3 */
	if (KEYPAD_ROW_3_CHECK) {
		return KEYPAD_INT_Buttons[2][column - 1];
	}
	/* Scan row 4 */
	if (KEYPAD_ROW_4_CHECK) {
		return KEYPAD_INT_Buttons[3][column - 1];
	}
	
	/* Not pressed */
	return KEYPAD_NO_PRESSED;
}


uint8_t TM_KEYPAD_INT_Read(void) {
	uint8_t check;
	/* Set row 1 to LOW */
	TM_KEYPAD_INT_SetColumn(1);
	/* Check rows */
	check = TM_KEYPAD_INT_CheckRow(1);
	if (check != KEYPAD_NO_PRESSED) {
		return check;
	}
	
	/* Set row 2 to LOW */
	TM_KEYPAD_INT_SetColumn(2);
	/* Check columns */
	check = TM_KEYPAD_INT_CheckRow(2);
	if (check != KEYPAD_NO_PRESSED) {
		return check;
	}
	
	/* Set row 3 to LOW */
	TM_KEYPAD_INT_SetColumn(3);
	/* Check columns */
	check = TM_KEYPAD_INT_CheckRow(3);
	if (check != KEYPAD_NO_PRESSED) {
		return check;
	}


	/* Set column 4 to LOW */
	TM_KEYPAD_INT_SetColumn(4);
	/* Check rows */
	check = TM_KEYPAD_INT_CheckRow(4);
	if (check != KEYPAD_NO_PRESSED) {
		return check;
	}

	/* Not pressed */
	return KEYPAD_NO_PRESSED;
}


void TM_KEYPAD_Update(void) {
	uint8_t data[2];

	KeyCode = (TM_KEYPAD_Button_t) TM_KEYPAD_INT_Read();


	/* Every X ms read */
	if (++millis >= KEYPAD_READ_INTERVAL) {
		/* Reset */
		millis = 0;
		if(KeyCode != TM_KEYPAD_Button_NOPRESSED)
			KeyPressCount++;
		else
			KeyPressCount = 0;
		if(KeyPressCount == 4)
			KeyPressCount = 0;
		KeypadStatus = TM_KEYPAD_Button_NOPRESSED;
		//pressed = 0;
		/* Read keyboard */
	}

}

