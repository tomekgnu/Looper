/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2019 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H__
#define __MAIN_H__

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* USER CODE BEGIN Includes */
#include "stm32f4xx.h"
#include "stdint.h"
#include "audio.h"
#include "tm_stm32f4_keypad.h"

#define SAMPLE_SIZE			4
#define SAMPLE_ARRAY		1024
#define SAMPLE_BYTES		SAMPLE_ARRAY * SAMPLE_SIZE
#define READ_SIZE			((SAMPLE_BYTES) / 4)
#define WRITE_SIZE			((SAMPLE_BYTES) / 4)
#define MEM_BLOCK			READ_SIZE

typedef enum{DRUMS_STOPPED,DRUMS_STARTED,DRUMS_READY,DRUMS_PAUSED,DRUMS_RECORD}DrumFunction;

typedef enum{KEY_UP,KEY_DOWN,KEY_LEFT,KEY_RIGHT,KEY_NONE}KeyDir;
typedef enum {
	FALSE, TRUE
} BOOL;
typedef enum {
	BUT_UP, BUT_DOWN
} BUTTON;
typedef enum {
	IDLE,
	AUDIO_ONLY,
	AUDIO_DRUMS,
	DRUMS_ONLY,
	DOWNLOAD_SRAM,
	PLAY_SONG,
	PLAY_SD
} FUNCTION;
//typedef enum{NONE,SINGLE_CHANNEL,CHANNEL_A,CHANNEL_B,READ_SD,PLAY_SD,READ_SF3,PLAY_SF3,READ_SRAM,PLAY_SRAM,DOWNLOAD_SRAM} FUNCTION;
typedef uint8_t byte;

// ----- Global structure for looper application
typedef struct {
	__IO BOOL Recording;
	__IO BOOL Playback;
	__IO BOOL StartLooper;
	__IO BOOL TwoChannels;
	__IO BOOL PlayBass;
	__IO BOOL Metronome;
	__IO uint32_t SamplesRead;
	__IO uint32_t SamplesWritten;
	__IO uint32_t SampleBytes;
	__IO uint32_t StartPattern;
	__IO uint32_t EndPattern;
	__IO DrumFunction DrumState;
	__IO FUNCTION Function;
	__IO CHANNEL ch1;
	__IO CHANNEL ch2;
	__IO int32_t timeIncrement;	// if negative - plays faster, positive - slower
} LooperApplication;



void buttonHandler();
void SystemClock_Config(void);
void Error_Handler(void);
void set_function(FUNCTION fun);

extern LooperApplication looper;
extern BOOL fs_mounted;
extern uint8_t switches;
extern char lcdline[];
extern uint32_t joystick_data[];
extern BOOL show_status_line;
extern FUNCTION saveFunction;
extern uint32_t sdram_pointer;

#define delayUS_ASM(us) do {\
	asm volatile (	"MOV R0,%[loops]\n\t"\
			"1: \n\t"\
			"SUB R0, #1\n\t"\
			"CMP R0, #0\n\t"\
			"BNE 1b \n\t" : : [loops] "r" (16*us) : "memory"\
		      );\
} while(0)
/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define SF3_CS_Pin GPIO_PIN_2
#define SF3_CS_GPIO_Port GPIOE
#define VS1053_RESET_Pin GPIO_PIN_3
#define VS1053_RESET_GPIO_Port GPIOE
#define KEYPAD_COLUMN_1_Pin GPIO_PIN_4
#define KEYPAD_COLUMN_1_GPIO_Port GPIOE
#define KEYPAD_COLUMN_2_Pin GPIO_PIN_5
#define KEYPAD_COLUMN_2_GPIO_Port GPIOE
#define KEYPAD_COLUMN_3_Pin GPIO_PIN_6
#define KEYPAD_COLUMN_3_GPIO_Port GPIOE
#define KEYPAD_COLUMN_4_Pin GPIO_PIN_13
#define KEYPAD_COLUMN_4_GPIO_Port GPIOC
#define PC14_OSC32_IN_Pin GPIO_PIN_14
#define PC14_OSC32_IN_GPIO_Port GPIOC
#define PC15_OSC32_OUT_Pin GPIO_PIN_15
#define PC15_OSC32_OUT_GPIO_Port GPIOC
#define A0_Pin GPIO_PIN_0
#define A0_GPIO_Port GPIOF
#define A1_Pin GPIO_PIN_1
#define A1_GPIO_Port GPIOF
#define A2_Pin GPIO_PIN_2
#define A2_GPIO_Port GPIOF
#define A3_Pin GPIO_PIN_3
#define A3_GPIO_Port GPIOF
#define A4_Pin GPIO_PIN_4
#define A4_GPIO_Port GPIOF
#define A5_Pin GPIO_PIN_5
#define A5_GPIO_Port GPIOF
#define Recording_Pin GPIO_PIN_6
#define Recording_GPIO_Port GPIOF
#define Recording_EXTI_IRQn EXTI9_5_IRQn
#define AD_KBD_BUT_DOWN_Pin GPIO_PIN_10
#define AD_KBD_BUT_DOWN_GPIO_Port GPIOF
#define AD_KBD_BUT_DOWN_EXTI_IRQn EXTI15_10_IRQn
#define PH0_OSC_IN_Pin GPIO_PIN_0
#define PH0_OSC_IN_GPIO_Port GPIOH
#define PH1_OSC_OUT_Pin GPIO_PIN_1
#define PH1_OSC_OUT_GPIO_Port GPIOH
#define SDNWE_Pin GPIO_PIN_0
#define SDNWE_GPIO_Port GPIOC
#define SRAM_3_CS_Pin GPIO_PIN_1
#define SRAM_3_CS_GPIO_Port GPIOC
#define ILI9341_CS_Pin GPIO_PIN_2
#define ILI9341_CS_GPIO_Port GPIOC
#define uSD_CS_Pin GPIO_PIN_3
#define uSD_CS_GPIO_Port GPIOC
#define Joystick_X_Pin GPIO_PIN_1
#define Joystick_X_GPIO_Port GPIOA
#define Joystick_Y_Pin GPIO_PIN_2
#define Joystick_Y_GPIO_Port GPIOA
#define SRAM_2_CS_Pin GPIO_PIN_3
#define SRAM_2_CS_GPIO_Port GPIOA
#define AD_KBD_Pin GPIO_PIN_4
#define AD_KBD_GPIO_Port GPIOA
#define SRAM_4_CS_Pin GPIO_PIN_6
#define SRAM_4_CS_GPIO_Port GPIOA
#define ACP_RST_Pin GPIO_PIN_7
#define ACP_RST_GPIO_Port GPIOA
#define OTG_FS_PSO_Pin GPIO_PIN_4
#define OTG_FS_PSO_GPIO_Port GPIOC
#define KeyClick_SCK_Pin GPIO_PIN_5
#define KeyClick_SCK_GPIO_Port GPIOC
#define SRAM_5_CS_Pin GPIO_PIN_0
#define SRAM_5_CS_GPIO_Port GPIOB
#define SRAM_6_CS_Pin GPIO_PIN_1
#define SRAM_6_CS_GPIO_Port GPIOB
#define BOOT1_Pin GPIO_PIN_2
#define BOOT1_GPIO_Port GPIOB
#define SDNRAS_Pin GPIO_PIN_11
#define SDNRAS_GPIO_Port GPIOF
#define A6_Pin GPIO_PIN_12
#define A6_GPIO_Port GPIOF
#define A7_Pin GPIO_PIN_13
#define A7_GPIO_Port GPIOF
#define A8_Pin GPIO_PIN_14
#define A8_GPIO_Port GPIOF
#define A9_Pin GPIO_PIN_15
#define A9_GPIO_Port GPIOF
#define A10_Pin GPIO_PIN_0
#define A10_GPIO_Port GPIOG
#define A11_Pin GPIO_PIN_1
#define A11_GPIO_Port GPIOG
#define D4_Pin GPIO_PIN_7
#define D4_GPIO_Port GPIOE
#define D5_Pin GPIO_PIN_8
#define D5_GPIO_Port GPIOE
#define D6_Pin GPIO_PIN_9
#define D6_GPIO_Port GPIOE
#define D7_Pin GPIO_PIN_10
#define D7_GPIO_Port GPIOE
#define D8_Pin GPIO_PIN_11
#define D8_GPIO_Port GPIOE
#define D9_Pin GPIO_PIN_12
#define D9_GPIO_Port GPIOE
#define D10_Pin GPIO_PIN_13
#define D10_GPIO_Port GPIOE
#define D11_Pin GPIO_PIN_14
#define D11_GPIO_Port GPIOE
#define D12_Pin GPIO_PIN_15
#define D12_GPIO_Port GPIOE
#define SRAM_7_CS_Pin GPIO_PIN_10
#define SRAM_7_CS_GPIO_Port GPIOB
#define KeyClick_SHLD_Pin GPIO_PIN_11
#define KeyClick_SHLD_GPIO_Port GPIOB
#define OTG_FS_ID_Pin GPIO_PIN_12
#define OTG_FS_ID_GPIO_Port GPIOB
#define VBUS_FS_Pin GPIO_PIN_13
#define VBUS_FS_GPIO_Port GPIOB
#define OTG_FS_DP_Pin GPIO_PIN_15
#define OTG_FS_DP_GPIO_Port GPIOB
#define D15_Pin GPIO_PIN_10
#define D15_GPIO_Port GPIOD
#define TE_Pin GPIO_PIN_11
#define TE_GPIO_Port GPIOD
#define ILI9341_RST_Pin GPIO_PIN_12
#define ILI9341_RST_GPIO_Port GPIOD
#define ILI9341_WRX_Pin GPIO_PIN_13
#define ILI9341_WRX_GPIO_Port GPIOD
#define D0_Pin GPIO_PIN_14
#define D0_GPIO_Port GPIOD
#define D1_Pin GPIO_PIN_15
#define D1_GPIO_Port GPIOD
#define SRAM_1_CS_Pin GPIO_PIN_2
#define SRAM_1_CS_GPIO_Port GPIOG
#define SRAM_0_CS_Pin GPIO_PIN_3
#define SRAM_0_CS_GPIO_Port GPIOG
#define BA0_Pin GPIO_PIN_4
#define BA0_GPIO_Port GPIOG
#define KeyClick_DATA_Pin GPIO_PIN_6
#define KeyClick_DATA_GPIO_Port GPIOG
#define HD44780_D7_Pin GPIO_PIN_7
#define HD44780_D7_GPIO_Port GPIOG
#define SDCLK_Pin GPIO_PIN_8
#define SDCLK_GPIO_Port GPIOG
#define HD44780_D6_Pin GPIO_PIN_6
#define HD44780_D6_GPIO_Port GPIOC
#define HD44780_D5_Pin GPIO_PIN_7
#define HD44780_D5_GPIO_Port GPIOC
#define HD44780_D4_Pin GPIO_PIN_8
#define HD44780_D4_GPIO_Port GPIOC
#define HD44780_E_Pin GPIO_PIN_9
#define HD44780_E_GPIO_Port GPIOC
#define HD44780_RS_Pin GPIO_PIN_8
#define HD44780_RS_GPIO_Port GPIOA
#define STLINK_RX_Pin GPIO_PIN_9
#define STLINK_RX_GPIO_Port GPIOA
#define STLINK_TX_Pin GPIO_PIN_10
#define STLINK_TX_GPIO_Port GPIOA
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define Joystick_SW_Pin GPIO_PIN_15
#define Joystick_SW_GPIO_Port GPIOA
#define Joystick_SW_EXTI_IRQn EXTI15_10_IRQn
#define ADS1256_CS_Pin GPIO_PIN_10
#define ADS1256_CS_GPIO_Port GPIOC
#define ADS1256_MISO_Pin GPIO_PIN_11
#define ADS1256_MISO_GPIO_Port GPIOC
#define ADS1256_MOSI_Pin GPIO_PIN_12
#define ADS1256_MOSI_GPIO_Port GPIOC
#define ADS1256_DRDY_Pin GPIO_PIN_2
#define ADS1256_DRDY_GPIO_Port GPIOD
#define ADS1256_DRDY_EXTI_IRQn EXTI2_IRQn
#define ADS1256_SYNC_Pin GPIO_PIN_3
#define ADS1256_SYNC_GPIO_Port GPIOD
#define ADS1256_RESET_Pin GPIO_PIN_4
#define ADS1256_RESET_GPIO_Port GPIOD
#define DAC8552_CS_Pin GPIO_PIN_5
#define DAC8552_CS_GPIO_Port GPIOD
#define B2_Pin GPIO_PIN_6
#define B2_GPIO_Port GPIOD
#define Overdubbing_Pin GPIO_PIN_7
#define Overdubbing_GPIO_Port GPIOD
#define Overdubbing_EXTI_IRQn EXTI9_5_IRQn
#define Playback_Pin GPIO_PIN_9
#define Playback_GPIO_Port GPIOG
#define Playback_EXTI_IRQn EXTI9_5_IRQn
#define KEYPAD_ROW_3_Pin GPIO_PIN_10
#define KEYPAD_ROW_3_GPIO_Port GPIOG
#define AD_KBD_INT_Pin GPIO_PIN_11
#define AD_KBD_INT_GPIO_Port GPIOG
#define AD_KBD_INT_EXTI_IRQn EXTI15_10_IRQn
#define uSD_CD_Pin GPIO_PIN_12
#define uSD_CD_GPIO_Port GPIOG
#define LD3_Pin GPIO_PIN_13
#define LD3_GPIO_Port GPIOG
#define LD4_Pin GPIO_PIN_14
#define LD4_GPIO_Port GPIOG
#define SDNCAS_Pin GPIO_PIN_15
#define SDNCAS_GPIO_Port GPIOG
#define ADS1256_SCK_Pin GPIO_PIN_3
#define ADS1256_SCK_GPIO_Port GPIOB
#define ToggleFunction_Pin GPIO_PIN_4
#define ToggleFunction_GPIO_Port GPIOB
#define ToggleFunction_EXTI_IRQn EXTI4_IRQn
#define SDCKE1_Pin GPIO_PIN_5
#define SDCKE1_GPIO_Port GPIOB
#define SDNE1_Pin GPIO_PIN_6
#define SDNE1_GPIO_Port GPIOB
#define KEYPAD_ROW_2_Pin GPIO_PIN_7
#define KEYPAD_ROW_2_GPIO_Port GPIOB
#define KEYPAD_ROW_1_Pin GPIO_PIN_8
#define KEYPAD_ROW_1_GPIO_Port GPIOB
#define KEYPAD_ROW_4_Pin GPIO_PIN_9
#define KEYPAD_ROW_4_GPIO_Port GPIOB
#define NBL1_Pin GPIO_PIN_1
#define NBL1_GPIO_Port GPIOE

/* ########################## Assert Selection ############################## */
/**
  * @brief Uncomment the line below to expanse the "assert_param" macro in the 
  *        HAL drivers code
  */
/* #define USE_FULL_ASSERT    1U */

/* USER CODE BEGIN Private defines */
#define SDRAM_SIZE			((uint32_t)0x800000)

typedef enum {
	LED_GREEN = 0, LED_RED = 1
} Led_Colors;

#define BUT_REC				1
#define BUT_PLAY			2
#define BUT_OVERDUB			4
#define BUT_TOGFUN			8
#define BUT_JOYSTICK		16
#define BUT_DOWN(BUT)		switches |= BUT
#define BUT_UP(BUT)			switches ^= BUT
#define IS_BUT_DOWN(BUT)	((switches & BUT) > 0)

typedef struct {
	uint8_t manufacturer;
	uint8_t memory;
	uint8_t capacity;
	uint8_t length;
	uint8_t extID[2];
	uint8_t optional[14];

} SF3ID;
/* USER CODE END Private defines */

#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
