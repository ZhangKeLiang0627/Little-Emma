#ifndef __MAX30102_H
#define __MAX30102_H
#include "gd32f30x.h"
#include "DataType.h"

////////////////////////////////////////////////////////////////////////////////// 	  

#define CACHE_NUMS 500//ª∫¥Ê ˝
#define PPG_DATA_THRESHOLD 100000 	//ºÏ≤‚„–÷µ

#define MAX30102_DATA_OK 1

#define MAX30102_INT PBin(12)

#define I2C_WR	0		/* –¥øÿ÷∆bit */
#define I2C_RD	1		/* ∂¡øÿ÷∆bit */

#define max30102_WR_address 0xAE

#define I2C_WRITE_ADDR 0xAE
#define I2C_READ_ADDR 0xAF

//register addresses
#define REG_INTR_STATUS_1 0x00
#define REG_INTR_STATUS_2 0x01
#define REG_INTR_ENABLE_1 0x02
#define REG_INTR_ENABLE_2 0x03
#define REG_FIFO_WR_PTR 0x04
#define REG_OVF_COUNTER 0x05
#define REG_FIFO_RD_PTR 0x06
#define REG_FIFO_DATA 0x07
#define REG_FIFO_CONFIG 0x08
#define REG_MODE_CONFIG 0x09
#define REG_SPO2_CONFIG 0x0A
#define REG_LED1_PA 0x0C
#define REG_LED2_PA 0x0D
#define REG_PILOT_PA 0x10
#define REG_MULTI_LED_CTRL1 0x11
#define REG_MULTI_LED_CTRL2 0x12
#define REG_TEMP_INTR 0x1F
#define REG_TEMP_FRAC 0x20
#define REG_TEMP_CONFIG 0x21
#define REG_PROX_INT_THRESH 0x30
#define REG_REV_ID 0xFE
#define REG_PART_ID 0xFF

void max30102_init(void);  
void max30102_reset(void);
u8 max30102_Bus_Write(u8 Register_Address, u8 Word_Data);
u8 max30102_Bus_Read(u8 Register_Address);
void max30102_FIFO_ReadWords(u8 Register_Address,u16  Word_Data[][2],u8 count);
void max30102_FIFO_ReadBytes(u8 Register_Address,u8* Data);

void maxim_max30102_write_reg(uint8_t uch_addr, uint8_t uch_data);
void maxim_max30102_read_reg(uint8_t uch_addr, uint8_t *puch_data);
void maxim_max30102_read_fifo(uint32_t *pun_red_led, uint32_t *pun_ir_led);

uint8_t MAX30102_Get_DATA(int32_t *HeartRate,int32_t *SpO2,uint32_t max30102_data[2]);
//uint8_t MAX30102_Get_DATA(uint16_t *HeartRate,float *SpO2,float max30102_data[2],float fir_output[2]);
float max30102_getSpO2(float *ir_input_data, float *red_input_data, uint16_t cache_nums);
uint16_t max30102_getHeartRate(float *input_data, uint16_t cache_nums);
void max30102_fifo_read(uint32_t *output_data);
#endif
















