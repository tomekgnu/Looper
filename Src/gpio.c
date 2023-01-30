/**
  ******************************************************************************
  * File Name          : gpio.c
  * Description        : This file provides code for the configuration
  *                      of all used GPIO pins.
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

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"
/* USER CODE BEGIN 0 */
#include "stm32f429i_discovery.h"
#include "audio.h"
#include "main.h"
#include "ads1256_test.h"
#include "tm_stm32_hd44780.h"
#include "tm_stm32f4_ili9341.h"
#include "stdlib.h"
#include "adc.h"
#include "midi.h"
#include "drums.h"
#include "memops.h"
#include "tim.h"
#include "dac.h"
#include "joystick.h"
#include "menu.h"

extern uint32_t sdram_pointer;
extern int16_t sample16s;
uint8_t displayTime[16];


/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
     PA11   ------> USB_OTG_FS_DM
     PD6   ------> LTDC_B2
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, SF3_CS_Pin|KEYPAD_COLUMN_1_Pin|KEYPAD_COLUMN_2_Pin|KEYPAD_COLUMN_3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(VS1053_RESET_GPIO_Port, VS1053_RESET_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, KEYPAD_COLUMN_4_Pin|SRAM_3_CS_Pin|ILI9341_CS_Pin|OTG_FS_PSO_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, uSD_CS_Pin|KeyClick_SCK_Pin|HD44780_D6_Pin|HD44780_D5_Pin 
                          |HD44780_D4_Pin|HD44780_E_Pin|ADS1256_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, SRAM_2_CS_Pin|SRAM_4_CS_Pin|HD44780_RS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(ACP_RST_GPIO_Port, ACP_RST_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, SRAM_5_CS_Pin|SRAM_6_CS_Pin|SRAM_7_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(KeyClick_SHLD_GPIO_Port, KeyClick_SHLD_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, ILI9341_RST_Pin|ILI9341_WRX_Pin|DAC8552_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOG, SRAM_1_CS_Pin|SRAM_0_CS_Pin|LD3_Pin|LD4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(HD44780_D7_GPIO_Port, HD44780_D7_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, ADS1256_SYNC_Pin|ADS1256_RESET_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : PEPin PEPin PEPin PEPin */
  GPIO_InitStruct.Pin = SF3_CS_Pin|KEYPAD_COLUMN_1_Pin|KEYPAD_COLUMN_2_Pin|KEYPAD_COLUMN_3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = VS1053_RESET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(VS1053_RESET_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PCPin PCPin PCPin PCPin */
  GPIO_InitStruct.Pin = KEYPAD_COLUMN_4_Pin|SRAM_3_CS_Pin|ILI9341_CS_Pin|OTG_FS_PSO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PFPin PFPin */
  GPIO_InitStruct.Pin = Recording_Pin|AD_KBD_BUT_DOWN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pins : PCPin PCPin PCPin */
  GPIO_InitStruct.Pin = uSD_CS_Pin|KeyClick_SCK_Pin|ADS1256_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PAPin PAPin */
  GPIO_InitStruct.Pin = SRAM_2_CS_Pin|SRAM_4_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = ACP_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(ACP_RST_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PBPin PBPin PBPin */
  GPIO_InitStruct.Pin = SRAM_5_CS_Pin|SRAM_6_CS_Pin|SRAM_7_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = BOOT1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BOOT1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = KeyClick_SHLD_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(KeyClick_SHLD_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = TE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(TE_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PDPin PDPin PDPin */
  GPIO_InitStruct.Pin = ILI9341_RST_Pin|ILI9341_WRX_Pin|DAC8552_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : PGPin PGPin PGPin PGPin */
  GPIO_InitStruct.Pin = SRAM_1_CS_Pin|SRAM_0_CS_Pin|LD3_Pin|LD4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pins : PGPin PGPin */
  GPIO_InitStruct.Pin = KeyClick_DATA_Pin|uSD_CD_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = HD44780_D7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(HD44780_D7_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PCPin PCPin PCPin PCPin */
  GPIO_InitStruct.Pin = HD44780_D6_Pin|HD44780_D5_Pin|HD44780_D4_Pin|HD44780_E_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = HD44780_RS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(HD44780_RS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PA11 */
  GPIO_InitStruct.Pin = GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = Joystick_SW_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(Joystick_SW_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = ADS1256_DRDY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(ADS1256_DRDY_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PDPin PDPin */
  GPIO_InitStruct.Pin = ADS1256_SYNC_Pin|ADS1256_RESET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = B2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
  HAL_GPIO_Init(B2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = Overdubbing_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(Overdubbing_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = Playback_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(Playback_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = KEYPAD_ROW_3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(KEYPAD_ROW_3_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = AD_KBD_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(AD_KBD_INT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = ToggleFunction_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ToggleFunction_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PBPin PBPin PBPin */
  GPIO_InitStruct.Pin = KEYPAD_ROW_2_Pin|KEYPAD_ROW_1_Pin|KEYPAD_ROW_4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 1);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 2, 3);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 2, 3);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 1);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 2 */

void KeyboardConfig(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	/* Configure keyboard pins as input */
	  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5;
	  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	  GPIO_InitStruct.Pull = GPIO_PULLUP;
	  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	  HAL_GPIO_WritePin(GPIOE,GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5,GPIO_PIN_SET);

	  /* Pins to select keyboard rows as output */
	  GPIO_InitStruct.Pin = GPIO_PIN_6;
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	  HAL_GPIO_WritePin(GPIOE,GPIO_PIN_6,GPIO_PIN_RESET);

	  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_8;
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_RESET);

	  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_7;
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4|GPIO_PIN_7,GPIO_PIN_RESET);


}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {

	switch (GPIO_Pin) {

		case Joystick_SW_Pin:
				if(IS_BUT_DOWN(BUT_JOYSTICK))
					return;
				BUT_DOWN(BUT_JOYSTICK);
				Update_Joystick();
				if(looper.StartLooper == TRUE)
					looper.StartLooper = FALSE;
				else
					looper.StartLooper = TRUE;
				if(looper.DrumState == DRUMS_STARTED)
					looper.DrumState = DRUMS_PAUSED;
				else if(looper.DrumState == DRUMS_PAUSED)
					looper.DrumState = DRUMS_STARTED;
				break;
		case GPIO_PIN_0:	// user button stops everything
				looper.Function = saveFunction;
				stopAll();
				break;

		case ADS1256_DRDY_Pin:

			if(looper.StartLooper == FALSE || looper.Function == IDLE)
				return;

			sample16s = (int16_t)(ADS1256_ReadData() >> 8);

			if(looper.Playback == TRUE){
				if(looper.TwoChannels == FALSE){
					read_sample(sample16s,GET_ACTIVE_CHANNEL);
					play_sample(GET_ACTIVE_CHANNEL);
				}
				else {
					read_samples(sample16s,&looper.ch1,&looper.ch2);
					play_samples(&looper.ch1,&looper.ch2);
				}

				return;
			}


			if(looper.Recording == TRUE){

				if(looper.TwoChannels == FALSE){
					record_sample(sample16s,GET_ACTIVE_CHANNEL);
				}
				else {
					record_samples(sample16s,&looper.ch1,&looper.ch2);
					if(pattern_audio_map[looper.StartPattern].channel_recorded[INACTIVE_CHANNEL_INDEX] == TRUE)
						play_sample(GET_INACTIVE_CHANNEL);
				}

			}

			break;

		case Recording_Pin:
			if(IS_BUT_DOWN(BUT_REC) == TRUE)
				return;
			BUT_DOWN(BUT_REC);
			if(looper.Recording == TRUE || looper.Function == IDLE)
				return;
			if(looper.DrumState == DRUMS_STARTED)
				return;
			looper.StartLooper = FALSE;
			//if(looper.Function == AUDIO_ONLY)
				//resetChannel(GET_ACTIVE_CHANNEL);

			BSP_LED_On(LED_RED);
			BSP_LED_Off(LED_GREEN);
			looper.ch1.Overdub = FALSE;
			looper.ch2.Overdub = FALSE;
			looper.Recording = TRUE;
			looper.Playback = FALSE;

			// start counting samples from current loop's start
			setSampleCounter(pattern_audio_map[looper.StartPattern].sample_position);
			setStartEndPatterns(looper.StartPattern,looper.EndPattern);
			looper.StartLooper = TRUE;
			show_status_line = TRUE;
			break;
		case Playback_Pin:
			if(IS_BUT_DOWN(BUT_PLAY) == TRUE)
				return;
			BUT_DOWN(BUT_PLAY);
			if(looper.Playback == TRUE || looper.Function == IDLE)
				return;
			if(looper.DrumState == DRUMS_STARTED)
				return;
			looper.StartLooper = FALSE;
			if(looper.ch1.Active == TRUE){
				looper.ch1.Clipping = FALSE;
				//looper.ch1.Overdub = FALSE;
				looper.ch1.CurrentSample = 0;
			}
			if(looper.ch2.Active == TRUE){
				looper.ch2.Clipping = FALSE;
				//looper.ch2.Overdub = FALSE;
				looper.ch2.CurrentSample = 0;
			}

			// this sequence is important!!
			looper.Playback = TRUE;
			setStartEndPatterns(looper.StartPattern,looper.EndPattern);
			looper.Recording = FALSE;

			if(looper.SamplesWritten == 0){
				looper.Playback = FALSE;
				return;
			}
			pattern_audio_map[looper.StartPattern].channel_recorded[ACTIVE_CHANNEL_INDEX] = TRUE;

			looper.StartLooper = TRUE;
			show_status_line = TRUE;
			BSP_LED_On(LED_GREEN);
			BSP_LED_Off(LED_RED);
			break;
		case Overdubbing_Pin:
			if(IS_BUT_DOWN(BUT_OVERDUB) == TRUE)
				return;
			BUT_DOWN(BUT_OVERDUB);
			if(looper.Recording == TRUE || looper.Function == IDLE)
				return;
			if(looper.SamplesWritten == 0)
				return;
			if(looper.TwoChannels == TRUE)
				return;
			if(looper.ch1.Active == TRUE){
				if(looper.ch1.Overdub == FALSE)
					looper.ch1.Overdub = TRUE;
				else
					looper.ch1.Overdub = FALSE;
			}
			else if(looper.ch2.Active == TRUE){
				if(looper.ch2.Overdub == FALSE)
					looper.ch2.Overdub = TRUE;
				else
					looper.ch2.Overdub = FALSE;
			}
			show_status_line = TRUE;
			break;
		case ToggleFunction_Pin:
			if(IS_BUT_DOWN(BUT_TOGFUN) == TRUE)
				return;
			BUT_DOWN(BUT_TOGFUN);
			// finish recording without playing immediately. Allows to switch on overdubbing
			if(looper.Recording == TRUE){
				// this sequence is important!!
				looper.Playback = TRUE;
				setStartEndPatterns(looper.StartPattern,looper.EndPattern);
				looper.Recording = FALSE;
				looper.Playback = FALSE;
				show_status_line = TRUE;
				return;
			}
			if(looper.Function == AUDIO_ONLY)
				pauseLoop();
			show_status_line = TRUE;
			break;


		}

}
/* USER CODE END 2 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
