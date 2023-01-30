/************************************************************************/
/*																		*/
/*	analogShield.h	--	Library for Analog Shield                       */
/*     Version - 2.1                                                    */
/*																		*/
/************************************************************************/
/*	Author: 	William J. Esposito										*/
/*	Copyright 2014, Digilent Inc.										*/
/************************************************************************/
/*
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
/************************************************************************/
/*  Module Description: 												*/
/*																		*/
/*	This module contains the implementation of the object class that	*/
/*	forms a chipKIT/Aruino interface to the Analog Shield functions of  */
/*  the Texas Instrument chipKit/arduino shield.						*/
/*																		*/
/*																		*/
/************************************************************************/
/*  Revision History:													*/
/*																		*/
/*	04/22/2014(WilliamE): Created										*/
/*  05/27/2014(MarshallW): Modified for readability and content         */
/*  02/16/2015(MarshallW): ChipKIT Efficiency update!                   */
/*  03/23/2015(WilliamE): Mega 2560 Support		                    	*/
/*																		*/
/*  Todo:                                                               */
/*    - Framework for DUE added but not tested                          */
/*																		*/
/************************************************************************/

/* ------------------------------------------------------------ */
/*				Include File Definitions						*/
/* ------------------------------------------------------------ */
#ifndef __ANALOGSHIELD_H__
#define __ANALOGSHIELD_H__

#include "main.h"
#include "stm32f4xx_hal.h"


#define ADC_CS_0()			HAL_GPIO_WritePin(ADCCS_GPIO_Port,ADCCS_Pin,GPIO_PIN_RESET);
#define ADC_CS_1()			HAL_GPIO_WritePin(ADCCS_GPIO_Port,ADCCS_Pin,GPIO_PIN_SET);

#define DAC_CS_0()			HAL_GPIO_WritePin(DACCS_GPIO_Port,DACCS_Pin,GPIO_PIN_RESET);
#define DAC_CS_1()			HAL_GPIO_WritePin(DACCS_GPIO_Port,DACCS_Pin,GPIO_PIN_SET);

#define SYNC_0()			HAL_GPIO_WritePin(SYNCPIN_GPIO_Port,SYNCPIN_Pin,GPIO_PIN_RESET);
#define SYNC_1()			HAL_GPIO_WritePin(SYNCPIN_GPIO_Port,SYNCPIN_Pin,GPIO_PIN_SET);

#define LDAC_0()			HAL_GPIO_WritePin(LDACPIN_GPIO_Port,LDACPIN_Pin,GPIO_PIN_RESET);
#define LDAC_1()			HAL_GPIO_WritePin(LDACPIN_GPIO_Port,LDACPIN_Pin,GPIO_PIN_SET);

#define WAIT_BUSY()			while(HAL_GPIO_ReadPin(ADCBUSY_GPIO_Port,ADCBUSY_Pin) == GPIO_PIN_RESET);

void initAnalogShield();
void writeNoUpdate(byte channel, uint16_t value);
void writeAllUpdate(int channel, unsigned int value);
void setChannelAndModeByte(byte channel, BOOL mode);




unsigned int unsignedRead(byte channel, BOOL mode);
int16_t signedRead(byte channel, BOOL mode);
void writeUpdate(byte channel, uint16_t value); //any one channel
void writeRawData(uint16_t value0, uint16_t value1, BOOL simul); //channels 0 and 1
void write3channels(uint16_t value0, uint16_t value1, uint16_t value2, BOOL simul); //channels 0-3
void write4channels(uint16_t value0, uint16_t value1, uint16_t value2, uint16_t value3, BOOL simul); //all channels

	
#endif
