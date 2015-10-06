/******************************************************************************
 * @file    mcp_23008.h 
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
#ifndef ARDUINO_LIB_LCD_MCP_23008_H_
#define ARDUINO_LIB_LCD_MCP_23008_H_

#include "lcd_i2c.h"

class MCP23008 : public LCDI2C{
public :
	static const uint8_t MCP23008_DEFAULT_ADDRESS;
public:
	MCP23008(uint8_t lcd_addr, uint8_t lcd_cols, uint8_t lcd_rows, uint8_t charsize = LCD_5x8DOTS);
	~MCP23008();

	/**
	 * Set the LCD display in the correct begin state, must be called before anything else is done.
	*/
	void begin();

	void noBacklight();
	void backlight();

private:
	void send(uint8_t, uint8_t);
	void expanderWrite(uint8_t _data);
};

#endif /* ARDUINO_LIB_LCD_MCP_23008_H_ */
