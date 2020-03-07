#include <Wire.h>
//#include <i2cmaster.h>
/////////////////////////////////
/*
GY-39-----mini
VCC----VCC
SCL----A5
SDA----A4
GND--GND
*/


//////////////////////////////////
#define uint16_t unsigned int
#define iic_add  0x5b
typedef struct
{
    uint32_t P;
    uint16_t Temp;
    uint16_t Hum;
    uint16_t Alt;
} bme;

bme Bme;
  uint32_t Lux;

void i2c_speed(uint16_t scl_speed)
{
  /* initialize TWI clock: 40 kHz clock, TWPS = 0 => prescaler = 1 */
  
  TWSR = 0;                         /* no prescaler */
  TWBR = ((F_CPU/scl_speed)-16)/2;  /* must be > 10 for stable operation */

}
void setup() {
       Serial.begin(115200);
       Wire.begin();
       i2c_speed(40000);
       delay(1); 
}
void loop() {

    get_bme();
    Serial.print("Temp: ");
    Serial.print( (float)Bme.Temp/100);
    Serial.print(" DegC  PRESS : ");
    Serial.print( ((float)Bme.P)/100);
    Serial.print(" Pa  HUM : ");
    Serial.print( (float)Bme.Hum/100);
    Serial.print(" % ALT:");
    Serial.print( Bme.Alt);
    Serial.println("m");
    get_lux();
    Serial.print( "Lux: ");
    Serial.print( ((float)Lux)/100);
    Serial.println(" lux");
    delay(200); 
}
void get_bme(void)
{
    uint16_t data_16[2]={0};
    uint8_t data[10]={0};
   iic_read(0x04,data,10);
    //iic_read2(0x04,data,10);
   Bme.Temp=(data[0]<<8)|data[1];
   data_16[0]=(data[2]<<8)|data[3];
   data_16[1]=(data[4]<<8)|data[5];
   Bme.P=(((uint32_t)data_16[0])<<16)|data_16[1];
   Bme.Hum=(data[6]<<8)|data[7];
   Bme.Alt=(data[8]<<8)|data[9];
}
void get_lux(void)
{     
    uint16_t data_16[2]={0};
    uint8_t data[10]={0};
    //iic_read2(0x00,data,4);
    iic_read(0x00,data,4);
    data_16[0]=(data[0]<<8)|data[1];
    data_16[1]=(data[2]<<8)|data[3];
    Lux=(((uint32_t)data_16[0])<<16)|data_16[1];

}
void iic_read(unsigned char reg,unsigned char *data,uint8_t len )//#include <Wire.h>
{
   Wire.beginTransmission(iic_add);  
   Wire.write(reg); 
   Wire.endTransmission(false);
   delayMicroseconds(10);
   if(len>4)
     Wire.requestFrom(iic_add,10);     
   else
     Wire.requestFrom(iic_add,4);
   for (uint8_t i = 0; i < len; i++)
    {  
       data[i] = Wire.read(); 
   }
  
}
//void iic_read2(unsigned char add,unsigned char *data,unsigned char len)//#include <i2cmaster.h>
//{
//  i2c_start_wait(0xb6);
//   i2c_write(add);
//   i2c_start_wait(0xb6+1);
//   while(len-1)
//   {
//    *data++=i2c_readAck();
//    len--;
//    }
//    *data=i2c_readNak();
//    i2c_stop();
//}
