/*-----------------------------------------------------------------------/
/  Low level disk interface modlue include file   (C)ChaN, 2013          /
/-----------------------------------------------------------------------*/

#ifndef _DISKIO_DEFINED_SD
#define _DISKIO_DEFINED_SD

#define _USE_WRITE	1	/* 1: Enable disk_write function */
#define _USE_IOCTL	1	/* 1: Enable disk_ioctl fucntion */

#include "diskio.h"
#include "integer.h"

#include "stm32f4xx.h"
#include "defines.h"
#include "main.h"

#ifndef FATFS_SPI
#define FATFS_SPI							SPI5
#define FATFS_SPI_PINSPACK					TM_SPI_PinsPack_1
#endif

#ifndef FATFS_CS_PIN		
#define FATFS_CS_PORT						GPIOB
#define FATFS_CS_PIN						GPIO_PIN_6
#endif

#ifndef FATFS_USE_DETECT_PIN
#define FATFS_USE_DETECT_PIN				0
#endif

#ifndef FATFS_USE_WRITEPROTECT_PIN
#define FATFS_USE_WRITEPROTECT_PIN			0
#endif

#if FATFS_USE_DETECT_PIN > 0
#ifndef FATFS_USE_DETECT_PIN_PIN			
#define FATFS_USE_DETECT_PIN_PORT			GPIOB
#define FATFS_USE_DETECT_PIN_PIN			GPIO_PIN_6
#endif
#endif

#if FATFS_USE_WRITEPROTECT_PIN > 0
#ifndef FATFS_USE_WRITEPROTECT_PIN_PIN
#define FATFS_USE_WRITEPROTECT_PIN_RCC		RCC_AHB1Periph_GPIOB			
#define FATFS_USE_WRITEPROTECT_PIN_PORT		GPIOB
#define FATFS_USE_WRITEPROTECT_PIN_PIN		GPIO_Pin_7
#endif
#endif

#define FATFS_CS_LOW						HAL_GPIO_WritePin(uSD_CS_GPIO_Port,uSD_CS_Pin,GPIO_PIN_RESET)
#define FATFS_CS_HIGH						HAL_GPIO_WritePin(uSD_CS_GPIO_Port,uSD_CS_Pin,GPIO_PIN_SET)


#endif

