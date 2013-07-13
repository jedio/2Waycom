//Timer interrupt Test trial
//Jeididah Oyeyemi
//David  Leibowitz
//june 29 2013
#include <SPI.h>
#include <RADIO.h>

/******GLOBAL VARIABLES***********/
int act;  // variable to store the value coming from the sensor
int mode;//
int  Speed=7812;
int synced;
int isc;
unsigned long start;
unsigned long now;
long diff;
long  count;
byte RegBuffer[50]={0};
byte TX_Buffer[10][3]={0};
byte RX_Buffer[10][3]={0};
byte i,length;
int light,light1;
int  rpi=0;//receive process index
int  rsi=0;//receive send index;
int  tpi=0;
int  tsi=0;
int  sent=0;
 byte TXT[3];
byte message=0;
float input;
void setup (void)
{Serial.begin(9600);
 pinMode(6,OUTPUT);
 pinMode(5,OUTPUT);
 pinMode(7,INPUT_PULLUP);
 
 synced=0;
 light=0;
 light1=0;
 setSPI();
// syncUp();
 syncDown();

 Serial.print("synched");
//   //toggle();
 Speed=7812;
 setInterrupt1();
}


void loop (void)
{
  while(1)
  {if(message==1)
  { message=0;
   int temp=analogRead(0);
   Serial.println(temp);
    input=(float)temp;
    Serial.print("light:");
    Serial.println(input);
    input=input/4.0;  
    TXT[0]=1;
    TXT[1]=0;
    TXT[2]=(byte)input;   
   // Serial.print("light:");
   // Serial.println(input);
    sent=1;
   
  }
  if(message==2)
  { message=0;
    input=(float)analogRead(1);
    input=input/4.0;  
    TXT[0]=2;
    TXT[1]=0;
    TXT[2]=(byte)input;   
    sent=1;
   
  }
   }
     
    
  
  
  
 
 }
 
 
 
/*********************I/O SOUBROUNTINES**************************/
void debouncer(int x)
{delay(50);
while( digitalRead(x)==LOW)
{// Serial.print("insidedebouncer");
}
delay(50);
  
}
      
void toggle()
{ if(light==0)
     {
     digitalWrite(6,HIGH);
     light=1;
      }
     else
     {
     digitalWrite(6,LOW);
     light=0;
     }
}
void toggle1()
{ if(light1==0)
     {
  digitalWrite(5,HIGH);
     light1=1;
      }
     else
     {
     digitalWrite(5,LOW);
     light1=0;
     }
}      
      
/********************* SETUP SUBROUTINES**************** */      
void setSPI(void)
	{
		SPI.begin ();
		SPI.setClockDivider(SPI_CLOCK_DIV4);   
		SPI.setBitOrder(MSBFIRST);   
		SPI.setDataMode(SPI_MODE0) ;

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

		
	}


void setRX(void)
	{   

		CC110L.GDO_Set();
		CC110L.Reset();
		CC110L.RegConfig();
		//CC110L.PATABLE();
		CC110L.SetReceive();
	}
/********************* TX/RX************************************************/
ISR(TIMER1_COMPA_vect)//first timer interrupt
	{      
                //Serial.print("ISR");
                
                
                //delay(500);
                isc +=1;               
                int count;                
                start=micros(); 
               // toggle1();
		if(mode==0)
		{light1=0;
                 toggle1();
              //Serial.println("TX");
                
                  mode=1;
                  if(sent==1)
                  { 
                  TX(TXT);
                   sent=0;
                   }
                 //transmit mode
  		      

		}
		else
                {//light1=0;
                 light1=1;
                 toggle1();
                 //light=0;
                  //toggle();
                byte RXT[]={0,0,0};
                  //recieve mode//
                  mode=0;
                 if(RX(RXT)==1)
                       {
                         Serial.println("got");
                         if(RXT[0]==5)
                            {//toggle1();
                             //syncUp();
                           syncDown();
                         //   //toggle();
                          
                           Speed=7812;
                           setInterrupt1();
                           return;
                            }
                            else if(RXT[0]==4)
                            {
                              message=RXT[2];
                             
                            }
                     
                         }
			
		}
		now=micros();
                diff=now-start;
                diff=diff*78;
                diff=diff/1000 ;                
                int diffi=(int)diff;
                Speed=78-diffi;
		setInterrupt1();//resetting the next interrupt
	}
/*********Sync routines*******/
void syncUp(void)
      {
        //Serial.println("sync up");
       synced=0;
       byte RXT[]={0,0,0};
       byte TXT[]={5,5,5};
        while(1)
          {
            TX(TXT);
            if(RX(RXT)==1&&RXT[2]==5&&RXT[1]==5&&RXT[0]==5)//cheking for a follow up reception
            { Serial.println("confirmed")  ;          
              break;
            }
          }
          mode=0;
          synced=1;
          light=0;
           //toggle();
          delay(500);
           //toggle();
          delay(500);
      }
void syncDown(void)
      {light=1;
       toggle1();
      //Serial.println("sync down");
      //noInterrupts();
      synced=0;
        byte RXT[]={0,0,0};
        byte TXT[]={5,5,5};
      //  byte TXN[]={9,9,9};
        while(1)
          {Serial.println("searching");
           if(RX(RXT)==1&&RXT[2]==5)//cheking for a follow up reception
            {  // Serial.println("sync recieved"); 
                TX(TXT);//send confirmation         
          
              break;
            }
           // TX(TXN);
          }
               
          mode=1;
          synced=1;
          light=0;
           //toggle();
          delay(500);
           //toggle();
          delay(500);
          interrupts();
      }
void TX(byte *TX_BFF)
     { 
      
        setTX();             
        CC110L.SendData(TX_BFF,3);       
       
      }
int RX(byte *RX_BFF)
     { 
       setRX();//switching to reieve mode            
        int rec=0;
        unsigned long count=0;
        long track=micros();
        while (count<100000)
         {  // Serial.println(count);
            if(CC110L.CheckReceiveFlag())
             {
              int size=CC110L.ReceiveData(RX_BFF);
                 // Serial.println(count);  
                   rec=1;
                  break;
             }
            //count=(micros()-start)/1000;// calculating the time that haspassed by
           count++;
          }
           
            // //toggle();
            return rec;//RETURNS 1 if something was recieved returns zero if something was transmitted
       } 
