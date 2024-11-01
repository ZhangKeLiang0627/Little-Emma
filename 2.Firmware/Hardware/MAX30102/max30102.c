#include "max30102.h"
#include "max30102_fir.h"
#include "myiic_MAX30102.h"
#include "Timer5_Delay.h"
#include "UART0.h"
#include "algorithm.h"

extern uint8_t max30102_set_flag;

extern uint8_t max30102_int_flag;
extern uint32_t ppg_data_cache_RED[CACHE_NUMS]; // ������
extern uint32_t ppg_data_cache_IR[CACHE_NUMS];  // ������
extern uint16_t cache_counter;



u8 max30102_Bus_Write(u8 Register_Address, u8 Word_Data)
{

	/* ���ô���EEPROM�漴��ȡָ�����У�������ȡ�����ֽ� */

	/* ��1��������I2C���������ź� */
	IIC_Start_MAX30102();

	/* ��2������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
	IIC_Send_Byte_MAX30102(max30102_WR_address | I2C_WR);	/* �˴���дָ�� */

	/* ��3��������ACK */
	if (IIC_Wait_Ack_MAX30102() != 0)
	{
		goto cmd_fail;	/* EEPROM������Ӧ�� */
	}

	/* ��4���������ֽڵ�ַ */
	IIC_Send_Byte_MAX30102(Register_Address);
	if (IIC_Wait_Ack_MAX30102() != 0)
	{
		goto cmd_fail;	/* EEPROM������Ӧ�� */
	}
	
	/* ��5������ʼд������ */
	IIC_Send_Byte_MAX30102(Word_Data);

	/* ��6��������ACK */
	if (IIC_Wait_Ack_MAX30102() != 0)
	{
		goto cmd_fail;	/* EEPROM������Ӧ�� */
	}

	/* ����I2C����ֹͣ�ź� */
	IIC_Stop_MAX30102();
	return 1;	/* ִ�гɹ� */

cmd_fail: /* ����ִ��ʧ�ܺ��мǷ���ֹͣ�źţ�����Ӱ��I2C�����������豸 */
	/* ����I2C����ֹͣ�ź� */
	IIC_Stop_MAX30102();
	return 0;
}


u8 max30102_Bus_Read(u8 Register_Address)
{
	u8  data;


	/* ��1��������I2C���������ź� */
	IIC_Start_MAX30102();

	/* ��2������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
	IIC_Send_Byte_MAX30102(max30102_WR_address | I2C_WR);	/* �˴���дָ�� */

	/* ��3��������ACK */
	if (IIC_Wait_Ack_MAX30102() != 0)
	{
		goto cmd_fail;	/* EEPROM������Ӧ�� */
	}

	/* ��4���������ֽڵ�ַ�� */
	IIC_Send_Byte_MAX30102((uint8_t)Register_Address);
	if (IIC_Wait_Ack_MAX30102() != 0)
	{
		goto cmd_fail;	/* EEPROM������Ӧ�� */
	}
	

	/* ��6������������I2C���ߡ����濪ʼ��ȡ���� */
	IIC_Start_MAX30102();

	/* ��7������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
	IIC_Send_Byte_MAX30102(max30102_WR_address | I2C_RD);	/* �˴��Ƕ�ָ�� */

	/* ��8��������ACK */
	if (IIC_Wait_Ack_MAX30102() != 0)
	{
		goto cmd_fail;	/* EEPROM������Ӧ�� */
	}

	/* ��9������ȡ���� */
	{
		data = IIC_Read_Byte_MAX30102(0);	/* ��1���ֽ� */

		IIC_NAck_MAX30102();	/* ���1���ֽڶ����CPU����NACK�ź�(����SDA = 1) */
	}
	/* ����I2C����ֹͣ�ź� */
	IIC_Stop_MAX30102();
	return data;	/* ִ�гɹ� ����dataֵ */

cmd_fail: /* ����ִ��ʧ�ܺ��мǷ���ֹͣ�źţ�����Ӱ��I2C�����������豸 */
	/* ����I2C����ֹͣ�ź� */
	IIC_Stop_MAX30102();
	return 0;
}


void max30102_FIFO_ReadWords(u8 Register_Address,u16 Word_Data[][2],u8 count)
{
	u8 i=0;
	u8 no = count;
	u8 data1, data2;
	/* ��1��������I2C���������ź� */
	IIC_Start_MAX30102();

	/* ��2������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
	IIC_Send_Byte_MAX30102(max30102_WR_address | I2C_WR);	/* �˴���дָ�� */

	/* ��3��������ACK */
	if (IIC_Wait_Ack_MAX30102() != 0)
	{
		goto cmd_fail;	/* EEPROM������Ӧ�� */
	}

	/* ��4���������ֽڵ�ַ�� */
	IIC_Send_Byte_MAX30102((uint8_t)Register_Address);
	if (IIC_Wait_Ack_MAX30102() != 0)
	{
		goto cmd_fail;	/* EEPROM������Ӧ�� */
	}
	

	/* ��6������������I2C���ߡ����濪ʼ��ȡ���� */
	IIC_Start_MAX30102();

	/* ��7������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
	IIC_Send_Byte_MAX30102(max30102_WR_address | I2C_RD);	/* �˴��Ƕ�ָ�� */

	/* ��8��������ACK */
	if (IIC_Wait_Ack_MAX30102() != 0)
	{
		goto cmd_fail;	/* EEPROM������Ӧ�� */
	}

	/* ��9������ȡ���� */
	while (no)
	{
		data1 = IIC_Read_Byte_MAX30102(0);	
		IIC_Ack_MAX30102();
		data2 = IIC_Read_Byte_MAX30102(0);
		IIC_Ack_MAX30102();
		Word_Data[i][0] = (((u16)data1 << 8) | data2);  //

		
		data1 = IIC_Read_Byte_MAX30102(0);	
		IIC_Ack_MAX30102();
		data2 = IIC_Read_Byte_MAX30102(0);
		if(1==no)
			IIC_NAck_MAX30102();	/* ���1���ֽڶ����CPU����NACK�ź�(����SDA = 1) */
		else
			IIC_Ack_MAX30102();
		Word_Data[i][1] = (((u16)data1 << 8) | data2); 

		no--;	
		i++;
	}
	/* ����I2C����ֹͣ�ź� */
	IIC_Stop_MAX30102();

cmd_fail: /* ����ִ��ʧ�ܺ��мǷ���ֹͣ�źţ�����Ӱ��I2C�����������豸 */
	/* ����I2C����ֹͣ�ź� */
	IIC_Stop_MAX30102();
}

void max30102_FIFO_ReadBytes(u8 Register_Address,u8* Data)
{	
	max30102_Bus_Read(REG_INTR_STATUS_1);
	max30102_Bus_Read(REG_INTR_STATUS_2);
	
	/* ��1��������I2C���������ź� */
	IIC_Start_MAX30102();

	/* ��2������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
	IIC_Send_Byte_MAX30102(max30102_WR_address | I2C_WR);	/* �˴���дָ�� */

	/* ��3��������ACK */
	if (IIC_Wait_Ack_MAX30102() != 0)
	{
		goto cmd_fail;	/* EEPROM������Ӧ�� */
	}

	/* ��4���������ֽڵ�ַ�� */
	IIC_Send_Byte_MAX30102((uint8_t)Register_Address);
	if (IIC_Wait_Ack_MAX30102() != 0)
	{
		goto cmd_fail;	/* EEPROM������Ӧ�� */
	}
	

	/* ��6������������I2C���ߡ����濪ʼ��ȡ���� */
	IIC_Start_MAX30102();

	/* ��7������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
	IIC_Send_Byte_MAX30102(max30102_WR_address | I2C_RD);	/* �˴��Ƕ�ָ�� */

	/* ��8��������ACK */
	if (IIC_Wait_Ack_MAX30102() != 0)
	{
		goto cmd_fail;	/* EEPROM������Ӧ�� */
	}

	/* ��9������ȡ���� */
	Data[0] = IIC_Read_Byte_MAX30102(1);	
	Data[1] = IIC_Read_Byte_MAX30102(1);	
	Data[2] = IIC_Read_Byte_MAX30102(1);	
	Data[3] = IIC_Read_Byte_MAX30102(1);
	Data[4] = IIC_Read_Byte_MAX30102(1);	
	Data[5] = IIC_Read_Byte_MAX30102(0);
	/* ���1���ֽڶ����CPU����NACK�ź�(����SDA = 1) */
	/* ����I2C����ֹͣ�ź� */
	IIC_Stop_MAX30102();

cmd_fail: /* ����ִ��ʧ�ܺ��мǷ���ֹͣ�źţ�����Ӱ��I2C�����������豸 */
	/* ����I2C����ֹͣ�ź� */
	IIC_Stop_MAX30102();

}

void max30102_init(void)
{
	
	IIC_Init_MAX30102();
	
	max30102_reset();
	
//	max30102_Bus_Write(REG_MODE_CONFIG, 0x0b);  //mode configuration : temp_en[3]      MODE[2:0]=010 HR only enabled    011 SP02 enabled
//	max30102_Bus_Write(REG_INTR_STATUS_2, 0xF0); //open all of interrupt
//	max30102_Bus_Write(REG_INTR_STATUS_1, 0x00); //all interrupt clear
//	max30102_Bus_Write(REG_INTR_ENABLE_2, 0x02); //DIE_TEMP_RDY_EN
//	max30102_Bus_Write(REG_TEMP_CONFIG, 0x01); //SET   TEMP_EN

//	max30102_Bus_Write(REG_SPO2_CONFIG, 0x47); //SPO2_SR[4:2]=001  100 per second    LED_PW[1:0]=11  16BITS

//	max30102_Bus_Write(REG_LED1_PA, 0x47); 
//	max30102_Bus_Write(REG_LED2_PA, 0x47); 
	
	
	
	max30102_Bus_Write(REG_INTR_ENABLE_1,0xd0);	// INTR setting // �����ⲿ�ж�����
	max30102_Bus_Write(REG_INTR_ENABLE_2,0x00);
	max30102_Bus_Write(REG_FIFO_WR_PTR,0x00);  	//FIFO_WR_PTR[4:0]
	max30102_Bus_Write(REG_OVF_COUNTER,0x00);  	//OVF_COUNTER[4:0]
	max30102_Bus_Write(REG_FIFO_RD_PTR,0x00);  	//FIFO_RD_PTR[4:0]
	max30102_Bus_Write(REG_FIFO_CONFIG,0x0f);  	//sample avg = 1, fifo rollover=false, fifo almost full = 17
	max30102_Bus_Write(REG_MODE_CONFIG,0x03);  	//0x02 for Red only, 0x03 for SpO2 mode 0x07 multimode LED
	max30102_Bus_Write(REG_SPO2_CONFIG,0x27);  	// SPO2_ADC range = 4096nA, SPO2 sample rate (100 Hz), LED pulseWidth (400uS)  
	max30102_Bus_Write(REG_LED1_PA,0x35);   	//Choose value for ~ 7mA for LED1
	max30102_Bus_Write(REG_LED2_PA,0x35);   	// Choose value for ~ 7mA for LED2
	max30102_Bus_Write(REG_PILOT_PA,0x0a);   	// Choose value for ~ 25mA for Pilot LED
//	max30102_Bus_Write(REG_TEMP_CONFIG,0x01);   
	max30102_Bus_Write(REG_PROX_INT_THRESH , 0x10);     //����Ѫ��LED�ķ�ֵ
	
//	// Interrupt Enable 1 Register. Set PPG_RDY_EN (data available in FIFO)
//	max30102_Bus_Write(0x2, 1<<6);

//	// FIFO configuration register
//	// SMP_AVE: 16 samples averaged per FIFO sample
//	// FIFO_ROLLOVER_EN=1
//	//max30102_Bus_Write(0x8,  1<<4);
//	max30102_Bus_Write(0x8, (0<<5) | 1<<4);

//	// Mode Configuration Register
//	// SPO2 mode
//	max30102_Bus_Write(0x9, 3);

//	// SPO2 Configuration Register
//	max30102_Bus_Write(0xa,
//			(3<<5)  // SPO2_ADC_RGE 2 = full scale 8192 nA (LSB size 31.25pA); 3 = 16384nA
//			| (1<<2) // sample rate: 0 = 50sps; 1 = 100sps; 2 = 200sps
//			| (3<<0) // LED_PW 3 = 411��s, ADC resolution 18 bits
//	);

//	// LED1 (red) power (0 = 0mA; 255 = 50mA)
//	max30102_Bus_Write(0xc, 0xb0);

//	// LED (IR) power
//	max30102_Bus_Write(0xd, 0xa0);
											
}

void max30102_reset(void)
{
	max30102_Bus_Write(REG_MODE_CONFIG,0x40);
	max30102_Bus_Write(REG_MODE_CONFIG,0x40);
}

/* д�Ĵ��� */
void maxim_max30102_write_reg(uint8_t uch_addr, uint8_t uch_data)
{
//  char ach_i2c_data[2];
//  ach_i2c_data[0]=uch_addr;
//  ach_i2c_data[1]=uch_data;
//	
//  IIC_WriteBytes(I2C_WRITE_ADDR, ach_i2c_data, 2);
	IIC_Write_One_Byte_MAX30102(I2C_WRITE_ADDR,uch_addr,uch_data);
}

/* ���Ĵ��� */
void maxim_max30102_read_reg(uint8_t uch_addr, uint8_t *puch_data)
{
//  char ch_i2c_data;
//  ch_i2c_data=uch_addr;
//  IIC_WriteBytes(I2C_WRITE_ADDR, &ch_i2c_data, 1);
//	
//  i2c.read(I2C_READ_ADDR, &ch_i2c_data, 1);
//  
//   *puch_data=(uint8_t) ch_i2c_data;
	IIC_Read_One_Byte_MAX30102(I2C_WRITE_ADDR,uch_addr,puch_data);
}

void maxim_max30102_read_fifo(uint32_t *pun_red_led, uint32_t *pun_ir_led)
{
	uint32_t un_temp;
	unsigned char uch_temp;
	char ach_i2c_data[6];
	*pun_red_led=0;
	*pun_ir_led=0;

  
  //read and clear status register
  maxim_max30102_read_reg(REG_INTR_STATUS_1, &uch_temp);
  maxim_max30102_read_reg(REG_INTR_STATUS_2, &uch_temp);
  
  IIC_ReadBytes_MAX30102(I2C_WRITE_ADDR,REG_FIFO_DATA,(u8 *)ach_i2c_data,6);
  
  un_temp=(unsigned char) ach_i2c_data[0];
  un_temp<<=16;
  *pun_red_led+=un_temp;
  un_temp=(unsigned char) ach_i2c_data[1];
  un_temp<<=8;
  *pun_red_led+=un_temp;
  un_temp=(unsigned char) ach_i2c_data[2];
  *pun_red_led+=un_temp;
  
  un_temp=(unsigned char) ach_i2c_data[3];
  un_temp<<=16;
  *pun_ir_led+=un_temp;
  un_temp=(unsigned char) ach_i2c_data[4];
  un_temp<<=8;
  *pun_ir_led+=un_temp;
  un_temp=(unsigned char) ach_i2c_data[5];
  *pun_ir_led+=un_temp;
  *pun_red_led&=0x03FFFF;  //Mask MSB [23:18]
  *pun_ir_led&=0x03FFFF;  //Mask MSB [23:18]
}



/**
 * @brief fifo����ȡ
 * @param output_data
 * @retval None
 */
void max30102_fifo_read(uint32_t *output_data)
{
	uint8_t receive_data[6];
	uint32_t data[2];
	// max30102_i2c_read(REG_FIFO_DATA, receive_data, 6);
	// IIC_ReadBytes_MAX30102(I2C_WRITE_ADDR,REG_FIFO_DATA,receive_data,6); // or try max30102_FIFO_ReadBytes
	max30102_FIFO_ReadBytes(REG_FIFO_DATA,receive_data); // or try max30102_FIFO_ReadBytes

	data[0] = ((receive_data[0] << 16 | receive_data[1] << 8 | receive_data[2]) & 0x03ffff);
	data[1] = ((receive_data[3] << 16 | receive_data[4] << 8 | receive_data[5]) & 0x03ffff);
	*output_data = data[0];
	*(output_data + 1) = data[1];
}


/**
 * @brief ��ȡ����
 * @param input_data cache_nums(���������������)
 * @retval (uint16_t)����
 */
uint16_t max30102_getHeartRate(float *input_data, uint16_t cache_nums)
{
	uint32_t input_data_sum_aver = 0;
	uint16_t i, temp;

	for (i = 0; i < cache_nums; i++)
	{
		input_data_sum_aver += *(input_data + i);
	}
	input_data_sum_aver = (float)input_data_sum_aver / cache_nums;
	for (i = 0; i < cache_nums; i++)
	{
		if ((*(input_data + i) > input_data_sum_aver) && (*(input_data + i + 1) < input_data_sum_aver))
		{
			temp = i;
			break;
		}
	}
	i++;
	for (; i < cache_nums; i++)
	{
		if ((*(input_data + i) > input_data_sum_aver) && (*(input_data + i + 1) < input_data_sum_aver))
		{
			temp = i - temp;
			break;
		}
	}
	if ((temp > 14) && (temp < 100))
	{
		return (uint16_t)(3000 / temp);
	}
	else
	{
		return 0;
	}
}

/**
 * @brief ��ȡѪ��
 * @param input_data red_input_data cache_nums(���������������)
 * @retval (float)Ѫ��
 */
float max30102_getSpO2(float *ir_input_data, float *red_input_data, uint16_t cache_nums)
{
	float ir_max = *ir_input_data, ir_min = *ir_input_data;
	float red_max = *red_input_data, red_min = *red_input_data;
	float R;
	uint16_t i;
	for (i = 1; i < cache_nums; i++)
	{
		if (ir_max < *(ir_input_data + i))
		{
			ir_max = *(ir_input_data + i);
		}
		if (ir_min > *(ir_input_data + i))
		{
			ir_min = *(ir_input_data + i);
		}
		if (red_max < *(red_input_data + i))
		{
			red_max = *(red_input_data + i);
		}
		if (red_min > *(red_input_data + i))
		{
			red_min = *(red_input_data + i);
		}
	}

	R = ((ir_max + ir_min) * (red_max - red_min)) / ((red_max + red_min) * (ir_max - ir_min));
	return ((-45.060f) * R * R + 30.354f * R + 94.845f);
}

/**
 * @brief MAX30102������
 * @param HeartRate(����) SpO2(Ѫ��) max30102_data fir_output
 * @retval (uint8_t)MAX30102_DATA_OK:������ȡ  (uint8_t)!MAX30102_DATA_OK:���ڶ�ȡ
 */

#define MAX_BRIGHTNESS 255
#define PPG_WAVE_COUNT 	4
#define PPG_MIN_COUNT 	0X3FFFF

//Continuously taking samples from MAX30102.  Heart rate and SpO2 are calculated every ST seconds
int32_t n_spo2; // Ѫ��ֵ
int8_t ch_spo2_valid;  //indicator to show if the SPO2 calculation is valid
int32_t n_heart_rate; //heart rate value
int8_t  ch_hr_valid;  //indicator to show if the heart rate calculation is valid

uint32_t un_min, un_max; 	//ԭʼ���ݵ����/��Сֵ

uint8_t peakfound_flag = 0; // �ҵ������־λ

uint8_t fingerfound_flag = 0; // ������־λ/�Ƿ�����ָ����

uint16_t i; // ����ֵ

extern uint32_t Time_ms;

float ir_ppgtemp, ir_ppgtemp_fir; // ir��ppg�ź�

uint8_t MAX30102_Get_DATA(int32_t *HeartRate,int32_t *SpO2,uint32_t max30102_data[2])
{	
	
	if(max30102_set_flag) // �ж��źŲ���
	{
		max30102_set_flag = 0;
		max30102_fifo_read(max30102_data); // ��ȡ����
		
		ir_ppgtemp = (float)max30102_data[0];
		ir_max30102_fir(&ir_ppgtemp, &ir_ppgtemp_fir); // �˲�
		
		if(max30102_data[0] > PPG_DATA_THRESHOLD && max30102_data[1] > PPG_DATA_THRESHOLD)
		{
			ppg_data_cache_IR[cache_counter] = max30102_data[0];
			ppg_data_cache_RED[cache_counter] = max30102_data[1];
			
			fingerfound_flag = 1; // ������־λ��1/����ָ����
			
			if(un_max > ir_ppgtemp_fir) // �Ҳ���
			{
				peakfound_flag = 1;
				un_max = 0;
			}
			else
			{
				peakfound_flag = 0;
				un_max = (uint32_t)ir_ppgtemp_fir;
			}
			
//			printf("IRac:%d   REDac:%d\r\n", max30102_data[0], max30102_data[1]);
			
			printf("@%d#", max30102_data[0]); // ���͸���λ����ppg����
			
			printf("Time_ms:%d\r\n", Time_ms);
			
			cache_counter++;

			printf("++%d   ", cache_counter);
			
			if (cache_counter >= BUFFER_SIZE) // �ռ�������
			{
				//calculate heart rate and SpO2 after BUFFER_SIZE samples (ST seconds of samples) using Robert's method
				maxim_heart_rate_and_oxygen_saturation(ppg_data_cache_IR, BUFFER_SIZE, ppg_data_cache_RED, &n_spo2, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid); 
				
				if(ch_hr_valid == 1 && ch_spo2_valid == 1)
				{
					*SpO2 = n_spo2;
					*HeartRate = n_heart_rate;
					
					printf("$%d,%d^",*HeartRate,*SpO2); // ���͸���λ������ֵ
				}
				else
				{
					*SpO2 = 0;
					*HeartRate = 0;
				}
				
				for(i = 100; i < 500; i++) // ������ǰ��100->Ŀ���Ƕ���500�������е�ǰ��100������,������100���²ɼ���ֵ
				{
					ppg_data_cache_IR[i - 100] = ppg_data_cache_IR[i];
					ppg_data_cache_RED[i - 100] = ppg_data_cache_RED[i];
					
				}
				
				cache_counter = 400;
				
				return MAX30102_DATA_OK;
			}
		}
		else // ���ݵ�����ֵ����ֵΪ��
		{
			*SpO2 = 0;
			*HeartRate = 0;
			cache_counter = 400;
			fingerfound_flag = 0; // ������־λ��0/����ָ����
		}
		
		
		
	}
	return !MAX30102_DATA_OK;
}



