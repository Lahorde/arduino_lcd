/******************************************************************************
 * @file    mcp_23008.cpp 
 * @author  Rémi Pincent - INRIA
 * @date    6 oct. 2015   
 *
 * @brief MCP23008 port expander driver
 * 
 * Project : arduino_lcd
 * Contact:  Rémi Pincent - remi.pincent@inria.fr
 * 
 * Revision History:
 * TODO_revision history
 *****************************************************************************/
#include <mcp_23008.h>
#include <Arduino.h>
#include <Wire.h>

/** The slave address contains four fixed bits (MSB)
 * and three user-defined hardware address bits
 * */
const uint8_t MCP23008::MCP23008_DEFAULT_ADDRESS = 0x20;

// registers
#define MCP23008_IODIR 0x00
#define MCP23008_IPOL 0x01
#define MCP23008_GPINTEN 0x02
#define MCP23008_DEFVAL 0x03
#define MCP23008_INTCON 0x04
#define MCP23008_IOCON 0x05
#define MCP23008_GPPU 0x06
#define MCP23008_INTF 0x07
#define MCP23008_INTCAP 0x08
#define MCP23008_GPIO 0x09
#define MCP23008_OLAT 0x0A

MCP23008::MCP23008(uint8_t lcd_addr, uint8_t lcd_cols, uint8_t lcd_rows, uint8_t charsiz) :LCDI2C(lcd_addr, lcd_cols, lcd_rows, charsiz){
}

MCP23008::~MCP23008() {
}

void MCP23008::begin() {
	// SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
	// according to datasheet, we need at least 40ms after power rises above 2.7V
	// before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
	delay(50);

	Wire.begin();
	// first thing we do is get the GPIO expander's head working straight, with a boatload of junk data.
	Wire.beginTransmission(_addr);
#if ARDUINO >= 100
	Wire.write((byte)MCP23008_IODIR);
	Wire.write((byte)0xFF);
	Wire.write((byte)0x00);
	Wire.write((byte)0x00);
	Wire.write((byte)0x00);
	Wire.write((byte)0x00);
	Wire.write((byte)0x00);
	Wire.write((byte)0x00);
	Wire.write((byte)0x00);
	Wire.write((byte)0x00);
	Wire.write((byte)0x00);
#else
	Wire.send(MCP23008_IODIR);
	Wire.send(0xFF);
	Wire.send(0x00);
	Wire.send(0x00);
	Wire.send(0x00);
	Wire.send(0x00);
	Wire.send(0x00);
	Wire.send(0x00);
	Wire.send(0x00);
	Wire.send(0x00);
	Wire.send(0x00);
#endif
	Wire.endTransmission();

	// now we set the GPIO expander's I/O direction to output since it's soldered to an LCD output.
	Wire.beginTransmission(_addr);
#if ARDUINO >= 100
	Wire.write((byte)MCP23008_IODIR);
	Wire.write((byte)0x00); // all output: 00000000 for pins 1...8
#else
	Wire.send(MCP23008_IODIR);
	Wire.send(0x00); // all output: 00000000 for pins 1...8
#endif
	Wire.endTransmission();

	if (_rows > 1) {
		_displayfunction |= LCD_2LINE;
	}

	// for some 1 line displays you can select a 10 pixel high font
	if ((_charsize != 0) && (_rows == 1)) {
		_displayfunction |= LCD_5x10DOTS;
	}

	//put the LCD into 4 bit mode
	// start with a non-standard command to make it realize we're speaking 4-bit here
	// per LCD datasheet, first command is a single 4-bit burst, 0011.
	//-----
	//  we cannot assume that the LCD panel is powered at the same time as
	//  the arduino, so we have to perform a software reset as per page 45
	//  of the HD44780 datasheet - (kch)
	//-----
	// bit pattern for the burstBits function is
	//
	//  7   6   5   4   3   2   1   0
	// LT  D7  D6  D5  D4  EN  RS  n/c
	//-----
	expanderWrite(B10011100); // send LITE D4 D5 high with enable
	expanderWrite(B10011000); // send LITE D4 D5 high with !enable
	expanderWrite(B10011100); //
	expanderWrite(B10011000); //
	expanderWrite(B10011100); // repeat twice more
	expanderWrite(B10011000); //
	expanderWrite(B10010100); // send D4 low and LITE D5 high with enable
	expanderWrite(B10010000); // send D4 low and LITE D5 high with !enable
	delay(5); // this shouldn't be necessary, but sometimes 16MHz is stupid-fast.

	command(LCD_FUNCTIONSET | _displayfunction); // then send 0010NF00 (N=lines, F=font)
	delay(5); // for safe keeping...
	command(LCD_FUNCTIONSET | _displayfunction); // ... twice.
	delay(5); // done!

	// turn on the LCD with our defaults. since these libs seem to use personal preference, I like a cursor.
	_displaycontrol = LCD_DISPLAYON;
	display();
	// clear it off
	clear();

	_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	// set the entry mode
	command(LCD_ENTRYMODESET | _displaymode);

	setBacklight(HIGH); // turn the backlight on if so equipped.
}

void MCP23008::noBacklight()
{
	bitWrite(_displaycontrol,3, 0); // flag that the backlight is enabled, for burst commands
	expanderWrite(0x00);
}
void MCP23008::backlight()
{
	bitWrite(_displaycontrol,3, 1); // flag that the backlight is enabled, for burst commands
	expanderWrite(0x80);
}

// write either command or data, burst it to the expander over I2C.
void MCP23008::send(uint8_t value, uint8_t mode) {
	// BURST SPEED, OH MY GOD
	// the (now High Speed!) I/O expander pinout
	// RS pin = 1
	// Enable pin = 2
	// Data pin 4 = 3
	// Data pin 5 = 4
	// Data pin 6 = 5
	// Data pin 7 = 6
	byte buf;
	// crunch the high 4 bits
	buf = (value & B11110000) >> 1; // isolate high 4 bits, shift over to data pins (bits 6-3: x1111xxx)
	if (mode) buf |= 3 << 1; // here we can just enable enable, since the value is immediately written to the pins
	else buf |= 2 << 1; // if RS (mode), turn RS and enable on. otherwise, just enable. (bits 2-1: xxxxx11x)
	buf |= (_displaycontrol & LCD_BACKLIGHT)?0x80:0x00; // using DISPLAYCONTROL command to mask backlight bit in _displaycontrol
	expanderWrite(buf); // bits are now present at LCD with enable active in the same write
	// no need to delay since these things take WAY, WAY longer than the time required for enable to settle (1us in LCD implementation?)
	buf &= ~(1<<2); // toggle enable low
	expanderWrite(buf); // send out the same bits but with enable low now; LCD crunches these 4 bits.
	// crunch the low 4 bits
	buf = (value & B1111) << 3; // isolate low 4 bits, shift over to data pins (bits 6-3: x1111xxx)
	if (mode) buf |= 3 << 1; // here we can just enable enable, since the value is immediately written to the pins
	else buf |= 2 << 1; // if RS (mode), turn RS and enable on. otherwise, just enable. (bits 2-1: xxxxx11x)
	buf |= (_displaycontrol & LCD_BACKLIGHT)?0x80:0x00; // using DISPLAYCONTROL command to mask backlight bit in _displaycontrol
	expanderWrite(buf);
	buf &= ~( 1 << 2 ); // toggle enable low (1<<2 = 00000100; NOT = 11111011; with "and", this turns off only that one bit)
	expanderWrite(buf);
}

void MCP23008::expanderWrite(uint8_t _data){
	// we use this to burst bits to the GPIO chip whenever we need to. avoids repetative code.
	Wire.beginTransmission(_addr);
#if ARDUINO >= 100
	Wire.write(MCP23008_GPIO);
	Wire.write(_data); // last bits are crunched, we're done.
#else
	Wire.send(MCP23008_GPIO);
	Wire.send(value); // last bits are crunched, we're done.
#endif
	while (Wire.endTransmission()) ;
}
