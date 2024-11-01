#include "MySPI.h"
#include "sys.h"
#include "DataType.h"
#include "UART0.h"

void MySPI0_Init(void)
{
	spi_parameter_struct spi_init_structure;
	
	rcu_periph_clock_enable(RCU_SPI0);
	rcu_periph_clock_enable(RCU_GPIOA);
	
	gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_MAX, GPIO_PIN_5 | GPIO_PIN_7);//PA5-CLK&PA7-MOSI(SDA)
	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_MAX, GPIO_PIN_4);//PA4-CS
	
	spi_init_structure.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE;
	spi_init_structure.device_mode = SPI_MASTER;
	spi_init_structure.endian = SPI_ENDIAN_MSB;
	spi_init_structure.frame_size = SPI_FRAMESIZE_8BIT;
	spi_init_structure.nss = SPI_NSS_SOFT;
	spi_init_structure.prescale = SPI_PSC_4;//spi+dma ����������ֻ����͵���һ��spi��Ƶ ����ͽ����
	spi_init_structure.trans_mode = SPI_TRANSMODE_BDTRANSMIT;
	
	spi_init(SPI0, &spi_init_structure);
	
	spi_enable(SPI0);
	
	spi_dma_enable(SPI0, SPI_DMA_TRANSMIT);
}

void LCD_Writ_Bus(u8 dat)
{
	LCD_CS_Clr();
	while(spi_i2s_flag_get(SPI0, SPI_FLAG_TBE) == RESET);
	spi_i2s_data_transmit(SPI0, dat);
//	while(spi_i2s_flag_get(SPI0 ,SPI_FLAG_TRANS) == SET);
	delay_us(1);//���delay����Ҫ����ɾȥ
	LCD_CS_Set();
}


void MySPI1_Init(void)
{ 
	spi_parameter_struct spi_init_structure;
	
	rcu_periph_clock_enable(RCU_SPI1);
	
	gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);//PB13-CLK&PB14-MISO&PB15-MOSI(SDA)

	spi_init_structure.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE;
	spi_init_structure.device_mode = SPI_MASTER;
	spi_init_structure.endian = SPI_ENDIAN_MSB;
	spi_init_structure.frame_size = SPI_FRAMESIZE_8BIT;
	spi_init_structure.nss = SPI_NSS_SOFT;
	spi_init_structure.prescale = SPI_PSC_256;
	spi_init_structure.trans_mode = SPI_TRANSMODE_FULLDUPLEX;
	spi_init(SPI1, &spi_init_structure);
	
	spi_enable(SPI1);
}


//SPI1 ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
u8 SPI1_ReadWriteByte(u8 TxData)
{		 			 
 
	while (spi_i2s_flag_get(SPI1, SPI_FLAG_TBE) == RESET); //�ȴ���������  
	
	spi_i2s_data_transmit(SPI1, TxData); //ͨ������SPIx����һ��byte������
		
	while (spi_i2s_flag_get(SPI1, SPI_FLAG_RBNE) == RESET); //�ȴ�������һ��byte  
 
	return spi_i2s_data_receive(SPI1); //����ͨ��SPIx������յ�����	
 		    
}


//SPI1�ٶ����ú���
//SpeedSet:0~7
//SPI�ٶ�=fAPB2/2^(SpeedSet+1)
//fAPB2ʱ��һ��Ϊ84Mhz
void SPI1_SetSpeed(spi_parameter_struct* spi_struct)
{
	spi_struct->clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE;
	spi_struct->device_mode = SPI_MASTER;
	spi_struct->endian = SPI_ENDIAN_MSB;
	spi_struct->frame_size = SPI_FRAMESIZE_8BIT;
	spi_struct->nss = SPI_NSS_SOFT;
	spi_struct->trans_mode = SPI_TRANSMODE_FULLDUPLEX;
	spi_init(SPI1, spi_struct);
	
	spi_enable(SPI1);
} 








