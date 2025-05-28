#ifndef LCD_H
#define LCD_H

#include "main.h"
#include <stdarg.h> 
#include <stdio.h>

void delay_ms(uint16_t time);
void lcd_init(void);
void lcd_clear(void);
void lcd_goto(uint8_t row, uint8_t col);
void lcd_print(const char *str);


#endif // LCD_H