#include "myiic.h"
#include "TIMER5_Delay.h"
#include "DataType.h"

/* 只能说江科协大佬是真的牛逼 */

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

//初始化IIC
void IIC_Init(void)
{	
	//开启rcc时钟	
	rcu_periph_clock_enable(RCU_GPIOB);

	//PB0-SCL, PB1-SDA
	gpio_init(GPIOB, GPIO_MODE_OUT_OD, GPIO_OSPEED_MAX, GPIO_PIN_0);
	gpio_init(GPIOB, GPIO_MODE_OUT_OD, GPIO_OSPEED_MAX, GPIO_PIN_1);
	
	MyI2C_W_SCL(1);
	MyI2C_W_SDA(1);
}


//产生IIC起始信号
void IIC_Start(void)
{
	MyI2C_W_SDA(1);
	MyI2C_W_SCL(1);
	MyI2C_W_SDA(0);
	MyI2C_W_SCL(0);
}	  
//产生IIC停止信号
void IIC_Stop(void)
{
	MyI2C_W_SDA(0);
	MyI2C_W_SCL(1);
	MyI2C_W_SDA(1);							   	
}

//等待应答信号到来
//返回值：1，接收应答失败
//返回值：0，接收应答成功
u8 IIC_Wait_Ack(void)
{
	uint8_t AckBit;
	MyI2C_W_SDA(1);
	MyI2C_W_SCL(1);
	AckBit = MyI2C_R_SDA();
	MyI2C_W_SCL(0);
	return AckBit;
} 

//产生ACK应答
void IIC_Ack(void)
{
	MyI2C_W_SDA(0);
	MyI2C_W_SCL(1);
	MyI2C_W_SCL(0);
}

//不产生ACK应答		    
void IIC_NAck(void)
{
	MyI2C_W_SDA(1);
	MyI2C_W_SCL(1);
	MyI2C_W_SCL(0);
}

//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
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

//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
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
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK   
    return Byte;
}



























