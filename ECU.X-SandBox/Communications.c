/*
 * File:   Communications.h
 * Author: Zac Kilburn
 *
 * Created on May 31, 2015
 *
 * Handles all of the communications system updates and error checking
 * -- Each board has a turn being communicated with
 *      --If the board doesnt respond, several attemps will be made to retry
 *      --At the end of the communications train there is room to send an error code
 *      --Rs485 direction is automated with packet sends.
 *      --All timers are inside T1 interrupt.. implied approx milliseconds
 */


#include <xc.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "PinDef.h"
#include "ADDRESSING.h"
#include "Communications.h"
#include "ThrottleBrakeControl.h"
#include "MCSComms.h"
#include "DDSComms.h"
#include "PDUComms.h"
#include "BMMComms.h"
#include "SScomms.h"
#include "Timers.h"
#include "UART.h"
#include "UART1.h"
#include "UART2.h"
#include "UART3.h"
#include "horn.h"
#include "CarCom_SAS_DDS_SS.h"
#include "CarCom_PDU_MCS_BMM.h"
#include "Functions.h"
#include "DriverConfigs.h"

char x = 0;

unsigned int BoardTimeOutCounterBus1 = 0;
unsigned int BoardTimeOutCounterBus2 = 0;

enum bus1CommState {
    SAS_UPDATE = 0, DDS_UPDATE = 1, SS_UPDATE = 2//,CHECK_STATE1 = 3, ERROR_STATE1 = 4, NUM_STATES1 = 5
};
enum bus1CommState commsBus1State = SAS_UPDATE;
    
enum bus2CommState {
    MCS_UPDATE = 0, BMM_UPDATE = 1,  PDU_UPDATE = 2//, CHECK_STATE2 = 3, ERROR_STATE2 = 4, NUM_STATES2 = 5
};
enum bus2CommState commsBus2State = MCS_UPDATE;

struct commsStates {
    bool DDS;
    bool MCS;
    bool SAS;
    bool BMM;
    bool PDU;
    bool SS;
    int DDS_SEND;
    int MCS_SEND;
    int SAS_SEND;
    int BMM_SEND;
    int PDU_SEND;
    int SS_SEND;
};
struct commsStates comms;

void ComStart(){
    UART_init();
    UART1_init();
    UART2_init();
    UART3_init();   
}
    
void updateComms() {
    bus1Update();
    if(!GetCarLock()){  //prevents MCS and BMM from coming up....Stopping system boot      
        bus2Update();
    }
   // }
    //checkCommDirection();
    //checkCommDirection1();
    //RS485_Direction1(TALK);
    receiveData_SAS_DDS_SS();
}

//void printBits(size_t const size, void const * const ptr)
//{
//    unsigned char *b = (unsigned char*) ptr;
//    unsigned char byte;
//    int i, j;
//
//    for (i=size-1;i>=0;i--)
//    {
//        for (j=7;j>=0;j--)
//        {
//            byte = (b[i] >> j) & 1;
//            printf("%u", byte);
//        }
//    }
//    printf("\n\r");
//}

//void Run(){
//
////            Delay(1);
////            Data[0] = constructPowerSet();
////            RS485_Direction2(TALK);         
////            sendData_PDU_MCS_BMM(PDU_ADDRESS, WRITE_TABLE, TABLE_FOUR_PDU, PDU_POWER_CONTROL, Data, 1);
////            Delay(3);
////            RS485_Direction2(LISTEN);
//            
//}

bool SASTx = true;
bool DDSTx = true;
bool SSTx = true;

unsigned int previousTime = 0;

void bus1Update() {
    switch (commsBus1State) {
        case SAS_UPDATE:
            if(SASTx){
                SASTx = false;
                unsigned char Data[10];
                Data[0] = 0x11;
                Data[1] = 0x22;
                RS485_Direction1(TALK);         
                sendData_SAS_DDS_SS(SAS_ADDRESS, READ_TABLE, TABLE_TWO_SAS, SAS_THROTTLE_1, Data, SS_FAULT_STATUS_LENTH);
                Delay(3);
                RS485_Direction1(LISTEN);
                SetTime(SASTIMER);
                previousTime = GetTime(SASTIMER);
            }
            //Packet received 
            if (RecivedValidPacket_SAS_DDS_SS()) {
                CheckThrotleConsistency();
                commsBus1State = DDS_UPDATE;
                SASTx = true;
                break;
            }
            //Time Out on packet
            if(GetTime(SASTIMER) - previousTime > 15){
                commsBus1State = DDS_UPDATE;
                SASTx = true;
            }
            else{
                break;
            }
        case DDS_UPDATE:
            if(DDSTx){
                DDSTx = false;
                unsigned char Data[2];
                Data[0] = 0x11;
                Data[1] = 0x22;
                RS485_Direction1(TALK);         
                sendData_SAS_DDS_SS(DDS_ADDRESS, READ_TABLE, TABLE_THREE_DDS, SAS_THROTTLE_1, Data, SS_FAULT_STATUS_LENTH);
                Delay(3);
                RS485_Direction1(LISTEN);
                SetTime(DDSTIMER);
                previousTime = GetTime(DDSTIMER);
            }
            if (RecivedValidPacket_SAS_DDS_SS()) {
                commsBus1State = SS_UPDATE;
                DDSTx = true;
                break;
            }
            if(GetTime(DDSTIMER) - previousTime > 15){
                commsBus1State = SS_UPDATE;
                DDSTx = true;
                break;
            }
            else{
                break;
            }
        case SS_UPDATE:
            if(SSTx){
                SSTx = false;
                unsigned char Data[2];
                Data[0] = 0x11;
                Data[1] = 0x22;
                RS485_Direction1(TALK);         
                sendData_SAS_DDS_SS(SS_ADDRESS, READ_TABLE, TABLE_ONE_SS, SS_FAULT_STATUS, Data, SS_FAULT_STATUS_LENTH);
                Delay(3);
                RS485_Direction1(LISTEN);
                SetTime(SSTIMER);
                previousTime = GetTime(SSTIMER);
            }
            if (RecivedValidPacket_SAS_DDS_SS()) {
                commsBus1State = SAS_UPDATE;
                SSTx = true;
                break;
            }
            if(GetTime(SSTIMER) - previousTime > 15){
                commsBus1State = SAS_UPDATE;
                SSTx = true;
                break;
            }
            else{
                break;
            }
    }
}

void checkCommDirection() {
    //you have finished send and time has elapsed.. start listen
    if ( TXStallGet1() && (GetTime(TALKTIME) > CLOSE_COMM_TIME)) { //
        RS485_Direction1(LISTEN);
        SetTime(TALKTIME);
    }
}

void resetCommTimers() {
//    SetTime(SASTIMER);
//    SetTime(DDSTIMER);
//    SetTime(SSTIMER);
    //PDUTimer = 0;
}

void RS485_Direction1(int T_L) {
    RS485_1_Direction = T_L;
    SetTime(TALKTIME);
}

bool MCSTx = false;
bool BMMTx = false;
bool PDUTx = false;

// MCS_UPDATE = 0, BMM_UPDATE = 1,  PDU_UPDATE = 2//,

void bus2Update() {
    switch (commsBus2State) {
        case MCS_UPDATE:
            if(MCSTx){
                MCSTx = false;
                unsigned char Data[10];
                Data[0] = 0x11;
                Data[1] = 0x22;
                RS485_Direction2(TALK);         
                sendData_PDU_MCS_BMM(MCS_ADDRESS, READ_TABLE, TABLE_TWO_SAS, SAS_THROTTLE_1, Data, SS_FAULT_STATUS_LENTH);
                Delay(3);
                RS485_Direction2(LISTEN);
                SetTime(MCSTIMER);
                previousTime = GetTime(MCSTIMER);
            }
            //Packet received 
            if (RecivedValidPacket_PDU_MCS_BMM()) {
                commsBus2State = DDS_UPDATE;
                MCSTx = true;
                break;
            }
            //Time Out on packet
            if(GetTime(MCSTIMER) - previousTime > 15){
                commsBus2State = DDS_UPDATE;
                MCSTx = true;
            }
            else{
                break;
            }
        case BMM_UPDATE:
            if(BMMTx){
                BMMTx = false;
                unsigned char Data[2];
                Data[0] = 0x11;
                Data[1] = 0x22;
                RS485_Direction2(TALK);         
                sendData_PDU_MCS_BMM(BMM_ADDRESS, READ_TABLE, TABLE_THREE_DDS, SAS_THROTTLE_1, Data, SS_FAULT_STATUS_LENTH);
                Delay(3);
                RS485_Direction2(LISTEN);
                SetTime(BMMTIMER);
                previousTime = GetTime(BMMTIMER);
            }
            if (RecivedValidPacket_PDU_MCS_BMM()) {
                commsBus2State = SS_UPDATE;
                BMMTx = true;
                break;
            }
            if(GetTime(BMMTIMER) - previousTime > 15){
                commsBus2State = SS_UPDATE;
                BMMTx = true;
                break;
            }
            else{
                break;
            }
        case PDU_UPDATE:
            if(PDUTx){
                PDUTx = false;
                unsigned char Data[2];
                Data[0] =  constructPowerSet();
                //Data[1] = 0x22;
                RS485_Direction2(TALK);         
                sendData_PDU_MCS_BMM(PDU_ADDRESS, WRITE_TABLE, TABLE_FOUR_PDU, PDU_POWER_CONTROL, Data, 1);
                Delay(3);
                RS485_Direction2(LISTEN);
                SetTime(PDUTIMER);
                previousTime = GetTime(PDUTIMER);
            }
            if (RecivedValidPacket_PDU_MCS_BMM()) {
                commsBus2State = MCS_UPDATE;
                PDUTx = true;
                break;
            }
            if(GetTime(PDUTIMER) - previousTime > 15){
                commsBus2State = MCS_UPDATE;
                PDUTx = true;
                break;
            }
            else{
                break;
            }
    }
}

void checkCommDirection1() {
    //you have finished send and time has elapsed.. start listen
    if ( TXStallGet() && (GetTime(TALKTIME1) > 2)) { //5CLOSE_COMM_TIME TXStallGet1() &&
        RS485_Direction2(LISTEN);
    }
}

void RS485_Direction2(int T_L) {
    RS485_2_Direction = T_L;
    SetTime(TALKTIME1);
}

//Debug interface for printf
extern void Send_put2(unsigned char _data);
extern unsigned char Receive_get2(void);

char getch(void) {
    return Receive_get2();
}

void putch(char txData) {
    Send_put2(txData);
}

bool ComCheck(char device){
    if(device == MCSSTATE){
        return comms.MCS;
    }
    if(device == DDSSTATE){
        return comms.DDS;
    }
    if(device == SASSTATE){
        return comms.SAS;
    }
    if(device == BMMSTATE){
        return comms.BMM;
    }
    if(device == PDUSTATE){
        return comms.PDU;
    }
    if(device == SSSTATE){
        return comms.PDU;
    }
    else{
        return 0;
    }
}