/**
*@file
*@brief     max30102�����ļ�
*@details    
*@author        ������
*@date        2022-09-19
*@version       v1.0
*/
#include "max30102.h"


void max30102_i2c_write(uint8_t reg_adder,uint8_t data)
{
	uint8_t transmit_data[2];
	transmit_data[0] = reg_adder;
	transmit_data[1] = data;
	i2c_transmit(transmit_data,2);
}


void max30102_i2c_read(uint8_t reg_adder,uint8_t *pdata, uint8_t data_size)
{
		uint8_t adder = reg_adder;
		i2c_transmit(&adder,1);
		i2c_receive(pdata,data_size);
}

void max30102_init(void)
{ 
	uint8_t data;
	
	max30102_i2c_write(MODE_CONFIGURATION,0x40);  //reset the device
	
	delay_ms(5);
	
	max30102_i2c_write(INTERRUPT_ENABLE1,0xE0);
	max30102_i2c_write(INTERRUPT_ENABLE2,0x02);  //interrupt enable: FIFO almost full flag, new FIFO Data Ready,
																						 	//                   ambient light cancellation overflow, power ready flag, 
																							//						    		internal temperature ready flag
	
	max30102_i2c_write(FIFO_WR_POINTER,0x00);
	max30102_i2c_write(FIFO_OV_COUNTER,0x00);
	max30102_i2c_write(FIFO_RD_POINTER,0x00);   //clear the pointer
	
	max30102_i2c_write(FIFO_CONFIGURATION,0x0F); //FIFO configuration: sample averaging(1),FIFO rolls on full(0), FIFO almost full value(15 empty data samples when interrupt is issued)  
	
	max30102_i2c_write(MODE_CONFIGURATION,0x03);  //FIFO configuration:SpO2 mode
	
	max30102_i2c_write(SPO2_CONFIGURATION,0x26); //SpO2 configuration:ACD resolution:15.63pA,sample rate control:100Hz, LED pulse width:215 us 
	
	max30102_i2c_write(LED1_PULSE_AMPLITUDE,0x2f);
	max30102_i2c_write(LED2_PULSE_AMPLITUDE,0x2f); //LED current
	
	max30102_i2c_write(TEMPERATURE_CONFIG,0x01);  //temperature
	

	
	max30102_i2c_read(INTERRUPT_STATUS1,&data,1);
	max30102_i2c_read(INTERRUPT_STATUS2,&data,1);//clear the flag
}


void max30102_fifo_read(uint32_t *data)
{
    uint8_t receive_data[6],temp_data=0;
	
		max30102_i2c_read(INTERRUPT_STATUS1,&temp_data,1);
	
		while((temp_data&0x40)!=0x40)
		{
			max30102_i2c_read(INTERRUPT_STATUS1,&temp_data,1);
		}
		max30102_i2c_read(FIFO_DATA,receive_data,6);
    data[0] = ((receive_data[0]<<16 | receive_data[1]<<8 | receive_data[2]) & 0x03ffff);
    data[1] = ((receive_data[3]<<16 | receive_data[4]<<8 | receive_data[5]) & 0x03ffff);
		

 
}
