/*
 * keyboard.c
 *
 *  Created on: 2 sie 2016
 *      Author: Tomek
 */
#include "stm32f4xx_hal.h"
uint8_t CommandKeyTable[4][4] = {
		{'1','2','3','A'},
		{'4','5','6','B'},
		{'7','8','9','C'},
		{'*','0','#','D'}

};
uint32_t selectRow(int rowNum) {
	uint32_t col;

	switch (rowNum) {
	case 0:
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
		break;
	case 1:
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
		break;
	case 2:
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
		break;
	case 3:
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
		break;

	}

	col = ~(((GPIOE->IDR & 0x3C) >> 2) | 0xFFFFFFF0);
	switch(col){
		case 1: return 0;
		case 2: return 1;
		case 4: return 2;
		case 8: return 3;
		default: return -1;
	}

}
