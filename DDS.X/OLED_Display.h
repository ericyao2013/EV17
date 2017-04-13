/* 
 * File:   OLED_Display.h
 * Author: Rick
 *
 * Created on May 1, 2016, 4:05 AM
 */

#ifndef OLED_DISPLAY_H
#define	OLED_DISPLAY_H

#define OLED 0x3C
#define SLAVE_I2C_GENERIC_RETRY_MAX     2

void oledClear();
void oledPrint( const char *s );
void OLED_init();
void OLED_command(char com);
void oledGotoYX(unsigned char Row, unsigned char Column);
void Display(void);

#endif	/* OLED_DISPLAY_H */

