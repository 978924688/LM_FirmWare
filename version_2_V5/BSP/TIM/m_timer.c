//#include "main.h" 
//#include "m_gpio.h"
//u16 g_TimeCount;									 
#include "sys.h"
#include "stm32f10x_tim.h"
#include "bsp_led.h"
#include "m_usart1.h"


extern vu32 TimeDelayCount;


void mb_enable_char_timer(void)
{
	TIM_Cmd(TIM3, ENABLE);
	TIM_SetCounter(TIM3,0);
}
void mb_disable_char_timer(void)
{
	TIM_Cmd(TIM3, DISABLE);
	TX_485_DI;
//	TIM_SetCounter(TIM3,0);
}


//1s
void TIM1_Configuration(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_DeInit(TIM1);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
  TIM_TimeBaseStructure.TIM_Period=1999;
  TIM_TimeBaseStructure.TIM_Prescaler=35999;
  TIM_TimeBaseStructure.TIM_ClockDivision=0;
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
  TIM_ITConfig(TIM1,TIM_IT_Update|TIM_IT_Trigger,ENABLE);
  TIM_Cmd(TIM1, ENABLE);
}

//0.5s
void TIM2_Configuration(void) 
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure; 

	TIM_DeInit(TIM2);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	TIM_TimeBaseStructure.TIM_Period=99; //99
	TIM_TimeBaseStructure.TIM_Prescaler=35999;//36000000 / 36000 = 1000
	TIM_TimeBaseStructure.TIM_ClockDivision=0;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	TIM_ITConfig(TIM2,TIM_IT_Update|TIM_IT_Trigger,ENABLE);
	TIM_Cmd(TIM2, ENABLE);
    
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);  													
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;	  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
u8 tim_cnt = 0;
u8 tim_100ms_flag = 0;
void TIM2_IRQHandler(void)  //200ms isr
{
  if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
  {
     TIM_ClearITPendingBit(TIM2,TIM_FLAG_Update); //clear int lfag
	  if(tim_100ms_flag<13)
	  {
		tim_100ms_flag++; //loop while study use
	  }
	  
	  if(tim_cnt++ <10)
	  {
		LED1_TOGGLE;
	  }
	  else if(tim_cnt++ <22)
	  {
		  LED1_ON;
	  }
	  else
		  tim_cnt = 0;
  }
}

//Modbus use to timeout
/*
if bsp=115200  timeout=1760us
if bsp=9600    timeout=3200us
*/
void TIM3_Configuration(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
	
  TIM_DeInit(TIM3);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  TIM_TimeBaseStructure.TIM_Period=3200;//1760; //1760
  TIM_TimeBaseStructure.TIM_Prescaler=35; //1MHz
  TIM_TimeBaseStructure.TIM_ClockDivision=0;
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
  TIM_ITConfig(TIM3,TIM_IT_Update|TIM_IT_Trigger,ENABLE);
  TIM_Cmd(TIM3, DISABLE);

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);  													
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;	  
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/*
 * when bsp>19200   T3.5=1.750ms
 * T3.5 -> receive & send
*/
void TIM3_IRQHandler(void) //1760us isr
{ //115200bps -> timeout=1760us
  if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)
  {
      TIM_ClearITPendingBit(TIM3,TIM_FLAG_Update);
	  
		mb_disable_char_timer();
	  //rev_CRC check
		check_mb_rcv_buffer();
  }
}

//use to sample realtime & wait for discharge...
void TIM4_Configuration(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
	
  TIM_DeInit(TIM4);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
  TIM_TimeBaseStructure.TIM_Period=999;//1s enter isr
  TIM_TimeBaseStructure.TIM_Prescaler=35999;
  TIM_TimeBaseStructure.TIM_ClockDivision=0;
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
  TIM_ITConfig(TIM4,TIM_IT_Update|TIM_IT_Trigger,ENABLE);
  TIM_Cmd(TIM4, ENABLE);
	
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);  													
  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;	  
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;	
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

//u8 tim4_cnt = 0;
u8 tim4_1s_flag = 0; //use for update AD1118 sample
u8 tim4_10s_discharge_cnt = 0;
void TIM4_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM4, TIM_IT_Update) == SET)
  {
	TIM_ClearITPendingBit(TIM4,TIM_FLAG_Update);
   
	  tim4_1s_flag = 1;
	  tim4_10s_discharge_cnt++;
  }
}
//1ms


void TIM1_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
  {
    TIM_ClearITPendingBit(TIM1,TIM_FLAG_Update);
  }
}






