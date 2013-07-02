

#include <SPI.h>
#include <RADIO.h>
 
 
 byte RX_Buffer[61]={0};
byte size,i;

 
 
 void setup()
{
  Serial.begin (9600);
//  Serial.println ();
  
     

  
  SPI.begin ();
  SPI.setClockDivider(SPI_CLOCK_DIV4);   
  SPI.setBitOrder(MSBFIRST);   
  SPI.setDataMode(SPI_MODE0) ;
  CC110L.GDO_Set();
  CC110L.Reset();
  //CC110L.RegConfig();
  //CC110L.PATABLE();
  CC110L.SetReceive();
  
}




void loop()
{
  if(CC110L.CheckReceiveFlag())
  {
    size=CC110L.ReceiveData(RX_Buffer);
  Serial.print("size=");
  Serial.println(size);
    for(i=0;i<20;i++)
    {
      Serial.print(RX_Buffer[i],DEC);
//      Serial.print(' ',BYTE);
    }
    Serial.println("");
    CC110L.SetReceive();
  }
}











