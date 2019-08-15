#include "m_usart3.h"


void USART3_Send_Byte(unsigned char data)
{
  	USART3->DR = data;
  	while((USART3->SR&0X40)==0);  
}

void USART3_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure; 

/**************************** USART3 **************************/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;       
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOB, &GPIO_InitStructure);		   
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;	        
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  	
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART3, &USART_InitStructure); 
//	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	
	USART_Cmd(USART3, ENABLE); 
	USART_ClearITPendingBit(USART3, USART_IT_TC);
	
	//RS485_EN
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;//RS485�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	USART3_485_TX_ENABLE;
}


///// �ض���c�⺯��printf��USART1
//int fputc(int ch, FILE *f)
//{
//		/* ����һ���ֽ����ݵ�USART1 */
//		USART_SendData(USART3, (uint8_t) ch);
//		
//		/* �ȴ�������� */
//		while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);		
//	
//		return (ch);
//}

///// �ض���c�⺯��scanf��USART1
//int fgetc(FILE *f)
//{
//		/* �ȴ�����1�������� */
//		while (USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == RESET);

//		return (int)USART_ReceiveData(USART3);
//}

static void TimeDelay(uint32_t nCount)
{ 
  while(nCount > 0)
  { 
    nCount--;   
  }
}

/***********************************************************************
�������ƣ�RS485_Send(unsigned char ch)
��    �ܣ�RS485���͸��ַ�
���������
���������
��дʱ�䣺2012.11.22
�� д �ˣ�
ע    �⣺
***********************************************************************/
void USART3_Send_Data(unsigned char *send_buff,unsigned int length)
{
 	unsigned int i = 0;
	USART3_485_TX_ENABLE;		//485����ʹ��
	TimeDelay(100);		//������ʱ��ע����ʱ�ĳ��̸��ݲ�����������������ԽС����ʱӦ��Խ��
//	__disable_irq();
	for(i = 0;i < length;i ++)
	{			
		USART3->DR = send_buff[i];
		while((USART3->SR&0X40)==0);	
	}
	USART3_485_RX_ENABLE;  		//485����ʹ��
}

