#ifndef __BEEPER_H__
#define __BEEPER_H__
#include "sys.h"

/*����Ƶ�ʱ��Ӧ��ÿһ����������*/
typedef enum Musical_Note
{
	//��ֹ��
    REST_NOTE,
    NOTE_C4, NOTE_D4b, NOTE_D4, NOTE_E4b, NOTE_E4, NOTE_F4, NOTE_G4b, NOTE_G4, NOTE_A4b, NOTE_A4, NOTE_B4b, NOTE_B4,
    NOTE_C5, NOTE_D5b, NOTE_D5, NOTE_E5b, NOTE_E5, NOTE_F5, NOTE_G5b, NOTE_G5, NOTE_A5b, NOTE_A5, NOTE_B5b, NOTE_B5,
    NOTE_C6, NOTE_D6b, NOTE_D6, NOTE_E6b, NOTE_E6, NOTE_F6, NOTE_G6b, NOTE_G6, NOTE_A6b, NOTE_A6, NOTE_B6b, NOTE_B6,
    NOTE_C7, NOTE_D7b, NOTE_D7, NOTE_E7b, NOTE_E7, NOTE_F7, NOTE_G7b, NOTE_G7, NOTE_A7b, NOTE_A7, NOTE_B7b, NOTE_B7,
    NOTE_C8, NOTE_D8b, NOTE_D8, NOTE_E8b, NOTE_E8, NOTE_F8, NOTE_G8b, NOTE_G8, NOTE_A8b, NOTE_A8, NOTE_B8b, NOTE_B8,
	//���λ
	CHECK_NOTE,
}note_t;

/*�����Ľṹ������*/
typedef struct
{
	note_t Note;//musical_note
	uint16_t Delay;//ms
} TONE;

/*�������Ľṹ��*/
typedef struct
{ 
	uint8_t Beeper_Enable;
	uint8_t Beeper_Continue_Flag;
	uint8_t Beeper_Set_Flag;
	uint16_t Beeper_Count;
	uint16_t Sound_Size;
	uint16_t Beep_Play_Schedule;
	uint8_t Sound_Loud;
} BEEPER_Tag;


extern TONE *MySound;
extern TONE BEEPER_KEYPRESS[];
extern TONE BEEPER_TRITONE[];
extern TONE BEEPER_WARNING[];
extern TONE BEEPER_PULSE[];
extern TONE BEEP1[];
extern TONE BEEP2[];
extern BEEPER_Tag Beeper0;

/*����Ƶ�ʱ�*/
extern const uint16_t MusicNoteFrequency[];

/*�ײ㺯��*/
void TIM2_PWM_Init(u16 arr);
void Beeper_Init(void);
void Beeper_Perform(TONE Sound[]);

/*Ӧ�ò㺯��*/
uint16_t Set_Musical_Note(uint16_t frq);
void Beeper_Proc(void);
#endif


