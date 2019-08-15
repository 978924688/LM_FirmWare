


/***********************************************************************
doc_name£ºUSART1.h
Function£º
Time    £º2019.06.05
Author  £ºLiuMing(Thermofisher)
Caution £º
***********************************************************************/
#ifndef  _USART1_H_
#define  _USART1_H_

#define TX_485_EN	PAout(8)=1
#define TX_485_DI   PAout(8)=0
/*************************define**************************************/
#define 	g_address   0x01


/* mb macros to parse values from buffer */
#define MB_RCVD_CRC \
        ((mb_rcv_buffer[mb_rxBufCount-3] << 8) \
        | (mb_rcv_buffer[mb_rxBufCount-2]))
//#define MB_DATA_LENGTH         mb_rcv_buffer[4]
//#define MB_FUNCTION_CODE       mb_rcv_buffer[2]
//#define MB_REG_START_ADDRESS   mb_rcv_buffer[3]
//#define MB_SLAVE_ADDRESS       mb_rcv_buffer[1]
#define MB_DATA_LENGTH         mb_rcv_buffer[3]
#define MB_FUNCTION_CODE       mb_rcv_buffer[1]
#define MB_REG_START_ADDRESS   mb_rcv_buffer[2]
#define MB_SLAVE_ADDRESS       mb_rcv_buffer[0]

/* mb Modbus macros to parse values from buffer */
#define MB_MB_RCVD_CRC \
        ((mb_rcv_buffer[mb_rxBufCount-1] << 8) \
        | (mb_rcv_buffer[mb_rxBufCount-2]))

#define MB_MB_SLAVE_ADDRESS                mb_rcv_buffer[0]
#define MB_MB_FUNCTION_CODE                mb_rcv_buffer[1]

#define FUNCT_CODE_0x03_START_ADDRESS           ((mb_rcv_buffer[2] << 8) | mb_rcv_buffer[3] )
#define FUNCT_CODE_0x03_QUANTITY_OF_REGS        ((mb_rcv_buffer[4] << 8) | mb_rcv_buffer[5] )

#define FUNCT_CODE_0x04_START_ADDRESS           ((mb_rcv_buffer[2] << 8) | mb_rcv_buffer[3] )
#define FUNCT_CODE_0x04_QUANTITY_OF_REGS        ((mb_rcv_buffer[4] << 8) | mb_rcv_buffer[5] )

#define FUNCT_CODE_0x06_REG_ADDRESS             ((mb_rcv_buffer[2] << 8) | mb_rcv_buffer[3] )
#define FUNCT_CODE_0x06_REG_VALUE               ((mb_rcv_buffer[4] << 8) | mb_rcv_buffer[5] )

#define FUNCT_CODE_0x10_START_ADDRESS           ((mb_rcv_buffer[2] << 8) | mb_rcv_buffer[3] )
#define FUNCT_CODE_0x10_QUANTITY_OF_REGS        ((mb_rcv_buffer[4] << 8) | mb_rcv_buffer[5] )
#define FUNCT_CODE_0x10_BYTE_COUNT              mb_rcv_buffer[6]
#define FUNCT_CODE_0x10_REGS_VALUE              mb_rcv_buffer[7]

#define FUNCT_CODE_0x17_READ_START_ADDRESS      ((mb_rcv_buffer[2] << 8) | mb_rcv_buffer[3] )
#define FUNCT_CODE_0x17_QUANTITY_TO_READ        ((mb_rcv_buffer[4] << 8) | mb_rcv_buffer[5] )
#define FUNCT_CODE_0x17_WRITE_START_ADDRESS     ((mb_rcv_buffer[6] << 8) | mb_rcv_buffer[7] )
#define FUNCT_CODE_0x17_QUANTITY_TO_WRITE       ((mb_rcv_buffer[8] << 8) | mb_rcv_buffer[9] )
#define FUNCT_CODE_0x17_WRITE_BYTE_COUNT        mb_rcv_buffer[10]
#define FUNCT_CODE_0x17_WRITE_REGS_VALUE        mb_rcv_buffer[11]


/* Modbus function code */
#define READ_HOLDING_REGS_FUNCT_CODE            0x03
#define READ_INPUT_REGS_FUNCT_CODE              0x04
#define WRITE_SINGLE_REG_FUNCT_CODE             0x06
#define WRITE_MULTIPLE_REGS_FUNCT_CODE          0x10
#define READ_WRITE_MULTIPLE_REGS_FUNCT_CODE     0x17

#define READ_HOLDING_REGS_FUNCT_CODE_ERR        0x83
#define READ_INPUT_REGS_FUNCT_CODE_ERR          0x84
#define WRITE_SINGLE_REG_FUNCT_CODE_ERR         0x86
#define WRITE_MULTIPLE_REGS_FUNCT_CODE_ERR      0x90
#define READ_WRITE_MULTIPLE_REGS_FUNCT_CODE_ERR 0x97

/* Modbus register addresses */
#define OUT_REGS_ADDR_START            0x0000
#define OUT_REGS_ADDR_END              511//0x01FF  //511

#define IN_REGS_ADDR_START             0x0200  //512
#define IN_REGS_ADDR_END               0x03FF  //1023

//#define FWUPDATE_REGS_ADDR_START       0x0A00
//#define FWUPDATE_REGS_ADDR_END         0x0BFF


/* Exported function variable ----------------------------------------------*/
/* Private Constants */
#define HIGH_BIT        0x80


//extern unsigned char  g_USART1_RxArray[USART1_BUFF_SIZE];
//extern unsigned char  g_USART1_TxArray[USART1_BUFF_SIZE];
//extern unsigned char  g_USART1_RxBuff[USART1_BUFF_SIZE];
//extern unsigned char  g_USART1_TxBuff[USART1_BUFF_SIZE];
//extern unsigned char  g_USART1_RxFlag;


void USART1_Configuration(void);
void USART1_Send_Data(unsigned char *send_buff,unsigned char length);
//void TimeDelay(uint32_t nCount);

void UART1_SendString(char* s);
void USART1_Send_Byte(unsigned char data);



unsigned short  modbus_crc16(unsigned char *crcBuf, int nSize);
void mb_reset_rxbuf(void);
void check_mb_rcv_buffer(void);
unsigned char mb_message_rev_ok(void);
void MB_execute_cmd(void);
void encode_switch_send(unsigned char _data);

void uart1_init(void);








#endif


