/***********************************************************************
�ļ����ƣ�USART1.h
��    �ܣ�
��дʱ�䣺2012.11.22
�� д �ˣ�
ע    �⣺
***********************************************************************/
#include "sys.h"
#include <stdio.h>
#ifndef  _USART3_H_
#define  _USART3_H_

#define USART3_485_RX_ENABLE 	GPIO_ResetBits(GPIOB , GPIO_Pin_9)	//����ʹ��
#define USART3_485_TX_ENABLE 	GPIO_SetBits(GPIOB , GPIO_Pin_9)  	//����ʹ��

#define USART3_BUFF_SIZE		(100)
#define USART3_FRAME_START1		(0x55)
#define USART3_FRAME_START2		(0xAA)
#define USART3_FRAME_END1		(0xCC)
#define USART3_FRAME_END2		(0xDD)

void USART3_Configuration(void);
void USART3_Send_Data(unsigned char *send_buff,unsigned int length);

int fputc(int ch, FILE *f);
int fgetc(FILE *f);

#endif


