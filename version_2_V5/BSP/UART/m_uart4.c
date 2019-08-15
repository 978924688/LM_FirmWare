#include "main.h" 
#include <string.h>	   

char Uart2_Buf[Buf2_Max];    //串口2接收字符缓存
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
/****************************以下为UART4配置**************************/
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
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8位数据
  USART_InitStructure.USART_StopBits = USART_StopBits_1;//1位停止位
  USART_InitStructure.USART_Parity = USART_Parity_No;//wu校验
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //硬件流控制失能*/
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //发送和接受使能
  USART_Init(UART4, &USART_InitStructure); 
/*  USART_InitStructure.USART_BaudRate = CommParmCfg.baudrate;
  USART_InitStructure.USART_WordLength = CommParmCfg.databit;//8位数据
  USART_InitStructure.USART_StopBits = CommParmCfg.stopbit;//1位停止位
  USART_InitStructure.USART_Parity = CommParmCfg.paritybit;//偶校验
  USART_InitStructure.USART_HardwareFlowControl = CommParmCfg.flowctrl; //硬件流控制失能 */
  NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;//??1????
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//?????3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;		//????3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ????
	NVIC_Init(&NVIC_InitStructure);	
	

  USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);

  
  USART_ClearITPendingBit(UART4, USART_IT_TC);//清除中断TC位
	USART_ClearITPendingBit(UART4, USART_IT_RXNE);//清除中断RX位 
	 
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
			Uart2_Buf[First_Int] = Res;  	  //将接收到的字符串存到缓存中
			First_Int++;                			//缓存指针向后移动
			if(First_Int >= Buf2_Max)       		//如果缓存满,将缓存指针指向缓存的首地址
			{
				First_Int = 0;
			}  
	}		
}

//void UART4_IRQHandler(void)  
//{
//  u8 temp;
//  if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)//接收到了数据
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
* 函数名  : UART1_SendString
* 描述    : USART2发送字符串
* 输入    : *s字符串指针
* 输出    : 无
* 返回    : 无 
* 说明    : 无
*******************************************************************************/
void UART4_SendString(char* s)
{
	while(*s)//检测字符串结束符,只要结束符不是‘0/'就一直循环
	{
	//		USART_485_TX_ENABLE;
		while(USART_GetFlagStatus(UART4, USART_FLAG_TC)==RESET);  //只要发送完成标没有置0，则一直执行下一句
		USART_SendData(UART4 ,*s++);//发送当前字符
		//	USART_485_RX_ENABLE;
	}
}