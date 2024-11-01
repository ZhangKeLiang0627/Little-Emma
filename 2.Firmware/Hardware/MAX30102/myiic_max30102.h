#ifndef __MYIIC_MAX30102_H
#define __MYIIC_MAX30102_H
#include "gd32f30x.h"	
#include "DataType.h"	
   	   		   
//IIC���в�������
void IIC_Init_MAX30102(void);                //��ʼ��IIC��IO��				 
void IIC_Start_MAX30102(void);				//����IIC��ʼ�ź�
void IIC_Stop_MAX30102(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte_MAX30102(u8 txd);			//IIC����һ���ֽ�
u8 IIC_Read_Byte_MAX30102(unsigned char ack);//IIC��ȡһ���ֽ�
u8 IIC_Wait_Ack_MAX30102(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack_MAX30102(void);					//IIC����ACK�ź�
void IIC_NAck_MAX30102(void);				//IIC������ACK�ź�
  
void IIC_Read_One_Byte_MAX30102(u8 daddr,u8 addr,u8* data);
void IIC_Write_One_Byte_MAX30102(u8 daddr,u8 addr,u8 data);

void IIC_WriteBytes_MAX30102(u8 WriteAddr,u8* data,u8 dataLength);
void IIC_ReadBytes_MAX30102(u8 deviceAddr, u8 writeAddr,u8* data,u8 dataLength);

#endif
















