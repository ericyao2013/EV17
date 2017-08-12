#include "Function.h"
#include "tmr0.h"
#include <xc.h>
#include "PinDef.h"
#include "UART.h"
#include "Communications.h"
#include "CarDataDictionary.h"


unsigned char ByteHigh, ByteLow = 0;
long Count = 0;

void ReadFaults(){
    
//    ByteHigh = ByteHigh | ((BPD_FAULT & 0x01) << 0);
//    ByteHigh = ByteHigh | ((BOTS_FAULT & 0x01) << 1);
//    ByteHigh = ByteHigh | ((AMD_FAULT & 0x01) << 2);
//    ByteHigh = ByteHigh | ((STOP_L_FAULT & 0x01) << 3);
//    ByteHigh = ByteHigh | ((STOP_R_FAULT & 0x01) << 4);
//    ByteHigh = ByteHigh | ((STOP_C_FAULT & 0x01) << 5);
//    ByteHigh = ByteHigh | ((IMD_FAULT & 0x01) << 6);
    ByteHigh = ECU_FAULT;//ByteHigh | ((ECU_FAULT) << 7);
    ByteLow = ByteLow | ((BPD_FAULT & 0x01) << 0);
    ByteLow = ByteLow | ((IS_FAULT & 0x01) << 1);
    ByteLow = ByteLow | ((AUX_FAULT & 0x01) << 2);
    
    //INDICATOR ^= 1;
    unsigned char Data[4];
    Data[0] = ByteHigh;
    Data[1] = ByteLow;
    
    SetDataDict(1,0,Data,2);
    
    
    

}


char GetFaults(char num){
    if(num == 1){
        return ByteHigh;
    }
    if(num == 0){
        return ByteLow;
    }
}

void Start(){
    // CONFIG1H
    #pragma config IESO = OFF    // Internal/External Oscillator Switchover bit->Oscillator Switchover mode disabled
    #pragma config PLLCFG = OFF    // 4X PLL Enable->Oscillator used directly
    #pragma config PRICLKEN = OFF    // Primary clock enable bit->Primary clock can be disabled by software
    #pragma config FOSC = INTIO67    // Oscillator Selection bits->Internal oscillator block
    #pragma config FCMEN = OFF    // Fail-Safe Clock Monitor Enable bit->Fail-Safe Clock Monitor disabled

    // CONFIG2L
    #pragma config BOREN = SBORDIS    // Brown-out Reset Enable bits->Brown-out Reset enabled in hardware only (SBOREN is disabled)
    #pragma config BORV = 190    // Brown Out Reset Voltage bits->VBOR set to 1.90 V nominal
    #pragma config PWRTEN = OFF    // Power-up Timer Enable bit->Power up timer disabled

    // CONFIG2H
    #pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits->1:32768
    #pragma config WDTEN = OFF    // Watchdog Timer Enable bits->Watch dog timer is always disabled. SWDTEN has no effect.

    // CONFIG3H
    #pragma config CCP2MX = PORTC1    // CCP2 MUX bit->CCP2 input/output is multiplexed with RC1
    #pragma config P2BMX = PORTD2    // ECCP2 B output mux bit->P2B is on RD2
    #pragma config HFOFST = ON    // HFINTOSC Fast Start-up->HFINTOSC output and ready status are not delayed by the oscillator stable status
    #pragma config PBADEN = ON    // PORTB A/D Enable bit->PORTB<5:0> pins are configured as analog input channels on Reset
    #pragma config CCP3MX = PORTB5    // P3A/CCP3 Mux bit->P3A/CCP3 input/output is multiplexed with RB5
    #pragma config MCLRE = EXTMCLR    // MCLR Pin Enable bit->MCLR pin enabled, RE3 input pin disabled
    #pragma config T3CMX = PORTC0    // Timer3 Clock input mux bit->T3CKI is on RC0

    // CONFIG4L
    #pragma config LVP = OFF    // Single-Supply ICSP Enable bit->Single-Supply ICSP disabled
    #pragma config STVREN = ON    // Stack Full/Underflow Reset Enable bit->Stack full/underflow will cause Reset
    #pragma config XINST = OFF    // Extended Instruction Set Enable bit->Instruction set extension and Indexed Addressing mode disabled (Legacy mode)
    #pragma config DEBUG = OFF    // Background Debug->Disabled

    // CONFIG5L
    #pragma config CP2 = OFF    // Code Protection Block 2->Block 2 (004000-005FFFh) not code-protected
    #pragma config CP1 = OFF    // Code Protection Block 1->Block 1 (002000-003FFFh) not code-protected
    #pragma config CP3 = OFF    // Code Protection Block 3->Block 3 (006000-007FFFh) not code-protected
    #pragma config CP0 = OFF    // Code Protection Block 0->Block 0 (000800-001FFFh) not code-protected

    // CONFIG5H
    #pragma config CPB = OFF    // Boot Block Code Protection bit->Boot block (000000-0007FFh) not code-protected
    #pragma config CPD = OFF    // Data EEPROM Code Protection bit->Data EEPROM not code-protected

    // CONFIG6L
    #pragma config WRT0 = OFF    // Write Protection Block 0->Block 0 (000800-001FFFh) not write-protected
    #pragma config WRT1 = OFF    // Write Protection Block 1->Block 1 (002000-003FFFh) not write-protected
    #pragma config WRT2 = OFF    // Write Protection Block 2->Block 2 (004000-005FFFh) not write-protected
    #pragma config WRT3 = OFF    // Write Protection Block 3->Block 3 (006000-007FFFh) not write-protected

    // CONFIG6H
    #pragma config WRTC = OFF    // Configuration Register Write Protection bit->Configuration registers (300000-3000FFh) not write-protected
    #pragma config WRTD = OFF    // Data EEPROM Write Protection bit->Data EEPROM not write-protected
    #pragma config WRTB = OFF    // Boot Block Write Protection bit->Boot Block (000000-0007FFh) not write-protected

    // CONFIG7L
    #pragma config EBTR3 = OFF    // Table Read Protection Block 3->Block 3 (006000-007FFFh) not protected from table reads executed in other blocks
    #pragma config EBTR1 = OFF    // Table Read Protection Block 1->Block 1 (002000-003FFFh) not protected from table reads executed in other blocks
    #pragma config EBTR2 = OFF    // Table Read Protection Block 2->Block 2 (004000-005FFFh) not protected from table reads executed in other blocks
    #pragma config EBTR0 = OFF    // Table Read Protection Block 0->Block 0 (000800-001FFFh) not protected from table reads executed in other blocks

    // CONFIG7H
    #pragma config EBTRB = OFF    // Boot Block Table Read Protection bit->Boot Block (000000-0007FFh) not protected from table reads executed in other blocks

    /*
     Set pins as inputs - This is the safety inputs
     */
    ECU_FAULT_TRIS = INPUT;
    BOTS_FAULT_TRIS = INPUT;
    AMD_FAULT_TRIS = INPUT;
    STOP_L_FAULT_TRIS = INPUT;
    STOP_R_FAULT_TRIS = INPUT;
    STOP_C_FAULT_TRIS = INPUT;
    IMD_FAULT_TRIS = INPUT;
    BPD_FAULT_TRIS = INPUT;
    TSMS_FAULT_TRIS = INPUT;
    IS_FAULT_TRIS = INPUT;
    AUX_FAULT_TRIS = INPUT;
    RTG_INPUT_TRIS = INPUT;
    IMD_INPUT_TRIS = INPUT;
    
    ANSELCbits.ANSC2 = 1;
    
    //LED for activity
    INDICATOR_TRIS = OUTPUT;
    RS485_TSS_Direction_Tris = OUTPUT;
    
    /*
     Set ADC pins as digital
     */
    ANSELA = 0;
    ANSELB = 0;
    ANSELC = 0;
    ANSELD = 0;
    ANSELE = 0;

    
    // SCS INTOSC; IDLEN disabled; IRCF 16MHz_HFINTOSC/4; 
    OSCCON = 0x72;
    // PRISD enabled; MFIOSEL disabled; SOSCGO disabled; 
    OSCCON2 = 0x04;
    // INTSRC disabled; PLLEN disabled; TUN 0x00; 
    OSCTUNE = 0x00;
    // Set the secondary oscillator

    ComStart();  //Set up usart and FastTransfer
    INTERRUPT_Initialize(); //Set up ISR for timer and USART TX / RX
    TMR0_Initialize(); //Set up TMR0 to use as a tmr counter
    
    unsigned char Data[5] = {4,2};
    SetDataDict(0, 0,Data,2);
}

void  INTERRUPT_Initialize (void)
{
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
    // Disable Interrupt Priority Vectors (16CXXX Compatibility Mode)
    RCONbits.IPEN = 0;

    // Clear peripheral interrupt priority bits (default reset value)

    // RCI
    IPR1bits.RC1IP = 0;
    // TXI
    IPR1bits.TX1IP = 0;
    // TMRI
    // Clear Interrupt flag before enabling the interrupt
    INTCONbits.TMR0IF = 0;

    // Enabling TMR0 interrupt.
    INTCONbits.TMR0IE = 1;
    // ADI
    IPR1bits.ADIP = 0;
    // SSPI
    IPR1bits.SSP1IP = 0;
    // BCLI
    IPR2bits.BCL1IP = 0;
    IPR1bits.TMR2IP=0;
    
}

void interrupt INTERRUPT_InterruptManager (void)
{
   // interrupt handler
    if(PIE1bits.RC1IE == 1 && PIR1bits.RC1IF == 1)
    {
        EUSART1_Receive_ISR();
    }
    else if(PIE1bits.TX1IE == 1 && PIR1bits.TX1IF == 1)
    {
        EUSART1_Transmit_ISR();
    }
    else if(INTCONbits.TMR0IE == 1 && INTCONbits.TMR0IF == 1)
    {
        TMR0_ISR();
    }
    else
    {
        //Unhandled Interrupt
    }
}

void Delay(int wait) {
    int x;
    for (x = 0; x < wait; x++) {
        __delay_ms(1);
    }
}
