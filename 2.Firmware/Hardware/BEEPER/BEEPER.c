#include "BEEPER.h"
#include "gd32f30x.h"
#include "Random.h"
#include "math.h"
#include "UART0.h"

/*����Beeper��handle*/
BEEPER_Tag Beeper0;

/*C4-B8��������Ӧ��Ƶ�ʴ�С*/
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

/*ȫ��TONE�ṹ��ָ�룬���ڶ�ʱ��TIM4�жϺ�����*/
TONE *MySound = NULL;

/*����*/
TONE BEEPER_KEYPRESS[] ={
{NOTE_C6, 50},
{CHECK_NOTE, 0},//���λ
};

TONE BEEPER_PULSE[] ={
{NOTE_F6, 300},
{CHECK_NOTE, 0},//���λ
};

TONE BEEPER_TRITONE[] ={
{NOTE_B5, 50},
{NOTE_D6, 50},
{NOTE_F6, 50},
{CHECK_NOTE, 0},//���λ
};

TONE BEEPER_WARNING[] ={
{NOTE_F4, 50},
{REST_NOTE, 50},
{NOTE_F4, 50},
{CHECK_NOTE, 0},//���λ
};



TONE BEEP1[] = {
{NOTE_C5, 110},
{REST_NOTE, 20},//��ֹ��
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
{CHECK_NOTE, 0},//���λ
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

    {CHECK_NOTE, 0},//���λ
};

void TIM2_PWM_Init(u16 arr)
{
    /*��ʼ���ṹ��*/
    timer_parameter_struct timer_parameter_Structure;
    timer_oc_parameter_struct timer_oc_parameter_Structure;
    
	/*����rccʱ��*/
    rcu_periph_clock_enable(RCU_TIMER2);

    /*��ʼ��TIM2*/
	timer_deinit(TIMER2);
	
    timer_struct_para_init(&timer_parameter_Structure);
	timer_parameter_Structure.alignedmode = TIMER_COUNTER_EDGE;
    timer_parameter_Structure.clockdivision = TIMER_CKDIV_DIV1;
    timer_parameter_Structure.counterdirection = TIMER_COUNTER_UP;
    timer_parameter_Structure.period = arr - 1;
    timer_parameter_Structure.prescaler = 12 - 1; // ʹ��Ƶ��ʣ��10000kHz
    timer_parameter_Structure.repetitioncounter = 0;

    timer_init(TIMER2, &timer_parameter_Structure);
	
    /*TIM2����PWMģʽ*/
    timer_channel_output_struct_para_init(&timer_oc_parameter_Structure);
    timer_oc_parameter_Structure.outputstate = TIMER_CCX_ENABLE;
    timer_oc_parameter_Structure.ocpolarity = TIMER_OC_POLARITY_HIGH;
    timer_oc_parameter_Structure.ocidlestate = TIMER_OC_IDLE_STATE_LOW;
	timer_oc_parameter_Structure.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
	timer_oc_parameter_Structure.ocnpolarity = TIMER_OCN_POLARITY_HIGH;
	timer_oc_parameter_Structure.outputnstate = TIMER_CCXN_DISABLE;
	
	timer_channel_output_config(TIMER2, TIMER_CH_0, &timer_oc_parameter_Structure);
	
    timer_channel_output_mode_config(TIMER2, TIMER_CH_0, TIMER_OC_MODE_PWM0);
	
    timer_channel_output_pulse_value_config(TIMER2, TIMER_CH_0, arr/2);//ռ�ձ�
	
	timer_primary_output_config(TIMER2, ENABLE);
	
	
    timer_disable(TIMER2);//�ȹص���ֹ�������ֽ�


}

void Beeper_Init(void)
{
   /*����rccʱ��*/
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_AF);

    /*��ʼ��PC6-TIM2_CH0*/
	gpio_pin_remap_config(GPIO_TIMER2_PARTIAL_REMAP, ENABLE);
	
	gpio_pin_remap_config(GPIO_SWJ_NONJTRST_REMAP, ENABLE);
	
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4);
	
	gpio_bit_reset(GPIOB, GPIO_PIN_4); // �������ŷ�ֹ�����ߵ�ƽ�ջ�������

    /*��ʼ����ʱ��TIM2*/
	TIM2_PWM_Init(8000);

    /* BEEPERʹ�ܱ�־λ */
    Beeper0.Beeper_Enable = 1;

    Beeper0.Sound_Loud = 98;

}

/*�����Ӧ��Ԥ��װֵARR��psc = 12 - 1��//��10000kHz / ����Ƶ��*/
uint16_t Set_Musical_Note(uint16_t frq)
{
    /*��ֹ��ֹ��ʱ�������ֽ�*/
    if(frq == 0) return 0;
    float temp = 0;
    temp = 10000000.0f / (float)frq;
    return (uint16_t)temp;
}


/**
  * @brief Beeper��Ӧ�ò㺯��
  * @param  TONE *Sound ����ṹ������
  * @retval ��
  */
void Beeper_Perform(TONE *Sound)
{
    /*�ñ������ڼ���ṹ������ĳ���*/
    uint16_t Note_Length;

    timer_disable(TIMER2);
    
    /*��ȫ�ֽṹ��ָ��ָ���������*/
    MySound = Sound;

    /*ͨ��Ѱ�Ҽ��λCHECK_NOTE�����㴫��Ľṹ�峤��//��Ϊsizeof���ڱ�������ɵ���������û����*/
    for(Note_Length = 0; MySound[Note_Length].Note != CHECK_NOTE; Note_Length++);
    
    /*���賤�ȴ�С*/
    Beeper0.Sound_Size = Note_Length;
    /*������������*/
    Beeper0.Beep_Play_Schedule = 0;
    /*����������ʹ�ܱ�־λ*/
    Beeper0.Beeper_Set_Flag = 1;
	
}

/* ����1ms��ʱ���жϽ���ѭ�� */
void Beeper_Proc(void)
{
    /*�ж��Ƿ����*/
    if(Beeper0.Beeper_Continue_Flag && Beeper0.Beeper_Enable)
    {
        /*�ж�����������û*/
        if(Beeper0.Beep_Play_Schedule < Beeper0.Sound_Size)
        {
            /*��Ԥ��װ��ֵ��ֵ���ı�����*/
			TIMER_CAR(TIMER2) = (uint16_t)Set_Musical_Note(MusicNoteFrequency[MySound[Beeper0.Beep_Play_Schedule].Note]);
            /*��PWMռ�ձȸ�ֵ���ı�����*/
			TIMER_CH0CV(TIMER2) = (uint16_t)Set_Musical_Note(MusicNoteFrequency[MySound[Beeper0.Beep_Play_Schedule].Note])/(100 - Beeper0.Sound_Loud); // ռ�ձ�
			
            /*ʱ�����1ms*/
            Beeper0.Beeper_Count --;
			
            timer_enable(TIMER2);
        }
        /*ʧ�ܷ���������ձ�־λ*/
        else {timer_disable(TIMER2); Beeper0.Beeper_Continue_Flag = 0; gpio_bit_reset(GPIOB, GPIO_PIN_4);} 
        /*�����������˼�����count = 65535ʱ����˼������ʱ�����ˣ��������������*/
        if(!(Beeper0.Beeper_Count < 65535))
        {
            /*�������ߵ���һ������*/
            Beeper0.Beep_Play_Schedule ++;
            /*��ֵ�µ���ʱ���ȸ�count*/
            Beeper0.Beeper_Count = MySound[Beeper0.Beep_Play_Schedule].Delay;
        }
    }else {timer_disable(TIMER2); gpio_bit_reset(GPIOB, GPIO_PIN_4);}
    /*�ж��Ƿ���������*///�о��ⲽ��̫��Ҫ��//����ȱһ����
    if(Beeper0.Beeper_Set_Flag && Beeper0.Beeper_Enable)
    {
        /*����ձ�־λ*/
        Beeper0.Beeper_Set_Flag = 0;
        /*�ж��������Ƿ�Ϊ��//�ⲽ�����õ�������־λ���жϣ�������������������*/
        if(Beeper0.Beep_Play_Schedule == 0) 
        {
            /*����������������־λ*/
            Beeper0.Beeper_Continue_Flag = 1;
            Beeper0.Beeper_Count = MySound[Beeper0.Beep_Play_Schedule].Delay;
        }
    }
}











