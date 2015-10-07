/******************************************************************************
 * @file    lcd_factory.h 
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
#ifndef ARDUINO_LIB_LCD_LCD_FACTORY_H_
#define ARDUINO_LIB_LCD_LCD_FACTORY_H_

#include "lcd_i2c.h"
#include "lcm_1602.h"
#include "mcp_23008.h"
#include "binary.h"

class LCDFactory {
public:
	typedef uint8_t LCDType;
	static const LCDType LCM1602_LCD = 0;
	static const LCDType FUNDUINO_I2C_LCD = LCM1602_LCD + 1;
	static const LCDType ADAFRUIT_I2C_LCD = FUNDUINO_I2C_LCD + 1;
	static const LCDType MCP23008_LCD = ADAFRUIT_I2C_LCD + 1;

private:
	static LCDFactory* instance;

public:
	virtual ~LCDFactory(){};
	static LCDFactory* getInstance(void)
	{
		if(instance == NULL){
			instance = new LCDFactory();
		}
		return instance;
	}

	/**
	 * Instantiate an I2C LCD of given type at given i2c address
	 * @param lcdType
	 * @param lcd_addr
	 * @param lcd_cols
	 * @param lcd_rows
	 * @return instantiated LCD if successful, NULL otherwise
	 */
	virtual LCDI2C* createLCD(LCDType lcdType, uint8_t lcd_addr, uint8_t lcd_cols, uint8_t lcd_rows);

	/**
	 * Instantiate an I2C LCD of given type using default LCD i2c address
	 * @param lcdType
	 * @param lcd_addr
	 * @param lcd_cols
	 * @param lcd_rows
	 * @return instantiated LCD if successful, NULL otherwise
	 */
	virtual LCDI2C* createLCD(LCDType lcdType, uint8_t lcd_cols, uint8_t lcd_rows);
};

#endif /* ARDUINO_LIB_LCD_LCD_FACTORY_H_ */
