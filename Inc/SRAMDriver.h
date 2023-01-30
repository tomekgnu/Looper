#ifndef _SRAM_H
#define _SRAM_H

#include "main.h"
#include "ff.h"

#define		SRAM_0							0
#define		SRAM_1							1
#define		SRAM_2							2
#define		SRAM_3							3
#define		SRAM_4							4
#define		SRAM_5							5
#define		SRAM_6							6
#define		SRAM_7							7

#define  	CMD_SRAMRead   					0x03     	//Read Command for SRAM
#define  	CMD_SRAMWrite  					0x02     	//Write Command for SRAM
#define 	CMD_SRAMRDSR   					0x05     	//Read the status register
#define  	CMD_SRAMWRSR  					0x01     	//Write the status register
#define		SRAMByteMode					0x00
#define		SRAMPageMode					0x80
#define		SRAMSeqMode						0x40
#define		SRAMChips						8
#define		SRAMPageSize					32
#define		SRAMPageCount					4096
#define		SRAMChipSize					(SRAMPageSize * SRAMPageCount)
#define		SRAMTotalSize					(SRAMChipSize * SRAMChips)
#define		SRAMTotalPages					(SRAMPageCount * SRAMChips)
#define		DummyByte						0xFF
#define		SRAM_SET						0
#define		SRAM_END						1
#define		SRAM_CUR						2

union offset{
	uint32_t value;
	uint8_t bytes[4];
};

typedef struct {
	union offset currentByte;
	union offset currentPage;
	union offset totalBytes;
	uint8_t currentSram;
} sramAddress;

extern uint32_t SRAM_read();
extern uint32_t SRAM_written();
extern uint32_t SRAM_readerPosition();
extern uint32_t SRAM_writerPosition();
extern void SRAM_seekRead(unsigned int size,unsigned int whence);
extern void SRAM_seekWrite(unsigned int size,unsigned int whence);
extern void readSRAM(unsigned char *buf,unsigned int size);
extern void writeSRAM(unsigned char *buf,unsigned int size);

extern BOOL checkSRAM();
extern void InitSRAM(void);
extern uint8_t SRAMReadStatusReg(void);
extern uint8_t SRAMWriteStatusReg(uint8_t WriteVal);
extern void SRAMCommand(uint8_t AddLB, uint8_t AddMB,uint8_t AddHB, uint8_t RWCmd);
extern uint8_t SRAMWriteByte(uint8_t AddLB, uint8_t AddMB,uint8_t AddHB, uint8_t WriteData);
extern uint8_t SRAMReadByte(uint8_t AddLB, uint8_t AddMB,uint8_t AddHB);
extern uint32_t SRAMWritePage(uint8_t AddLB, uint8_t AddMB,uint8_t AddHB, uint8_t *WriteData);
extern uint32_t SRAMReadPage(uint8_t AddLB, uint8_t AddMB,uint8_t AddHB, uint8_t *ReadData);
extern uint8_t SRAMWriteSeq(uint8_t AddLB, uint8_t AddMB,uint8_t AddHB, uint8_t *WriteData,uint32_t WriteCnt);
extern uint8_t SRAMReadSeq(uint8_t AddLB, uint8_t AddMB,uint8_t AddHB, uint8_t *ReadData,uint32_t ReadCnt);
extern void SRAM_WriteMultiplePages(uint8_t *data,uint32_t size);

#endif
