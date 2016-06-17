#include <stdio.h>

#include <inttypes.h>

#include <Arduino.h>

#include "gpio_MCP23S17.h"
#include <SPI.h>//this chip needs SPI



gpio_MCP23S17::gpio_MCP23S17(){

}



//return 255 if the choosed pin has no INT, otherwise return INT number
//if there's support for SPI transactions it will use SPI.usingInterrupt(intNum);
//to prevent problems from interrupt
/*USE:
  int intNumber = mcp.getInterruptNumber(gpio_int_pin);
  if (intNumber < 255){
    attachInterrupt(intNumber, keypress, FALLING);//attack interrupt
  } else {
    Serial.println("sorry, pin has no INT capabilities!");
  }
 */

int gpio_MCP23S17::getInterruptNumber(byte pin) {
	int intNum = digitalPinToInterrupt(pin);
	if (intNum != NOT_AN_INTERRUPT) {
		#if defined (SPI_HAS_TRANSACTION) && !defined(ESP8266)
			SPI.usingInterrupt(intNum);
		#endif
		return intNum;
	}
	return 255;
}




#if defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MKL26Z64__) || defined(__MK64FX512__) || defined(__MK66FX1M0__)
gpio_MCP23S17::gpio_MCP23S17(const uint8_t csPin,const uint8_t haenAdrs,const uint8_t mosi_pin,const uint8_t sclk_pin,const uint8_t miso_pin){
	_mosi = mosi_pin;
	_miso = miso_pin;
	_sclk = sclk_pin;
	if (mosi_pin != 11 || sclk_pin != 13 || miso_pin != 12){
		_useAltSPI = true;
	} else {
		_useAltSPI = false;
	}
	postSetup(csPin,haenAdrs);
}

void gpio_MCP23S17::postSetup(const uint8_t csPin,const uint8_t haenAdrs,const uint8_t mosi_pin,const uint8_t sclk_pin,const uint8_t miso_pin){
	_cs = csPin;
	_mosi = mosi_pin;
	_miso = miso_pin;
	_sclk = sclk_pin;
	if (!_useAltSPI){
		if (mosi_pin != 11 || sclk_pin != 13 || miso_pin != 12){
			_useAltSPI = true;
		} else {
			_useAltSPI = false;
		}
	}
	if (haenAdrs > 0x19 && haenAdrs < 0x28){//HAEN works between 0x20...0x27
		_adrs = haenAdrs;
		_useHaen = 1;
	} else {
		_adrs = 0;
		_useHaen = 0;
	}
	_readCmd =  (_adrs << 1) | 1;
	_writeCmd = _adrs << 1;
}
#else
gpio_MCP23S17::gpio_MCP23S17(const uint8_t csPin,const uint8_t haenAdrs){
	postSetup(csPin,haenAdrs);
}

void gpio_MCP23S17::postSetup(const uint8_t csPin,const uint8_t haenAdrs){
	_cs = csPin;
	if (haenAdrs > 0x19 && haenAdrs < 0x28){//HAEN works between 0x20...0x27
		_adrs = haenAdrs;
		_useHaen = 1;
	} else {
		_adrs = 0;
		_useHaen = 0;
	}
	#if defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MKL26Z64__) || defined(__MK64FX512__) || defined(__MK66FX1M0__)
	_useAltSPI = false;
	#endif
	_readCmd =  (_adrs << 1) | 1;
	_writeCmd = _adrs << 1;
}
#endif



void gpio_MCP23S17::begin(bool protocolInitOverride) {
	if (!protocolInitOverride){
		#if defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MKL26Z64__) || defined(__MK64FX512__) || defined(__MK66FX1M0__)
		if (_useAltSPI){
			if ((_mosi == 11 || _mosi == 7) && (_miso == 12 || _miso == 8) && (_sclk == 13 || _sclk == 14)) {//valid SPI pins?
				SPI.setMOSI(_mosi);
				SPI.setMISO(_miso);
				SPI.setSCK(_sclk);
			} else {
				return;
			}
		}
		if (!SPI.pinIsChipSelect(_cs)) return;
		#endif
		SPI.begin();
		#if !defined (SPI_HAS_TRANSACTION)
			SPI.setClockDivider(SPI_CLOCK_DIV4); // 4 MHz (half speed)
			SPI.setBitOrder(MSBFIRST);
			SPI.setDataMode(SPI_MODE0);
		#endif
	}
	pinMode(_cs, OUTPUT);
	#if defined(ESP8266)
		GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, _pinRegister(_cs));//H
	#else
		digitalWrite(_cs, HIGH);
	#endif
	delay(100);
	
	_useHaen == 1 ? _GPIOwriteByte(MCP23S17_IOCON,0b00101000) : _GPIOwriteByte(MCP23S17_IOCON,0b00100000);
	_gpioDirection = 0xFFFF;//all in
	_gpioState = 0xFFFF;//all low 
}



uint16_t gpio_MCP23S17::gpioReadAddress(byte addr){
	_GPIOstartSend(1);
	SPI.transfer(addr);
	#if !defined(__SAM3X8E__) && ((ARDUINO >= 160) || (TEENSYDUINO > 121))
		uint16_t temp = SPI.transfer16(0x0);
		_GPIOendSend();
		return temp;
	#else
		byte low_byte  = SPI.transfer(0x0);
		byte high_byte = SPI.transfer(0x0);
		_GPIOendSend();
		uint16_t temp = low_byte | (high_byte << 8);
		return temp;
	#endif
}


void gpio_MCP23S17::gpioPinMode(uint16_t mode){
	if (mode == INPUT){
		_gpioDirection = 0xFFFF;
	} else if (mode == OUTPUT){	
		_gpioDirection = 0x0000;
		_gpioState = 0x0000;
	} else {
		_gpioDirection = mode;
	}
	_GPIOwriteWord(MCP23S17_IODIR,_gpioDirection);
}

void gpio_MCP23S17::gpioPinMode(uint8_t pin, bool mode){
	if (pin < 16){//0...15
		mode == INPUT ? _gpioDirection |= (1 << pin) :_gpioDirection &= ~(1 << pin);
		_GPIOwriteWord(MCP23S17_IODIR,_gpioDirection);
	}
}


void gpio_MCP23S17::gpioPort(uint16_t value){
	if (value == HIGH){
		_gpioState = 0xFFFF;
	} else if (value == LOW){	
		_gpioState = 0x0000;
	} else {
		_gpioState = value;
	}
	_GPIOwriteWord(MCP23S17_GPIO,_gpioState);
}

void gpio_MCP23S17::gpioPort(byte lowByte, byte highByte){
	_gpioState = highByte | (lowByte << 8);
	_GPIOwriteWord(MCP23S17_GPIO,_gpioState);
}


uint16_t gpio_MCP23S17::readGpioPort(){
	return gpioReadAddress(MCP23S17_GPIO);
}

uint16_t gpio_MCP23S17::readGpioPortFast(){
	return _gpioState;
}

void gpio_MCP23S17::portPullup(uint16_t data) {
	if (data == HIGH){
		_gpioState = 0xFFFF;
	} else if (data == LOW){	
		_gpioState = 0x0000;
	} else {
		_gpioState = data;
	}
	_GPIOwriteWord(MCP23S17_GPPU, _gpioState);
}


void gpio_MCP23S17::gpioDigitalWrite(uint8_t pin, bool value){
	if (pin < 16){//0...15
		value == HIGH ? _gpioState |= (1 << pin) : _gpioState &= ~(1 << pin);
		_GPIOwriteWord(MCP23S17_GPIO,_gpioState);
	}
}

void gpio_MCP23S17::gpioDigitalWriteFast(uint8_t pin, bool value){
	if (pin < 16){//0...15
		value == HIGH ? _gpioState |= (1 << pin) : _gpioState &= ~(1 << pin);
	}
}

void gpio_MCP23S17::gpioPortUpdate(){
	_GPIOwriteWord(MCP23S17_GPIO,_gpioState);
}

int gpio_MCP23S17::gpioDigitalRead(uint8_t pin){
	if (pin < 16) return (int)(gpioReadAddress(MCP23S17_GPIO) & 1 << pin);
	return 0;
}


int gpio_MCP23S17::gpioDigitalReadFast(uint8_t pin){
	int temp = 0;
	if (pin < 16) temp = bitRead(_gpioState,pin);
	return temp;
}

uint8_t gpio_MCP23S17::gpioRegisterReadByte(byte reg){
  uint8_t data = 0;
    _GPIOstartSend(1);
    SPI.transfer(reg);
    data = SPI.transfer(0);
    _GPIOendSend();
  return data;
}

uint16_t gpio_MCP23S17::gpioRegisterReadWord(byte reg){
  uint16_t data = 0;
    _GPIOstartSend(1);
    SPI.transfer(reg);
	#if !defined(__SAM3X8E__) && ((ARDUINO >= 160) || (TEENSYDUINO > 121))
		data = SPI.transfer16(0);
	#else
		data = SPI.transfer(0);
		data = SPI.transfer(0) << 8;
	#endif
    _GPIOendSend();
  return data;
}

void gpio_MCP23S17::gpioRegisterWriteByte(byte reg,byte data,bool both){
	if (!both){
		_GPIOwriteByte(reg,(byte)data);
	} else {
		_GPIOstartSend(0);
		SPI.transfer(reg);
		SPI.transfer(data);
		SPI.transfer(data);
		_GPIOendSend();
	}
}



void gpio_MCP23S17::gpioRegisterWriteWord(byte reg,word data){
	_GPIOwriteWord(reg,(word)data);
}




