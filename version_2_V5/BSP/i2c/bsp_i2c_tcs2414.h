//#ifndef __I2C_EE_H
//#define	__I2C_EE_H

#include "stm32f10x.h"


/* 传感器基本地址 */
#define COLOR_SENSOR_ADDR			0x39
#define REG_CONTROL					0x80	/* 0x00 */
#define REG_TIMING					0x81	/* 0x01 */
#define REG_INTRRUPT				0x82	/* 0x02 */
#define REG_INT_SOURCE				0x83	/* 0x03 */
#define REG_ID						0x84	/* 0x04 */
#define REG_GAIN					0x87	/* 0x07 */
#define REG_LOW_THRESH_LOW_BYTE		0x88	/* 0x08 */
#define REG_LOW_THRESH_HIGH_BYTE	0x89	/* 0x09 */
#define REG_HIGH_THRESH_LOW_BYTE	0x8A	/* 0x0A */
#define REG_HIGH_THRESH_HIGH_BYTE	0x8B	/* 0x0B */

#define REG_GREEN_LOW				0xD0	/* 0x10 */
#define REG_GREEN_HIGH				0xD1	/* 0x11 */
#define REG_RED_LOW					0xD2	/* 0x12 */
#define REG_RED_HIGH				0xD3	/* 0x13 */
#define REG_BLUE_LOW				0xD4	/* 0x14 */
#define REG_BLUE_HIGH				0xD5	/* 0x15 */
#define REG_CLEAR_LOW				0xD6	/* 0x16 */
#define REG_CLEAR_HIGH				0xD7	/* 0x17 */

#define CLR_INT						0xE0
#define CTL_DAT_INIITIATE			0x03

/* Timing Register */
#define SYNC_EDGE					0x40
#define INTEG_MODE_FREE				0x00
#define INTEG_MODE_MANUAL			0x10
#define INTEG_MODE_SYN_SINGLE		0x20
#define INTEG_MODE_SYN_MULTI		0x30

#define INTEG_PARAM_PULSE_COUNT1	0x00
#define INTEG_PARAM_PULSE_COUNT2	0x01
#define INTEG_PARAM_PULSE_COUNT4	0x02
#define INTEG_PARAM_PULSE_COUNT8	0x03

/* Interrupt Control Register */
#define INTR_STOP			40
#define INTR_DISABLE		0x00
#define INTR_LEVEL			0x10			/* 兼容SMB-Alert */
#define INTR_PERSIST_EVERY	0x00			/* 每个ADC都会产生中断 */
#define INTR_PERSIST_SINGLE 0x01

/* Interrupt Souce Register */
#define INT_SOURCE_GREEN	0x00
#define INT_SOURCE_RED		0x01
#define INT_SOURCE_BLUE		0x10
#define INT_SOURCE_CLEAR	0x03

/* Gain Register */
#define GAIN_1			0x00
#define GAIN_4			0x10
#define GAIN_16			0x20
#define GAIN_64			0x30
#define PRESCALER_1		0x00
#define PRESCALER_2		0x01
#define PRESCALER_4		0x02
#define PRESCALER_8		0x03
#define PRESCALER_16	0x04
#define PRESCALER_32	0x05
#define PRESCALER_64	0x06
typedef struct
{
	uint16_t	Red;
	uint16_t	Green;
	uint16_t	Blue;
	uint16_t	Clear;
} _RGB;

uint8_t TCS3414_CheckOk(void);
void TCS3414_Init(void);
void readRGB_Block(void);
uint8_t readID(void);

uint8_t TCS_Read_Byte(uint8_t reg);

//#endif
