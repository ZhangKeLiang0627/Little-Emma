#include "OLED.h"
#include "u8g2.h"
#include "math.h"
#include "HugoUI_User.h"
#include "UART0.h"
extern u8g2_t u8g2;



void Oled_u8g2_ShowStr(uint16_t x, uint16_t y, char *str)
{
    u8g2_DrawStr(&u8g2, x, y, str);
}

void Oled_u8g2_ShowUTF8(uint16_t x, uint16_t y, char *str)
{
    u8g2_DrawUTF8(&u8g2, x, y, str);
}

/**
* @brief   showFloat/显示浮点数
* @param   x        横坐标位置
* @param   y        纵坐标位置
* @param   dat      传入需要显示的数据
* @param   num      小数点前有几位数
* @param   pointNum 小数点后有几位数
* @return  void
*/
char formatControl[10];
char NumForShow[10];
void Oled_u8g2_ShowFloat(uint16_t x, uint16_t y, float dat, uint8_t num, uint8_t pointNum)
{

    // sprintf(formatControl, "%s%d%c", "%.", pointNum, 'f');//只控制小数点后几位
    
    num += pointNum + 1;//%m.nf中的m指该浮点数所有的打印空间（包括小数点和小数点后几位）
    sprintf(formatControl, "%s%d%c%d%c", "%", num, '.', pointNum, 'f');//控制小数点前几位和后几位

    sprintf(NumForShow, formatControl, dat);
    sprintf(NumForShow, "%.2f", dat);

    u8g2_DrawStr(&u8g2, x, y, NumForShow);
}

/**
* @brief   showNum/显示正数
* @param   x        横坐标位置
* @param   y        纵坐标位置
* @param   dat      传入需要显示的数据
* @return  void
*/
void Oled_u8g2_ShowNum(uint16_t x, uint16_t y, uint16_t dat)
{

    sprintf(NumForShow, "%d", dat);

    u8g2_DrawStr(&u8g2, x, y, NumForShow);
}

/**
* @brief   DrawPixel/画点函数
* @param   x        横坐标位置
* @param   y        纵坐标位置
* @return  void
*/
void Oled_u8g2_DrawPoint(uint16_t x, uint16_t y)
{
    u8g2_DrawPixel(&u8g2, x, y);
}

/**
* @brief   DrawBox/画实心矩形
* @param   x        横坐标位置
* @param   y        纵坐标位置
* @param   width    矩形的宽度
* @param   height   矩形的长度
* @return  void
*/
void Oled_u8g2_DrawBox(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    u8g2_DrawBox(&u8g2, x, y, width, height);
}

/**
* @brief   DrawFrame/画空心矩形
* @param   x        横坐标位置
* @param   y        纵坐标位置
* @param   width    矩形的宽度
* @param   height   矩形的长度
* @return  void
*/
void Oled_u8g2_DrawFrame(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    u8g2_DrawFrame(&u8g2, x, y, width, height);
}

/**
* @brief   DrawRFrame/画空心圆角矩形
* @param   x        横坐标位置
* @param   y        纵坐标位置
* @param   width    矩形的宽度
* @param   height   矩形的长度
* @param   r        圆角半径
* @return  void
*/
void Oled_u8g2_DrawRFrame(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t r)
{
    u8g2_DrawRFrame(&u8g2, x, y, width, height, r);
}


/**
* @brief   DrawRBox/画实心圆角矩形
* @param   x        横坐标位置
* @param   y        纵坐标位置
* @param   width    矩形的宽度
* @param   height   矩形的长度
* @param   r        圆角半径
* @return  void
*/
void Oled_u8g2_DrawRBox(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t r)
{
    u8g2_DrawRBox(&u8g2, x, y, width, height, r);
}


/**
* @brief   清空发送缓冲区
* @return  void
*/
void Oled_u8g2_ClearBuffer(void)
{
    u8g2_ClearBuffer(&u8g2);
}

/**
* @brief   刷新发送缓冲区
* @return  void
*/
void Oled_u8g2_SendBuffer(void)
{
    u8g2_SendBuffer(&u8g2);
}

/**
* @brief   反色函数/基于u8g2
* @param   mode     颜色模式（0->透显/黑色 1->实显/白色 2->XOR模式/反色）如果对某物体进行反色处理记得设置回1模式
* @return  void
*/
void Oled_u8g2_SetDrawColor(uint8_t mode)
{
    u8g2_SetDrawColor(&u8g2, mode);
}

/**
* @brief   DrawPic/画图
* @param   x        横坐标位置
* @param   y        纵坐标位置
* @param   width    矩形的宽度
* @param   height   矩形的长度
* @param   pic      图片
* @return  void
*/
void Oled_u8g2_ShowBMP(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t *pic)
{
    u8g2_DrawXBMP(&u8g2, x, y, width, height, pic);
}

/**
* @brief   全屏反色函数/基于寄存器的
* @param   mode     颜色模式0->正常显示 1->反色显示
* @return  void
*/
void Oled_u8g2_ModifyColor(uint8_t mode)
{
   OLED_ColorTurn(mode);
}


uint32_t Oled_u8g2_Get_UTF8_ASCII_PixLen(char* s)
{
    return u8g2_GetUTF8Width(&u8g2, s);
}

void Oled_u8g2_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    u8g2_DrawLine(&u8g2, x1, y1, x2, y2);
}

void Oled_u8g2_DrawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
    u8g2_DrawTriangle(&u8g2, x0, y0, x1, y1, x2, y2);
}

uint16_t Oled_u8g2_GetBufferTilHeight(void)
{
    return u8g2_GetBufferTileHeight(&u8g2);
}

uint16_t Oled_u8g2_GetBufferTileWidth(void)
{
   return u8g2_GetBufferTileWidth(&u8g2);
}

uint8_t* Oled_u8g2_GetBufferPtr(void)
{
   return u8g2_GetBufferPtr(&u8g2);
}

/* 对于所有的Oled通用的调节屏幕亮度的函数 */
void Oled_EventUpdateOledLightLevel(void)
{
    u8g2_SendF(&u8g2, "c", 0x81);  //向SSD1306发送指令：设置内部电阻微调
    u8g2_SendF(&u8g2, "c",(uint8_t)*Slide_space[Slide_space_ScreenBrightness].val); //微调范围（0-255）
}

/* 选择字库 */
void Oled_u8g2_SetFont(const uint8_t *font)
{
	u8g2_SetFont(&u8g2, font);
}

