/******************************************************************************
 * @file    lcd_i2c.cpp 
 * @author  Rémi Pincent - INRIA
 * @date    6 oct. 2015   
 *
 * @brief TODO_one_line_description_of_file
 * 
 * Project : arduino_lcd
 * Contact:  Rémi Pincent - remi.pincent@inria.fr
 * 
 * Revision History:
 * TODO_revision history
 *****************************************************************************/
#include <lcd_i2c.h>
#include <Arduino.h>

// Note, however, that resetting the Arduino doesn't reset the LCD, so we
// can't assume that its in that state when a sketch starts (and the
// LiquidCrystal constructor is called).

LCDI2C::LCDI2C(uint8_t lcd_addr, uint8_t lcd_cols, uint8_t lcd_rows, uint8_t charsize)
{
	_addr = lcd_addr;
	_cols = lcd_cols;
	_rows = lcd_rows;
	_charsize = charsize;
	_backlightval = LCD_BACKLIGHT;
	_displaymode = LCD_ENTRYLEFT;
	_displaycontrol = LCD_DISPLAYON;
	_displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS; // in case they forget to call begin() at least we have something
}

LCDI2C::~LCDI2C() {
	// TODO Auto-generated destructor stub
}

/********** high level commands, for the user! */
void LCDI2C::clear(){
	command(LCD_CLEARDISPLAY);// clear display, set cursor position to zero
	delayMicroseconds(2000);  // this command takes a long time!
}

void LCDI2C::home(){
	command(LCD_RETURNHOME);  // set cursor position to zero
	delayMicroseconds(2000);  // this command takes a long time!
}

void LCDI2C::setCursor(uint8_t col, uint8_t row){
	int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
	if (row > _rows) {
		row = _rows-1;    // we count rows starting w/0
	}
	command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

void LCDI2C::rowSetCols(uint8_t arg_u8_line, char arg_s8_char, uint8_t arg_u8_nbChars)
{
	setCursor(0, arg_u8_line);
	for(uint8_t loc_u8_colIndex = 0; loc_u8_colIndex < arg_u8_nbChars; loc_u8_colIndex++)
	{
		print(arg_s8_char);
	}
	setCursor(0, arg_u8_line);
}

void LCDI2C::rowSet(uint8_t arg_u8_line, char arg_s8_char)
{
	rowSetCols(arg_u8_line, arg_s8_char, _cols);
}
void LCDI2C::lcdSet(char arg_s8_char)
{
	setCursor(0, 0);
	for(uint8_t loc_u8_rowIndex = 0; loc_u8_rowIndex < _rows; loc_u8_rowIndex++)
	{
		setCursor(0, loc_u8_rowIndex);
		rowSet(loc_u8_rowIndex, arg_s8_char);
	}
}

// Turn the display on/off (quickly)
void LCDI2C::noDisplay() {
	_displaycontrol &= ~LCD_DISPLAYON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LCDI2C::display() {
	_displaycontrol |= LCD_DISPLAYON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
void LCDI2C::noCursor() {
	_displaycontrol &= ~LCD_CURSORON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LCDI2C::cursor() {
	_displaycontrol |= LCD_CURSORON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turn on and off the blinking cursor
void LCDI2C::noBlink() {
	_displaycontrol &= ~LCD_BLINKON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LCDI2C::blink() {
	_displaycontrol |= LCD_BLINKON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void LCDI2C::scrollDisplayLeft(void) {
	command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void LCDI2C::scrollDisplayRight(void) {
	command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void LCDI2C::leftToRight(void) {
	_displaymode |= LCD_ENTRYLEFT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
void LCDI2C::rightToLeft(void) {
	_displaymode &= ~LCD_ENTRYLEFT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'right justify' text from the cursor
void LCDI2C::autoscroll(void) {
	_displaymode |= LCD_ENTRYSHIFTINCREMENT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
void LCDI2C::noAutoscroll(void) {
	_displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void LCDI2C::createChar(uint8_t location, uint8_t charmap[]) {
	location &= 0x7; // we only have 8 locations 0-7
	command(LCD_SETCGRAMADDR | (location << 3));
	for (int i=0; i<8; i++) {
		write(charmap[i]);
	}
}

void LCDI2C::setBacklight(uint8_t new_val){
	if (new_val) {
		backlight();		// turn backlight on
	} else {
		noBacklight();		// turn backlight off
	}
}
/*********** mid level commands, for sending data/cmds */

inline void LCDI2C::command(uint8_t value) {
	send(value, 0);
}

inline size_t LCDI2C::write(uint8_t value) {
	send(value, Rs);
}
