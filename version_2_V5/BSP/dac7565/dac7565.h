#ifndef __DAC7565_H_
#define __DAC7565_H_
#include "sys.h"

/*
SPI_CS_4  PA11
SPI_SCK		PA5
SPI_MOSI	PA7
RST_1		PD11
DAC_EN_1	PD9


*/

void DAC_7565_Init(void);
void dac7565_write_1(u8 channal,u16 _data);
void dac7565_write_2(u8 channal,u16 _data);

void dac_7565_test(void); 

#define dac_7565_channal_1 0x20
#define dac_7565_channal_2 0x22
#define dac_7565_channal_3 0x24
#define dac_7565_channal_4 0x26


#define DAC_7565_CS1_ENABLE		PCout(4)=0
#define DAC_7565_CS1_DISABLE	PCout(4)=1

#define DAC_7565_CS2_ENABLE		PAout(1)=0
#define DAC_7565_CS2_DISABLE	PAout(1)=1

//#define SPI_CS_HIGH_1		PAout(11)=1
//#define SPI_CS_LOW_1		PAout(11)=0

//#define SPI_CS_HIGH_2		PAout(12)=1
//#define SPI_CS_LOW_2		PAout(12)=0

#define DAC_SCK_LOW		PAout(5)=0
#define DAC_SCK_HIGH	PAout(5)=1

#define DAC_ENABLE_LOW_1	PDout(9)=0
#define DAC_ENABLE_HIGH_1	PDout(9)=1

#define DAC_ENABLE_LOW_2	PDout(13)=0
#define DAC_ENABLE_HIGH_2	PDout(13)=1


void dac_7565_init(void);

#endif
