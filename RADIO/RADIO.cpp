/* RADIO LIBRARY FOR TI RADIO MODUELS
   RADIO_CC110L.h - CC110L  module library
	  Author  : BHARAT UDDAGIRI
	  Version : 0.1
  	DATE    : MARCH 9, 2012
  	EMAIL   : UDDAGIRI@GMAIL.COM
    This library is designed to use CC110L module on Arduino platform.
	
*/

#include<RADIO.h>
#include "pins_arduino.h"
#include "SPI.h"

RADIOClass CC110L;

byte PaTabel[8] = {0x86 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00};

void RADIOClass::Reset (void)
{
	digitalWrite(SS, LOW);
	delay(1);
	digitalWrite(SS, HIGH);
	delay(1);
	digitalWrite(SS, LOW);
	while(digitalRead(MISO));
	SPI.transfer(RADIO_SRES);
	while(digitalRead(MISO));
	digitalWrite(SS, HIGH);
}



void RADIOClass::WriteSingleReg(byte addr, byte value) 
{
	byte temp;
  temp = addr|WRITE_SINGLE;
  digitalWrite(SS, LOW);
	while(digitalRead(MISO));     
  SPI.transfer(temp);
  SPI.transfer(value);        
  digitalWrite(SS, HIGH);
 
 }


byte RADIOClass::ReadSingleReg(byte addr) 
{
	byte temp, value;

    temp = addr|READ_SINGLE;
   
	digitalWrite(SS, LOW);
	while(digitalRead(MISO));
        SPI.transfer(temp);
	value=SPI.transfer(0);
	digitalWrite(SS, HIGH);

	return value;
 
}


void RADIOClass::ReadBurstReg(byte addr, byte *buffer, byte count)
{
	byte i,temp;

	temp = addr | READ_BURST;
	digitalWrite(SS, LOW);
	while(digitalRead(MISO));
	SPI.transfer(temp);
	for(i=0;i<count;i++)
	{
		buffer[i]=SPI.transfer(0);
	}
	digitalWrite(SS, HIGH);
}


void RADIOClass::WriteBurstReg(byte addr, byte *buffer, byte count)
{
	byte i, temp;

	temp = addr | WRITE_BURST;
    digitalWrite(SS, LOW);
    while(digitalRead(MISO));
    SPI.transfer(temp);
    for (i = 0; i < count; i++)
 	{
        SPI.transfer(buffer[i]);
    }
    digitalWrite(SS, HIGH);
}







void RADIOClass::PATABLE(void)
{
	WriteBurstReg(RADIO_PATABLE, PaTabel, 8);
}



void RADIOClass::SendData(byte *txBuffer,byte size)
{
	WriteSingleReg(RADIO_TXFIFO,size);
	WriteBurstReg(RADIO_TXFIFO,txBuffer,size);			//write data to send
	Strobe(RADIO_STX);									//start send	
    while (!digitalRead(GDO0));								// Wait for GDO0 to be set -> sync transmitted  
    while (digitalRead(GDO0));								// Wait for GDO0 to be cleared -> end of packet
	Strobe(RADIO_SFTX);									//flush TXfifo
}



void RADIOClass::Strobe(byte strobe)
{
	digitalWrite(SS, LOW);
	while(digitalRead(MISO));
	SPI.transfer(strobe);
	digitalWrite(SS, HIGH);
}



void RADIOClass::GDO_Set (void)
{
	pinMode(GDO0, INPUT);
	pinMode(GDO2, INPUT);
}


// REVEVE MODE SET UP AND RECEIVING 


void RADIOClass::SetReceive(void)
{
	Strobe(RADIO_SRX);
}




byte RADIOClass::CheckReceiveFlag(void)
{
	if(digitalRead(GDO0))			//receive data
	{
		while (digitalRead(GDO0));
		return 1;
	}
	else							// no data
	{
		return 0;
	}
}




byte RADIOClass::ReadStatusReg(byte addr) 
{
	byte temp, value;

    temp = addr|READ_BURST;
   
	digitalWrite(SS, LOW);
	while(digitalRead(MISO));
        SPI.transfer(temp);
	value=SPI.transfer(0);
	digitalWrite(SS, HIGH);

	return value;
 
}



byte RADIOClass::ReceiveData(byte *rxBuffer)
{
	byte size;
	byte status[2];

	if(ReadStatusReg(RADIO_RXBYTES) & BURST_BYTES_IN_TXFIFO)  // 0x7F MULTIPLE TRY PRESENT BF
	{
		size=ReadSingleReg(RADIO_RXFIFO);
		ReadBurstReg(RADIO_RXFIFO,rxBuffer,size);
		ReadBurstReg(RADIO_RXFIFO,status,2);
		Strobe(RADIO_SFRX);
		return size;
	}
	else
	{
		Strobe(RADIO_SFRX);
		return 0;
	}
	
}





// RADIO CONFIG REGSITER
// ML4_2FSK 1.2KB 237KHZ DEVEATION 675KHZ RXBW

void RADIOClass::RegConfig(void) 
{
   
 WriteSingleReg(RADIO_IOCFG2,     0x29  );      // GDO2 output pin configuration
 WriteSingleReg(RADIO_IOCFG1,     0x2E  );     // GDO1 output pin configuration
 WriteSingleReg(RADIO_IOCFG0,     0x06  );    // GDO0 output pin configuration
 WriteSingleReg(RADIO_FIFOTHR,    0x07  );    // RX FIFO and TX FIFO thresholds
 WriteSingleReg(RADIO_SYNC1,      0xD3  );     // Sync word, high INT8U
 WriteSingleReg(RADIO_SYNC0,      0x91  );     // Sync word, low INT8U
 WriteSingleReg(RADIO_PKTLEN,     0x3D  );    // Packet length
 WriteSingleReg(RADIO_PKTCTRL1,   0x04  );    // Packet automation control
 WriteSingleReg(RADIO_PKTCTRL0,   0x05  );     // Packet automation control
 WriteSingleReg(RADIO_ADDR,       0x00  );     // Device address
 WriteSingleReg(RADIO_CHANNR,     0x00  );    // Channel number
 WriteSingleReg(RADIO_FSCTRL1,    0x0C  );    // Frequency synthesizer control
 WriteSingleReg(RADIO_FSCTRL0,    0x00  );    // Frequency synthesizer control
 WriteSingleReg(RADIO_FREQ2,      0x10  );    // Frequency control word, high INT8U
 WriteSingleReg(RADIO_FREQ1,      0xA7  );   // Frequency control word, middle INT8U
 WriteSingleReg(RADIO_FREQ0,      0x62  );   // Frequency control word, low INT8U
 WriteSingleReg(RADIO_MDMCFG4,    0x15  );   // Modem configuration
 WriteSingleReg(RADIO_MDMCFG3,    0x75  );   // Modem configuration
 WriteSingleReg(RADIO_MDMCFG2,    0x03  );   // Modem configuration
 WriteSingleReg(RADIO_MDMCFG1,    0x21  );   // Modem configuration
 WriteSingleReg(RADIO_MDMCFG0,    0xE5  );   // Modem configuration
 WriteSingleReg(RADIO_DEVIATN,    0x71  );  // Modem deviation setting
 WriteSingleReg(RADIO_MCSM2,      0x07  );      // Main Radio Control State Machine configuration
 WriteSingleReg(RADIO_MCSM1,      0x30  );     // Main Radio Control State Machine configuration
 WriteSingleReg(RADIO_MCSM0,      0x18  );    // Main Radio Control State Machine configuration
 WriteSingleReg(RADIO_FOCCFG,     0x1D  );   // Frequency Offset Compensation configuration
 WriteSingleReg(RADIO_BSCFG,      0x1C  );  // Bit Synchronization configuration
 WriteSingleReg(RADIO_AGCCTRL2,   0x47  ); // AGC control
 WriteSingleReg(RADIO_AGCCTRL1,   0x40  );// AGC control
 WriteSingleReg(RADIO_AGCCTRL0,   0xB0  );       // AGC control
 WriteSingleReg(RADIO_WOREVT1,    0x00  );      // High INT8U Event 0 timeout
 WriteSingleReg(RADIO_WOREVT0,    0x00  );     // Low INT8U Event 0 timeout
 WriteSingleReg(RADIO_WORCTRL,    0xF8  );    // Wake On Radio control
 WriteSingleReg(RADIO_FREND1,     0xB7  );  // Front end RX configuration
 WriteSingleReg(RADIO_FREND0,     0x10  );  // Front end TX configuration
 WriteSingleReg(RADIO_FSCAL3,     0xE9  ); // Frequency synthesizer calibration
 WriteSingleReg(RADIO_FSCAL2,     0x2A  ); // Frequency synthesizer calibration
 WriteSingleReg(RADIO_FSCAL1,     0x00  ); // Frequency synthesizer calibration
 WriteSingleReg(RADIO_FSCAL0,     0x1F  ); // Frequency synthesizer calibration
 WriteSingleReg(RADIO_RCCTRL1,    0x00  ); // RC oscillator configuration
 WriteSingleReg(RADIO_RCCTRL0,    0x00  );// RC oscillator configuration
 WriteSingleReg(RADIO_FSTEST,     0x59  );    // Frequency synthesizer calibration control
 WriteSingleReg(RADIO_PTEST,      0x7F  );    // Production test
 WriteSingleReg(RADIO_AGCTEST,    0x3F  );   // AGC test
 WriteSingleReg(RADIO_TEST2,      0x88  );  // Various test settings
 WriteSingleReg(RADIO_TEST1,      0x31  );  // Various test settings
 WriteSingleReg(RADIO_TEST0,      0x09  );  // Various test settings

}
