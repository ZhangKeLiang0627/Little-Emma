#ifndef __TIMER5_DELAY_H__
#define __TIMER5_DELAY_H__

#include "gd32f30x.h"                   // Device header

#define DELAY_TIMER		TIMER5

#define TMR_ENABLE()	TIMER_CTL0(DELAY_TIMER) |= (uint32_t)TIMER_CTL0_CEN
#define TMR_DISABLE()	TIMER_CTL0(DELAY_TIMER) &= ~(uint32_t)TIMER_CTL0_CEN
#define TMR_CLR_FLAG()	TIMER_INTF(DELAY_TIMER) = (~(uint32_t)TIMER_INT_FLAG_UP)
#define TMR_SET_CNT(X) 	TIMER_CNT(DELAY_TIMER) = (uint32_t)(X)

extern uint16_t us_count;
	
void Timer5_Delay_Init(void);
void delay_us(uint16_t count);
void delay_ms(uint16_t count);

#endif



