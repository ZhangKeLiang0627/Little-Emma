#include "sys.h"
#include "Timer5_Delay.h"
/*移植的是ssd1306 128*32 的库*/
#define SCL_Pin	GPIO_PIN_8
#define SDA_Pin	GPIO_PIN_9
#define IIC_GPIO_Port GPIOB


uint8_t u8x8_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    switch (msg)
    {
    case U8X8_MSG_DELAY_100NANO: // delay arg_int * 100 nano seconds
        __NOP();
        break;
    case U8X8_MSG_DELAY_10MICRO: // delay arg_int * 10 micro seconds
        for (uint16_t n = 0; n < 320; n++)
        {
            __NOP();
        }
        break;
    case U8X8_MSG_DELAY_MILLI: // delay arg_int * 1 milli second
        delay_ms(1);
        break;
    case U8X8_MSG_DELAY_I2C: // arg_int is the I2C speed in 100KHz, e.g. 4 = 400 KHz
        delay_us(5);
        break;                    // arg_int=1: delay by 5us, arg_int = 4: delay by 1.25us
    case U8X8_MSG_GPIO_I2C_CLOCK: // arg_int=0: Output low at I2C clock pin
		if(arg_int == 1) 
		{
			gpio_bit_set(IIC_GPIO_Port, SCL_Pin);
		}
		else if(arg_int == 0)
		{
			gpio_bit_reset(IIC_GPIO_Port, SCL_Pin);  
		}  
        break;                    // arg_int=1: Input dir with pullup high for I2C clock pin
    case U8X8_MSG_GPIO_I2C_DATA:  // arg_int=0: Output low at I2C data pin
        if(arg_int == 1) 
		{
			gpio_bit_set(IIC_GPIO_Port, SDA_Pin);
		}
		else if(arg_int == 0)
		{
			gpio_bit_reset(IIC_GPIO_Port, SDA_Pin);  
		} 
        break;                    // arg_int=1: Input dir with pullup high for I2C data pin
    case U8X8_MSG_GPIO_MENU_SELECT:
        u8x8_SetGPIOResult(u8x8, /* get menu select pin state */ 0);
        break;
    case U8X8_MSG_GPIO_MENU_NEXT:
        u8x8_SetGPIOResult(u8x8, /* get menu next pin state */ 0);
        break;
    case U8X8_MSG_GPIO_MENU_PREV:
        u8x8_SetGPIOResult(u8x8, /* get menu prev pin state */ 0);
        break;
    case U8X8_MSG_GPIO_MENU_HOME:
        u8x8_SetGPIOResult(u8x8, /* get menu home pin state */ 0);
        break;
    default:
        u8x8_SetGPIOResult(u8x8, 1); // default return value
        break;
    }
    return 1;
}

uint8_t u8x8_byte_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
  static uint8_t buffer[32];		/* u8g2/u8x8 will never send more than 32 bytes between START_TRANSFER and END_TRANSFER */
  static uint8_t buf_idx;
  uint8_t *data;
 
  switch(msg){
		
    case U8X8_MSG_BYTE_SEND:
      data = (uint8_t *)arg_ptr;      
      while( arg_int > 0 )
		  {
				buffer[buf_idx++] = *data;
				data++;
				arg_int--;
		  }      
    break;
			
    case U8X8_MSG_BYTE_INIT:
      /* add your custom code to init i2c subsystem */
    break;
		
    case U8X8_MSG_BYTE_START_TRANSFER:
      buf_idx = 0;
    break;
		
    case U8X8_MSG_BYTE_END_TRANSFER:

//		HW_I2cWrite(u8x8,buffer,buf_idx);
	//居然给我蒙对了！
		if(buf_idx<=0)
				break;

		/* wait for the busy falg to be reset */
//		while(I2C_GetFlagStatus(I2C1,I2C_FLAG_BUSY));
		while(i2c_flag_get(I2C0, I2C_FLAG_I2CBSY));
		/* start transfer */
//		I2C_GenerateSTART(I2C1,ENABLE);
		i2c_start_on_bus (I2C0);
//		while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT));
		while(!i2c_flag_get(I2C0, I2C_FLAG_SBSEND));
//		I2C_Send7bitAddress(I2C1,u8x8_GetI2CAddress(u8x8),I2C_Direction_Transmitter);
		i2c_master_addressing(I2C0, u8x8_GetI2CAddress(u8x8), I2C_TRANSMITTER);
		
		while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND));//????????
		i2c_flag_clear (I2C0, I2C_FLAG_ADDSEND);//??ADDSEND?
		
//		while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
		while(SET != i2c_flag_get(I2C0, I2C_FLAG_TBE));
		for(uint8_t i=0;i<buf_idx;i++)
		{
//			I2C_SendData(I2C1,buffer[i]);
//			while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTING));

			i2c_data_transmit (I2C0, buffer[i]);
			while(!i2c_flag_get(I2C0, I2C_FLAG_BTC));
		}
		
//		I2C_GenerateSTOP(I2C1,ENABLE);
		i2c_stop_on_bus (I2C0);
		while(I2C_CTL0(I2C0)&0x0200);
    break;
		
    default:
      return 0;
  }
  return 1;
}



//void HW_I2cWrite(uint8_t *buf,uint8_t len)
//{
//	if(len<=0)
//		return ;

//	/* wait for the busy falg to be reset */
//	while(I2C_GetFlagStatus(I2C1,I2C_FLAG_BUSY));

//	/* start transfer */
//	I2C_GenerateSTART(I2C1,ENABLE);
//	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT));
//  
//	I2C_Send7bitAddress(I2C1,u8x8_GetI2CAddress(u8x8),I2C_Direction_Transmitter);
//    
//	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
//	
//	for(uint8_t i=0;i<len;i++)
//	{
//		I2C_SendData(I2C1,buf[i]);
//		
//		while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTING));
//	}
//	I2C_GenerateSTOP(I2C1,ENABLE);
//}


void u8g2Init(u8g2_t *u8g2)
{
	u8g2_Setup_ssd1306_i2c_128x64_noname_f(u8g2, U8G2_R0, u8x8_byte_hw_i2c, u8x8_gpio_and_delay);  // 初始化u8g2结构体
	u8g2_InitDisplay(u8g2); //根据所选的芯片进行初始化工作，初始化完成后，显示器处于关闭状态
	u8g2_SetPowerSave(u8g2, 0); //打开显示器
	u8g2_ClearBuffer(u8g2);
}






















