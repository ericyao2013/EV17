#include "Function.h"
#include "timers.h"
#include "Communications.h"
#include "BatteryManagment.h"
#include "spi2.h"
#include "UART1.h"
#include "PinDef.h"
void Setup(void) {
    PinSetMode();
   // INDICATOR = 1;
    FaultValue=0;
    // setup internal clock for 72MHz/36MIPS
    // 12 /3 = 4  *32 = 128 / 2=64
    CLKDIVbits.PLLPRE = 0x01; // PLLPRE (N2) 0=/2c
    CLKDIVbits.DOZE = 0;
    PLLFBD = 0x1E; // pll multiplier (M) = +2
    CLKDIVbits.PLLPOST = 0x00; // PLLPOST (N1) 0=/2 
    // Initiate Clock Switch to Primary Oscillator with PLL (NOSC = 0b011)
    __builtin_write_OSCCONH(0x03);
    __builtin_write_OSCCONL(OSCCON | 0x01);
    // Wait for Clock switch to occur
    while (OSCCONbits.COSC != 0b011);
    while (!OSCCONbits.LOCK); // wait for PLL ready
    TRISBbits.TRISB9=0;
    TRISBbits.TRISB8=0;
    TRISBbits.TRISB7=0;
    IEC1bits.INT1IE=1;
//    LATBbits.LATB9=0;
//    LATBbits.LATB8=0;
//    delay_ms(1000);
//    LATBbits.LATB9=1;
//    LATBbits.LATB8=1;

    //INTCON1bits.NSTDIS = 1; //no nesting of interrupts

    PPSUnLock;
    //PPSout(_OC1, _RP5);
    Pin_24_Output = TX1_OUTPUT;
    RX1_Pin_Map = 25;
    Pin_22_Output = TX2_OUTPUT;
    RX2_Pin_Map = 23;
    PPSout(_SDO1, _RP21);
    PPSout(_SCK1, _RP19);
    PPSin(_SDI1, _RP20);
    PPSLock;

    
    initTimerOne();
    CommStart();
    UART1_init();
   //initTimerTwo();
    //Start_BMS();
    SPI2_Initialize();
   // InitI2C();
    //PWM_Init();
}

void Delay(int wait) {
    int x;
    for (x = 0; x < wait; x++) {
        delay_ms(1); //using predif fcn
    }
}

void PinSetMode(void) {
    AD1PCFGL = 0XFFFF;
    TRISBbits.TRISB1 = OUTPUT; //Set LED as output
    TRISBbits.TRISB5 = OUTPUT; //BMS Slave Turn on
    TRISBbits.TRISB6 = OUTPUT; //Fan control OUT
    TRISAbits.TRISA4 = OUTPUT;
    TRISAbits.TRISA7 = OUTPUT;
    TRISAbits.TRISA8 = OUTPUT;
    TRISBbits.TRISB7 = OUTPUT; // RS485 Direction Pin OUTPUT
    LATBbits.LATB6 = 1; 
    LATBbits.LATB7 = 0;
    //SS CLEAR AND SET
    TRISAbits.TRISA1=OUTPUT;
    TRISBbits.TRISB0=OUTPUT;
    TRISBbits.TRISB2 = OUTPUT;
    TRISBbits.TRISB3 = OUTPUT;
    TRISAbits.TRISA9= OUTPUT;
    TRISBbits.TRISB4 = OUTPUT;
    TRISCbits.TRISC7 = OUTPUT;
    TRISCbits.TRISC6 = INPUT;
    //LATCbits.LATC6=1;
    //LATCbits.LATC7=1;
    
    BMS_TURN_ON = 1;
    
}

 void ledDebug() {

        if (time_get(LEDTM) > 500) {
            INDICATOR = !INDICATOR;
           // LATBbits.LATB6 =!LATBbits.LATB6 ;
           // ReadCurrentVolt();
         /*  
            hi++;
            printf( "hi Value %i", hi);
            if (samp==0){
                yo=hi;
                samp=1;
            }
            else{
             yo=((.8*yo )+ (.2*hi));
            }
            
             printf( "yo Value %f", yo);
            */ 
            
            //printf("ADC: %d , %d , %d", CurrentGet(0,1),CurrentGet(0,2),CurrentGet(0,3));
            //printf(SetUnderOverVoltage(5,8));
            //printf("5) Errors\n");
//            if (x == 0) {
//            Saftey_Relay_Reset = 0;
//        } else if (x == 1) {
//            Saftey_Relay_Set = 1;
//        } else if (x == 2) {
//            Saftey_Relay_Set = 0;
//        } else if (x == 3) {
//            Saftey_Relay_Reset = 1;
//        } else if (x == 4) {
//            x = 0;
//        }
            //ReadCurrentVolt();
            time_Set(LEDTM, 0);
        }
    }