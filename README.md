# gpio_MCP23S17 The 'third reincarnation'
This is the last reincarnation for the MCP23S17 GPIO expander http://ww1.microchip.com/downloads/en/DeviceDoc/21952b.pdf, a 16 port I/O expander with advanced features working in SPI. It was completely recoded and this time uses specialized CPU specified SPI sub-libraries to simplify library and get the most from this chip.<br>
I simplified many commands and now it's possible use complicated configurations very easily and fixed several issues of the old libraries.<br>
I have included full SPI Transaction (where supported) and the ability to use any possible MCU SPI bus or alternative SPI pin and you can access any possible feature of the chip easily.<br>
This version is a 'step ahead' compared the old libraries, commands are slight different and unfortunatly not compatible, this is why I pushed in  another fork.<br>
I have included many examples and you will find some advanced example that uses different chip in the same time as example.<br>
MCP23Sxx series has very interesting features like HAEN that let you use several chip sharing the same SPI BUS and CS!<br>

<b>Features:</b><br>
 - Full access to all MCP features.
 - Uses HAEN to have several MCP23Sxx chip in the same SPI bus (and CS!).
 - Scalable complexity, easy to use as GPIO but advanced use always possible.
 - Mature and stable library, I've used in many projects.
 - Fully SPI transaction compatible, it doesn't interfere with other SPI devices.
 - Compatible with all 8bit arduino, DUE, Teensy(all), ESP8266.
 - 
 
------------------------------ MCP23S17 WIRING ------------------------------------<br>
This chip has a very useful feature called HAEN that allow you to share the same CS pin trough<br>
8 different addresses. Of course chip has to be Microchip and should be assigned to different addresses!<br>
<br>
Basic Address:  00100 A2 A1 A0 (from 0x20 to 0x27)<br>
A2,A1,A0 tied to ground = 0x20<br>
<br>
IOB-0.[|...U...|] IOA-7<br>
IOB-1.[|.........|] IOA-6<br>
IOB-2.[|.........|] IOA-5<br>
IOB-3.[|.........|] IOA-4<br>
IOB-4.[|.........|] IOA-3<br>
IOB-5.[|.........|] IOA-2<br>
IOB-6.[|.........|] IOA-1<br>
IOB-7.[|.........|] IOA-0<br>
VCC...[|.........|] INT-A<br>
GND..[|.........|] INT-B<br>
CS.....[|.........|] RST (connect to +)<br>
SCK...[|.........|] A2<br>
MOSI.[|.........|] A1<br>
MISO.[|____|] A0<br>
<br>
