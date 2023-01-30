/*
 * joystick.c
 *
 *  Created on: 19.08.2018
 *      Author: Tomek
 */
#include "stm32f429xx.h"
#include "main.h"
#include "joystick.h"
#include "adc.h"
#include "stdlib.h"

static __IO JOYSTICK jstick;

static void Read_Joystick_INT(){

	jstick.but = (HAL_GPIO_ReadPin(Joystick_SW_GPIO_Port,Joystick_SW_Pin) == GPIO_PIN_RESET);
	jstick.xpos = (joystick_data[0] >> 3);
	jstick.ypos = (joystick_data[1] >> 3);
}

JOYSTICK Read_Joystick(){
	return jstick;
}

BOOL Movement_Y(){
	return jstick.ypos != CENTER;
}

BOOL Movement_X(){
	return jstick.xpos != CENTER;
}

void Update_Joystick(){
	Read_Joystick_INT();
}

BOOL Active_Joystick(){
	return (jstick.xpos != CENTER || jstick.ypos != CENTER || jstick.but != FALSE);
}

void Wait_Joystick(){
	uint16_t wait;

	// center position: no wait
	if(jstick.xpos == 4 && jstick.ypos == 4 && jstick.but == FALSE)
		return;
	// button press
	if(jstick.but == TRUE)
		wait = 1000;
	// not in any extreme position
	if((jstick.xpos < MAX_JOY && jstick.xpos > MIN_JOY) && (jstick.ypos < MAX_JOY && jstick.ypos > MIN_JOY))
		wait = 200;
	else	// extreme position
		wait = 100;

	HAL_Delay(wait);

}
