#include <reg52.h>
#include "usart.h"
#include "iic.h"  
/*
硬件接法：
GY-39---c51
1、GY-39_RX---c51_TX,c51复位将发送A5 82 27 给模块
2、c51_TX---FT232,STM32将数据上传给上位机
3、GY-39_TX---c51_RX，接收模块角度数据
软件说明:
该程序采用串口方式获取模块数据，波特率9600
所以得用上位机先将模块串口设置成9600，然后再进行以上操作：
指令:A5 AE 53,模块需复位生效

注：中断函数位于stc_it.c
联系方式：
http://shop62474960.taobao.com/?spm=a230r.7195193.1997079397.2.9qa3Ky&v=1
*/
void send_com(u8 datas)
{
	u8 bytes[3]={0};
	bytes[0]=0xa5;
	bytes[1]=datas;//功能字节
	USART_Send(bytes,3);//发送帧头、功能字节、校验和
}
typedef struct
{
    uint32_t P;
    uint16_t Temp;
    uint16_t Hum;
    uint16_t Alt;
} bme;

int main(void)
{
   uint16_t data_16[2]={0};
	 bme Bme={0,0,0,0};
    u8 sum=0,i=0;
	Usart_Int(9600);
	send_com(0x82);//发送读方位角指令
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
				
			}
			Receive_ok=0;//处理数据完毕标志
		}	
	}
}
