//Timer interrupt Test trial
//Jeididah Oyeyemi
//june 29 2013
#include <SPI.h>
#include <RADIO.h>
 
int sensorValue = 0;  // variable to store the value coming from the sensor
int mode;//
int  Speed=7812;
void RegConfigPrint();
//innitializing all buffers
byte RegBuffer[50]={0};
byte TX_Buffer[10]={0};
byte RX_Buffer[10]={0};
byte i,length;
unsigned long start;
unsigned long count;
void setup (void)
{
//mode=0;

Serial.begin (9600);
//Serial.flush();
//Serial.print("set up");
setSPI();
//setInterrupt1();
syncup();
//syncdown();
//Serial.print("done");
}
void loop(void){
// Serial.flush();
setInterrupt1();
while (mode !=7){
  
 Serial.println("frame start");

delay(2000);
 Serial.println("frame end");
}}

void setSPI(void)
{
   SPI.begin ();
   SPI.setClockDivider(SPI_CLOCK_DIV4);   
   SPI.setBitOrder(MSBFIRST);   
   SPI.setDataMode(SPI_MODE0) ;

}

ISR(TIMER1_COMPA_vect)//first timer interrupt
{//Serial.flush();
  //Serial.println("work?"); 
if(mode==0)//transmit mode
{ 
  TX_Buffer[1]=TX_Buffer[1]+1;
  TX_Buffer[2]=TX_Buffer[2]+2;
  TX_Buffer[3]=3;
  TX_Buffer[4]=4;
  TX_Buffer[5]=5;
  TX_Buffer[6]=6+TX_Buffer[1]; 
   Serial.println("mode0");
   TX(); 
  mode=1;
 
}
else{//recieve mode//
Serial.println("mode1");
RX();
if(RX_Buffer[3]==6)
{
  syncup();  
  setInterrupt1();
  return;
}

mode=0;
 count=(micros()-start)/1000;// calculating the time that haspassed by
 Speed=7812-count*7812/500;
}
//Serial.println("reset");

//Serial.println(mode);
Serial.println("after");
setInterrupt1();//resetting the next interrupt
}


void setInterrupt1(void)//sets up the conditions of interrupt1
 {  noInterrupts();           // disable all interrupts
   // initialize timer1 
//  Serial.print("set");
  
 //setting up SPI 
 
 //set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments

  OCR1A = Speed;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  interrupts();             // enable all interrupts
  
      }
void setTX(void)
{  

  CC110L.GDO_Set();
  CC110L.Reset();
  CC110L.RegConfig();
  CC110L.PATABLE();

// SpiWriteReg(0x09,0x10);
//CC110L.WriteSingleReg(0x09,0x09);
//CC110L.WriteSingleReg(0x06,0xC9);
  CC110L.ReadBurstReg(0,RegBuffer,47);
/*for( i=0;i<10;i++)
  {
     TX_Buffer[i]=0;
  }
*/
}
void setRX(void)
{   

  CC110L.GDO_Set();
  CC110L.Reset();
  CC110L.RegConfig();
  //CC110L.PATABLE();
  CC110L.SetReceive();
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
//sync routines
void syncup(void)
{
  for( int i=0;i<10;i++)//innitialing tranfer to sync code
 {
   TX_Buffer[i]=5;
 }
while(1){//transmiting and waiting for recieved
    TX();//transmitting
  Serial.print("Syncup");
 if(RX()==1)
  {if(RX_Buffer[3]==6){
    TX();//sending confirmation
    Serial.print("syncked");
    break;
  }
  }   
}
mode=0;
//setInterrupt1();
}
void syncdown(void)
{ for(int i=0;i<10;i++)//innitializing Transmit to sync message
 {
     TX_Buffer[i]=6;
 }
  while(1)
  {
    if(RX()==1)//cheking if sync is reieved
     {Serial.print("Sync down");
      if(RX_Buffer[3]==5)//if synce is reiceived transmit sync
        {Serial.print("Stage1");
           TX();//transmitting sync
             break;//breaking
         }
       }
 

}
while(1)//waiting for confirm
{if(RX()==1)//if confirmed exit
  {
    Serial.print("Synced");
  break;
}
}
  
//setInterrupt1();  
mode=1;
}
void TX(void)
{ start=micros();
 // long count=0;
 setTX();//switching to transmit mode
  Serial.println("trans");
  for( int i=0;i<10;i++)//innitialing tranfer to sync code
 {
  Serial.print( TX_Buffer[i]);
 }
 Serial.println("");
 
 CC110L.SendData(TX_Buffer,10);

 count=(micros()-start)/1000;// calculating the time that haspassed by
 Speed=7812-count*7812/500;
}
int RX(void)
{Serial.println("recieve");
setRX();//switching to reieve mode
//unsigned long start=millis();
int rec=0;
int count=0;
while (count<500)
{  //Serial.println("stuck?"); 
     
   if(CC110L.CheckReceiveFlag())
 {
   int  size=CC110L.ReceiveData(RX_Buffer);
 //Serial.println(size);  
  rec=1;
  break;
 }
count=(micros()-start)/1000;// calculating the time that haspassed by

count++;
}
if(rec==1){
 for(i=0;i<10;i++)
   {
      Serial.print(RX_Buffer[i],DEC);
//      Serial.print(' ',BYTE);
    }
  }
return rec;
}


