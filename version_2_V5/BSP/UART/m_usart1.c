//#include "sys.h"
#include "stm32f10x_usart.h"
#include "m_usart1.h"
#include "m_usart3.h"
#include "m_timer.h"
#include "database.h"

/* ----------------------------------------------------
	CRC lookup table for CRC16 (reverse).
 	Polynomial:  X16 + X15 + X2 + 1 (0x1021)
 	Usage:
 	1.  Init CRC to 0xffff
 	2.  Index = CRC ^ Buffer[I]
 	3.  CRC = (CRC >> 8) ^ CRC_table[Index]
 	4.  When done, swap bytes of CRC
 ------------------------------------------------------ */
const unsigned short modbus_crc16_table[256] = {
    0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
    0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
    0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
    0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
    0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
    0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
    0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
    0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
    0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
    0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
    0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
    0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
    0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
    0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
    0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
    0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
    0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
    0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
    0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
    0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
    0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
    0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
    0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
    0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
    0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
    0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
    0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
    0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
    0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
    0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
    0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
    0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
};


u8 mb_rcv_buffer[50];
u8 *mb_rcv_data_ptr = mb_rcv_buffer;
static u8 mb_rxBufCount = 0;
static volatile u8 mb_msgStatus = 0;
static u8 mb_message_length=0;
static u8 mb_response_buffer[50];
/* Public Variable */
u8 mb_tx_complete = 0;
u8 which_uart_irq_flag = 0; //1:USART1   2:USART3

static void uart_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//USART1_RX ->PA10          
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // _IN_FLOATING
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
		/* RS485_Control */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	TX_485_DI;  //disable send
}

/*
	The message ends when no characers are received for 3.5 character timeout
*/
void check_mb_rcv_buffer()
{
    u16 calc_crc;

    mb_disable_char_timer();	/* disable char S/W timer */
    mb_message_length = mb_rxBufCount;
    if((g_address) == MB_SLAVE_ADDRESS)
    {
        calc_crc = modbus_crc16(mb_rcv_buffer, (mb_message_length-2));

        /* Check the CRC */
        if ( calc_crc != MB_MB_RCVD_CRC)
        {
            mb_reset_rxbuf();
            return;
        }
		
        mb_rcv_data_ptr = &mb_rcv_buffer[0];	/* Restore ptr to top of buffer */
        mb_rxBufCount = 0;         /* Reset the byte pointer */
        mb_msgStatus = 1;		/* Set Message indicator */

        /******** test test test *****************************************************/
        //CLEAR_LED_1;
        /*****************************************************************************/
    }
    else
    {
        mb_reset_rxbuf();
    }
}

/****************************************************************************
   Resets the recieve buffer after a timeout or an error in transmission
*/
void mb_reset_rxbuf(void)
{
    mb_rcv_data_ptr = &mb_rcv_buffer[0];  // Restore ptr to top of buffer
    mb_rxBufCount = 0;                 // Reset the byte pointer
    mb_msgStatus  = 0;             // Set Message indicator
    mb_message_length = 0;
    mb_disable_char_timer();
}

u8 mb_message_rev_ok(void)
{
    return  mb_msgStatus;
}

void USART1_Send_Byte(unsigned char data)
{
  	USART1->DR = data;
  	while((USART1->SR&0X40)==0);
}

void USART1_Configuration(void)
{
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);	//|RCC_APB2Periph_AFIO 

  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //硬件流控制失能
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; 
  USART_Init(USART1, &USART_InitStructure); 
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

  USART_Cmd(USART1, ENABLE); 
  USART_ClearITPendingBit(USART1, USART_IT_TC|USART_IT_TXE);
  USART_ClearITPendingBit(USART1, USART_IT_RXNE);

  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void uart1_init(void)
{
	uart_gpio_init(); //GPIO Init
	USART1_Configuration();
}


static void TimeDelay(uint32_t nCount)
{
  while(nCount > 0)
  { 
    nCount--;   
  }
}

void USART1_Send_Data(u8 *send_buff,u8 length)
{
	unsigned int i = 0;
//	u16 delay;

	TX_485_EN;
	TimeDelay(100); //LM Add
	
	for(i=0;i<length;i++)
	{
		USART1->DR = send_buff[i];
		while((USART1->SR&0X40)==0);	
	}
	
	TX_485_DI;  //I don't use send finish T3.5
}

void USART1_IRQHandler(void)  
{
 	 u8 Res=0;
	if(USART_GetITStatus(USART1,USART_IT_RXNE) != RESET)
	{
		mb_enable_char_timer();
		
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
			Res =USART_ReceiveData(USART1);
			 /* Load the character into the receive buffer. */
		*(mb_rcv_data_ptr + mb_rxBufCount++) = Res;
		
		which_uart_irq_flag = 1; //distinguish data from uart1 or uart3 as MB Port
	}
}


/*
* this is from usart3, but the function is the same with usart1, use as Modbus func
*/
void USART3_IRQHandler(void)  
{
	u8 Res;
	if(USART_GetITStatus(USART3,USART_IT_RXNE) != RESET)
	{	
		mb_enable_char_timer();
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
		Res =USART_ReceiveData(USART3);
		
		/* Load the character into the receive buffer. */
		*(mb_rcv_data_ptr + mb_rxBufCount++) = Res;
		
		which_uart_irq_flag = 2; //distinguish data from uart1 or uart3 as MB Port
	}
}

void UART1_SendString(char* s)
{
	while(*s)
	{
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET);
		USART_SendData(USART1 ,*s++);
	}
}



///// 重定向c库函数printf到USART1
//int fputc(int ch, FILE *f)
//{
//		/* 发送一个字节数据到USART1 */
//		USART_SendData(USART1, (uint8_t) ch);
//		
//		/* 等待发送完毕 */
//		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);		
//	
//		return (ch);
//}

///// 重定向c库函数scanf到USART1
//int fgetc(FILE *f)
//{
//		/* 等待串口1输入数据 */
//		while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);

//		return (int)USART_ReceiveData(USART1);
//}

/*-----------------------------------------------------------------------
  Procedure:  ModbusCRC_16()
 	Computes a CRC-16 based on the polynomial
    (initial condition = 0xffff): X16 + X15 + X2 + 1 (polynomial = 0x1021)

  Calling Syntax:
       crc = crc_16(msg, bytecnt);
  Input Parameters:
    msg               - (pointer) pointer to a byte array
    bytecnt           - (unsigned int) number of bytes in array
  Output Parameters:  None
  Return Value:
    crc               - (unsigned int) computed CRC (the CRC16 swaped ready to go in a buffer)

--------------------------------------------------------------------------*/


///****************************************************************************
//   Executes Serial interface Modbus (MB) commands and transmits reply message.
//*/
void MB_execute_cmd(void)
{
    u16 length_of_reply=4,index;
    u16 crc=0;
    u8  exception_code = 0;

    switch (MB_FUNCTION_CODE)
    {
        case READ_HOLDING_REGS_FUNCT_CODE: // 0x03  same as 0x04
        case READ_INPUT_REGS_FUNCT_CODE: // 0x04
		{
            if ( (FUNCT_CODE_0x04_QUANTITY_OF_REGS < 1) || (FUNCT_CODE_0x04_QUANTITY_OF_REGS > 255) )
            {
                exception_code = 3;
            }
            else
                exception_code = read_data(FUNCT_CODE_0x04_START_ADDRESS, FUNCT_CODE_0x04_QUANTITY_OF_REGS * 2, &mb_response_buffer[3]);
        
            if (exception_code == 0)
            {
                mb_response_buffer[0] = MB_SLAVE_ADDRESS;
                mb_response_buffer[1] = MB_FUNCTION_CODE;
                mb_response_buffer[2] = FUNCT_CODE_0x04_QUANTITY_OF_REGS * 2; /* 2 bytes per register */
                /* s_sci_a0_xmt_buffer[3] contains Registers value, filled by read_data() */
                length_of_reply = 3 + FUNCT_CODE_0x04_QUANTITY_OF_REGS * 2;
            }
            break;
		}
        // not tested
        case WRITE_SINGLE_REG_FUNCT_CODE: // 0x06
		{
            /* exception_code = 3 won't happen because the statement "if( 0 <= Register Value <= 0xFFFF )" is never false */
            exception_code = write_data(FUNCT_CODE_0x06_REG_ADDRESS, 1 * 2, &mb_rcv_buffer[4]);

            if (exception_code == 0)
            {
                mb_response_buffer[0] = MB_SLAVE_ADDRESS;
                mb_response_buffer[1] = MB_FUNCTION_CODE;
                mb_response_buffer[2] = mb_rcv_buffer[2]; /* FUNCT_CODE_0x06_REG_ADDRESS high */
                mb_response_buffer[3] = mb_rcv_buffer[3]; /* FUNCT_CODE_0x06_REG_ADDRESS low */
                mb_response_buffer[4] = mb_rcv_buffer[4]; /* FUNCT_CODE_0x06_REG_VALUE high */
                mb_response_buffer[5] = mb_rcv_buffer[5]; /* FUNCT_CODE_0x06_REG_VALUE low */
                length_of_reply = 6;
            }
            break;
		}
        case WRITE_MULTIPLE_REGS_FUNCT_CODE: // 0x10
		{
            if ((FUNCT_CODE_0x10_QUANTITY_OF_REGS < 1) || (FUNCT_CODE_0x10_QUANTITY_OF_REGS > 255))
            {
                exception_code = 03; /* out of range */
            }
            else if( FUNCT_CODE_0x10_BYTE_COUNT != (u8)FUNCT_CODE_0x10_QUANTITY_OF_REGS * 2 )
                exception_code = 03; /* out of range */

			if (exception_code == 0)
                exception_code = write_data(FUNCT_CODE_0x10_START_ADDRESS, FUNCT_CODE_0x10_QUANTITY_OF_REGS * 2, &FUNCT_CODE_0x10_REGS_VALUE);
       
            if (exception_code == 0)
            {
                mb_response_buffer[0] = MB_SLAVE_ADDRESS;
                mb_response_buffer[1] = MB_FUNCTION_CODE;
                mb_response_buffer[2] = mb_rcv_buffer[2]; /* FUNCT_CODE_0x10_START_ADDRESS high */
                mb_response_buffer[3] = mb_rcv_buffer[3]; /* FUNCT_CODE_0x10_START_ADDRESS low */
                mb_response_buffer[4] = mb_rcv_buffer[4]; /* FUNCT_CODE_0x10_QUANTITY_OF_REGS high */
                mb_response_buffer[5] = mb_rcv_buffer[5]; /* FUNCT_CODE_0x10_QUANTITY_OF_REGS low */
                length_of_reply = 6; //no  + FUNCT_CODE_0x10_QUANTITY_OF_REGS * 2;
            }
            break;
		}
        // not tested                
        case READ_WRITE_MULTIPLE_REGS_FUNCT_CODE: // 0x17
		{
            /* read input register and write holding registers */
            if( (FUNCT_CODE_0x17_QUANTITY_TO_READ < 1) || (FUNCT_CODE_0x17_QUANTITY_TO_READ > 0x007D) )
                exception_code = 03;
            else if( (FUNCT_CODE_0x17_QUANTITY_TO_WRITE < 1) || (FUNCT_CODE_0x17_QUANTITY_TO_WRITE > 0x0079) )
                exception_code = 03;
            else if(FUNCT_CODE_0x17_WRITE_BYTE_COUNT != (u8)FUNCT_CODE_0x17_QUANTITY_TO_WRITE * 2)
                exception_code = 03;
            
            if (exception_code == 0)
                exception_code = read_data(FUNCT_CODE_0x17_READ_START_ADDRESS, FUNCT_CODE_0x17_QUANTITY_TO_READ * 2, &mb_response_buffer[3]);
            
            if (exception_code == 0)
                exception_code = write_data(FUNCT_CODE_0x17_WRITE_START_ADDRESS, FUNCT_CODE_0x17_QUANTITY_TO_WRITE * 2, &FUNCT_CODE_0x17_WRITE_REGS_VALUE);

            if (exception_code == 0)
            {
                mb_response_buffer[0] = MB_SLAVE_ADDRESS;
                mb_response_buffer[1] = MB_FUNCTION_CODE;
                mb_response_buffer[2] = FUNCT_CODE_0x17_QUANTITY_TO_READ * 2;
                length_of_reply = 3 + FUNCT_CODE_0x17_QUANTITY_TO_READ * 2;
            }
            break;
		}
    
        default:    /* Illegal Function Code. */
		{
            mb_response_buffer[0] = MB_SLAVE_ADDRESS;
            mb_response_buffer[1] = MB_FUNCTION_CODE | HIGH_BIT;
            mb_response_buffer[2] = 01; /* exception_code = 01 */
            length_of_reply = 3;
            break;
		}
    }

    if (exception_code != 0)
    {
        mb_response_buffer[0] = MB_SLAVE_ADDRESS;
        mb_response_buffer[1] = MB_FUNCTION_CODE | HIGH_BIT;
        mb_response_buffer[2] = exception_code;
        length_of_reply = 3;
    }

    index = length_of_reply;

    /* now calculate the CRC16 */
    crc = modbus_crc16(mb_response_buffer, length_of_reply);

    /* swap calculated CRC code */
    mb_response_buffer[index] = crc & 0x00FF; /* low byte */
    mb_response_buffer[index + 1] = (crc & 0xFF00)>>8; /* high byte */
            
    /* Done executing command.  As long as no CRC error was detected, transmit
       reply mesage to host. */
    mb_tx_complete = 0;
//    usci_a0_sendTxBuffer((void*)s_sci_a0_xmt_buffer, length_of_reply + 2); /* +2 for CRC */
	if(which_uart_irq_flag ==1)
	{
		which_uart_irq_flag = 0;
		USART1_Send_Data(mb_response_buffer,length_of_reply + 2); /* +2 for CRC */
	}
	else if(which_uart_irq_flag == 2)
	{
		which_uart_irq_flag = 0;
		USART3_Send_Data(mb_response_buffer,length_of_reply + 2); /* +2 for CRC */
	}
    mb_msgStatus = 0;

    /******** test test test *****************************************************/
    //SET_LED_1;
//    CLEAR_LED_1;
    /*****************************************************************************/
    return;
}



unsigned short  modbus_crc16(unsigned char *crcBuf, int nSize)
{
unsigned char  index;
unsigned short crc;
int i;

	//cal the CRC
    crc = 0xFFFF;                   // start with all 1's for a modbus CRC

    // process each character in the message - 2 steps per char only!
    for( i = 0; i < nSize; i++)
    {
        index = (unsigned char)crc ^ crcBuf[i];
        crc = (crc >> 8) ^ modbus_crc16_table[index];
    }

    return(crc);
}

/*
	专门用于Modbus发送
*/
void encode_switch_send(u8 _data)
{
	u8 sn_data[8];
	u16 crc_data;
	sn_data[0] = 0x01;
	sn_data[1] = 0x06;
	sn_data[2] = 0x00;
	sn_data[3] = 0x01;
	sn_data[4] = 0x00;
	sn_data[5] = _data;
	
	crc_data = modbus_crc16(sn_data, 6);
	sn_data[6] = (crc_data & 0xff);
	sn_data[7] = ((crc_data>>8) & 0xff);
	
	USART1_Send_Data(sn_data,8);
}
