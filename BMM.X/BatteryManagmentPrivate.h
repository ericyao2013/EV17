/* 
 * File:   BatteryManagmentPrivate.h
 * Author: Rick
 *
 * Created on April 11, 2016, 6:30 PM
 */

#ifndef BATTERYMANAGMENTPRIVATE_H
#define	BATTERYMANAGMENTPRIVATE_H

#include <stdbool.h>
#include <math.h>

bool CarOn = 0;
bool CurrentDir = 1; //Discharge by default

int k = 0;
int j = 0;

float CurrentOffset[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
int Current[6];
float CVolt[8];
double Volt1 = 0;
double Volt2 = 0;
float CC1;
float CC2;
float CC3;

char CFGR0 = 0;
char CFGR1 = 0;
char CFGR2 = 0;
char CFGR3 = 0;
char CFGR4 = 0;
char CFGR5 = 0;




#define Under_Voltage_Value_LTC  0x7CF // Compare Voltage = (Under_Voltage_Value +1) * 16 * 100uV  
// Default Under Voltage_LTC Value should be= 3.2 3.2=(1999+1)*16*100*10^-6
// 1999=0x7CF

#define  All_Stats 0x000 //SOC, ITMP, VA, VD
#define SOC  0x01 //Sum Of Cells
#define ITMP 0x02 //Internal Die Temp
#define VA   0x03 //Analog Power Supply
#define VD   0x04 //Digital Power Supply
#define Discharge_Time_Out_Value 0x000 // For chart refering to values Refer to 680412fb data sheet, Page 51,  Variable DCTO
#define NUMBEROFIC 12
#define NUMBEROFCH 2

#define NUMBEROFDATA 6
#define Amount_Of_Samples 10
//Discharge time out value 
#define DCTO 0 

#define CURRENTGAIN 40
#define VOLTAGERATIO 158.18
#define SHUNTOHMS 0.0001
#define ADCBIT 4095
#define Bypass_High_Limit 100 //Needs configuration
#define Bypass_Low_Limit 20 // Needs Configuration




int Voltage_data[1][12];
int Aux_data[1][6];

//LOGIC FOR BANK IS 1 and 2 Not 1 and 0
 int LTC6804_DATA_ConfigBank1[NUMBEROFIC][NUMBEROFDATA];
 int LTC6804_DATA_ConfigBank2[NUMBEROFIC][NUMBEROFDATA];
 int LTC6804_DATA_ConfigBank1Read[NUMBEROFIC][NUMBEROFDATA];
 int LTC6804_DATA_ConfigBank2Read[NUMBEROFIC][NUMBEROFDATA];
 int Stat_codes_Bank1[NUMBEROFIC][6];
 int Stat_codes_Bank2[NUMBEROFIC][6];
 int RETURN_LTC6804_DATA_ConfigBank1[NUMBEROFIC][NUMBEROFDATA];
 int RETURN_LTC6804_DATA_ConfigBank2[NUMBEROFIC][NUMBEROFDATA];
/*!< 
  The cell codes will be stored in the cell_codes[][12] array in the following format:
  
  |  cell_codes[0][0]| cell_codes[0][1] |  cell_codes[0][2]|    .....     |  cell_codes[0][11]|  cell_codes[1][0] | cell_codes[1][1]|  .....   |
  |------------------|------------------|------------------|--------------|-------------------|-------------------|-----------------|----------|
  |IC1 Cell 1        |IC1 Cell 2        |IC1 Cell 3        |    .....     |  IC1 Cell 12      |IC2 Cell 1         |IC2 Cell 2       | .....    |
 ****/


/*!<
 The GPIO codes will be stored in the aux_codes[][6] array in the following format:
 
 |  aux_codes[0][0]| aux_codes[0][1] |  aux_codes[0][2]|  aux_codes[0][3]|  aux_codes[0][4]|  aux_codes[0][5]| aux_codes[1][0] |aux_codes[1][1]|  .....    |
 |-----------------|-----------------|-----------------|-----------------|-----------------|-----------------|-----------------|---------------|-----------|
 |IC1 GPIO1        |IC1 GPIO2        |IC1 GPIO3        |IC1 GPIO4        |IC1 GPIO5        |IC1 Vref2        |IC2 GPIO1        |IC2 GPIO2      |  .....    |
 */
#define Start_Run_Mode 1
#define Start_Charge_Mode 2
void Charge_Mode();
void Run_Mode(bool Start_Setup);
int Run_ByPass(int cell_codesBank1[][12], int cell_codesBank2[][12]);
int Read_Battery(int BatteryPlacement, int cell_codes[NUMBEROFIC][12]);
//Configuration set functions
int SetTempEnable(int bank, int ic, bool value); //This sets the temp sensor on GPIO 5 to be  enabled or not  the bool will determine the value.
int SetUnderOverVoltage(int VUV, int VOV, int bank, int ic); //This sets the under voltage and overvoltage flag of the ltc6804. The values are #defines
int Set_ADC_Mode(int bank, int ic, bool ADCOPT_Mode); //This sets which adc modes are available for the MD Parameter one mode picks 3 available ADC while the the other one will pick the other ADC's
int Set_DCC_Mode_OFF(int bank, int ic); //This is to send all the DCC modes (Short the cell) off. This option is used when the car is operating normally. 
int Set_DCTO_Mode_OFF(int bank, int ic); // This is to set all the DCTO (Time to discharge  ) to 0. This works with DCC to 0 so the car can operate normally. 
int Set_REFON_Pin(int bank, int ic, bool REFON_Mode); //This is to set the Refrence voltage on and off based on mode  1=on 0=off
int SetBypass(int bank, int ic, int cell, bool value); //This sets the bypass for a cell on. This is so it can discharge the cell. This is placed in the configuration register to be sent. 

int Startuptests(int Stat_codes[NUMBEROFIC][6]); //This function is to test the LTC6804 to make sure everything is in check.
int CheckTestReading(int Stat_codes[NUMBEROFIC][6]); //Actually Checks the test from the data.

void Configure_LT6804();  //Setups the config registers to be sent the LT6804B.
int RunBypass_Set(int bank, double Conv_Cell_CodesBank[][12]);
void Run_GPIO_Temp_ColumbCounting_Timer();
int Read_GPIO(int BatteryPlacement, int aux_codes[NUMBEROFIC][6]);


void Open_All_ByPass(); 

//Fault Functions



#endif	/* BATTERYMANAGMENTPRIVATE_H */

