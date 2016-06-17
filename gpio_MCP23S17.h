/*
 ___  _   _  _ __ ___    ___  | |_  ___   _   _ 
/ __|| | | || '_ ` _ \  / _ \ | __|/ _ \ | | | |
\__ \| |_| || | | | | || (_) || |_| (_) || |_| |
|___/ \__,_||_| |_| |_| \___/  \__|\___/  \__, |
                                          |___/ 
										  
	gpio_MCP23S17 - A complete library for Microchip MCP23S17 for many MCU's.
	
model:			company:		pins:		protocol:		Special Features:
---------------------------------------------------------------------------------------------------------------------
mcp23s17		Microchip		 16			SPI					INT/HAEN
---------------------------------------------------------------------------------------------------------------------
Version history:
0.9 Fixed an example, added getInterruptNumber function.
0.95 Added compatibility with ESP8266
---------------------------------------------------------------------------------------------------------------------
		Copyright (c) 2013-2014, s.u.m.o.t.o.y [sumotoy(at)gmail.com]
---------------------------------------------------------------------------------------------------------------------

    gpio_MCP23S17 Library is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    gpio_MCP23S17 Library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.

*/

/* ------------------------------ MCP23S17 WIRING ------------------------------------
This chip has a very useful feature called HAEN that allow you to share the same CS pin trough
8 different addresses. Of course chip has to be Microchip and should be assigned to different addresses!

Basic Address:  00100 A2 A1 A0 (from 0x20 to 0x27)
A2,A1,A0 tied to ground = 0x20
				__ __
		IOB-0 [|  U  |] IOA-7
		IOB-1 [|     |] IOA-6
		IOB-2 [|     |] IOA-5
		IOB-3 [|     |] IOA-4
		IOB-4 [|     |] IOA-3
		IOB-5 [|     |] IOA-2
		IOB-6 [|     |] IOA-1
		IOB-7 [|     |] IOA-0
		++++  [|     |] INT-A
		GND   [|     |] INT-B
		CS    [|     |] RST (connect to +)
		SCK   [|     |] A2
		MOSI  [|     |] A1
		MISO  [|_____|] A0
*/
#ifndef _GPIO_MCP23S17_H_
#define _GPIO_MCP23S17_H_

#include <inttypes.h>

#include <SPI.h>//this chip needs SPI

#if defined (SPI_HAS_TRANSACTION)
#if defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__)
const static uint32_t _MCPMaxSpeed = 30000000UL;
#elif defined(ESP8266)
const static uint32_t _MCPMaxSpeed = 80000000UL;
#elif defined(__MKL26Z64__) //Teensy LC
const static uint32_t _MCPMaxSpeed = 12000000UL;
#elif defined(ARDUINO) && defined(__arm__) && !defined(CORE_TEENSY)	//DUE	
const static uint32_t _MCPMaxSpeed = 24000000UL;
#elif defined(__32MX320F128H__) || defined(__32MX795F512L__) //uno and max	chipkit	
const static uint32_t _MCPMaxSpeed = 8000000UL;
#elif defined(__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__) || defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
const static uint32_t _MCPMaxSpeed = 2000000UL;
#elif defined (__AVR_ATmega328P__) || defined (__AVR_ATmega168P__) || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega644A__) || defined(__AVR_ATmega644PA__) || defined(__AVR_ATmega644__) || defined(__AVR_ATmega1284P__) || defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega1281__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2561__)
const static uint32_t _MCPMaxSpeed = 8000000UL;
#else
const static uint32_t _MCPMaxSpeed = 2000000UL;
#endif
#endif


/*   The IOCON register!
		 7		6     5	     4     3   2     1    0
IOCON = BANK MIRROR SEQOP DISSLW HAEN ODR INTPOL -NC-
-----------------------------------------------------------------------
0b01101100
BANK: (Controls how the registers are addressed)
1 The registers associated with each port are separated into different banks
0 The registers are in the same bank (addresses are sequential)
MIRROR: (INT Pins Mirror bit)
1 The INT pins are internally connected
0 The INT pins are not connected. INTA is associated with PortA and INTB is associated with PortB
SEQOP: (Sequential Operation mode bit)
1 Sequential operation disabled, address pointer does not increment
0 Sequential operation enabled, address pointer increments.
DISSLW: (Slew Rate control bit for SDA output, only I2C)
HAEN: (Hardware Address Enable bit, SPI only)
1 Enables the MCP23S17 address pins
0 Disables the MCP23S17 address pins
ODR: (This bit configures the INT pin as an open-drain output)
1 Open-drain output (overrides the INTPOL bit).
0 Active driver output (INTPOL bit sets the polarity).
INTPOL: (This bit sets the polarity of the INT output pin)
1 Active high
0 Active low
*/

#include "_includes/MCP23S17_registers.h"
	
#if defined(ESP8266)
	#include <eagle_soc.h>
#endif

class gpio_MCP23S17 {

public:
	
	#if defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MKL26Z64__) || defined(__MK64FX512__) || defined(__MK66FX1M0__)
	gpio_MCP23S17(const uint8_t csPin,const uint8_t haenAdrs,const uint8_t mosi_pin=11,const uint8_t sclk_pin=13,const uint8_t miso_pin=12);//any pin,0x20....0x27
	void 			postSetup(const uint8_t csPin,const uint8_t haenAdrs,const uint8_t mosi_pin=11,const uint8_t sclk_pin=13,const uint8_t miso_pin=12);//used with other libraries only
	#else
	gpio_MCP23S17(const uint8_t csPin,const uint8_t haenAdrs);//any pin,0x20....0x27
	void 			postSetup(const uint8_t csPin,const uint8_t haenAdrs);//used with other libraries only
	#endif
	gpio_MCP23S17();//For include inside other libraries

	void 			begin(bool protocolInitOverride=false); //protocolInitOverride=true	will not init the SPI	
    
	void 			gpioPinMode(uint16_t mode);					//OUTPUT=all out,INPUT=all in,0xxxx=you choose
	void 			gpioPinMode(uint8_t pin, bool mode);		//set a unique pin as IN(1) or OUT (0)
	void 			gpioPort(uint16_t value);					//HIGH=all Hi, LOW=all Low,0xxxx=you choose witch low or hi
	void			gpioPort(byte lowByte, byte highByte);		//same as abowe but uses 2 separate bytes
	uint16_t 		readGpioPort();								//read the state of the pins (all)
	uint16_t 		readGpioPortFast();							
	
	void 			gpioDigitalWrite(uint8_t pin, bool value);  //write data to one pin
	void			gpioDigitalWriteFast(uint8_t pin, bool value);
	int 			gpioDigitalRead(uint8_t pin);				//read data from one pin
	uint8_t		 	gpioRegisterReadByte(byte reg);					//read a byte from chip register
	uint16_t		gpioRegisterReadWord(byte reg);					//read a word from chip register
	int 			gpioDigitalReadFast(uint8_t pin);
	void 			gpioRegisterWriteByte(byte reg,byte data,bool both=false);//write a byte in a chip register, optional for both ports
					//if both=true it will write the same register in bank A & B
	void 			gpioRegisterWriteWord(byte reg,word data);		//write a word in a chip register
	void			portPullup(uint16_t data);						// HIGH=all pullup, LOW=all pulldown,0xxxx=you choose witch
	void			gpioPortUpdate();
	int 			getInterruptNumber(byte pin);
	// direct access commands
	uint16_t 		gpioReadAddress(byte addr);

	

protected:
	
	
	inline __attribute__((always_inline))
	void _GPIOstartSend(bool mode) {
	#if defined (SPI_HAS_TRANSACTION)
		SPI.beginTransaction(SPISettings(_MCPMaxSpeed, MSBFIRST, SPI_MODE0));
	#endif
	#if defined(ESP8266)
		GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, _pinRegister(_cs));//L
	#elif defined(__FASTWRITE)
		digitalWriteFast(_cs, LOW);
	#else
		digitalWrite(_cs, LOW);
	#endif
		mode == 1 ? SPI.transfer(_readCmd) : SPI.transfer(_writeCmd);
	}
	
	
	inline __attribute__((always_inline))
	void _GPIOendSend(void){
	#if defined(ESP8266)
		GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, _pinRegister(_cs));//H
	#elif defined(__FASTWRITE)
		digitalWriteFast(_cs, HIGH);
	#else
		digitalWrite(_cs, HIGH);
	#endif

	#if defined (SPI_HAS_TRANSACTION)
		SPI.endTransaction();
	#endif
	}
	
	inline __attribute__((always_inline))
	void _GPIOwriteByte(byte addr, byte data){
		_GPIOstartSend(0);
		SPI.transfer(addr);
		SPI.transfer(data);
		_GPIOendSend();
	}
	
	inline __attribute__((always_inline))
	void _GPIOwriteWord(byte addr, uint16_t data){
		_GPIOstartSend(0);
		SPI.transfer(addr);
		#if !defined(__SAM3X8E__) && ((ARDUINO >= 160) || (TEENSYDUINO > 121))
			SPI.transfer16(data);
		#else
			//SPI.transfer(word2lowByte(data));
			//SPI.transfer(word2highByte(data));
			SPI.transfer(data >> 8);
			SPI.transfer(data & 0xFF);
		#endif
		_GPIOendSend();
	}
	
	#if defined(ESP8266)
	uint32_t _pinRegister(uint8_t pin)
	__attribute__((always_inline)) {
		return _BV(pin);
	}
	#endif
	
	
	
private:
    uint8_t 		_cs;
	uint8_t 		_adrs;
	#if defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MKL26Z64__) || defined(__MK64FX512__) || defined(__MK66FX1M0__)
		uint8_t _miso, _mosi, _sclk;
		boolean	_useAltSPI;
	#endif

	
	uint8_t 		_useHaen;
	uint8_t 		_readCmd;
	uint8_t 		_writeCmd;
	uint16_t		_gpioDirection;
	uint16_t		_gpioState;
};
#endif