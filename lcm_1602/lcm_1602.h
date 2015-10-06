/******************************************************************************
 * @file    lcm_1602.h
 * @author  Rémi Pincent - INRIA
 * @date    6 oct. 2015   
 *
 * @brief LCM1602 port expander driver
 * 
 * Project : lcd
 * Contact:  Rémi Pincent - remi.pincent@inria.fr
 * 
 * Revision History:
 * TODO_revision history
 *****************************************************************************/
#ifndef LCM_1602_H_
#define LCM_1602_H_

#include "lcd_i2c.h"

class LCM1602 : public LCDI2C{
public:
	static const uint8_t LCM1602_DEFAULT_ADDRESS;

public:
	LCM1602(uint8_t lcd_addr, uint8_t lcd_cols, uint8_t lcd_rows, uint8_t charsize = LCD_5x8DOTS);
	~LCM1602();

	/**
	 * Set the LCD display in the correct begin state, must be called before anything else is done.
	*/
	void begin();

	void noBacklight();
	void backlight();

private:
	void send(uint8_t, uint8_t);
	void expanderWrite(uint8_t _data);
	void write4bits(uint8_t);
	void pulseEnable(uint8_t);
};

#endif /* LCM_1602_H_ */
