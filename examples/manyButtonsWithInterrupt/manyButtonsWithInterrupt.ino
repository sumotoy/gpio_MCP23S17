/*
In this example we will use a MCP23S17 with 16 push buttons.
 The example show how to deal with registers to set-up the GPIO chip in different mode than default.
 It use a processor interrupt to trig the key decode.
 */
#include <SPI.h>
#include <gpio_MCP23S17.h>   // import library

#define GPIO_ADRS          0x20//SPI
#define GPIO_CS            10
#define GPIO_INTPIN        2

volatile boolean keyPressed     = false;

gpio_MCP23S17 mcp(GPIO_CS, GPIO_ADRS);


void setup() {
  pinMode(GPIO_INTPIN, INPUT);
  Serial.begin(38400);
  long unsigned debug_start = millis ();
  while (!Serial && ((millis () - debug_start) <= 5000));
  Serial.println("started");
  mcp.begin();//it will initialize SPI as well
  //setup the gpio!
  /*        7     6     5	     4     3   2     1    0
   IOCON = BANK MIRROR SEQOP DISSLW HAEN ODR INTPOL -NC- */
  mcp.gpioRegisterWriteByte(MCP23S17_IOCON, 0b01101100);//HAEN,SEQOP,MIRROR,OPENDRAIN(add a 10K res on INT) 0b01100000
  mcp.gpioRegisterWriteByte(MCP23S17_GPPU, 0xFF, true);//pull-up every port (A&B)
  mcp.gpioRegisterWriteByte(MCP23S17_IPOL, 0xFF, true);//invert polarity on every port (A&B)
  mcp.gpioRegisterWriteByte(MCP23S17_GPINTEN, 0xFF, true);//enable interrups on every port (A&B)
  mcp.gpioRegisterReadByte(MCP23S17_INTCAP);    //read interrupt capture port A (it clear port)
  mcp.gpioRegisterReadByte(MCP23S17_INTCAP + 1);//read interrupt capture port B (it clear port)

  int intNumber = mcp.getInterruptNumber(GPIO_INTPIN);
  if (intNumber < 255) {
    attachInterrupt(intNumber, keypress, FALLING);//attack interrupt
  } else {
    Serial.println("sorry, pin has no INT capabilities!");
  }

}

int onKeypress() {
  uint16_t keyValue = 0;
  delay(10);//debounce
  int result = -1;//default, no button pressed
  uint8_t i;
  keyPressed = false;//time to reset keypress

  if (mcp.gpioRegisterReadByte(MCP23S17_INTF)) {
    keyValue &= 0x00FF;
    keyValue |= mcp.gpioRegisterReadByte(MCP23S17_INTCAP);    // read value at time of interrupt
  }
  if (mcp.gpioRegisterReadByte(MCP23S17_INTF + 1)) {
    keyValue &= 0xFF00;
    keyValue |= mcp.gpioRegisterReadByte(MCP23S17_INTCAP + 1) << 8;        // port B is in low-order byte
  }
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
    if (key >= 0)  Serial.println(key);
  }
}

static void keypress() {
  keyPressed = true;
}
