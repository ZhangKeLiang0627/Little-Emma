#ifndef __KEY_H__
#define __KEY_H__

#define SHORT_PRESS		1	//����
#define LONG_PRESS		2	//����
#define DOUBLE_PRESS 	3	//˫��

typedef struct{
	uint16_t Time_1;				//Time_1�жϳ���ʱ��
	uint16_t Time_2;				//Time_2�ж�˫��ʱ��
	uint8_t FirstPress_Flag;		//��һ�ΰ��±�־
	uint8_t SecondPress_Flag;		//�ڶ��ΰ��±�־
	uint8_t LongPressOpen_Flag;		//�Ƿ�򿪳�����־(1��/0�ر�)
	uint8_t DoublePressOpen_Flag;	//�Ƿ��˫����־(1��/0�ر�)
} Key_Tag;


void Key_Init(void);
uint8_t Key_GetNum(void);
void TIM1_Key_Init(void);

extern uint8_t Key_KeyNumber;
#endif













