/*
                         \\\|///
                       \\  - -  //
                        (  @ @  )
+---------------------oOOo-(_)-oOOo----------------------+
|                                                        |   
|        STM32F103VET6 + AD7606 + DA7565 + AD1118        |    
|                   LED cluster tester                   |
|                     Author:  LiuMing                   |
|                       2019.07.30                       |
|                                                        |
|                  www.thermofisher.com                  |
|				       Version: V3.0                	 |
|                             Oooo                       |
+-----------------------oooO--(   )----------------------+
                       (   )   ) /
                        \ (   (_/
                         \_) 
						 
Resease Log:
2019-07-29:
	1) New add led voltage sample realtime update
	2) New add EE save func
	3) Send Board to Consumer
	4) New Add TIM4 func to provide 1s

2019-7-30:
	1) Add TIM4 control discharge and delete delay(1000) style

2019-8-8
	1) if modify to J-link version, put Target->IROM1->0x8010000 to 0x8000000
		Modify VECT_TAB_OFFSET  0x10000 to 0x00
		
2019-8-12
	1) Add channel select func 
	
2019-8-13
	1) Channel_num para store in EEPROM
*/

#include <string.h>
#include "bsp_SysTick.h"
#include "bsp_led.h"
#include "m_timer.h"
#include "sys.h"
#include "ad7606.h"
#include "dac7565.h"
#include "ad5551.h"
#include "m_usart1.h"
#include "m_usart2.h"
#include "m_usart3.h"
#include "database.h"
#include "bsp_switch_exti.h"
#include "bsp_i2c_ee.h"
#include "spi_ads1118.h"
#include "app.h"


#define DYNAMIC_IDLE  		0
#define DYNAMIC_STUDY  		1
#define DYNAMIC_DISCHARGE   2
#define DISCHARGE_TIME		70  //per 500ms

#define EE_STORE_NUM_BYTES  11

extern Dat_ramDb dat_ram;
extern u16 da7565_study_value;
extern u16 ad5551_study_value;
extern u8 tim4_1s_flag;
extern u8 tim4_10s_discharge_cnt;

u8 gear_num = GEAR_NONE; //record Gear num

//0&1:da7565_study_value 2&3:ad5551_study_value  4:gear_num  5-8:reg_set_current  9&10: reg_study_voltage
u8 eeprom_data[12] = {0};
u16 ad1118_vol[8]={0};
float ad1118_vol_v0[8];

float ad1118_study_value = 0; //test use
float screen_set_current = 0.0;



//u16 ad7606_vol_L[8]={0};
float A0=0;
u16 AD5551_final_value = 0;


int main(void)
{
	u8 run_compare_flag = 1;
	u8 tim4_1s_begin_flag = 0;
	
	u8 dynamic_adjust = DYNAMIC_IDLE;
	SysTick_Init();
	All_GPIO_Config();
	RELAY_DISCHARGE=0;
	TIM2_Configuration(); //led ctr
	TIM3_Configuration();
	TIM4_Configuration();
	AD7606_Init();
	
	DAC_7565_Init();
	uart1_init();
	USART2_Configuration();
	USART3_Configuration();
	mb_resgister_init();
	AD5551_Init();
	ADS1118_Init();
	ad5551_write(0);
	
	if (ee_CheckOk() == 0)
	{
		log_print("Not found EEPROM Device!\r\n");
	}
	
	ee_ReadBytes(eeprom_data,0,(EE_STORE_NUM_BYTES+1));
	da7565_study_value = (eeprom_data[1] << 8)|(eeprom_data[0]);
	ad5551_study_value = (eeprom_data[3] << 8)|(eeprom_data[2]);
	dat_ram.write.fe_5014.reg_set_current =((eeprom_data[8]<<24)|(eeprom_data[7]<<16)|(eeprom_data[6]<<8)|(eeprom_data[5])); //read out 
	dat_ram.write.fe_5014.reg_current_value = dat_ram.write.fe_5014.reg_set_current;
	dat_ram.write.fe_5014.reg_study_voltage = (eeprom_data[10] << 8)|(eeprom_data[9]);
	dat_ram.write.fe_5014.reg_cmp_channel_num = eeprom_data[11]; //channel_num
	
	ad5551_write(ad5551_study_value);//turn on external big power
	log_print("da7565_study_value=%d\n",da7565_study_value);
	log_print("ad5551_study_value=%d\n",ad5551_study_value);
	set_gear_num(eeprom_data[4]);
	if(eeprom_data[0]!=0xff && eeprom_data[1]!=0xff)
	{
		dat_ram.write.fe_5014.reg_study_status = 1;
	}
	log_print("EEPROM electric ON\n");
	log_print("eeprom_data[5]=0x%x,0x%x,0x%x,0x%x,%d\n",eeprom_data[0],eeprom_data[1],eeprom_data[2],eeprom_data[3],eeprom_data[4]);
	memset(eeprom_data,0,sizeof(eeprom_data));
/* Read End */
	
	delay_ms(500);
	
	channel_off();
	close_led_compare(); //close all output
	log_print("booting!\n");
	
	for(;;)
	{
		/* MB rev success */
		if(mb_message_rev_ok())
		{
			MB_execute_cmd();//MB handler
		}
		
#if 1
/*########################### study process #################################*/
		if(dat_ram.write.fe_5014.reg_set_current != dat_ram.write.fe_5014.reg_current_value) //study...
		{
			run_compare_flag = 1;  				//for close "Begin" Mode
			ad5551_write(0);
			close_led_compare();   				//for close "Begin" Mode  When study,other channel is off
			dat_ram.read.fe_5014.reg_begin = 0; //for close "Begin" Mode
			
			log_print("Enter DYNAMIC_DISCHARGE-ing...\n");
			screen_set_current = (float)dat_ram.write.fe_5014.reg_set_current/1000;
			dat_ram.write.fe_5014.reg_current_value = dat_ram.write.fe_5014.reg_set_current;
			dat_ram.write.fe_5014.reg_study_status = 0; //1: study finish  0: studying

			turn_on_switch(); //choose gear
			dynamic_adjust = DYNAMIC_DISCHARGE; //for enter Next if loop
			tim4_10s_discharge_cnt = 0; //for discharge 10s
		}
		if((dynamic_adjust==DYNAMIC_DISCHARGE) && (tim4_10s_discharge_cnt>=DISCHARGE_TIME)) //wait for discharge ending...
		{
			RELAY_DISCHARGE = 0; //stop discharge
			dac7565_write_1(dac_7565_channal_1, 3276); //open audion
			dynamic_adjust = DYNAMIC_STUDY; //for enter Next if loop
		}
		if((dynamic_adjust==DYNAMIC_STUDY) && (tim_100ms_flag>=10)) //100ms
		{
			u8 res=0;
			tim_100ms_flag = 0;
			log_print("Enter DYNAMIC_STUDY\n");
			res = dynamic_study();
			
			if(res == FLOW_IDLE) //Got it? study end!!!
			{
				u16 ad1118_study_value_ = 0;
				dynamic_adjust = DYNAMIC_IDLE;
				dat_ram.write.fe_5014.reg_study_status = 1;//1: study finish 0: studying
				
				/* read LED vol */
				ad1118_study_value_ = ADS1118_Get_value(1); //MUST throw it, dirty dirty dirty!!!
//				log_print("enter for loop\n");
				ad1118_study_value_ = ADS1118_Get_value(1);
				ad1118_study_value = ad1118_study_value_ * 0.125;
				/* Note: ad1118_study_value ¡Â 10 ¾ÍÊÇÆÁÏÔÖµ */
				log_print("FLOW_1->ad1118_study_value=%f mV\n",ad1118_study_value);
				dat_ram.write.fe_5014.reg_study_voltage = (u16)ad1118_study_value;
				
				/* Write data to EEPROM */
				eeprom_data[0] = (u8)(da7565_study_value & 0xff);//LSB before
				eeprom_data[1] = (u8)((da7565_study_value>>8) & 0xff);//LSB before
				eeprom_data[2] = (u8)(ad5551_study_value & 0xff);//LSB before
				eeprom_data[3] = (u8)((ad5551_study_value>>8) & 0xff);//LSB before
				eeprom_data[4] = get_gear_num(); //record relay gear num
				log_print("eeprom_data[4]=0x%x,0x%x,0x%x,0x%x\n",eeprom_data[0],eeprom_data[1],eeprom_data[2],eeprom_data[3]);
				/* store reg_set_current to EE */
				eeprom_data[5] = (u8)(dat_ram.write.fe_5014.reg_set_current & 0xff);
				eeprom_data[6] = (u8)((dat_ram.write.fe_5014.reg_set_current>>8) & 0xff);
				eeprom_data[7] = (u8)((dat_ram.write.fe_5014.reg_set_current>>16) & 0xff);
				eeprom_data[8] = (u8)((dat_ram.write.fe_5014.reg_set_current>>24) & 0xff);
				/* store ADS1118 study value */
				eeprom_data[9] = (u8)(dat_ram.write.fe_5014.reg_study_voltage & 0xff);
				eeprom_data[10] = (u8)((dat_ram.write.fe_5014.reg_study_voltage>>8) & 0xff);
				ee_WriteBytes(eeprom_data,0,EE_STORE_NUM_BYTES);
				/* Write end */
				turn_off_switch(); //when finished study, turn off the whole switch.
			}
		}
		
/*########################### study end #################################*/	

/*########################### run compare process #################################*/
		if(KEY0_READ_STATUS) //external KEY0 start?
		{
			delay_ms(10);
			while(KEY0_READ_STATUS); //wait for loosen hand
			if(!KEY0_READ_STATUS)
			{
				if(dat_ram.read.fe_5014.reg_begin)
				{
					dat_ram.read.fe_5014.reg_begin = 0; //close start
					log_print("press KEY0 close!\n");
				}
				else
				{
					dat_ram.read.fe_5014.reg_begin = 1; //start
					log_print("press KEY0 start!\n");
				}
			}
		}
		
		if(dat_ram.read.fe_5014.reg_begin  && dat_ram.write.fe_5014.reg_study_status) //start
		{ //start
			if(run_compare_flag)
			{
				run_compare_flag = 0;
				run_led_compare();
				tim4_1s_begin_flag = 1;
				log_print("Begin Compare!\n");
			}
		}
		else if(!run_compare_flag) //stop
		{
			//turn off AD5551 & DA7565
			run_compare_flag = 1;
			close_led_compare();
			ad1118_clear_all_channel();
			tim4_1s_begin_flag = 0;
			log_print("Close Compare!\n");
		}
		
		if(tim4_1s_flag && tim4_1s_begin_flag) //when press begin, updating...
		{ //Only for read reg
			tim4_1s_flag = 0;
			ad1118_read_all_channel();
			read_all_channel_current(get_gear_num()); 
		}
		else if(tim4_1s_flag) //for clear vol & current registers.
		{
			tim4_1s_flag = 0;
			ad1118_clear_all_channel();
		}
#endif
	}
}
