#include "spi_ads1118.h"
#include "bsp_led.h"
#include "bsp_SysTick.h"

#define SPI_2_CS_1 GPIO_Pin_8 //PB8
#define SPI_2_CS_2 GPIO_Pin_7 //PB7
#define SPI_2_CS_3 GPIO_Pin_6 //PB6
#define SPI_2_CS_4 GPIO_Pin_5 //PB5
#define SPI_2_CS_5 GPIO_Pin_4 //PB4
#define SPI_2_CS_6 GPIO_Pin_3 //PB3
#define SPI_2_CS_7 GPIO_Pin_1 //PB2
#define SPI_2_CS_8 GPIO_Pin_0 //PB1

#define SPI_2_SCK  GPIO_Pin_13 //PB13
#define SPI_2_MISO GPIO_Pin_14 //PB14
#define SPI_2_MOSI GPIO_Pin_15 //PB15


static void ads1118_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOB, ENABLE); 
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);

/*++++++++++++++++++++++++++++++++++ GPIOB Init ++++++++++++++++++++++++++++++++*/
	GPIO_InitStructure.GPIO_Pin = SPI_2_CS_1|SPI_2_CS_2|SPI_2_CS_3|SPI_2_CS_4|SPI_2_CS_5
													|SPI_2_CS_6|SPI_2_CS_7|SPI_2_CS_8;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}


void Delay(u32 cnt)
{
	while(cnt)
	{
		cnt--;
		__NOP;
	}
}

void ADS1118_Init(void)
{
  SPI_InitTypeDef  SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  /*!< spi Periph clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
  /*!< Configure SCK pins:  */
  GPIO_InitStructure.GPIO_Pin = SPI_2_SCK;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /*!< Configure MISO pins:  */
  GPIO_InitStructure.GPIO_Pin = SPI_2_MISO;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  /*!< Configure MOSI pins:  */
  GPIO_InitStructure.GPIO_Pin = SPI_2_MOSI;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

	ads1118_gpio_init();  //cs gpio init
  /* Deselect the ADS1118: Chip Select high */
  ADS1118_CS1_HIGH();
  ADS1118_CS2_HIGH();
  ADS1118_CS3_HIGH();
  ADS1118_CS4_HIGH();
  ADS1118_CS5_HIGH();
  ADS1118_CS6_HIGH();
  ADS1118_CS7_HIGH();
  ADS1118_CS8_HIGH();

  /* SPI1 configuration */
  // Data on the DO and DIO pins are clocked out on the falling edge of CLK.
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI2, &SPI_InitStructure);
  /* Enable SPI1  */
  SPI_Cmd(SPI2, ENABLE);
}

u8 ADS1118_SendByte(u8 byte)
{
  /* Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);

  /* Send byte through the SPI1 peripheral */
  SPI_I2S_SendData(SPI2, byte);

  /* Wait to receive a byte */
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);

  /* Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(SPI2);
}

u16 ADS1118_Read_Data(u8 cs_num,u16 ctr_data)
{
	u8 ctr_msb,ctr_lsb;
	u8 data_h,data_l;
	u16 ADC_data;
	
	ctr_msb = (ctr_data>>8);
	ctr_lsb = (ctr_data & 0xff);
	ADS1118_CS_LOW(cs_num);
	Delay(200);

	data_h = ADS1118_SendByte(ctr_msb);
	data_l = ADS1118_SendByte(ctr_lsb);
	
//	ADS1118_SendByte(ctr_msb);  //if read 32bit, concel TBD
//	ADS1118_SendByte(ctr_lsb);
	ADS1118_CS_HIGH(cs_num);
	Delay(500);
	
	if(data_h>=0x7f)
	{
		ADC_data = 0;
	}
	else
	{
		ADC_data = (data_h<<8)|data_l;
	}
	return ADC_data;
}

/*
ADS1118 has 4channels, AIN0-AIN3
Only AIN0 in use, others are reserved.
the para channel:1-8
*/
u16 ADS1118_Get_value(u8 channel)
{
	u8 temp = 0;
	u32 ads1118_sample_data;
	for(temp=0;temp<40;temp++)
	{
		ads1118_sample_data += ADS1118_Read_Data(channel,ADS1118_CHANNAL_0);
		delay_us(50);
	}
	ads1118_sample_data/=40;
	return ads1118_sample_data;
}


void ADS1118_CS_LOW(u8 which)
{
	switch(which)
	{
		case 1:
		{
			ADS1118_CS1_LOW();
			DG408_output(1);
			break;
		}
		case 2:
		{
			ADS1118_CS2_LOW();
			DG408_output(2);
			break;
		}
		case 3:
		{
			ADS1118_CS3_LOW();
			DG408_output(3);
			break;
		}
		case 4:
		{
			ADS1118_CS4_LOW();
			DG408_output(4);
			break;
		}
		case 5:
		{
			ADS1118_CS5_LOW();
			DG408_output(5);
			break;
		}
		case 6:
		{
			ADS1118_CS6_LOW();
			DG408_output(6);
			break;
		}
		case 7:
		{
			ADS1118_CS7_LOW();
			DG408_output(7);
			break;
		}
		case 8:
		{
			ADS1118_CS8_LOW();
			DG408_output(8);
			break;
		}
	}
}

void ADS1118_CS_HIGH(u8 which)
{
	switch(which)
	{
		case 1:
		{
			ADS1118_CS1_HIGH();
			DG408_output(0);
			break;
		}
		case 2:
		{
			ADS1118_CS2_HIGH();
			DG408_output(0);
			break;
		}
		case 3:
		{
			ADS1118_CS3_HIGH();
			DG408_output(0);
			break;
		}
		case 4:
		{
			ADS1118_CS4_HIGH();
			DG408_output(0);
			break;
		}
		case 5:
		{
			ADS1118_CS5_HIGH();
			DG408_output(0);
			break;
		}
		case 6:
		{
			ADS1118_CS6_HIGH();
			DG408_output(0);
			break;
		}
		case 7:
		{
			ADS1118_CS7_HIGH();
			DG408_output(0);
			break;
		}
		case 8:
		{
			ADS1118_CS8_HIGH();
			DG408_output(0);
			break;
		}
		default: break;
	}
}


