#ifndef _BSP_I2C_GPIO_H
#define _BSP_I2C_GPIO_H

#include <inttypes.h>

#define I2C_WR	0		/* Ð´¿ØÖÆbit */
#define I2C_RD	1		/* ¶Á¿ØÖÆbit */

#define SDA_IN()  GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=4<<28
#define SDA_OUT() GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=7<<28


void i2c_Start(void);
void i2c_Stop(void);
void i2c_SendByte(uint8_t _ucByte);
uint8_t i2c_ReadByte(void);
uint8_t i2c_WaitAck(void);
void i2c_Ack(void);
void i2c_NAck(void);
uint8_t i2c_CheckDevice(uint8_t _Address);

uint8_t i2c_read_num_bytes(uint8_t *data, uint8_t len); //LM add
//void SDA_IN();
//void SDA_OUT();

#endif
