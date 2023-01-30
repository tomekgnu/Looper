#ifndef __MIDI_H__
#define  __MIDI_H__

#define MIDI_QUEUE 	1000	// size of event array
#include "main.h"
#include "stm32f429xx.h"
#include "tm_stm32f4_keypad.h"
void playBass(byte onoff,byte tone);



extern __IO DrumFunction DrumState;
extern __IO uint16_t midiMetronomeClock;

extern __IO uint32_t midiMetronomePointer;
extern __IO uint8_t midiRecording;
extern __IO uint8_t midiPlayback;
extern __IO uint8_t midiMetronome;

void setupMidi();
void noteOn(byte channel, byte note, byte attack_velocity);
void noteOff(byte channel, byte note, byte release_velocity);
void talkMIDI(byte cmd, byte data1, byte data2);
void playPercussion(byte onoff,byte instrument);
void playUsbDrums();

#define	VS1053_LOW()	HAL_GPIO_WritePin(VS1053_RESET_GPIO_Port,VS1053_RESET_Pin,GPIO_PIN_RESET);
#define	VS1053_HIGH()	HAL_GPIO_WritePin(VS1053_RESET_GPIO_Port,VS1053_RESET_Pin,GPIO_PIN_SET);

extern uint16_t beats_per_minute;			// beats per minute
extern uint16_t millis_per_beat;

#define NOTEON				1
#define NOTEOFF				0

#define No_Event			255
#define	High_Q				27
#define	Slap				28
#define	Scratch_Push		29
#define	Scratch_Pull		30
#define	Sticks				31
#define	Square_Click		32
#define	Metronome_Click		33
#define	Metronome_Bell		34
#define	Acoustic_Bass_Drum	35
#define	Bass_Drum_1			36
#define	Side_Stick			37
#define	Acoustic_Snare		38
#define	Hand_Clap			39
#define	Electric_Snare		40
#define	Low_Floor_Tom		41
#define	Closed_Hi_Hat		42
#define	High_Floor_Tom		43
#define	Pedal_Hi_Hat		44
#define	Low_Tom				45
#define	Open_Hi_Hat			46
#define	Low_Mid_Tom			47
#define	Hi_Mid_Tom			48
#define	Crash_Cymbal_1		49
#define	High_Tom			50
#define	Ride_Cymbal_1		51
#define	Chinese_Cymbal		52
#define	Ride_Bell			53
#define	Tambourine			54
#define	Splash_Cymbal		55
#define	Cowbell				56
#define	Crash_Cymbal_2		57
#define	Vibraslap			58
#define	Ride_Cymbal_2		59
#define	Hi_Bongo			60
#define	Low_Bongo			61
#define	Mute_Hi_Conga		62
#define	Open_Hi_Conga		63
#define	Low_Conga			64
#define	High_Timbale		65
#define	Low_Timbale			66
#define	High_Agogo			67
#define	Low_Agogo			68
#define	Cabasa				69
#define	Maracas				70
#define	Short_Whistle		71
#define	Long_Whistle		72
#define	Short_Guiro			73
#define	Long_Guiro			74
#define	Claves				75
#define	Hi_Wood_Block		76
#define	Low_Wood_Block		77
#define	Mute_Cuica			78
#define	Open_Cuica			79
#define	Mute_Triangle		80
#define	Open_Triangle		81
#define	Shaker				82
#define	Jingle_bell			83
#define	Bell_tree			84
#define	Castanets			85
#define	Mute_Surdo			86
#define	Open_Surdo			87



#endif
