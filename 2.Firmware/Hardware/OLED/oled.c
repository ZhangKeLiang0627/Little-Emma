#include "gd32f30x.h"                   // Device header
#include "oled.h"
#include "stdlib.h"
#include "oledfont.h"  	 
#include "systick.h"


u8 OLED_GRAM[128][8];


void I2C_WriteByte(uint8_t addr,uint8_t data)
{
	while(i2c_flag_get(I2C0, I2C_FLAG_I2CBSY));
	i2c_start_on_bus(I2C0);
	while(!i2c_flag_get(I2C0, I2C_FLAG_SBSEND));
	i2c_master_addressing(I2C0, OLED_ADDRESS, I2C_TRANSMITTER);
	while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND));
	i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
	while(SET != i2c_flag_get(I2C0, I2C_FLAG_TBE));
	i2c_data_transmit(I2C0, addr);
	while(!i2c_flag_get(I2C0, I2C_FLAG_BTC));
	i2c_data_transmit(I2C0, data);
	while(!i2c_flag_get(I2C0, I2C_FLAG_BTC));
	i2c_stop_on_bus(I2C0);
	while(I2C_CTL0(I2C0)&0x0200);
	
}


void WriteCmd(unsigned char I2C_Command)
{
   	I2C_WriteByte(0x00, I2C_Command);
}


void WriteDat(unsigned char I2C_Data)
{
   	I2C_WriteByte(0x40, I2C_Data);
}




//���Ժ���
void OLED_ColorTurn(u8 i)
{
	if(i==0)
		{
			WriteCmd(0xA6);
		}
	if(i==1)
		{
			WriteCmd(0xA7);
		}
}

//��Ļ��ת180��
void OLED_DisplayTurn(u8 i)
{
	if(i==0)
		{
			WriteCmd(0xC8);//������ʾ
			WriteCmd(0xA1);
		}
	if(i==1)
		{
			WriteCmd(0xC0);//��ת��ʾ
			WriteCmd(0xA0);
		}
}



//����OLED��ʾ 
void OLED_DisPlay_On(void)
{
	WriteCmd(0x8D);//��ɱ�ʹ��
	WriteCmd(0x14);//������ɱ�
	WriteCmd(0xAF);//������Ļ
}

//�ر�OLED��ʾ 
void OLED_DisPlay_Off(void)
{
	WriteCmd(0x8D);//��ɱ�ʹ��
	WriteCmd(0x10);//�رյ�ɱ�
	WriteCmd(0xAE);//�ر���Ļ
}

//�����Դ浽OLED	
void OLED_Refresh(void)
{
	u8 i,n;
	for(i=0;i<8;i++)	//���ﲻһ�� i<8
	{
		WriteCmd(0xb0+i); //��������ʼ��ַ
		WriteCmd(0x00);   //���õ�����ʼ��ַ
		WriteCmd(0x10);   //���ø�����ʼ��ַ

		for(n=0;n<128;n++)
		{
			WriteDat(OLED_GRAM[n][i]);
		}
  }
}

void OLED_PartialRefresh(uint8_t x1,uint8_t x2,uint8_t y1,uint8_t y2)
{
	u8 i,n;
	for(i=y1;i<y2;i++)	//���ﲻһ�� i<4
	{
		WriteCmd(0xb0+i); //???????
		WriteCmd(0x00);   //????????
		WriteCmd(0x10);   //????????
		for(n=x1;n<x2;n++)
		{
			WriteDat(OLED_GRAM[n][i]);
		}
  }
}
	
void OLED_PartialRefreshForBuff(uint8_t x1,uint8_t x2,uint8_t y1,uint8_t y2)
{
	u8 i,n;
	for(i=y1;i<y2;i++)	//���ﲻһ�� i<4
	{
		WriteCmd(0xb0+i); //???????
		WriteCmd(0x00);   //????????
		WriteCmd(0x10);   //????????
		for(n=x1;n<x2;n++)
		{
			WriteDat(OLED_GRAM[n][i]);
		}
	}
}

//��������
void OLED_Clear(void)
{
	u8 i,n;
	for(i=0;i<8;i++)	//���ﲻһ�� i<4
	{
	   for(n=0;n<128;n++)
			{
			 OLED_GRAM[n][i]=0;//�����������
			}
  }
	OLED_Refresh();//������ʾ
}

//���� 
//x:0~127
//y:0~63
//t:1 ��� 0,���	
void OLED_DrawPoint(u8 x,u8 y,u8 t)
{
	u8 i,m,n;
	i=y/8;
	m=y%8;
	n=1<<m;
	if(t){OLED_GRAM[x][i]|=n;}
	else
	{
		OLED_GRAM[x][i]=~OLED_GRAM[x][i];
		OLED_GRAM[x][i]|=n;
		OLED_GRAM[x][i]=~OLED_GRAM[x][i];
	}
}

//����
//x1,y1:�������
//x2,y2:��������
void OLED_DrawLine(u8 x1,u8 y1,u8 x2,u8 y2,u8 mode)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;
	delta_x=x2-x1; //������������ 
	delta_y=y2-y1;
	uRow=x1;//�����������
	uCol=y1;
	if(delta_x>0)incx=1; //���õ������� 
	else if (delta_x==0)incx=0;//��ֱ�� 
	else {incx=-1;delta_x=-delta_x;}
	if(delta_y>0)incy=1;
	else if (delta_y==0)incy=0;//ˮƽ�� 
	else {incy=-1;delta_y=-delta_x;}
	if(delta_x>delta_y)distance=delta_x; //ѡȡ�������������� 
	else distance=delta_y;
	for(t=0;t<distance+1;t++)
	{
		OLED_DrawPoint(uRow,uCol,mode);//����
		xerr+=delta_x;
		yerr+=delta_y;
		if(xerr>distance)
		{
			xerr-=distance;
			uRow+=incx;
		}
		if(yerr>distance)
		{
			yerr-=distance;
			uCol+=incy;
		}
	}
}
//x,y:Բ������
//r:Բ�İ뾶
void OLED_DrawCircle(u8 x,u8 y,u8 r)
{
	int a, b,num;
    a = 0;
    b = r;
    while(2 * b * b >= r * r)      
    {
        OLED_DrawPoint(x + a, y - b,1);
        OLED_DrawPoint(x - a, y - b,1);
        OLED_DrawPoint(x - a, y + b,1);
        OLED_DrawPoint(x + a, y + b,1);
 
        OLED_DrawPoint(x + b, y + a,1);
        OLED_DrawPoint(x + b, y - a,1);
        OLED_DrawPoint(x - b, y - a,1);
        OLED_DrawPoint(x - b, y + a,1);
        
        a++;
        num = (a * a + b * b) - r*r;//���㻭�ĵ���Բ�ĵľ���
        if(num > 0)
        {
            b--;
            a--;
        }
    }
}



//��ָ��λ����ʾһ���ַ�,���������ַ�
//x:0~127
//y:0~63
//size1:ѡ������ 6x8/6x12/8x16/12x24
//mode:0,��ɫ��ʾ;1,������ʾ
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size1,u8 mode)
{
	u8 i,m,temp,size2,chr1;
	u8 x0=x,y0=y;
	if(size1==8)size2=6;
	else size2=(size1/8+((size1%8)?1:0))*(size1/2);  //�õ�����һ���ַ���Ӧ������ռ���ֽ���
	chr1=chr-' ';  //����ƫ�ƺ��ֵ
	for(i=0;i<size2;i++)
	{
		if(size1==8)
			  {temp=asc2_0806[chr1][i];} //����0806����
		else if(size1==12)
        {temp=asc2_1206[chr1][i];} //����1206����
		else if(size1==16)
        {temp=asc2_1608[chr1][i];} //����1608����
		else if(size1==24)
        {temp=asc2_2412[chr1][i];} //����2412����
		else return;
		for(m=0;m<8;m++)
		{
			if(temp&0x01)OLED_DrawPoint(x,y,mode);
			else OLED_DrawPoint(x,y,!mode);
			temp>>=1;
			y++;
		}
		x++;
		if((size1!=8)&&((x-x0)==size1/2))
		{x=x0;y0=y0+8;}
		y=y0;
  }
}


//��ʾ�ַ���
//x,y:�������  
//size1:�����С 
//*chr:�ַ�����ʼ��ַ 
//mode:0,��ɫ��ʾ;1,������ʾ
/**
  * @brief ��ʾ�ַ��������֡����ġ�Ӣ�ķ��ţ�
  * @param x	������			0~127
  * @param y	������			0~63
  * @param *chr	�ַ�����ʼ��ַ
  * @param size1 �����С		д16����Ϊֻ����16*16�Ĺ�����
  * @param mode ��ɫģʽ		��0������ʾ�� ��1������ʾ    	
  * @retval 
  */
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 size1,u8 mode)
{
	while(*chr != '\0')
	{
		if((*chr >= ' ')&&(*chr <= '~'))
		{
			if(size1 == 8)
			{
				if((x + 6) > 128)
					x = 0;
					y = y + size1;
			}
			else if((x + size1/2) > 128)
			{
				x = 0;
				y = y + size1;
			}
			OLED_ShowChar(x, y, *chr, size1, mode);
			if(size1 == 8)x += 6;
			else x += size1/2;
			chr++;
		}
		else
		{
			if(size1 == 8)
			{
				if((x + 6) > 128)
					x = 0;
					y = y + size1;
			}
			else if((x +size1) > 128)
			{
				x = 0;
				y = y + size1;
			}
			
			OLED_ShowChinese(x, y, chr, size1, mode);
			if(size1 == 8)x += 6;
			else x += size1;
			chr += 2;
		}

	}
}


//m^n
u32 OLED_Pow(u8 m,u8 n)
{
	u32 result=1;
	while(n--)
	{
	  result*=m;
	}
	return result;
}

//��ʾ����
//x,y :�������	 
//len :���ֵ�λ��
//size:�����С
//mode:0,��ɫ��ʾ;1,������ʾ
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size1,u8 mode)
{
	u8 t,temp,m=0;
	if(size1==8)m=2;
	for(t=0;t<len;t++)
	{
		temp=(num/OLED_Pow(10,len-t-1))%10;
			if(temp==0)
			{
				OLED_ShowChar(x+(size1/2+m)*t,y,'0',size1,mode);
      }
			else 
			{
			  OLED_ShowChar(x+(size1/2+m)*t,y,temp+'0',size1,mode);
			}
  }
}

//��ʾ����
//x,y:�������
//num:���ֶ�Ӧ�����
//mode:0,��ɫ��ʾ;1,������ʾ
void OLED_ShowChinese(uint8_t x, uint8_t y, uint8_t* str, uint8_t size1, uint8_t mode)
{
	uint8_t num;
	int index = 0;
	uint8_t m,temp;
	uint8_t x0=x,y0=y;
	uint16_t i,size3=(size1/8+((size1%8)?1:0))*size1;  //�õ�����һ���ַ���Ӧ������ռ���ֽ���
	for(i=0;i<size3;i++)
	{
		if(size1==16)
		{
			for(index = 0; index < sizeof(Hzk)/35; index++)
			{
				if(Hzk[index].name[0] == str[0]&&Hzk[index].name[1] == str[1])
					temp = Hzk[index].dat[i];
			}
		}	
		else if(size1==24)
				{temp=Hzk2[num][i];}//����24*24����
		else if(size1==32)       
				{temp=Hzk3[num][i];}//����32*32����
		else if(size1==64)
				{temp=Hzk4[num][i];}//����64*64����
		else return;
		for(m=0;m<8;m++)
		{
			if(temp&0x01)OLED_DrawPoint(x,y,mode);
			else OLED_DrawPoint(x,y,!mode);
			temp>>=1;
			y++;
		}
		x++;
		if((x-x0)==size1)
		{x=x0;y0=y0+8;}
		y=y0;
	}
}


//x,y���������
//sizex,sizey,ͼƬ����
//BMP[]��Ҫд���ͼƬ����
//mode:0,��ɫ��ʾ;1,������ʾ
void OLED_ShowPicture(u8 x,u8 y,u8 sizex,u8 sizey,u8 *BMP,u8 mode)
{
	u16 j=0;
	u8 i,n,temp,m;
	u8 x0=x,y0=y;
	sizey=sizey/8+((sizey%8)?1:0);
	for(n=0;n<sizey;n++)
	{
		 for(i=0;i<sizex;i++)
		 {
				temp=BMP[j];
				j++;
				for(m=0;m<8;m++)
				{
					if(temp&0x01)OLED_DrawPoint(x,y,mode);
					else OLED_DrawPoint(x,y,!mode);
					temp>>=1;
					y++;
				}
				x++;
				if((x-x0)==sizex)
				{
					x=x0;
					y0=y0+8;
				}
				y=y0;
		}
	 }
}
//OLED�ĳ�ʼ��
void OLED_Init(void)
{
	rcu_periph_clock_enable(RCU_GPIOB);//��gpiobʱ��
	rcu_periph_clock_enable(RCU_I2C0);//��i2c0ʱ��
	rcu_periph_clock_enable(RCU_AF);//�򿪸���ʱ��
	
	gpio_pin_remap_config(GPIO_I2C0_REMAP, ENABLE);//��i2c0����������ӳ����pb8/pb9
	//gpio��ʼ��
	gpio_init(GPIOB, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_8|GPIO_PIN_9);
	
	//HW_i2c��ʼ��
	i2c_deinit(I2C0);
	i2c_clock_config(I2C0, 1000000, I2C_DTCY_2);
	i2c_mode_addr_config(I2C0, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, 0x77);
	i2c_enable(I2C0);
	i2c_ack_config(I2C0, I2C_ACK_ENABLE);
	
	delay_1ms(200);
	
	WriteCmd(0xAE); /*display off*/
	WriteCmd(0x00); /*set lower column address*/ 
	WriteCmd(0x10); /*set higher column address*/
	WriteCmd(0x40); /*set start line address Set Mapping RAM Display Start Line (0x00~0x3F)*/ 
	WriteCmd(0x81); /*set contrast control register*/
	WriteCmd(0xCF); /*Set SEG Output Current Brightness*/
	WriteCmd(0xA1); /*Set SEG/Column Mapping / 0xa0���ҷ��� 0xa1����*/ 
	WriteCmd(0xC8); /*Set COM/Row Scan Direction / 0xc0���·��� 0xc8����*/
	WriteCmd(0xA6); /*set normal display*/
	WriteCmd(0xA8); /*set multiplex ratio(1 to 64)*/
	WriteCmd(0x3f); /*1/64 duty*/
	WriteCmd(0xD3); /*set display offset / Shift Mapping RAM Counter (0x00~0x3F)*/
	WriteCmd(0x00); /*not offset*/
	WriteCmd(0xd5); /*set display clock divide ratio/oscillator frequency*/
	WriteCmd(0x80); /*set divide ratio, Set Clock as 100 Frames/Sec*/
	WriteCmd(0xD9); /*set pre-charge period*/
	WriteCmd(0xF1); /*Set Pre-Charge as 15 Clocks & Discharge as 1 Clock*/
	WriteCmd(0xDA); /*set com pins hardware configuration*/
	WriteCmd(0x12); 
	WriteCmd(0xDB); /*set vcomh*/
	WriteCmd(0x30); /*Set VCOM Deselect Level*/
	WriteCmd(0x20); /*Set Page Addressing Mode (0x00/0x01/0x02)*/
	WriteCmd(0x02);
	WriteCmd(0x8D); /*set Charge Pump enable/disable*/
	WriteCmd(0x14); /*set(0x10) disable*/
	OLED_Clear();
	WriteCmd(0xAF); /*display ON*/
}

