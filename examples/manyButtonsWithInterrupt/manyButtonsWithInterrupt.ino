/*
In this example we will use a MCP23S17 with 16 push buttons.
 The example show how to deal with registers to set-up the GPIO chip in different mode than default.
 It use a processor interrupt to trig the key decode.
 */
#include <SPI.h>
#include <gpio_MCP23S17.h>   // import library

#define GPIO_ADRS          0x20//SPI

const uint8_t gpio_cs_pin = 10;
const uint8_t gpio_int_pin = 2;//pin connected to INTA mcp23s17

volatile boolean keyPressed     = false;

#if defined (SPI_HAS_TRANSACTION)
gpio_MCP23S17 mcp(gpio_cs_pin, GPIO_ADRS);
#else
gpio_MCP23S17 mcp(gpio_cs_pin, GPIO_ADRS);
#endif


void setup() {
  pinMode(gpio_int_pin, INPUT);
  Serial.begin(38400);
  long unsigned debug_start = millis ();
  while (!Serial && ((millis () - debug_start) <= 5000));
  Serial.println("started");
  mcp.begin();//it will initialize SPI as well
  //setup the gpio!
  /*        7     6     5	     4     3   2     1    0
   IOCON = BANK MIRROR SEQOP DISSLW HAEN ODR INTPOL -NC- */
  mcp.gpioRegisterWriteByte(MCP23S17_IOCON, 0b01101000);//HAEN,SEQOP,MIRROR
  mcp.gpioRegisterWriteByte(MCP23S17_GPPU, 0xFF, true);//pull-up every port (A&B)
  mcp.gpioRegisterWriteByte(MCP23S17_IPOL, 0xFF, true);//invert polarity on every port (A&B)
  mcp.gpioRegisterWriteByte(MCP23S17_GPINTEN, 0xFF, true);//enable interrups on every port (A&B)
  mcp.gpioRegisterReadByte(MCP23S17_INTCAP);    //read interrupt capture port A (it clear port)
  mcp.gpioRegisterReadByte(MCP23S17_INTCAP + 1);//read interrupt capture port B (it clear port)

  int intNumber = mcp.getInterruptNumber(gpio_int_pin);
  if (intNumber < 255){
    attachInterrupt(intNumber, keypress, FALLING);//attack interrupt
  } else {
    Serial.println("sorry, pin has no INT capabilities!");
  }
}

int onKeypress() {
  unsigned int keyValue = 0;
  delay(10);//debounce
  int result = -1;//default, means no button pressed
  uint8_t i;
  keyPressed = false;//time to reset keypress
  if (mcp.gpioRegisterReadByte(MCP23S17_INTF)) keyValue |= mcp.gpioRegisterReadByte(MCP23S17_INTCAP);//read value port A
  if (mcp.gpioRegisterReadByte(MCP23S17_INTF + 1)) keyValue |= mcp.gpioRegisterReadByte(MCP23S17_INTCAP + 1) << 8;//read value port B
  for (i = 0; i < 16; i++) {
    if (keyValue & (1 << i)) {
      result = i;
      break;
    }
  }
  return result;
}

void loop() {
  if (keyPressed) {
    int key = onKeypress();
    if (key > -1)  Serial.println(key);
  }
}

void keypress() {
  keyPressed = true;
}