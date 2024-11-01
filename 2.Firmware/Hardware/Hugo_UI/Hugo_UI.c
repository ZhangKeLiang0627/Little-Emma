#include "Hugo_UI.h"
#include "Malloc.h"
#include "UART0.h"
#include "Key.h"
#include "HugoUI_User.h"
#include "u8g2.h"

/******************************关键变量存放区******************************/
/****************************best not to change****************************/

/*定义指向Page结构体指针的头尾指针*/
HugoUIPage_t* pageHead = NULL;
HugoUIPage_t* pageTail = NULL;

HugoUIItem_t* ItemHead = NULL;
HugoUIItem_t* ItemTail = NULL;

/* 以下是HugoUI_Control Func中的关键参数变量 */
HugoUIPage_t *currentPage; // 根据ui_index遍历出当前Page
HugoUIPage_t *lastPage; // JumpPage时拷贝一份上一个Page
HugoUIItem_t *currentItem; // 根据ui_select遍历出当前Item

/* 记录当前Page和Item的关键变量 */
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

uint8_t ui_Key_num, ui_Encoder_num; // user可以将自己的实现函数的变量直接赋值给这两个Num

uint8_t disappear_flag = 0;

int16_t Blur_Effect_temp = 0; // UI模糊转场动效

uint8_t ItemUIisRunning = false;

/******************************关键变量存放区******************************/


/**
  * @brief HugoUI_Effect实现滑动效果
  * @param  *a			当前坐标
  * @param	*a_trg		目标坐标
  * @param 	step		运动速度
  * @param	slow_cnt 	减速
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


/* 用改变值item的*Param去索引Slide_space结构体数组 */
/**
  * @brief findParam_forChangeVal
  * @brief 找到和当前Param对应的Slide_space结构体
  * @param  float *param 当前item的*Param
  * @retval 对应Slide_space结构体在Slide_space结构体数组的位置
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
* @param   thisPage         把这个item要放在的page
* @param   itemType         item的作用类型
* @param   ...              根据item的作用类型来选填相应的参数
* @return  void
*/
HugoUIItem_t* AddItem(HugoUIPage_t *thisPage, char* title, HugoUIItem_e itemType, ...)
{
    /*初始化Item结构体，并分配内存*/
    HugoUIItem_t* ItemAdd = mymalloc(SRAMIN, sizeof(HugoUIItem_t));

    ItemAdd->lineId = 0;
    ItemAdd->funcType = itemType;//给Item赋予作用类型
    ItemAdd->title = title;//给Item命名
    ItemAdd->inPage = thisPage->pageId;//这个Item在哪一页
    ItemAdd->next = NULL;
    ItemAdd->y = 0;
    ItemAdd->y_trg = 0;


    ItemAdd->SetIconSrc = SetIconSrc; // 初始化item的回调函数
    ItemAdd->SetDescripition = SetDescripition;
    ItemAdd->ReturnThisItem = ReturnThisItem;
    ItemAdd->SetJumpId = NULL;
    ItemAdd->CallBack = NULL;

    /* 让page的尾指针指向这一页 非常重要 Important!!! */
    /* 这个操作是为了在UI初始化函数中的item结构体回调函数连续指->->->做准备 */
    pageTail = thisPage;

    /*给Item头指针赋值*/
    /*生成所有item中的顺序itemid*/
    if(ItemHead == NULL)
    {
        ItemAdd->itemId = 0;
        ItemHead = ItemAdd;
        ItemTail = ItemAdd;
        
    }else//给开始尾插链表
    {   
        ItemAdd->itemId = ItemTail->itemId + 1;
        ItemTail->next = ItemAdd;
        ItemTail = ItemTail->next;
    }

    /*给thisPage的item头指针赋值*/
    /*生成在thisPage中的item顺序lineid*/
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

    va_list variableArg;            //创建一个可变参数列表
    va_start(variableArg, itemType);//把这个枚举赋值给可变参数列表
 
    switch (ItemAdd->funcType)
    {
        case ITEM_JUMP_PAGE:
                ItemAdd->SetJumpId = SetJumpId;  //把jumpId函数给到这个item
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
            ItemAdd->CallBack = va_arg(variableArg, void (*)(void));   //这里我想应该是传了一个item的函数
            break;
        case ITEM_MESSAGE:
            ItemAdd->msg = va_arg(variableArg, char *);//massage的内容
            // ItemAdd->CallBack = va_arg(variableArg, void (*)(void));   //这里为啥还要传入函数呀
        case ITEM_CALL_FUNCTION:
            ItemAdd->CallBack = va_arg(variableArg, void (*)(void)); 
        default:
            break;
    }
    va_end(variableArg);//关闭可变参数列表

    return ItemAdd;
}


/**
* @brief   HugoUI AddPage
* @param   mode             List/Custom
* @param   name             PageName
* @return  HugoUIPage_t*    返回一个Page结构体指针
*/
HugoUIPage_t* AddPage(HugoUIPage_e mode, char *name)
{
    /*初始化结构体,并分配内存*/
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

    /*给此page赋予功能和名字*/
    pageAdd->funcType = mode;
    pageAdd->title = name;

    /*给Page头指针赋值*//*生成在所有page中的顺序pageid*/
    if(pageHead == NULL)
    {
        pageAdd->pageId = 0;
        pageHead = pageAdd;
        pageTail = pageAdd;
        
    }else//给开始尾插链表
    {   
        pageAdd->pageId = pageTail->pageId + 1;
        pageTail->next = pageAdd;
        pageTail = pageTail->next;
    }

    printf("thispageidInAddPage:%d    ", pageTail->pageId);

    /*返回Page结构体指针*/
    return pageAdd;

}




void HugoUI_JumpPage_ShowLastPage(HugoUIPage_t *lastPage)
{

    if(lastPage->funcType == PAGE_LIST)
    {  

        //绘制目录树和目录名
        for(HugoUIItem_t *item = lastPage->itemHead; (item->lineId <= lastPage->itemMax) && (item->inPage == lastPage->pageId); item = item->next)
        {

            int16_t Item_x = lastPage->page_x;
            int16_t Item_y = 12 + lastPage->page_y + item->lineId*13 + lastPage->page_y_forlist;
            switch (item->funcType)
            {
                case ITEM_PAGE_DESCRIPTION: // 12是调整距离 // 这底下加的统统都是根据情况微调
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

            // 绘制滑动条bar
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

        // 绘制frameBox反色选择框
        HugoUISetDrawColor(2);
        HugoUIDrawRBox(lastPage->page_x, lastPage->page_y + frame_y, frame_width + 5, 15, 0);
        HugoUISetDrawColor(1);

        // 绘制滑动条slidbar(func todo)
        Oled_u8g2_DrawLine(126, lastPage->page_y, 126, lastPage->page_y + SCREEN_HEIGHT - 1);
        Oled_u8g2_DrawBox(lastPage->page_x + 125, lastPage->page_y + slidbar_y, 3, 5);
    }
    else
    {

    }

}



/**
  * @brief ui模糊转场效果
  * @param  none
  * @retval 0-未完成/1-已完成
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





/* UI主控制函数 */
/**
  * @brief HugoUI_Control
  * @brief 渲染当前Page
  * @param  none
  * @retval none
  */
void HugoUI_Control(void)
{   

    // 计算缓动动画
    if (*Switch_space[SwitchSpace_SmoothAnimation]) // 缓动动画标志位 放在了HugoUI_User.c中(in HugoUI_User.c)
    {   
        //若当前Page没有开题图标化则使用普通文本list的模式进行渲染显示 || 开启了PageOnlyList（Page2List）标志位
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
    else // 缓动动画标志位为False则没有缓动效果
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

    //若当前Page没有开题图标化则使用普通文本list的模式进行渲染显示 || 开启了PageOnlyList（Page2List）标志位
    if(currentPage->funcType == PAGE_LIST || *Switch_space[PageOnlyListModeConfig])
    {  
        //绘制目录树和目录名
		int16_t Item_x = currentPage->page_x;
		int16_t Item_y;
        for(HugoUIItem_t *item = currentPage->itemHead; (item->lineId <= currentPage->itemMax) && (item->inPage == currentPage->pageId); item = item->next)
        {
			Item_y = 12 + currentPage->page_y + item->lineId*FONT_HEIGHT + currentPage->page_y_forlist;
            switch (item->funcType)
            {   
                // 此页的描述
                case ITEM_PAGE_DESCRIPTION: // 12是调整距离 // 这底下加的统统都是根据情况微调
                    HugoUIDisplayStr(2 + Item_x, Item_y, "-");
                    HugoUIDisplayStr(2 + 9 + Item_x, Item_y, item->title);
                    break;
                // 页面跳转
                case ITEM_JUMP_PAGE:
                    HugoUIDisplayStr(2 + Item_x, Item_y, "+");
                    HugoUIDisplayStr(2 + 10 + Item_x, Item_y, item->title);
                    break;
                // 勾选框
                case ITEM_CHECKBOX:
                    HugoUIDisplayStr(2 + Item_x, Item_y, "-");
                    HugoUIDisplayStr(2 + 9 + Item_x, Item_y, item->title);

                    // 判断flag画框内的*标记
                    if(*item->flag == true)
                    {
                        HugoUIDisplayStr(SCREEN_WIDTH - 18, Item_y + 2, "*");
                    }

                    // 画单选框
                    HugoUIDrawFrame(SCREEN_WIDTH - 20, Item_y - 12 + 3, 10, 10);

                    // 当前选项高亮
                    if(item->lineId == ui_select)
                    {
                        HugoUISetDrawColor(2);
                        HugoUIDrawBox(SCREEN_WIDTH - 21, Item_y - 12 + 2, 12, 12);
                        HugoUISetDrawColor(1);
                    }
                    break;
                
                case ITEM_RADIO_BUTTON:
                // 开关控件
                case ITEM_SWITCH:
                    HugoUIDisplayStr(2 + Item_x, Item_y, "-");
                    HugoUIDisplayStr(2 + 9 + Item_x, Item_y, item->title);

                    // 摆放 on/off的位置
                    HugoUIDisplayStr(*item->flag == true ? SCREEN_WIDTH - FONT_WIDTH*3 : SCREEN_WIDTH - FONT_WIDTH*4, Item_y, *item->flag == true ? "On" : "Off");

                    break;
                
                case ITEM_PROGRESS_BAR:
                //改变值
                case ITEM_CHANGE_VALUE:
                    HugoUIDisplayStr(2 + Item_x, Item_y, "-");
                    HugoUIDisplayStr(2 + 9 + Item_x, Item_y, item->title);
                    // 打印浮点数 判断该浮点数正负来决定宽度
                    HugoUIDisplayFloat(*item->param < 0 ? SCREEN_WIDTH - FONT_WIDTH*7 : SCREEN_WIDTH - FONT_WIDTH*5, Item_y, *item->param, 2, 2);
                    
                    // 当前数字高亮 // ps:如果想要闪烁效果需要获取定时器的时间
                    if(item->lineId == ui_select && ChangeVal_flag)
                    {   // 判断该浮点数正负来决定宽度
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

            // 绘制滑动条bar
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

        // 绘制frameBox反色选择框
        HugoUISetDrawColor(2);
        HugoUIDrawRBox(currentPage->page_x, currentPage->page_y + frame_y, frame_width + 5, 15, 0);
        HugoUISetDrawColor(1);

        // 绘制滑动条slidbar(func todo)
        Oled_u8g2_DrawLine(126, currentPage->page_y, 126, currentPage->page_y + SCREEN_HEIGHT - 1);
        // 绘制滑动条里的会滚动的box
        Oled_u8g2_DrawBox(currentPage->page_x + 125, currentPage->page_y + slidbar_y, 3, SCREEN_HEIGHT / currentPage->itemMax);


        // 项目滚动处理
        switch (ui_state)
        {
            case STATE_NONE: // ui无状态
                frame_width_trg = Oled_u8g2_Get_UTF8_ASCII_PixLen(currentItem->title) + FONT_WIDTH;
                slidbar_y_trg = ui_select*(SCREEN_HEIGHT / currentPage->itemMax);
                break;

            case STATE_RUN_PAGE_DOWN: // ui该page向下滚动
                // 判断该往下滚动多少
                // 通用滚动顺序 // ListMode1
                
				if(ui_select >= SCREEN_HEIGHT/16)
					currentPage->page_y_forlist_trg -= FONT_HEIGHT;
				if(ui_select == 0) currentPage->page_y_forlist_trg = 0;

				
				if(ui_select < SCREEN_HEIGHT/16)
				{
					frame_y_trg += FONT_HEIGHT;
				}
				if(ui_select == 0) frame_y_trg = 0; // 复位
				frame_y = frame_y_trg - FONT_HEIGHT*2;
                

                frame_width_trg = Oled_u8g2_Get_UTF8_ASCII_PixLen(currentItem->title) + FONT_WIDTH;
                slidbar_y_trg = ui_select*(SCREEN_HEIGHT / currentPage->itemMax);

                ui_state = STATE_NONE;
                break;
            case STATE_RUN_PAGE_UP: // ui 该page向上滚动

                if(frame_y_trg == 0)
					currentPage->page_y_forlist_trg = -ui_select*FONT_HEIGHT;
                if(ui_select == 0) currentPage->page_y_forlist_trg = 0;

                frame_y = frame_y_trg + FONT_HEIGHT*2;

				frame_y_trg -= FONT_HEIGHT;
				if(frame_y_trg <= 0) frame_y_trg = 0;
                
                frame_width_trg = Oled_u8g2_Get_UTF8_ASCII_PixLen(currentItem->title) + FONT_WIDTH;
                slidbar_y_trg = ui_select*(SCREEN_HEIGHT / currentPage->itemMax);

                disappear_flag = 1; // 消失effect_flag

                ui_state = STATE_NONE;
                break;

            case STATE_READY_TO_JUMP_PAGE: // ui 准备跳转页面
                // lastPage->page_y_trg = -40;
                currentPage->page_y_trg = 0;
                currentPage->page_y = 50;
                
                icon_move_x = 0; //倘若跳转到custom界面可以起到一个复位动效

                frame_y = SCREEN_HEIGHT*2;
				
				slidbar_y = SCREEN_HEIGHT;
			
                ui_state = STATE_JUMP_PAGE;

                // 判断当前list的位置 // 暂时放在这里可以用
                if((ui_select % (SCREEN_HEIGHT/16)) - 1) 
                {
                    if(currentPage->page_y_forlist_trg/FONT_HEIGHT == -(ui_select - 1))
                        currentPage->page_y_forlist_trg -= FONT_HEIGHT;
                    else
                        currentPage->page_y_forlist_trg -= FONT_HEIGHT*(SCREEN_HEIGHT/16);
                }

                if(ui_select == 0) currentPage->page_y_forlist_trg = 0;

                break;

            case STATE_JUMP_PAGE: // 目前发现只显示当前页最流畅 // ui 正在跳转页面
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

            case STATE_JUMP_PAGE_ARRIVE: // ui跳转页面完成
                frame_y_trg = (ui_select % (SCREEN_HEIGHT/16)) * 13;
                frame_width_trg = Oled_u8g2_Get_UTF8_ASCII_PixLen(currentItem->title) + FONT_WIDTH;
                slidbar_y_trg = ui_select*(SCREEN_HEIGHT / currentPage->itemMax);

                ui_state = STATE_NONE;
                break;
            default:
                break;
        }
    }
    // 否则使用图形化模式渲染UI
    else
    {
        //绘制目录树和目录名
		int16_t Item_y = currentPage->page_y;
		int16_t Item_x;
        for(HugoUIItem_t *item = currentPage->itemHead; (item->lineId <= currentPage->itemMax) && (item->inPage == currentPage->pageId); item = item->next)
        {

            Item_x = 48 + currentPage->page_x + icon_move_x*item->lineId;
            
            HugoUIDisplayBMP(Item_x, Item_y, 32, 32, item->pic);
			
        }
		
		Oled_u8g2_SetFont(u8g2_font_luBS14_tr);
		HugoUIDisplayStr((128 - Oled_u8g2_Get_UTF8_ASCII_PixLen(currentItem->title))/2,
			Item_y + SCREEN_HEIGHT/2 + 22 + (FONT_HEIGHT - icon_rectangle_x), currentItem->title); // 居中显示项目名
		Oled_u8g2_SetFont(u8g2_font_profont15_mr);

        
        HugoUISetDrawColor(2);
		// 绘制frameBox选择框
        HugoUIDrawRBox(48 + frame_x, currentPage->page_y, 32, 32, 0);
		// 绘制icon_rectangle
		HugoUIDrawBox(0, currentPage->page_y + SCREEN_HEIGHT/2 + 4, icon_rectangle_x, 24);
        HugoUISetDrawColor(1);
		

		
        // 项目滚动处理
        switch (ui_state)
        {
            case STATE_NONE: // ui无状态

                break;

            case STATE_RUN_PAGE_DOWN: // ui该page向下滚动
                // 判断该往下滚动多少
                currentPage->page_x_trg = -(ui_select*48); // 修改ICON的x位移
                frame_x -= 24; // frameBox的动效
				
				icon_rectangle_x = -4;// 复位icon_rectangle
				
                if(ui_select == 0)
				{
					page_x_step = 16;
					icon_move_x = 0;
				}else page_x_step = 12;

                ui_state = STATE_NONE;
                break;
            case STATE_RUN_PAGE_UP: // ui 该page向上滚动
                // 判断该往上滚动多少
                currentPage->page_x_trg = -(ui_select*48);
                if(ui_select != 0) frame_x += 24;
				
				icon_rectangle_x = -4;// 复位icon_rectangle
			
                ui_state = STATE_NONE;
                break;

            case STATE_READY_TO_JUMP_PAGE: // ui 准备跳转页面
//                lastPage->page_y_trg = -40;
                currentPage->page_y_trg = 0;
                currentPage->page_y = 50;
				
                frame_y = SCREEN_HEIGHT*2;
			
				slidbar_y = SCREEN_HEIGHT;
				
                ui_state = STATE_JUMP_PAGE;

                icon_move_x = 0;

                // 判断当前list的位置 // 暂时放在这里可以用
                if((ui_select % (SCREEN_HEIGHT/16)) - 1) 
                {
                    if(currentPage->page_y_forlist_trg/FONT_HEIGHT == -(ui_select - 1))
                        currentPage->page_y_forlist_trg -= FONT_HEIGHT;
                    else
                        currentPage->page_y_forlist_trg -= FONT_HEIGHT*(SCREEN_HEIGHT/16);
                }
                if(ui_select == 0) currentPage->page_y_forlist_trg = 0;

                break;

            case STATE_JUMP_PAGE: // 目前发现只显示当前页最流畅 // ui 正在跳转页面

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

            case STATE_JUMP_PAGE_ARRIVE: // ui跳转页面完成
                frame_y_trg = (ui_select % (SCREEN_HEIGHT/16)) * 13;
                frame_width_trg = Oled_u8g2_Get_UTF8_ASCII_PixLen(currentItem->title) + FONT_WIDTH;
                slidbar_y_trg = ui_select*(SCREEN_HEIGHT / currentPage->itemMax);

                ui_state = STATE_NONE;
                break;
            default:
                break;
        }

    }
    

    /* 设置按键操作 */
    if(ui_Encoder_num == 2)
    {   
        if(ChangeVal_flag) // 如果改变值标志位开启
        {
            *currentItem->param += Slide_space[findParam_forChangeVal(currentItem->param)].step;
            // 限制param大小
            if(*currentItem->param >= Slide_space[findParam_forChangeVal(currentItem->param)].max)
                *currentItem->param = Slide_space[findParam_forChangeVal(currentItem->param)].max;
            // 执行当前Item的cb函数
            if(currentItem->CallBack != NULL) currentItem->CallBack();
        }
        else
        {
            ui_select++;
            ui_state = STATE_RUN_PAGE_DOWN;
            if(ui_select > currentPage->itemTail->lineId) ui_select = 0;
            printf("Enocder正转\r\n");
        }
 
    }
    else if(ui_Encoder_num == 1)
    {   
        if(ChangeVal_flag) // 如果改变值标志位开启
        {
            *currentItem->param -= Slide_space[findParam_forChangeVal(currentItem->param)].step;
            // 限制param大小
            if(*currentItem->param <= Slide_space[findParam_forChangeVal(currentItem->param)].min)
                *currentItem->param = Slide_space[findParam_forChangeVal(currentItem->param)].min;
            // 执行当前Item的cb函数
            if(currentItem->CallBack != NULL) currentItem->CallBack();
        }
        else
        {
            ui_select--;
            ui_state = STATE_RUN_PAGE_UP;
            if(ui_select < 1) ui_select = 0;
            printf("Enocder反转\r\n");
        }

    }
    else if(ui_Key_num == 1)
    {
       switch(currentItem->funcType)
       {
            case ITEM_JUMP_PAGE:
                // 页面跳转操作
                ui_state = STATE_READY_TO_JUMP_PAGE; // 设置成准备跳转状态
                ui_index = currentItem->JumpPage;
                ui_select = currentItem->JumpItem;
                // 保留上一页的信息
                lastPage = currentPage;

                // 设置跳转值(在switch(ui_state)的函数中完成)
                break;
            
            case ITEM_CALL_FUNCTION:
                ItemUIisRunning = true;
                break;

            case ITEM_CHECKBOX:
                // 反转该item的flag
                *currentItem->flag = !*currentItem->flag;
                // 执行该item的cb函数
                if(currentItem->CallBack != NULL) currentItem->CallBack();
                break;

            case ITEM_SWITCH:
                // 反转该item的flag
                *currentItem->flag = !*currentItem->flag;
                // 执行该item的cb函数
                if(currentItem->CallBack != NULL) currentItem->CallBack();
                break;

            case ITEM_CHANGE_VALUE:
                // 反转ChangeVal_flag
                ChangeVal_flag = !ChangeVal_flag;

                break;

            default:
                break;
            
       }
       printf("Key单击\r\n");
    }
    else if(ui_Key_num == 2)
    {
        // return Last_page or exti
        // 返回上一页面操作
        if(lastPage != NULL)
        {
            ui_state = STATE_READY_TO_JUMP_PAGE; // 设置成准备跳转状态
            ui_index = lastPage->pageId;
            ui_select = 0;
            lastPage->page_y_forlist_trg = 0;
            lastPage->page_x_trg = 0;
            // currentPage->page_y_forlist_trg = 0;
            // currentPage->page_x_trg = 0;
            // 保留上一页的信息
            lastPage = currentPage;

            ChangeVal_flag = 0; // 把改变值标志位置零 进行强制跳转 同时防止bug
        }
        printf("Key长按\r\n");
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
        else ItemUIisRunning = false; // 如果该item没函数就直接退出 防止bug

        if(ui_Key_num == 2)
        {
            ItemUIisRunning = false; // 长按退出应用
        }

        HugoUISendBuffer();
    }
    else
    {
        HugoUI_Control();
    }
	
//	printf("runUI\r\n");
}





