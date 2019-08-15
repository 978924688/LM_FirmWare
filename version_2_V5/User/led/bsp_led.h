#ifndef __LED_H
#define	__LED_H

#include "sys.h"

#define KEY0_READ_STATUS  PCin(0)
#define KEY0_ON		1
#define KEY0_OFF	0

#define RELAY_DISCHARGE PAout(15)  //discharge

#define RELAY_CH1_ON  PEout(8)=1  //poke to 10R gear
#define RELAY_CH1_OFF PEout(8)=0  //

#define RELAY_CH2_ON  PEout(9)=1
#define RELAY_CH2_OFF PEout(9)=0

#define RELAY_CH3_ON  PEout(10)=1
#define RELAY_CH3_OFF PEout(10)=0

#define RELAY_CH4_ON  PEout(11)=1
#define RELAY_CH4_OFF PEout(11)=0

#define RELAY_CH5_ON  PEout(12)=1
#define RELAY_CH5_OFF PEout(12)=0

#define RELAY_CH6_ON  PEout(13)=1
#define RELAY_CH6_OFF PEout(13)=0

#define RELAY_CH7_ON  PEout(14)=1
#define RELAY_CH7_OFF PEout(14)=0

#define RELAY_CH8_ON  PEout(15)=1
#define RELAY_CH8_OFF PEout(15)=0

#define EN_1_ON   PEout(7)=1
#define EN_1_OFF  PEout(7)=0

#define EN_2_ON   PEout(6)=1
#define EN_2_OFF  PEout(6)=0

#define EN_3_ON   PEout(5)=1
#define EN_3_OFF  PEout(5)=0

#define EN_4_ON   PEout(4)=1
#define EN_4_OFF  PEout(4)=0

#define EN_5_ON   PEout(3)=1
#define EN_5_OFF  PEout(3)=0

#define EN_6_ON   PEout(2)=1
#define EN_6_OFF  PEout(2)=0

#define EN_7_ON   PEout(1)=1
#define EN_7_OFF  PEout(1)=0

#define EN_8_ON   PEout(0)=1
#define EN_8_OFF  PEout(0)=0

/*++++++++++++++++++++++++++++++++++++++++++++++++*/
#define DG408_A0_H PAout(12)=1
#define DG408_A0_L PAout(12)=0

#define DG408_A1_H PDout(0)=1
#define DG408_A1_L PDout(0)=0

#define DG408_A2_H PDout(1)=1
#define DG408_A2_L PDout(1)=0

#define DG408_EN_H PAout(11)=1
#define DG408_EN_L PAout(11)=0

/** the macro definition to trigger the led on or off 
  * 1 - off
  *0 - on
  */
#define ON  1
#define OFF 0

#define LED1_PORT	GPIOA
#define LED1_PIN	GPIO_Pin_0
//#define LED1_PORT	GPIOD
//#define LED1_PIN	GPIO_Pin_13

#define LED2_PORT	GPIOA
#define LED2_PIN	GPIO_Pin_0


/* 带参宏，可以像内联函数一样使用 */
#define LED1(a)	if (a)	\
					GPIO_SetBits(LED1_PORT,LED1_PIN);\
					else		\
					GPIO_ResetBits(LED1_PORT,LED1_PIN)

#define LED2(a)	if (a)	\
					GPIO_SetBits(LED2_PORT,LED2_PIN);\
					else		\
					GPIO_ResetBits(LED2_PORT,LED2_PIN)

//#define LED3(a)	if (a)	\
//					GPIO_SetBits(GPIOC,GPIO_Pin_3);\
//					else		\
//					GPIO_ResetBits(GPIOC,GPIO_Pin_3)


/* 直接操作寄存器的方法控制IO */
#define	digitalHi(p,i)			{p->BSRR=i;}			//设置为高电平		
#define digitalLo(p,i)			{p->BRR=i;}				//输出低电平
#define digitalToggle(p,i)		{p->ODR ^=i;}			//输出反转状态


/* 定义控制IO的宏 */
#define LED1_TOGGLE		digitalToggle(LED1_PORT,LED1_PIN)
#define LED1_OFF		digitalHi(LED1_PORT,LED1_PIN)
#define LED1_ON			digitalLo(LED1_PORT,LED1_PIN)

#define LED2_TOGGLE		digitalToggle(LED2_PORT,LED2_PIN)
#define LED2_OFF		digitalHi(LED2_PORT,LED2_PIN)
#define LED2_ON			digitalLo(LED2_PORT,LED2_PIN)

//#define LED3_TOGGLE		digitalToggle(GPIOC,GPIO_Pin_3)
//#define LED3_OFF		digitalHi(GPIOC,GPIO_Pin_3)
//#define LED3_ON			digitalLo(GPIOC,GPIO_Pin_3)

void All_GPIO_Config(void);
void relay_on(void);
void relay_off(void);
void channel_on(void);
void channel_off(void);
void DG408_output(u8 channel);
#endif /* __LED_H */
