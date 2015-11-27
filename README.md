# gpio_MCP23S17
A complete library for Microchip MCP23S17 http://ww1.microchip.com/downloads/en/DeviceDoc/21952b.pdf, a 16 port I/O GPIO with Interrupt and *HAEN* that let you use 8 chip (16 * 8 ports!) by using just 3 (or 4 if you need input) CPU pins!<br>
The library it's fully *SPI transaction compatible* and can be easily included in other libraries, has methods that let's you change ports values fastly and you can easily access chip register directly.<br>
This library has been extracted from my universal gpio_library and I will maintain separately.<br>
I have included several examples but a complete wiki and detailed images will be published soon.<br>
Features:<br>
 - Full access to all MCP features.
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
