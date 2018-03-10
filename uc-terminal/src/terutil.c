/*
*********************************************************************************************************
*                                           Terminal Utils Interface
*
*
*
* Filename   : terutil.c
* Version    : V1.00
* Programmer : Puleo
*********************************************************************************************************
*/


#define  UTIL_GLOBALS
#include <includes.h>

#if USBD_MODULE == DEF_ENABLED
/*
*********************************************************************************************************
*                                               CONSTANTS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                           LOCAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/
static  void    View_CmdFhtDefault(void);
static  void    View_CmdTfDefault(void);
static  void    View_CmdFhtTfDefault(void);

static  void    View_CmdFhtHcRealtime(void);
static  void    View_CmdFhtHcExRealtime(void);
static  void    View_CmdTfRealtime(void);
static  void    View_CmdLastValRealtime(void);
static  void    View_CmdLogRealtime(void);
static  void    View_CmdClrRealtime(void);
static  void    View_CmdUnTF(void);

/*
*********************************************************************************************************
*                                       uC/OS-View COMMAND HANDLER
*                                            (Util Parse Packet)
*********************************************************************************************************
*/

void    Util_CmdParsePkt (INT8U cmd0)
{


    switch (cmd0) {
                         
        /* Default FHT and TF Structs send */
             
        case 'f':
             View_TxStoINT8U(cmd0);
             View_TxStoINT8U(0x7C);
             View_CmdFhtDefault();
             break;

        case 'a':
             View_TxStoINT8U(cmd0);
             View_TxStoINT8U(0x7C);
             View_CmdTfDefault();
             break;
           
        case 'c':
             View_TxStoINT8U(cmd0);
             View_TxStoINT8U(0x7C);
             View_CmdFhtTfDefault();
             break;  
             
        /* Realtime FHT and TF Structs send */
             
        case 'H':
             View_TxStoINT8U(cmd0);
             View_TxStoINT8U(0x7C);
             View_CmdFhtHcRealtime();
             break;
             
        case 'E':
             View_TxStoINT8U(cmd0);
             View_TxStoINT8U(0x7C);
             View_CmdFhtHcExRealtime();
             break;

        case 'A':
             View_TxStoINT8U(cmd0);
             View_TxStoINT8U(0x7C);
             View_CmdTfRealtime();
             break;
             
        case 'V':
             View_TxStoINT8U(cmd0);
             View_TxStoINT8U(0x7C);
             View_CmdLastValRealtime();
             break;
            
        case 'L':
             View_TxStoINT8U(cmd0);
             View_TxStoINT8U(0x7C);
             View_CmdLogRealtime();
             break;
       
        case 'U':
             View_TxStoINT8U(cmd0);
             View_TxStoINT8U(0x7C);
             View_CmdUnTF();
             break;
            
        case 'D':
             View_TxStoINT8U(cmd0);
             View_TxStoINT8U(0x7C);
             View_CmdClrRealtime();
             break;
             
        default:
             break;
    }
}

/*
*********************************************************************************************************
*                                       uC/OS-View COMMAND HANDLER
*                                           (Write Functions)
*********************************************************************************************************
*/

static  void  View_CmdFhtDefault(void)
{
    INT8U i;
    
    for (i=0; i < maxDevices; i++){
        View_TxStoINT8U(FHTDevAccess[i].hc1);
        View_TxStoINT8U(0x20);
        View_TxStoINT8U(FHTDevAccess[i].hc2);
        View_TxStoINT8U(0x7C);
    }
}

/********************************************************************************************************/

static  void  View_CmdTfDefault(void)
{
    INT8U i;
    
    for (i=0; i < TFmaxDev; i++){
        View_TxStoINT8U(TFDevAccess[i].hc1);
        View_TxStoINT8U(0x20);
        View_TxStoINT8U(TFDevAccess[i].hc2);
         View_TxStoINT8U(0x20);
        View_TxStoINT8U(TFDevAccess[i].hc3);
        View_TxStoINT8U(0x7C);
    }
}
/********************************************************************************************************/

static  void  View_CmdFhtTfDefault(void)
{
    INT8U i;
    
    for (i=0; i < TFmaxDev; i++){
        View_TxStoINT8U(FHTTFPaar[i].hc1);
        View_TxStoINT8U(0x20);
        View_TxStoINT8U(FHTTFPaar[i].idx);
        View_TxStoINT8U(0x7C);
    }
}
/********************************************************************************************************/
static  void  View_CmdFhtHcRealtime(void)
{
    INT8U i;
    
    for (i=0; i < maxDevices; i++){
        View_TxStoINT8U(FHTDev[i].hc1);
        View_TxStoINT8U(0x20);
        View_TxStoINT8U(FHTDev[i].hc2);
        View_TxStoINT8U(0x7C);
    }
}

/********************************************************************************************************/
static  void  View_CmdFhtHcExRealtime(void)
{
    INT8U i;
    
    for (i=0; i < maxDevices; i++){
        
        View_TxStoINT8U(FHTDev[i].command);
        View_TxStoINT8U(0x20);
        View_TxStoINT8U(FHTDev[i].extension);
        View_TxStoINT8U(0x7C);
    }
}
/********************************************************************************************************/

static  void  View_CmdTfRealtime(void)
{
    INT8U i;
    
    for (i=0; i < TFmaxDev; i++){
        View_TxStoINT8U(TFDev[i].hc1);
        View_TxStoINT8U(0x20);
        View_TxStoINT8U(TFDev[i].hc2);
        View_TxStoINT8U(0x20);
        View_TxStoINT8U(TFDev[i].hc3);
        View_TxStoINT8U(0x20);
        View_TxStoINT8U(TFDev[i].command);
        View_TxStoINT8U(0x7C);
    }
}

/********************************************************************************************************/

static  void  View_CmdLastValRealtime(void)
{
        View_TxStoINT8U(FHTLedVal);
        View_TxStoINT8U(0x7C);
}

/********************************************************************************************************/

static  void  View_CmdLogRealtime(void)
{
        View_TxStoStr(logMsg);
        View_TxStoINT8U(0x7C);
}

/********************************************************************************************************/

static  void  View_CmdUnTF(void)
{
        View_TxStoStr(logMsgTF);
        View_TxStoINT8U(0x7C);
}

/********************************************************************************************************/

static  void  View_CmdClrRealtime(void)
{
        InitStructs();
        FHTLedVal = 0;
        View_TxStoINT8U(0x01);
        View_TxStoINT8U(0x7C);
}
#endif