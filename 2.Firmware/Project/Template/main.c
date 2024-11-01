#include "sys.h"
#include "systick.h"
#include "BEEPER.h"
#include "Encoder.h"
#include "Malloc.h"
#include "HugoUI_User.h"
#include "Hugo_UI.h"
#include "UART0.h"

#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h" 

#include "mmc_sd.h"	
#include "w25qxx.h"    
#include "ff.h"
#include "exfuns.h"    
#include "fattester.h"

#include "led.h"

#include "FileSystem.h"

#include "max30102.h"
#include "max30102_fir.h"
#include "max3010X.h"

/********************Ѫ���Ǳ���********************/
uint32_t max30102_data[2];
int32_t HeartRate = 0;
int32_t SpO2 = 0;

uint32_t ppg_data_cache_RED[CACHE_NUMS] = {0}; // ������
uint32_t ppg_data_cache_IR[CACHE_NUMS] = {0};  // ������
uint16_t cache_counter = 0; // ���������

extern uint8_t max30102_set_flag;
extern uint16_t max30102_count;
/********************Ѫ���Ǳ���********************/


extern float pitch,roll,yaw; 		//ŷ����
extern short aacx,aacy,aacz;		//���ٶȴ�����ԭʼ����
extern short gyrox,gyroy,gyroz;		//������ԭʼ����

uint8_t KeyNum;	//���صļ���ֵ������������ѭ����ã�
uint8_t EncoderNum;

u8g2_t u8g2; //��ʼ��u8g2�ṹ��

uint8_t res = 1;
u32 total,free1;

int main(void)
{
    /* configure systick */
    systick_config();
	Timer5_Delay_Init();
	InitUART0(115200);
	
	OLED_Init();				//��ʼ��OLED 
	LED_Init();
	
	Key_Init();
	Beeper_Init();
	Encoder_Init();
	max30102_init();
	max30102_fir_init();
	
	TIM4_Random_Init(12000 - 1, 10 - 1);
	
	W25QXX_Init();				//��ʼ��W25Q128
	
	my_mem_init(SRAMIN);
	
	HugoUI_InitLayout();
	
	res = exfuns_init();						//Ϊfatfs��ر��������ڴ�		
	
	printf("exfuns = %d\r\n", res);
	
	res = f_mount(fs[1],"1:",1); 				//����FLASH
	printf("flash = %x\r\n", res);
	
	
	if(res==0X0D) 								//FLASH����,FAT�ļ�ϵͳ����,���¸�ʽ��FLASH
	{
	    //��ʽ��FLASH
		res = f_mkfs("1:",1,4096);				//��ʽ��FLASH,1,�̷�;1,����Ҫ������,8������Ϊ1����

		if(res==0)
		{
			f_setlabel((const TCHAR *)"1:HugoUI");		//����Flash���̵�����Ϊ��HugoUI
			printf("Flash Disk Format Finish\r\n");		//��ʽ�����
		}else printf("Flash Disk Format Error!\r\n"); 	//��ʽ��ʧ��
		delay_ms(1000);
	}	
	
	while(exf_getfree("1:",&total,&free1))	//�õ�Flash����������ʣ������
	{
		delay_ms(2);		  
	}
	printf("FATFS OK!\r\n");
	printf("Flash Total Size:%dMB\r\n",total>>10);//��ʾSD�������� MB
	printf("Flash Free Size:%dMB\r\n",free1>>10);//��ʾSD��ʣ������ MB
	
	u8g2Init(&u8g2); //u8g2ͼ�ο��ʼ��
	u8g2_SetFont(&u8g2,u8g2_font_profont15_mr); //ѡ���ֿ�

	printf("�ڴ�ռ�������%d\r\n", my_mem_perused(SRAMIN));
	
	EventLoadSettingConfig();
	
    while (1)
	{
		KeyNum = Key_GetNum();
		EncoderNum = Encoder_GetNum();
		HugoUI_System();
//		PulseOximeter_proc();
//		MAX30102_Get_DATA(&HeartRate,&SpO2,max30102_data,fir_output);
//		delay_ms(50);
    }
}


