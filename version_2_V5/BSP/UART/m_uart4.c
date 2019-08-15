#include "main.h" 
#include <string.h>	   

char Uart2_Buf[Buf2_Max];    //����2�����ַ�����
u8 Times=0,First_Int = 0,shijian=0;
u8 glx;
void Print(u8 *str)
{
  u16 len = 0;	
  len = strlen((const char *)str);
  UART4_Send_Data(str, len);
}	

void UART4_Send_Byte(unsigned char data)
{	  
  	UART4->DR = data;
  	while((UART4->SR&0X40)==0);  
}
 
// UART4 for Console
void UART4_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure; 
 NVIC_InitTypeDef NVIC_InitStructure;
/****************************����ΪUART4����**************************/
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);   //???
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;           
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_Init(GPIOC, &GPIO_InitStructure);  	   
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;  
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8λ����
  USART_InitStructure.USART_StopBits = USART_StopBits_1;//1λֹͣλ
  USART_InitStructure.USART_Parity = USART_Parity_No;//wuУ��
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //Ӳ��������ʧ��*/
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //���ͺͽ���ʹ��
  USART_Init(UART4, &USART_InitStructure); 
/*  USART_InitStructure.USART_BaudRate = CommParmCfg.baudrate;
  USART_InitStructure.USART_WordLength = CommParmCfg.databit;//8λ����
  USART_InitStructure.USART_StopBits = CommParmCfg.stopbit;//1λֹͣλ
  USART_InitStructure.USART_Parity = CommParmCfg.paritybit;//żУ��
  USART_InitStructure.USART_HardwareFlowControl = CommParmCfg.flowctrl; //Ӳ��������ʧ�� */
  NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;//??1????
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//?????3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;		//????3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ????
	NVIC_Init(&NVIC_InitStructure);	
	

  USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);

  
  USART_ClearITPendingBit(UART4, USART_IT_TC);//����ж�TCλ
	USART_ClearITPendingBit(UART4, USART_IT_RXNE);//����ж�RXλ 
	 
	USART_Cmd(UART4, ENABLE); 
	// UART4_Send_Byte(0x20); // *
}

void UART4_Send_Data(unsigned char *send_buff,unsigned int length)
{
	unsigned int i = 0;
	for(i = 0;i < length;i ++)
	{  	  
		UART4->DR = send_buff[i];
    	while((UART4->SR&0X40)==0);  
	}
}

void UART4_IRQHandler(void)  
{
	char buf[100];
 		u8 Res=0;
	if(USART_GetITStatus(UART4,USART_IT_RXNE) != RESET)
	{
		USART_ClearITPendingBit(UART4, USART_IT_RXNE);
			Res =USART_ReceiveData(UART4);
			Uart2_Buf[First_Int] = Res;  	  //�����յ����ַ����浽������
			First_Int++;                			//����ָ������ƶ�
			if(First_Int >= Buf2_Max)       		//���������,������ָ��ָ�򻺴���׵�ַ
			{
				First_Int = 0;
			}  
	}		
}

//void UART4_IRQHandler(void)  
//{
//  u8 temp;
//  if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)//���յ�������
//  {  
//    temp=UART4->DR;
// //   DAQ_DataRecv(CHANNEL_UART4,temp);
//		Print(&temp);
//  	USART_ClearITPendingBit(UART4, USART_IT_RXNE);
//  }
//  if (USART_GetITStatus(UART4, USART_IT_TXE) != RESET) 
//  {
//    USART_ClearITPendingBit(UART4, USART_IT_TXE);           /* Clear the USART transmit interrupt                  */
//  }
//}

/*******************************************************************************
* ������  : UART1_SendString
* ����    : USART2�����ַ���
* ����    : *s�ַ���ָ��
* ���    : ��
* ����    : �� 
* ˵��    : ��
*******************************************************************************/
void UART4_SendString(char* s)
{
	while(*s)//����ַ���������,ֻҪ���������ǡ�0/'��һֱѭ��
	{
	//		USART_485_TX_ENABLE;
		while(USART_GetFlagStatus(UART4, USART_FLAG_TC)==RESET);  //ֻҪ������ɱ�û����0����һֱִ����һ��
		USART_SendData(UART4 ,*s++);//���͵�ǰ�ַ�
		//	USART_485_RX_ENABLE;
	}
}