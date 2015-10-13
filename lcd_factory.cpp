/******************************************************************************
 * @file    lcd_factory.cpp 
 * @author  Rémi Pincent - INRIA
 * @date    6 oct. 2015   
 *
 * @brief TODO_one_line_description_of_file
 * 
 * Project : container_node
 * Contact:  Rémi Pincent - remi.pincent@inria.fr
 * 
 * Revision History:
 * TODO_revision history
 *****************************************************************************/
#include <lcd_factory.h>

LCDFactory* LCDFactory::instance = NULL;

LCDI2C* LCDFactory::createLCD(LCDType lcdType, uint8_t lcd_addr, uint8_t lcd_cols, uint8_t lcd_rows)
{
	if(lcdType == FUNDUINO_I2C_LCD
			|| lcdType == LCM1602_LCD)
	{
		return new LCM1602(lcd_addr, lcd_cols, lcd_rows);
	}
	else if(lcdType == ADAFRUIT_I2C_LCD
			|| lcdType == MCP23008_LCD)
	{
		return new MCP23008(lcd_addr, lcd_cols, lcd_rows);
	}
	return NULL;
}


LCDI2C* LCDFactory::createLCD(LCDType lcdType, uint8_t lcd_cols, uint8_t lcd_rows)
{
	if(lcdType == LCM1602_LCD)
	{
		return new LCM1602(LCM1602::LCM1602_DEFAULT_ADDRESS, lcd_cols, lcd_rows);
	}
	else if(lcdType == FUNDUINO_I2C_LCD){
		/** default configuration */
		return new LCM1602(LCM1602::LCM1602_DEFAULT_ADDRESS + B00000111, lcd_cols, lcd_rows);
	}
	else if(lcdType == ADAFRUIT_I2C_LCD
			|| lcdType == MCP23008_LCD)
	{
		return new MCP23008(MCP23008::MCP23008_DEFAULT_ADDRESS, lcd_cols, lcd_rows);
	}
	return NULL;
}
