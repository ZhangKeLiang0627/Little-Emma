#include "led.h" 
#include "gd32f30x.h"

//LED IO��ʼ��
void LED_Init(void)
{    	 
	rcu_periph_clock_enable(RCU_GPIOC);
	//GPIOC13��ʼ������
	gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13);

	gpio_bit_set(GPIOC, GPIO_PIN_13);//GPIOC13���øߣ�����
}

void LED0(uint8_t BitValue)
{
	if(BitValue) gpio_bit_write(GPIOC, GPIO_PIN_13, SET);
	else gpio_bit_write(GPIOC, GPIO_PIN_13, RESET);
}




