/*
***********************************************************************************************
*                                           uC/OS-View
*
*                            (c) Copyright 2005, Micrium, Weston, FL
*                                       All Rights Reserved
*
*
* Filename   : VIEW.H
* Version    : V1.33
* Programmer : JJL
***********************************************************************************************
*/

#ifdef   VIEW_GLOBALS
#define  VIEW_EXT
#else
#define  VIEW_EXT  extern
#endif


#ifndef  VIEW_H
#define  VIEW_H

/*
*********************************************************************************************************
*                                              CONSTANTS
*********************************************************************************************************
*/

#define  VIEW_RX_BUF_SIZE              28    /* Rx buffer size (should not need to change)       */
#define  VIEW_TX_BUF_SIZE             128    /* Tx buffer size (should not need to change)       */
#define  VIEW_TX_STR_SIZE             128    /* Tx string size (should not need to change)       */

/*
***********************************************************************************************
*                                         VARIABLES
***********************************************************************************************
*/


VIEW_EXT  INT8U              View_RxState;
VIEW_EXT  INT8U              View_RxRemainLen;
VIEW_EXT  INT8U              View_RxChkSum;
VIEW_EXT  INT8U              View_RxBuf[VIEW_RX_BUF_SIZE];
VIEW_EXT  INT8U              View_RxBufCnt;
VIEW_EXT  INT8U              View_RxRdIx;
VIEW_EXT  INT32U             View_RxCtr;
VIEW_EXT  INT16U             View_RxPktCtr;

VIEW_EXT  INT16U             View_TxLen;
VIEW_EXT  INT16U             View_TxIx;
VIEW_EXT  INT8U              View_TxChkSum;
VIEW_EXT  INT8U              View_TxState;
VIEW_EXT  BOOLEAN            View_TxActiveFlag;
VIEW_EXT  INT8U              View_TxBuf[VIEW_TX_BUF_SIZE];
VIEW_EXT  INT16U             View_TxBufCnt;
VIEW_EXT  INT8U             *View_TxBufPtr;
VIEW_EXT  INT32U             View_TxCtr;
VIEW_EXT  INT16U             View_TxPktCtr;

VIEW_EXT  void               View_TxStoINT8U(INT8U tx_data);
VIEW_EXT  void               View_TxStoStr(INT8U *s);

VIEW_EXT  INT16U             View_TxStrLen;
VIEW_EXT  INT8U              View_TxStrBuf[VIEW_TX_STR_SIZE];
VIEW_EXT  INT16U             View_TxStrCnt;
VIEW_EXT  INT16U             View_TxStrDlyCtr;

VIEW_EXT  INT16U             View_TxLenEnd;
/*
***********************************************************************************************
*                                        PROTOTYPES
*                                    (PORT INDEPENDENT)
***********************************************************************************************
*/

void    View_RxHandler(INT8U data);               /* Processor independent Rx handler               */
void    View_TxHandler(void);                     /* Processor independent Tx handler               */
void    View_TxHandler_USB(void);                     /* Processor independent Tx handler               */

void    View_TxStr(INT8U *s, INT16U dly);         /* Send an ASCII string to terminal window        */

void    View_TerminalTx(INT8U data);              /* Write data to output window                    */

//INT32U  View_TimeGetCycles(void);                 /* Function to read fine resolution time base     */

//void    View_RxTxISR(void);                       /* Assembly Language Rx/Tx ISR                    */
//void    View_RxTxISRHandler(void);                /* C code to process Rx or Tx character           */

void    View_RxISR(void);                         /* Assembly Language Rx ISR                       */
void    View_RxISRHandler(void);                  /* C code to process character received           */
void    View_RxIntEn(void);                       /* Enable  Rx interrupts                          */
void    View_RxIntDis(void);                      /* Disable Rx interrupts                          */

void    View_TxISR(void);                         /* Assembly Language Tx ISR                       */
void    View_TxISRHandler(void);                  /* C code to process next character to send       */
void    View_TxIntEn(void);                       /* Enable  Tx interrupts                          */
void    View_TxIntDis(void);                      /* Disable Tx interrupts                          */
void    View_Tx1(INT8U c);                        /* Function to send a single character            */

void    View_GetCPUName(INT8U *s);                /* Returns the name of the CPU used               */
//INT32U  View_GetIntStkBase(void);                 /* Get the address of the interrupt stack base    */
//INT32U  View_GetIntStkSize(void);                 /* Get the address of the interrupt stack size    */
VIEW_EXT  void (*View_TerminalRxCallbackFnct)(INT8U data);

/*
*********************************************************************************************************
*                                      BSP FUNCTION PROTOTYPES
*********************************************************************************************************
*/

/*
***********************************************************************************************
*                                         #error DIRECTIVES
***********************************************************************************************
*/
#endif