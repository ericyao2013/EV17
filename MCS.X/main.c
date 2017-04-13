/* 
 * File:   main.c
 * Author: Rick
 *
 * Created on May 11, 2015, 11:19 PM
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "xc.h"
#include "PinDef.h"
#include "Communications.h"
#include "ADDRESSING.h"
#include "Function.h"
#include "MotorControler.h"
/*
 * 
 */

void MCStest();


int main(int argc, char** argv) {
    
    Setup();
    Delay(100);
    while (1) {
        updateComms();
        ledDebug();
        MotorUpdate();
        //MCStest();
        //Delay(1000);
        
    }

    return (EXIT_SUCCESS);
}

void MCStest(){
//    MotorEnable();
//    Delay(5000);
//    SetMotor(0, forward);
//    Delay(5000);
    int i = 0;
    for(i = 0;i<4095;i++){
        SetDAC1(i); 
        Delay(10);
    }
//    Delay(5000);
//    SetMotor(0, forward);
//    Delay(5000);
//    MotorDisable();
}

