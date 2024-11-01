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

/********************血氧仪变量********************/
uint32_t max30102_data[2];
int32_t HeartRate = 0;
int32_t SpO2 = 0;

uint32_t ppg_data_cache_RED[CACHE_NUMS] = {0}; // 缓存区
uint32_t ppg_data_cache_IR[CACHE_NUMS] = {0};  // 缓存区
uint16_t cache_counter = 0; // 缓存计数器

extern uint8_t max30102_set_flag;
extern uint16_t max30102_count;
/********************血氧仪变量********************/


extern float pitch,roll,yaw; 		//欧拉角
extern short aacx,aacy,aacz;		//加速度传感器原始数据
extern short gyrox,gyroy,gyroz;		//陀螺仪原始数据

uint8_t KeyNum;	//返回的键码值（在主程序中循环获得）
uint8_t EncoderNum;

u8g2_t u8g2; //初始化u8g2结构体

uint8_t res = 1;
u32 total,free1;

int main(void)
{
    /* configure systick */
    systick_config();
	Timer5_Delay_Init();
	InitUART0(115200);
	
	OLED_Init();				//初始化OLED 
	LED_Init();
	
	Key_Init();
	Beeper_Init();
	Encoder_Init();
	max30102_init();
	max30102_fir_init();
	
	TIM4_Random_Init(12000 - 1, 10 - 1);
	
	W25QXX_Init();				//初始化W25Q128
	
	my_mem_init(SRAMIN);
	
	HugoUI_InitLayout();
	
	res = exfuns_init();						//为fatfs相关变量申请内存		
	
	printf("exfuns = %d\r\n", res);
	
	res = f_mount(fs[1],"1:",1); 				//挂载FLASH
	printf("flash = %x\r\n", res);
	
	
	if(res==0X0D) 								//FLASH磁盘,FAT文件系统错误,重新格式化FLASH
	{
	    //格式化FLASH
		res = f_mkfs("1:",1,4096);				//格式化FLASH,1,盘符;1,不需要引导区,8个扇区为1个簇

		if(res==0)
		{
			f_setlabel((const TCHAR *)"1:HugoUI");		//设置Flash磁盘的名字为：HugoUI
			printf("Flash Disk Format Finish\r\n");		//格式化完成
		}else printf("Flash Disk Format Error!\r\n"); 	//格式化失败
		delay_ms(1000);
	}	
	
	while(exf_getfree("1:",&total,&free1))	//得到Flash的总容量和剩余容量
	{
		delay_ms(2);		  
	}
	printf("FATFS OK!\r\n");
	printf("Flash Total Size:%dMB\r\n",total>>10);//显示SD卡总容量 MB
	printf("Flash Free Size:%dMB\r\n",free1>>10);//显示SD卡剩余容量 MB
	
	u8g2Init(&u8g2); //u8g2图形库初始化
	u8g2_SetFont(&u8g2,u8g2_font_profont15_mr); //选择字库

	printf("内存占用情况：%d\r\n", my_mem_perused(SRAMIN));
	
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


