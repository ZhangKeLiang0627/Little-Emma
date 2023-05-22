#ifndef __MYSPI_H__
#define __MYSPI_H__
#include "gd32f30x.h"      
#include "DataType.h"

#define LCD_CS_Clr() 	gpio_bit_reset(GPIOA, GPIO_PIN_4)
#define LCD_CS_Set()	gpio_bit_set(GPIOA, GPIO_PIN_4)

#define LCD_CS1_Clr() 	gpio_bit_reset(GPIOB, GPIO_PIN_12)
#define LCD_CS1_Set()	gpio_bit_set(GPIOB, GPIO_PIN_12)


void MySPI0_Init(void);

void LCD_Writ_Bus(u8 dat);

void MySPI1_Init(void);

void SPI1_SetSpeed(spi_parameter_struct* spi_struct);

u8 SPI1_ReadWriteByte(u8 TxData);

#endif



