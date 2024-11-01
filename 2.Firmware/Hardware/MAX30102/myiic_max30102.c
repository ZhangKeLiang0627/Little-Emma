#include "myiic_MAX30102.h"
#include "Timer5_Delay.h"
#include "DataType.h"


void IIC_SCL_MAX30102(uint8_t BitValue)
{
	if(BitValue) gpio_bit_write(GPIOB, GPIO_PIN_10, SET);
	else gpio_bit_write(GPIOB, GPIO_PIN_10, RESET);
	delay_us(5);
}

void IIC_SDA_MAX30102(uint8_t BitValue)
{
	if(BitValue) gpio_bit_write(GPIOB, GPIO_PIN_11, SET);
	else gpio_bit_write(GPIOB, GPIO_PIN_11, RESET);
	delay_us(5);
}

uint8_t READ_SDA_MAX30102(void)
{
	uint8_t BitValue;
	BitValue = gpio_input_bit_get(GPIOB, GPIO_PIN_11);
	delay_us(5);
	return BitValue;
}

//初始化IIC
void IIC_Init_MAX30102(void)
{	
	//开启rcc时钟	
	rcu_periph_clock_enable(RCU_GPIOB);

	//PB10-SCL, PB11-SDA
	gpio_init(GPIOB, GPIO_MODE_OUT_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_10);
	gpio_init(GPIOB, GPIO_MODE_OUT_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_11);
	
	IIC_SCL_MAX30102(1);
	IIC_SDA_MAX30102(1);
}
//产生IIC起始信号
void IIC_Start_MAX30102(void)
{
	IIC_SDA_MAX30102(1);
	IIC_SCL_MAX30102(1);
	IIC_SDA_MAX30102(0);
	IIC_SCL_MAX30102(0);
}	  
//产生IIC停止信号
void IIC_Stop_MAX30102(void)
{
	IIC_SDA_MAX30102(0);
	IIC_SCL_MAX30102(1);
	IIC_SDA_MAX30102(1);							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC_Wait_Ack_MAX30102(void)
{
	uint8_t AckBit;
	IIC_SDA_MAX30102(1);
	IIC_SCL_MAX30102(1);
	AckBit = READ_SDA_MAX30102();
	IIC_SCL_MAX30102(0);
	return AckBit; 
} 
//产生ACK应答
void IIC_Ack_MAX30102(void)
{
	IIC_SDA_MAX30102(0);
	IIC_SCL_MAX30102(1);
	IIC_SCL_MAX30102(0);
}
//不产生ACK应答		    
void IIC_NAck_MAX30102(void)
{
	IIC_SDA_MAX30102(1);
	IIC_SCL_MAX30102(1);
	IIC_SCL_MAX30102(0);
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte_MAX30102(u8 Byte)
{                        
	uint8_t i;
	for (i = 0; i < 8; i ++)
	{
		IIC_SDA_MAX30102(Byte & (0x80 >> i));
		IIC_SCL_MAX30102(1);
		IIC_SCL_MAX30102(0);
	} 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 IIC_Read_Byte_MAX30102(unsigned char ack)
{
	uint8_t i, Byte = 0x00;
	IIC_SDA_MAX30102(1);
	for (i = 0; i < 8; i ++)
	{
		IIC_SCL_MAX30102(1);
		if (READ_SDA_MAX30102() == 1){Byte |= (0x80 >> i);}
		IIC_SCL_MAX30102(0);
	}				 
    if (!ack)
        IIC_NAck_MAX30102();//发送nACK
    else
        IIC_Ack_MAX30102(); //发送ACK   
    return Byte;
}


void IIC_Read_One_Byte_MAX30102(u8 daddr,u8 addr,u8* data)
{				  	  	    																 
    IIC_Start_MAX30102();  
	
	IIC_Send_Byte_MAX30102(daddr);		// 发送写命令
	IIC_Wait_Ack_MAX30102();
	IIC_Send_Byte_MAX30102(addr);		// 发送地址
	IIC_Wait_Ack_MAX30102();		 
	IIC_Start_MAX30102();  	 	   
	IIC_Send_Byte_MAX30102(daddr|0X01);	// 进入接收模式			   
	IIC_Wait_Ack_MAX30102();	 
    *data = IIC_Read_Byte_MAX30102(0);		   
    IIC_Stop_MAX30102();					// 产生停止信号	    
}

void IIC_Write_One_Byte_MAX30102(u8 daddr,u8 addr,u8 data)
{				   	  	    																 
    IIC_Start_MAX30102();  
	
	IIC_Send_Byte_MAX30102(daddr);	    // 发送写命令
	IIC_Wait_Ack_MAX30102();
	IIC_Send_Byte_MAX30102(addr);		// 发送地址
	IIC_Wait_Ack_MAX30102();	   	 										  		   
	IIC_Send_Byte_MAX30102(data);     	// 发送一个字节						   
	IIC_Wait_Ack_MAX30102();  		    	   
    IIC_Stop_MAX30102();					// 产生停止条件
	delay_ms(10);	 
}

void IIC_WriteBytes_MAX30102(u8 WriteAddr,u8* data,u8 dataLength)
{		
	u8 i;	
    IIC_Start_MAX30102();  

	IIC_Send_Byte_MAX30102(WriteAddr);	// 发送写命令
	IIC_Wait_Ack_MAX30102();
	
	for(i=0;i<dataLength;i++)
	{
		IIC_Send_Byte_MAX30102(data[i]);
		IIC_Wait_Ack_MAX30102();
	}				    	   
    IIC_Stop_MAX30102();				// 产生停止条件 
	delay_ms(10);	 
}

void IIC_ReadBytes_MAX30102(u8 deviceAddr, u8 writeAddr,u8* data,u8 dataLength)
{		
	u8 i;	
    IIC_Start_MAX30102();  

	IIC_Send_Byte_MAX30102(deviceAddr);
	IIC_Wait_Ack_MAX30102();
	IIC_Send_Byte_MAX30102(writeAddr);
	IIC_Wait_Ack_MAX30102();
	IIC_Send_Byte_MAX30102(deviceAddr|0X01);		   
	IIC_Wait_Ack_MAX30102();
	
	for(i=0;i<dataLength-1;i++)
	{
		data[i] = IIC_Read_Byte_MAX30102(1);
	}		
	data[dataLength-1] = IIC_Read_Byte_MAX30102(0);	
    IIC_Stop_MAX30102();
	delay_ms(10);	 
}





















