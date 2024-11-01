#include "TIMER5_Delay.h"
#include "gd32f30x.h"                   // Device header

uint16_t us_count;


/**
  * @brief ��ʼ����ʱ������ʱ��
  * @param  ��
  * @retval ��
  */
void Timer5_Delay_Init(void)
{
	timer_parameter_struct timer_init_structure;
	rcu_periph_clock_enable(RCU_TIMER5);
	timer_init_structure.alignedmode = TIMER_COUNTER_EDGE;
	timer_init_structure.clockdivision = TIMER_CKDIV_DIV1;
	timer_init_structure.counterdirection = TIMER_COUNTER_UP;
	timer_init_structure.period = (SystemCoreClock / 1000 / 1000) - 1;//SystemCoreClock / 1000 / 1000	//71
	timer_init_structure.prescaler = 1 - 1;//��Ƶֵ0��Ƶ�ʺ�ϵͳʱ��һ��
	timer_init_structure.repetitioncounter = 0;
	timer_init(TIMER5, &timer_init_structure);
	
	timer_flag_clear(TIMER5, TIMER_FLAG_UP);
	
	timer_interrupt_enable(TIMER5, TIMER_INT_UP);//ʹ�ܸ����ж�
	nvic_irq_enable(TIMER5_IRQn, 2, 0);//ʹ�����ȼ�
	
	TMR_DISABLE();//�رն�ʱ����
}

/**
  * @brief ��ʱ���жϺ���
  * @param  ��
  * @retval ��
  */
void TIMER5_IRQHandler(void)
{
	TMR_CLR_FLAG();
	us_count++;//΢���ۼ�
}

void delay_us(uint16_t count)
{
	TMR_SET_CNT(0);
	TMR_ENABLE();
	us_count = 0;
	while(us_count < count);
	TMR_DISABLE();
}

void delay_ms(uint16_t count)
{
	while(count--)
	{
		delay_us(1000);//Ϊ�˱ܿ��ж�
	}
}









