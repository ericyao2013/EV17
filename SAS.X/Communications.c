
#include "Communications.h"
#include "ADC.h"

float i = 0;
bool pendingSend = false;
bool portClosed=true;
void updateComms() {
    checkCommDirection();
    //LED ^= 1;
    if (receiveData() && !pendingSend) {
        //LED ^= 1;
        talkTime = 0;
        pendingSend = true;
        portClosed=true;
    }
    else if(pendingSend && portClosed && talkTime > 5){
        talkTime=0;
        portClosed=false;
        RS485_1_Port = TALK;
    }
    else if (pendingSend && talkTime > 5 && !portClosed) {
        talkTime = 0;
        prepAndSendData();
        pendingSend = false;
    }   
}

void prepAndSendData() {
    ToSend(RESPONSE_ADDRESS, SAS_ADDRESS);
    ToSend(THROTTLE1_SAS, GetADC(Throttle1)); //GetADC(Throttle1)
    ToSend(THROTTLE2_SAS, GetADC(Throttle2));
    ToSend(BRAKE1_SAS, GetADC(Brake1));
    ToSend(BRAKE2_SAS, GetADC(Brake2));
    //ToSend(WHEELSPEED1_SAS, sender++);
    //ToSend(WHEELSPEED2_SAS, sender++);
    //ToSend(WHEELSPEED3_SAS, sender++);
    //ToSend(WHEELSPEED4_SAS, sender++);
    //    ToSend(8, sender++);
    //    ToSend(9, sender++);
    //    ToSend(10, sender++);
    //    ToSend(11, sender++);
    //    ToSend(12, sender++);
    //    ToSend(13, sender++);
    sendData(ECU_ADDRESS);
    talkTime = 0;
}

void checkCommDirection() {
    //you have finished send and time has elapsed.. start listen
    if (Transmit_stall && (talkTime > 10) && (RS485_1_Port == TALK) && !pendingSend && !portClosed) {
        RS485_1_Port = LISTEN;
        portClosed=true;
    }
}