/*************************************************

Arduino Sketch to demonstrate the ADT7310 library

Steven Cogswell
November 2010 

This uses the Hardware SPI port 

Arduino   ADT7310
pin 13    pin 1  (SCLK)
pin 12    pin 2  (DOUT)
pin 11    pin 3  (DIN)
pin 9     pin 4  (CS)

ADT7310's VDD is connected to Arduino 5v, and GND to arduino GND.  


Output from the serial line looks like this: 

ADT7310 Demo
Writing config:  done
reading Tcrit:  Read complete [0x4980]
Temperature: 24.547 C, [0xC46], [0b110001000110]
Temperature: 24.547 C, [0xC46], [0b110001000110]
Temperature: 24.547 C, [0xC46], [0b110001000110]
Temperature: 24.500 C, [0xC40], [0b110001000000]
Temperature: 24.562 C, [0xC48], [0b110001001000]

**************************************************/

#include <SPI.h>
#include <ADT7310.h>

#define SELPIN 9             //Selection Pin  - to chip pin 4 (CS-)
#define MUST_BE_OUTPUT 10    // This pin must be set to output for Hardware SPI to work, even if we don't use it. 

ADT7310 adt7310(SELPIN); 
unsigned int ADTcheck = 0; 

void setup()
{
	// Serial communication back to the user. 
	Serial.begin(9600);
	Serial.println("ADT7310 Demo"); 

	//set pin modes for ADT7310
	pinMode(SELPIN, OUTPUT); 
	pinMode(MUST_BE_OUTPUT,OUTPUT); 

	adt7310.init(); 
	adt7310.reset(); 

	Serial.print("Writing config: "); 
	adt7310.setmode(ADT7310_1FAULT | ADT7310_CT_POLARITY_LOW | ADT7310_INT_POLARITY_LOW | 
		ADT7310_INTCT_INTERRUPT | ADT7310_CONTINUOUS | ADT7310_16BIT); 
	Serial.println(" done"); 

	// On powerup, register 0x04 Tcrit should be value 0x4980
	Serial.print("reading Tcrit: "); 
	unsigned int value = adt7310.read(0x04, 16);
	Serial.print(" Read complete [0x");
	Serial.print(value,HEX);
	Serial.println("]");
	// Save this value for later to compare against to make sure communications still synchronized.  
	ADTcheck=value;     
	delay(240);   // First conversion takes ~ 240 ms 
}

void loop() {
	unsigned int value = 0; 
	unsigned long ok = 0; 
	float t = 0; 

	value=0;
	// Read the 16-bit temperature value from register 0x02
	value = adt7310.read(0x02, 16);
	// Convert to a floating point temperature in C
	t=adt7310.temperature(value,16); 

	Serial.print("Temperature: "); 
	Serial.print(t,3); 
	Serial.print(" C, [0x"); 
	Serial.print(value,HEX); 
	Serial.print("], [0b"); 
	Serial.print(value,BIN); 
	Serial.println("]"); 

	// Read Tcrit register 0x04 again, if the communications are still good
	// then should return the value read at the start, 0x4980. 
	value = adt7310.read(0x04, 16);
    if (ADTcheck != value) {
		Serial.print(" ERROR expect [0x"); 
		Serial.print(ADTcheck,HEX);
		Serial.print(" - 0b"); 
		Serial.print(ADTcheck,BIN); 
		Serial.print("] got [0x"); 
		Serial.print(value,HEX); 
		Serial.print(" - 0b"); 
		Serial.print(value,BIN); 
		Serial.println("] - reset"); 
		// No choice, reset chip to get communications back. 
		adt7310.reset(); 
		adt7310.setmode(ADT7310_1FAULT | ADT7310_CT_POLARITY_LOW | ADT7310_INT_POLARITY_LOW | 
			ADT7310_INTCT_INTERRUPT | ADT7310_CONTINUOUS | ADT7310_16BIT); 
		Serial.print(ok); 
		Serial.println(" measurements without error"); 
		WAIT_FOR_KEY(); 

		ok=0; 
    } else {
		ok=ok+1;
    }
    
    delay(1000);
    
}

// Just a little routine to wait for someone to press a key. 
void WAIT_FOR_KEY() {
    Serial.println("press a key to continue"); 
    while (!Serial.available());
      Serial.flush();
}
 

