#ifndef __FILEOPS_H
#define __FILEOPS_H

#include "stm32f429xx.h"
#include "ff.h"
#include "spiffs.h"

#define WORD_SIZE		128
#define WORD_HALF_SIZE	(WORD_SIZE / 2)
#define BYTE_SIZE	(WORD_SIZE * 2)

extern FATFS FatFs;
extern spiffs fs;

void SD_readSingleTrack(FIL *fp);
void SD_WriteAudio(uint32_t start,uint32_t end,FIL *fp);
uint32_t SD_ReadAudio(uint32_t start,FIL *fp);
int16_t * SD_ReadAudioToMemory(uint32_t start,uint32_t end,FIL *fp,int16_t **ptr);
void checkSD();
uint32_t SRAM_download_rhythm(void);

#endif
