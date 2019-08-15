
#include "ad7606.h"
#include "bsp_SysTick.h"

#define ADC_RESET_Pin GPIO_Pin_3  //PD3
#define ADC_DOUTB_Pin GPIO_Pin_12 //PB12
#define ADC_DOUTA_Pin GPIO_Pin_11 //PC11



#define ADC_BUSY1_Pin GPIO_Pin_7 //PD7
#define ADC_BUSY2_Pin GPIO_Pin_6 //PD6
//#define ADC_BUSY_GPIO_Port GPIOB
#define ADC_CS1_Pin GPIO_Pin_6	 //PC6
#define ADC_CS2_Pin GPIO_Pin_5	 //PC5
//#define ADC_CS_GPIO_Port GPIOC
#define ADC_SCK_Pin GPIO_Pin_7	 //PC7
//#define ADC_SCK_GPIO_Port GPIOC
#define ADC_CONVST1_Pin GPIO_Pin_10 //PC10
#define ADC_CONVST2_Pin GPIO_Pin_9	//PC9
//#define ADC_CONVST_GPIO_Port GPIOC
#define FRSTDATA_1_Pin	GPIO_Pin_5	//PD5
#define FRSTDATA_2_Pin	GPIO_Pin_4  //PD4




static void ad7606_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD, ENABLE); 

/*++++++++++++++++++++++++++++++++++ GPIOA Init ++++++++++++++++++++++++++++++++*/
	GPIO_InitStructure.GPIO_Pin = ADC_DOUTB_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = ADC_DOUTA_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = ADC_CONVST2_Pin|ADC_CS2_Pin|ADC_CS1_Pin|ADC_SCK_Pin|ADC_CONVST1_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = ADC_RESET_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = FRSTDATA_2_Pin|FRSTDATA_1_Pin|ADC_BUSY1_Pin|ADC_BUSY2_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
}
/*
****************************************************************************
* *:
* *:
****************************************************************************
*/
void AD7606_Delay(void)
{
	volatile uint8_t i;
	for( i = 0;i < 25;i++)
	{
	}
}

void Delay_1_nop(void)
{
	uint8_t i;
	for( i = 0;i < 2;i++)
	{
	}
}
/*
****************************************************************************
* *:
* *:
****************************************************************************
*/
void AD7606_Start_Conversion_2(void)
{
	CONVST_HIGH_2();
	CONVST_LOW_2();
	CONVST_LOW_2();
	CONVST_LOW_2();
	CONVST_HIGH_2();
}


/*
****************************************************************************
* *:
* *:
****************************************************************************
*/
void AD7606_Start_Conversion_1(void)
{
	CONVST_HIGH_1();
	CONVST_LOW_1();
	CONVST_LOW_1();
	CONVST_LOW_1();
	CONVST_HIGH_1();
}
/*
****************************************************************************
* *:
* *:
****************************************************************************
*/
void AD7606_Reset(void)
{
	AD_RESET_LOW();
	AD_RESET_HIGH();
	AD_RESET_HIGH();
	AD_RESET_HIGH();
	AD_RESET_HIGH();
	AD_RESET_LOW();
}


void AD7606_COM_Choice(uint8_t serial_parallel)
{
	switch(serial_parallel)
	{
		case 1:
		{
			PCout(6) = 1;
			PCout(5) = 1;
//			PCout(4) = 1;
			break;
		}
		case 2:
		{
			PCout(6) = 1;
			PCout(5) = 1;
			PCout(4) = 1;
			break;
		}
		case 3:
		{
			PCout(6) = 1;
			PCout(5) = 1;
			PCout(4) = 1;
			break;
		}
		default:break;
	}
}
/*
****************************************************************************
* *:
* *:
****************************************************************************
*/
uint8_t AD7606_WaitBusy(void)
{
//	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_15) == 1)
//	if(PBin(15) == 1) //First chip
	if(PDin(7) == 1)  //Second chip
		return 1;
	else
		return 0;
}

/*
****************************************************************************
* *:
* *:
****************************************************************************
*/
int16_t Read_Words_1(void)
{
	uint8_t i;
	int16_t readA = 0;
	
	for(i = 0;i < 16;i ++)
	{
		AD_SCK_LOW();
		AD7606_Delay();
		
		readA <<= 1;

//		if(PBin(14))
		if(PCin(11))
			readA |= 0x01;
		
		AD_SCK_HIGH();
		AD7606_Delay();
	}
	return readA;
}

int16_t Read_Words_2(void)
{
	uint8_t i;
	int16_t readA = 0;
	
	for(i = 0;i < 16;i ++)
	{
		AD_SCK_LOW();
		AD7606_Delay();
		
		readA <<= 1;

//		if(PBin(14))
		if(PBin(12))
			readA |= 0x01;
		
		AD_SCK_HIGH();
		AD7606_Delay();
	}
	return readA;
}
/*
****************************************************************************
* *:
* *:
****************************************************************************
*/
void AD7606_Init(void)
{
	ad7606_gpio_init();
	AD7606_COM_Choice(1);  //*选择哪一个AD7606片子
	AD7606_Reset();
}
/*
****************************************************************************
* *:
* *:
****************************************************************************
*/
void AD7606_GetValue_2(uint16_t *pbuf,uint8_t len)
{
	uint8_t channel_num = len;
	uint8_t i = 0;
	int adc_value = 0;
		
	AD7606_Start_Conversion_2();
	
	if(channel_num > 8)
		channel_num = 8;
	
	while(PDin(6))
	{
	}
	
	AD_CS_LOW_2();
		
	for(i = 0;i < channel_num;i++)
	{
		adc_value = (int)Read_Words_2();  //Only use in chip 1
		if(adc_value & 0x8000) adc_value = 0;
		pbuf[i] = adc_value * 10000 / 32767; //(adc_value * 5000) / 32767; 
//		pbuf[i] = adc_value * 5000 / 32767;
	}
	AD_CS_HIGH_2();
}

/*
****************************************************************************
* *:
* *:
****************************************************************************
*/
void AD7606_GetValue_1(uint16_t *pbuf,uint8_t len)
{
	uint8_t channel_num = len;
	uint8_t i = 0;
	int adc_value = 0;
		
	AD7606_Start_Conversion_1();
	
	if(channel_num > 8)
		channel_num = 8;
	
	while(PDin(7))
	{}
	
	AD_CS_LOW_1();
	Delay_1_nop();

	for(i = 0;i < channel_num;i++)
	{
		adc_value = (int)Read_Words_1();
		if(adc_value & 0x8000) adc_value = 0;
//		pbuf[i] = adc_value * 10000 / 32767; //(adc_value * 5000) / 32767; 
		pbuf[i] = adc_value * 5000 / 32767;
	}
	AD_CS_HIGH_1();
}

//void AD7606_Get_1()
//{
//	u16 vol_data[10][8];
//	u16 vol_data_2[8];
//	u8 i;
//	for(i=0;i<10;i++)
//	{
//		AD7606_GetValue_1(vol_data_2,8);
//		vol_data[i][]
//	}
//	
//}


