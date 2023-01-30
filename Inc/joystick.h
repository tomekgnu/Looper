#ifndef __JOYSTICK_H__
#define  __JOYSTICK_H__

#include "main.h"

typedef struct{
	uint8_t xpos;
	uint8_t ypos;
	BOOL	but;
} JOYSTICK;

#define CENTER		4
#define MAX_JOY		7
#define MIN_JOY		0

JOYSTICK Read_Joystick();
BOOL Movement_X();
BOOL Movement_Y();
void Wait_Joystick();
void Update_Joystick();
BOOL Active_Joystick();

#endif
