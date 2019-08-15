#include "sys.h"

#define log_print	printf
#define FLOW_1    1
#define FLOW_2 	  2
#define FLOW_IDLE 10
#define FLOW_1_a  4
#define FLOW_2_b  5

#define GEAR_10R  1
#define GEAR_3K   2
#define GEAR_NONE 0


void set_flow(u8 flow_num);
u16 dynamic_study(void);
void turn_on_switch(void);
void turn_off_switch(void);
u8 run_led_compare(void);
void close_led_compare(void);

u8 get_gear_num(void);
void set_gear_num(u8 Gear_Num);
void ad1118_clear_all_channel();
void ad1118_read_all_channel();
void read_all_channel_current(u8 channel_);