#include "main.h"

void UART5_Send_Byte(unsigned char data)
{	  
  	UART5->DR = data;
  	while((UART5->SR&0X40)==0);  
}

void UART5_Configuration(void)
{ 
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure; 
	
/****************************����ΪUART5����**************************/
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD,ENABLE);
	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;	         
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_Init(GPIOC, &GPIO_InitStructure);		   
	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;  
  GPIO_Init(GPIOD, &GPIO_InitStructure);
	
  /*
  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8λ����
  USART_InitStructure.USART_StopBits = USART_StopBits_1;//1λֹͣλ
  USART_InitStructure.USART_Parity = USART_Parity_Even;//żУ��
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //Ӳ��������ʧ��*/
  USART_InitStructure.USART_BaudRate = CommParmCfg.baudrate;
  USART_InitStructure.USART_WordLength = CommParmCfg.databit;//8λ����
  USART_InitStructure.USART_StopBits = CommParmCfg.stopbit;//1λֹͣλ
  USART_InitStructure.USART_Parity = CommParmCfg.paritybit;//żУ��
  USART_InitStructure.USART_HardwareFlowControl = CommParmCfg.flowctrl; //Ӳ��������ʧ��
    
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //���ͺͽ���ʹ��
  USART_Init(UART5, &USART_InitStructure); 	
  USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);

  USART_Cmd(UART5, ENABLE); 
  USART_ClearITPendingBit(UART5, USART_IT_TC);//����ж�TCλ
	UART5_Send_Byte(0x20);
}

void UART5_Send_Data(unsigned char *send_buff,unsigned int length)
{
  unsigned int i = 0;
	__disable_irq();
  for(i = 0;i < length;i ++)
  {			
    UART5->DR = send_buff[i];
    while((UART5->SR&0X40)==0);	
  }	
	__enable_irq();
}

void UART5_IRQHandler(void)  
{
  u8 temp;
  if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)//���յ�������
  {					 
    temp=UART5->DR;
    DAQ_DataRecv(CHANNEL_UART5,temp);
  	USART_ClearITPendingBit(UART5, USART_IT_RXNE);
  }
  if (USART_GetITStatus(UART5, USART_IT_TXE) != RESET) 
  {
    USART_ClearITPendingBit(UART5, USART_IT_TXE);           /* Clear the USART transmit interrupt                  */
  }
}
