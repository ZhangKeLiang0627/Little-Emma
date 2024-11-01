#include "sys.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "BEEPER.h"
#include "UART0.h"

//使用了timer4来计算帧率FPS

uint32_t Time_ms = 0;	//毫秒计数

/**
  * @brief 初始化TIM4(已经配置好优先级了) 
  * @param  ARR(重装载值）//PSC(预分频值)
  * @retval 无
  */
void TIM4_Random_Init(uint16_t ARR, uint16_t PSC)
{	
	timer_parameter_struct timer_init_structure;
	
	rcu_periph_clock_enable(RCU_TIMER4);
	
	timer_deinit(TIMER4);
	timer_init_structure.alignedmode = TIMER_COUNTER_EDGE;
	timer_init_structure.clockdivision = TIMER_CKDIV_DIV1;
	timer_init_structure.counterdirection = TIMER_COUNTER_UP;
	timer_init_structure.period = ARR;//1200 - 1
	timer_init_structure.prescaler = PSC;//10 - 1 
	timer_init_structure.repetitioncounter = 0;
	timer_init(TIMER4, &timer_init_structure);
	timer_flag_clear(TIMER4, TIMER_FLAG_UP);
	
	nvic_irq_enable(TIMER4_IRQn, 2, 2);
	
	timer_interrupt_enable(TIMER4, TIMER_INT_UP);
	
	timer_enable(TIMER4);
	
}

/**
  * @brief 产生一个0到100的随机数
  * @param  无
  * @retval 返回这个得到的随机数
  */
uint32_t RandomCreate(void)
{
	uint32_t Num = 0;
	
	srand(Time_ms);
	
	Num = rand() % 100000000;		//产生一个0到100的随机数//改成一亿先
	
	return Num;
}

/**
  * @brief 再产生一个0到1000的随机数
  * @param  Number给srand当种子
  * @retval 返回这个得到的随机数
  */
uint32_t RandomCreateAgain(uint16_t Number)
{
	uint32_t Num = 0;
	srand(Number);
	Num = rand() % 100000000;	//改成一亿先
	
	return Num;
}

uint8_t max30102_set_flag = 0;
uint16_t max30102_count = 0;
uint8_t mpu6050_set_flag;
uint8_t mpu6050_count;
/**
  * @brief TIM4中断（us计数器）
  * @param  无
  * @retval 无
  */
void TIMER4_IRQHandler(void)
{
	if(timer_interrupt_flag_get(TIMER4, TIMER_INT_FLAG_UP) == SET)
	{
		timer_interrupt_flag_clear(TIMER4, TIMER_INT_FLAG_UP);
		Time_ms++;

		mpu6050_count++;
		
		if(mpu6050_count >= 10)
		{
			mpu6050_set_flag = 1;
			mpu6050_count = 0;
		}
		
		max30102_count ++;
		
		if(max30102_count > 10)
		{
			max30102_set_flag = 1;
			max30102_count = 0;
		}
			
		Beeper_Proc();
	}
}




