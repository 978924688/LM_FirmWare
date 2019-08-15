

#include "bsp_i2c_tcs2414.h"
#include "bsp_i2c_gpio.h"
#include "m_usart1.h"
#include "delay.h"
#include "rgb_math.h"


uint8_t				triggerMode_ 	 = (INTEG_MODE_FREE | INTEG_PARAM_PULSE_COUNT1);
uint8_t				interruptSource_ = (INT_SOURCE_CLEAR);
uint8_t				interruptMode_ 	 = (INTR_LEVEL | INTR_PERSIST_EVERY);
uint8_t				gainAndPrescaler_= (GAIN_1 | PRESCALER_4); //yuan =PRESCALER_1
uint8_t				sensorAddress_ 	 = (COLOR_SENSOR_ADDR);

u16 Tcs3414_Read_word(u8 addr);

/*
 ===============================================================================
    IIC写一个字节 ;
    reg:寄存器地址 ;
    data:数据 ;
    返回值:0,正常 ;
    其他,错误代码
 ===============================================================================
 */
uint8_t TCS_Write_Byte(uint8_t reg, uint8_t data)
{
	i2c_Start();
	i2c_SendByte(COLOR_SENSOR_ADDR<<1 | I2C_WR);
	if (i2c_WaitAck() != 0)
	{
		i2c_Stop();
		printf("send COLOR_SENSOR_ADDR error!\n");
		return 1;
	}
	i2c_SendByte(reg);
	if (i2c_WaitAck() != 0)
	{
		i2c_Stop();
		printf("send reg error!\n");
		return 1;
	}
	i2c_SendByte(data);
	if (i2c_WaitAck() != 0)
	{
		i2c_Stop();
		printf("send data error!\n");
		return 1;
	}
	i2c_Stop();
	return 0;
}


/*
 ===============================================================================
    IIC读一个字节 ;
    reg:寄存器地址 ;
    返回值:读到的数据
 ===============================================================================
 */
uint8_t TCS_Read_Byte(uint8_t reg)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	uint8_t I2cRecvTemp = 0x00;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	i2c_Start();
	i2c_SendByte(COLOR_SENSOR_ADDR<<1 | I2C_WR);
	if (i2c_WaitAck() != 0)
	{
		i2c_Stop();
		printf("send COLOR_SENSOR_ADDR error_1\n");
		return 1;
	}
	i2c_SendByte(reg);
	if (i2c_WaitAck() != 0)
	{
		i2c_Stop();
		printf("send reg error\n");
		return 1;
	}
	
	i2c_Start();
	i2c_SendByte(COLOR_SENSOR_ADDR<<1 | I2C_RD);
	if (i2c_WaitAck() != 0)
	{
		i2c_Stop();
		printf("send COLOR_SENSOR_ADDR error_2\n");
		return 1;
	}
	
	I2cRecvTemp = i2c_ReadByte();
	i2c_NAck();//must add
	i2c_Stop();
	return I2cRecvTemp;
}

/*
 ===============================================================================
 ===============================================================================
 */
void set_TimingReg(void)
{
	
	TCS_Write_Byte(REG_TIMING, triggerMode_); //triggerMode_ = 0
	delay_ms(10);
	//usleep(10 * 1000);
}

/*
 ===============================================================================
 ===============================================================================
 */
void set_InterruptSourceReg(void)
{
	TCS_Write_Byte(REG_INT_SOURCE, 0x01); //interruptSource_ = 0x03
	delay_ms(10);
	//usleep(10 * 1000);
}

/*
 ===============================================================================
 ===============================================================================
 */
void set_InterruptControlReg(void)
{
	TCS_Write_Byte(REG_INTRRUPT, interruptMode_);
	delay_ms(10);
	//usleep(10 * 1000);
}

/*
 ===============================================================================
 ===============================================================================
 */
void set_Gain(void)
{
	TCS_Write_Byte(REG_GAIN, gainAndPrescaler_);
	delay_ms(10);
//	//usleep(10 * 1000);
}

/*
 ===============================================================================
 ===============================================================================
 */
void set_EnableADC(void)
{
	TCS_Write_Byte(REG_CONTROL, CTL_DAT_INIITIATE);
	delay_ms(10);
//	//usleep(10 * 1000);
}

/*
 ===============================================================================
 ===============================================================================
 */
uint8_t clearInterrupt(void)
{
	i2c_SendByte(COLOR_SENSOR_ADDR);
	if (i2c_WaitAck() != 0)
	{
		i2c_Stop();
		return 1;
	}
	
	i2c_SendByte(CLR_INT);
	if (i2c_WaitAck() != 0)
	{
		i2c_Stop();
		return 1;
	}

	/*
	 * mraa_i2c_stop(i2c);
	 */
	return 0;
}




/*
 ===============================================================================
    IIC读一块区域 ;
    data:存放数据的地址地址 ;
    len:读的次数
 ===============================================================================
 */
uint8_t TCS_Read_Block(uint8_t reg, uint8_t len, uint8_t *data)
{
	/*~~~~~~~~*/
	int num = 0;
	
	/*~~~~~~~~*/
	i2c_Start();
	i2c_SendByte(COLOR_SENSOR_ADDR<<1 | I2C_WR);
	if (i2c_WaitAck() != 0)
	{
		i2c_Stop();
		printf("TCS read Block error1!\n");
		return 1;
	}
	
	i2c_SendByte(reg);
	if (i2c_WaitAck() != 0)
	{
		i2c_Stop();
		printf("TCS read Block error2!\n");
		return 1;
	}
	
	i2c_Start();
	i2c_SendByte(COLOR_SENSOR_ADDR<<1 | I2C_RD);
	if (i2c_WaitAck() != 0)
	{
		i2c_Stop();
		printf("TCS read Block error3!\n");
		return 1;
	}
	
	num = i2c_ReadByte();
	i2c_Ack();
	
	num = i2c_read_num_bytes(data, len);
	i2c_Stop();
	
	return num;
	//printf("RecevNum:%d   ", num);
}




/********************************************************************
*name:			Tcs3414_Read_word
*description:	读取指定的寄存器的一个word
*param: 		u8 addr 寄存器地址
*return: u16 
********************************************************************/
u16 Tcs3414_Read_word(u8 addr)
{
	u16 temp = 0;
	
	i2c_Start();				//数据手册 P12 Figure.19 读取word 数据
	i2c_SendByte(0x39<<1 | 0x00);
	i2c_WaitAck();
	i2c_SendByte(addr);
	i2c_WaitAck();
	
	i2c_Start();
	i2c_SendByte(0x39<<1 | 0x01);
	i2c_WaitAck();
	
	temp = i2c_ReadByte(); // Byte Count
	i2c_Ack();
	
//	temp = i2c_ReadByte();
//	i2c_Ack();
	
	temp = (i2c_ReadByte()<<8) | temp;
	i2c_Ack();
	i2c_Stop();
	return temp;
}


/*
*********************************************************************************************************
*	函 数 名: ee_CheckOk
*	功能说明: 判断串行EERPOM是否正常
*	形    参：无
*	返 回 值: 1 表示正常， 0 表示不正常
*********************************************************************************************************
*/
uint8_t TCS3414_CheckOk(void)
{
	if (i2c_CheckDevice(COLOR_SENSOR_ADDR) == 0)
	{
		return 1;
	}
	else
	{
		/* 失败后，切记发送I2C总线停止信号 */
		i2c_Stop();		
		return 0;
	}
}

void TCS3414_Init(void)
{
	delay_ms(100);
	set_TimingReg();
	set_InterruptSourceReg();
	set_InterruptControlReg();
	set_Gain();
	set_EnableADC();
	delay_ms(100);
}


uint8_t readID(void)
{
	return TCS_Read_Byte(REG_ID);	/* TCS3414-->0X11 */
}


/*
 ===============================================================================
 ===============================================================================
 */
void readRGB_Block(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
	uint16_t		Green, Red, Blue, Clear;
//	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#if 0	
	uint8_t color[8] = { 0x00 },
	i = 0;
	for(i = 0; i < 8; i++) color[i] = 0;
	TCS_Read_Block(REG_GREEN_LOW, 8, color); //REG_GREEN_LOW
	Green = (color[1] << 8) | color[0];
	Red = (color[3] << 8) | color[2];
	Blue = (color[5] << 8) | color[4];
	Clear = (color[7] << 8) | color[6];
#else
//	Green = Tcs3414_Read_word(0xA0 | 0x10); //REG_GREEN_LOW
//	Red   = Tcs3414_Read_word(0xA0 | 0x12);
//	Blue   = Tcs3414_Read_word(0xA0 | 0x14);
//	Clear   = Tcs3414_Read_word(0xA0 | 0x16);
	uint8_t color[8] = { 0x00 },i=0;
	for(i=0;i<8;i++)
	{
		color[i] = TCS_Read_Byte(0x90+i);
	}
	Green = (color[1]<<8)|color[0];
	Red   = (color[3]<<8)|color[2];
	Blue  = (color[5]<<8)|color[4];
	Clear = (color[7]<<8)|color[6];
	
	
	/* Calculate */
	analog_to_RGB(Red,Green,Blue);

#endif	
	
//	printf("Green:%d Red :%d Blue:%d Clear:%d\n", Green, Red, Blue, Clear);
}



