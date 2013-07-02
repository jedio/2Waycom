#include <SPI.h>
#include <RADIO.h>

#define size 61

void RegConfigPrint();
byte RegBuffer[50]={0};
byte TX_Buffer[size]={0};
byte i;
int sensorValue = 0;  // variable to store the value coming from the sensor


void setup (void)
{
      
  
  Serial.begin (9600);
  Serial.println ();
  
 pinMode(A0, OUTPUT);     

  
  SPI.begin ();
  SPI.setClockDivider(SPI_CLOCK_DIV4);   
  SPI.setBitOrder(MSBFIRST);   
  SPI.setDataMode(SPI_MODE0) ;
  CC110L.GDO_Set();
  CC110L.Reset();
  CC110L.RegConfig();
  CC110L.PATABLE();

// SpiWriteReg(0x09,0x10);
//CC110L.WriteSingleReg(0x09,0x09);
//CC110L.WriteSingleReg(0x06,0xC9);
CC110L.ReadBurstReg(0,RegBuffer,47);

for( i=0;i<size;i++)
  {
     TX_Buffer[i]=i;
  }


}  // end of setup





void loop (void)
{
  
  
sensorValue = analogRead(A0);    


  // apply the calibration to the sensor reading
  sensorValue = map(sensorValue, 0, 1023, 0, 255);
  
  
  
//RegConfigPrint();
TX_Buffer[1]=sensorValue;
TX_Buffer[2]=0X0B;
TX_Buffer[3]=0X0C;
TX_Buffer[4]=0X0D;
TX_Buffer[5]=0X0E;
TX_Buffer[6]=0X0F;

CC110L.SendData(TX_Buffer,size);
for (int i = 0 ; i<size ;i++)
{
//Serial.print(TX_Buffer[i]);
}
//digitalWrite(A0, HIGH);   // set the LED on
//  delay(1000);              // wait for a second
//  Serial.println("ON");
//digitalWrite(A0, LOW);    // set the LED off
//  delay(1000);              // wait for a second
//Serial.println("OFF");
Serial.println(TX_Buffer[1]);
}





void RegConfigPrint()
{
  for(int i = 0; i<=50;i++)
{

  Serial.print ("REG ADD :");
  Serial.print (i,HEX);
  Serial.print (" ");
  Serial.print("Value : ");
  Serial.print (RegBuffer[i],HEX);
  Serial.println(" ");

  delay (1000);  // 1 second delay 

}
}
