#include "Hugo_UI.h"
#include "Malloc.h"
#include "UART0.h"
#include "Key.h"
#include "HugoUI_User.h"
#include "u8g2.h"

/******************************�ؼ����������******************************/
/****************************best not to change****************************/

/*����ָ��Page�ṹ��ָ���ͷβָ��*/
HugoUIPage_t* pageHead = NULL;
HugoUIPage_t* pageTail = NULL;

HugoUIItem_t* ItemHead = NULL;
HugoUIItem_t* ItemTail = NULL;

/* ������HugoUI_Control Func�еĹؼ��������� */
HugoUIPage_t *currentPage; // ����ui_index��������ǰPage
HugoUIPage_t *lastPage; // JumpPageʱ����һ����һ��Page
HugoUIItem_t *currentItem; // ����ui_select��������ǰItem

/* ��¼��ǰPage��Item�Ĺؼ����� */
static int16_t ui_index = 0, ui_select = 0, ui_state = STATE_NONE;

static uint8_t jumpPage_flag = 0;
static uint8_t ChangeVal_flag = 0;

int16_t frame_y, frame_y_trg;
int16_t frame_x = 0, frame_x_trg = 0;
int16_t frame_width, frame_width_trg;
int16_t slidbar_y, slidbar_y_trg;

int16_t icon_move_x = 0, icon_move_x_trg = 48;
uint8_t page_x_step = 12;
int16_t icon_desc_y = 0, icon_desc_y_trg = 24;
int16_t list_move_y = 0, list_move_y_trg = FONT_HEIGHT;
int16_t icon_rectangle_x = 0, icon_rectangle_x_trg = 13;

uint8_t ui_Key_num, ui_Encoder_num; // user���Խ��Լ���ʵ�ֺ����ı���ֱ�Ӹ�ֵ��������Num

uint8_t disappear_flag = 0;

int16_t Blur_Effect_temp = 0; // UIģ��ת����Ч

uint8_t ItemUIisRunning = false;

/******************************�ؼ����������******************************/


/**
  * @brief HugoUI_Effectʵ�ֻ���Ч��
  * @param  *a			��ǰ����
  * @param	*a_trg		Ŀ������
  * @param 	step		�˶��ٶ�
  * @param	slow_cnt 	����
  * @retval 
  */
uint8_t HugoUI_Run(short *a, short *a_trg, u8 step, u8 slow_cnt)
{
	
	if (*a == *a_trg)
	{
		return 0;
	}
	else if(*a < *a_trg)
	{
		u8 temp;
		temp = abs(*a_trg - *a) > slow_cnt ? step: 1;
		*a += temp;
	}
	else if(*a > *a_trg)
	{
		u8 temp;
		temp = abs(*a_trg - *a) > slow_cnt ? step: 1;
		*a -= temp;
	}
	
	return 1;
}


/* �øı�ֵitem��*Paramȥ����Slide_space�ṹ������ */
/**
  * @brief findParam_forChangeVal
  * @brief �ҵ��͵�ǰParam��Ӧ��Slide_space�ṹ��
  * @param  float *param ��ǰitem��*Param
  * @retval ��ӦSlide_space�ṹ����Slide_space�ṹ�������λ��
  */
uint16_t findParam_forChangeVal(float *param)
{
    uint16_t i = 0;
    for(i = 0; i < Slide_space_Obj_Max; i++)
    {
        if(param == Slide_space[i].val)
        {
            return i;
        }
    }
    return 0;
}



HugoUIItem_t* ReturnThisItem(HugoUIItem_t* thisItem)
{
    thisItem = pageTail->itemTail;

    return thisItem;
}


HugoUIItem_t* SetJumpId(uint8_t pageId, uint8_t itemLineId)
{
    pageTail->itemTail->JumpPage = pageId;
    pageTail->itemTail->JumpItem = itemLineId;

    // printf("thisitemid:%d ", pageTail->itemTail->itemId);
    return pageTail->itemTail;
}


HugoUIItem_t* SetIconSrc(const uint8_t *pic)
{
    pageTail->itemTail->pic = (uint8_t *)pic;
    return pageTail->itemTail;
}


HugoUIItem_t* SetDescripition(char *desc)
{
    pageTail->itemTail->desc = desc;
    return pageTail->itemTail;
}



/**
* @brief   HugoUI AddItem
* @param   thisPage         �����itemҪ���ڵ�page
* @param   itemType         item����������
* @param   ...              ����item������������ѡ����Ӧ�Ĳ���
* @return  void
*/
HugoUIItem_t* AddItem(HugoUIPage_t *thisPage, char* title, HugoUIItem_e itemType, ...)
{
    /*��ʼ��Item�ṹ�壬�������ڴ�*/
    HugoUIItem_t* ItemAdd = mymalloc(SRAMIN, sizeof(HugoUIItem_t));

    ItemAdd->lineId = 0;
    ItemAdd->funcType = itemType;//��Item������������
    ItemAdd->title = title;//��Item����
    ItemAdd->inPage = thisPage->pageId;//���Item����һҳ
    ItemAdd->next = NULL;
    ItemAdd->y = 0;
    ItemAdd->y_trg = 0;


    ItemAdd->SetIconSrc = SetIconSrc; // ��ʼ��item�Ļص�����
    ItemAdd->SetDescripition = SetDescripition;
    ItemAdd->ReturnThisItem = ReturnThisItem;
    ItemAdd->SetJumpId = NULL;
    ItemAdd->CallBack = NULL;

    /* ��page��βָ��ָ����һҳ �ǳ���Ҫ Important!!! */
    /* ���������Ϊ����UI��ʼ�������е�item�ṹ��ص���������ָ->->->��׼�� */
    pageTail = thisPage;

    /*��Itemͷָ�븳ֵ*/
    /*��������item�е�˳��itemid*/
    if(ItemHead == NULL)
    {
        ItemAdd->itemId = 0;
        ItemHead = ItemAdd;
        ItemTail = ItemAdd;
        
    }else//����ʼβ������
    {   
        ItemAdd->itemId = ItemTail->itemId + 1;
        ItemTail->next = ItemAdd;
        ItemTail = ItemTail->next;
    }

    /*��thisPage��itemͷָ�븳ֵ*/
    /*������thisPage�е�item˳��lineid*/
    if(thisPage->itemHead == NULL)
    {   
        ItemAdd->lineId = 0;
        thisPage->itemHead = ItemAdd;
        thisPage->itemTail = ItemAdd;
    }
    else
    {
        ItemAdd->lineId = thisPage->itemTail->lineId + 1;

        thisPage->itemMax = ItemAdd->lineId;

        thisPage->itemTail->next = ItemAdd;
        thisPage->itemTail = thisPage->itemTail->next; 
    }

    printf("thislineIdInAddItem:%d    ", thisPage->itemTail->itemId);

    va_list variableArg;            //����һ���ɱ�����б�
    va_start(variableArg, itemType);//�����ö�ٸ�ֵ���ɱ�����б�
 
    switch (ItemAdd->funcType)
    {
        case ITEM_JUMP_PAGE:
                ItemAdd->SetJumpId = SetJumpId;  //��jumpId�����������item
            break;
        case ITEM_CHECKBOX:
        case ITEM_RADIO_BUTTON:
        case ITEM_SWITCH:
            ItemAdd->flag = va_arg(variableArg, bool *);
            ItemAdd->flagDefault = *ItemAdd->flag;
            ItemAdd->CallBack = va_arg(variableArg, void (*)(void)); 
            break;
        case ITEM_PROGRESS_BAR:
        case ITEM_CHANGE_VALUE:
            ItemAdd->param = va_arg(variableArg, paramType *);
            ItemAdd->CallBack = va_arg(variableArg, void (*)(void));   //��������Ӧ���Ǵ���һ��item�ĺ���
            break;
        case ITEM_MESSAGE:
            ItemAdd->msg = va_arg(variableArg, char *);//massage������
            // ItemAdd->CallBack = va_arg(variableArg, void (*)(void));   //����Ϊɶ��Ҫ���뺯��ѽ
        case ITEM_CALL_FUNCTION:
            ItemAdd->CallBack = va_arg(variableArg, void (*)(void)); 
        default:
            break;
    }
    va_end(variableArg);//�رտɱ�����б�

    return ItemAdd;
}


/**
* @brief   HugoUI AddPage
* @param   mode             List/Custom
* @param   name             PageName
* @return  HugoUIPage_t*    ����һ��Page�ṹ��ָ��
*/
HugoUIPage_t* AddPage(HugoUIPage_e mode, char *name)
{
    /*��ʼ���ṹ��,�������ڴ�*/
    HugoUIPage_t *pageAdd = mymalloc(SRAMIN, sizeof(HugoUIPage_t));

    pageAdd->page_x = 0; pageAdd->page_x_trg = 0;
    pageAdd->page_y = 0; pageAdd->page_y_trg = 0;
    pageAdd->page_y_forlist = 0; pageAdd->page_y_forlist_trg = 0;

    pageAdd->AddItem = AddItem;
    pageAdd->pageId = 0;
    pageAdd->itemMax = 0;
    pageAdd->itemHead = NULL;
    pageAdd->itemTail = NULL;
    pageAdd->next = NULL;

    /*����page���蹦�ܺ�����*/
    pageAdd->funcType = mode;
    pageAdd->title = name;

    /*��Pageͷָ�븳ֵ*//*����������page�е�˳��pageid*/
    if(pageHead == NULL)
    {
        pageAdd->pageId = 0;
        pageHead = pageAdd;
        pageTail = pageAdd;
        
    }else//����ʼβ������
    {   
        pageAdd->pageId = pageTail->pageId + 1;
        pageTail->next = pageAdd;
        pageTail = pageTail->next;
    }

    printf("thispageidInAddPage:%d    ", pageTail->pageId);

    /*����Page�ṹ��ָ��*/
    return pageAdd;

}




void HugoUI_JumpPage_ShowLastPage(HugoUIPage_t *lastPage)
{

    if(lastPage->funcType == PAGE_LIST)
    {  

        //����Ŀ¼����Ŀ¼��
        for(HugoUIItem_t *item = lastPage->itemHead; (item->lineId <= lastPage->itemMax) && (item->inPage == lastPage->pageId); item = item->next)
        {

            int16_t Item_x = lastPage->page_x;
            int16_t Item_y = 12 + lastPage->page_y + item->lineId*13 + lastPage->page_y_forlist;
            switch (item->funcType)
            {
                case ITEM_PAGE_DESCRIPTION: // 12�ǵ������� // ����¼ӵ�ͳͳ���Ǹ������΢��
                    HugoUIDisplayStr(2 + Item_x, Item_y, "-");
                    HugoUIDisplayStr(2 + 9 + Item_x, Item_y, item->title);
                    break;
                case ITEM_JUMP_PAGE:
                    HugoUIDisplayStr(2 + Item_x, Item_y, "+");
                    HugoUIDisplayStr(2 + 10 + Item_x, Item_y, item->title);
                    break;
                case ITEM_CHECKBOX:
                case ITEM_RADIO_BUTTON:
                case ITEM_SWITCH:
                case ITEM_PROGRESS_BAR:
                case ITEM_CHANGE_VALUE:
                    HugoUIDisplayStr(2 + Item_x, Item_y, "-");
                    HugoUIDisplayStr(2 + 9 + Item_x, Item_y, item->title);
                    break;
                default:
                    HugoUIDisplayStr(2 + Item_x, Item_y, "-");
                    HugoUIDisplayStr(2 + 9 + Item_x, Item_y, item->title);
                    break;
            }

            // ���ƻ�����bar
            if(item->lineId % 2 == 0)
            {
                Oled_u8g2_DrawLine(lastPage->page_x + 125, lastPage->page_y + item->lineId*(SCREEN_HEIGHT / lastPage->itemMax),
                                 lastPage->page_x + 127, lastPage->page_y +  item->lineId*(SCREEN_HEIGHT / lastPage->itemMax));             
            }
            else
            {
               Oled_u8g2_DrawLine(lastPage->page_x + 125, lastPage->page_y + item->lineId*(SCREEN_HEIGHT / lastPage->itemMax),
                                 lastPage->page_x + 126, lastPage->page_y +  item->lineId*(SCREEN_HEIGHT / lastPage->itemMax));             
            } 
        }

        // ����frameBox��ɫѡ���
        HugoUISetDrawColor(2);
        HugoUIDrawRBox(lastPage->page_x, lastPage->page_y + frame_y, frame_width + 5, 15, 0);
        HugoUISetDrawColor(1);

        // ���ƻ�����slidbar(func todo)
        Oled_u8g2_DrawLine(126, lastPage->page_y, 126, lastPage->page_y + SCREEN_HEIGHT - 1);
        Oled_u8g2_DrawBox(lastPage->page_x + 125, lastPage->page_y + slidbar_y, 3, 5);
    }
    else
    {

    }

}



/**
  * @brief uiģ��ת��Ч��
  * @param  none
  * @retval 0-δ���/1-�����
  */
uint8_t HugoUI_Blur_Effect(void)
{
	int len = 8 * Oled_u8g2_GetBufferTilHeight() * Oled_u8g2_GetBufferTileWidth();
	uint8_t *p = Oled_u8g2_GetBufferPtr();
	uint8_t return_flag = 0;
	
	for(int i = 0; i < len; i++)
	{
		if(i%2==0)
		p[i] = p[i] & (0x55);
	}
	if(Blur_Effect_temp >= 2)
	{
		for(int i = 0; i < len; i++)
		{
			if(i%2!=0)
			p[i] = p[i] & (0xaa);
		}
	}
	if(Blur_Effect_temp >= 5)
	{
		for(int i = 0; i < len; i++)
		{
			if(i%2==0)
			p[i] = p[i] & (0x00);
		}
	}

	Blur_Effect_temp += 1;
	if(Blur_Effect_temp > 10)
	{
		return_flag = 1;
		Blur_Effect_temp = 0;
	}
	return return_flag ? 0 : 1;
}





/* UI�����ƺ��� */
/**
  * @brief HugoUI_Control
  * @brief ��Ⱦ��ǰPage
  * @param  none
  * @retval none
  */
void HugoUI_Control(void)
{   

    // ���㻺������
    if (*Switch_space[SwitchSpace_SmoothAnimation]) // ����������־λ ������HugoUI_User.c��(in HugoUI_User.c)
    {   
        //����ǰPageû�п���ͼ�껯��ʹ����ͨ�ı�list��ģʽ������Ⱦ��ʾ || ������PageOnlyList��Page2List����־λ
        if(currentPage->funcType == PAGE_LIST || *Switch_space[PageOnlyListModeConfig])
        {
            HugoUI_Run(&currentPage->page_y_forlist ,&currentPage->page_y_forlist_trg, 4, 6);
            HugoUI_Run(&frame_y, &frame_y_trg, 4, 6);
            HugoUI_Run(&frame_width, &frame_width_trg, 8, 4);
            HugoUI_Run(&slidbar_y, &slidbar_y_trg, 3, 5);
//            HugoUI_Run(&list_move_y, &list_move_y_trg, 1, 8);
        }
        else if(currentPage->funcType == PAGE_CUSTOM)
        {
            HugoUI_Run(&currentPage->page_x, &currentPage->page_x_trg, page_x_step, 4);
            HugoUI_Run(&frame_y, &frame_y_trg, 8, 4);
            HugoUI_Run(&frame_x, &frame_x_trg, 8, 10);
            HugoUI_Run(&icon_move_x, &icon_move_x_trg, 4, 6);
			HugoUI_Run(&icon_rectangle_x, &icon_rectangle_x_trg, 4, 5);
        }
    }
    else // ����������־λΪFalse��û�л���Ч��
    {
        if(currentPage->funcType == PAGE_LIST || *Switch_space[PageOnlyListModeConfig])
        {
            currentPage->page_y_forlist = currentPage->page_y_forlist_trg;
            frame_y = frame_y_trg;
            frame_width = frame_width_trg;
            slidbar_y = slidbar_y_trg;
            list_move_y = list_move_y_trg;
        }
        else if(currentPage->funcType == PAGE_CUSTOM)
        {
            currentPage->page_x = currentPage->page_x_trg;
            frame_y = frame_y_trg;
            frame_x = frame_x_trg;
            icon_move_x = icon_move_x_trg;
        }
    }

    HugoUIClearBuffer(); // u8g2_ScreenClear

    //����ǰPageû�п���ͼ�껯��ʹ����ͨ�ı�list��ģʽ������Ⱦ��ʾ || ������PageOnlyList��Page2List����־λ
    if(currentPage->funcType == PAGE_LIST || *Switch_space[PageOnlyListModeConfig])
    {  
        //����Ŀ¼����Ŀ¼��
		int16_t Item_x = currentPage->page_x;
		int16_t Item_y;
        for(HugoUIItem_t *item = currentPage->itemHead; (item->lineId <= currentPage->itemMax) && (item->inPage == currentPage->pageId); item = item->next)
        {
			Item_y = 12 + currentPage->page_y + item->lineId*FONT_HEIGHT + currentPage->page_y_forlist;
            switch (item->funcType)
            {   
                // ��ҳ������
                case ITEM_PAGE_DESCRIPTION: // 12�ǵ������� // ����¼ӵ�ͳͳ���Ǹ������΢��
                    HugoUIDisplayStr(2 + Item_x, Item_y, "-");
                    HugoUIDisplayStr(2 + 9 + Item_x, Item_y, item->title);
                    break;
                // ҳ����ת
                case ITEM_JUMP_PAGE:
                    HugoUIDisplayStr(2 + Item_x, Item_y, "+");
                    HugoUIDisplayStr(2 + 10 + Item_x, Item_y, item->title);
                    break;
                // ��ѡ��
                case ITEM_CHECKBOX:
                    HugoUIDisplayStr(2 + Item_x, Item_y, "-");
                    HugoUIDisplayStr(2 + 9 + Item_x, Item_y, item->title);

                    // �ж�flag�����ڵ�*���
                    if(*item->flag == true)
                    {
                        HugoUIDisplayStr(SCREEN_WIDTH - 18, Item_y + 2, "*");
                    }

                    // ����ѡ��
                    HugoUIDrawFrame(SCREEN_WIDTH - 20, Item_y - 12 + 3, 10, 10);

                    // ��ǰѡ�����
                    if(item->lineId == ui_select)
                    {
                        HugoUISetDrawColor(2);
                        HugoUIDrawBox(SCREEN_WIDTH - 21, Item_y - 12 + 2, 12, 12);
                        HugoUISetDrawColor(1);
                    }
                    break;
                
                case ITEM_RADIO_BUTTON:
                // ���ؿؼ�
                case ITEM_SWITCH:
                    HugoUIDisplayStr(2 + Item_x, Item_y, "-");
                    HugoUIDisplayStr(2 + 9 + Item_x, Item_y, item->title);

                    // �ڷ� on/off��λ��
                    HugoUIDisplayStr(*item->flag == true ? SCREEN_WIDTH - FONT_WIDTH*3 : SCREEN_WIDTH - FONT_WIDTH*4, Item_y, *item->flag == true ? "On" : "Off");

                    break;
                
                case ITEM_PROGRESS_BAR:
                //�ı�ֵ
                case ITEM_CHANGE_VALUE:
                    HugoUIDisplayStr(2 + Item_x, Item_y, "-");
                    HugoUIDisplayStr(2 + 9 + Item_x, Item_y, item->title);
                    // ��ӡ������ �жϸø������������������
                    HugoUIDisplayFloat(*item->param < 0 ? SCREEN_WIDTH - FONT_WIDTH*7 : SCREEN_WIDTH - FONT_WIDTH*5, Item_y, *item->param, 2, 2);
                    
                    // ��ǰ���ָ��� // ps:�����Ҫ��˸Ч����Ҫ��ȡ��ʱ����ʱ��
                    if(item->lineId == ui_select && ChangeVal_flag)
                    {   // �жϸø������������������
                        HugoUISetDrawColor(2);
                        HugoUIDrawBox(*item->param < 0 ? SCREEN_WIDTH - FONT_WIDTH*7 : SCREEN_WIDTH - FONT_WIDTH*5, Item_y - 12 + 2, *item->param < 0 ? FONT_WIDTH*6 : FONT_WIDTH*4, 11);
                        HugoUISetDrawColor(1);
                    }
                    break;

                default:
                    HugoUIDisplayStr(2 + Item_x, Item_y, "-");
                    HugoUIDisplayStr(2 + 9 + Item_x, Item_y, item->title);
                    break;
            }

            // ���ƻ�����bar
            if(item->lineId%2 == 0)
            {
                Oled_u8g2_DrawLine(currentPage->page_x + 125, currentPage->page_y + item->lineId*(SCREEN_HEIGHT / currentPage->itemMax),
                                 currentPage->page_x + 127, currentPage->page_y +  item->lineId*(SCREEN_HEIGHT / currentPage->itemMax));             
            }
            else
            {
               Oled_u8g2_DrawLine(currentPage->page_x + 125, currentPage->page_y + item->lineId*(SCREEN_HEIGHT / currentPage->itemMax),
                                 currentPage->page_x + 126, currentPage->page_y +  item->lineId*(SCREEN_HEIGHT / currentPage->itemMax));             
            } 
        }

        // ����frameBox��ɫѡ���
        HugoUISetDrawColor(2);
        HugoUIDrawRBox(currentPage->page_x, currentPage->page_y + frame_y, frame_width + 5, 15, 0);
        HugoUISetDrawColor(1);

        // ���ƻ�����slidbar(func todo)
        Oled_u8g2_DrawLine(126, currentPage->page_y, 126, currentPage->page_y + SCREEN_HEIGHT - 1);
        // ���ƻ�������Ļ������box
        Oled_u8g2_DrawBox(currentPage->page_x + 125, currentPage->page_y + slidbar_y, 3, SCREEN_HEIGHT / currentPage->itemMax);


        // ��Ŀ��������
        switch (ui_state)
        {
            case STATE_NONE: // ui��״̬
                frame_width_trg = Oled_u8g2_Get_UTF8_ASCII_PixLen(currentItem->title) + FONT_WIDTH;
                slidbar_y_trg = ui_select*(SCREEN_HEIGHT / currentPage->itemMax);
                break;

            case STATE_RUN_PAGE_DOWN: // ui��page���¹���
                // �жϸ����¹�������
                // ͨ�ù���˳�� // ListMode1
                
				if(ui_select >= SCREEN_HEIGHT/16)
					currentPage->page_y_forlist_trg -= FONT_HEIGHT;
				if(ui_select == 0) currentPage->page_y_forlist_trg = 0;

				
				if(ui_select < SCREEN_HEIGHT/16)
				{
					frame_y_trg += FONT_HEIGHT;
				}
				if(ui_select == 0) frame_y_trg = 0; // ��λ
				frame_y = frame_y_trg - FONT_HEIGHT*2;
                

                frame_width_trg = Oled_u8g2_Get_UTF8_ASCII_PixLen(currentItem->title) + FONT_WIDTH;
                slidbar_y_trg = ui_select*(SCREEN_HEIGHT / currentPage->itemMax);

                ui_state = STATE_NONE;
                break;
            case STATE_RUN_PAGE_UP: // ui ��page���Ϲ���

                if(frame_y_trg == 0)
					currentPage->page_y_forlist_trg = -ui_select*FONT_HEIGHT;
                if(ui_select == 0) currentPage->page_y_forlist_trg = 0;

                frame_y = frame_y_trg + FONT_HEIGHT*2;

				frame_y_trg -= FONT_HEIGHT;
				if(frame_y_trg <= 0) frame_y_trg = 0;
                
                frame_width_trg = Oled_u8g2_Get_UTF8_ASCII_PixLen(currentItem->title) + FONT_WIDTH;
                slidbar_y_trg = ui_select*(SCREEN_HEIGHT / currentPage->itemMax);

                disappear_flag = 1; // ��ʧeffect_flag

                ui_state = STATE_NONE;
                break;

            case STATE_READY_TO_JUMP_PAGE: // ui ׼����תҳ��
                // lastPage->page_y_trg = -40;
                currentPage->page_y_trg = 0;
                currentPage->page_y = 50;
                
                icon_move_x = 0; //������ת��custom���������һ����λ��Ч

                frame_y = SCREEN_HEIGHT*2;
				
				slidbar_y = SCREEN_HEIGHT;
			
                ui_state = STATE_JUMP_PAGE;

                // �жϵ�ǰlist��λ�� // ��ʱ�������������
                if((ui_select % (SCREEN_HEIGHT/16)) - 1) 
                {
                    if(currentPage->page_y_forlist_trg/FONT_HEIGHT == -(ui_select - 1))
                        currentPage->page_y_forlist_trg -= FONT_HEIGHT;
                    else
                        currentPage->page_y_forlist_trg -= FONT_HEIGHT*(SCREEN_HEIGHT/16);
                }

                if(ui_select == 0) currentPage->page_y_forlist_trg = 0;

                break;

            case STATE_JUMP_PAGE: // Ŀǰ����ֻ��ʾ��ǰҳ������ // ui ������תҳ��
                if(HugoUI_Run(&currentPage->page_y, &currentPage->page_y_trg, 4, 5) == 0)
                {
                    jumpPage_flag |= 0xf0;
                }
                if(HugoUI_Run(&lastPage->page_y, &lastPage->page_y_trg, 8, 8) == 0)
                {
                    jumpPage_flag |= 0x0f;
                }
                if(jumpPage_flag == 0xff)
                {
                    jumpPage_flag = 0;
                    ui_state = STATE_JUMP_PAGE_ARRIVE;
                }
                
                break;

            case STATE_JUMP_PAGE_ARRIVE: // ui��תҳ�����
                frame_y_trg = (ui_select % (SCREEN_HEIGHT/16)) * 13;
                frame_width_trg = Oled_u8g2_Get_UTF8_ASCII_PixLen(currentItem->title) + FONT_WIDTH;
                slidbar_y_trg = ui_select*(SCREEN_HEIGHT / currentPage->itemMax);

                ui_state = STATE_NONE;
                break;
            default:
                break;
        }
    }
    // ����ʹ��ͼ�λ�ģʽ��ȾUI
    else
    {
        //����Ŀ¼����Ŀ¼��
		int16_t Item_y = currentPage->page_y;
		int16_t Item_x;
        for(HugoUIItem_t *item = currentPage->itemHead; (item->lineId <= currentPage->itemMax) && (item->inPage == currentPage->pageId); item = item->next)
        {

            Item_x = 48 + currentPage->page_x + icon_move_x*item->lineId;
            
            HugoUIDisplayBMP(Item_x, Item_y, 32, 32, item->pic);
			
        }
		
		Oled_u8g2_SetFont(u8g2_font_luBS14_tr);
		HugoUIDisplayStr((128 - Oled_u8g2_Get_UTF8_ASCII_PixLen(currentItem->title))/2,
			Item_y + SCREEN_HEIGHT/2 + 22 + (FONT_HEIGHT - icon_rectangle_x), currentItem->title); // ������ʾ��Ŀ��
		Oled_u8g2_SetFont(u8g2_font_profont15_mr);

        
        HugoUISetDrawColor(2);
		// ����frameBoxѡ���
        HugoUIDrawRBox(48 + frame_x, currentPage->page_y, 32, 32, 0);
		// ����icon_rectangle
		HugoUIDrawBox(0, currentPage->page_y + SCREEN_HEIGHT/2 + 4, icon_rectangle_x, 24);
        HugoUISetDrawColor(1);
		

		
        // ��Ŀ��������
        switch (ui_state)
        {
            case STATE_NONE: // ui��״̬

                break;

            case STATE_RUN_PAGE_DOWN: // ui��page���¹���
                // �жϸ����¹�������
                currentPage->page_x_trg = -(ui_select*48); // �޸�ICON��xλ��
                frame_x -= 24; // frameBox�Ķ�Ч
				
				icon_rectangle_x = -4;// ��λicon_rectangle
				
                if(ui_select == 0)
				{
					page_x_step = 16;
					icon_move_x = 0;
				}else page_x_step = 12;

                ui_state = STATE_NONE;
                break;
            case STATE_RUN_PAGE_UP: // ui ��page���Ϲ���
                // �жϸ����Ϲ�������
                currentPage->page_x_trg = -(ui_select*48);
                if(ui_select != 0) frame_x += 24;
				
				icon_rectangle_x = -4;// ��λicon_rectangle
			
                ui_state = STATE_NONE;
                break;

            case STATE_READY_TO_JUMP_PAGE: // ui ׼����תҳ��
//                lastPage->page_y_trg = -40;
                currentPage->page_y_trg = 0;
                currentPage->page_y = 50;
				
                frame_y = SCREEN_HEIGHT*2;
			
				slidbar_y = SCREEN_HEIGHT;
				
                ui_state = STATE_JUMP_PAGE;

                icon_move_x = 0;

                // �жϵ�ǰlist��λ�� // ��ʱ�������������
                if((ui_select % (SCREEN_HEIGHT/16)) - 1) 
                {
                    if(currentPage->page_y_forlist_trg/FONT_HEIGHT == -(ui_select - 1))
                        currentPage->page_y_forlist_trg -= FONT_HEIGHT;
                    else
                        currentPage->page_y_forlist_trg -= FONT_HEIGHT*(SCREEN_HEIGHT/16);
                }
                if(ui_select == 0) currentPage->page_y_forlist_trg = 0;

                break;

            case STATE_JUMP_PAGE: // Ŀǰ����ֻ��ʾ��ǰҳ������ // ui ������תҳ��

                if(HugoUI_Run(&currentPage->page_y, &currentPage->page_y_trg, 4, 5) == 0)
                {
                    jumpPage_flag |= 0xf0;
                }
                if(HugoUI_Run(&lastPage->page_y, &lastPage->page_y_trg, 8, 8) == 0)
                {
                    jumpPage_flag |= 0x0f;
                }
                if(jumpPage_flag == 0xff)
                {
                    jumpPage_flag = 0;
                    ui_state = STATE_JUMP_PAGE_ARRIVE;
                }
                
                break;

            case STATE_JUMP_PAGE_ARRIVE: // ui��תҳ�����
                frame_y_trg = (ui_select % (SCREEN_HEIGHT/16)) * 13;
                frame_width_trg = Oled_u8g2_Get_UTF8_ASCII_PixLen(currentItem->title) + FONT_WIDTH;
                slidbar_y_trg = ui_select*(SCREEN_HEIGHT / currentPage->itemMax);

                ui_state = STATE_NONE;
                break;
            default:
                break;
        }

    }
    

    /* ���ð������� */
    if(ui_Encoder_num == 2)
    {   
        if(ChangeVal_flag) // ����ı�ֵ��־λ����
        {
            *currentItem->param += Slide_space[findParam_forChangeVal(currentItem->param)].step;
            // ����param��С
            if(*currentItem->param >= Slide_space[findParam_forChangeVal(currentItem->param)].max)
                *currentItem->param = Slide_space[findParam_forChangeVal(currentItem->param)].max;
            // ִ�е�ǰItem��cb����
            if(currentItem->CallBack != NULL) currentItem->CallBack();
        }
        else
        {
            ui_select++;
            ui_state = STATE_RUN_PAGE_DOWN;
            if(ui_select > currentPage->itemTail->lineId) ui_select = 0;
            printf("Enocder��ת\r\n");
        }
 
    }
    else if(ui_Encoder_num == 1)
    {   
        if(ChangeVal_flag) // ����ı�ֵ��־λ����
        {
            *currentItem->param -= Slide_space[findParam_forChangeVal(currentItem->param)].step;
            // ����param��С
            if(*currentItem->param <= Slide_space[findParam_forChangeVal(currentItem->param)].min)
                *currentItem->param = Slide_space[findParam_forChangeVal(currentItem->param)].min;
            // ִ�е�ǰItem��cb����
            if(currentItem->CallBack != NULL) currentItem->CallBack();
        }
        else
        {
            ui_select--;
            ui_state = STATE_RUN_PAGE_UP;
            if(ui_select < 1) ui_select = 0;
            printf("Enocder��ת\r\n");
        }

    }
    else if(ui_Key_num == 1)
    {
       switch(currentItem->funcType)
       {
            case ITEM_JUMP_PAGE:
                // ҳ����ת����
                ui_state = STATE_READY_TO_JUMP_PAGE; // ���ó�׼����ת״̬
                ui_index = currentItem->JumpPage;
                ui_select = currentItem->JumpItem;
                // ������һҳ����Ϣ
                lastPage = currentPage;

                // ������תֵ(��switch(ui_state)�ĺ��������)
                break;
            
            case ITEM_CALL_FUNCTION:
                ItemUIisRunning = true;
                break;

            case ITEM_CHECKBOX:
                // ��ת��item��flag
                *currentItem->flag = !*currentItem->flag;
                // ִ�и�item��cb����
                if(currentItem->CallBack != NULL) currentItem->CallBack();
                break;

            case ITEM_SWITCH:
                // ��ת��item��flag
                *currentItem->flag = !*currentItem->flag;
                // ִ�и�item��cb����
                if(currentItem->CallBack != NULL) currentItem->CallBack();
                break;

            case ITEM_CHANGE_VALUE:
                // ��תChangeVal_flag
                ChangeVal_flag = !ChangeVal_flag;

                break;

            default:
                break;
            
       }
       printf("Key����\r\n");
    }
    else if(ui_Key_num == 2)
    {
        // return Last_page or exti
        // ������һҳ�����
        if(lastPage != NULL)
        {
            ui_state = STATE_READY_TO_JUMP_PAGE; // ���ó�׼����ת״̬
            ui_index = lastPage->pageId;
            ui_select = 0;
            lastPage->page_y_forlist_trg = 0;
            lastPage->page_x_trg = 0;
            // currentPage->page_y_forlist_trg = 0;
            // currentPage->page_x_trg = 0;
            // ������һҳ����Ϣ
            lastPage = currentPage;

            ChangeVal_flag = 0; // �Ѹı�ֵ��־λ���� ����ǿ����ת ͬʱ��ֹbug
        }
        printf("Key����\r\n");
    }

    HugoUISendBuffer(); // u8g2_ScreenRefresh
}


void HugoUI_System(void)
{
    // Get ControlNum
    ui_Key_num = KeyNum;
    ui_Encoder_num = EncoderNum;

    // Get currentPage by id
    if(currentPage->pageId != ui_index)
    {
        currentPage = pageHead;
        while (currentPage->pageId != ui_index)
        {
            currentPage = currentPage->next;
        }
        printf("pageid:%d\r\n", currentPage->pageId);
    }

    // Get currentItem by id
    if(currentItem->lineId != ui_select || currentItem->inPage != ui_index)
    { 
        currentItem = currentPage->itemHead;
        while (currentItem->lineId != ui_select)
        {
            currentItem = currentItem->next;
        }
        printf("itemlid:%d\r\n", currentItem->lineId);
    }

    if(ItemUIisRunning)
    {   
        HugoUIClearBuffer();

        if(currentItem->CallBack != NULL)
            currentItem->CallBack();
        else ItemUIisRunning = false; // �����itemû������ֱ���˳� ��ֹbug

        if(ui_Key_num == 2)
        {
            ItemUIisRunning = false; // �����˳�Ӧ��
        }

        HugoUISendBuffer();
    }
    else
    {
        HugoUI_Control();
    }
	
//	printf("runUI\r\n");
}





