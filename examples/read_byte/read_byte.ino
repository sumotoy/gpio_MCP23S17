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
  uint8_t key_byte = 0;
  key_byte = mcp.gpioRegisterReadByte(MCP23S17_GPIO);
  Serial.print("A: ");
  Serial.println(key_byte, HEX);
  key_byte = mcp.gpioRegisterReadByte(MCP23S17_GPIO + 1);
  Serial.print("B: ");
  Serial.println(key_byte, HEX);
  delay(1000);
}
