/************************************************************************/
/*                                                      */
/*  SF3.cpp  --  Function Declarations for SF3               */
/*                                                      */
/************************************************************************/
/*  Author:   Samuel Lowe                               */
/*  Copyright (c) 2016, Digilent Inc. All rights reserved.          */

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
/*  Module Description:                         */
/*                                            */
/*  This is the function cpp file for the SF3 library         */
/*                                            */
/************************************************************************/
/*  Revision History:                           */
/*                                            */
/*  10/5/2016 (AndrewH) : started                         */
/*  10/7/2016(SamL): created                   */
/*                                            */
/************************************************************************/

#include "SF3.h"
#include "main.h"
#include "stm32f4xx_hal.h"
/* ------------------------------------------------------------ */
/*  SF3
 **
 **  Parameters:
 **    none
 **
 **  Return Value:
 **    none
 **
 **  Errors:
 **    none
 **
 **  Description:
 **    starts the spi controller the sf3 uses
 */

/* ------------------------------------------------------------ */
/*  getDeviceID
 **
 **  Parameters:
 **    none
 **
 **  Return Value:
 **
 **
 **  Errors:
 **    none
 **
 **  Description:
 **    queries and recieves the DeviceID of the SF3 (0x19)
 */

extern SPI_HandleTypeDef hspi5;

void resetDevice(){
	uint8_t cmd = COMMAND_RESET_ENABLE;

		waitForWrite();
		writeEnable();
		SF3_CS0();
		HAL_SPI_Transmit(&hspi5,&cmd,1,100);
		SF3_CS1();
		cmd = COMMAND_RESET_MEMORY;

		SF3_CS0();
		HAL_SPI_Transmit(&hspi5,&cmd,1,100);
		SF3_CS1();

}

int getDeviceID(uint8_t * rcvBytes) {
	uint8_t cmd = COMMAND_READ_ID;

	waitForWrite();
	writeEnable();

	SF3_CS0();

	HAL_SPI_Transmit(&hspi5,&cmd,1,100);
	HAL_SPI_Receive(&hspi5,rcvBytes,20,100);

	SF3_CS1();

	return rcvBytes[2];
}

/* ------------------------------------------------------------ */
/*  normalRead
 **
 **  Parameters:
 **    address: address to read
 **
 **  Return Value:
 **    value: byte at selected address
 **
 **  Errors:
 **    none
 **
 **  Description:
 **    reads a byte value from flash memory
 */
uint8_t normalRead(int address) {
	uint8_t sndByte[4]= {0};
	uint8_t rcvByte;
	uint8_t empty = 0;

	sndByte[0] = COMMAND_RANDOM_READ;
	sndByte[1] = (address >> 16);
	sndByte[2] = (address >> 8);
	sndByte[3] = address;

	waitForWrite();
	writeEnable();

	SF3_CS0();

	HAL_SPI_Transmit(&hspi5,&sndByte[0],1,100);
	HAL_SPI_Transmit(&hspi5,&sndByte[1],1,100);
	HAL_SPI_Transmit(&hspi5,&sndByte[2],1,100);
	HAL_SPI_Transmit(&hspi5,&sndByte[3],1,100);
	HAL_SPI_TransmitReceive(&hspi5,&empty,&rcvByte,1,100);

	SF3_CS1();

	return rcvByte;
}

/* ------------------------------------------------------------ */
/*  writePage
 **
 **  Parameters:
 **    uint8_t page: page data to write to the sf3
 **    address: address to read
 **
 **  Return Value:
 **    value: byte at selected address
 **
 **  Errors:
 **    none
 **
 **  Description:
 **    writes a page to memory. see data sheet for consequences
 **    of different start addreesses
 */
void writePage(uint8_t* writeData, int address) {
	uint8_t sndBytes[4] = {0};

	sndBytes[0] = COMMAND_PAGE_PROGRAM;
	sndBytes[1] = (uint16_t)(address >> 16);
	sndBytes[2] = (uint16_t)(address >> 8);
	sndBytes[3] = (uint16_t)address;

	waitForWrite();
	writeEnable();

	SF3_CS0();

	HAL_SPI_Transmit(&hspi5,&sndBytes[0],1,100);
	HAL_SPI_Transmit(&hspi5,&sndBytes[1],1,100);
	HAL_SPI_Transmit(&hspi5,&sndBytes[2],1,100);
	HAL_SPI_Transmit(&hspi5,&sndBytes[3],1,100);


	HAL_SPI_Transmit(&hspi5,writeData,256,100);

	SF3_CS1();
}

/* ------------------------------------------------------------ */
/*  subSectorErase
 **
 **  Parameters:
 **    address: address of sector to erase (any address in sector is valid)
 **
 **  Return Value:
 **    none
 **
 **  Errors:
 **    none
 **
 **  Description:
 **    erases a subsector of memory
 */
void subSectorErase(int address) {
	uint8_t sndBytes[4] = {0};

	sndBytes[0] = COMMAND_SUB_SECTOR_ERASE;
	sndBytes[1] = (address >> 16);
	sndBytes[2] = (address >> 8);
	sndBytes[3] = address;

	waitForWrite();
	writeEnable();

	SF3_CS0();
	HAL_SPI_Transmit(&hspi5,&sndBytes[0],1,100);
	HAL_SPI_Transmit(&hspi5,&sndBytes[1],1,100);
	HAL_SPI_Transmit(&hspi5,&sndBytes[2],1,100);
	HAL_SPI_Transmit(&hspi5,&sndBytes[3],1,100);

	SF3_CS1();

	HAL_Delay(10);
}

/* ------------------------------------------------------------ */
/*  bulkErase
 **
 **  Parameters:
 **    none
 **
 **  Return Value:
 **    none
 **
 **  Errors:
 **    none
 **
 **  Description:
 **    erases all of the memory
 */
void bulkErase() {
	uint8_t cmd = COMMAND_BULK_ERASE;
	writeSR(0x00);

	waitForWrite();
	writeEnable();

	SF3_CS0();

	HAL_SPI_Transmit(&hspi5,&cmd,1,100);

	SF3_CS1();
}

/* ------------------------------------------------------------ */
/*  writeSR
 **
 **  Parameters:
 **    setReg: the byte to write to the status reg
 **
 **  Return Value:
 **    none
 **
 **  Errors:
 **    none
 **
 **  Description:
 **    sets the status register
 */
void writeSR(uint8_t setReg) {
	uint8_t cmd = COMMAND_WRITE_STATUSREG;
	waitForWrite();
	writeEnable();

	SF3_CS0();

	HAL_SPI_Transmit(&hspi5,&cmd,1,100);
	HAL_SPI_Transmit(&hspi5,&setReg,1,100);

	SF3_CS1();
}

/* ------------------------------------------------------------ */
/*  writeEnable
 **
 **  Parameters:
 **    none
 **
 **  Return Value:
 **    none
 **
 **  Errors:
 **    none
 **
 **  Description:
 **    prepares the device to write
 */
void writeEnable() {
	uint8_t cmd = COMMAND_WRITE_ENABLE;
	SF3_CS0();

	HAL_SPI_Transmit(&hspi5,&cmd,1,100);

	SF3_CS1();
}

/* ------------------------------------------------------------ */
/*  readSR
 **
 **  Parameters:
 **    none
 **
 **  Return Value:
 **    none
 **
 **  Errors:
 **    none
 **
 **  Description:
 **    prepares the device to write
 */
uint8_t readSR(void) {
	uint8_t SR;
	uint8_t empty = 0;
	uint8_t cmd = COMMAND_READ_STATUSREG;
	SF3_CS0();

	HAL_SPI_Transmit(&hspi5,&cmd,1,100); // command read status register
	HAL_SPI_TransmitReceive(&hspi5,&empty,&SR,1,100);
	SF3_CS1();

	return SR;
}

/* ------------------------------------------------------------ */
/*  waitForWrite
 **
 **  Parameters:
 **    none
 **
 **  Return Value:
 **    none
 **
 **  Errors:
 **    none
 **
 **  Description:
 **    waits for the status to read that the device can be writtin
 */
void waitForWrite() {
	uint8_t SR;

	SR = readSR();

	while(SR & 0x1)
		SR = readSR();
}

