#include "main.h"
#include "audio.h"
#include "stm32f429i_discovery_sdram.h"
#include "ads1256_test.h"
#include "string.h"
#include "limits.h"
#include "waveplayer.h"
#include "math.h"
#include "tm_stm32_hd44780.h"
#include "stdlib.h"
#include "math.h"
#include "midi.h"
#include "drums.h"

uint32_t sdram_pointer = 0;
int16_t sample16s;	// sample obtained from ADS1256

void record_samples(int16_t swrite,__IO CHANNEL *cha,__IO CHANNEL *chb){
	int16_t sread;
	if(StartLooper == FALSE ){
		return;
	}
	if(cha->Active == TRUE){
		BSP_SDRAM_WriteData16b(SDRAM_DEVICE_ADDR + sdram_pointer,(uint16_t *) &swrite, 1);
		BSP_SDRAM_ReadData16b(SDRAM_DEVICE_ADDR + sdram_pointer + 2,(uint16_t *)&sread,1);
		cha->SamplesWritten++;
		chb->CurrentSample = sread;
	}
	else{
		BSP_SDRAM_WriteData16b(SDRAM_DEVICE_ADDR + sdram_pointer  + 2,(uint16_t *) &swrite, 1);
		BSP_SDRAM_ReadData16b(SDRAM_DEVICE_ADDR + sdram_pointer,(uint16_t *)&sread,1);
		chb->SamplesWritten++;
		cha->CurrentSample = sread;
	}


	sdram_pointer += 4;
	if(cha->SamplesWritten == chb->SamplesWritten){
		StartLooper = FALSE;
	}
	if(sdram_pointer == SDRAM_SIZE){
		sdram_pointer = 0;
		cha->SamplesRead = 0;
		chb->SamplesRead = 0;
		resetDrums();
	}
}

void record_sample(int16_t swrite,__IO CHANNEL *cha){
	if(StartLooper == FALSE ){
		return;
	}
	BSP_SDRAM_WriteData16b(SDRAM_DEVICE_ADDR + sdram_pointer,(uint16_t *) &swrite, 1);
	cha->SamplesWritten++;
	sdram_pointer += 2;
	if(sdram_pointer == SDRAM_SIZE){
		sdram_pointer = 0;
		cha->SamplesRead = 0;
		resetDrums();
	}
}

void read_sample(int16_t swrite,__IO CHANNEL *cha){
	int16_t sread;
	if(StartLooper == FALSE ){
			return;
	}
	BSP_SDRAM_ReadData16b(SDRAM_DEVICE_ADDR + sdram_pointer,(uint16_t *) &sread, 1);
	cha->mix32tmp = sread  + swrite;
	cha->CurrentSample = sread;
	cha->SamplesRead++;

	if(cha->Clipping == TRUE)
		cha->mix32tmp = (int32_t)(cha->gain * (float)sread  + cha->gain * (float)swrite);

	if(cha->mix32tmp > cha->mix32Max)
		cha->mix32Max = cha->mix32tmp;

	if(cha->Overdub == TRUE)
		BSP_SDRAM_WriteData16b(SDRAM_DEVICE_ADDR + sdram_pointer,(uint16_t *) &cha->mix32tmp, 1);

	if(cha->SamplesRead == cha->SamplesWritten){
		if(cha->Overdub == TRUE && cha->mix32Max > 16383){
			cha->Clipping = TRUE;
			cha->gain = 16383.00 / cha->mix32Max;
		}
		else{
			cha->Clipping = FALSE;
			cha->gain = 1.0;
		}

		cha->mix32Max = 16383;
		sdram_pointer = 0;
		cha->SamplesRead = 0;
		resetDrums();
		return;
	}

	sdram_pointer += 2;

	if(sdram_pointer == SDRAM_SIZE)
		sdram_pointer = 0;
}


void play_sample(__IO CHANNEL *cha){
	Write_DAC8552(channel_A,(uint16_t)(cha->CurrentSample + 16383));
}

void play_samples(__IO CHANNEL *cha,__IO CHANNEL *chb){

	if((cha->Active == TRUE && chb->SamplesWritten > 0) || (chb->Active == TRUE && cha->SamplesWritten > 0))
		Write_DAC8552_Both((uint16_t)(cha->CurrentSample + 16383),(uint16_t)(chb->CurrentSample + 16383));
	else if(cha->Active == TRUE)
		Write_DAC8552(channel_A,(uint16_t)(cha->CurrentSample + 16383));
	else if(chb->Active == TRUE)
		Write_DAC8552(channel_B,(uint16_t)(chb->CurrentSample + 16383));

}

void read_samples(int16_t swrite,__IO CHANNEL *cha,__IO CHANNEL *chb){
	__IO CHANNEL *active;	// currently active channel
	__IO CHANNEL *other;	// other channel
	int16_t sread[2];

	if(StartLooper == FALSE ){
		return;
	}

	BSP_SDRAM_ReadData(SDRAM_DEVICE_ADDR + sdram_pointer,(uint32_t *) sread, 1);
	if(cha->Active == TRUE){
		active = cha;
		other = chb;
		active->mix32tmp = sread[0]  + swrite;
		active->CurrentSample = sread[0];
		chb->CurrentSample = sread[1];

	}
	else if(chb->Active == TRUE){
		active = chb;
		other = cha;
		active->mix32tmp = sread[1]  + swrite;
		active->CurrentSample = sread[1];
		cha->CurrentSample = sread[0];
	}
	else return;

	active->SamplesRead++;
	if(other->SamplesRead < other->SamplesWritten)
		other->SamplesRead++;

	if(active->Clipping == TRUE)
		active->mix32tmp = (int32_t)(active->gain * (float)sread[0]  + active->gain * (float)swrite);
	if(active->mix32tmp > active->mix32Max)
		active->mix32Max = active->mix32tmp;
	if(active->Overdub == TRUE){
		if(active->Number == ONE)
			BSP_SDRAM_WriteData16b(SDRAM_DEVICE_ADDR + sdram_pointer,(uint16_t *) &active->mix32tmp, 1);
		else if(active->Number == TWO)
			BSP_SDRAM_WriteData16b(SDRAM_DEVICE_ADDR + sdram_pointer + 2,(uint16_t *) &active->mix32tmp, 1);
	}
	if(active->SamplesRead == active->SamplesWritten){
		if(active->Overdub == TRUE && active->mix32Max > 16383){
			active->Clipping = TRUE;
			active->gain = 16383.00 / active->mix32Max;
		}
		else{
			active->Clipping = FALSE;
			active->gain = 1.0;
		}

		active->mix32Max = 16383;
		sdram_pointer = 0;
		active->SamplesRead = 0;
		resetDrums();
		return;
		}

		sdram_pointer += 4;

	if(sdram_pointer == SDRAM_SIZE)
		sdram_pointer = 0;

}

void resetChannels(__IO CHANNEL *ch1,__IO CHANNEL *ch2){
	ch1->Clipping = FALSE;
	ch2->Clipping = FALSE;
	ch1->Overdub = FALSE;
	ch2->Overdub = FALSE;
	ch1->mix32Max = 16383;
	ch2->mix32Max = 16383;
	ch1->gain = 1.0;
	ch2->gain = 1.0;
	ch1->SamplesRead = 0;
	ch2->SamplesRead = 0;
	ch1->SamplesWritten = 0;
	ch2->SamplesWritten = 0;
	ch1->CurrentSample = 0;
	ch2->CurrentSample = 0;
}

void showMinMaxSamples(int32_t max,int32_t min){
	char minstr[10],maxstr[10];
	itoa(max,maxstr,10);
	itoa(min,minstr,10);

	TM_HD44780_Clear();
	TM_HD44780_Puts(0,0,maxstr);
	TM_HD44780_Puts(0,1,minstr);
}
