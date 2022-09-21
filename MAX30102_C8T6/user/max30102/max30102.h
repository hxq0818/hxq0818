#ifndef __MAX30102_H
#define __MAX30102_H

/*******************************以下根据实际情况设置*******************************/
#include "main.h"
extern I2C_HandleTypeDef hi2c1;
#define  i2c_transmit(pdata,data_size)              HAL_I2C_Master_Transmit(&hi2c1,I2C_WRITE_ADDR,pdata,data_size,10)
#define  i2c_receive(pdata,data_size)   						HAL_I2C_Master_Receive(&hi2c1,I2C_READ_ADDR,pdata,data_size,10)
#define  delay_ms(ms)                                HAL_Delay(ms)
/***********************************************************************************/



#define I2C_WRITE_ADDR 0xAE
#define I2C_READ_ADDR 0xAF

#define INTERRUPT_STATUS1 0X00
#define INTERRUPT_STATUS2 0X01
#define INTERRUPT_ENABLE1 0X02
#define INTERRUPT_ENABLE2 0X03

#define FIFO_WR_POINTER 0X04
#define FIFO_OV_COUNTER 0X05
#define FIFO_RD_POINTER 0X06
#define FIFO_DATA 0X07

#define FIFO_CONFIGURATION 0X08
#define MODE_CONFIGURATION 0X09
#define SPO2_CONFIGURATION 0X0A
#define LED1_PULSE_AMPLITUDE 0X0C
#define LED2_PULSE_AMPLITUDE 0X0D

#define MULTILED1_MODE 0X11
#define MULTILED2_MODE 0X12

#define TEMPERATURE_INTEGER 0X1F
#define TEMPERATURE_FRACTION 0X20
#define TEMPERATURE_CONFIG 0X21

#define VERSION_ID 0XFE
#define PART_ID 0XFF


extern bool Hand_Contact;


void max30102_init(void);
void max30102_fifo_read(float *data);
void max30102_i2c_read(uint8_t reg_adder,uint8_t *pdata, uint8_t data_size);
uint16_t max30102_getHeartRate(float *input_data,uint16_t cache_nums);
float max30102_getSpO2(float *ir_input_data,float *red_input_data,uint16_t cache_nums);

int16_t Max30120GeiHeartData(int *input_data);
int16_t Max30120GeiSpo2Data(float *input_data);

#endif /* __MAX30102_H */
