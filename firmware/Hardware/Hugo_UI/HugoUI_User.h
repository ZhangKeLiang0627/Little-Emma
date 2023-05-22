#ifndef __HUGOUI_USER_H
#define __HUGOUI_USER_H

#include "oled.h"
#include "OLED_User.h"
#include "Hugo_UI.h"
#include "gd32f30x.h"                   // Device header


/* ����ȫ�ֱ��� */
extern uint8_t* Switch_space[];

/**
*    @������ *Slide_space[]
*    @brief �������ؼ� ���ڴ洢�������ؼ���ֵ
*    @param none
*    
*    @ע��
*        -0->��Ļ����
*        -1->��ѡ�����
*        -2->ѡ�����̶�����
*/
enum Slide_space_Obj
{
    Slide_space_ScreenBrightness = 0,
	
    Slide_space_val1,
    Slide_space_val2,
    Slide_space_val3,
    Slide_space_Volume_Ctrl,
	Slide_space_SPO2MinVal,
	Slide_space_HRMaxVal,
	Slide_space_HRMinVal,

    Slide_space_Obj_Max, // ��ǳ����ֵ

};

/**
*    @������ *Switch_space[]
*    @brief ���ؿؼ� ���ڴ洢���ؿؼ���ֵ
*    @param none
*    
*    @ע��
*        -0->ƽ������
*        -1->��ѡ�����
*        -2->ѡ�����̶�����
*/
enum Switch_space_Obj
{
    SwitchSpace_SmoothAnimation = 0, // ��������flag��Ҫ��HugoUI_Control��ʹ�ã����鲻Ҫ�Ķ�����don't change this one!!!��
    SwitchSpace_OptionStripFixedLength,
    CheckBox1,
    CheckBox2,
    CheckBox3,
    Switch1,
    Switch2,
    Switch3,
    BeeperEnableConfig,
    PageOnlyListModeConfig,

};

void HugoUI_InitLayout(void);

/* Event */
void EventUpdateOledLightLevel(void);
void EventBeeperEnableConfig(void);
void EventBeeperVolumeControl(void);

void EventShowGyroUI(void);
void EventShowAboutUI(void);
void EventPulseOximeter(void);
void EventTreasureBox(void);
void EventChartTest(void);
#endif
