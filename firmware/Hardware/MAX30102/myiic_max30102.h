#ifndef __MYIIC_MAX30102_H
#define __MYIIC_MAX30102_H
#include "gd32f30x.h"	
#include "DataType.h"	
   	   		   
//IIC所有操作函数
void IIC_Init_MAX30102(void);                //初始化IIC的IO口				 
void IIC_Start_MAX30102(void);				//发送IIC开始信号
void IIC_Stop_MAX30102(void);	  			//发送IIC停止信号
void IIC_Send_Byte_MAX30102(u8 txd);			//IIC发送一个字节
u8 IIC_Read_Byte_MAX30102(unsigned char ack);//IIC读取一个字节
u8 IIC_Wait_Ack_MAX30102(void); 				//IIC等待ACK信号
void IIC_Ack_MAX30102(void);					//IIC发送ACK信号
void IIC_NAck_MAX30102(void);				//IIC不发送ACK信号
  
void IIC_Read_One_Byte_MAX30102(u8 daddr,u8 addr,u8* data);
void IIC_Write_One_Byte_MAX30102(u8 daddr,u8 addr,u8 data);

void IIC_WriteBytes_MAX30102(u8 WriteAddr,u8* data,u8 dataLength);
void IIC_ReadBytes_MAX30102(u8 deviceAddr, u8 writeAddr,u8* data,u8 dataLength);

#endif
















