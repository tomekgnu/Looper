#include "main.h"
#include "stm32f4xx_hal.h"
#include "ads1256_test.h"
#include "keyclick.h"

static uint16_t shiftIn(){
	uint16_t result = 0;
	//HAL_GPIO_WritePin(KeyClick_CS_GPIO_Port,KeyClick_CS_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(KeyClick_SHLD_GPIO_Port,KeyClick_SHLD_Pin,GPIO_PIN_RESET);
	ShortDelayUS(5);
	HAL_GPIO_WritePin(KeyClick_SHLD_GPIO_Port,KeyClick_SHLD_Pin,GPIO_PIN_SET);
	//HAL_GPIO_WritePin(KeyClick_CS_GPIO_Port,KeyClick_CS_Pin,GPIO_PIN_RESET);

	for(int i = 0; i < 16; i++) {
		uint16_t value = !HAL_GPIO_ReadPin(KeyClick_DATA_GPIO_Port,KeyClick_DATA_Pin);
		result |= value << (15 - i);
		HAL_GPIO_WritePin(KeyClick_SCK_GPIO_Port,KeyClick_SCK_Pin,GPIO_PIN_SET);
		ShortDelayUS(5);
		HAL_GPIO_WritePin(KeyClick_SCK_GPIO_Port,KeyClick_SCK_Pin,GPIO_PIN_RESET);
	}

	return result;
}

uint16_t read_shift_regs()
{
   return shiftIn();

}


