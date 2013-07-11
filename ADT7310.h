/******************************************************************************* 
ADT7310 - An Arduino Library to use an ADT7310 Temperature sensor 
Copyright (C) 2010 Steven Cogswell

http://www.analog.com/en/temperature-sensing-and-thermal-management/digital-temperature-sensors/adt7310/products/product.html

Version 20130711A.   

Version History:
20130711A:

- Compatibility with Arduino 1.0+ library spec.  Tested with Arduino 1.0.5 and an UNO r2 Board. 

This library uses hardware SPI via SPI.h

The ADT7310 has lots of modes to operate in.  I have only really tested the 16-bit continuous operation
situtation. Why would you buy this chip and want to use less bits, I ask?  The read()/write() routines are
generic so you should be able to run the one-shot/1SPS/shutdown modes without issue.  

The first version of this library was all bit-banging on the SPI lines directly, and not using the hardware SPI. 
In those versions I found this chip seems to be really fussy about timing and sequence of bits.   If you mess up 
the sequence of bits the chip it desynchronizes communicationa and essentially crosses its arms and refuses to play until you reset. 

Some chips essentially do a reset every time you select them with the SS pin, the ADT7310 is not like that. 

The big problem I found when reading temperatures is you can't get a confident response that the value you read was 
correct and the chip was properly synchronized.  If the chip is confused on communication you just get and endless 
string of zeros for data regardless of register you read.  

So to check to see if the chip is still communicating correctly, after a temperature read I read one of the setpoint alarm
registers (in my case, 0x04, Tcrit, which defaults at 0x4980).  I figure after a temperature read if I can read 0x4980 
back from Tcrit that means communications are still good.  If not (either zero or missing bits off the end), then all 
you can do is do a chip reset, re-write your registers and continue on.  It doesn't take long but it essentially means two 
reads for every temperature.   Better to be safe than sorry.    Maybe there's a better way to do it, but that worked for me. 

(Note that the library does none of that, that's something to do in the main program). 

The ADT7310 does a lot of communication at high speed.  Early on (in the bit-banging simulated SPI version) I found it was 
definitely affected by wire length.  If the leads were too long the error rate goes up.  Keep the wiring from the bus to the 
chip short.  

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


#ifndef ADT7310_h
#define ADT7310_h

// Compatibility with the Arduino 1.0 library standard
#if defined(ARDUINO) && ARDUINO >= 100  
#include "Arduino.h"  
#else  
#include "WProgram.h"   
#endif

#define ADT7310_VERBOSE_DEBUG 1
#undef ADT7310_VERBOSE_DEBUG

//#define SELPIN 9 //Selection Pin  - to chip pin 4 (CS-)
#define ADT7310_DATAOUT 11 //MOSI  - to chip pin 3 (DIN)
#define ADT7310_DATAIN  12 //MISO  - to chip pin 2 (DOUT) 
#define ADT7310_SPICLOCK 13  //Clock  - to chip pin 1


// Some constants used in the setup for the ADT7310, see datasheet. 
#define ADT7310_1FAULT (0b00)
#define ADT7310_2FAULT (0b01)
#define ADT7310_3FAULT (0b10)
#define ADT7310_4FAULT (0b11)

#define ADT7310_CT_POLARITY_LOW (0<<2)
#define ADT7310_CT_POLARITY_HIGH (1<<2) 
#define ADT7310_INT_POLARITY_LOW (0<<3) 
#define ADT7310_INT_POLARITY_HIGH (1<<3) 

#define ADT7310_INTCT_INTERRUPT (0<<4)
#define ADT7310_INTCT_COMPARATOR (1<<4)

#define ADT7310_CONTINUOUS (0b00<<6)
#define ADT7310_ONESHOT (0b01<<6)
#define ADT7310_1SPS (0b10<<6) 
#define ADT7310_SHUTDOWN (0b11<<6)

#define ADT7310_13BIT (0 << 7)
#define ADT7310_16BIT (1 << 7)

class ADT7310
{
	public:
		ADT7310(int SELPIN); 
		void write(int reg, unsigned int value, int numbits); 
		unsigned int read(int reg, int numbits); 
		void reset(); 
		void init(); 
		void close(); 
		float temperature(unsigned int value, int numbits);
		void setmode(int value);
	private:
		int _SELPIN; 
		void clock(); 

};


#endif ADT7310_h