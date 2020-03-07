/////////////////////
/*
GY39----MINI
VCC----VCC
GND----GND
1:RX---TX,send A5 02 A7 to GY-39
2:TX---RX
3:MINI_TX---FT232_RX
*/
//////////////////
unsigned char Re_buf[15],counter=0;
unsigned char sign=0;
typedef struct
{
    uint32_t P;
    uint16_t Temp;
    uint16_t Hum;
    uint16_t Alt;
} bme;
void setup() {
 
   Serial.begin(9600);  
  delay(1);    
  Serial.write(0XA5); 
  Serial.write(0X02);    //初始化,连续输出模式
  Serial.write(0XA7);    //初始化,连续输出模式
}

void loop() {
  unsigned char i=0,sum=0;
   uint16_t data_16[2]={0};
    bme Bme;
  if(sign)
  {   
  
     for(i=0;i<14;i++)
      sum+=Re_buf[i]; 
     if(sum==Re_buf[i] )        //检查帧头，帧尾
     {  	 
       Bme.Temp=(Re_buf[4]<<8)|Re_buf[5];
        data_16[0]=(Re_buf[6]<<8)|Re_buf[7];
        data_16[1]=(Re_buf[8]<<8)|Re_buf[9];
       Bme.P=(((uint32_t)data_16[0])<<16)|data_16[1];
       Bme.Hum=(Re_buf[10]<<8)|Re_buf[11];
       Bme.Alt=(Re_buf[12]<<8)|Re_buf[13];      
       Serial.print("Temp: ");
       Serial.print( (float)Bme.Temp/100);
       Serial.print(" DegC  PRESS : ");
       Serial.print( ((float)Bme.P)/100);
       Serial.print(" Pa  HUM : ");
       Serial.print( (float)Bme.Hum/100);
       Serial.print(" % ALT:");
       Serial.print( Bme.Alt);
       Serial.println("m");
 
    
    sign=0;        
   }
  } 

}
void serialEvent() {

 
      while (Serial.available()) {   
      Re_buf[counter]=(unsigned char)Serial.read();
      if(counter==0&&Re_buf[0]!=0x5A) return;      // 检查帧头         
      counter++;       
      if(counter==15)                //接收到数据
      {    
         counter=0;                 //重新赋值，准备下一帧数据的接收 
         sign=1;
      }      
      }

}
