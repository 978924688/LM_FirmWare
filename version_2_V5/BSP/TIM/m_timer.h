#ifndef _TIME_H
#define _TIME_H	  
#include "sys.h"

extern u8 tim_100ms_flag;

void TIM1_Configuration(void);
void TIM2_Configuration(void);
void TIM3_Configuration(void);
void TIM4_Configuration(void);
void TIM5_Configuration(void);
void TIM6_Configuration(void);
void TIM7_Configuration(void);
void Delay1ms(uint32_t nTime);

void mb_enable_char_timer(void); //Modbus use

void mb_disable_char_timer(void);
void mb_enable_char_timer(void);

#endif
