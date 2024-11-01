#include "systick.h"
#include "gd32f30x.h"                   // Device header
#include "key.h"
#include "led.h" 
#include "BEEPER.h"
#include "UART0.h"
/*Only For 1 Key//只能适配一个按键//不适用多按键*/

uint8_t Key_KeyNumber;//在定时器中得到的键码值（不受主程序中的delay影响）

Key_Tag Key_A0;	//定义一个A0引脚口的按键结构体

/**
  * @brief 返回键码值（放在主程序中循环）
  * @param  无
  * @retval 键码值
  */
uint8_t Key_GetNum(void)
{
	uint8_t Temp;
	Temp = Key_KeyNumber;
	
	/* 绑定BEEPER */
	if(Key_KeyNumber == SHORT_PRESS) Beeper_Perform(BEEPER_TRITONE);
	if(Key_KeyNumber == LONG_PRESS) Beeper_Perform(BEEPER_WARNING);
	
	/* 绑定U_led */
	if(gpio_input_bit_get(GPIOA, GPIO_PIN_0) == 0) LED0(0);
	else LED0(1);
	
	Key_KeyNumber = 0;
	return Temp;
}

/**
  * @brief 独立按键初始化
  * @param  无
  * @retval 无
  */
void Key_Init(void)
{
	rcu_periph_clock_enable(RCU_GPIOA);
	
	gpio_init(GPIOA, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_0);
	
	TIM1_Key_Init(); // 初始化TIM1
	
	Key_A0.LongPressOpen_Flag = 1;		//长按开关标志位开启
	Key_A0.DoublePressOpen_Flag = 0;	//双击开关标志位关闭
}

/**
  * @brief 采用定时器扫描按键的方式（已经初始化优先级和中断）
  * @param  无
  * @retval 无
  */
void TIM1_Key_Init(void)
{	
	timer_parameter_struct timer_init_structure;
	
	rcu_periph_clock_enable(RCU_TIMER1);//定时器1
	
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
  * @brief 初步获得按键的电平
  * @param  无
  * @retval 返回键码值
  */
uint8_t Key_GetState(void)
{
	uint8_t KeyNumber = 0;
	
	if(gpio_input_bit_get(GPIOA, GPIO_PIN_0) == 0){KeyNumber = 1;}
	
	return KeyNumber;
}

/**
  * @brief 判断单击&长按&双击&软件消抖
  * @param  无
  * @retval 无
  */
void Key_Loop(void)
{
	
	static uint8_t NowState, LastState;	//定义现在时刻的按键电平状态和前一时刻的按键电平状态
	LastState = NowState;//更新前一时刻状态
	NowState = Key_GetState();//更新现在的状态
	
	if(LastState==0 && NowState==1 && Key_A0.LongPressOpen_Flag)	//按键按下，长按计时开始
	{
		Key_A0.Time_1 = 0;//长按判断时长清零
	}
	
	if(LastState==1 && NowState==1 && Key_A0.Time_1 <= 1000 && Key_A0.LongPressOpen_Flag)	//按键持续下压，长按时间累加（1.0s）
	{
		Key_A0.Time_1 += 20;	//加20是因为TIM2定时20ms
	}
	
	if(LastState == 1 && Key_A0.Time_1 > 1000 && Key_A0.LongPressOpen_Flag)				//长按1.0s
	{
		Key_KeyNumber = LONG_PRESS;//返回长按键码值2
		Key_A0.Time_1 = 201;//优雅地避开长按后头触发短按的尴尬
	}
	
	
	
	if(LastState==1 && NowState==0 && Key_A0.Time_1 <= 200)	//短按&&双击
	{
		
		if(Key_A0.DoublePressOpen_Flag == 0)	//双击开关标志为零//双击没有打开
		{
			Key_KeyNumber = SHORT_PRESS;//返回单击键码值1
		}
		
		if(Key_A0.DoublePressOpen_Flag == 1)	//双击打开
		{
			if(Key_A0.FirstPress_Flag == 0)		//第一次按下判断
			{	
				Key_A0.FirstPress_Flag = 1;
				Key_A0.Time_2 = 0;//双击判断时长清零
			}
			else if(Key_A0.FirstPress_Flag == 1)	//如果已经按下第一次
			{
				if(Key_A0.Time_2 <= 200)	//如果前后两次按下时间间隔小于0.3s//这里改成0.2s了 实测问题不大
				{
					Key_KeyNumber = DOUBLE_PRESS;//返回双击键码值3
					Key_A0.FirstPress_Flag = 0;//清零第一次按下标志位
				}
			}
		}
	}

	if(Key_A0.FirstPress_Flag)//如果第一次已经按下
	{
		Key_A0.Time_2 += 20;	//双击判断时长开始累加//这里加20是因为TIM2计时20ms
		if(Key_A0.Time_2 > 200)//如果双击判断时长大于0.3s//则判断为单击//这里改成0.2s了//0.2s大概是极限了
		{
			Key_KeyNumber = SHORT_PRESS;//返回单击键码值1
			Key_A0.FirstPress_Flag = 0;//清零第一次按下标志位
		}
	}
}

uint32_t Time_10ms = 0;

/**
  * @brief TIM2中断扫描按键
  * @param  无
  * @retval 无
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




