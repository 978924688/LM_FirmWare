#ifndef __AD5551_H_
#define __AD5551_H_
#include "sys.h"


//#define AD5551_CS_HIGH		PAout(3)=1
//#define AD5551_CS_LOW		PAout(3)=0

#define AD5551_CS_HIGH		PCout(13)=1
#define AD5551_CS_LOW		PCout(13)=0


#define AD5551_SCK_LOW		PAout(5)=0
#define AD5551_SCK_HIGH		PAout(5)=1


void AD5551_Init(void);
void ad5551_write(u16 _data);







#endif
