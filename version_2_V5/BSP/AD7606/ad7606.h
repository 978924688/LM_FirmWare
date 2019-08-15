#ifndef __AD7606_H_
#define __AD7606_H_

/*----------------------include-----------------*/
#include "sys.h"

/*-------------------define--------------------*/


#define CONVST_LOW_2()	PCout(9) = 0;  //SECOND Chip
#define CONVST_HIGH_2()	PCout(9) = 1;

#define CONVST_LOW_1()	PCout(10) = 0;  //First Chip
#define CONVST_HIGH_1()	PCout(10) = 1;

#define AD_RESET_LOW()	PDout(3) = 0; //HAL_GPIO_WritePin(ADC_SRESET_GPIO_Port,ADC_SRESET_Pin,GPIO_PIN_RESET)
#define AD_RESET_HIGH()	PDout(3) = 1; //HAL_GPIO_WritePin(ADC_SRESET_GPIO_Port,ADC_SRESET_Pin,GPIO_PIN_SET)

#define AD_CS_LOW_2()	PCout(5) = 0;  //AD7606 of U27  second chip
#define AD_CS_HIGH_2()	PCout(5) = 1;

#define AD_CS_LOW_1()	PCout(6) = 0; //AD7606 of U18   First chip
#define AD_CS_HIGH_1()	PCout(6) = 1;

#define AD_SCK_LOW()	PCout(7) = 0; //HAL_GPIO_WritePin(ADC_SCK_GPIO_Port,ADC_SCK_Pin,GPIO_PIN_RESET)
#define AD_SCK_HIGH()	PCout(7) = 1; //HAL_GPIO_WritePin(ADC_SCK_GPIO_Port,ADC_SCK_Pin,GPIO_PIN_SET)

/*-----------------------------------function-------------------------------*/

void AD7606_Start_Conversion_1(void);
void AD7606_Start_Conversion_2(void);
void AD7606_Reset(void);
void AD7606_Init(void);
void AD7606_GetValue_2(uint16_t * pbuf,uint8_t len);
void AD7606_GetValue_1(uint16_t *pbuf,uint8_t len);

void AD7606_Set_Reference(uint8_t internal_external);
	
#endif

