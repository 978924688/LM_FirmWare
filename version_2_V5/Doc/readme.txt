

/* Debug Log: */
2019-7-27  ADS1118 ��ʱ500ms�����⣬��ʱ1000ms������WHY
������3.69     2.86   3.81
��ѹ��14.812   8.881  11.846


2019-06-23
	��ʼ����̣�

PD8 == LDAC_1  		(OUT)
PD9 == DAC_EN_1		(out)
PD10 == RESTSEL_1	(OUT) 0:������   1�������Ʋ���
PD11 == RST_1 		(OUT) 0: reset

PD12 == LDAC_2      (OUT) Load DACs; rising edge triggered, loads all DAC register
PD13 == DAC_EN_2	(OUT)
PD14 == RESTSEL_2	(OUT) 0:������   1�������Ʋ���
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
set current value -> ���ϴμ�¼�ȶԣ�����ͬ����ѧϰ״̬
-> �����趨��������>1mA�̵�������10R�����򲻱� -> ENͨ��ʹ�ܴ� -> DAC_CH1(DAC7565) =10V
-> ad5551�����ⲿ���Դ(0-10 <--> 0-380 ,ÿ�β���1V����ʱ100ms��֮��ɼ�һ�µ�����
����Ŀ��ֵ�ȶԣ�>=��1�رմ��Դ�����㣩��2��¼�洢��ʱAD5551�ͳ���ֵ+3,)��
����DAC_CH1(DAC7565) =0V; 

FLOW_2��
�������ԴΪ�ϴμ�¼ֵ-> DAC_CH1(DAC7565���������ܱ�������)��0��ʼ�ۼ�һ�Σ����ɼ�������������>=�趨ֵʱ��
(1)��¼��ʱ��DAC_CH1(DAC7565����������)��(2)��¼��ʱ��AD1118�ĵƴ���ѹֵ,(3)��¼AD5551��ֵ(�ⲿ��Դ);

Question_1�� ����FLOW�У�DAC_CH1(DAC7565)�кα仯��
Question_1�� ѧϰ��ɺ��Ƿ�رռ̵�����DAC_CH1(DAC7565)��


AD5551 : 0-10V <-> 0-360

ѧϰ���֮��
Func 2:
Button(��ʼ) ->���¿�ʼ��ť��ֱ�Ӵ��ϴ�ѧϰʱ��¼��ֵ -> ��ȡADS1118��ѹֵ�������һ·��ȶ�
���С�ڻ����2.5V����ʾ��·���ϣ�״̬�Ĵ�����1��

ѧϰ����ֵ��mA & uA
100000
65535

/* �Ĵ���ӳ���� */

1)13.62    13.64  4.416   49.62
2->11.32   12.90  4.47    49.43
3->14.42   14.27  4.69	  48.98
4->14.80   14.80  4.70 	  49.16

Vin=14.86    14.86
	5mA

/* 
���¿�ʼ��ť��8·�������������ѹ�����������һ·���ȶԡ�����űȶԼĴ�����

*/
7mA   5mA   6mA	  8mA   9mA   10mA
5.88  3.81  4.98  6.88  8.03  8.88

Pin5   68.6
Audion 68.5
R177   80.7  68
