/*
***********************************************************************************************
*                                           RF12B Interface
*
*                            
*
* Filename   : rfm69cw.h
* Version    : V1.
* Programmer : Puleo
***********************************************************************************************
*/


#ifdef   RFM69CW_GLOBALS
#define  RFM69CW_EXT
#else
#define  RFM69CW_EXT  extern
#endif

#ifndef  RFM69CW_H
#define  RFM69CW_H
/*
***********************************************************************************************
*                                         VARIABLES
***********************************************************************************************
*/
RFM69CW_EXT     INT8U regVal[0x6F];
/*
***********************************************************************************************
*                                         CONSTANS
***********************************************************************************************
*/
#define RF69OOK_MODE_SLEEP       0 // XTAL OFF
#define RF69OOK_MODE_STANDBY     1 // XTAL ON
#define RF69OOK_MODE_SYNTH       2 // PLL ON
#define RF69OOK_MODE_RX          3 // RX MODE
#define RF69OOK_MODE_TX          4 // TX MODE

#define null                     0
#define COURSE_TEMP_COEF        -90                 // puts the temperature reading in the ballpark, user can fine tune the returned value
#define RF69OOK_FSTEP            61.03515625        // == FXOSC/2^19 = 32mhz/2^19 (p13 in DS) 
/*
***********************************************************************************************
*                                        PROTOTYPES
*                                    (PORT INDEPENDENT)
***********************************************************************************************
*/


/*
*********************************************************************************************************
*                                      BSP FUNCTION PROTOTYPES
*********************************************************************************************************
*/

RFM69CW_EXT     void RFM69CW_ExtiEnabled(void);

RFM69CW_EXT     CPU_BOOLEAN RFM69CW_InitializeOOK(void);

RFM69CW_EXT     INT64U RFM69CW_GetFrequency(void);
RFM69CW_EXT     void RFM69CW_SetFrequencyMHz(CPU_FP32 f);
RFM69CW_EXT     void RFM69CW_SetFrequencyKHz(INT32U f);

RFM69CW_EXT     INT8U RFM69CW_ReadRSSI(CPU_BOOLEAN forceTrigger);
RFM69CW_EXT     void RFM69CW_SetHighPowerRegs(CPU_BOOLEAN onOFF);                 
RFM69CW_EXT     void RFM69CW_SetHighPower(CPU_BOOLEAN onOFF);                  //have to call it after initialize for RFM69HW
RFM69CW_EXT     void RFM69CW_SetPowerLevel(INT8U level);                       //reduce/increase transmit power level
RFM69CW_EXT     void RFM69CW_Sleep(void);
RFM69CW_EXT     INT8U RFM69CW_ReadTemperature(INT8U calFactor);                //get CMOS temperature (8bit)
RFM69CW_EXT     void RFM69CW_RcCalibration(void);                              //calibrate the internal RC oscillator for use in wide temperature variations - see datasheet section [4.3.5. RC Timer Accuracy]

// allow hacking registers by making these public
RFM69CW_EXT     void RFM69CW_ReadAllRegs(void);
RFM69CW_EXT     void RFM69CW_RcCalibitrateation(void);


// functions related to OOK mode
RFM69CW_EXT     void RFM69CW_ReceiveBegin(void);
RFM69CW_EXT     void RFM69CW_ReceiveEnd(void);
RFM69CW_EXT     void RFM69CW_TransmitBegin(void);
RFM69CW_EXT     void RFM69CW_TransmitEnd(void);
RFM69CW_EXT     CPU_BOOLEAN RFM69CW_Poll(void);
RFM69CW_EXT     void RFM69CW_SetBandwidth(INT8U bw);
RFM69CW_EXT     void RFM69CW_SetBitrate(INT32U bitrate);
RFM69CW_EXT     void RFM69CW_SetRSSIThreshold(INT8U rssi);
RFM69CW_EXT     void RFM69CW_SetFixedThreshold(INT8U threshold);
RFM69CW_EXT     void RFM69CW_SetSensitivityBoost(INT8U value); 

RFM69CW_EXT     INT8U RF69CW_GetIRQ1Flags(void);
RFM69CW_EXT     INT8U RF69CW_Poll(void);




#endif