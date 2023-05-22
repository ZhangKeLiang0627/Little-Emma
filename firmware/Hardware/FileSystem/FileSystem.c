#include "FileSystem.h"

#include "ff.h"
#include "exfuns.h"    
#include "fattester.h"

#include "HugoUI_User.h"
#include "Key.h"

const char SystemSavePath[] = "1:/HugoUISAV.txt";
extern struct Slide_Bar Slide_space[];


/* 用于保存数据的事件函数 */
void EventSaveSettingConfig(void)
{
	static uint8_t SaveFinish_flag = 1; // 初始化该标志为1
	
	static int16_t move_x = -10, move_x_trg = 10;
	static int16_t move_y = -10, move_y_trg = (SCREEN_HEIGHT/2 - 15);
	static int16_t movebox_width = 140, movebox_width_trg = 105;
	static int16_t movebox_height = 80, movebox_height_trg = 30;
	
	if(SaveFinish_flag)
	{
		Oled_u8g2_ShowStr(0, 13, "Config Is Saving...");

		f_open(file, "1:/HugoUISAV.txt", FA_OPEN_ALWAYS | FA_WRITE);
		
		f_write(file, Slide_space[Slide_space_ScreenBrightness].val, sizeof(&Slide_space[Slide_space_ScreenBrightness].val), &bw);
		
		f_write(file, Slide_space[Slide_space_val1].val, sizeof(&Slide_space[Slide_space_val1].val), &bw);
		f_write(file, Slide_space[Slide_space_val2].val, sizeof(&Slide_space[Slide_space_val2].val), &bw);
		f_write(file, Slide_space[Slide_space_val3].val, sizeof(&Slide_space[Slide_space_val3].val), &bw);
		f_write(file, Slide_space[Slide_space_Volume_Ctrl].val, sizeof(&Slide_space[Slide_space_Volume_Ctrl].val), &bw);
		f_write(file, Slide_space[Slide_space_SPO2MinVal].val, sizeof(&Slide_space[Slide_space_SPO2MinVal].val), &bw);
		f_write(file, Slide_space[Slide_space_HRMaxVal].val, sizeof(&Slide_space[Slide_space_HRMaxVal].val), &bw);
		SaveFinish_flag = f_write(file, Slide_space[Slide_space_HRMinVal].val, sizeof(&Slide_space[Slide_space_HRMinVal].val), &bw); // 如果保存成功会将该标志位置0
		
		f_close(file);
	}
	else 
	{
		Oled_u8g2_ShowStr((SCREEN_WIDTH - Oled_u8g2_Get_UTF8_ASCII_PixLen("Saved Success!"))/2,
							SCREEN_HEIGHT/2 + 3, "Saved Success!"); // 居中显示
		
		Oled_u8g2_SetDrawColor(2); // XOR
		
//		Oled_u8g2_DrawRBox(10, (SCREEN_HEIGHT/2 - 15), Oled_u8g2_Get_UTF8_ASCII_PixLen("Saved Success! "), 30, 4);
		
		Oled_u8g2_DrawRBox(move_x, move_y, movebox_width, movebox_height, 4);
		
		Oled_u8g2_SetDrawColor(1);
		
		HugoUI_Run(&move_x, &move_x_trg, 2, 10);
		HugoUI_Run(&move_y, &move_y_trg, 2, 10);
		HugoUI_Run(&movebox_width, &movebox_width_trg, 2, 10);
		HugoUI_Run(&movebox_height, &movebox_height_trg, 4, 8);
		
	}

	if(KeyNum == 2)
	{
		SaveFinish_flag = 1;
		move_x = -10;
		move_y = -10;
		movebox_width = 140;
		movebox_height = 80;
		
	}
}

/* 用于上电载入数据的事件函数 */
void EventLoadSettingConfig(void)
{
	FRESULT res;
	res = f_open(file, SystemSavePath, FA_READ);
	if(res == 0)
	{
		f_read(file, Slide_space[Slide_space_ScreenBrightness].val, sizeof(&Slide_space[Slide_space_ScreenBrightness].val), &bw);
		
		f_read(file, Slide_space[Slide_space_val1].val, sizeof(&Slide_space[Slide_space_val1].val), &bw);
		f_read(file, Slide_space[Slide_space_val2].val, sizeof(&Slide_space[Slide_space_val2].val), &bw);
		f_read(file, Slide_space[Slide_space_val3].val, sizeof(&Slide_space[Slide_space_val3].val), &bw);
		f_read(file, Slide_space[Slide_space_Volume_Ctrl].val, sizeof(&Slide_space[Slide_space_Volume_Ctrl].val), &bw);
		f_read(file, Slide_space[Slide_space_SPO2MinVal].val, sizeof(&Slide_space[Slide_space_SPO2MinVal].val), &bw);
		f_read(file, Slide_space[Slide_space_HRMaxVal].val, sizeof(&Slide_space[Slide_space_HRMaxVal].val), &bw);
		f_read(file, Slide_space[Slide_space_HRMinVal].val, sizeof(&Slide_space[Slide_space_HRMinVal].val), &bw);
	}
	f_close(file);
	
}
















