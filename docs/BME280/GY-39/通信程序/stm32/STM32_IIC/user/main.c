#include "stm32f10x.h"
#include "LED.h"
#include "IIC.h"
#include "delay.h"
#include "usart.h"
#include "stdio.h"
/*
Keil: MDK5.10.0.2
MCU:stm32f103c8
硬件接法：
GY-39---STM32
SCL---PB6
SDA---PB7
STM32---FT232
TX---RX
RX---TX
软件说明:
该程序采用IIC对GY-39进行读取数据,然后串口打印，波特率为115200

注：
	IIC时钟频率需低于40Khz；
	中断函数位于stm32f10x_it.c
	下好程序后如没有数据输出请复位stm32
联系方式：
http://shop62474960.taobao.com/?spm=a230r.7195193.1997079397.2.9qa3Ky&v=1
*/

int fputc(int ch, FILE *f)
{
 while (!(USART1->SR & USART_FLAG_TXE));
 USART_SendData(USART1, (unsigned char) ch);// USART1 可以换成 USART2 等
 return (ch);
}
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_X;
  
  /* 4个抢占优先级，4个响应优先级 */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  /*抢占优先级可打断中断级别低的中断*/
	/*响应优先级按等级执行*/
	NVIC_X.NVIC_IRQChannel = USART1_IRQn;//中断向量
  NVIC_X.NVIC_IRQChannelPreemptionPriority = 0;//抢占优先级
  NVIC_X.NVIC_IRQChannelSubPriority = 0;//响应优先级
  NVIC_X.NVIC_IRQChannelCmd = ENABLE;//使能中断响应
  NVIC_Init(&NVIC_X);
}
typedef struct
{
    uint32_t P;
    uint16_t Temp;
    uint16_t Hum;
    uint16_t Alt;
} bme;

bme Bme={0,0,0,0};

int main(void)
{
	u8  raw_data[13]={0};
	uint16_t data_16[2]={0};
	uint32_t Lux;
	delay_init(72);
	LED_Int(GPIOB,GPIO_Pin_9,RCC_APB2Periph_GPIOB);
	NVIC_Configuration();
	Usart_Int(115200);
	I2C_GPIO_Config();
	delay_ms(100);//等待模块初始化完成
	while(1)
	{
			if(Single_ReadI2C(0xb6,0x04,raw_data,10))
			{
				Bme.Temp=(raw_data[0]<<8)|raw_data[1];
				data_16[0]=(((uint16_t)raw_data[2])<<8)|raw_data[3];
				data_16[1]=(((uint16_t)raw_data[4])<<8)|raw_data[5];
				Bme.P=(((uint32_t)data_16[0])<<16)|data_16[1];
				Bme.Hum=(raw_data[6]<<8)|raw_data[7];
				Bme.Alt=(raw_data[8]<<8)|raw_data[9];
			}
			if(Single_ReadI2C(0xb6,0x00,raw_data,4))
			data_16[0]=(((uint16_t)raw_data[0])<<8)|raw_data[1];
			data_16[1]=(((uint16_t)raw_data[2])<<8)|raw_data[3];
			Lux=(((uint32_t)data_16[0])<<16)|data_16[1];
			
		  printf("Temp: %.2f  DegC  ",(float)Bme.Temp/100);
		  printf("  P: %.2f  Pa ",(float)Bme.P/100);
			printf("  Hum: %.2f   ",(float)Bme.Hum/100);
		  printf("  Alt: %.2f  m\r\n ",(float)Bme.Alt);
			printf("\r\n Lux: %.2f  lux\r\n ",(float)Lux/100);  
			delay_ms(200);
			
	}		
}
