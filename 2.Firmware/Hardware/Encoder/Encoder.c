#include "Encoder.h"
#include "BEEPER.h"
#include "UART0.h"
void Encoder_Init(void)
{  
    /*初始化结构体*/
    timer_parameter_struct timer_parameter_Structure;
    timer_ic_parameter_struct timer_ic_parameter_Structure;
	
	/*开启rcc时钟*/
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_TIMER3);
	rcu_periph_clock_enable(RCU_AF);

     /*初始化PB6&PB7*/
    gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_MAX, GPIO_PIN_6 | GPIO_PIN_7); // 在gd32f3中配置输入捕获需要引脚为浮空输入模式
	
	/*TIM3初始化*/
    timer_struct_para_init(&timer_parameter_Structure);
	timer_parameter_Structure.alignedmode = TIMER_COUNTER_EDGE;
    timer_parameter_Structure.clockdivision = TIMER_CKDIV_DIV1;
    timer_parameter_Structure.counterdirection = TIMER_COUNTER_UP;
    timer_parameter_Structure.period = 65535;
    timer_parameter_Structure.prescaler = 1 - 1;
    timer_parameter_Structure.repetitioncounter = 0;

    timer_init(TIMER3, &timer_parameter_Structure);
	
	/*输入捕获初始化*/
    timer_channel_input_struct_para_init(&timer_ic_parameter_Structure);
    timer_ic_parameter_Structure.icfilter = 10;
	timer_input_capture_config(TIMER3, TIMER_CH_0, &timer_ic_parameter_Structure);
	timer_input_capture_config(TIMER3, TIMER_CH_1, &timer_ic_parameter_Structure);
	
    /*配置编码器函数*/
    timer_quadrature_decoder_mode_config(TIMER3, TIMER_ENCODER_MODE0, TIMER_IC_POLARITY_RISING, TIMER_IC_POLARITY_RISING);

	/*初始化计数值*/
    timer_counter_value_config(TIMER3, 32768);
	
	/*配置优先级*/
    nvic_irq_enable(TIMER3_IRQn, 1, 2);
	
	/*开启定时器TIM3*/
    timer_enable(TIMER3);

}

/**
  * @brief 读取CE11的值（1or-1）
  * @param  无
  * @retval 返回（1or-1）
  */
int Read_value(void)
{
	int value_1 = 0;
		
	value_1 = (int16_t)(timer_counter_read(TIMER3) - 32768);timer_counter_value_config(TIMER3, 32768);	

	return value_1;
}

/**
  * @brief 应用层函数（在主函数中调用）
  * @param  无
  * @retval 正转返回1/反转返回2
  */
uint8_t Encoder_GetNum(void)
{
    int Encoder_Num = 0;
    static uint8_t Encoder_ReturnNum1, Encoder_ReturnNum2;
    Encoder_Num = Read_value();
    switch(Encoder_Num)
    {
        case 1: Encoder_ReturnNum1 ++; break;
        case -1: Encoder_ReturnNum2 ++; break;
        default: break;
    }
	if(Encoder_ReturnNum1 >= 2)
	{
		Encoder_ReturnNum1 = 0;
		Encoder_ReturnNum2 = 0;
        Beeper_Perform(BEEPER_KEYPRESS);
		return 1;
	}
	else if(Encoder_ReturnNum2 >= 2 || (Encoder_ReturnNum1 == 1 && Encoder_ReturnNum2 == 1))
	{
		Encoder_ReturnNum1 = 0;
		Encoder_ReturnNum2 = 0;
        Beeper_Perform(BEEPER_KEYPRESS);
		return 2;
	}	
	else return 0;
}











