/**
  ******************************************************************************
  * @file    Audio/Audio_playback_and_record/Src/waveplayer.c 
  * @author  MCD Application Team
  * @version V1.3.5
  * @date    04-November-2016
  * @brief   I2S Audio player program. 
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright © 2016 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
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
#include "main.h"
#include "stm32f429i_discovery.h"
#include "ff.h"
#include "waveplayer.h"
#include "drums.h"

uint16_t snare[SNARE_SAMPLES];
uint16_t kick[KICK_SAMPLES];
uint16_t tomhi[TOM_HI_SAMPLES];
uint16_t tomlo[TOM_LO_SAMPLES];
uint16_t crash[CRASH_SAMPLES];
uint16_t hihat[HIHAT_SAMPLES];

extern char SD_Path[];
extern uint8_t retSD;
DIR Directory;
FIL snareFile;
FIL kickFile;
FIL crashFile;
FIL tomhiFile;
FIL tomloFile;
FIL hihatFile;

void ReadDrumSamples(void)
{
  FATFS mynewdiskFatFs;
  WAVE_FormatTypeDef waveformat;
  char* wavefilename = NULL;
  UINT wbytes;
  /* File write counts */

  /* File system object for User logical drive */

  if(retSD == 0)
    {
      if(f_mount(&mynewdiskFatFs, (TCHAR const*)SD_Path, 0) != FR_OK)
      {
    	  Error_Handler();

       }
     }

  /* Get the read out protection status */
  if(f_opendir(&Directory, SD_Path) != FR_OK)
  {
	  BSP_LED_On(RED);
	  Error_Handler();
   }
    /* Open drum sample files */
    if(f_open(&kickFile, "kick.raw" , FA_READ) != FR_OK)
    {
      BSP_LED_On(RED);
      Error_Handler();
    }
    else
    {    
      /* Read sizeof(WaveFormat) from the selected file */
      f_read (&kickFile, (uint8_t *)kick, sizeof(kick), &wbytes);
      f_close(&kickFile);

    }
    if(f_open(&snareFile, "snare.raw" , FA_READ) != FR_OK)
        {
          BSP_LED_On(RED);
          Error_Handler();
        }
        else
        {
          /* Read sizeof(WaveFormat) from the selected file */
          f_read (&snareFile, (uint8_t *)snare, sizeof(snare), &wbytes);
          f_close(&snareFile);
        }
    if(f_open(&hihatFile, "hihat.raw" , FA_READ) != FR_OK)
        {
          BSP_LED_On(RED);
          Error_Handler();
        }
        else
        {
          /* Read sizeof(WaveFormat) from the selected file */
          f_read (&hihatFile, (uint8_t *)hihat, sizeof(hihat), &wbytes);
          f_close(&hihatFile);
        }
    if(f_open(&crashFile, "crash.raw" , FA_READ) != FR_OK)
        {
          BSP_LED_On(RED);
          Error_Handler();
        }
        else
        {
          /* Read sizeof(WaveFormat) from the selected file */
          f_read (&crashFile, (uint8_t *)kick, sizeof(kick), &wbytes);
          f_close(&crashFile);

        }
    if(f_open(&tomloFile, "tomlo.raw" , FA_READ) != FR_OK)
        {
          BSP_LED_On(RED);
          Error_Handler();
        }
        else
        {
          /* Read sizeof(WaveFormat) from the selected file */
          f_read (&tomloFile, (uint8_t *)kick, sizeof(kick), &wbytes);
          f_close(&tomloFile);
        }
    	if(f_open(&tomhiFile, "tomhi.raw" , FA_READ) != FR_OK)
          {
            BSP_LED_On(RED);
            Error_Handler();
          }
          else
          {
            /* Read sizeof(WaveFormat) from the selected file */
            f_read (&tomhiFile, (uint8_t *)kick, sizeof(kick), &wbytes);
            f_close(&tomhiFile);
          }

  }


