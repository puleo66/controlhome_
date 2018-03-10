/*
***********************************************************************************************
*                                           FS20 Interface
*
*                            
*
* Filename   : fs20.h
* Version    : V1.
* Programmer : Puleo
***********************************************************************************************
*/


#ifdef   FS20_GLOBALS
#define  FS20_EXT
#else
#define  FS20_EXT  extern
#endif

#ifndef  FS20_H
#define  FS20_H
/*
***********************************************************************************************
*                                         CONSTANS
***********************************************************************************************
*/
#define maxDevices  7      // Bei änderungen der max funktioniert die Windows Software nicht mehr!
#define TFmaxDev    3
#define maxFloor    4

/*
***********************************************************************************************
*                                         VARIABLES
***********************************************************************************************
*/

FS20_EXT struct {
                 INT8S  bits;
                 INT8U  state; 
                 INT8U  prev;
                 INT64U data;
                } FS20;

FS20_EXT struct {
                 INT8U hc1;
                 INT8U hc2;
                 INT8U command;
                 INT8U extension;
                 INT8U lastVal;
                } FHTDev[maxDevices];

FS20_EXT struct {
                 INT8U hc1;
                 INT8U hc2;
                 INT8U hc3;
                 INT8U command;
                 INT8U lastVal;
                } TFDev[TFmaxDev];

FS20_EXT struct {
                 INT8U devNum[3];
                 INT8U devAct;
                 INT8U value;
                } FloorNum[maxFloor];

FS20_EXT struct {
                 INT8U hc1;
                 INT8U hc2;
                } FHTDevAccess[maxDevices];

FS20_EXT struct {
                 INT8U hc1;
                 INT8U hc2;
                 INT8U hc3;
                } TFDevAccess[TFmaxDev];

FS20_EXT struct {
                 INT8U hc1;
                 INT8U idx;
                } FHTTFPaar[TFmaxDev];

FS20_EXT        INT8U FHTDevAct;
FS20_EXT        INT8U TFDevAct;

FS20_EXT        INT8U FHTTFPaarAct;
FS20_EXT        INT8U FHTLoadAct;
FS20_EXT        INT8U TFLoadAct;
                 
FS20_EXT        INT8U FHTLedVal;
FS20_EXT        INT8U logMsg[15];
FS20_EXT        INT8U logMsgTF[15];


/*
***********************************************************************************************
*                                        PROTOTYPES
*                                    
***********************************************************************************************
*/

INT8S FHTDevNr(INT8U hc1, INT8U hc2);
INT8S FHTDevEnabled(INT8U hc1, INT8U hc2);
INT8S TFDevNr(INT8U hc1, INT8U hc2, INT8U hc3);
INT8S TFDevEnabled(INT8U hc1, INT8U hc2, INT8U hc3);
INT8S TFFloor(INT8U FHTNr);

void  FS20_ISRHandler (void);
void  FS20_TimeISRHandler (void);
void  reset_FS20(void);
                 
void FHTValue(INT8U Nr);
void TFValue(INT8U Nr);
void FHTScan(INT8U hc1, INT8U hc2, INT8U hc4, INT8U hc5);
void TFScan(INT8U hc1, INT8U hc2, INT8U hc3, INT8U hc4);
void FHTFloor(INT8U Nr);
void PaarLoad(void);


static          void FHTDevClear(void);
static          void TFDevClear(void);
static          void FloorClear(void);

FS20_EXT        void InitStructs(void);
FS20_EXT        void ReceiverScan(void);

FS20_EXT        void FHTDevLoad(void);
FS20_EXT        void TFDevLoad(void);


#endif