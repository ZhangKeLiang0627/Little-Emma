#include "myiic.h"
#include "TIMER5_Delay.h"
#include "DataType.h"

/* ֻ��˵����Э���������ţ�� */

void MyI2C_W_SCL(uint8_t BitValue)
{
	if(BitValue) gpio_bit_write(GPIOB, GPIO_PIN_0, SET);
	else gpio_bit_write(GPIOB, GPIO_PIN_0, RESET);
	delay_us(5);
}

void MyI2C_W_SDA(uint8_t BitValue)
{
	if(BitValue) gpio_bit_write(GPIOB, GPIO_PIN_1, SET);
	else gpio_bit_write(GPIOB, GPIO_PIN_1, RESET);
	delay_us(5);
}

uint8_t MyI2C_R_SDA(void)
{
	uint8_t BitValue;
	BitValue = gpio_input_bit_get(GPIOB, GPIO_PIN_1);
	delay_us(5);
	return BitValue;
}

//��ʼ��IIC
void IIC_Init(void)
{	
	//����rccʱ��	
	rcu_periph_clock_enable(RCU_GPIOB);

	//PB0-SCL, PB1-SDA
	gpio_init(GPIOB, GPIO_MODE_OUT_OD, GPIO_OSPEED_MAX, GPIO_PIN_0);
	gpio_init(GPIOB, GPIO_MODE_OUT_OD, GPIO_OSPEED_MAX, GPIO_PIN_1);
	
	MyI2C_W_SCL(1);
	MyI2C_W_SDA(1);
}


//����IIC��ʼ�ź�
void IIC_Start(void)
{
	MyI2C_W_SDA(1);
	MyI2C_W_SCL(1);
	MyI2C_W_SDA(0);
	MyI2C_W_SCL(0);
}	  
//����IICֹͣ�ź�
void IIC_Stop(void)
{
	MyI2C_W_SDA(0);
	MyI2C_W_SCL(1);
	MyI2C_W_SDA(1);							   	
}

//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//����ֵ��0������Ӧ��ɹ�
u8 IIC_Wait_Ack(void)
{
	uint8_t AckBit;
	MyI2C_W_SDA(1);
	MyI2C_W_SCL(1);
	AckBit = MyI2C_R_SDA();
	MyI2C_W_SCL(0);
	return AckBit;
} 

//����ACKӦ��
void IIC_Ack(void)
{
	MyI2C_W_SDA(0);
	MyI2C_W_SCL(1);
	MyI2C_W_SCL(0);
}

//������ACKӦ��		    
void IIC_NAck(void)
{
	MyI2C_W_SDA(1);
	MyI2C_W_SCL(1);
	MyI2C_W_SCL(0);
}

//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void IIC_Send_Byte(u8 Byte)
{                        
	uint8_t i;
	for (i = 0; i < 8; i ++)
	{
		MyI2C_W_SDA(Byte & (0x80 >> i));
		MyI2C_W_SCL(1);
		MyI2C_W_SCL(0);
	} 
} 

//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	uint8_t i, Byte = 0x00;
	MyI2C_W_SDA(1);
	for (i = 0; i < 8; i ++)
	{
		MyI2C_W_SCL(1);
		if (MyI2C_R_SDA() == 1){Byte |= (0x80 >> i);}
		MyI2C_W_SCL(0);
	}				 
    if (!ack)
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK   
    return Byte;
}



























