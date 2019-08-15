#ifndef  _DATABASE_H_
#define  _DATABASE_H_
#include "sys.h"

typedef struct Dat_ramDb
{
	/* out registers addr:0-255 */
    union
    {
        u16 registers[30];
        u8  bytes[30 * sizeof(u16)];
		
		/* addr: 0-511 */
        struct
        {
			u16   reg_study_status;  //0: [1: study finish 0: studying]
			u16   reg_study_voltage; //1
			u32	  reg_current_value; //2 与设定电流不一致，开始学习(存的是上一次的屏设电流)
			u32   reg_set_current;   //4 被扩大了1000倍 (当前下发的电流)
			u32   reg_study_current; //6 学习到的电流
			
/********** press BEGIN, 8 channel study value **********/
			u16   reg_cmp_voltage_1;//addr=8
			u16   reg_cmp_voltage_2;//addr=9
			u16   reg_cmp_voltage_3;//addr=10
			u16   reg_cmp_voltage_4;//addr=11
			u16   reg_cmp_voltage_5;//addr=12
			u16   reg_cmp_voltage_6;//addr=13
			u16   reg_cmp_voltage_7;//addr=14
			u16   reg_cmp_voltage_8;//addr=15
			
			u32   reg_cmp_current_1;//addr=16
			u32   reg_cmp_current_2;//addr=18
			u32   reg_cmp_current_3;//addr=20
			u32   reg_cmp_current_4;//addr=22	
			u32   reg_cmp_current_5;//addr=24	
			u32   reg_cmp_current_6;//addr=26
			u32   reg_cmp_current_7;//addr=28
			u32   reg_cmp_current_8;//addr=30
			
			u16   reg_cmp_status_1;//addr=32
			u16   reg_cmp_status_2;//addr=33
			u16   reg_cmp_status_3;//addr=34
			u16   reg_cmp_status_4;//addr=35
			u16   reg_cmp_status_5;//addr=36
			u16   reg_cmp_status_6;//addr=37
			u16   reg_cmp_status_7;//addr=38
			u16   reg_cmp_status_8;//addr=39
			
			u16   reg_cmp_channel_num; //addr=40
			
//			u16 reg_is_study; //1学习过，0未学习过
        } fe_5014;
    } write;

    /* output registers */
    union
    {
        u16 registers[30];
        u8  bytes[30 * sizeof(u16)];
				/*
					8 relays
					8 channel
				*/
		/* addr: 512-1023 */
        struct
        {
			u16 reg_begin;  //reg:512  After learn, start test LED vol compare.
			u16 reg_switch;
		    u16 reg_relay;   //bit0-bit7 means
			
		    u16 reg_channel;
			u16 registers_3;
			u16 registers_4;
			u16 registers_5;
			u16 registers_6;
        } fe_5014;
    } read;

} Dat_ramDb;


u8 write_data(u16 address, u8 num, u8* buf);
u8 read_data(u16 address, u8 num, u8* buf);
void mb_resgister_init(void);

#endif
