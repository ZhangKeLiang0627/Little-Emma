#include "BEEPER.h"
#include "gd32f30x.h"
#include "Random.h"
#include "math.h"
#include "UART0.h"

/*创建Beeper的handle*/
BEEPER_Tag Beeper0;

/*C4-B8的音调对应的频率大小*/
const uint16_t MusicNoteFrequency[] = {
//rest_note
    0,
//   C    C#   D   Eb    E    F    F#   G    G#   A   Bb    B
    262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494,
    523, 554, 587, 622, 659, 698, 740, 784, 830, 880, 932, 988,
    1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661, 1760, 1865, 1976,
    2093, 2218, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951,
    4186, 4435, 4699, 4978, 5274, 5588, 5920, 6272, 6645, 7040, 7459, 7902,
//check_note
    0,
};

/*全局TONE结构体指针，用于定时器TIM4中断函数中*/
TONE *MySound = NULL;

/*乐曲*/
TONE BEEPER_KEYPRESS[] ={
{NOTE_C6, 50},
{CHECK_NOTE, 0},//检查位
};

TONE BEEPER_PULSE[] ={
{NOTE_F6, 300},
{CHECK_NOTE, 0},//检查位
};

TONE BEEPER_TRITONE[] ={
{NOTE_B5, 50},
{NOTE_D6, 50},
{NOTE_F6, 50},
{CHECK_NOTE, 0},//检查位
};

TONE BEEPER_WARNING[] ={
{NOTE_F4, 50},
{REST_NOTE, 50},
{NOTE_F4, 50},
{CHECK_NOTE, 0},//检查位
};



TONE BEEP1[] = {
{NOTE_C5, 110},
{REST_NOTE, 20},//休止符
{NOTE_C5, 110},
{REST_NOTE, 20},
{NOTE_G5, 110},
{REST_NOTE, 20},
{NOTE_G5, 110},
{REST_NOTE, 20},
{NOTE_A5, 110},
{REST_NOTE, 20},
{NOTE_A5, 110},
{REST_NOTE, 20},
{NOTE_G5, 220},
{REST_NOTE, 20},
{NOTE_F6, 110},
{REST_NOTE, 20},
{NOTE_F6, 110},
{REST_NOTE, 20},
{NOTE_E7, 110},
{REST_NOTE, 20},
{NOTE_E7, 110},
{REST_NOTE, 20},
{NOTE_D8, 110},
{REST_NOTE, 20},
{NOTE_D8, 110},
{REST_NOTE, 20},
{NOTE_C4, 220},
{CHECK_NOTE, 0},//检查位
};



TONE BEEP2[] = {
    {REST_NOTE, 200},
    {REST_NOTE, 200},
    {REST_NOTE, 200},
    {NOTE_C5, 100},
    {NOTE_B4, 100},

    {NOTE_A4, 200},
    {NOTE_E5, 400},
    {NOTE_C5, 100},
    {NOTE_A4, 100},

    {NOTE_B4, 200},
    {NOTE_F5, 200},
    {NOTE_E5, 100},
    {NOTE_D5, 300},

    {NOTE_C5, 100},
    {NOTE_D5, 100},
    {NOTE_C5, 100},
    {NOTE_D5, 100},
    {NOTE_E5, 200},
    {NOTE_C5, 100},
    {NOTE_B4, 100},

    {NOTE_A4, 200},
    {NOTE_D5, 200},
    {NOTE_C5, 100},
    {NOTE_B4, 100},
    {REST_NOTE, 100},
    {NOTE_A4, 50},
    {NOTE_B4, 50},

    {NOTE_C5, 200},
    {NOTE_A4, 200},
    {NOTE_E5, 200},
    {NOTE_C5, 200},

    {NOTE_D5, 200},
    {NOTE_A5, 200},
    {NOTE_G5, 200},
    {NOTE_F5, 100},
    {NOTE_E5, 50},
    {NOTE_D5, 50},

    {NOTE_E5, 800},

    {CHECK_NOTE, 0},//检查位
};

void TIM2_PWM_Init(u16 arr)
{
    /*初始化结构体*/
    timer_parameter_struct timer_parameter_Structure;
    timer_oc_parameter_struct timer_oc_parameter_Structure;
    
	/*开启rcc时钟*/
    rcu_periph_clock_enable(RCU_TIMER2);

    /*初始化TIM2*/
	timer_deinit(TIMER2);
	
    timer_struct_para_init(&timer_parameter_Structure);
	timer_parameter_Structure.alignedmode = TIMER_COUNTER_EDGE;
    timer_parameter_Structure.clockdivision = TIMER_CKDIV_DIV1;
    timer_parameter_Structure.counterdirection = TIMER_COUNTER_UP;
    timer_parameter_Structure.period = arr - 1;
    timer_parameter_Structure.prescaler = 12 - 1; // 使得频率剩下10000kHz
    timer_parameter_Structure.repetitioncounter = 0;

    timer_init(TIMER2, &timer_parameter_Structure);
	
    /*TIM2配置PWM模式*/
    timer_channel_output_struct_para_init(&timer_oc_parameter_Structure);
    timer_oc_parameter_Structure.outputstate = TIMER_CCX_ENABLE;
    timer_oc_parameter_Structure.ocpolarity = TIMER_OC_POLARITY_HIGH;
    timer_oc_parameter_Structure.ocidlestate = TIMER_OC_IDLE_STATE_LOW;
	timer_oc_parameter_Structure.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
	timer_oc_parameter_Structure.ocnpolarity = TIMER_OCN_POLARITY_HIGH;
	timer_oc_parameter_Structure.outputnstate = TIMER_CCXN_DISABLE;
	
	timer_channel_output_config(TIMER2, TIMER_CH_0, &timer_oc_parameter_Structure);
	
    timer_channel_output_mode_config(TIMER2, TIMER_CH_0, TIMER_OC_MODE_PWM0);
	
    timer_channel_output_pulse_value_config(TIMER2, TIMER_CH_0, arr/2);//占空比
	
	timer_primary_output_config(TIMER2, ENABLE);
	
	
    timer_disable(TIMER2);//先关掉防止蜂鸣器怪叫


}

void Beeper_Init(void)
{
   /*开启rcc时钟*/
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_AF);

    /*初始化PC6-TIM2_CH0*/
	gpio_pin_remap_config(GPIO_TIMER2_PARTIAL_REMAP, ENABLE);
	
	gpio_pin_remap_config(GPIO_SWJ_NONJTRST_REMAP, ENABLE);
	
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4);
	
	gpio_bit_reset(GPIOB, GPIO_PIN_4); // 拉低引脚防止持续高电平烧坏蜂鸣器

    /*初始化定时器TIM2*/
	TIM2_PWM_Init(8000);

    /* BEEPER使能标志位 */
    Beeper0.Beeper_Enable = 1;

    Beeper0.Sound_Loud = 98;

}

/*计算对应的预重装值ARR（psc = 12 - 1）//用10000kHz / 音调频率*/
uint16_t Set_Musical_Note(uint16_t frq)
{
    /*防止休止符时蜂鸣器怪叫*/
    if(frq == 0) return 0;
    float temp = 0;
    temp = 10000000.0f / (float)frq;
    return (uint16_t)temp;
}


/**
  * @brief Beeper的应用层函数
  * @param  TONE *Sound 传入结构体数组
  * @retval 无
  */
void Beeper_Perform(TONE *Sound)
{
    /*该变量用于计算结构体数组的长度*/
    uint16_t Note_Length;

    timer_disable(TIMER2);
    
    /*让全局结构体指针指向传入的乐曲*/
    MySound = Sound;

    /*通过寻找检查位CHECK_NOTE来计算传入的结构体长度//因为sizeof是在编译中完成的所以这里没法用*/
    for(Note_Length = 0; MySound[Note_Length].Note != CHECK_NOTE; Note_Length++);
    
    /*赋予长度大小*/
    Beeper0.Sound_Size = Note_Length;
    /*把音符表清零*/
    Beeper0.Beep_Play_Schedule = 0;
    /*开启蜂鸣器使能标志位*/
    Beeper0.Beeper_Set_Flag = 1;
	
}

/* 用于1ms定时器中断进行循环 */
void Beeper_Proc(void)
{
    /*判断是否继续*/
    if(Beeper0.Beeper_Continue_Flag && Beeper0.Beeper_Enable)
    {
        /*判断音符表走完没*/
        if(Beeper0.Beep_Play_Schedule < Beeper0.Sound_Size)
        {
            /*给预重装载值赋值，改变音调*/
			TIMER_CAR(TIMER2) = (uint16_t)Set_Musical_Note(MusicNoteFrequency[MySound[Beeper0.Beep_Play_Schedule].Note]);
            /*给PWM占空比赋值，改变音量*/
			TIMER_CH0CV(TIMER2) = (uint16_t)Set_Musical_Note(MusicNoteFrequency[MySound[Beeper0.Beep_Play_Schedule].Note])/(100 - Beeper0.Sound_Loud); // 占空比
			
            /*时间减短1ms*/
            Beeper0.Beeper_Count --;
			
            timer_enable(TIMER2);
        }
        /*失能蜂鸣器，清空标志位*/
        else {timer_disable(TIMER2); Beeper0.Beeper_Continue_Flag = 0; gpio_bit_reset(GPIOB, GPIO_PIN_4);} 
        /*这个操作的意思是如果count = 65535时，意思就是延时结束了，这个音符演完了*/
        if(!(Beeper0.Beeper_Count < 65535))
        {
            /*音符表走到下一个音符*/
            Beeper0.Beep_Play_Schedule ++;
            /*赋值新的延时长度给count*/
            Beeper0.Beeper_Count = MySound[Beeper0.Beep_Play_Schedule].Delay;
        }
    }else {timer_disable(TIMER2); gpio_bit_reset(GPIOB, GPIO_PIN_4);}
    /*判断是否开启蜂鸣器*///感觉这步不太需要了//但是缺一不可
    if(Beeper0.Beeper_Set_Flag && Beeper0.Beeper_Enable)
    {
        /*先清空标志位*/
        Beeper0.Beeper_Set_Flag = 0;
        /*判断音符表是否为零//这步不能拿到继续标志位中判断，否则蜂鸣器会持续长鸣*/
        if(Beeper0.Beep_Play_Schedule == 0) 
        {
            /*开启蜂鸣器继续标志位*/
            Beeper0.Beeper_Continue_Flag = 1;
            Beeper0.Beeper_Count = MySound[Beeper0.Beep_Play_Schedule].Delay;
        }
    }
}











