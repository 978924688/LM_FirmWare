#ifndef  _UART4_H_											 
#define  _UART4_H_
void UART4_Configuration(void);
void UART4_Send_Data(unsigned char *send_buff,unsigned int length);		 
void UART4_SendString(char* s);
//������4G�ļ����õ�
#define Buf2_Max 600 					  //����2���泤��
extern char Uart2_Buf[Buf2_Max];    //����2�����ַ�����
extern unsigned char Times,First_Int ,shijian;
#endif
