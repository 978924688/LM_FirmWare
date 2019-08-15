#ifndef  _BSP_SWITCH_EXTI_H_
#define  _BSP_SWITCH_EXTI_H_
#include "sys.h"

#define KEY0 PAin(15)

void EXTI_Switch_Config(void);
extern unsigned char switch_status;


u8 key_scan();


#endif

