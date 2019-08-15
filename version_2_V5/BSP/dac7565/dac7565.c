//Author by LiuMing @2019-06-11

#include "dac7565.h"
#include "bsp_SysTick.h"

//#define SPI_1_CS_3	GPIO_Pin_2 //PA2
#define SPI_1_CS_3	GPIO_Pin_4 //PC4
#define SPI_1_CS_4	GPIO_Pin_1 //PA1
#define SPI_1_SCK	GPIO_Pin_5 //PA5
#define SPI_1_MOSI	GPIO_Pin_7 //PA7

#define LDAC_1		GPIO_Pin_8 //PD7
#define DAC_EN_1	GPIO_Pin_9 //PD9
#define RESTSEL_1	GPIO_Pin_10 //PD10
#define RST_1		GPIO_Pin_11 //PD11

#define LDAC_2		GPIO_Pin_12 //PD12
#define DAC_EN_2	GPIO_Pin_13 //PD13
#define RESTSEL_2	GPIO_Pin_14 //PD14
#define RST_2		GPIO_Pin_15 //PD15




static void dac7565_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD, ENABLE); 

/*++++++++++++++++++++++++++++++++++ GPIOA Init ++++++++++++++++++++++++++++++++*/
	GPIO_InitStructure.GPIO_Pin = SPI_1_CS_4|SPI_1_SCK|SPI_1_MOSI;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = SPI_1_CS_3; //PC4
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = LDAC_1|DAC_EN_1|RESTSEL_1|RST_1|
									LDAC_2|DAC_EN_2|RESTSEL_2|RST_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
	GPIO_Init(GPIOD, &GPIO_InitStructure);
}


/*
	hardware connect: 
MOSI->PA7
SCK ->PA5

*/

/*******************************************************************************
* Function Name  : SPI_FLASH_SectorErase
* Description    : Erases the specified FLASH sector.
* Input          : SectorAddr: address of the sector to erase.
* Output         : None
* Return         : None
*******************************************************************************/
#ifdef SPI_DAC7606

void spi_1_init()
{
   SPI_InitTypeDef  SPI_InitStructure;
	
	 /*!< SPI_FLASH_SPI Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	
  SPI_CS_HIGH;

  /* SPI1 configuration */
  // W25X16: data input on the DIO pin is sampled on the rising edge of the CLK. 
  // Data on the DO and DIO pins are clocked out on the falling edge of CLK.
  SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI1, &SPI_InitStructure);

  /* Enable SPI1  */
  SPI_Cmd(SPI1, ENABLE);
}
#endif

void dac_7565_init()
{
	dac7565_gpio_init();
	
	PDout(11) = 1;	//rst1
	PDout(8)  = 0;	//LDAC1
	PDout(9)  = 1;	//ENABLE SPI
	PDout(10) = 0;  //0:binary  
	DAC_7565_CS1_DISABLE;

	PDout(15) = 1; //µÚ2Æ¬ DACµÄRST2
	PDout(12) = 0; //LDAC2
	PDout(13) = 1; //DAC_EN_2 TBD
	PDout(14) = 0; //RSETSEL_2
	DAC_7565_CS2_DISABLE;
}

void DAC_7565_Init(void)
{
	dac_7565_init();
	
#ifdef SPI_DAC7606
	spi_1_init();
#endif
}

void LDAC_load()
{
	PDout(8) = 1;
	__NOP;
	__NOP;
	__NOP;
	PDout(8) = 0;
}



void spi_dac7565_send_byte(u8 byte)
{
	 /* Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);

  /* Send byte through the SPI1 peripheral */
  SPI_I2S_SendData(SPI1, byte);

  /* Wait to receive a byte */
//  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
}



static void DAC7565_Delay(void)
{
	volatile uint8_t i;
	for( i = 0;i < 35;i++)
	{
	
	}
}


#ifdef SPI_DAC7606
void dac7565_write(u8 channal,u16 _data)
{
	u16 Data = (_data <<4);
	
	DAC_ENABLE_LOW;
	SPI_CS_LOW;
	
	spi_dac7565_send_byte(channal);
	spi_dac7565_send_byte((Data>>8)&0xff);
	spi_dac7565_send_byte((Data)&0xff);
	
	SPI_CS_HIGH;
	DAC_ENABLE_HIGH;
//	LDAC_load();
	
}

#else
/*
channel: 4 channels -> A,B,C,D
  _data: 12 bits DAC, -> 0-4095 <-> 0-10V
*/
void dac7565_write_1(u8 channal,u16 _data)
{
	u8 i;
	u32 DATA;
	
	_data = _data << 4;
    DATA = (channal<<16) | _data;
//	DATA = 0x20fff0;
	
	DAC_ENABLE_LOW_1;
	DAC_7565_CS1_ENABLE;
	
	for(i = 0;i < 24;i++)
	{
		DAC_SCK_HIGH;
		DAC7565_Delay();
		PAout(7)= ((DATA>>(23-i)) & 0x1);
		
		DAC_SCK_LOW;
		
		DAC7565_Delay();
	}
	
	DAC_7565_CS1_DISABLE;
	DAC_ENABLE_HIGH_1;
}
#endif
void dac7565_write_2(u8 channal,u16 _data)
{
	u8 i;
	u32 DATA;
	
	_data = _data << 4;
    DATA = (channal<<16) | _data;
	
	DAC_ENABLE_LOW_2;
	DAC_7565_CS2_ENABLE;
	
	for(i = 0;i < 24;i++)
	{
		DAC_SCK_HIGH;
		DAC7565_Delay();
		PAout(7)= ((DATA>>(23-i)) & 0x1);
		
		DAC_SCK_LOW;
		
		DAC7565_Delay();
	}
	
	DAC_7565_CS2_DISABLE;
	DAC_ENABLE_HIGH_2;
}

void dac_7565_test(void)
{
//	dac7565_write_1(dac_7565_channal_1,909);   //0.55V
//	dac7565_write_1(dac_7565_channal_2,1818);  //1.1v
//	dac7565_write_1(dac_7565_channal_3,2727);  //1.66v
//	dac7565_write_1(dac_7565_channal_4,4095);  //2.5v
	
	dac7565_write_2(dac_7565_channal_1,909);   //0.5V
	dac7565_write_2(dac_7565_channal_2,1818);  //1.0v
	dac7565_write_2(dac_7565_channal_3,2727);  //1.5v
	dac7565_write_2(dac_7565_channal_4,4095);  //2.5v
}
