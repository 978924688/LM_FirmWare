//Author by LiuMing @2019-06-11
#include "ad5551.h"




static void ad5551_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC, ENABLE); 

/*++++++++++++++++++++++++++++++++++ GPIOA Init ++++++++++++++++++++++++++++++++*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13; //ADC_CS_2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void AD5551_Init(void)
{
	ad5551_gpio_init();
	AD5551_CS_HIGH;
}

static void AD5551_Delay(void)
{
	volatile uint8_t i;
	for( i = 0;i < 20;i++)
	{
	
	}
}

/*
 Note:  14 bits DAC
_data:  0-16384 <--> 0-5000mV
3.2768 <-> 1 mV

与外部大电源对应关系：0-10V <-> 0-360V
0-16384 <-> 0-360V
步进：one step: 45
*/
void ad5551_write(u16 _data)
{
	u8 i;
	u32 DATA;

    DATA = _data;
	AD5551_CS_LOW;
	
	for(i = 0;i < 16;i++)
	{
		AD5551_SCK_LOW;
		AD5551_Delay();
		PAout(7)= ((DATA>>(15-i)) & 0x1);
		
		AD5551_SCK_HIGH;
		
		AD5551_Delay();
	}
	AD5551_CS_HIGH;
}



