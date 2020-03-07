#include "stm32f10x.h"
#include "delay.h"
#include "usart.h"
#include "string.h"
 #include "LED.h"
 #include "stdio.h"
 /*
 Keil: MDK5.10.0.2
MCU:stm32f103c8
硬件接法：
GY-39---STM32
1、GY-39_RX---STM32_TX,STM32复位将发送A5 82 27 给模块
2、STM32_TX---FT232,STM32将数据上传给上位机
3、GY-39_TX---STM32_RX，接收模块数据
软件说明:
该程序采用串口方式获取模块数据，波特率9600

注：模块波特率需和该程序波特率一样为9600，中断函数位于stm32f10x_it.c
联系方式：
http://shop62474960.taobao.com/?spm=a230r.7195193.1997079397.2.9qa3Ky&v=1
*/
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
void send_com(u8 data)
{
	u8 bytes[3]={0};
	bytes[0]=0xa5;
	bytes[1]=data;//功能字节
	USART_Send(bytes,3);//发送帧头、功能字节、校验和
}
typedef struct
{
    uint32_t P;
    uint16_t Temp;
    uint16_t Hum;
    uint16_t Alt;
} bme;

int fputc(int ch, FILE *f)
{
 while (!(USART1->SR & USART_FLAG_TXE));
 USART_SendData(USART1, (unsigned char) ch);// USART1 可以换成 USART2 等
 return (ch);
}
int main(void)
{
  u8 sum=0,i=0;
	int16_t data=0;
	uint16_t data_16[2]={0};
	bme Bme={0,0,0,0};
	delay_init(72);
	NVIC_Configuration();
	Usart_Int(9600);
	delay_ms(100);//等待模块初始化完成
	send_com(0x82);//发送读气压温湿度指令
	while(1)
	{
		if(Receive_ok)//串口接收完毕
		{
			for(sum=0,i=0;i<(raw_data[3]+4);i++)//rgb_data[3]=3
			sum+=raw_data[i];
			if(sum==raw_data[i])//校验和判断
			{
				Bme.Temp=(raw_data[4]<<8)|raw_data[5];
				data_16[0]=(((uint16_t)raw_data[6])<<8)|raw_data[7];
				data_16[1]=(((uint16_t)raw_data[8])<<8)|raw_data[9];
				Bme.P=(((uint32_t)data_16[0])<<16)|data_16[1];
        Bme.Hum=(raw_data[10]<<8)|raw_data[11];
        Bme.Alt=(raw_data[12]<<8)|raw_data[13]; 
				send_3out(&raw_data[4],10,0x45);//上传给上位机
				
//			  printf("Temp: %.2f  DegC  ",(float)Bme.Temp/100);
//		    printf("  P: %.2f  Pa ",(float)Bme.P/100);
//			  printf("  Hum: %.2f   ",(float)Bme.Hum/100);
//		    printf("  Alt: %.2f  m\r\n ",(float)Bme.Alt);
			}
			Receive_ok=0;//处理数据完毕标志
		}
	}
}
