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
* @brief   showFloat/��ʾ������
* @param   x        ������λ��
* @param   y        ������λ��
* @param   dat      ������Ҫ��ʾ������
* @param   num      С����ǰ�м�λ��
* @param   pointNum С������м�λ��
* @return  void
*/
char formatControl[10];
char NumForShow[10];
void Oled_u8g2_ShowFloat(uint16_t x, uint16_t y, float dat, uint8_t num, uint8_t pointNum)
{

    // sprintf(formatControl, "%s%d%c", "%.", pointNum, 'f');//ֻ����С�����λ
    
    num += pointNum + 1;//%m.nf�е�mָ�ø��������еĴ�ӡ�ռ䣨����С�����С�����λ��
    sprintf(formatControl, "%s%d%c%d%c", "%", num, '.', pointNum, 'f');//����С����ǰ��λ�ͺ�λ

    sprintf(NumForShow, formatControl, dat);
    sprintf(NumForShow, "%.2f", dat);

    u8g2_DrawStr(&u8g2, x, y, NumForShow);
}

/**
* @brief   showNum/��ʾ����
* @param   x        ������λ��
* @param   y        ������λ��
* @param   dat      ������Ҫ��ʾ������
* @return  void
*/
void Oled_u8g2_ShowNum(uint16_t x, uint16_t y, uint16_t dat)
{

    sprintf(NumForShow, "%d", dat);

    u8g2_DrawStr(&u8g2, x, y, NumForShow);
}

/**
* @brief   DrawPixel/���㺯��
* @param   x        ������λ��
* @param   y        ������λ��
* @return  void
*/
void Oled_u8g2_DrawPoint(uint16_t x, uint16_t y)
{
    u8g2_DrawPixel(&u8g2, x, y);
}

/**
* @brief   DrawBox/��ʵ�ľ���
* @param   x        ������λ��
* @param   y        ������λ��
* @param   width    ���εĿ��
* @param   height   ���εĳ���
* @return  void
*/
void Oled_u8g2_DrawBox(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    u8g2_DrawBox(&u8g2, x, y, width, height);
}

/**
* @brief   DrawFrame/�����ľ���
* @param   x        ������λ��
* @param   y        ������λ��
* @param   width    ���εĿ��
* @param   height   ���εĳ���
* @return  void
*/
void Oled_u8g2_DrawFrame(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    u8g2_DrawFrame(&u8g2, x, y, width, height);
}

/**
* @brief   DrawRFrame/������Բ�Ǿ���
* @param   x        ������λ��
* @param   y        ������λ��
* @param   width    ���εĿ��
* @param   height   ���εĳ���
* @param   r        Բ�ǰ뾶
* @return  void
*/
void Oled_u8g2_DrawRFrame(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t r)
{
    u8g2_DrawRFrame(&u8g2, x, y, width, height, r);
}


/**
* @brief   DrawRBox/��ʵ��Բ�Ǿ���
* @param   x        ������λ��
* @param   y        ������λ��
* @param   width    ���εĿ��
* @param   height   ���εĳ���
* @param   r        Բ�ǰ뾶
* @return  void
*/
void Oled_u8g2_DrawRBox(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t r)
{
    u8g2_DrawRBox(&u8g2, x, y, width, height, r);
}


/**
* @brief   ��շ��ͻ�����
* @return  void
*/
void Oled_u8g2_ClearBuffer(void)
{
    u8g2_ClearBuffer(&u8g2);
}

/**
* @brief   ˢ�·��ͻ�����
* @return  void
*/
void Oled_u8g2_SendBuffer(void)
{
    u8g2_SendBuffer(&u8g2);
}

/**
* @brief   ��ɫ����/����u8g2
* @param   mode     ��ɫģʽ��0->͸��/��ɫ 1->ʵ��/��ɫ 2->XORģʽ/��ɫ�������ĳ������з�ɫ����ǵ����û�1ģʽ
* @return  void
*/
void Oled_u8g2_SetDrawColor(uint8_t mode)
{
    u8g2_SetDrawColor(&u8g2, mode);
}

/**
* @brief   DrawPic/��ͼ
* @param   x        ������λ��
* @param   y        ������λ��
* @param   width    ���εĿ��
* @param   height   ���εĳ���
* @param   pic      ͼƬ
* @return  void
*/
void Oled_u8g2_ShowBMP(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t *pic)
{
    u8g2_DrawXBMP(&u8g2, x, y, width, height, pic);
}

/**
* @brief   ȫ����ɫ����/���ڼĴ�����
* @param   mode     ��ɫģʽ0->������ʾ 1->��ɫ��ʾ
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

/* �������е�Oledͨ�õĵ�����Ļ���ȵĺ��� */
void Oled_EventUpdateOledLightLevel(void)
{
    u8g2_SendF(&u8g2, "c", 0x81);  //��SSD1306����ָ������ڲ�����΢��
    u8g2_SendF(&u8g2, "c",(uint8_t)*Slide_space[Slide_space_ScreenBrightness].val); //΢����Χ��0-255��
}

/* ѡ���ֿ� */
void Oled_u8g2_SetFont(const uint8_t *font)
{
	u8g2_SetFont(&u8g2, font);
}

