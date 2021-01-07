#include <Arduino.h>
#include <SPI.h>
#include <gpio_MCP23S17.h>   // import library

#define GPIO_ADRS          0x20//SPI

const uint8_t gpio_cs_pin = 10; //UNO
//const uint8_t gpio_cs_pin = 5; //ESP32, VSPI

gpio_MCP23S17 mcp(gpio_cs_pin, GPIO_ADRS);//using transactions?

void setup() {
  Serial.begin(57600);
  mcp.begin();//it will initialize SPI as well
  mcp.gpioPinMode(INPUT);
}

void loop(){
  unsigned int key = 0;
  for (int i = 0; i < 16; i++)
  {
      key = mcp.gpioDigitalRead(i);
      Serial.print(i);
      Serial.print(": ");
      Serial.println(key, HEX);
  }
  delay(1000);
}
