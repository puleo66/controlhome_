/*
*********************************************************************************************************
*                                           uC/OS-View
*
*                             (c) Copyright 2005, Micrium, Weston, FL
*                                       All Rights Reserved
*
*
* Filename   : VIEW.C
* Version    : V1.33
* Programmer : JJL
*********************************************************************************************************
*/

#define  VIEW_GLOBALS
#include <includes.h>

#if USBD_MODULE == DEF_ENABLED
/*
*********************************************************************************************************
*                                               CONSTANTS
*********************************************************************************************************
*/

#ifndef  VIEW_SUPPORT_MEM
#define  VIEW_SUPPORT_MEM               0                /* Current version doesn't support memory Rd/Wr    */
#endif
                                                            /* Inbound  packets (from PC)                      */
#define  VIEW_PROTOCOL_RX_SD0        0xED                /* Start delimiters                                */
#define  VIEW_PROTOCOL_RX_SD1        0x8C
#define  VIEW_PROTOCOL_RX_ED         0x8D                /* End   delimiter                                 */

                                                            /* Outbound packets (to PC)                        */
#define  VIEW_PROTOCOL_TX_SD0        0x8C                /* Start delimiters                                */
#define  VIEW_PROTOCOL_TX_SD1        0xED
#define  VIEW_PROTOCOL_TX_ED         0x8D                /* End   delimiter                                 */


#define  VIEW_RX_STATE_SD0              0                /* waiting for start first  start delimiter (SD0)  */
#define  VIEW_RX_STATE_SD1              1                /* waiting for start second start delimiter (SD1)  */
#define  VIEW_RX_STATE_LEN              2                /* waiting for len byte                            */
#define  VIEW_RX_STATE_DATA             3                /* waiting for data                                */
#define  VIEW_RX_STATE_CHKSUM           4                /* waiting for checksum                            */
#define  VIEW_RX_STATE_ED               5                /* waiting for end delimiter                       */

#define  VIEW_TX_STATE_SD0              0                /* Transmit state machine states                   */
#define  VIEW_TX_STATE_SD1              1
#define  VIEW_TX_STATE_DATA_LEN         2
#define  VIEW_TX_STATE_DATA             3
#define  VIEW_TX_STATE_CHKSUM           4
#define  VIEW_TX_STATE_ED               5
#define  VIEW_TX_STATE_STR_LEN          6
#define  VIEW_TX_STATE_STR_TYPE         7
#define  VIEW_TX_STATE_STR_DATA         8

#define  VIEW_CPU_NAME_SIZE            30

/*$PAGE*/
/*
*********************************************************************************************************
*                                           LOCAL VARIABLES
*********************************************************************************************************
*/
OS_TCB  *ptcb;
INT8U    stat;

/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  INT8U   View_CmdParsePkt(void);
static  void    View_CmdGetSysInfo(void);
static  void    View_CmdGetTaskList(void);
static  void    View_CmdGetTaskInfo(void);
static  void    View_CmdGetStackInfo (void);
static  void    View_CmdGetTaskInfo_InvalidTask(void);
static  INT8U   View_CmdGetTaskInfo_TCBStat(OS_TCB *ptcb);
static  void    View_CmdMemRdINT8U(void);
static  void    View_CmdMemRdINT16U(void);
static  void    View_CmdMemRdINT32U(void);
static  void    View_CmdMemWrINT8U(void);
static  void    View_CmdMemWrINT16U(void);
static  void    View_CmdMemWrINT32U(void);

void    View_RxPkt(void);
static  void    View_RxStoINT8U(INT8U rx_data);
static  void    View_RxBufClr(void);
static  INT8U   View_RxGetINT8U(void);
static  INT16U  View_RxGetINT16U(void);
static  INT32U  View_RxGetINT32U(void);

static  void    View_TxStart(void);
static  void    View_TxStoINT16U(INT16U tx_data);
static  void    View_TxStoINT32U(INT32U tx_data);
static  void    View_TxStoPtr(void *ptr);

static  void    View_TxBufClr (void);
static  void    View_TxStoUSB (INT8U tx_data);
static  void    View_Tx_USB (void);
static  void    View_TxDefClr (void);
/*$PAGE*/
/*
*********************************************************************************************************
*                                       uC/OS-View COMMAND HANDLER
*                                            (Parse Packet)
*********************************************************************************************************
*/

static  INT8U  View_CmdParsePkt (void)
{
    INT8U cmd0;


    cmd0 = View_RxGetINT8U();                             /* First byte contains command                     */
    Util_CmdParsePkt (cmd0);
/*
    if (cmd0 == 'C') {
        //OSView_CmdSendToApp();
        View_TxBufCnt = 0;
        return (0);
    }
*/
    //if (View_TxBufCnt > 0) {                              /* Do cmds only if Tx of the last one is done      */
    //    return (0);
    //}

    switch (cmd0) {
             
        case 's':
             View_TxStoINT8U(cmd0);
             View_TxStoINT8U(0x7C);
             View_CmdGetSysInfo();
             break;

        case 't':
             View_TxStoINT8U(cmd0);
             View_TxStoINT8U(0x7C);
             View_CmdGetTaskInfo();
             break;
             
        case 'T':
             View_TxStoINT8U(cmd0);
             View_TxStoINT8U(0x7C);
             View_CmdGetStackInfo();
             break;

        case 'l':
             View_TxStoINT8U(cmd0);
             View_TxStoINT8U(0x7C);
             View_CmdGetTaskList();
             break;

        case 'b':
             View_TxStoINT8U(cmd0);
             View_TxStoINT8U(0x7C);
             View_CmdMemRdINT8U();
             break;
             
        case 'w':
             View_TxStoINT8U(cmd0);
             View_CmdMemRdINT16U();
             break;

        case '0':
             View_TxStoINT8U(cmd0);
             View_CmdMemRdINT32U();
             break;

        case '1':
             View_TxStoINT8U(cmd0);
             View_CmdMemWrINT8U();
             break;

        case '2':
             View_TxStoINT8U(cmd0);
             View_CmdMemWrINT16U();
             break;

        case '4':
             View_TxStoINT8U(cmd0);
             View_CmdMemWrINT32U();
             break;

        default:
             break;
    }
    
    /* Terminal Util */
/*
    if(View_TxBufCnt == 0){
      Util_CmdParsePkt (cmd0);
    }
*/    
    return (View_TxBufCnt);
}

/*
*********************************************************************************************************
*                                       uC/OS-View COMMAND HANDLER
*                                        (Get System Information)
*********************************************************************************************************
*/

static  void  View_CmdGetSysInfo (void)
{
    INT8U   s[VIEW_CPU_NAME_SIZE+1];
    INT8U   i;
    INT32U  time;

    time = OSTimeGet();                                   /* Send current value of tick counter              */
    View_TxStoINT32U(time);
    View_TxStoINT8U(0x7C);                                   /* Indicate profiling information will be sent     */
    View_TxStoPtr(OSTCBCur);                              /* Current task's OS_TCB                           */
    View_TxStoINT8U(0x7C);                                   /* Indicate profiling information will be sent     */
    View_TxStoINT16U(OS_VERSION);                         /* Send uC/OS-II's version number                  */
    View_TxStoINT8U(0x7C);                                   /* Indicate profiling information will be sent     */

    for (i = 0; i < VIEW_CPU_NAME_SIZE; i++) {            /* Clear buffer to help with debug.                */
        s[i] = 0x00;
    }
    View_GetCPUName(s);                                   /* Get Processor Name in string 's'                */
    View_TxStoStr(s);

}

/*
*********************************************************************************************************
*                                       uC/OS-View COMMAND HANDLER
*                                            (Get Task List)
*********************************************************************************************************
*/

static  void  View_CmdGetTaskList (void)
{
    INT8U   prio;
                                                      /* Identifier for data type                        */
    for (prio = 0; prio <= OS_LOWEST_PRIO; prio++) {
        ptcb = OSTCBPrioTbl[prio];
        if (ptcb != (OS_TCB *)0 && ptcb != (OS_TCB *)1) {        /* Does task exist? (Not a Mutex reserved task)    */
            View_TxStoINT32U((INT32U)ptcb);                      /* Yes, send the task's TCB address                */
            View_TxStoINT8U(0x7C);
        }
    }
}

/*
*********************************************************************************************************
*                                       uC/OS-View COMMAND HANDLER
*                                         (Get Task Information)
*********************************************************************************************************
*/

static  void  View_CmdGetStackInfo (void)
{


    ptcb   = (OS_TCB *)View_RxGetINT32U();                         /* Get the desired task's TCB address     */

    if (ptcb != (OS_TCB *)0 && ptcb != (OS_TCB *)1) {              /* Make sure we have a valid TCB pointer  */
        View_TxStoINT32U((INT32U)ptcb->OSTCBStkBase);              /* Stack base address                     */
        View_TxStoINT8U(0x7C);
        View_TxStoINT32U(ptcb->OSTCBStkSize * sizeof(OS_STK));     /* Stack size (in #bytes)                 */
        View_TxStoINT8U(0x7C);
        View_TxStoINT32U(ptcb->OSTCBStkUsed);                      /* Number of bytes used                   */
        View_TxStoINT8U(0x7C);
        View_TxStoINT32U((INT32U)ptcb->OSTCBStkPtr);               /* Current stack pointer address          */
        View_TxStoINT8U(0x7C);
    } else {
        View_CmdGetTaskInfo_InvalidTask();
    }
}

/*$PAGE*/

/*
*********************************************************************************************************
*                                       uC/OS-View COMMAND HANDLER
*                                         (Get Task Stack Information)
*********************************************************************************************************
*/

static  void  View_CmdGetTaskInfo (void)
{


    ptcb   = (OS_TCB *)View_RxGetINT32U();                         /* Get the desired task's TCB address     */

    if (ptcb != (OS_TCB *)0 && ptcb != (OS_TCB *)1) {              /* Make sure we have a valid TCB pointer  */
        View_TxStoStr(ptcb->OSTCBTaskName); 			   /* Task name                              */
        View_TxStoINT8U(0x7C);
        View_TxStoINT8U(ptcb->OSTCBPrio);                          /* Prio                                   */
        View_TxStoINT8U(0x7C);
        stat = View_CmdGetTaskInfo_TCBStat(ptcb);                  /* Task status                            */
        View_TxStoINT8U(stat);
        View_TxStoINT8U(0x7C);
        View_TxStoINT32U((INT32U)ptcb->OSTCBEventPtr);             /* Pointer to event task is waiting for.  */
        View_TxStoINT8U(0x7C);
        View_TxStoINT32U((INT32U)ptcb->OSTCBDly);                  /* Timeout (i.e. ticks of delayed task)   */
        View_TxStoINT8U(0x7C);
        View_TxStoINT32U(ptcb->OSTCBCyclesTot);                    /* ExecTime                               */
        View_TxStoINT8U(0x7C);
        View_TxStoINT32U(ptcb->OSTCBCtxSwCtr);                     /* NumActivations                         */
        View_TxStoINT8U(0x7C);
    } else {
        View_CmdGetTaskInfo_InvalidTask();
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                       uC/OS-View COMMAND HANDLER
*                                             (Invalid Task)
*********************************************************************************************************
*/

static  void  View_CmdGetTaskInfo_InvalidTask (void)
{
    View_TxStoStr((INT8U *)"INVALID TASK!");	    	  /* Task name                                       */
    View_TxStoINT8U(0x7C);
    View_TxStoINT8U(0);                                   /* Prio                                            */
    View_TxStoINT8U(0x7C);
    View_TxStoINT8U(0);                                   /* Stat                                            */
    View_TxStoINT8U(0x7C); 
    View_TxStoINT32U(0);                                  /* Data                                            */
    View_TxStoINT8U(0x7C);
    View_TxStoINT32U(0);                                  /* Timeout                                         */
    View_TxStoINT8U(0x7C);
    View_TxStoINT32U(0);                                  /* ExecTime                                        */
    View_TxStoINT8U(0x7C);
    View_TxStoINT32U(0);                                  /* NumActivations                                  */
}

/*
*********************************************************************************************************
*                                           Obtain CPU name
*********************************************************************************************************
*/

void  View_GetCPUName (INT8U *s)
{
    INT8U  cpu_clk_freq;


    cpu_clk_freq = (INT8U)(BSP_CPU_ClkFreq() / 1000000L);
    strcpy((char *)s, "ControlHome 2.02 STM32 ( xx MHz )");
    s[25] = cpu_clk_freq / 10 + '0';
    s[26] = cpu_clk_freq % 10 + '0';
}

/*
*********************************************************************************************************
*                                       uC/OS-View COMMAND HANDLER
*                                         (Get ptcb->OSTCBStat)
*********************************************************************************************************
*/

static  INT8U  View_CmdGetTaskInfo_TCBStat (OS_TCB *ptcb)
{

    if (ptcb->OSTCBStat != OS_STAT_RDY) {
        stat = ptcb->OSTCBStat;
    } else {
        if (ptcb->OSTCBDly > 0) {                           /* Task status                                     */
            stat = 0xF0;
        } else {
            stat = ptcb->OSTCBStat;
        }
    }
    return (stat);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                       uC/OS-View COMMAND HANDLER
*                                              (Memory)
*********************************************************************************************************
*/

static  void  View_CmdMemRdINT8U (void)
{
    INT32U addr;
    INT8U  nbytes;


    addr   = View_RxGetINT32U();
    nbytes = View_RxGetINT8U();
    while (nbytes--) {
        View_TxStoINT8U(*(INT8U *)addr);
        View_TxStoINT8U(0x7C);
        addr++;
    }
}

static  void  View_CmdMemRdINT16U (void)
{
    INT32U addr;
    INT8U  nbytes;


    addr   = View_RxGetINT32U();
    nbytes = View_RxGetINT8U();
    while (nbytes--) {
        View_TxStoINT16U(*(INT16U *)addr);
        addr += 2;
    }
}


static  void  View_CmdMemRdINT32U (void)
{
    INT32U addr;
    INT8U  nbytes;


    addr   = View_RxGetINT32U();
    nbytes = View_RxGetINT8U();
    while (nbytes--) {
        View_TxStoINT32U(*(INT32U *)addr);
        addr += 4;
    }
}

/*
*********************************************************************************************************
*                                       uC/OS-View COMMAND HANDLER
*                                           (Write Functions)
*********************************************************************************************************
*/

static  void  View_CmdMemWrINT8U (void)
{
    INT8U *p;


    p    = (INT8U *)View_RxGetINT32U();
    (*p) = View_RxGetINT8U();
}


static  void  View_CmdMemWrINT16U (void)
{
    INT16U *p;


    p    = (INT16U *)View_RxGetINT32U();
    (*p) = View_RxGetINT16U();
}


static  void  View_CmdMemWrINT32U (void)
{

    INT32U *p;


    p    = (INT32U *)View_RxGetINT32U();
    (*p) = View_RxGetINT32U();

}

/*
*********************************************************************************************************
*                                              Rx Handler
*
* Description: This routine is called from the Rx interrupt service handler.
*********************************************************************************************************
*/

void  View_RxHandler (INT8U rx_data)
{
    View_RxCtr++;
    switch (View_RxState) {
        case VIEW_RX_STATE_SD0:                          /* waiting for start first  start delimiter (SD0)  */
             if (rx_data == VIEW_PROTOCOL_RX_SD0) {
                 View_RxState = VIEW_RX_STATE_SD1;
                 View_RxBufClr();
             }
             break;

        case VIEW_RX_STATE_SD1:                          /* waiting for start second start delimiter (SD1)  */
             if (rx_data == VIEW_PROTOCOL_RX_SD1) {
                 View_RxState = VIEW_RX_STATE_LEN;
             } else {
                 View_RxState = VIEW_RX_STATE_SD0;
             }
             break;

        case VIEW_RX_STATE_LEN:                          /* waiting for 'len' byte                          */
             if ((rx_data == 0x00) | (rx_data > VIEW_RX_BUF_SIZE)) {
                 View_RxState  = VIEW_RX_STATE_SD0;    /* Can not handle this size ...                    */
             } else {
                 View_RxChkSum    = rx_data;
                 View_RxRemainLen = rx_data;
                 View_RxState     = VIEW_RX_STATE_DATA;
             }
             break;

        case VIEW_RX_STATE_DATA:                         /* waiting for data                                */
             View_RxStoINT8U(rx_data);
             View_RxChkSum += rx_data;
             if (--View_RxRemainLen == 0) {
                 View_RxState = VIEW_RX_STATE_CHKSUM;
             }
             break;

        case VIEW_RX_STATE_CHKSUM:                       /* waiting for checksum                            */
             View_RxChkSum += rx_data;
             if ((View_RxChkSum & 0xFF) == 0x00) {
                 View_RxState = VIEW_RX_STATE_ED;
             } else {
                 View_RxState = VIEW_RX_STATE_SD0;
             }
             break;

        case VIEW_RX_STATE_ED:
             if (rx_data == VIEW_PROTOCOL_RX_ED) {       /* End delimiter ?                                 */
                 View_RxPktCtr++;
                 View_TxBufClr();
                 View_TxDefClr();
                 View_RxPkt();                            /* Parse received packet                           */
             }
             View_RxState = VIEW_RX_STATE_SD0;
             break;

        default:
             View_RxState = VIEW_RX_STATE_SD0;
             break;
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        PARSE RECEIVED PACKET
*
* Description: This function is called when a packet has been received and needs to be processed.
*********************************************************************************************************
*/

void  View_RxPkt (void)
{
    INT16U   len;


    len = View_CmdParsePkt();                    /* We have a whole packet, parse it!                */
    if (len > 0) {
        View_TxLen = len + 3;
        View_TxLenEnd = len;
        View_TxStart();
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        STORE DATA IN Rx BUFFER
*
* Description: This routine is called whenever a valid byte has been received.
*********************************************************************************************************
*/

static  void  View_RxStoINT8U (INT8U rx_data)
{
    if (View_RxBufCnt < VIEW_RX_BUF_SIZE) {
        View_RxBuf[View_RxBufCnt++] = rx_data;
    }
}


/*
*********************************************************************************************************
*                                           CLEAR Rx BUFFER
*********************************************************************************************************
*/

static  void  View_RxBufClr (void)
{
    View_RxBufCnt = 0;
    View_RxRdIx   = 0;
}

/*
*********************************************************************************************************
*                                           CLEAR Tx BUFFER
*********************************************************************************************************
*/

static  void  View_TxBufClr (void)
{
    View_TxBufCnt = 0;
    View_TxStrCnt = 0;
    View_TxCtr    = 0;
}
/*
*********************************************************************************************************
*                                     GET 8, 16 or 32-bit DATA
*********************************************************************************************************
*/

static  INT8U  View_RxGetINT8U (void)
{
    return (View_RxBuf[View_RxRdIx++]);
}


static  INT16U  View_RxGetINT16U (void)
{
    INT16U  lowbyte;
    INT16U  highbyte;


    highbyte = View_RxGetINT8U();
    lowbyte  = View_RxGetINT8U();
    return ((highbyte << 8) | lowbyte);
}


static  INT32U  View_RxGetINT32U (void)
{
    INT32U  highword;
    INT32U  lowword;


    highword = View_RxGetINT16U();
    lowword  = View_RxGetINT16U();
    return ((highword << 16) | lowword);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                 SEND AN ASCII STRING TO TERMINAL WINDOW
*
* Description: This function allows an application to send strings to the TERMINAL window in the viewer.
*
* Arguments  : s     is a pointer to the string to send.
*
*              dly   allows the calling task to delay itself for 'dly' ticks until the current string
*                    is sent.  If 'dly' is set to 0, then the string will not be sent if a string is
*                    currently in the process of being sent.  In other words, if there is a string currently
*                    being sent and you sent 'dly' to 0, View_TxStr() will return to the caller and the
*                    string will not be sent.
*
* Note(s)    : 1) This function MUST be called from a TASK and NOT an ISR.
*********************************************************************************************************
*/

void  View_TxStr (INT8U *s, INT16U dly)
{
    INT16U  len;


    len = (INT16U)OS_StrLen(s);                           /* Make sure string fits in buffer           */
    if (len >= VIEW_TX_STR_SIZE) {                        /* Force end of string if string too long    */
        len    = VIEW_TX_STR_SIZE - 1;
        s[len] = '\0';
    }
    if (View_TxStrLen > 0) {                              /* Are currently busy sending a string?      */
        if (dly > 0) {                                    /* Yes, does caller want to wait?            */
            while (View_TxStrLen > 0) {                   /* Yes, let other tasks run                  */
                View_TxStrDlyCtr++;
                OSTimeDly(dly);
            }
            View_TxStrLen = len;                          /* Update the string length                  */
            strcpy((char *)&View_TxStrBuf[0], (char *)s); /* Copy string to send to buffer             */
            if (View_TxStrLen > 0) {
                View_TxStart();
            }
        }
    } else {
        View_TxStrLen = len;                             /* Update the string length                  */
        strcpy((char *)&View_TxStrBuf[0], (char *)s);    /* Copy string to send to buffer             */
        if (View_TxStrLen > 0) {
            View_TxStart();
        }
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                  START TRANSMISSION OF REPLY PACKET
*********************************************************************************************************
*/

static  void  View_TxStart (void)
{
#if OS_CRITICAL_METHOD == 3
    OS_CPU_SR  cpu_sr = 0;
#endif


    OS_ENTER_CRITICAL();
    if (View_TxActiveFlag == OS_FALSE) {
      do{
        View_TxHandler_USB();
        View_TxLen--;
      }while(View_TxLen > 0);
      
      View_Tx_USB ();
      
    }
    OS_EXIT_CRITICAL();
}

/*
*********************************************************************************************************
*                                      Communication for uC/OS-View
*
* Description: Send 1 character to COM Port
*********************************************************************************************************
*/

void  View_Tx1 (INT8U c)
{
    usb_SendBuf((INT8U *)c, 1);
}

/*
*********************************************************************************************************
*                                      Communication for uC/OS-View
*
* Description: Send String to USB Port
*********************************************************************************************************
*/

static void  View_Tx_USB (void)
{
   usb_SendBuf((unsigned char*)View_TxStrBuf,strlen((char const*)View_TxStrBuf));
}

/*
*********************************************************************************************************
*                                      Communication for uC/OS-View
*
* Description: Send String to USB Port
*********************************************************************************************************
*/
static void  View_TxDefClr (void)
{
  INT8U i;
  
   View_TxState      = VIEW_TX_STATE_SD0;
   View_TxActiveFlag = OS_FALSE;
   for(i=0; i < VIEW_TX_BUF_SIZE; i++){
      View_TxBuf[i] = 0x00;
   }
}


/*
*********************************************************************************************************
*                                               Tx Handler
*
* Description: This routine is called from the transmitter buffer empty interrupt service handler.
*              It will send out the next byte in the buffer.
*
* Returns:     none
*********************************************************************************************************
*/

void  View_TxHandler_USB (void)
{
    INT8U  tx_data;
      
    tx_data = 0;
    
    switch (View_TxState) {
        case VIEW_TX_STATE_SD0:
             if (View_TxLen > 0) {                         /* Packet in buffer or string waiting to be send ? */
                 View_TxStoUSB(VIEW_PROTOCOL_TX_SD0);
                 View_TxCtr++;
                 View_TxActiveFlag = OS_TRUE;
                 View_TxState      = VIEW_TX_STATE_SD1;
                 View_TxIx         = 0;
             } else {                                       /* If there is nothing to do end transmission      */
                 View_TxActiveFlag = OS_FALSE;
                 break;
             }
             break;

        case VIEW_TX_STATE_SD1:
             View_TxStoUSB(VIEW_PROTOCOL_TX_SD1);
             View_TxCtr++;
             View_TxState = VIEW_TX_STATE_DATA_LEN;
             break;

        case VIEW_TX_STATE_DATA_LEN:                       /* Include the packet length in the packet         */
             View_TxStoUSB(View_TxLen);
             View_TxCtr++;
             View_TxState  = VIEW_TX_STATE_DATA;
             View_TxChkSum = View_TxLen;
             break;

        case VIEW_TX_STATE_DATA:
             tx_data = View_TxBuf[View_TxIx];           
             View_TxStoUSB(tx_data);
             View_TxCtr++;
             View_TxChkSum += tx_data;
             View_TxIx++;
             if (View_TxIx >= View_TxLenEnd) {               /* See if we are done sending the packet           */
                 View_TxState  = VIEW_TX_STATE_CHKSUM;
                 View_TxLen    = 3;
             }
             break;

        case VIEW_TX_STATE_CHKSUM:                         /* Epilog ... checksum & end delimiter             */
             View_TxStoUSB(View_TxChkSum);
             View_TxCtr++;
             View_TxState = VIEW_TX_STATE_ED;
             break;

        case VIEW_TX_STATE_ED:
             View_TxStoUSB(VIEW_PROTOCOL_TX_ED);
             View_TxCtr++;
             View_TxStoUSB(0x00);
             View_TxDefClr ();
             View_TxPktCtr++;
             break;

        default:
             View_TxDefClr ();
             break;
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                     STORE 8, 16 or 32-bit DATA
*********************************************************************************************************
*/
static  void  View_TxStoUSB (INT8U tx_data)
{
    if (View_TxCtr < VIEW_TX_BUF_SIZE) {
        View_TxStrBuf[View_TxCtr] = tx_data;
    }
}

void  View_TxStoINT8U (INT8U tx_data)
{
    if (View_TxBufCnt < VIEW_TX_BUF_SIZE) {
      if(tx_data != 0x00){
        View_TxBuf[View_TxBufCnt++] = tx_data;
      }else{
        View_TxBuf[View_TxBufCnt++] = (INT8U)'!';
        View_TxBuf[View_TxBufCnt++] = (INT8U)'0';
      }
    }
}

static  void  View_TxStoINT16U (INT16U tx_data)
{
    View_TxStoINT8U(tx_data >> 8);
    View_TxStoINT8U(tx_data & 0x00FF);
}


static  void  View_TxStoINT32U (INT32U tx_data)
{
    View_TxStoINT16U(tx_data >> 16);
    View_TxStoINT16U(tx_data & 0x0000FFFFL);
}


static  void  View_TxStoPtr (void *ptr)
{
    INT32U  tx_data;


    tx_data = (INT32U)ptr;
    View_TxStoINT16U(tx_data >> 16);
    View_TxStoINT16U(tx_data & 0x0000FFFFL);
}


void  View_TxStoStr (INT8U *s)
{
    INT8U   len;
    INT8U   i;
    INT8U  *ps;


    len = 0;
    ps  = s;
    if (ps != (INT8U *)0) {                                 /* Calculate length of string                      */
        while (*ps) {
            ps++;
            len++;
        }
    }
    View_TxStoINT8U(len);                                 /* Store string, length first                      */
    ps = s;
    for (i = 0; i < len; i++) {
        View_TxStoINT8U((INT8U)*ps);
        ps++;
    }
}

#endif