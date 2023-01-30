/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;                                                                     
;                     Software License Agreement                      
;                                                                     
;     ©2007 Microchip Technology Inc
;     Mirochip Technology Inc. ("Microchip") licenses this software to 
;     you solely for the use with Microchip Products. The software is
;     owned by Microchip and is protected under applicable copyright
;     laws. All rights reserved.
;
;     SOFTWARE IS PROVIDED "AS IS." MICROCHIP EXPRESSLY DISCLAIMS ANY
;     WARRANTY OF ANY KIND, WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT
;     LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
;     PARTICULAR PURPOSE, OR NON-INFRINGEMENT. IN NO EVENT SHALL MICROCHIP
;     BE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR CONSEQUENTIAL
;     DAMAGES, LOST PROFITS OR LOST DATA, HARM TO YOUR EQUIPMENT, COST OF
;     PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS
;     BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF),
;     ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, OR OTHER SIMILAR COSTS.
;     
;                                                                
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	Filename:			    	Main.c            
;	Date:				    	January 30, 2009
;	File Version:		  		1.0                             
;	Compiler Used:			MPLAB IDE 8.20
; 	Author:			  	    	Pinakin K Makwana
;	Company:			    	Microchip Technology, Inc.
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include<plib.h>
#include <string.h>
#include "SRAMDriver.h"

// Configuration Bit settings
// SYSCLK = 80 MHz (8MHz Crystal/ FPLLIDIV * FPLLMUL / FPLLODIV)
// PBCLK = 40 MHz
// Primary Osc w/PLL (XT+,HS+,EC+PLL)
// WDT OFF
// Other options are don't care
//
#pragma config FPLLMUL = MUL_20, FPLLIDIV = DIV_2, FPLLODIV = DIV_1, FWDTEN = OFF
#pragma config POSCMOD = HS, FNOSC = PRIPLL, FPBDIV = DIV_1
	
int main(void)
{
	unsigned char ReadVal,Cnt;
	unsigned char SRAMBuf[SRAMPageSize];
	// Configure the device for maximum performance but do not change the PBDIV
	// Given the options, this function will change the flash wait states, RAM
	// wait state and enable prefetch cache but will not change the PBDIV.
	// The PBDIV value is already set via the pragma FPBDIV option above..
	SYSTEMConfig(SYS_FREQ, SYS_CFG_WAIT_STATES | SYS_CFG_PCACHE);
	
	InitSRAM();

	while(1)
	{
		//Select Byte mode in SRAM
	
		SRAMWriteStatusReg(SRAMPageMode);
				
		ReadVal = SRAMReadStatusReg();
	
		//Write Byte operation
		//Write 0x28 to 0x0010 memory location of SRAM
		SRAMWriteByte(0x00,0x10,0x28);
		ReadVal = 0x00;
		
		//Read Byte operation
		//Read 0x0010 memory location of SRAM
		ReadVal = SRAMReadByte(0x00,0x10);
		
		//Page Write operation
		//Write 32bytes from SRAMBuf array to first page of SRAM
		SRAMWriteStatusReg(SRAMPageMode);
		for(Cnt = 0;Cnt<32;Cnt++)
		{
			SRAMBuf[Cnt] = (0xAA - Cnt);
		}
	
		SRAMWritePage(0x00,0x20,SRAMBuf);
		
		//Page Read operation
		//Read 32bytes from SRAMBuf array from first page of SRAM
		memset(SRAMBuf,0,sizeof(SRAMBuf));	//Reset SRAMBuf location to 0x00 value
		
		SRAMReadPage(0x00,0x20,SRAMBuf);
		
		//Sequential Write operation
		//Write 10bytes from SRAMBuf to SRAM starting from 0x0010 memory location
		memset(SRAMBuf,0,sizeof(SRAMBuf));	//Reset SRAMBuf location to 0x00 value
		for(Cnt = 0;Cnt<32;Cnt++)
		{
			SRAMBuf[Cnt] =(0x55-Cnt);
		}
		SRAMWriteStatusReg(SRAMSeqMode);
		
		SRAMWriteSeq(0x10,0x10,SRAMBuf,10);
		
		//Sequential Read operation
		//Read 10bytes starting from 0x1010 memory location of SRAM and store it to SRAMBuf array
		memset(SRAMBuf,0,sizeof(SRAMBuf));	//Reset SRAMBuf location to 0x00 value
		
		SRAMReadSeq(0x10,0x10,SRAMBuf,10);
		
	}
	
}
