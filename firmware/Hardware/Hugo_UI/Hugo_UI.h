#ifndef __HUGO_UI_H
#define __HUGO_UI_H

#ifdef __cplusplus
extern "C"
{
#endif

/*User_Add*/
/*����ؼ��ֲ�ʶ������*/
#include "gd32f30x.h"                   // Device header
#include "oled.h"
#include "OLED_User.h"
#include "DataType.h"
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

/* ������� */
#define SCREEN_WIDTH            128
#define SCREEN_HEIGHT           64
#define FONT_WIDTH              7
#define FONT_HEIGHT             13
#define SCROLL_BAR_WIDTH        3


/* USER_NEED_TO_FILL_THIESE_FUNCTION */
#define HugoUIScreenInit()                                      (OLED_Init())
#define HugoUIDisplayStr(x, y, str)                             (Oled_u8g2_ShowStr(x, y, str))
#define HugoUIDisplayFloat(x, y, dat, num, pointNum)            (Oled_u8g2_ShowFloat(x, y, dat, num, pointNum))
#define HugoUIDrawDot(x, y)                                     (Oled_u8g2_DrawPoint(x, y))
#define HugoUIDrawBox(x, y, width, height)                      (Oled_u8g2_DrawBox(x, y, width, height))
#define HugoUIDrawFrame(x, y, width, height)                    (Oled_u8g2_DrawFrame(x, y, width, height))
#define HugoUIDrawRFrame(x, y, width, height, r)                (Oled_u8g2_DrawRFrame(x, y, width, height, r))
#define HugoUIDrawRBox(x, y, width, height, r)                  (Oled_u8g2_DrawRBox(x, y, width, height, r))
#define HugoUIClearBuffer()                                     (Oled_u8g2_ClearBuffer())
#define HugoUISendBuffer()                                      (Oled_u8g2_SendBuffer())
#define HugoUISetDrawColor(mode)                                (Oled_u8g2_SetDrawColor(mode))
#define HugoUIDisplayBMP(x, y, width, height, pic)              (Oled_u8g2_ShowBMP(x, y, width, height, pic))
#define HugoUIModifyColor(mode)                                 (Oled_u8g2_ModifyColor(mode))


#ifdef FPU
// typedef     double      paramType;
typedef     float      paramType;//��ʹ��int32_t
#else
typedef     int32_t     paramType;//��ʹ��int32_t
#endif

/* UI_STATE ������ǰUI����״̬ */
typedef enum
{
    STATE_NONE = 0, // ��ǰ��״̬
    STATE_RUN_PAGE_DOWN, // ��ǰ�������в˵�
    STATE_RUN_PAGE_UP,
    STATE_READY_TO_JUMP_PAGE, // ׼����ת״̬ �ڴ�״̬������ת����
    STATE_JUMP_PAGE, // ��ǰ����ҳ����ת
    STATE_JUMP_PAGE_ARRIVE, // ��ǰҳ����ת����
}HugoUIState_e;


/* Item��functype���� */
typedef enum
{
    ITEM_PAGE_DESCRIPTION,//��ҳ������
    ITEM_CALL_FUNCTION,//�˹Ҽ��򿪾��Ǻ���
    ITEM_JUMP_PAGE,//��ת����һҳor��һҳ
    ITEM_SWITCH,//����
    ITEM_CHANGE_VALUE,//�ı�ֵ
    ITEM_PROGRESS_BAR,//�����
    ITEM_RADIO_BUTTON,//���Ͱ�ť//on&off
    ITEM_CHECKBOX,
    ITEM_MESSAGE,
} HugoUIItem_e;

/* Page��functype���� */
typedef enum
{
    PAGE_LIST,//��ҳ���б�
    PAGE_CUSTOM,//��ҳ�Ƕ�����(icon)
} HugoUIPage_e;


/* Item�Ľṹ�� */
typedef struct HugoUI_item//С�Ҽ��Ľṹ��
{
    struct HugoUI_item *next;
    HugoUIItem_e funcType;//��������
    uint8_t itemId;//С�Ҽ���id
    uint8_t lineId;//��ÿһҳ��id
    int16_t x, x_trg;
    int16_t y, y_trg;
    uint8_t *pic;
    uint8_t step;
    char *title;//С�Ҽ�������
    char *msg;                                  // ITEM_MESSAGE
    char *desc;
    bool *flag;                                 // ITEM_CHECKBOX and ITEM_RADIO_BUTTON and ITEM_SWITCH //С�Ҽ�������Щ����ʱ�ı�־λ
    bool flagDefault;                           // Factory default setting//�ָ���������
    paramType *param;                           // ITEM_CHANGE_VALUE and ITEM_PROGRESS_BAR//С�Ҽ��ɸı�Ĳ���
    uint8_t inPage;                             // ITEM_JUMP_PAGE//Item����һҳ
    uint8_t JumpPage;                           //��Ҫ��ת����һ��page
    uint8_t JumpItem;                           //��Ҫ��ת����һ��Item
    void (*CallBack)(void);                    //�ص�����// ITEM_CHANGE_VALUE and ITEM_PROGRESS_BAR//�ùҼ��ĺ���
    struct HugoUI_item * (*SetIconSrc)(const uint8_t *pic);//����ͼƬ
    struct HugoUI_item * (*SetJumpId)(uint8_t pageId, uint8_t itemLineId);//����PageId��ItemLineId
    struct HugoUI_item * (*SetDescripition)(char *desc);//����descripition
    struct HugoUI_item * (*ReturnThisItem)(struct HugoUI_item *thisItem);//����descripition
} HugoUIItem_t;


/* Page�Ľṹ�� */
typedef struct HugoUI_page
{
    struct HugoUI_page *next;
    HugoUIPage_e funcType;
    HugoUIItem_t *itemHead, *itemTail;
    char *title;//��ҳ������
    uint8_t pageId;
    uint8_t itemMax; // ��page���е�item��
    int16_t page_x, page_x_trg;
    int16_t page_y, page_y_trg;
    int16_t page_y_forlist, page_y_forlist_trg;
    HugoUIItem_t* (*AddItem)(struct HugoUI_page *thisPage, char *title, HugoUIItem_e itemType, ...);
    
    void (*CallBack)(void); 
} HugoUIPage_t;


/* API-Function */
void HugoUI_System(void);

void HugoUI_Control(void);

void HugoUI_JumpPage_ShowLastPage(HugoUIPage_t *lastPage);

HugoUIItem_t* ReturnThisItem(HugoUIItem_t* thisItem);

HugoUIItem_t* SetJumpId(uint8_t pageId, uint8_t itemLineId);

HugoUIItem_t* SetIconSrc(const uint8_t *pic);

HugoUIItem_t* SetDescripition(char *desc);

HugoUIItem_t* AddItem(HugoUIPage_t *thisPage, char* title, HugoUIItem_e itemType, ...);

HugoUIPage_t* AddPage(HugoUIPage_e mode, char *name);

uint8_t HugoUI_Blur_Effect(void);

uint16_t findParam_forChangeVal(float *param);

uint8_t HugoUI_Run(short *a, short *a_trg, u8 step, u8 slow_cnt);

/* ��user.c������ö��enum Slide_space_Obj ��ö����ÿһ��ö������˳���ӦSlide_space�ṹ�����鼴��*/

/**
*	@���� Slide_Bar
*	@����
*		x    ֵ
*		min  ��Сֵ
*		max  ���ֵ
*		step ����
*/
struct Slide_Bar
{
    float* val;     //ֵ
    float min;
    float max;
    float step;
};

extern struct Slide_Bar Slide_space[];


/* �������� */
extern char *HugoUIVersion;
extern HugoUIPage_t *pageHead, *pageTail;
extern HugoUIItem_t *ItemHead, *ItemTail;

extern uint8_t KeyNum;	
extern uint8_t EncoderNum;	


#ifdef __cplusplus
}
#endif

#endif
