/**
  ******************************************************************************
  * @file    bsp_led.c
  * @author  LiuMing
  * @version V1.0
  * @date    2019-06-08
  * @brief   led app port
  */
  
#include "bsp_led.h"   
#include "sys.h"


#define KEY_0_PIN		  GPIO_Pin_0

 /**
  * @brief  LED Control
  * @param  None
  * @retval None
  */
void All_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOD, ENABLE); 

/*++++++++++++++++++++++++++++++++++ GPIOA Init ++++++++++++++++++++++++++++++++*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_15; //DG408
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1; //DG408
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
/*++++++++++++++++++++++++++++++++++ GPIOB Init +++++++++++++++++++++++++++++++++*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5
								|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8
								|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11
								|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
	GPIO_Init(GPIOE, &GPIO_InitStructure);

/*++++++++++++++++++++++++++++++++++ GPIOC Init +++++++++++++++++++++++++++++++++*/
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //ADC_CS_3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_InitStructure.GPIO_Pin = KEY_0_PIN; //KEY0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	
	GPIO_Init(GPIOC, &GPIO_InitStructure);

		PAout(0) = 0;
}
/*********************************************END OF FILE**********************/

void relay_on()
{
	PEout(8) = 1;
	PEout(9) = 1;
	PEout(10) = 1;
	PEout(11) = 1;
	PEout(12) = 1;
	PEout(13) = 1;
	PEout(14) = 1;
	PEout(15) = 1;
}

void relay_off()
{
	PEout(8) = 0;
	PEout(9) = 0;
	PEout(10) = 0;
	PEout(11) = 0;
	PEout(12) = 0;
	PEout(13) = 0;
	PEout(14) = 0;
	PEout(15) = 0;
}

void channel_on()
{
//	PEout(0) = 1;
//	PEout(1) = 1;
//	PEout(2) = 1;
//	PEout(3) = 1;
//	PEout(4) = 1;
//	PEout(5) = 1;
	PEout(6) = 1;
//	PEout(7) = 1;
}

void channel_off()
{
//	PEout(0) = 0;
//	PEout(1) = 0;
//	PEout(2) = 0;
//	PEout(3) = 0;
//	PEout(4) = 0;
//	PEout(5) = 0;
	PEout(6) = 0;
//	PEout(7) = 0;
}


void DG408_output(u8 channel)
{
	switch(channel)  //connect "MCU_AD_SDO"
	{
		case 1:
		{
			DG408_EN_H;
			DG408_A0_L;
			DG408_A1_L;
			DG408_A2_L;
			break;
		}
		
		case 2:
		{
			DG408_EN_H;
			DG408_A0_H;
			DG408_A1_L;
			DG408_A2_L;
			break;
		}
		case 3:
		{
			DG408_EN_H;
			DG408_A0_L;
			DG408_A1_H;
			DG408_A2_L;
			break;
		}
		case 4:
		{
			DG408_EN_H;
			DG408_A0_H;
			DG408_A1_H;
			DG408_A2_L;
			break;
		}
		case 5:
		{
			DG408_EN_H;
			DG408_A0_L;
			DG408_A1_L;
			DG408_A2_H;
			break;
		}
		case 6:
		{
			DG408_EN_H;
			DG408_A0_H;
			DG408_A1_L;
			DG408_A2_H;
			break;
		}
		case 7:
		{
			DG408_EN_H;
			DG408_A0_L;
			DG408_A1_H;
			DG408_A2_H;
			break;
		}
		case 8:
		{
			DG408_EN_H;
			DG408_A0_H;
			DG408_A1_H;
			DG408_A2_H;
			break;
		}
		default:  //not 1-8?  disable DG408
		{
			DG408_EN_L;
			DG408_A0_L;
			DG408_A1_L;
			DG408_A2_L;
			break;
		}
	}
}
