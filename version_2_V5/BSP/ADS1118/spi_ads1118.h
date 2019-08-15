#ifndef __SPI_FLASH_H
#define __SPI_FLASH_H

//#include "stm32f10x.h"
#include "sys.h"


//#define ADS1118_CHANNAL_0 0xc18b    //ADS1118ÅäÖÃ¼Ä´æÆ÷
#define ADS1118_CHANNAL_0 0xc38b    //ref=+4.096V
#define ADS1118_CHANNAL_1 0xd18b
#define ADS1118_CHANNAL_2 0xe18b
#define ADS1118_CHANNAL_3 0xf18b



#define SPI_FLASH_SPI                           SPI1
#define SPI_FLASH_SPI_CLK                       RCC_APB2Periph_SPI1
#define SPI_FLASH_SPI_SCK_PIN                   GPIO_Pin_5                  /* PA.05 */
#define SPI_FLASH_SPI_SCK_GPIO_PORT             GPIOA                       /* GPIOA */
#define SPI_FLASH_SPI_SCK_GPIO_CLK              RCC_APB2Periph_GPIOA
#define SPI_FLASH_SPI_MISO_PIN                  GPIO_Pin_6                  /* PA.06 */
#define SPI_FLASH_SPI_MISO_GPIO_PORT            GPIOA                       /* GPIOA */
#define SPI_FLASH_SPI_MISO_GPIO_CLK             RCC_APB2Periph_GPIOA
#define SPI_FLASH_SPI_MOSI_PIN                  GPIO_Pin_7                  /* PA.07 */
#define SPI_FLASH_SPI_MOSI_GPIO_PORT            GPIOA                       /* GPIOA */
#define SPI_FLASH_SPI_MOSI_GPIO_CLK             RCC_APB2Periph_GPIOA
#define SPI_FLASH_CS_PIN                        GPIO_Pin_4                  /* PA.04 */
#define SPI_FLASH_CS_GPIO_PORT                  GPIOA                       /* GPIOA */
#define SPI_FLASH_CS_GPIO_CLK                   RCC_APB2Periph_GPIOA


#define ADS1118_CS1_LOW()       PBout(8)=0
#define ADS1118_CS1_HIGH()      PBout(8)=1
#define ADS1118_CS2_LOW()       PBout(7)=0
#define ADS1118_CS2_HIGH()      PBout(7)=1
#define ADS1118_CS3_LOW()       PBout(6)=0
#define ADS1118_CS3_HIGH()      PBout(6)=1
#define ADS1118_CS4_LOW()       PBout(5)=0
#define ADS1118_CS4_HIGH()      PBout(5)=1
#define ADS1118_CS5_LOW()       PBout(4)=0
#define ADS1118_CS5_HIGH()      PBout(4)=1
#define ADS1118_CS6_LOW()       PBout(3)=0
#define ADS1118_CS6_HIGH()      PBout(3)=1
#define ADS1118_CS7_LOW()       PBout(1)=0
#define ADS1118_CS7_HIGH()      PBout(1)=1
#define ADS1118_CS8_LOW()       PBout(0)=0
#define ADS1118_CS8_HIGH()      PBout(0)=1

void ADS1118_Init(void);
u16 ADS1118_Read_Data(u8 cs_num,u16 ctr_data);

void ADS1118_CS_LOW(u8 which);
void ADS1118_CS_HIGH(u8 which);
u16 ADS1118_Get_value(u8 channel);


#endif

