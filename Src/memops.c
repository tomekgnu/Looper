#include "audio.h"
#include "memops.h"
#include "main.h"
#include "stm32f429i_discovery_sdram.h"
#include "tm_stm32f4_ili9341.h"
#include "tm_stm32f4_keypad.h"
#include "tm_stm32_hd44780.h"
#include "spiffs.h"
#include "spiffs_nucleus.h"
#include "fatfs_sd.h"
#include "dma.h"
#include "tim.h"
#include "dac.h"
#include "SRAMDriver.h"
#include "usbd_cdc_if.h"
#include "stdlib.h"
#include "string.h"
#include "drums.h"

static __IO uint8_t play_buffer = 0;					//Keeps track of which buffer is currently being used
static __IO BOOL need_new_data = FALSE;
static __IO uint32_t word_count = 0;
int16_t audio_buf[WORD_SIZE];
uint32_t audio_pointer = 0;
BOOL audio_finished = FALSE;
FIL *fil;
static UINT bytes_read;
static UINT bytes_written;
__IO int16_t note;
extern uint32_t sdram_pointer;
extern TM_KEYPAD_Button_t Keypad_Button;

void HAL_DACEx_ConvCpltCallbackCh2(DAC_HandleTypeDef* hdac){
	need_new_data = TRUE;
	play_buffer = 0;
	return;
}

void HAL_DACEx_ConvHalfCpltCallbackCh2(DAC_HandleTypeDef* hdac){
	need_new_data = TRUE;
	play_buffer = 1;
	return;
}


void SD_readSingleTrack(FIL *fp){

	f_read(fp,(uint8_t *)audio_buf,WORD_SIZE,&bytes_read);
	f_read(fp,(uint8_t *)audio_buf + WORD_SIZE,WORD_SIZE,&bytes_read);
	signed16_unsigned12(audio_buf,0,WORD_SIZE);
	play_buffer = 0;
	word_count = 0;
	need_new_data = FALSE;
	while(looper.Function != PLAY_SD)	// wait for function switch
		 continue;


	HAL_DAC_Start(&hdac,DAC_CHANNEL_2);
	HAL_DAC_Start_DMA(&hdac,DAC_CHANNEL_2,(uint32_t*)audio_buf,WORD_SIZE,DAC_ALIGN_12B_R);
	HAL_TIM_Base_Start_IT(&htim8);

	while(looper.Function == PLAY_SD){
		 while(need_new_data == FALSE){
			 if(looper.Function != PLAY_SD){
				 HAL_DAC_Stop_DMA(&hdac,DAC_CHANNEL_2);
				 HAL_DAC_Stop(&hdac,DAC_CHANNEL_2);
				 HAL_TIM_Base_Stop_IT(&htim8);
				 return;
			 }
		 }

		need_new_data = FALSE;
		if(play_buffer == 0)	//play_buffer indicates which buffer is now empty
	    {
	   		f_read(fp, (uint8_t *)audio_buf + WORD_SIZE, WORD_SIZE,&bytes_read);
	   		signed16_unsigned12(audio_buf,WORD_HALF_SIZE,WORD_SIZE);
	    }
	     else
	     {
	    	 f_read(fp, (uint8_t *)audio_buf,WORD_SIZE,&bytes_read);
	    	 signed16_unsigned12(audio_buf,0,WORD_HALF_SIZE);
	     }	     	//new_buffer_ready flag tells the ISR that the buffer has been filled.
	     //If file_read returns 0 or -1 file is over. Find the next file!
	      if(bytes_read == 0)
	      {
	    	HAL_DAC_Stop_DMA(&hdac,DAC_CHANNEL_2);
	    	HAL_DAC_Stop(&hdac,DAC_CHANNEL_2);
	    	//HAL_TIM_Base_Stop(&htim8);
	    	f_lseek(fp,SEEK_SET);
	    	f_read(fp,(uint8_t *)audio_buf,WORD_SIZE,&bytes_read);
	    	f_read(fp,(uint8_t *)audio_buf + WORD_SIZE,WORD_SIZE,&bytes_read);
	    	signed16_unsigned12(audio_buf,0,WORD_SIZE);
	        need_new_data = FALSE;
	        word_count = 0;
	        play_buffer = 0;
	        //HAL_TIM_Base_Start(&htim8);
	        HAL_DAC_Start(&hdac,DAC_CHANNEL_2);
	        HAL_DAC_Start_DMA(&hdac,DAC_CHANNEL_2,(uint32_t*)audio_buf,WORD_SIZE,DAC_ALIGN_12B_R);

	      }

	}
	HAL_TIM_Base_Stop_IT(&htim8);

}

uint32_t SD_ReadAudio(uint32_t start,FIL *fp){
	uint8_t *_buf;
	uint32_t samples = 0;
	uint32_t total_bytes_read = 0;
	uint32_t skip = 0,progress = 0;
	DWORD size_bytes = fp->fsize;
	fil = fp;
	_buf = (uint8_t *)malloc(8192);
	bytes_read = 0;
	sdram_pointer = start * 2;
	if((size_bytes + sdram_pointer) > SDRAM_SIZE)
		size_bytes = SDRAM_SIZE;

	while(1){
		f_read(fp,(uint8_t *)_buf,8192,&bytes_read);
		BSP_SDRAM_WriteData16b(SDRAM_DEVICE_ADDR + sdram_pointer,(uint16_t*)_buf,bytes_read / 2);
		samples += (bytes_read / looper.SampleBytes);
		total_bytes_read += bytes_read;
		progress = (uint32_t)(((float)total_bytes_read / (float)size_bytes) * 256.00);
		TM_ILI9341_DrawFilledRectangle(10 + skip,120,10 + progress,130,ILI9341_COLOR_YELLOW);
		skip = progress;
		sdram_pointer += bytes_read;
		if(f_eof(fp))
			break;
	}
	TM_ILI9341_DrawFilledRectangle(10 + skip,120,10 + progress,130,ILI9341_COLOR_YELLOW);
	free(_buf);
	return samples;
}


void SD_WriteAudio(uint32_t start,uint32_t end,FIL *fp){
	uint8_t *_buf;
	uint32_t bytesRemaining = (end - start) * 2;
	_buf = (uint8_t *)malloc(8192);
	bytes_written = 0;
	sdram_pointer = start * 2;
	f_truncate(fp);
	while(bytesRemaining > 0){
		if(bytesRemaining >= 8192){
			BSP_SDRAM_ReadData16b(SDRAM_DEVICE_ADDR + sdram_pointer,(uint16_t *) _buf, 4096);
			f_write(fp,_buf,8192,&bytes_written);
		}
		else{
			BSP_SDRAM_ReadData16b(SDRAM_DEVICE_ADDR + sdram_pointer,(uint16_t *) _buf, bytesRemaining / 2);
			f_write(fp,_buf,bytesRemaining,&bytes_written);
		}
		bytesRemaining -= bytes_written;
		sdram_pointer += bytes_written;
	}


	free(_buf);

}

void checkSD(){
	if(HAL_GPIO_ReadPin(uSD_CD_GPIO_Port,uSD_CD_Pin) == GPIO_PIN_RESET){
		  if(fs_mounted == FALSE){
			  if (f_mount(&FatFs, "", 1) != FR_OK){
				  TM_HD44780_Puts(0,1,"SD card error   ");//
			  }
		  else{
				fs_mounted = TRUE;
				TM_HD44780_Puts(0,1,"SD card ready  ");
			  }
		  }
	  }
	  else{
		  TM_HD44780_Puts(0,1,"SD card removed");
		  if(fs_mounted == TRUE){
			  f_mount(0, "", 1);
			  fs_mounted = FALSE;
		  }
	  }
}

uint32_t SRAM_download_rhythm(void){
	BOOL header_received = FALSE;
	uint32_t bytes_total = 0;
	bytes_written = 0;
	UserWritePtr = 0;
	UserReadPtr = 0;
	TM_HD44780_Puts(0, 1, "Download ready ");
	SRAM_seekWrite(0, SRAM_SET);
	BSP_LED_On(LED_GREEN);
	while ((Keypad_Button = TM_KEYPAD_Read()) != TM_KEYPAD_Button_0) {
		if (usbRecv == TRUE) {
			if(header_received == FALSE){
				bytes_total = *((uint32_t *)&UserWorkBufferHS[0]);	// read first int: number of bytes
				header_received = TRUE;
				TM_HD44780_Puts(0, 1, "Download started");
			}

			usbRecv = FALSE;
			if(strncmp((char *)&UserWorkBufferHS[UserReadPtr],"STOP",4) == 0)
				break;
			writeSRAM(&UserWorkBufferHS[UserReadPtr], usbBytes);
			bytes_written += usbBytes;
			sprintf(lcdline,"%u/%u",(unsigned int)bytes_written,(unsigned int)bytes_total);
			TM_ILI9341_Puts(10, 80, lcdline, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
			if(bytes_written >= bytes_total)
				break;
			UserReadPtr += 64;
			if (UserReadPtr == 128)
				UserReadPtr = 0;

		}
	}

	BSP_LED_Off(LED_GREEN);
	utoa(bytes_written, lcdline, 10);
	if(bytes_written == bytes_total)
		TM_HD44780_Puts(0, 1, "Download OK     ");
	else
		TM_HD44780_Puts(0, 1, "Download error  ");

	if(bytes_total != 0 && bytes_written != bytes_total)
		return 0;
	else
		return bytes_written;
}

void readLoopFromSD(uint32_t n,char *filename){

	FIL fil;
	if(filename[0] == '\0')
		return;
	//Mounted OK, turn on RED LED
	BSP_LED_On(LED_RED);

	if (f_open(&fil, filename, FA_OPEN_ALWAYS | FA_READ) == FR_OK){
		pattern_audio_map[n + 1].sample_position = pattern_audio_map[n].sample_position + SD_ReadAudio(pattern_audio_map[n].sample_position,&fil);
		f_close(&fil);
		BSP_LED_Off(LED_GREEN);
		//Unmount drive, don't forget this!

	}

	BSP_LED_Off(LED_RED);

	return;

}

void readRhythmFromSD(char *filename){

	FIL fil;
	if(filename[0] == '\0')
		return;
	//Mounted OK, turn on RED LED
	BSP_LED_On(LED_RED);
	SRAM_seekWrite(0,SRAM_SET);
	if (f_open(&fil, filename, FA_OPEN_ALWAYS | FA_READ) == FR_OK){
		while(1){
			f_read(&fil,(uint8_t *)audio_buf,BYTE_SIZE,&bytes_read);
			writeSRAM((unsigned char *)audio_buf,bytes_read);
			if(f_eof(&fil))
				break;
		}
		f_close(&fil);
		BSP_LED_Off(LED_GREEN);
		//Unmount drive, don't forget this!

	}

	BSP_LED_Off(LED_RED);

	return;

}

void writeSRAMtoSD(uint32_t bts,char *filename){
	FIL fil;
	bytes_written = 0;
	if(filename[0] == '\0')
		return;

	if(f_open(&fil, filename, FA_OPEN_ALWAYS | FA_WRITE) == FR_OK){
		BSP_LED_On(LED_RED);
		BSP_LED_On(LED_GREEN);
		SRAM_seekRead(0,SRAM_SET);
		while(bts > 0){
			if(bts > BYTE_SIZE){
				readSRAM((unsigned char *)audio_buf,BYTE_SIZE);
				f_write(&fil,audio_buf,BYTE_SIZE,&bytes_written);
			}
			else{
				readSRAM((unsigned char *)audio_buf,bts);
				f_write(&fil,audio_buf,bts,&bytes_written);
			}

			bts -= bytes_written;
		}

		f_close(&fil);
		BSP_LED_Off(LED_GREEN);
		BSP_LED_Off(LED_RED);
	}
}

void saveAllLoopsToSD(char *filename){
	FIL fil;
		if(filename[0] == '\0')
			return;
		//Mounted OK, turn on RED LED
		BSP_LED_On(LED_RED);
		if (f_open(&fil, filename, FA_OPEN_ALWAYS | FA_WRITE) == FR_OK){
			SD_WriteAudio(pattern_audio_map[looper.StartPattern].sample_position,pattern_audio_map[looper.EndPattern + 1].sample_position,&fil);
			f_close(&fil);
			BSP_LED_Off(LED_GREEN);
			BSP_LED_Off(LED_RED);
		}

		return;

}

void saveLoopToSD(uint32_t n,char *filename){
	FIL fil;
	if(filename[0] == '\0')
		return;
	//Mounted OK, turn on RED LED
	BSP_LED_On(LED_RED);
	if (f_open(&fil, filename, FA_OPEN_ALWAYS | FA_WRITE) == FR_OK){
		SD_WriteAudio(pattern_audio_map[n].sample_position,pattern_audio_map[n + 1].sample_position,&fil);
		f_close(&fil);
		BSP_LED_Off(LED_GREEN);
		BSP_LED_Off(LED_RED);
	}

	return;
}

