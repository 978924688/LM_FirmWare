#ifndef  _USART2_H_
#define  _USART2_H_

#define USART2_RX_ENABLE 	GPIO_SetBits(GPIOA , GPIO_Pin_1)	//接收使能
#define USART2_TX_ENABLE	GPIO_SetBits(GPIOA, GPIO_Pin_0)  	//发送使能
#define USART2_RX_DISABLE 	GPIO_ResetBits(GPIOA , GPIO_Pin_1)	//接收使能
#define USART2_TX_DISABLE	GPIO_ResetBits(GPIOA, GPIO_Pin_0)  	//发送使能

void USART2_Configuration(void);
void USART2_Send_Data(unsigned char *send_buff,unsigned int length);  



#endif


