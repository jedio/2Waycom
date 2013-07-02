#include <SPI.h>
#include <RADIO.h>
 
 
 byte RX_Buffer[10]={0};
byte size,i;

 
 
 void setup()
{
  Serial.begin (9600);
  Serial.println ("start");
  
     

  
  SPI.begin ();
  SPI.setClockDivider(SPI_CLOCK_DIV4);   
  SPI.setBitOrder(MSBFIRST);   
  SPI.setDataMode(SPI_MODE0) ;
  CC110L.GDO_Set();
  CC110L.Reset();
  CC110L.RegConfig();
  //CC110L.PATABLE();
  CC110L.SetReceive();
  
}




void loop()
{
  if(CC110L.CheckReceiveFlag())
  {
    size=CC110L.ReceiveData(RX_Buffer);
    Serial.println(size);
    for(i=0;i<10;i++)
    {
      Serial.print(RX_Buffer[i],DEC);
     Serial.print(' ');
    }
    Serial.println("");
    CC110L.SetReceive();
  }
}











