

/* Debug Log: */
2019-7-27  ADS1118 延时500ms有问题，延时1000ms正常，WHY
电流：3.69     2.86   3.81
电压：14.812   8.881  11.846


2019-06-23
	开始搭建工程：

PD8 == LDAC_1  		(OUT)
PD9 == DAC_EN_1		(out)
PD10 == RESTSEL_1	(OUT) 0:二进制   1：二进制补码
PD11 == RST_1 		(OUT) 0: reset

PD12 == LDAC_2      (OUT) Load DACs; rising edge triggered, loads all DAC register
PD13 == DAC_EN_2	(OUT)
PD14 == RESTSEL_2	(OUT) 0:二进制   1：二进制补码
PD15 == RST_2		(OUT) 0: reset


		
Module Connect
(1) ADS1118 (16bit ADC)
SPI_2_CS_1 -> PB8
SPI_2_CS_2 -> PB7
SPI_2_CS_3 -> PB6
SPI_2_CS_4 -> PB5
SPI_2_CS_5 -> PB4
SPI_2_CS_6 -> PB3
SPI_2_CS_7 -> PB1
SPI_2_CS_8 -> PB0

SPI_2_SCK  -> PB13
SPI_2_miso -> PB14
SPI_2_MOSI -> PB15

(2) AD7606 (16bit ADC)
Common use:
ADC_SRESET -> PD3
ADC_SCK	   -> PC7
ADC_DOUTB  -> PB12

AD7606 chip_1:
ADC_CONVST_1 -> PC10
ADC_CS_1     -> PC6
ADC_1_BUSY   -> PD7
FRSTDATA_1   -> PD5

AD7606 chip_2:
ADC_CONVST_2 -> PC9
ADC_CS_2	 -> PC5
ADC_2_BUSY	 -> PD6
FRSTDATA_2	 -> PD4

(3) DAC7565  (14bit DAC)
common use Pin
SPI_1_CS_1	-> PA4
SPI_1_CS_2  -> PA3
SPI_1_CS_3  -> PA2
SPI_1_CS_4  -> PA1

SPI_1_SCK	-> PA5
SPI_1_MOSI  -> PA7

+++++ DAC7565 chip_1:
LDAC_1		-> PD8
DAC_EN_1	-> PD9
RESTSEL_1	-> PD10
RST_1		-> PD11

+++++ DAC7565 chip_2:
LDAC_2		-> PD12
DAC_EN_2	-> PD13
RESTSEL_2	-> PD14
RST_2		-> PD15

(4) AD5551  (16bit DAC  1 channel)
SPI_1_CS_2 -> PA3
SPI_1_SCK  -> PA5
SPI_1_MOSI -> PA7

DG408
A0 -> PA12
A1 -> PD0
A2 -> PD1
EN -> PA11






>>>>>>>>>>>>>>>>>> study function description: <<<<<<<<<<<<<<<<<<<<<<

FLOW_1:
set current value -> 与上次记录比对，若不同进入学习状态
-> 根据设定电流，若>1mA继电器拨到10R，否则不变 -> EN通道使能打开 -> DAC_CH1(DAC7565) =10V
-> ad5551控制外部大电源(0-10 <--> 0-380 ,每次步进1V，延时100ms，之后采集一下电流，
并与目标值比对，>=后，1关闭大电源（置零），2记录存储此时AD5551送出的值+3,)，
并将DAC_CH1(DAC7565) =0V; 

FLOW_2：
开启大电源为上次记录值-> DAC_CH1(DAC7565控制三极管比例开关)从0开始累加一次，并采集电流，当电流>=设定值时，
(1)记录此时的DAC_CH1(DAC7565控制三极管)，(2)记录此时的AD1118的灯串电压值,(3)记录AD5551的值(外部电源);

Question_1： 两次FLOW中，DAC_CH1(DAC7565)有何变化？
Question_1： 学习完成后，是否关闭继电器和DAC_CH1(DAC7565)？


AD5551 : 0-10V <-> 0-360

学习完成之后：
Func 2:
Button(开始) ->按下开始按钮后，直接打开上次学习时记录的值 -> 读取ADS1118电压值，并与第一路相比对
如果小于或大于2.5V，表示该路故障，状态寄存器置1。

学习电流值分mA & uA
100000
65535

/* 寄存器映射区 */

1)13.62    13.64  4.416   49.62
2->11.32   12.90  4.47    49.43
3->14.42   14.27  4.69	  48.98
4->14.80   14.80  4.70 	  49.16

Vin=14.86    14.86
	5mA

/* 
按下开始按钮，8路输出，并读出电压电流，并与第一路作比对。并存放比对寄存器；

*/
7mA   5mA   6mA	  8mA   9mA   10mA
5.88  3.81  4.98  6.88  8.03  8.88

Pin5   68.6
Audion 68.5
R177   80.7  68
