#include "app.h"
#include "ad7606.h"
#include "ad5551.h"
#include "database.h"
#include "dac7565.h"
#include "bsp_led.h"
#include "spi_ads1118.h"
#include "m_usart3.h"
#include "bsp_SysTick.h"

#define DEBUG_PRINT_TEST	1

/*************** Global Variable Area ************/
u16 da7565_study_value = 0;  
extern float ad1118_study_value; //mv
u16 ad5551_study_value = 0;

extern u8 gear_num;

/*+++++++++++++++++++++++++++++++++++++++++++++++*/
static u8 CMP_2_VOL(u16 vol_A,u16 vol_B);



extern Dat_ramDb dat_ram;
extern float screen_set_current;

u8 study_flow = FLOW_1;

enum errStatus { NoInitValue, err0 };


void set_flow(u8 flow_num)
{
	study_flow =flow_num;
}

/*
0-10V   ~ 0-380V
26.3 mV ~ 1V

return :
0: Not get
!0: Get it,study end!
*/
u16 dynamic_study(void) //execute in 100ms
{
	static u16 vol_step=45;
	static u16 da7565_step = 0;
	u16 ad7606_vol_L[8]={0}; //Only use ad7606_vol_L[0]
	float cur_current=0.0;
	
	
	if(screen_set_current >= 1) //sample 10R current
	{
//		AD7606_GetValue_1(ad7606_vol_L,1);
		AD7606_GetValue_1(ad7606_vol_L,8);
		cur_current = ad7606_vol_L[0] / 10; //relay =10R
		dat_ram.write.fe_5014.reg_study_current = (u32)(1000 * cur_current); //store realtime study value
		log_print("10R gear:cur_current=%f mA\n",cur_current);
	}
	else  ////sample 3K current
	{
		AD7606_GetValue_2(ad7606_vol_L,8);
		// /10000; here I don't divided 1W, bacause end will be 0
		cur_current = ad7606_vol_L[0] / 3; //3K
		dat_ram.write.fe_5014.reg_study_current = (u32)(1000 * cur_current); //store realtime study value
		log_print("3K gear:cur_current=%f uA\n",cur_current);
	}
	
	switch(study_flow)
	{
		case FLOW_1:
		{
			log_print("FLOW_1 enter!\n");
			if(cur_current < screen_set_current) //add one by one
			{
				ad5551_write(vol_step);
				log_print("ad5551->vol_step=%d\n",vol_step);
				if(vol_step<16298) //<=16384
				{
					vol_step+=45;
				}
				else
				{
					log_print("FLOW1:ad5551 output overlimit!\n");
#if DEBUG_PRINT_TEST==1
					study_flow = FLOW_IDLE;
#endif
				}
			}
			else //FLOW_1 finished
			{
				ad5551_study_value = vol_step+225; //record ad5551 value
				ad5551_write(vol_step+135); //add 3V
				vol_step = 45; //deinit this value
				dac7565_write_1(dac_7565_channal_1, 0); //turn off audion
				study_flow = FLOW_2;
				log_print("+++++++++++Switch to FLOW_2 Mode ++++++++!\n");
			}
			break;
		}
		
		case FLOW_2:
		{
			log_print("FLOW_2 enter!\n");
			if(cur_current < screen_set_current)
			{
				/* add one by one：*/
				dac7565_write_1(dac_7565_channal_1, da7565_step);
				if(da7565_step <=4093)  //overflow
				{
					da7565_step+=2;
				}
				else //if(cur_current >= screen_set_current)
				{
					log_print("FLOW2:da7565 output overlimit!\n");
#if DEBUG_PRINT_TEST==1
					study_flow = FLOW_IDLE;
#endif
				}
			}
			else //Got it!
			{
				da7565_study_value = da7565_step; //save study da7565 value
				da7565_step = 0; //clear audion num
				
#if DEBUG_PRINT_TEST==1
				log_print("da7565_study_value=%d\n",da7565_study_value);
				log_print("+++++++ study success!!! +++++++++\n");
#endif
				/* turn off external power */
//				ad5551_write(0); 
//				dac7565_write_1(dac_7565_channal_1, 0);
				delay_ms(500);
				study_flow = FLOW_IDLE;
			}
			break;
		}
/*======================= 10K gear ===========================*/
		case FLOW_1_a:
		{
			log_print("FLOW_1_a enter!\n");
			if(cur_current < (screen_set_current*1000)) //add one by one
			{
				if(vol_step == 45) //避开 static u16 vol_step=45;
				{
					vol_step = 5;
				}
				ad5551_write(vol_step);
				log_print("ad5551 vol_step=%d\n",vol_step);
				if(vol_step<16298) //<=16384
				{
					vol_step+=9; //5 is 0.11V
				}
				else
				{
					log_print("FLOW1:ad5551 output overlimit!\n");
#if DEBUG_PRINT_TEST==1
					
#endif
					study_flow = FLOW_IDLE;
				}
			}
			else if(cur_current >= (screen_set_current*1000)) //FLOW_1 finished
			{
				log_print("FLOW_1_a finished!\n");
				ad5551_study_value = vol_step+27; //record ad5551 value
				ad5551_write(vol_step+27); //add 3V
				vol_step = 45;  //deinit this value
				dac7565_write_1(dac_7565_channal_1, 0); //turn off audion
				study_flow = FLOW_2_b;
			}
			break;
		}
		
		case FLOW_2_b:  // gear = 10K
		{
			log_print("FLOW_2_b enter!\n");
			if(cur_current < (screen_set_current * 1000))
			{
				/* add one by one：*/
				dac7565_write_1(dac_7565_channal_1, da7565_step);
				if(da7565_step <=4093)
				{
					da7565_step+=10;// original=2
				}
				else
				{
#if DEBUG_PRINT_TEST==1
					log_print("FLOW2:da7565 output overlimit!\n");
#endif
					study_flow = FLOW_IDLE;
				}
			}
			else //Got it!
			{
				delay_ms(500);
				da7565_study_value = da7565_step; //save study da7565 value
#if DEBUG_PRINT_TEST==1
				log_print("da7565_study_value=%d\n",da7565_study_value);
//				log_print("FLOW_2_b -> ad1118_study_value=%f mv\n",ad1118_study_value);
				log_print("study success!!!\n");
#endif
				study_flow = FLOW_IDLE;
			}
			break;
		}
		default: break;
	}
	return study_flow;
}

/*
	when receive screen begin button.
	start run compare.
	called only after study.
*/
u8 run_led_compare(void)
{
	u16 ad7606_buf_L[8];
	float cur_1118_vol;
	u8 cur_reg = 0;
	
	if(da7565_study_value ==0 || ad5551_study_value ==0)
	{
		log_print("Begin err,bacause NoInitValue\n");
		return NoInitValue; //error
	}
	
	/* turn on 8 channel */

	if(get_gear_num() == GEAR_10R)
	{
		relay_on();
	}
	else if(get_gear_num() == GEAR_3K)
	{
		relay_off();
	}
	else
	{
		log_print("Begin err,bacause gear=none\n");
		return 0;
	}
	
	channel_on();
	delay_ms(10);
//	ad5551_write(ad5551_study_value);//turn on external big power
	
	switch(dat_ram.write.fe_5014.reg_cmp_channel_num)
	{
		case 0:
		{
			log_print("+++ Warning: reg_cmp_channel_num=0!\n");
			break;
		}
		case 1:
		{
			dac7565_write_1(dac_7565_channal_1, da7565_study_value);
			log_print("reg_cmp_channel_num=1\n");
			break;
		}
		case 2:
		{
			dac7565_write_1(dac_7565_channal_1, da7565_study_value);
			dac7565_write_1(dac_7565_channal_2, da7565_study_value);
			log_print("reg_cmp_channel_num=2\n");
			break;
		}
		case 3:
		{
			dac7565_write_1(dac_7565_channal_1, da7565_study_value);
			dac7565_write_1(dac_7565_channal_2, da7565_study_value);
			dac7565_write_1(dac_7565_channal_3, da7565_study_value);
			log_print("reg_cmp_channel_num=3\n");
			break;
		}
		case 4:
		{
			dac7565_write_1(dac_7565_channal_1, da7565_study_value);
			dac7565_write_1(dac_7565_channal_2, da7565_study_value);
			dac7565_write_1(dac_7565_channal_3, da7565_study_value);
			dac7565_write_1(dac_7565_channal_4, da7565_study_value);
			log_print("reg_cmp_channel_num=4\n");
			break;
		}
		case 5:
		{
			dac7565_write_1(dac_7565_channal_1, da7565_study_value);
			dac7565_write_1(dac_7565_channal_2, da7565_study_value);
			dac7565_write_1(dac_7565_channal_3, da7565_study_value);
			dac7565_write_1(dac_7565_channal_4, da7565_study_value);
			dac7565_write_2(dac_7565_channal_1, da7565_study_value);
			log_print("reg_cmp_channel_num=5\n");
			break;
		}
		case 6:
		{
			dac7565_write_1(dac_7565_channal_1, da7565_study_value);
			dac7565_write_1(dac_7565_channal_2, da7565_study_value);
			dac7565_write_1(dac_7565_channal_3, da7565_study_value);
			dac7565_write_1(dac_7565_channal_4, da7565_study_value);
			dac7565_write_2(dac_7565_channal_1, da7565_study_value);
			dac7565_write_2(dac_7565_channal_2, da7565_study_value);
			log_print("reg_cmp_channel_num=6\n");
			break;
		}
		case 7:
		{
			dac7565_write_1(dac_7565_channal_1, da7565_study_value);
			dac7565_write_1(dac_7565_channal_2, da7565_study_value);
			dac7565_write_1(dac_7565_channal_3, da7565_study_value);
			dac7565_write_1(dac_7565_channal_4, da7565_study_value);
			dac7565_write_2(dac_7565_channal_1, da7565_study_value);
			dac7565_write_2(dac_7565_channal_2, da7565_study_value);
			dac7565_write_2(dac_7565_channal_3, da7565_study_value);
			log_print("reg_cmp_channel_num=7\n");
			break;
		}
		case 8:
		{
			dac7565_write_1(dac_7565_channal_1, da7565_study_value);
			dac7565_write_1(dac_7565_channal_2, da7565_study_value);
			dac7565_write_1(dac_7565_channal_3, da7565_study_value);
			dac7565_write_1(dac_7565_channal_4, da7565_study_value);
			dac7565_write_2(dac_7565_channal_1, da7565_study_value);
			dac7565_write_2(dac_7565_channal_2, da7565_study_value);
			dac7565_write_2(dac_7565_channal_3, da7565_study_value);
			dac7565_write_2(dac_7565_channal_4, da7565_study_value);
			log_print("reg_cmp_channel_num=8\n");
			break;
		}
		default:
		{
			log_print("+++ reg_cmp_channel_num err!\n");
			break;
		}
		
	}
	
	/* Read sample value */
	
/*
	cur_1118_vol = ADS1118_Get_value(1);
	dat_ram.write.fe_5014.reg_cmp_voltage_1 = (u16)cur_1118_vol * 0.125;
	cur_reg = CMP_2_VOL(dat_ram.write.fe_5014.reg_cmp_voltage_1,dat_ram.write.fe_5014.reg_study_voltage);
	if(cur_reg)
	{
		dat_ram.write.fe_5014.reg_cmp_status_1 = 0; //CMP OK
		log_print("channal 1 CMP OK\n");
	}
	else
	{
		dat_ram.write.fe_5014.reg_cmp_status_1 = 1; //CMP VOL different
		log_print("channal 1 CMP different\n");
	}
	delay_ms(100);
	
	cur_1118_vol = ADS1118_Get_value(2);
	dat_ram.write.fe_5014.reg_cmp_voltage_2 = (u16)cur_1118_vol * 0.125;
	cur_reg = CMP_2_VOL(dat_ram.write.fe_5014.reg_cmp_voltage_2,dat_ram.write.fe_5014.reg_study_voltage);
	if(cur_reg)
	{
		dat_ram.write.fe_5014.reg_cmp_status_2 = 0; //CMP OK
		log_print("channal 2 CMP OK\n");
	}
	else
	{
		dat_ram.write.fe_5014.reg_cmp_status_2 = 1; //CMP VOL different
		log_print("channal 2 CMP different\n");
	}
	
	cur_1118_vol = ADS1118_Get_value(3);
	dat_ram.write.fe_5014.reg_cmp_voltage_3 = (u16)cur_1118_vol * 0.125;
	cur_reg = CMP_2_VOL(dat_ram.write.fe_5014.reg_cmp_voltage_3,dat_ram.write.fe_5014.reg_study_voltage);
	if(cur_reg)
		dat_ram.write.fe_5014.reg_cmp_status_3 = 0; //CMP OK
	else
		dat_ram.write.fe_5014.reg_cmp_status_3 = 1; //CMP VOL different
	
	cur_1118_vol = ADS1118_Get_value(4);
	dat_ram.write.fe_5014.reg_cmp_voltage_4 = (u16)cur_1118_vol * 0.125;
	cur_reg = CMP_2_VOL(dat_ram.write.fe_5014.reg_cmp_voltage_4,dat_ram.write.fe_5014.reg_study_voltage);
	if(cur_reg)
		dat_ram.write.fe_5014.reg_cmp_status_4 = 0; //CMP OK
	else
		dat_ram.write.fe_5014.reg_cmp_status_4 = 1; //CMP VOL different
	
	cur_1118_vol = ADS1118_Get_value(5);
	dat_ram.write.fe_5014.reg_cmp_voltage_5 = (u16)cur_1118_vol * 0.125;
	cur_reg = CMP_2_VOL(dat_ram.write.fe_5014.reg_cmp_voltage_5,dat_ram.write.fe_5014.reg_study_voltage);
	if(cur_reg)
		dat_ram.write.fe_5014.reg_cmp_status_5 = 0; //CMP OK
	else
		dat_ram.write.fe_5014.reg_cmp_status_5 = 1; //CMP VOL different
	
	cur_1118_vol = ADS1118_Get_value(6);
	dat_ram.write.fe_5014.reg_cmp_voltage_6 = (u16)cur_1118_vol * 0.125;
	cur_reg = CMP_2_VOL(dat_ram.write.fe_5014.reg_cmp_voltage_6,dat_ram.write.fe_5014.reg_study_voltage);
	if(cur_reg)
		dat_ram.write.fe_5014.reg_cmp_status_6 = 0; //CMP OK
	else
		dat_ram.write.fe_5014.reg_cmp_status_6 = 1; //CMP VOL different
	
	cur_1118_vol = ADS1118_Get_value(7);
	dat_ram.write.fe_5014.reg_cmp_voltage_7 = (u16)cur_1118_vol * 0.125;
	cur_reg = CMP_2_VOL(dat_ram.write.fe_5014.reg_cmp_voltage_7,dat_ram.write.fe_5014.reg_study_voltage);
	if(cur_reg)
		dat_ram.write.fe_5014.reg_cmp_status_7 = 0; //CMP OK
	else
		dat_ram.write.fe_5014.reg_cmp_status_7 = 1; //CMP VOL different
	
	cur_1118_vol = ADS1118_Get_value(8);
	dat_ram.write.fe_5014.reg_cmp_voltage_8 = (u16)cur_1118_vol * 0.125;
	cur_reg = CMP_2_VOL(dat_ram.write.fe_5014.reg_cmp_voltage_8,dat_ram.write.fe_5014.reg_study_voltage);
	if(cur_reg)
		dat_ram.write.fe_5014.reg_cmp_status_8 = 0; //CMP OK
	else
		dat_ram.write.fe_5014.reg_cmp_status_8 = 1; //CMP VOL different
*/
	
	return 1;
}

void ad1118_read_all_channel()
{
	float cur_1118_vol;
	
	switch(dat_ram.write.fe_5014.reg_cmp_channel_num)
	{
		case 0:
		{
			log_print("+++ Warning: reg_cmp_channel_num=0!\n");
			break;
		}
		case 1:
		{
			cur_1118_vol = ADS1118_Get_value(1);
			dat_ram.write.fe_5014.reg_cmp_voltage_1 = (u16)cur_1118_vol * 0.125;
			
			dat_ram.write.fe_5014.reg_cmp_voltage_2 = 0;
			dat_ram.write.fe_5014.reg_cmp_voltage_3 = 0;
			dat_ram.write.fe_5014.reg_cmp_voltage_4 = 0;
			dat_ram.write.fe_5014.reg_cmp_voltage_5 = 0;
			dat_ram.write.fe_5014.reg_cmp_voltage_6 = 0;
			dat_ram.write.fe_5014.reg_cmp_voltage_7 = 0;
			dat_ram.write.fe_5014.reg_cmp_voltage_8 = 0;
			break;
		}
		case 2:
		{
			cur_1118_vol = ADS1118_Get_value(1);
			dat_ram.write.fe_5014.reg_cmp_voltage_1 = (u16)cur_1118_vol * 0.125;
			cur_1118_vol = ADS1118_Get_value(2);
			dat_ram.write.fe_5014.reg_cmp_voltage_2 = (u16)cur_1118_vol * 0.125;
			
			dat_ram.write.fe_5014.reg_cmp_voltage_3 = 0;
			dat_ram.write.fe_5014.reg_cmp_voltage_4 = 0;
			dat_ram.write.fe_5014.reg_cmp_voltage_5 = 0;
			dat_ram.write.fe_5014.reg_cmp_voltage_6 = 0;
			dat_ram.write.fe_5014.reg_cmp_voltage_7 = 0;
			dat_ram.write.fe_5014.reg_cmp_voltage_8 = 0;
			break;
		}
		case 3:
		{
			cur_1118_vol = ADS1118_Get_value(1);
			dat_ram.write.fe_5014.reg_cmp_voltage_1 = (u16)cur_1118_vol * 0.125;
			cur_1118_vol = ADS1118_Get_value(2);
			dat_ram.write.fe_5014.reg_cmp_voltage_2 = (u16)cur_1118_vol * 0.125;
			cur_1118_vol = ADS1118_Get_value(3);
			dat_ram.write.fe_5014.reg_cmp_voltage_3 = (u16)cur_1118_vol * 0.125;
			
			dat_ram.write.fe_5014.reg_cmp_voltage_4 = 0;
			dat_ram.write.fe_5014.reg_cmp_voltage_5 = 0;
			dat_ram.write.fe_5014.reg_cmp_voltage_6 = 0;
			dat_ram.write.fe_5014.reg_cmp_voltage_7 = 0;
			dat_ram.write.fe_5014.reg_cmp_voltage_8 = 0;
			
			break;
		}
		case 4:
		{
			cur_1118_vol = ADS1118_Get_value(1);
			dat_ram.write.fe_5014.reg_cmp_voltage_1 = (u16)cur_1118_vol * 0.125;
			cur_1118_vol = ADS1118_Get_value(2);
			dat_ram.write.fe_5014.reg_cmp_voltage_2 = (u16)cur_1118_vol * 0.125;
			cur_1118_vol = ADS1118_Get_value(3);
			dat_ram.write.fe_5014.reg_cmp_voltage_3 = (u16)cur_1118_vol * 0.125;
			cur_1118_vol = ADS1118_Get_value(4);
			dat_ram.write.fe_5014.reg_cmp_voltage_4 = (u16)cur_1118_vol * 0.125;

			dat_ram.write.fe_5014.reg_cmp_voltage_5 = 0;
			dat_ram.write.fe_5014.reg_cmp_voltage_6 = 0;
			dat_ram.write.fe_5014.reg_cmp_voltage_7 = 0;
			dat_ram.write.fe_5014.reg_cmp_voltage_8 = 0;
			break;
		}
		case 5:
		{
			cur_1118_vol = ADS1118_Get_value(1);
			dat_ram.write.fe_5014.reg_cmp_voltage_1 = (u16)cur_1118_vol * 0.125;
			cur_1118_vol = ADS1118_Get_value(2);
			dat_ram.write.fe_5014.reg_cmp_voltage_2 = (u16)cur_1118_vol * 0.125;
			cur_1118_vol = ADS1118_Get_value(3);
			dat_ram.write.fe_5014.reg_cmp_voltage_3 = (u16)cur_1118_vol * 0.125;
			cur_1118_vol = ADS1118_Get_value(4);
			dat_ram.write.fe_5014.reg_cmp_voltage_4 = (u16)cur_1118_vol * 0.125;
			cur_1118_vol = ADS1118_Get_value(5);
			dat_ram.write.fe_5014.reg_cmp_voltage_5 = (u16)cur_1118_vol * 0.125;
			
			dat_ram.write.fe_5014.reg_cmp_voltage_6 = 0;
			dat_ram.write.fe_5014.reg_cmp_voltage_7 = 0;
			dat_ram.write.fe_5014.reg_cmp_voltage_8 = 0;
			break;
		}
		case 6:
		{
			cur_1118_vol = ADS1118_Get_value(1);
			dat_ram.write.fe_5014.reg_cmp_voltage_1 = (u16)cur_1118_vol * 0.125;
			cur_1118_vol = ADS1118_Get_value(2);
			dat_ram.write.fe_5014.reg_cmp_voltage_2 = (u16)cur_1118_vol * 0.125;
			cur_1118_vol = ADS1118_Get_value(3);
			dat_ram.write.fe_5014.reg_cmp_voltage_3 = (u16)cur_1118_vol * 0.125;
			cur_1118_vol = ADS1118_Get_value(4);
			dat_ram.write.fe_5014.reg_cmp_voltage_4 = (u16)cur_1118_vol * 0.125;
			cur_1118_vol = ADS1118_Get_value(5);
			dat_ram.write.fe_5014.reg_cmp_voltage_5 = (u16)cur_1118_vol * 0.125;
			cur_1118_vol = ADS1118_Get_value(6);
			dat_ram.write.fe_5014.reg_cmp_voltage_6 = (u16)cur_1118_vol * 0.125;
			
			dat_ram.write.fe_5014.reg_cmp_voltage_7 = 0;
			dat_ram.write.fe_5014.reg_cmp_voltage_8 = 0;
			break;
		}
		case 7:
		{
			cur_1118_vol = ADS1118_Get_value(1);
			dat_ram.write.fe_5014.reg_cmp_voltage_1 = (u16)cur_1118_vol * 0.125;
			cur_1118_vol = ADS1118_Get_value(2);
			dat_ram.write.fe_5014.reg_cmp_voltage_2 = (u16)cur_1118_vol * 0.125;
			cur_1118_vol = ADS1118_Get_value(3);
			dat_ram.write.fe_5014.reg_cmp_voltage_3 = (u16)cur_1118_vol * 0.125;
			cur_1118_vol = ADS1118_Get_value(4);
			dat_ram.write.fe_5014.reg_cmp_voltage_4 = (u16)cur_1118_vol * 0.125;
			cur_1118_vol = ADS1118_Get_value(5);
			dat_ram.write.fe_5014.reg_cmp_voltage_5 = (u16)cur_1118_vol * 0.125;
			cur_1118_vol = ADS1118_Get_value(6);
			dat_ram.write.fe_5014.reg_cmp_voltage_6 = (u16)cur_1118_vol * 0.125;
			cur_1118_vol = ADS1118_Get_value(7);
			dat_ram.write.fe_5014.reg_cmp_voltage_7 = (u16)cur_1118_vol * 0.125;
			
			dat_ram.write.fe_5014.reg_cmp_voltage_8 = 0;
			break;
		}
		case 8:
		{
			cur_1118_vol = ADS1118_Get_value(1);
			dat_ram.write.fe_5014.reg_cmp_voltage_1 = (u16)cur_1118_vol * 0.125;
			cur_1118_vol = ADS1118_Get_value(2);
			dat_ram.write.fe_5014.reg_cmp_voltage_2 = (u16)cur_1118_vol * 0.125;
			cur_1118_vol = ADS1118_Get_value(3);
			dat_ram.write.fe_5014.reg_cmp_voltage_3 = (u16)cur_1118_vol * 0.125;
			cur_1118_vol = ADS1118_Get_value(4);
			dat_ram.write.fe_5014.reg_cmp_voltage_4 = (u16)cur_1118_vol * 0.125;
			cur_1118_vol = ADS1118_Get_value(5);
			dat_ram.write.fe_5014.reg_cmp_voltage_5 = (u16)cur_1118_vol * 0.125;
			cur_1118_vol = ADS1118_Get_value(6);
			dat_ram.write.fe_5014.reg_cmp_voltage_6 = (u16)cur_1118_vol * 0.125;
			cur_1118_vol = ADS1118_Get_value(7);
			dat_ram.write.fe_5014.reg_cmp_voltage_7 = (u16)cur_1118_vol * 0.125;
			cur_1118_vol = ADS1118_Get_value(8);
			dat_ram.write.fe_5014.reg_cmp_voltage_8 = (u16)cur_1118_vol * 0.125;
			break;
		}
		default:
		{
			log_print("+++ reg_cmp_channel_num err!\n");
			break;
		}
	}
}

void read_all_channel_current(u8 channel_)
{
	u16 ad7606_vol_buf[8]={0};
	float cur_current;
	
	switch(channel_)
	{
		case GEAR_10R:
		{
			AD7606_GetValue_1(ad7606_vol_buf,8);
			
			switch(dat_ram.write.fe_5014.reg_cmp_channel_num)
			{
				case 0:
				{
					log_print("+++ Warning: reg_cmp_channel_num=0!\n");
					break;
				}
				case 1:
				{
					cur_current = ad7606_vol_buf[0] * 100; //relay =10R
					dat_ram.write.fe_5014.reg_cmp_current_1 = cur_current;
					
					dat_ram.write.fe_5014.reg_cmp_current_2 = 0;
					dat_ram.write.fe_5014.reg_cmp_current_3 = 0;
					dat_ram.write.fe_5014.reg_cmp_current_4 = 0;
					dat_ram.write.fe_5014.reg_cmp_current_5 = 0;
					dat_ram.write.fe_5014.reg_cmp_current_6 = 0;
					dat_ram.write.fe_5014.reg_cmp_current_7 = 0;
					dat_ram.write.fe_5014.reg_cmp_current_8 = 0;
					break;
				}
				case 2:
				{
					cur_current = ad7606_vol_buf[0] * 100; //relay =10R
					dat_ram.write.fe_5014.reg_cmp_current_1 = cur_current;
					
					cur_current = ad7606_vol_buf[1] * 100; //relay =10R
					dat_ram.write.fe_5014.reg_cmp_current_2 = cur_current;
					
					dat_ram.write.fe_5014.reg_cmp_current_3 = 0;
					dat_ram.write.fe_5014.reg_cmp_current_4 = 0;
					dat_ram.write.fe_5014.reg_cmp_current_5 = 0;
					dat_ram.write.fe_5014.reg_cmp_current_6 = 0;
					dat_ram.write.fe_5014.reg_cmp_current_7 = 0;
					dat_ram.write.fe_5014.reg_cmp_current_8 = 0;
					break;
				}
				case 3:
				{
					cur_current = ad7606_vol_buf[0] * 100; //relay =10R
					dat_ram.write.fe_5014.reg_cmp_current_1 = cur_current;
					
					cur_current = ad7606_vol_buf[1] * 100; //relay =10R
					dat_ram.write.fe_5014.reg_cmp_current_2 = cur_current;
					
					cur_current = ad7606_vol_buf[2] * 100; //relay =10R
					dat_ram.write.fe_5014.reg_cmp_current_3 = cur_current;
					
					dat_ram.write.fe_5014.reg_cmp_current_4 = 0;
					dat_ram.write.fe_5014.reg_cmp_current_5 = 0;
					dat_ram.write.fe_5014.reg_cmp_current_6 = 0;
					dat_ram.write.fe_5014.reg_cmp_current_7 = 0;
					dat_ram.write.fe_5014.reg_cmp_current_8 = 0;
					break;
				}
				case 4:
				{
					cur_current = ad7606_vol_buf[0] * 100; //relay =10R
					dat_ram.write.fe_5014.reg_cmp_current_1 = cur_current;
					
					cur_current = ad7606_vol_buf[1] * 100; //relay =10R
					dat_ram.write.fe_5014.reg_cmp_current_2 = cur_current;
					
					cur_current = ad7606_vol_buf[2] * 100; //relay =10R
					dat_ram.write.fe_5014.reg_cmp_current_3 = cur_current;
					
					cur_current = ad7606_vol_buf[3] * 100; //relay =10R
					dat_ram.write.fe_5014.reg_cmp_current_4 = cur_current;
					
					dat_ram.write.fe_5014.reg_cmp_current_5 = 0;
					dat_ram.write.fe_5014.reg_cmp_current_6 = 0;
					dat_ram.write.fe_5014.reg_cmp_current_7 = 0;
					dat_ram.write.fe_5014.reg_cmp_current_8 = 0;
					break;
				}
				case 5:
				{
					cur_current = ad7606_vol_buf[0] * 100; //relay =10R
					dat_ram.write.fe_5014.reg_cmp_current_1 = cur_current;
					
					cur_current = ad7606_vol_buf[1] * 100; //relay =10R
					dat_ram.write.fe_5014.reg_cmp_current_2 = cur_current;
					
					cur_current = ad7606_vol_buf[2] * 100; //relay =10R
					dat_ram.write.fe_5014.reg_cmp_current_3 = cur_current;
					
					cur_current = ad7606_vol_buf[3] * 100; //relay =10R
					dat_ram.write.fe_5014.reg_cmp_current_4 = cur_current;
					
					cur_current = ad7606_vol_buf[4] * 100; //relay =10R
					dat_ram.write.fe_5014.reg_cmp_current_5 = cur_current;
					
					dat_ram.write.fe_5014.reg_cmp_current_6 = 0;
					dat_ram.write.fe_5014.reg_cmp_current_7 = 0;
					dat_ram.write.fe_5014.reg_cmp_current_8 = 0;
					break;
				}
				case 6:
				{
					cur_current = ad7606_vol_buf[0] * 100; //relay =10R
					dat_ram.write.fe_5014.reg_cmp_current_1 = cur_current;
					
					cur_current = ad7606_vol_buf[1] * 100; //relay =10R
					dat_ram.write.fe_5014.reg_cmp_current_2 = cur_current;
					
					cur_current = ad7606_vol_buf[2] * 100; //relay =10R
					dat_ram.write.fe_5014.reg_cmp_current_3 = cur_current;
					
					cur_current = ad7606_vol_buf[3] * 100; //relay =10R
					dat_ram.write.fe_5014.reg_cmp_current_4 = cur_current;
					
					cur_current = ad7606_vol_buf[4] * 100; //relay =10R
					dat_ram.write.fe_5014.reg_cmp_current_5 = cur_current;
					
					cur_current = ad7606_vol_buf[5] * 100; //relay =10R
					dat_ram.write.fe_5014.reg_cmp_current_6 = cur_current;
					
					dat_ram.write.fe_5014.reg_cmp_current_7 = 0;
					dat_ram.write.fe_5014.reg_cmp_current_8 = 0;
					break;
				}
				case 7:
				{
					cur_current = ad7606_vol_buf[0] * 100; //relay =10R
					dat_ram.write.fe_5014.reg_cmp_current_1 = cur_current;
					
					cur_current = ad7606_vol_buf[1] * 100; //relay =10R
					dat_ram.write.fe_5014.reg_cmp_current_2 = cur_current;
					
					cur_current = ad7606_vol_buf[2] * 100; //relay =10R
					dat_ram.write.fe_5014.reg_cmp_current_3 = cur_current;
					
					cur_current = ad7606_vol_buf[3] * 100; //relay =10R
					dat_ram.write.fe_5014.reg_cmp_current_4 = cur_current;
					
					cur_current = ad7606_vol_buf[4] * 100; //relay =10R
					dat_ram.write.fe_5014.reg_cmp_current_5 = cur_current;
					
					cur_current = ad7606_vol_buf[5] * 100; //relay =10R
					dat_ram.write.fe_5014.reg_cmp_current_6 = cur_current;
					cur_current = ad7606_vol_buf[6] * 100; //relay =10R
					dat_ram.write.fe_5014.reg_cmp_current_7 = cur_current;
					
					dat_ram.write.fe_5014.reg_cmp_current_8 = 0;
					break;
				}
				case 8:
				{
					cur_current = ad7606_vol_buf[0] * 100; //relay =10R
					dat_ram.write.fe_5014.reg_cmp_current_1 = cur_current;
					
					cur_current = ad7606_vol_buf[1] * 100; //relay =10R
					dat_ram.write.fe_5014.reg_cmp_current_2 = cur_current;
					
					cur_current = ad7606_vol_buf[2] * 100; //relay =10R
					dat_ram.write.fe_5014.reg_cmp_current_3 = cur_current;
					
					cur_current = ad7606_vol_buf[3] * 100; //relay =10R
					dat_ram.write.fe_5014.reg_cmp_current_4 = cur_current;
					
					cur_current = ad7606_vol_buf[4] * 100; //relay =10R
					dat_ram.write.fe_5014.reg_cmp_current_5 = cur_current;
					
					cur_current = ad7606_vol_buf[5] * 100; //relay =10R
					dat_ram.write.fe_5014.reg_cmp_current_6 = cur_current;
					
					cur_current = ad7606_vol_buf[6] * 100; //relay =10R
					dat_ram.write.fe_5014.reg_cmp_current_7 = cur_current;
					
					cur_current = ad7606_vol_buf[7] * 100; //relay =10R
					dat_ram.write.fe_5014.reg_cmp_current_8 = cur_current;
					break;
				}
				default:
				{
					log_print("+++ reg_cmp_channel_num err!\n");
					break;
				}
				
			}
			break;
		}
		
		case GEAR_3K:
		{
			AD7606_GetValue_2(ad7606_vol_buf,8);
			
			switch(dat_ram.write.fe_5014.reg_cmp_channel_num)
			{
				case 0:
				{
					log_print("+++ Warning: reg_cmp_channel_num=0!\n");
					break;
				}
				case 1:
				{
					cur_current = ad7606_vol_buf[0] / 3; //ad7606_vol_buf[0] / 3K * 1000
					dat_ram.write.fe_5014.reg_cmp_current_1 = cur_current;
					
					dat_ram.write.fe_5014.reg_cmp_current_2 = 0;
					dat_ram.write.fe_5014.reg_cmp_current_3 = 0;
					dat_ram.write.fe_5014.reg_cmp_current_4 = 0;
					dat_ram.write.fe_5014.reg_cmp_current_5 = 0;
					dat_ram.write.fe_5014.reg_cmp_current_6 = 0;
					dat_ram.write.fe_5014.reg_cmp_current_7 = 0;
					dat_ram.write.fe_5014.reg_cmp_current_8 = 0;
					break;
				}
				case 2:
				{
					cur_current = ad7606_vol_buf[0] / 3; //ad7606_vol_buf[0] / 3K * 1000
					dat_ram.write.fe_5014.reg_cmp_current_1 = cur_current;
					
					cur_current = ad7606_vol_buf[1] / 3; //relay = 3K
					dat_ram.write.fe_5014.reg_cmp_current_2 = cur_current;
					
					dat_ram.write.fe_5014.reg_cmp_current_3 = 0;
					dat_ram.write.fe_5014.reg_cmp_current_4 = 0;
					dat_ram.write.fe_5014.reg_cmp_current_5 = 0;
					dat_ram.write.fe_5014.reg_cmp_current_6 = 0;
					dat_ram.write.fe_5014.reg_cmp_current_7 = 0;
					dat_ram.write.fe_5014.reg_cmp_current_8 = 0;
					break;
				}
				case 3:
				{
					cur_current = ad7606_vol_buf[0] / 3; //ad7606_vol_buf[0] / 3K * 1000
					dat_ram.write.fe_5014.reg_cmp_current_1 = cur_current;
					
					cur_current = ad7606_vol_buf[1] / 3; //relay = 3K
					dat_ram.write.fe_5014.reg_cmp_current_2 = cur_current;
					
					cur_current = ad7606_vol_buf[2] / 3; //relay = 3K
					dat_ram.write.fe_5014.reg_cmp_current_3 = cur_current;
					
					dat_ram.write.fe_5014.reg_cmp_current_4 = 0;
					dat_ram.write.fe_5014.reg_cmp_current_5 = 0;
					dat_ram.write.fe_5014.reg_cmp_current_6 = 0;
					dat_ram.write.fe_5014.reg_cmp_current_7 = 0;
					dat_ram.write.fe_5014.reg_cmp_current_8 = 0;
					break;
				}
				case 4:
				{
					cur_current = ad7606_vol_buf[0] / 3; //ad7606_vol_buf[0] / 3K * 1000
					dat_ram.write.fe_5014.reg_cmp_current_1 = cur_current;
					
					cur_current = ad7606_vol_buf[1] / 3; //relay = 3K
					dat_ram.write.fe_5014.reg_cmp_current_2 = cur_current;
					
					cur_current = ad7606_vol_buf[2] / 3; //relay = 3K
					dat_ram.write.fe_5014.reg_cmp_current_3 = cur_current;
					
					cur_current = ad7606_vol_buf[3] / 3; //relay = 3K
					dat_ram.write.fe_5014.reg_cmp_current_4 = cur_current;
					
					cur_current = ad7606_vol_buf[4] / 3; //relay = 3K
					dat_ram.write.fe_5014.reg_cmp_current_5 = cur_current;
					
					dat_ram.write.fe_5014.reg_cmp_current_6 = 0;
					dat_ram.write.fe_5014.reg_cmp_current_7 = 0;
					dat_ram.write.fe_5014.reg_cmp_current_8 = 0;
					break;
				}
				case 5:
				{
					cur_current = ad7606_vol_buf[0] / 3; //ad7606_vol_buf[0] / 3K * 1000
					dat_ram.write.fe_5014.reg_cmp_current_1 = cur_current;
					
					cur_current = ad7606_vol_buf[1] / 3; //relay = 3K
					dat_ram.write.fe_5014.reg_cmp_current_2 = cur_current;
					
					cur_current = ad7606_vol_buf[2] / 3; //relay = 3K
					dat_ram.write.fe_5014.reg_cmp_current_3 = cur_current;
					
					cur_current = ad7606_vol_buf[3] / 3; //relay = 3K
					dat_ram.write.fe_5014.reg_cmp_current_4 = cur_current;
					
					cur_current = ad7606_vol_buf[4] / 3; //relay = 3K
					dat_ram.write.fe_5014.reg_cmp_current_5 = cur_current;
					
					dat_ram.write.fe_5014.reg_cmp_current_6 = 0;
					dat_ram.write.fe_5014.reg_cmp_current_7 = 0;
					dat_ram.write.fe_5014.reg_cmp_current_8 = 0;
					break;
				}
				case 6:
				{
					cur_current = ad7606_vol_buf[0] / 3; //ad7606_vol_buf[0] / 3K * 1000
					dat_ram.write.fe_5014.reg_cmp_current_1 = cur_current;
					
					cur_current = ad7606_vol_buf[1] / 3; //relay = 3K
					dat_ram.write.fe_5014.reg_cmp_current_2 = cur_current;
					
					cur_current = ad7606_vol_buf[2] / 3; //relay = 3K
					dat_ram.write.fe_5014.reg_cmp_current_3 = cur_current;
					
					cur_current = ad7606_vol_buf[3] / 3; //relay = 3K
					dat_ram.write.fe_5014.reg_cmp_current_4 = cur_current;
					
					cur_current = ad7606_vol_buf[4] / 3; //relay = 3K
					dat_ram.write.fe_5014.reg_cmp_current_5 = cur_current;
					
					cur_current = ad7606_vol_buf[5] / 3; //relay = 3K
					dat_ram.write.fe_5014.reg_cmp_current_6 = cur_current;
					
					dat_ram.write.fe_5014.reg_cmp_current_7 = 0;
					dat_ram.write.fe_5014.reg_cmp_current_8 = 0;
					break;
				}
				case 7:
				{
					cur_current = ad7606_vol_buf[0] / 3; //ad7606_vol_buf[0] / 3K * 1000
					dat_ram.write.fe_5014.reg_cmp_current_1 = cur_current;
					
					cur_current = ad7606_vol_buf[1] / 3; //relay = 3K
					dat_ram.write.fe_5014.reg_cmp_current_2 = cur_current;
					
					cur_current = ad7606_vol_buf[2] / 3; //relay = 3K
					dat_ram.write.fe_5014.reg_cmp_current_3 = cur_current;
					
					cur_current = ad7606_vol_buf[3] / 3; //relay = 3K
					dat_ram.write.fe_5014.reg_cmp_current_4 = cur_current;
					
					cur_current = ad7606_vol_buf[4] / 3; //relay = 3K
					dat_ram.write.fe_5014.reg_cmp_current_5 = cur_current;
					
					cur_current = ad7606_vol_buf[5] / 3; //relay = 3K
					dat_ram.write.fe_5014.reg_cmp_current_6 = cur_current;
					
					cur_current = ad7606_vol_buf[6] / 3; //relay = 3K
					dat_ram.write.fe_5014.reg_cmp_current_7 = cur_current;
					
					dat_ram.write.fe_5014.reg_cmp_current_8 = 0;
					break;
				}
				case 8:
				{
					cur_current = ad7606_vol_buf[0] / 3; //ad7606_vol_buf[0] / 3K * 1000
					dat_ram.write.fe_5014.reg_cmp_current_1 = cur_current;
					
					cur_current = ad7606_vol_buf[1] / 3; //relay = 3K
					dat_ram.write.fe_5014.reg_cmp_current_2 = cur_current;
					
					cur_current = ad7606_vol_buf[2] / 3; //relay = 3K
					dat_ram.write.fe_5014.reg_cmp_current_3 = cur_current;
					
					cur_current = ad7606_vol_buf[3] / 3; //relay = 3K
					dat_ram.write.fe_5014.reg_cmp_current_4 = cur_current;
					
					cur_current = ad7606_vol_buf[4] / 3; //relay = 3K
					dat_ram.write.fe_5014.reg_cmp_current_5 = cur_current;
					
					cur_current = ad7606_vol_buf[5] / 3; //relay = 3K
					dat_ram.write.fe_5014.reg_cmp_current_6 = cur_current;
					
					cur_current = ad7606_vol_buf[6] / 3; //relay = 3K
					dat_ram.write.fe_5014.reg_cmp_current_7 = cur_current;
					
					cur_current = ad7606_vol_buf[7] / 3; //relay = 3K
					dat_ram.write.fe_5014.reg_cmp_current_8 = cur_current;
					break;
				}
				default:
				{
					log_print("+++ reg_cmp_channel_num err!\n");
					break;
				}
				
			}
			break;
		}
		
		default:
		{
			log_print("Relay gear num = none, So no current!\n");
			break;
		}
	}
}


void ad1118_clear_all_channel()
{
	dat_ram.write.fe_5014.reg_cmp_voltage_1 = 0;
	dat_ram.write.fe_5014.reg_cmp_voltage_2 = 0;
	dat_ram.write.fe_5014.reg_cmp_voltage_3 = 0;
	dat_ram.write.fe_5014.reg_cmp_voltage_4 = 0;
	dat_ram.write.fe_5014.reg_cmp_voltage_5 = 0;
	dat_ram.write.fe_5014.reg_cmp_voltage_6 = 0;
	dat_ram.write.fe_5014.reg_cmp_voltage_7 = 0;
	dat_ram.write.fe_5014.reg_cmp_voltage_8 = 0;
	
	dat_ram.write.fe_5014.reg_cmp_current_1 = 0;
	dat_ram.write.fe_5014.reg_cmp_current_2 = 0;
	dat_ram.write.fe_5014.reg_cmp_current_3 = 0;
	dat_ram.write.fe_5014.reg_cmp_current_4 = 0;
	dat_ram.write.fe_5014.reg_cmp_current_5 = 0;
	dat_ram.write.fe_5014.reg_cmp_current_6 = 0;
	dat_ram.write.fe_5014.reg_cmp_current_7 = 0;
	dat_ram.write.fe_5014.reg_cmp_current_8 = 0;
}

static u8 CMP_2_VOL(u16 vol_A,u16 vol_B)
{
	if(vol_A > (vol_B+20) || vol_A < (vol_B-20))
	{
		return 0; //cmp err
	}
	return 1; //cmp OK
}

void close_led_compare(void)
{
//	ad5551_write(0); //turn on external power
	channel_off();
	dac7565_write_1(dac_7565_channal_1, 0);
	dac7565_write_1(dac_7565_channal_2, 0);
	dac7565_write_1(dac_7565_channal_3, 0);
	dac7565_write_1(dac_7565_channal_4, 0);
	
	dac7565_write_2(dac_7565_channal_1, 0);
	dac7565_write_2(dac_7565_channal_2, 0);
	dac7565_write_2(dac_7565_channal_3, 0);
	dac7565_write_2(dac_7565_channal_4, 0);
}


/*
	打开RELAY_CH1, EN_1, DAC_CH1(三极管)
Note: Only ues in study mode.
*/
void turn_on_switch(void)
{
	if(dat_ram.write.fe_5014.reg_set_current >= 1000) //扩大了1K倍
	{
		RELAY_CH1_ON;//relay = 10R
		study_flow = FLOW_1; 
		set_gear_num(GEAR_10R);
	}
	else
	{
		RELAY_CH1_OFF;//relay = 3K
		study_flow = FLOW_1_a;
		set_gear_num(GEAR_3K);
	}
	
	EN_2_ON;			  //EN_2 is tied All ENABLE
	RELAY_DISCHARGE = 1;  /* discharging */
}
void turn_off_switch(void)
{
//	RELAY_CH1_OFF;//relay = 3K
	EN_2_OFF;	
	dac7565_write_1(dac_7565_channal_1, 0); //BL_DAC1=0V 关闭三极管
}


/*
0: NONE
1: 10R
2: 3K
*/
u8 get_gear_num(void)
{
	return gear_num;
}

void set_gear_num(u8 Gear_Num)
{
	gear_num = Gear_Num;
}
