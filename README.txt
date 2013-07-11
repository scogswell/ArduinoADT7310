An Arduino Library to use an ADT7310 Temperature sensor by Steven Cogswell 
Originally documented here: http://awtfy.com/2010/11/09/an-arduino-library-for-the-adt7310-spi-temperature-sensor/

From the ADT7310.h file: 

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