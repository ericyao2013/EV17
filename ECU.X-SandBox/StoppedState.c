
#include "StoppedState.h"
#include "PinDef.h"
#include "debug.h"
#include "horn.h"
#include "SlaveAddressing.h"
#include <stdio.h>

struct powerStates {
    bool DDS;
    bool MCS;
    bool SAS;
    bool BMM;
};
extern struct powerStates powerSet;

enum BMM {
    BATTERY_FAULT = 0,
    BATTERY_VOLTS = 1,
    BATTERY_TEMPS = 2,
    BATTERY_POWER = 3
};

struct commsStates {
    bool DDS;
    bool MCS;
    bool SAS;
    bool BMM;
    bool PDU;
    int DDS_SEND;
    int MCS_SEND;
    int SAS_SEND;
    enum BMM BMM_SEND;
    int PDU_SEND;
};
extern struct commsStates comms;

enum ECUstates {
    stopped = 0,
    booting = 1,
    running = 2,
    stopping = 3,
    fault = 4,
    override = 5,
    NUM_STATES = 6
};
extern enum ECUstates currentState;
extern enum debugStates debugState;
#define EGG 5
void doStuff();

void updateStoppedState() {
    //handleDebugRequests();
    //If start button is depressed, do start system request, show on LED
    switch (seekButtonChange()) {
        case DDS_START_BUTTON:
            if (!buttonArray[DDS_START_BUTTON]) {
                changeLEDState(DDS_ACTIVE_LED, !buttonArray[DDS_START_BUTTON]);
                currentState++;
            }
            //doStuff();
            break;
    }
}

//void doStuff() {
//    static int Easter = 0;
//    if (!buttonArray[DDS_DEBUG_BUTTON] && buttonArray[DDS_START_BUTTON]) {
//        Easter = 0;
//    } else {
//        //check if the start button is also held
//        if (easterEggs()) {
//            //this is the first time
//            if (!Easter)
//                Easter++;
//                //has been a few?
//            else if (Easter < 8) {
//                Easter++;
//            } else {
//                //Power up the MCS
//                powerSet.MCS = true;
//                //Set the safety system to boot
//                SS_RELAY = 1;
//                //Set the current state to running
//                currentState = override;
//            }
//            //Start button not held
//        } else {
//            Easter = 0;
//        }
//    }
//
//}
//Just for debugging and so on

//bool easterEggs() {
//    if (!buttonArray[DDS_DEBUG_BUTTON]) {
//        if (!buttonArray[DDS_START_BUTTON]) {
//            return true;
//        }
//    }
//    return false;
//
//}

void Display(){
    printf("\n");
    printf("**************************\n");
    printf("*  Software Written By:  *\n");
    printf("*   Andrew Thornborough  *\n");
    printf("*     Richard Johnson    *\n");
    printf("*       Zac Kilburn      *\n");
    printf("**************************\n");
    printf("\n"); 
    printf("    .-.    \n");                                     
    printf(".  (/^\\)  \n");   
    printf(".  (\\ /)  \n");                                       
    printf(".  .-'-.   \n");                                       
    printf(". /(_I_)\\ \n");                                     
    printf(". \\ ) ( /   \n");                                    
    printf(".  /   \\  \n");                                     
    printf(".  \\ | /   \n");                                      
    printf(".   \\|/     \n");                                     
    printf(".   /|\\    \n");                                     
    printf(".   \\|/     \n");                                     
    printf(".   /Y      \n");  
}