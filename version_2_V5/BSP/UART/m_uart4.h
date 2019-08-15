#ifndef  _UART4_H_											 
#define  _UART4_H_
void UART4_Configuration(void);
void UART4_Send_Data(unsigned char *send_buff,unsigned int length);		 
void UART4_SendString(char* s);
//下面是4G文件夹用的
#define Buf2_Max 600 					  //串口2缓存长度
extern char Uart2_Buf[Buf2_Max];    //串口2接收字符缓存
extern unsigned char Times,First_Int ,shijian;
#endif
