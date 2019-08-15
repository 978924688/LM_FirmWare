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
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;//RS485方向控制
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	USART3_485_TX_ENABLE;
}


///// 重定向c库函数printf到USART1
//int fputc(int ch, FILE *f)
//{
//		/* 发送一个字节数据到USART1 */
//		USART_SendData(USART3, (uint8_t) ch);
//		
//		/* 等待发送完毕 */
//		while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);		
//	
//		return (ch);
//}

///// 重定向c库函数scanf到USART1
//int fgetc(FILE *f)
//{
//		/* 等待串口1输入数据 */
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
函数名称：RS485_Send(unsigned char ch)
功    能：RS485发送个字符
输入参数：
输出参数：
编写时间：2012.11.22
编 写 人：
注    意：
***********************************************************************/
void USART3_Send_Data(unsigned char *send_buff,unsigned int length)
{
 	unsigned int i = 0;
	USART3_485_TX_ENABLE;		//485发送使能
	TimeDelay(100);		//稍作延时，注意延时的长短根据波特率来定，波特率越小，延时应该越长
//	__disable_irq();
	for(i = 0;i < length;i ++)
	{			
		USART3->DR = send_buff[i];
		while((USART3->SR&0X40)==0);	
	}
	USART3_485_RX_ENABLE;  		//485接收使能
}

