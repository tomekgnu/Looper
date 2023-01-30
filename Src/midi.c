#include "main.h"
#include "midi.h"
#include "usart.h"
#include "tm_stm32f4_keypad.h"


uint16_t beats_per_minute	= 60;			// 1s = 1 quarter
uint16_t millis_per_beat = 60000 / 60;
extern uint8_t UserRxBufferHS[];

void setupMidi(){
	//Reset the VS1053
	  VS1053_LOW();
	  HAL_Delay(100);
	  VS1053_HIGH();
	  HAL_Delay(100);
	  talkMIDI(0xB0, 0x07, 120); //0xB0 is channel message, set channel volume to near max (127)

}

void noteOn(byte channel, byte note, byte attack_velocity) {
  talkMIDI( (0x90 | channel), note, attack_velocity);
}

//Send a MIDI note-off message.  Like releasing a piano key
void noteOff(byte channel, byte note, byte release_velocity) {
  talkMIDI( (0x80 | channel), note, release_velocity);
}

//Plays a MIDI note. Doesn't check to see that cmd is greater than 127, or that data values are less than 127
void talkMIDI(byte cmd, byte data1, byte data2) {
	HAL_UART_Transmit(&huart1,&cmd,1,100);
	HAL_UART_Transmit(&huart1,&data1,1,100);

  //Some commands only have one data byte. All cmds less than 0xBn have 2 data bytes
  //(sort of: http://253.ccarh.org/handout/midiprotocol/)
  if( (cmd & 0xF0) <= 0xB0)
	  HAL_UART_Transmit(&huart1,&data2,1,100);

}

void playPercussion(byte onoff,byte instrument){
	if(onoff == NOTEON)
		noteOn(9, instrument, 60);
	else
		//Turn off the note with a given off/release velocity
		noteOff(9, instrument, 60);
}


void playBass(byte onoff,byte tone){
	talkMIDI(0xC0,34,0);
	if(onoff == NOTEON)
			noteOn(0,tone,60);
		else
			//Turn off the note with a given off/release velocity
			noteOff(0,tone,60);
}

void playUsbDrums(){
int i;
  for(i = 0; i < 4; i++){
	if(UserRxBufferHS[i] != 0)
	 playPercussion(NOTEON,UserRxBufferHS[i]);
  }
  if(UserRxBufferHS[4] != 0)
	playBass(NOTEON,UserRxBufferHS[4]);
}
