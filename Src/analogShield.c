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

#include "analogShield.h"
#include "main.h"
#include "spi.h"
#include "stm32f4xx_hal.h"
/* ------------------------------------------------------------ */
/*        analogShield::analogShield
**
**        Synopsis:
**			none
**        Parameters:
**			none
**        Errors:
**			none
**
**        Description:
**			Default constructor that sets the tristate buffers
**			for the ADC and DAC control pins
**
**
*/
void initAnalogShield(){
	//initialize the ADC and DAC pins
		//ADC pins
		//pinMode(adccs, OUTPUT);
		//digitalWrite(adccs, HIGH);
		ADC_CS_1();
		//pinMode(adcbusy, INPUT);

		//DAC pins
		//pinMode(syncPin, OUTPUT);
		//pinMode(ldacPin, OUTPUT);
		//digitalWrite(syncPin,HIGH);
		SYNC_1();
		//digitalWrite(ldacPin,LOW);
		LDAC_0();
		//shieldMode = 0;

		//begin();
	}

/* ------------------------------------------------------------ */
/*        analogShield::begin
**
**        Synopsis:
**				begin();
**
**		  Parameters:
**				none
**
**        Return Values:
**              none
**
**        Errors:
**
**        Description:
**			Initializes the SPI interface on the microcontroller
**
**
*/



/* ------------------------------------------------------------ */
/*        analogShield::end
**
**        Synopsis:
**				end();
**
**		  Parameters:
**				none
**
**        Return Values:
**              none
**
**        Errors:
**
**        Description:
**			Closes the SPI communication interface
**
**
*/

/* ------------------------------------------------------------ */
/*				Helper Functions                                */
/* ------------------------------------------------------------ */

/* ------------------------------------------------------------ */
/*        analogShield::setChannelAndModeByte
**
**        Synopsis:
**				setChannelAndModeByte(channel, mode);
**
**		  Parameters:
**				int channel - The channel being set from 0 to 3
**				BOOL mode - BOOLean to activate differential mode
**
**        Return Values:
**              none
**
**        Errors:
**				none
**        Description:
**				Helper function that sets the channel and mode
**				control byte and sent it over SPI
**
**
*/
	void setChannelAndModeByte(byte channel, BOOL mode){
		byte empty = 0x00;
		//control byte
		//S - sentinel, always 1
		//A2 - channel select
		//A1 - channel select
		//A0 - channel select
		//- N/C
		//Single / Diff pair
		//PD1 - power down mode
		//PD0 - power down mode
		byte control = 0x82; //default to channel 1 '001'

		//channel mask
		if(channel == 3){
			control = control | 0xE0;
		}

		else if(channel == 2){
			control = control | 0xA0;
		}

		else if(channel == 1){
			control = control | 0xD0;
		}

		else if(channel == 0){
			control = control | 0x10;
		}

		//differential mode active
		if(mode){
			control = control & 0xFB;
		}

		else{
			control = control | 0x04;
		}

		HAL_SPI_TransmitReceive(&hspi2,&control,&empty,1,100);
		return;
	}


/* ------------------------------------------------------------ */
/*				ADC Functions                                   */
/* ------------------------------------------------------------ */

/* ------------------------------------------------------------ */
/*        analogShield::read
**
**        Synopsis:
**				read(channel, mode);
**
**		  Parameters:
**				int channel - The channel being set from 0 to 3
**				BOOL mode - BOOLean to activate differential mode
**
**        Return Values:
**              unsigned int - Raw data from 0-65535 corresponding
**				to the voltage presented to the ADC
**
**        Errors:
**				none
**
**        Description:
**				Reads a raw data value from the ADC using the
**				SPI interface
**
**
*/
	unsigned int unsignedRead(byte channel, BOOL mode) {
		byte high,low,empty = 0x00;
		uint16_t result;
		// take the SS pin low to select the chip, transfer the command, then bring the bit back high:

		ADC_CS_0();

		setChannelAndModeByte(channel, mode);

		ADC_CS_1();

		//wait for busy signal to fall. If it lasts a while, try resending.
		WAIT_BUSY();

		//Result ready. Read it in
		ADC_CS_0();

		//collect data
		HAL_SPI_TransmitReceive(&hspi2,&empty,&high,1,100);
		HAL_SPI_TransmitReceive(&hspi2,&empty,&low,1,100);
		//release chip select
		ADC_CS_1();

		//compile the result into a 16 bit integer.

		//result = (int)word(high, low);
		result = (uint16_t)high << 8;
		result += low;

		//make into an unsigned int for compatibility with the DAC used on the analog shield.
		if(result < 0)
		{
			result &= 0x7FFF;
		}
		else
		{
			result |= 0x8000;
		}

		return result;

	}


/* ------------------------------------------------------------ */
/*        analogShield::signedRead
**
**        Synopsis:
**				signedRead(channel, mode);
**
**		  Parameters:
**				int channel - The channel being set from 0 to 3
**				BOOL mode - BOOLean to activate differential mode
**
**        Return Values:
**              int - Signed data from -32768 - 32767 corresponding
**				to the voltage presented to the ADC
**
**        Errors:
**				none
**
**        Description:
**				Reads a signed data value from the ADC using the
**				SPI interface
**
**
*/
	int16_t signedRead(byte channel, BOOL mode) {
		byte high,low,empty=0x00;
		int16_t result;
		// take the SS pin low to select the chip, transfer the command, then bring the bit back high:
		ADC_CS_0();
		setChannelAndModeByte(channel, mode);
		ADC_CS_1();

		//wait for busy signal to fall. If it lasts a while, try resending.
		WAIT_BUSY();

		//Result ready. Read it in
		ADC_CS_0();

		//collect data
		HAL_SPI_TransmitReceive(&hspi2,&empty,&high,1,100);
		HAL_SPI_TransmitReceive(&hspi2,&empty,&low,1,100);

		//release chip select
		ADC_CS_1();

		//compile the result into a 16 bit integer.

		//result = (int)word(high, low);
		result = (int16_t)high<<8;
		result += low;

		//make into an unsigned int for compatibility with the DAC used on the analog shield.
		return result;
	}

/* ------------------------------------------------------------ */
/*				DAC Functions                                   */
/* ------------------------------------------------------------ */

/* ------------------------------------------------------------ */
/*        analogShield::writeNoUpdate
**
**        Synopsis:
**				writeNoUpdate(channel, value);
**
**		  Parameters:
**				int channel - The channel being set from 0 to 3
**				unsigned int value - raw data value to write to
**					the DAC
**
**        Return Values:
**              none
**
**        Errors:
**				none
**
**        Description:
**				Writes a raw data value to the selected DAC data
**					buffer but does not update the DAC output
**
**
*/
	void writeNoUpdate(byte channel, uint16_t value){
		byte call = 0x00;//00
		byte empty = 0x00;
		//prep work, break up the into into two 8 bit bytes
		byte high = value >> 8;//highByte(value);
		byte low = value & 0x00FF;//lowByte(value);

		// take the SS pin low to select the chip:
		DAC_CS_0();


		//  send in the address and value via SPI:

		if(channel == 1)
			call = 0x02;

		else if(channel == 2)
			call = 0x04;

		else if(channel == 3)
			call = 0x06;

		//send command byte
		HAL_SPI_TransmitReceive(&hspi2,&call,&empty,1,100);
		HAL_SPI_TransmitReceive(&hspi2,&high,&empty,1,100);
		HAL_SPI_TransmitReceive(&hspi2,&low,&empty,1,100);

		DAC_CS_1();

	}

/* ------------------------------------------------------------ */
/*        analogShield::writeAllUpdate
**
**        Synopsis:
**				writeAllUpdate(channel, value);
**
**		  Parameters:
**				int channel - The channel being set from 0 to 3
**				unsigned int value - raw data value to write to
**					the DAC
**
**        Return Values:
**              none
**
**        Errors:
**				none
**
**        Description:
**				Writes a raw data value to the selected DAC data
**					buffer and updates DAC output
**
**
*/
	void writeAllUpdate(int channel, unsigned int value){
		byte call = 0x20;//20
		byte empty= 0x00;
		//prep work, break up the into into two 8 bit bytes
		byte high = value >> 8;//highByte(value);
		byte low = value & 0x00FF;//lowByte(value);

		// take the SS pin low to select the chip:
		DAC_CS_0();

		//  send in the address and value via SPI:

		if(channel == 1)
			call = 0x22;

		else if(channel == 2)
			call = 0x24;

		else if(channel == 3)
			call = 0x26;

		//send command byte
		HAL_SPI_TransmitReceive(&hspi2,&call,&empty,1,100);
		HAL_SPI_TransmitReceive(&hspi2,&high,&empty,1,100);
		HAL_SPI_TransmitReceive(&hspi2,&low,&empty,1,100);
		DAC_CS_1();
	}

/* ------------------------------------------------------------ */
/*        analogShield::write
**
**        Synopsis:
**				write(channel, value);
**
**		  Parameters:
**				int channel - The channel being set from 0 to 3
**				unsigned int value - raw data value to write to
**					the DAC
**
**        Return Values:
**              none
**
**        Errors:
**				none
**
**        Description:
**				Writes a raw data value to the selected DAC data
**					buffer and updates DAC output
**
**
*/
	void writeUpdate(byte channel, uint16_t value) {
		byte call = 0x10;
		byte empty = 0x00;
		//prep work, break up the into into two 8 bit bytes
		byte high = value >> 8;//highByte(value);
		byte low = value & 0x00FF;//lowByte(value);

		// take the SS pin low to select the chip:
		DAC_CS_0();

		//  send in the address and value via SPI:

		if(channel == 1)
			call = 0x12;

		else if(channel == 2)
			call = 0x14;

		else if(channel == 3)
			call = 0x16;


			//send command byte
		HAL_SPI_TransmitReceive(&hspi2,&call,&empty,1,100);
		HAL_SPI_TransmitReceive(&hspi2,&high,&empty,1,100);
		HAL_SPI_TransmitReceive(&hspi2,&low,&empty,1,100);
		DAC_CS_1();
}

/* ------------------------------------------------------------ */
/*        analogShield::write
**
**        Synopsis:
**				write(value0, value1, simul);
**
**		  Parameters:
**				unsigned int value0 - Raw value from 0 to 65535
**					corresponding to -5V - 5V
**				unsigned int value1 - Raw value from 0 to 65535
**					corresponding to -5V - 5V
**				BOOL simul - used to synchronize the DAC to update
**					both channels at once
**
**        Return Values:
**              none
**
**        Errors:
**				none
**
**        Description:
**				Writes raw data values to the DAC.
**					value0 => DAC channel 0
**					value1 => DAC channel 1
**
*/

	void writeRawData(uint16_t value0, uint16_t value1, BOOL simul){
		writeNoUpdate(0,value0);
		writeAllUpdate(1,value1);
	}

/* ------------------------------------------------------------ */
/*        analogShield::write
**
**        Synopsis:
**				write(value0, value1, value2, simul);
**
**		  Parameters:
**				unsigned int value0 - Raw value from 0 to 65535
**					corresponding to -5V - 5V
**				unsigned int value1 - Raw value from 0 to 65535
**					corresponding to -5V - 5V
**				unsigned int value2 - Raw value from 0 to 65535
**					corresponding to -5V - 5V
**				BOOL simul - used to synchronize the DAC to update
**					both channels at once
**
**        Return Values:
**              none
**
**        Errors:
**				none
**
**        Description:
**				Writes raw data values to the DAC.
**					value0 => DAC channel 0
**					value1 => DAC channel 1
**					value2 => DAC channel 2
**
**
*/
	void write3channels(uint16_t value0, uint16_t value1, uint16_t value2, BOOL simul){
		writeNoUpdate(0,value0);
		writeNoUpdate(1,value1);
		writeAllUpdate(2,value2);
	}


/* ------------------------------------------------------------ */
/*        analogShield::write
**
**        Synopsis:
**				write(value0, value1, value2, value3, simul);
**
**		  Parameters:
**				unsigned int value0 - Raw value from 0 to 65535
**					corresponding to -5V - 5V
**				unsigned int value1 - Raw value from 0 to 65535
**					corresponding to -5V - 5V
**				unsigned int value2 - Raw value from 0 to 65535
**					corresponding to -5V - 5V
**				unsigned int value3 - Raw value from 0 to 65535
**					corresponding to -5V - 5V
**				BOOL simul - used to synchronize the DAC to update
**					both channels at once
**
**        Return Values:
**              none
**
**        Errors:
**				none
**
**        Description:
**				Writes raw data values to the DAC.
**					value0 => DAC channel 0
**					value1 => DAC channel 1
**					value2 => DAC channel 2
**					value3 => DAC channel 3
**
**
*/
	void write4channels(uint16_t value0, uint16_t value1, uint16_t value2, uint16_t value3, BOOL simul){
		writeNoUpdate(0,value0);
		writeNoUpdate(1,value1);
		writeNoUpdate(2,value2);
		writeAllUpdate(3,value3);
	}




