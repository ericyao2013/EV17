/*******************************************************************
 * @brief           EEprom.c
 * @brief           gives access to the EEPROM to set and store data
 * @return          N/A
 * @note            The lib is written for FT24C64A-ULR-T IC (http://www.fremontmicrousa.com/pdf/EN-24C32_64-300-V1p8.pdf)
 *******************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "I2C.h"
#include "EEprom.h"
#include "Functions.h"
#include "DriverConfigs.h"


#define ADDRESS 0x50

char data[8];



/*******************************************************************
 * @brief           EEpromInit
 * @brief           Setup EEprom
 * @return          none
 * @note            Sets up the I2C here
 *******************************************************************/

void EEpromInit(){
    InitI2C();
}

/*******************************************************************
 * @brief           writeRegister
 * @brief           Writes data to I2C device
 * @return          none
 * @note            uses commands from the I2C lib to construct packets to send out on the bus 
 *******************************************************************/
void writeRegister(char i2cAddress, int reg, char value)
{
    IdleI2C();
	StartI2C();						//Initiate start condition
	WriteI2C(i2cAddress << 1);	    //write 1 byte
	AckI2C();                       //Wait for ACK
    IdleI2C();                      //Ensure module is Idle
    WriteI2C(reg >> 8);			    //Write High word address
    AckI2C();   					//Wait for ACK
	IdleI2C();					    //Ensure module is Idle
    WriteI2C(reg & 0xff);			//Write Low word address
	AckI2C();						//Wait for ACK
    IdleI2C();                      //Ensure module is idle
    WriteI2C(value);				//Write Low word address
	AckI2C();					    //Wait for ACK
    IdleI2C();                      //Ensure module is idle
	StopI2C();				        //Send stop condition
    Delay(3);
}

/*******************************************************************
 * @brief           readRegister
 * @brief           reads data from the I2C device
 * @return          returns a byte from the bus
 * @note            uses commands from the I2C lib to construct packets to send out on the bus 
 *******************************************************************/
char readRegister(char i2cAddress, int reg)
{
    IdleI2C();
    StartI2C();						    //Initiate start condition
	WriteI2C(i2cAddress << 1);	        //write 1 byte
    AckI2C();   					    //Wait for ACK
	IdleI2C();						    //Ensure module is Idle
    WriteI2C(reg >> 8);			    	//Write High word address
    AckI2C();   					    //Wait for ACK
	IdleI2C();						    //Ensure module is Idle
    WriteI2C(reg & 0xff);			    //Write Low word address
    AckI2C();   					    //Wait for ACK
	IdleI2C();						    //Ensure module is Idle
	StopI2C();				            //Send stop condition
    IdleI2C();
	StartI2C();					     	//Initiate start condition
	WriteI2C((i2cAddress << 1) | 0x01);	//Write 1 byte - R/W bit should be 1 for read
    AckI2C();   					    //Wait for ACK
	IdleI2C();						    //Ensure bus is idle
	getsI2C(data, 1);		     	    //Read in multiple bytes
	//AckI2C();				     	    //Send Not Ack
    IdleI2C();						    //Ensure bus is idle
	StopI2C();						    //Send stop condition
    
    return data[0];
}



/*******************************************************************
 * @brief           ReadCarDriver
 * @brief           Reading data location for "current" car driver
 * @return          returns char
 * @note            assembles bytes together to make a valid data packet 
 *******************************************************************/
char ReadCarDriver(){
    return readRegister(ADDRESS, DRIVER_ACTIVE_EEPROM_LOCATION);
}

/*******************************************************************
 * @brief           SaveCarDriver
 * @brief           Saving data location for "current" car driver
 * @return          none
 * @note            assembles bytes together to make a valid data packet
 *******************************************************************/
void SaveCarDriver(char value){
    writeRegister(ADDRESS, DRIVER_ACTIVE_EEPROM_LOCATION,value);
}

/*******************************************************************
 * @brief           ReadCarDriverCount
 * @brief           Reading car driver count - how many drivers we have saved
 * @return          returns byte
 * @note            assembles bytes together to make a valid data packet 
 *******************************************************************/
char ReadCarDriverCount(){
    return readRegister(ADDRESS, DRIVER_SAVED_EEPROM_LOCATION);
}

/*******************************************************************
 * @brief           SaveCarDriverCount
 * @brief           Saving car driver count
 * @return          none
 * @note            assembles bytes together to make a valid data packet 
 *******************************************************************/
void SaveCarDriverCount(char value){
    writeRegister(ADDRESS, DRIVER_SAVED_EEPROM_LOCATION,value);
}

/*******************************************************************
 * @brief           ReadThrottlePrecent
 * @brief           Reading data location for car throttle mismatch %
 * @return          return byte
 * @note            assembles bytes together to make a valid data packet 
 *******************************************************************/
int ReadThrottlePrecent(){
    return readRegister(ADDRESS, THROTTLE_ERROR_EEPROM_LOCATION);
}

/*******************************************************************
 * @brief           SaveThrottlePrecent
 * @brief           Saving data location for car throttle mismatch %
 * @return          none
 * @note            assembles bytes together to make a valid data packet 
 *******************************************************************/
void SaveThrottlePrecent(int value){
    writeRegister(ADDRESS, THROTTLE_ERROR_EEPROM_LOCATION, value);
}

/*******************************************************************
 * @brief           ReadThrottleTrigger
 * @brief           Reading data location for throttle max value for software safety system
 * @return          return byte
 * @note            assembles bytes together to make a valid data packet 
 *******************************************************************/
int ReadThrottleTrigger(){
    return readRegister(ADDRESS, THROTTLE_MAX_EEPROM_LOCATION);
}

/*******************************************************************
 * @brief           SaveThrottleTrigger
 * @brief           Saving data location for throttle max value for software safety system
 * @return          none
 * @note            assembles bytes together to make a valid data packet 
 *******************************************************************/
void SaveThrottleTrigger(int value){
    writeRegister(ADDRESS, THROTTLE_MAX_EEPROM_LOCATION, value);
}

/*******************************************************************
 * @brief           ReadBrakeTrigger
 * @brief           Reading data location for brake max value for software safety system
 * @return          return byte
 * @note            assembles bytes together to make a valid data packet 
 *******************************************************************/
int ReadBrakeTrigger(){
    return readRegister(ADDRESS, BRAKE_MAX_EEPROM_LOCATION);
}

/*******************************************************************
 * @brief           SaveBrakeTrigger
 * @brief           Saving data location for brake max value for software safety system
 * @return          none
 * @note            assembles bytes together to make a valid data packet 
 *******************************************************************/
void SaveBrakeTrigger(int value){
    writeRegister(ADDRESS, BRAKE_MAX_EEPROM_LOCATION, value);
    
}

/*******************************************************************
 * @brief           ReadBrakeLightTrigger
 * @brief           Reading data location for brake light threshold
 * @return          return byte
 * @note            assembles bytes together to make a valid data packet 
 *******************************************************************/
int ReadBrakeLightTrigger(){
    return readRegister(ADDRESS, BRAKE_LIGHT_THRESHOLDS_EEPROM_LOCATION);
}

/*******************************************************************
 * @brief           SaveBrakeLightTrigger
 * @brief           Saving data location for brake light threshold
 * @return          none
 * @note            assembles bytes together to make a valid data packet 
 *******************************************************************/
void SaveBrakeLightTrigger(int value){
    writeRegister(ADDRESS, BRAKE_LIGHT_THRESHOLDS_EEPROM_LOCATION, value);
    SetBrakeLightValue(value);
}

/*******************************************************************
 * @brief           ReadPinCode
 * @brief           Reading data location for code
 * @return          return int of pin
 * @note            assembles bytes together to make a valid data packet 
 *******************************************************************/
int ReadPinCode(){
    char High, Low = 0;
    High = readRegister(ADDRESS, PIN_HIGH_EEPROM_LOCATION);
    Low = readRegister(ADDRESS, PIN_LOW_EEPROM_LOCATION);
    return (Low | (High << 8));
}

/*******************************************************************
 * @brief           SavePinCode
 * @brief           Saving data location for pin code
 * @return          none
 * @note            assembles bytes together to make a valid data packet 
 *******************************************************************/
void SavePinCode(int value){
    writeRegister(ADDRESS, PIN_HIGH_EEPROM_LOCATION, (value & 0xFF));
    writeRegister(ADDRESS, PIN_LOW_EEPROM_LOCATION, ((value >> 8) & 0xFF));
}

/*******************************************************************
 * @brief           ReadCarLock
 * @brief           Reading data location to see car lock condition
 * @return          return int of pin
 * @note            assembles bytes together to make a valid data packet 
 *******************************************************************/
bool ReadCarLock(){
    return readRegister(ADDRESS, CAR_LOCK_EEPROM_LOCATION);
}

/*******************************************************************
 * @brief           SaveCarLock
 * @brief           Saving data location to lock / unlock car
 * @return          none
 * @note            assembles bytes together to make a valid data packet 
 *******************************************************************/
void SaveCarLock(bool value){
    writeRegister(ADDRESS, CAR_LOCK_EEPROM_LOCATION, value);
}

/*******************************************************************
 * @brief           ReadCarStartSwitch
 * @brief           reading data location for start button
 * @return          char with switch input location
 * @note            assembles bytes together to make a valid data packet 
 *******************************************************************/
char ReadCarStartSwitch(){
    return readRegister(ADDRESS, CAR_SW_START_EEPROM_LOCATION);
}

/*******************************************************************
 * @brief           SaveCarStartSwitch
 * @brief           Saving data location for start button
 * @return          none
 * @note            assembles bytes together to make a valid data packet 
 *******************************************************************/
void SaveCarStartSwitch(char value){
    writeRegister(ADDRESS, CAR_SW_START_EEPROM_LOCATION, value);
}

/*******************************************************************
 * @brief           ReadCarTestSwitch
 * @brief           reading data location for TEST button
 * @return          char with switch input location
 * @note            assembles bytes together to make a valid data packet 
 *******************************************************************/
char ReadCarTestSwitch(){
    return readRegister(ADDRESS, CAR_SW_TEST_EEPROM_LOCATION);
}

/*******************************************************************
 * @brief           SaveCarStartSwitch
 * @brief           Saving data location for start button
 * @return          none
 * @note            assembles bytes together to make a valid data packet 
 *******************************************************************/
void SaveCarTestSwitch(char value){
    writeRegister(ADDRESS, CAR_SW_TEST_EEPROM_LOCATION, value);
}

/*******************************************************************
 * @brief           ReadCarBMMLight
 * @brief           reading data location for BMM light
 * @return          char with BMM light location
 * @note            assembles bytes together to make a valid data packet 
 *******************************************************************/
char ReadCarBMMLight(){
    return readRegister(ADDRESS, CAR_LT_BMM_EEPROM_LOCATION);
}

/*******************************************************************
 * @brief           SaveCarBMMLight
 * @brief           Saving data location for BMM light
 * @return          none
 * @note            assembles bytes together to make a valid data packet 
 *******************************************************************/
void SaveCarBMMLight(char value){
    writeRegister(ADDRESS, CAR_LT_BMM_EEPROM_LOCATION, value);
}

/*******************************************************************
 * @brief           ReadCarIMDLight
 * @brief           reading data location for IMD light
 * @return          char with BMM light location
 * @note            assembles bytes together to make a valid data packet 
 *******************************************************************/
char ReadCarIMDLight(){
    return readRegister(ADDRESS, CAR_LT_IMD_EEPROM_LOCATION);
}

/*******************************************************************
 * @brief           SaveCarIMDLight
 * @brief           Saving data location for IMD light
 * @return          none
 * @note            assembles bytes together to make a valid data packet 
 *******************************************************************/
void SaveCarIMDLight(char value){
    writeRegister(ADDRESS, CAR_LT_IMD_EEPROM_LOCATION, value);
}

/*******************************************************************
 * @brief           ReadCarACTLight
 * @brief           reading data location for IMD light
 * @return          char with BMM light location
 * @note            assembles bytes together to make a valid data packet 
 *******************************************************************/
char ReadCarACTLight(){
    return readRegister(ADDRESS, CAR_LT_ACT_EEPROM_LOCATION);
}

/*******************************************************************
 * @brief           SaveCarACTLight
 * @brief           Saving data location for IMD light
 * @return          none
 * @note            assembles bytes together to make a valid data packet 
 *******************************************************************/
void SaveCarACTLight(char value){
    writeRegister(ADDRESS, CAR_LT_ACT_EEPROM_LOCATION, value);
}

/*******************************************************************
 * @brief           ReadDriverConfig
 * @brief           Reading driver config and save it in struct for getters 
 * @return          N/A
 * @note            assembles bytes together to make a valid data packet 
 *******************************************************************/
void ReadDriverConfig(char dvr){
    char data[2];
    int offset = DRIVERCONFIGSTART*dvr;
    DriverNameChar(readRegister(ADDRESS, (offset)), 0);
    DriverNameChar(readRegister(ADDRESS, (offset+1)), 1);
    DriverNameChar(readRegister(ADDRESS, (offset+2)), 2);
    data[0] = readRegister(ADDRESS, offset+3);
    data[1] = readRegister(ADDRESS, offset+4);
    DriverMaxThrottle((data[0] << 8) + data[1]);
    data[0] = readRegister(ADDRESS, offset+5);
    data[1] = readRegister(ADDRESS, offset+6);
    DriverMaxMaxRegen((data[0] << 8) + data[1]);
    data[0] = readRegister(ADDRESS, offset+7);
    data[1] = readRegister(ADDRESS, offset+8);
    DriverLowBatCutoff((data[0] << 8) + data[1]);
    data[0] = readRegister(ADDRESS, offset+9);
    data[1] = readRegister(ADDRESS, offset+10);
    DriverRamp((data[0] << 8) + data[1]);
    DriverFalt(readRegister(ADDRESS, offset+11));
    data[0] = readRegister(ADDRESS, offset+12);
    DriverFW_RW_EN((data[0] >> 1)&(0x01));
    DriverRegenInput(((data[0] >> 2)&(0x01)));
    DriverDebugEn(((data[0] >> 3)&(0x01)));
}

/*******************************************************************
 * @brief           SaveDriverConfig
 * @brief           Saving data location for Driver Data
 * @return          none
 * @note            assembles bytes together to make a valid data packet 
 *******************************************************************/
void SaveDriverConfig(char NAME[3], int MaxThrottle, int MaxRegen, int LowBatCutoff, int Ramp, char Fault, bool Fw_Rv, bool RegenInput, bool DebugEn){
    char number = ReadCarDriverCount();
    number++;
    SaveCarDriverCount(number);
    int offset = DRIVERCONFIGSTART*number;
    writeRegister(ADDRESS, offset, NAME[0]);
    writeRegister(ADDRESS, offset+1, NAME[1]);
    writeRegister(ADDRESS, offset+2, NAME[2]);
    writeRegister(ADDRESS, offset+3, MaxThrottle>>8);
    writeRegister(ADDRESS, offset+4, MaxThrottle & 0xff);
    writeRegister(ADDRESS, offset+5, MaxRegen>>8);
    writeRegister(ADDRESS, offset+6, MaxRegen & 0xff);
    writeRegister(ADDRESS, offset+7, LowBatCutoff>>8);
    writeRegister(ADDRESS, offset+8, LowBatCutoff & 0xff);
    writeRegister(ADDRESS, offset+9, Ramp>>8);
    writeRegister(ADDRESS, offset+10, Ramp & 0xff);
    writeRegister(ADDRESS, offset+11, Fault);
    writeRegister(ADDRESS, offset+12, ((DebugEn&0x01) << 2) | ((RegenInput&0x01) << 2) | ((Fw_Rv&0x01) << 1));
}

/*******************************************************************
 * @brief           ReadDriverName
 * @brief           Reading driver names for list  
 * @return          N/A
 * @note            assembles bytes together to make a valid data packet 
 *******************************************************************/
void ReadDriverNames(char dvr){
    int offset = DRIVERCONFIGSTART*dvr;
    insert(readRegister(ADDRESS, (offset)));
    insert(readRegister(ADDRESS, (offset+1)));
    insert(readRegister(ADDRESS, (offset+2)));
    insert('0');
}