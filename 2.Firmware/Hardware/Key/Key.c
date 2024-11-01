#include "systick.h"
#include "gd32f30x.h"                   // Device header
#include "key.h"
#include "led.h" 
#include "BEEPER.h"
#include "UART0.h"
/*Only For 1 Key//ֻ������һ������//�����öఴ��*/

uint8_t Key_KeyNumber;//�ڶ�ʱ���еõ��ļ���ֵ�������������е�delayӰ�죩

Key_Tag Key_A0;	//����һ��A0���ſڵİ����ṹ��

/**
  * @brief ���ؼ���ֵ��������������ѭ����
  * @param  ��
  * @retval ����ֵ
  */
uint8_t Key_GetNum(void)
{
	uint8_t Temp;
	Temp = Key_KeyNumber;
	
	/* ��BEEPER */
	if(Key_KeyNumber == SHORT_PRESS) Beeper_Perform(BEEPER_TRITONE);
	if(Key_KeyNumber == LONG_PRESS) Beeper_Perform(BEEPER_WARNING);
	
	/* ��U_led */
	if(gpio_input_bit_get(GPIOA, GPIO_PIN_0) == 0) LED0(0);
	else LED0(1);
	
	Key_KeyNumber = 0;
	return Temp;
}

/**
  * @brief ����������ʼ��
  * @param  ��
  * @retval ��
  */
void Key_Init(void)
{
	rcu_periph_clock_enable(RCU_GPIOA);
	
	gpio_init(GPIOA, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_0);
	
	TIM1_Key_Init(); // ��ʼ��TIM1
	
	Key_A0.LongPressOpen_Flag = 1;		//�������ر�־λ����
	Key_A0.DoublePressOpen_Flag = 0;	//˫�����ر�־λ�ر�
}

/**
  * @brief ���ö�ʱ��ɨ�谴���ķ�ʽ���Ѿ���ʼ�����ȼ����жϣ�
  * @param  ��
  * @retval ��
  */
void TIM1_Key_Init(void)
{	
	timer_parameter_struct timer_init_structure;
	
	rcu_periph_clock_enable(RCU_TIMER1);//��ʱ��1
	
	timer_deinit(TIMER1);
	timer_init_structure.prescaler = 120 - 1;
	timer_init_structure.period = 20000 - 1;
	timer_init_structure.alignedmode = TIMER_COUNTER_EDGE;
	timer_init_structure.counterdirection = TIMER_COUNTER_UP;
	timer_init_structure.clockdivision = TIMER_CKDIV_DIV1;
	timer_init_structure.repetitioncounter = 0;
	timer_init(TIMER1, &timer_init_structure);
	timer_flag_clear(TIMER1, TIMER_FLAG_UP);
	
	nvic_irq_enable(TIMER1_IRQn, 0, 0);
	
	timer_interrupt_enable(TIMER1, TIMER_INT_UP);
	
	timer_enable(TIMER1);
}

/**
  * @brief ������ð����ĵ�ƽ
  * @param  ��
  * @retval ���ؼ���ֵ
  */
uint8_t Key_GetState(void)
{
	uint8_t KeyNumber = 0;
	
	if(gpio_input_bit_get(GPIOA, GPIO_PIN_0) == 0){KeyNumber = 1;}
	
	return KeyNumber;
}

/**
  * @brief �жϵ���&����&˫��&�������
  * @param  ��
  * @retval ��
  */
void Key_Loop(void)
{
	
	static uint8_t NowState, LastState;	//��������ʱ�̵İ�����ƽ״̬��ǰһʱ�̵İ�����ƽ״̬
	LastState = NowState;//����ǰһʱ��״̬
	NowState = Key_GetState();//�������ڵ�״̬
	
	if(LastState==0 && NowState==1 && Key_A0.LongPressOpen_Flag)	//�������£�������ʱ��ʼ
	{
		Key_A0.Time_1 = 0;//�����ж�ʱ������
	}
	
	if(LastState==1 && NowState==1 && Key_A0.Time_1 <= 1000 && Key_A0.LongPressOpen_Flag)	//����������ѹ������ʱ���ۼӣ�1.0s��
	{
		Key_A0.Time_1 += 20;	//��20����ΪTIM2��ʱ20ms
	}
	
	if(LastState == 1 && Key_A0.Time_1 > 1000 && Key_A0.LongPressOpen_Flag)				//����1.0s
	{
		Key_KeyNumber = LONG_PRESS;//���س�������ֵ2
		Key_A0.Time_1 = 201;//���ŵرܿ�������ͷ�����̰�������
	}
	
	
	
	if(LastState==1 && NowState==0 && Key_A0.Time_1 <= 200)	//�̰�&&˫��
	{
		
		if(Key_A0.DoublePressOpen_Flag == 0)	//˫�����ر�־Ϊ��//˫��û�д�
		{
			Key_KeyNumber = SHORT_PRESS;//���ص�������ֵ1
		}
		
		if(Key_A0.DoublePressOpen_Flag == 1)	//˫����
		{
			if(Key_A0.FirstPress_Flag == 0)		//��һ�ΰ����ж�
			{	
				Key_A0.FirstPress_Flag = 1;
				Key_A0.Time_2 = 0;//˫���ж�ʱ������
			}
			else if(Key_A0.FirstPress_Flag == 1)	//����Ѿ����µ�һ��
			{
				if(Key_A0.Time_2 <= 200)	//���ǰ�����ΰ���ʱ����С��0.3s//����ĳ�0.2s�� ʵ�����ⲻ��
				{
					Key_KeyNumber = DOUBLE_PRESS;//����˫������ֵ3
					Key_A0.FirstPress_Flag = 0;//�����һ�ΰ��±�־λ
				}
			}
		}
	}

	if(Key_A0.FirstPress_Flag)//�����һ���Ѿ�����
	{
		Key_A0.Time_2 += 20;	//˫���ж�ʱ����ʼ�ۼ�//�����20����ΪTIM2��ʱ20ms
		if(Key_A0.Time_2 > 200)//���˫���ж�ʱ������0.3s//���ж�Ϊ����//����ĳ�0.2s��//0.2s����Ǽ�����
		{
			Key_KeyNumber = SHORT_PRESS;//���ص�������ֵ1
			Key_A0.FirstPress_Flag = 0;//�����һ�ΰ��±�־λ
		}
	}
}

uint32_t Time_10ms = 0;

/**
  * @brief TIM2�ж�ɨ�谴��
  * @param  ��
  * @retval ��
  */
void TIMER1_IRQHandler(void)
{
	
	if(timer_interrupt_flag_get(TIMER1, TIMER_INT_FLAG_UP))
	{
		timer_interrupt_flag_clear(TIMER1, TIMER_INT_FLAG_UP);
		Time_10ms ++;
		Key_Loop();
	}
}




