/* 
 * File:   Function.h
 * Author: Rick
 *
 * Created on May 12, 2015, 1:00 AM
 */

#ifndef FUNCTION_H
#define	FUNCTION_H


#ifndef __DELAY_H
#define FOSC  66000010LL  // clock-frequecy in Hz with suffix LL (64-bit-long), eg. 32000000LL for 32MHz
#define FCY       (FOSC/2)  // MCU is running at FCY MIPS
#define delay_us(x) __delay32(((x*FCY)/1000000L)) // delays x us
#define delay_ms(x) __delay32(((x*FCY)/1000L))  // delays x ms
#define __DELAY_H 1
#include <libpic30.h>
#endif

#define PPSin(fn,pin)    iPPSInput(IN_FN_PPS##fn,IN_PIN_PPS##pin)
#define PPSout(fn,pin)    iPPSOutput(OUT_PIN_PPS##pin,OUT_FN_PPS##fn)

extern volatile int receiveArray[20];

extern void begin(volatile int * ptr, unsigned char maxSize, unsigned char givenAddress, bool error, void (*stufftosend)(unsigned char), unsigned char (*stufftoreceive)(void), int (*stuffavailable)(void), unsigned char (*stuffpeek)(void));

extern unsigned char Receive_peek(void);
extern int Receive_available(void);
extern unsigned char Receive_get(void);
extern void Send_put(unsigned char _data);

extern void UART_init(void);
void ledDebug();

extern void timerOne();
extern void timerTwo();

extern void Setup(void);
extern void Delay(int wait);
void PinSetMode(void);

void ledDebug();
#endif	/* FUNCTION_H */

