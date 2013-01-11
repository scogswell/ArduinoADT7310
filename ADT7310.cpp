/******************************************************************************* 
ADT7310 - An Arduino Library to use an ADT7310 Temperature sensor 
Copyright (C) 2010 Steven Cogswell

http://www.analog.com/en/temperature-sensing-and-thermal-management/digital-temperature-sensors/adt7310/products/product.html

Version 20101109A.   

See ADT7310.h for history

As usual, I wrote this for myself, and it works for me.  It may not work for you, in 
which case I sympathize.  

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

	
***********************************************************************************/


#include "WProgram.h"
#include "ADT7310.h"
#include <SPI.h>



// Constructor 
// This uses the hardware SPI, so the only pin we need to set is the SELECT pin for
// the chip select (CS).    Note that according to details you may have to make the hardware
// SPI slave select (SS, pin 10) an output to make this work even if you're not using pin 10 
// to drive this chip. 
//  All this does is assign the selection pin internally to the object, it does no initialization. 
//
ADT7310::ADT7310(int SELPIN)
{
	_SELPIN=SELPIN; 
	pinMode(_SELPIN,OUTPUT); 
}

// init() will startup the hardware SPI with the proper modes to drive the ADT7310. 
void ADT7310::init()
{
	SPI.setDataMode(SPI_MODE3);
	SPI.setClockDivider(SPI_CLOCK_DIV4);
	SPI.setBitOrder(MSBFIRST);
	SPI.begin(); 
}

// close() Ends the SPI object. 
void ADT7310::close()
{
	SPI.end(); 
}

// Writes to register 0x01, the configuation register.  You can use the define's in ADT7310.h to make this easier
// e.g.       adt7310.setmode(ADT7310_1FAULT | ADT7310_CT_POLARITY_LOW | ADT7310_INT_POLARITY_LOW | 
//            ADT7310_INTCT_INTERRUPT | ADT7310_CONTINUOUS | ADT7310_16BIT); 
void ADT7310::setmode(int value) {

	#ifdef ADT7310_VERBOSE_DEBUG
		Serial.print(" setmode is 0x"); 
		Serial.print(value,HEX); 
		Serial.print(" or 0b"); 
		Serial.print(value,BIN); 
	#endif

	write(0x01,value,8);

}

// Write to a generic register of the ADT7310.  numbits indicates either an 8 or 16 bit write. 
// 
// The ADT7310 works by sending a command byte, which indicates which register you want to 
// write to, followed by a second write (8/16 bit) which is the data for that register.  
//
// Note that the command has a flag to put the ADT7310 into continuous read, which I don't
// do anything with. 
// 
// Data is clocked out of the ADT7310 on the negative edge of SCLK, and clocked into the device
// on the positive edge of SCLK.   Hardware SPI looks after this for us, as long as the modes are 
// set right.  
// 
void ADT7310::write(int reg, unsigned int value, int numbits){
  
  unsigned int bitval = 0; 
  byte byteval = 0; 
  byte commandbits = 0b00000000; //command bits - 0, R/W, 3 bits address, continuous read, 0, 0
  commandbits = commandbits | (reg << 3);
  
  digitalWrite(_SELPIN,LOW); //Select ADT7310
  
  // setup command to be written
  #ifdef ADT7310_VERBOSE_DEBUG
  Serial.print(" write to (0x");
  Serial.print(reg,HEX);
  Serial.print("):");
  #endif
  
  // Send the command.
  SPI.transfer(commandbits); 

  // write value to SPI bus
  #ifdef ADT7310_VERBOSE_DEBUG
  Serial.print(" write value 0x");
  Serial.print(value,HEX); 
  Serial.print("( "); 
  Serial.print(numbits);
  Serial.print(" bits): "); 
  #endif

  if (numbits == 8) { 
    // If 8-bit data, send that as one transfer 
    byteval = value & 0xFF; 

    // setup command to be written
    #ifdef ADT7310_VERBOSE_DEBUG
    Serial.print(" write:"); 
    Serial.print(byteval,BIN); 
    #endif
    SPI.transfer(byteval); 
  } else if (numbits == 16) {
	// if 16-bit data, send as two chunks - MSB first 
    byteval = (value >>8); 
    #ifdef ADT7310_VERBOSE_DEBUG
    Serial.print(" write:"); 
    Serial.print(byteval,BIN); 
    #endif
	// Send high 8-bits
    SPI.transfer(byteval);

    // send low 8-bits
    byteval = (value & 0xFF);  
    SPI.transfer(byteval); 	
	
    #ifdef ADT7310_VERBOSE_DEBUG
    Serial.print(" write:"); 
    Serial.print(byteval,BIN); 
    #endif

  }	
  
  digitalWrite(_SELPIN, HIGH); //turn off device

  //return 0;
}

// Read from a generic register of the ADT7310.  numbits indicates either an 8 or 16 bit read. 
// 
// The ADT7310 works by sending a command byte, which indicates which register you want to 
// read from, followed by a second read (8/16 bit) which is the data from that register.  
//
// Note that the command has a flag to put the ADT7310 into continuous read, which I don't
// do anything with. 
// 
// Data is clocked out of the ADT7310 on the negative edge of SCLK, and clocked into the device
// on the positive edge of SCLK.   Hardware SPI looks after this for us, as long as the modes are 
// set right.  
// 
unsigned int ADT7310::read(int reg, int numbits) {
  unsigned int readval = 0;
  unsigned int readval2 = 0; 
  unsigned int bitval = 0; 
  byte commandbits = 0b01000000; //command bits - 0, R/W, 3 bits address, continuous read, 0, 0
  commandbits = commandbits | (reg << 3);
  
  digitalWrite(_SELPIN,LOW); //Select ADT7310
  

  #ifdef ADT7310_VERBOSE_DEBUG
	  Serial.print(" write: 0b"); 
	  Serial.print(commandbits,BIN); 
  #endif
  // Send command byte via SPI
  SPI.transfer(commandbits); 

  //read bits from ADT7310, 16-bit read
  #ifdef ADT7310_VERBOSE_DEBUG
	  Serial.print(" Read ("); 
	  Serial.print(numbits);
	  Serial.print(" bits): "); 
  #endif
  readval=0;
  // Doing an SPI.transfer(0x00) essentially does a 'read' 
  readval = SPI.transfer(0x00); 
  #ifdef ADT7310_VERBOSE_DEBUG
	  Serial.print(" byte:"); 
	  Serial.print(readval,BIN);
  #endif 
  
  // See if we have to read more bits yet. 
  numbits -= 8; 
  if (numbits > 0) {
     readval=readval << 8; 
	 // Do a second read of the next 8 bits 
     readval2 = SPI.transfer(0x00); 

     readval = readval | readval2;
    #ifdef ADT7310_VERBOSE_DEBUG
		Serial.print(" byte:"); 
		Serial.print(readval,BIN);
		Serial.print(" total: ");
		Serial.print(readval,BIN); 
    #endif 
  }
  
  digitalWrite(_SELPIN, HIGH); //turn off device
  
  #ifdef ADT7310_VERBOSE_DEBUG
	  Serial.print(" value: 0x");
	  Serial.print(readval,HEX); 
	  Serial.print(" value: ");
	  Serial.println(readval,DEC); 
  #endif

  return readval;
}


// I find that the ADT7310 is really fussy about timing, and if it gets unsynchonized in communication
// it has to be reset.   To do a reset you clock in a "1" for at least 32 clock cycles.  Doing a reset clears
// the configuration registers (essentially same as a power up), so you will need to re-configure the chip 
// again.   Also note there is a delay() at the end to wait for the chip initialization (datasheet says 
// 500 microseconds). 
//
// There's probably a good way to do this directly with hardware SPI, but this worked for me.  
// 
void ADT7310::reset()  {
  // We're going to run the SPI bus directly, so let's turn off the existing SPI definitions 
  SPI.end(); 
  pinMode(_SELPIN, OUTPUT);     // We drive the SPI bus direct. 
  pinMode(ADT7310_DATAOUT, OUTPUT); 
  pinMode(ADT7310_DATAIN, INPUT); 
  //digitalWrite(DATAIN,HIGH); // Activate pullup resistor
  pinMode(ADT7310_SPICLOCK, OUTPUT); 
  digitalWrite(_SELPIN,HIGH); //Select ADT7310
  digitalWrite(_SELPIN,LOW); //Select ADT7310
  
  // Do a serial reset, you have to clock at least 32 consecutive 1's into the DATAIN to reset the serial 
  // communication link. 
  #ifdef ADT7310_VERBOSE_DEBUG
	Serial.print("Reset chip "); 
  #endif
  digitalWrite(ADT7310_DATAOUT,1); 
  // Yes, 100 is definitely greater than 32.   It's good to be sure. 
  for(int i=0; i<100; i++) {
      clock();
  }
  delay(100);   // wait for chip to finish reset, give it lots of time. 
  digitalWrite(_SELPIN,HIGH); //deselect ADT7310
  #ifdef ADT7310_VERBOSE_DEBUG
	Serial.println(" - done"); 
  #endif
  SPI.begin(); 
}

// This is only used for the reset() routine to simulate the SPI clock 
void ADT7310::clock() {
    digitalWrite(ADT7310_SPICLOCK,LOW);
    delayMicroseconds(1);
    digitalWrite(ADT7310_SPICLOCK,HIGH);  
}

// This converts the read value from the temperature register (which you have previously used a read() to get 
// from register 0x02.) into a nice floating point number.  The ADT7310 supports a lot of different bit-ednes 
// in the reply.   It also gives negative temperatures as two's complement,
//
// There probably is a much more efficient way to do this, but this works for me. 
// 
float ADT7310::temperature(unsigned int value, int numbits)
{
	float temperature;
	boolean isNegative = false;  
	
	#ifdef ADT7310_VERBOSE_DEBUG
		Serial.print("value is 0x"); 
		Serial.print(value,HEX); 
	#endif

	value = (value >> (16-numbits));   // Bits are left-aligned in two-byte response
	#ifdef ADT7310_VERBOSE_DEBUG
		Serial.print(" in "); 
		Serial.print(numbits); 
		Serial.print("-bit as 0x"); 
		Serial.print(value,HEX); 
	#endif 
	// Check for two's complement (negative) temperature value
	if (bitRead(value,numbits-1)==1)  {
		value |= (0xFFFF << numbits);   // Extend sign bit to the end of the 16-bit space
		value = (~value +1);   // calculate two's complement 
		#ifdef ADT7310_VERBOSE_DEBUG 
			Serial.print(" as two's complement: 0x"); 
			Serial.print(value,HEX); 
		#endif
		isNegative=true;
	}
	
	temperature == -9999.99;   // Dummy value to get us started.   If the routine returns the -9999.99 you know something fell through incorrectly. 
	// Taken directly from the datasheet 
	if (numbits == 16) {
			temperature = value/128.0;
	} else if (numbits == 13) {
			temperature = value/16.0;
	} else if (numbits == 10) {
			temperature = value/2.0; 
	} else if (numbits == 9) {
			temperature = value*1.0; 
	}
	
	if (isNegative)
		temperature=-temperature; 
		
	#ifdef ADT7310_VERBOSE_DEBUG
		Serial.print(" temp: "); 
		Serial.println(temperature,5);
	#endif
	return temperature;
}