/*******************************************************************
 * @brief           Functions.c
 * @brief           A catch all for random fcn's for the device
 * @return          N/A
 * @note            Author: Zac Kilburn
 *******************************************************************/
#include "xc.h"
#include "pps.h"
#include "PinDef.h"
#include <libpic30.h>
#include <stdio.h>
#include <stdbool.h>
#include "Functions.h"
#include "EEprom.h"
#include "Timers.h"
#include "DriverConfigs.h"
#include "ThrottleBrakeControl.h"
#include "Communications.h"

#include "SlaveAddressing.h"


int BrakeLightThreshold = 0;

int read= 0;

/*******************************************************************
 * @brief           Setup
 * @brief           This sets up the device
 * @return          N/A
 * @note            OSSC - USART - I2C - PPS - CONFIGS
 *******************************************************************/
void Setup(void) {

                  // __C30_UART=2;
    PinSetMode();
    // setup internal clock for 72MHz/36MIPS
    // 12/2=6*24=132/2=72
    CLKDIVbits.PLLPRE = 0; // PLLPRE (N2) 0=/2
    PLLFBD = 22; // pll multiplier (M) = +2
    CLKDIVbits.PLLPOST = 0; // PLLPOST (N1) 0=/2
      // Initiate Clock Switch to Primary Oscillator with PLL (NOSC = 0b011)
    __builtin_write_OSCCONH(0x03);
    __builtin_write_OSCCONL(OSCCON | 0x01);
    // Wait for Clock switch to occur
    while (OSCCONbits.COSC != 0b011);
    while (!OSCCONbits.LOCK); // wait for PLL ready

    INTCON1bits.NSTDIS = 1; //No nesting of interrupts

    PPSUnLock;
    //RX0/TX0  -- RS485-1 (U3) --SAS -DDS
    Pin_42_Output = TX2_Output;
    RX1_Pin_Map = 48;

    //RX1/TX1  -- RS485-2 (U1) --BMM -MCS
    Pin_49_Output = TX1_Output;
    RX2_Pin_Map = 43;

    //RX/TX  --SWITCH becomes RX3/TX3 (USB) -> RX4/TX4 (WIRELESS)
    Pin_55_Output = TX3_Output;
    RX3_Pin_Map = 56;

    //RX2/TX2 -- RS485 Full Duplex --Telem Master
    Pin_70_Output = TX4_Output;
    RX4_Pin_Map = 57;

    //PPSout(_OC1, _RP37);
    PPSLock;
    


    //Start comm's
    ComStart();
    
    //This sets up all non constants from external EEPROM 
    SetUpDataSets();
    //Set up IO from DDS
    SetUpDDSIO();
    //This controls the timing system to control communication rates  
    initTimerOne();
    
    //Clear Debug Console
    ClearScreen();
    
}

/*******************************************************************
 * @brief           Delay
 * @brief           its a delay..duh
 * @return          N/A
 * @note            This fcn uses delay_ms - provided by xc.h
 *******************************************************************/
void Delay(int wait) {
    int x;
    for (x = 0; x < wait; x++) {
        delay_ms(1); //using predef fcn
    }
}

/*******************************************************************
 * @brief           Delay
 * @brief           its a delay..duh
 * @return          N/A
 * @note            This fcn uses delay_ms - provided by xc.h
 *******************************************************************/
void DelayUS(int wait) {
    int x;
    for (x = 0; x < wait; x++) {
        delay_us(1); //using predef fcn
    }
}

/*******************************************************************
 * @brief           PinSetMode
 * @brief           Pin configuration
 * @return          N/A
 * @note            Sets up I/O on the device
 *******************************************************************/
void PinSetMode(void) {
    TRISBbits.TRISB10=INPUT;
    TRISEbits.TRISE13 = OUTPUT; //Set LED as output
    TRISBbits.TRISB6 = OUTPUT; //Set Brake Light as OUTPUT
    TRISBbits.TRISB5 = OUTPUT; //Set HORN PWM as OUTPUT
    SS_state_TRS = INPUT;     //Set Safty feedback as input
    RS485_1_Direction_Tris = OUTPUT;
    RS485_2_Direction_Tris = OUTPUT;
    RS485_1_Direction = LISTEN;
    RS485_2_Direction = LISTEN;
    TRISCbits.TRISC10=OUTPUT;
    TRISCbits.TRISC4=OUTPUT;
    TRISCbits.TRISC3=OUTPUT;
    TRISAbits.TRISA9=OUTPUT;
    LATCbits.LATC10 = 0;
    ANSELCbits.ANSC0 = 0;
    ANSELAbits.ANSA4 = 1;
    ANSELCbits.ANSC3 = 1;
    //RX0_Tris=OUTPUT;
    //TX0_Tris=OUTPUT;
    //RX1_Tris=OUTPUT;
    //TX1_Tris=OUTPUT;
    //RX_Tris=OUTPUT;
    //TX_Tris=OUTPUT;
    //RX2_Tris=OUTPUT;
    //TX2_Tris=OUTPUT;

}

bool m = 0;

/*******************************************************************
 * @brief           ledDebug
 * @brief           Allows us to see device activity
 * @return          N/A
 * @note            uses timer to control the function tick rate
 *******************************************************************/
void ledDebug(){
    if (GetTime(TIME) > 250) {
            //INDICATOR ^= 1;
            // HORN_EN ^=1;
            // BRAKELT ^= 1;
            //SS_RELAY ^= 1;
            Run();
            SetTime(TIME);            
        }
    }

/*******************************************************************
 * @brief           updateBrakeLight
 * @brief           This fcn allows us to control the brake light on brake press
 * @return          N/A
 * @note            This fcn uses data from the SAS to read in the brake pressure sensor
 *******************************************************************/
void updateBrakeLight() {
    if(GetThrottleBrakeValue(GETSASB1) > BrakeLightThreshold) {
        BRAKELT = 1;
    }
    else if(GetThrottleBrakeValue(GETSASB1)<(BrakeLightThreshold-3)){  //Prevent Oscillation Number
       BRAKELT = 0;
    }
}

/*******************************************************************
 * @brief           ReadReset
 * @brief           This fcn reads in the RCON
 * @return          N/A
 * @note            This fcn saves the RCON RIGHT AT BOOT! This is important for v
 *******************************************************************/
void ReadReset(){
    read = RCON;
    RCON = 0;
}

/*******************************************************************
 * @brief           GetResetValue
 * @brief           getter
 * @return          return RCON (int - 16 bit)
 * @note            Acts are a getter to keep data integrity 
 *******************************************************************/
int GetResetValue(){
    return read;
}

/*******************************************************************
 * @brief           SetBrakeLightValue
 * @brief           setter
 * @return          N/A
 * @note            Acts are a setter to keep data integrity 
 *******************************************************************/
void SetBrakeLightValue(int val){
    BrakeLightThreshold = val;
}


