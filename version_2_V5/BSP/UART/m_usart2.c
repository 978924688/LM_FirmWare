/* Log print */

#include "m_usart2.h"
#include "sys.h"
#include "stdio.h"

void USART2_Send_Byte(unsigned char data)
{	  
  	USART2->DR = data;
  	while((USART2->SR&0X40)==0);  
}

void USART2_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure; 

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //TX
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; //RX
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

  USART_InitStructure.USART_Mode = USART_Mode_Tx;
  USART_Init(USART2, &USART_InitStructure);    
//  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);   
  USART_Cmd(USART2, ENABLE);
  USART_ClearITPendingBit(USART2, USART_IT_RXNE);  
  USART_ClearITPendingBit(USART2, USART_IT_TC); 
}


/// 重定向c库函数printf到USART1
int fputc(int ch, FILE *f)
{
		/* 发送一个字节数据到USART1 */
		USART_SendData(USART2, (uint8_t) ch);
		
		/* 等待发送完毕 */
		while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);		
	
		return (ch);
}

/// 重定向c库函数scanf到USART1
int fgetc(FILE *f)
{
		/* 等待串口1输入数据 */
		while (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET);

		return (int)USART_ReceiveData(USART2);
}


void USART2_Send_Data(unsigned char *send_buff,unsigned int length)
{
	for(u8 i=0;i<length;i++)
	{
		USART2->DR = send_buff[i];
		while((USART2->SR&0X40)==0); 
	}
}
void USART2_IRQHandler(void)
{
  u8 temp;
  if(USART_GetITStatus(USART2,USART_IT_RXNE) != RESET)
  {
   
  }
  if (USART_GetITStatus(USART2, USART_IT_TXE) != RESET) 
  {
    USART_ClearITPendingBit(USART2, USART_IT_TXE);           // Clear the USART transmit 
  }  
	USART_ClearITPendingBit(USART2, USART_IT_RXNE);
  USART_ClearITPendingBit(USART2, USART_IT_TXE);
}


