#ifndef __LED_H
#define __LED_H
#include "gd32f30x.h"
#include "DataType.h"

//LED端口定义
// #define LED0 PCout(13)	
void LED0(uint8_t BitValue);

void LED_Init(void);//初始化		 				    
#endif
