
#include <xc.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "UART1.h"
#include "PinDef.h"
#include <libpic30.h> 
#include <stdio.h>
#define ON         0
#define OFF        1

void *memset(void *s, int c, size_t n);

struct UART1_ring_buff {
    unsigned char buf[UART_BUFFER_SIZE];
    int head;
    int tail;
    int count;
};

void UART_buff_init2(struct UART1_ring_buff* _this);
void UART_buff_put2(struct UART1_ring_buff* _this, const unsigned char c);
unsigned char UART_buff_get2(struct UART1_ring_buff* _this);
void UART_buff_flush2(struct UART1_ring_buff* _this, const int clearBuffer);
int UART_buff_size2(struct UART1_ring_buff* _this);
unsigned int UART_buff_modulo_inc2(const unsigned int value, const unsigned int modulus);
unsigned char UART_buff_peek2(struct UART1_ring_buff* _this);

struct UART1_ring_buff input_buffer1;
struct UART1_ring_buff output_buffer1;

bool Transmit_stall1 = true;

void UART1_init(void) {
    // UART config
    U2MODEbits.STSEL = 0; // 1-stop bit
    U2MODEbits.PDSEL = 0; // No parity, 8-data bits
    U2MODEbits.ABAUD = 0; // Auto-baud disabled
    U2BRG = BAUD_RATE; // Baud Rate setting for 57600
    U2STAbits.URXISEL = 0b01; // Interrupt after all TX character transmitted
    U2STAbits.URXISEL = 0b00; // Interrupt after one RX character is received
    IFS1bits.U2RXIF = 0; // Clear RX interrupt flag
    IFS1bits.U2TXIF = 0; // Clear TX interrupt flag
    IEC1bits.U2RXIE = 1; // Enable RX interrupt
    IEC1bits.U2TXIE = 1; // Enable TX interrupt
    U2MODEbits.UARTEN = 1; // Enable UART
    U2STAbits.UTXEN = 1; // Enable UART TX
    
    UART_buff_init2(&input_buffer1);
    UART_buff_init2(&output_buffer1);
    __C30_UART=2;
}

void UART_buff_init2(struct UART1_ring_buff* _this) {
    /*****
      The following clears:
        -> buf
        -> head
        -> tail
        -> count
      and sets head = tail
     ***/
    memset(_this, 0, sizeof (*_this));
}

void UART_buff_put2(struct UART1_ring_buff* _this, const unsigned char c) {
    if (_this->count < UART_BUFFER_SIZE) {
        _this->buf[_this->head] = c;
        _this->head = UART_buff_modulo_inc2(_this->head, UART_BUFFER_SIZE);
        ++_this->count;
    } else {
        _this->buf[_this->head] = c;
        _this->head = UART_buff_modulo_inc2(_this->head, UART_BUFFER_SIZE);
        _this->tail = UART_buff_modulo_inc2(_this->tail, UART_BUFFER_SIZE);

    }
}

unsigned char UART_buff_get2(struct UART1_ring_buff* _this) {
    unsigned char c;
    if (_this->count > 0) {
        c = _this->buf[_this->tail];
        _this->tail = UART_buff_modulo_inc2(_this->tail, UART_BUFFER_SIZE);
        --_this->count;
    } else {
        c = 0;
    }
    return (c);
}

void UART_buff_flush2(struct UART1_ring_buff* _this, const int clearBuffer) {
    _this->count = 0;
    _this->head = 0;
    _this->tail = 0;
    if (clearBuffer) {
        memset(_this->buf, 0, sizeof (_this->buf));
    }
}

int UART_buff_size2(struct UART1_ring_buff* _this) {
    return (_this->count);
}

unsigned int UART_buff_modulo_inc2(const unsigned int value, const unsigned int modulus) {
    unsigned int my_value = value + 1;
    if (my_value >= modulus) {
        my_value = 0;
    }
    return (my_value);
}

unsigned char UART_buff_peek2(struct UART1_ring_buff* _this) {
    return _this->buf[_this->tail];
}

unsigned char Receive_peek2(void) {
    return UART_buff_peek2(&input_buffer1);
}

int Receive_available2(void) {
    return UART_buff_size2(&input_buffer1);
}

unsigned char Receive_get2(void) {
    return UART_buff_get2(&input_buffer1);
}

void Send_put2(unsigned char _data) {
    UART_buff_put2(&output_buffer1, _data);
    if (Transmit_stall1 == true) {
        Transmit_stall1 = false;
        U2TXREG = UART_buff_get2(&output_buffer1);
    }
}

void __attribute__((interrupt, no_auto_psv)) _U2RXInterrupt(void) {
    if (U2STAbits.OERR) {
        U2STAbits.OERR = 0;
    }
    if (U2STAbits.FERR) {
        U2RXREG;
        IFS1bits.U2RXIF = 0; // Clear RX interrupt flag    
        return;
    }
    if (U2STAbits.PERR) {
        U2RXREG;
        IFS1bits.U2RXIF = 0; // Clear RX interrupt flag
        return;
    }
    if(U2STAbits.URXDA){
    unsigned char data = U2RXREG;
    UART_buff_put2(&input_buffer1, data);
    }
    IFS1bits.U2RXIF = 0; // Clear RX interrupt flag    
}

void __attribute__((interrupt, no_auto_psv)) _U2TXInterrupt(void) {
    //LED ^= 1;
    if (UART_buff_size2(&output_buffer1) > 0) {
        U2TXREG = UART_buff_get2(&output_buffer1);
    } else {
        Transmit_stall1 = true;
    }
    IFS1bits.U2TXIF = 0; // Clear TX interrupt flag
}