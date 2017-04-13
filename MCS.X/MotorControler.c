/*
 * File:   MotorController.c
 * Author: Rick
 *
 * Created on May 20, 2015, 3:05 AM
 *
 * This controls the IO for the motor controller
 * It uses the MCP4725 and IO to set modes of the device (ex FW / REV)
 *
 * Data sheet:
 */

#include "MotorControler.h"
#include <stdbool.h>
#include <stdlib.h>
#include "PinDef.h"
#include "Function.h"
#include "MCP4725.h"
#include <xc.h>
//#include "UART2.h" Not working HELP

unsigned int MotorState, spd, brk, prevspd, prevbrk = 0;
bool CarActiveMode =0;
char dir = forward;
char MotorModeActive = 0;

void MotorStateControl(){
    if(MotorModeActive == 0){
        MotorModeActive++;
        MotorMode(MotorModeActive);
    }
    else if(MotorModeActive == MCTURNON){
        MotorModeActive++;
        MotorMode(MotorModeActive);
    }
    else if(MotorModeActive == MCSTARTUP){
        MotorModeActive++;
        MotorMode(MotorModeActive);
    }
    else if((MotorModeActive == MCWAIT) && CarActiveMode){
        MotorModeActive++;
        MotorMode(MotorModeActive);
    }
    else if(MotorModeActive == MCSET){
        MotorModeActive++;
        MotorMode(MotorModeActive);
    }
    else if(MotorModeActive == MCRUN){

        if(!CarActiveMode){
//            spd = 0;
//            brk = 0;
//            MotorMode(MotorModeActive);
//            MotorUpdate();
//            MotorMode(MCSTOP);
        }
    }
    else if(MotorModeActive == MCSTOP){
        MotorMode(MotorModeActive);
        if(CarActiveMode){
            MotorMode(MotorModeActive);
        }
    }
}


void MotorUpdate(){
    MotorStateControl();
    switch(MotorState){
        case MCTURNON:
            DACRELAY = 0;
            //SetDAC1(0);
            //SetDAC2(0);   
            BRAKE = 1;
            REVERSE  = 0;
            FORWARD = 0;
            DC12ENABLE;
            break;
        case MCSTARTUP:
            IGNEN = 1;
            REVERSE  = 0;
            FORWARD = 0;
            break;
        case MCWAIT:
            DACRELAY = 0;
            REVERSE  = 0;
            FORWARD = 0;
            break;
        case MCSET:
            if(dir == forward){
                REVERSE = 0;
                FORWARD = 1;
            }
            if(dir == backward){
                FORWARD = 0;
                REVERSE = 1; 
            }
            DACRELAY = 1;
            break;
        case MCRUN:
//            if(spd != prevspd){
//                prevspd = spd;
//                SetDAC1(spd);
//            }
//            if(brk != prevbrk){
//                prevbrk = brk;
//                //SetDAC2(brk);
//            }
            break;
        case MCSTOP:
            BRAKE = 0;
            DACRELAY = 0;
            DC12DISABLE;
            //SetDAC1(0);
            //SetDAC2(0);
            break;
    }
}

void MotorMode(int value){
    MotorState = value;
}

int GetMotorMode(){
   return MotorState;
}

void SetDirection(char direction){
    dir = direction;
}

void SetSpeed(unsigned int value){
    spd = value;
    if(spd != prevspd){
        prevspd = spd;
         SetDAC1(spd);
    }
}

void SetRegen(unsigned int value){
    brk = value;
    if(brk != prevbrk){
        prevbrk = brk;
        SetDAC2(brk);
    }
}

void SetCarMode(unsigned int value){
    CarActiveMode = value;
}
// toggles regen

void RegenEn(bool enable) {
    if (enable == 1) {
        REGENEN = 1;
    } else
        REGENEN = 0;
}

/*  Reads the rs232 data. It gets 4 nibbles as ascii this function converts each paramter 
 *  into int's and store them into the structure to be used later.
 *  Still needs to work on implementation i have a rough idea below
    
 */

//Not sure how to read ascii HELP it could be for something else.

  //Converst value to to the right value depending on the type of value it is like temprature etc.




