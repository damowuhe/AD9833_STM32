#ifndef __AD9833_H__
#define __AD9833_H__

#include "gpio.h"
#define TRI_WAVE 	0  		//输出三角波
#define SIN_WAVE 	1		//输出正弦波
#define SQU_WAVE 	2		//输出方波

void write_ad9833_d16(unsigned int data);
void ad9833_init(void);
void AD9833_WaveSeting(double Freq,unsigned int Freq_SFR,unsigned int WaveMode,unsigned int Phase );
void wave_out(void);
void triangle_wave_out(void);
void square_wave_out(void);
#endif
