#include "database.h"
#include "m_usart1.h"
#include "bsp_i2c_ee.h"

//u8 data_open[8]={0x01,0x06,0x00,0x03,0x00,0x00,0x79,0xca};
//u8 data_close[8]={0x01,0x06,0x00,0x03,0x00,0x01,0xb8,0x0a};
//u8 data_persent[8]={0x01,0x06,0x00,0x03,0x00,0x00,0x79,0xca};

Dat_ramDb dat_ram;


/****************************************************************************
   This Function will write to registers either RAM  Non_volatile portions of
   memory. the data to be written can be a string of contiguous data.
*/
u8 write_data(u16 address, u8 num, u8* buf)
{
	u8 i;
	u16 offset;
	u8* bytes_ptr;
	u8  exception_code = 0;
	u8 channel_num_changed = 0;
	u8 cal_block_changed  = 0;
	u8 input_regs_changed = 0;
	u8 channel_num_buf[1];

    /* Read/write registers; Block name ModuleInfo */
    if ( (address >= IN_REGS_ADDR_START) && (address <= IN_REGS_ADDR_END) ) // ModuleInfo
    {
            address = address - IN_REGS_ADDR_START;
            offset = address * sizeof(u16);//same as     u8 offset = address * 2;
            
            if (offset + num > sizeof(dat_ram.read))
                exception_code = 2; /* out of range */
            else
            {
                bytes_ptr = (u8*)&dat_ram.read;
                channel_num_changed = 1; /* Something changed */
            }
    }
	/* New Add */
	else if ( (address >= OUT_REGS_ADDR_START) && (address <= OUT_REGS_ADDR_END) ) // ModuleInfo
    {
            address = address - OUT_REGS_ADDR_START;
            offset = address * sizeof(u16);//same as     u8 offset = address * 2;
            
            if (offset + num > sizeof(dat_ram.write))
                exception_code = 2; /* out of range */
            else
            {
                bytes_ptr = (u8*)&dat_ram.write;
				
				/* LM add for save channel_num to EE */
				if(address == 40)
				{
					 channel_num_changed = 1; /* Something changed */
				}
               
            }
    }
    else
        exception_code = 2; /* Bad address */

    if (exception_code == 0)
    {
        for(i = 0; i < num; i += 2)
        {
            bytes_ptr[offset+i] = buf[i+1];
            bytes_ptr[offset+i+1] = buf[i];
        }
    }
	
	/* LM add for save channel_num to EE  */
	if(channel_num_changed)
	{
		channel_num_changed=0;
		channel_num_buf[0] = dat_ram.write.fe_5014.reg_cmp_channel_num;
		ee_WriteBytes(channel_num_buf,11,1); //save to EE addr=11 area
	}
	
    return exception_code;
}

/****************************************************************************
   This Function will read from registers in either RAM. The data to be read
   can be a string of contiguous data
*/
u8 read_data(u16 address, u8 num, u8* buf)  //addr:0-255
{
    u8 i = 0;
    u16 offset;
    u8* bytes_ptr;
    u8  exception_code = 0;

	if (address <= OUT_REGS_ADDR_END) //*1024
    {
        address = address - OUT_REGS_ADDR_START;
        offset = address * sizeof(u16);//same as     u8 offset = address * 2;

        if (offset + num > sizeof(dat_ram.write))
            exception_code = 2; /* out of range */
        else
        {
            bytes_ptr = &dat_ram.write.bytes[0];
        }
    }
	/* New Add */
	 /* Read/write registers; Block name ModuleInfo */
    else if ( (address >= IN_REGS_ADDR_START) && (address <= IN_REGS_ADDR_END) ) //*[512,1021]
    {
        address = address - IN_REGS_ADDR_START;
        offset = address * sizeof(u16);//same as     U8 offset = address * 2;

        if (offset + num > sizeof(dat_ram.read))
            exception_code = 2; /* out of range */
        else
        {
            //bytes_ptr = (U8*)&dat_ram.read_write_info;
            bytes_ptr = (u8*)&dat_ram.read.bytes[0];
        }
    }
    else
        exception_code = 2; /* Bad Starting Address */

    if (exception_code == 0)
    {
        for(i = 0; i  < num; i += 2)
        {
            buf[i] = bytes_ptr[offset + i + 1]; //byte swap
            buf[i+1] = bytes_ptr[offset + i];
        }
    }

    return exception_code;
}



void mb_resgister_init(void)
{
//	dat_ram.write.fe_5014.registers_1 =10;
//	dat_ram.write.fe_5014.registers_2 =20;
//	dat_ram.write.fe_5014.registers_3 =30;
//	dat_ram.write.fe_5014.registers_4 =40;
}






