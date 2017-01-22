#include "main.h"
#include "ads1256_test.h"
#include "stm32f429i_discovery_sdram.h"
#include "string.h"

uint16_t read_buffer_1[SAMPLE_ARRAY];
uint16_t read_buffer_2[SAMPLE_ARRAY];
uint16_t write_buffer_1[SAMPLE_ARRAY];
uint16_t write_buffer_2[SAMPLE_ARRAY];

uint16_t * rdptr;
uint16_t * wrptr;	// pointer for writing
uint32_t SamplesRead = 0;
uint32_t SamplesWritten = 0;
uint32_t DataReady = 0;
__IO uint32_t Recording = 2;
__IO uint32_t Dubbing = 0;
__IO uint32_t DubbingPressed = 0;
__IO uint32_t BufferCount = 0;
__IO uint32_t TapToneBufferCount = 1500;
__IO uint32_t BufferSwitch = 0;
__IO uint32_t TimeCounter = 0;
__IO uint8_t ToggleRecording = 0;
__IO uint8_t StartApp = 0;
__IO uint32_t CurrentSize;
__IO uint32_t CurrentBytes;
__IO uint8_t DmaTransferReady = 0;
uint16_t sample;
uint32_t read_pointer;
uint32_t write_pointer;
uint8_t first_filled = 0;

uint16_t mix(double a, double b) {
	return (uint16_t) ((a + b) * 0.5);
}

int32_t ads1256data;

static uint16_t getSample() {

	ADS1256_WaitDRDY();
	ads1256data = ADS1256_ReadData();
	return (uint16_t) (((ads1256data >> 8) + 32768));
}
static void resetVars(){
	BufferCount = 0;
	write_pointer = 0;
	read_pointer = SAMPLE_BYTES;
	if(Recording == 1){
		SamplesWritten = 0;
		SamplesRead = 0;
		wrptr = write_buffer_1;
	}
	else{
		SamplesRead = 0;
		rdptr = read_buffer_1;
	}
	DataReady = 0;
	BufferSwitch = 1;



}

inline void adc_play_dac(){
	uint16_t sample = getSample();
	Write_DAC8552(channel_A,sample);
	Write_DAC8552(channel_B,sample);
}


void play_memory(){
	BufferCount = 0;
	SamplesRead = 0;
	while(Recording == 0){
		dac_adc_memory();
		BufferCount++;
		if(BufferCount == 90000 || BufferCount == SamplesWritten){
			BufferCount = 0;
		}
	}

}

void audio_loop(){
	while(1){





	}

}

void record_memory(){
	BufferCount = 0;
	SamplesWritten = 0;
	while(Recording == 1){
		dac_adc_memory();
		BufferCount++;
		if(BufferCount == 90000){
			BufferCount = 0;
		}
	}
}



inline void dac_adc(){
	uint16_t newsample = getSample();
	uint16_t oldsample = rdptr[BufferCount];
	uint16_t mixed = mix(newsample,oldsample);

	if(Recording == 0){
		Write_DAC8552(channel_A,newsample);
		Write_DAC8552(channel_B,oldsample);
		if(Dubbing == 1)
			wrptr[BufferCount] = mixed;
		SamplesRead++;
	}
	else{
		Write_DAC8552(channel_A,newsample);
		wrptr[BufferCount] = newsample;
		SamplesWritten++;
	}

	BufferCount++;
	if(BufferCount == SAMPLE_ARRAY){
		DataReady = 1;
		BufferCount = 0;
	}
}

void write_sdram(){
	//uint32_t size = (BufferCount * 2 / 4) + (BufferCount % 2);
	if (BufferSwitch == 1) {
			BufferSwitch = 0;
			wrptr = write_buffer_2;
			BSP_SDRAM_WriteData_DMA(SDRAM_DEVICE_ADDR + write_pointer,(uint32_t *) write_buffer_1, MEM_BLOCK);
			if (first_filled == 0) {
				memcpy(read_buffer_1, write_buffer_1, BufferCount * 2);
				first_filled = 1;
			}
			bzero(write_buffer_1, SAMPLE_BYTES);
			write_pointer += (BufferCount * 2);
		}
	else {
			BufferSwitch = 1;
			wrptr = write_buffer_1;
			BSP_SDRAM_WriteData_DMA(SDRAM_DEVICE_ADDR + write_pointer,(uint32_t *) write_buffer_2, MEM_BLOCK);
			if (first_filled == 1) {
				memcpy(read_buffer_2, write_buffer_2, MEM_BLOCK);
				first_filled = 2;
			}
			bzero(write_buffer_2, MEM_BLOCK);
			write_pointer += (BufferCount * 2);
		}

}
void read_sdram(){
	uint32_t size = (BufferCount * 2 / 4) + (BufferCount % 2);
	if (BufferSwitch == 1) {
		BufferSwitch = 0;
		wrptr = write_buffer_2;
		rdptr = read_buffer_2;
		if (Dubbing == 1)
			BSP_SDRAM_WriteData_DMA(SDRAM_DEVICE_ADDR + read_pointer - SAMPLE_BYTES,(uint32_t *) write_buffer_1, size);
		read_pointer += (BufferCount * 2);
		BSP_SDRAM_ReadData_DMA(SDRAM_DEVICE_ADDR + read_pointer,(uint32_t *) read_buffer_1, size);
	}
	else {
			BufferSwitch = 1;
			wrptr = write_buffer_1;
			rdptr = read_buffer_1;
			if (Dubbing == 1)
				BSP_SDRAM_WriteData_DMA(SDRAM_DEVICE_ADDR + read_pointer - SAMPLE_BYTES,(uint32_t *) write_buffer_2, size);
			read_pointer += (BufferCount * 2);
			BSP_SDRAM_ReadData_DMA(SDRAM_DEVICE_ADDR + read_pointer,(uint32_t *) read_buffer_2, size);

		}


}

void eraseMemory(){
	uint32_t pointer = 0;
	bzero(write_buffer_1, SAMPLE_BYTES);
	while(pointer < SDRAM_SIZE){
		BSP_SDRAM_WriteData(SDRAM_DEVICE_ADDR + pointer,(uint32_t *) write_buffer_1, MEM_BLOCK);
		pointer += SAMPLE_BYTES;
	}
}
void recordLoop() {
	uint32_t write_pointer = 0;
	uint8_t first_filled = 0;	// first read buffer
	DataReady = 0;
	BufferSwitch = 1;
	BufferCount = 0;
	SamplesWritten = 0;
	DmaTransferReady = 0;
	memset(write_buffer_1, 0, SAMPLE_BYTES);
	memset(write_buffer_2, 0, SAMPLE_BYTES);
	wrptr = write_buffer_1;

	while (Recording == 1) {
		while (DataReady == 0)
			continue;
		CurrentSize = (BufferCount * 2 / 4) + (BufferCount % 2);
		CurrentBytes = CURRENT_BYTES;
		DataReady = 0;
		BufferCount = 0;
		if (BufferSwitch == 1) {
			BufferSwitch = 0;
			wrptr = write_buffer_2;
			DmaTransferReady = 0;
			BSP_SDRAM_WriteData_DMA(SDRAM_DEVICE_ADDR + write_pointer,(uint32_t *) write_buffer_1, CurrentSize);
			while(!DmaTransferReady)
				continue;

			if (first_filled == 0) {
				memcpy(read_buffer_1, write_buffer_1, SAMPLE_BYTES);
				first_filled = 1;
			}
			memset(write_buffer_1, 0, SAMPLE_BYTES);
		} else {
			BufferSwitch = 1;
			wrptr = write_buffer_1;
			DmaTransferReady = 0;
			BSP_SDRAM_WriteData_DMA(SDRAM_DEVICE_ADDR + write_pointer,(uint32_t *) write_buffer_2, CurrentSize);
			while(!DmaTransferReady)
							continue;

			if (first_filled == 1) {
				memcpy(read_buffer_2, write_buffer_2, SAMPLE_BYTES);
				first_filled = 2;
			}
			memset(write_buffer_2, 0, SAMPLE_BYTES);
		}

		write_pointer += CurrentBytes;

		if (write_pointer == SDRAM_SIZE) {
			write_pointer = 0;
			wrptr = write_buffer_1;
			SamplesWritten = 0;
			BufferCount = 0;
		}
	}

}
void playbackLoop() {
	uint32_t read_pointer = SAMPLE_BYTES;
	uint32_t time = 0;
	DataReady = 0;
	BufferSwitch = 1;
	BufferCount = 0;
	SamplesRead = 0;
	if(DubbingPressed == 1)
		Dubbing = 1;
	else
		Dubbing = 0;
	rdptr = read_buffer_1;
	wrptr = write_buffer_1;

	while(!DmaTransferReady)
		continue;
	DmaTransferReady = 0;

	while (Recording == 0) {
		while (DataReady == 0){
			if(SamplesRead == SamplesWritten){
				DataReady = 1;
				SamplesRead = 0;
				if(DubbingPressed == 1)
						Dubbing = 1;
					else
						Dubbing = 0;
				goto END_PLAYBACK;
			}

			continue;
		}
		DataReady = 0;
		BufferCount = 0;
		if (BufferSwitch == 1) {
			BufferSwitch = 0;
			rdptr = read_buffer_2;
			wrptr = write_buffer_2;
			if (Dubbing == 1){
				DmaTransferReady = 0;
				BSP_SDRAM_WriteData_DMA(SDRAM_DEVICE_ADDR + read_pointer - SAMPLE_BYTES,(uint32_t *) write_buffer_1, MEM_BLOCK);
				while(!DmaTransferReady)
					continue;

			}
			read_pointer += SAMPLE_BYTES;
			DmaTransferReady = 0;
			BSP_SDRAM_ReadData_DMA(SDRAM_DEVICE_ADDR + read_pointer,(uint32_t *) read_buffer_1, MEM_BLOCK);
			while(!DmaTransferReady)
				continue;

		} else {
			BufferSwitch = 1;
			rdptr = read_buffer_1;
			wrptr = write_buffer_1;
			if (Dubbing == 1){
				DmaTransferReady = 0;
				BSP_SDRAM_WriteData_DMA(SDRAM_DEVICE_ADDR + read_pointer - SAMPLE_BYTES,(uint32_t *) write_buffer_2, MEM_BLOCK);
				while(!DmaTransferReady)
					continue;

			}
			read_pointer += SAMPLE_BYTES;
			DmaTransferReady = 0;
			BSP_SDRAM_ReadData_DMA(SDRAM_DEVICE_ADDR + read_pointer,(uint32_t *) read_buffer_2, MEM_BLOCK);
			while(!DmaTransferReady)
				continue;

		}

		time++;

		if (read_pointer == SDRAM_SIZE) {
			END_PLAYBACK:
			read_pointer = SAMPLE_BYTES;
			time = 0;
			DataReady = 0;
			BufferSwitch = 1;
			BufferCount = 0;
			DmaTransferReady = 0;
			BSP_SDRAM_ReadData_DMA(SDRAM_DEVICE_ADDR,(uint32_t *) read_buffer_1, MEM_BLOCK);
			while(!DmaTransferReady)
				continue;

			DmaTransferReady = 0;
			BSP_SDRAM_ReadData_DMA(SDRAM_DEVICE_ADDR + read_pointer,(uint32_t *) read_buffer_2, MEM_BLOCK);
			while(!DmaTransferReady)
				continue;

			rdptr = read_buffer_1;
			wrptr = write_buffer_1;

		}

	}
}


void tapToggle() {
	ToggleRecording = !ToggleRecording;
	if (ToggleRecording == 1)
		TapToneBufferCount = 0;
}

void recordToggle() {
	if(StartApp == 1){
			ToggleRecording = 1;
		}
	if (Recording == 0x00 || Recording == 0x02) {
		Recording = 0x01;

	} else {
		Recording = 0x00;

	}
	if (Recording == 0x01) {
		Dubbing = 0;
		//SamplesRead = 0;
		//DubbingBufferCount = BufferCount;
		BSP_LED_On(LED4);
		BSP_LED_Off(LED3);
	} else {
		BSP_LED_Off(LED4);

	}

}
void dubToggle() {
	if (Recording == 1)
		return;
	DubbingPressed = !DubbingPressed;
	if (DubbingPressed == 1) {
		//DubbingPressed = 0;
		BSP_LED_On(LED3);
		BSP_LED_Off(LED4);
	} else {
		BSP_LED_Off(LED3);
	}
}

void play_record(){

	uint16_t newsample;
	uint16_t oldsample;
	if(StartApp == 0){
		return;
	}
	newsample = getSample();
	oldsample = rdptr[BufferCount];
	if(Recording == 0){
		Write_DAC8552(channel_A,oldsample);
		if(Dubbing == 1)
				wrptr[BufferCount] = mix(newsample,oldsample);
			SamplesRead++;
		}
		else{
			wrptr[BufferCount] = newsample;
			SamplesWritten++;
		}

		BufferCount++;

		if(ToggleRecording == 1){
			DataReady = 1;
			ToggleRecording = 0;
			return;
		}
		if(BufferCount == SAMPLE_ARRAY){
			DataReady = 1;
		}


}
