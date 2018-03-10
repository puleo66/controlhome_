/*
***********************************************************************************************
*                                           FHT8X Interface
*
*                            
*
* Filename   : fht8x.h
* Version    : V1.
* Programmer : Puleo
***********************************************************************************************
*/


#ifdef   FHT8X_GLOBALS
#define  FHT8X_EXT
#else
#define  FHT8X_EXT  extern
#endif

#ifndef  FHT8X_H
#define  FHT8X_H
/*
***********************************************************************************************
*                                         VARIABLES
***********************************************************************************************
*/
FHT8X_EXT  INT8U  fhtCount;             // Anzahl fhts (ergibt sich aus den gespeicherten Daten)

FHT8X_EXT struct {
  INT8U hc1;
  INT8U hc2;
  INT8U status;
}FHTDev[maxDevices];

FHT8X_EXT INT8U FHTDevAct;
/*
***********************************************************************************************
*                                         CONSTANS
***********************************************************************************************
*/



/*
*********************************************************************************************************
*                                      FUNCTION PROTOTYPES
*********************************************************************************************************
*/
void setFhtTimes(void);
void setFhtIni(INT16U devadr);

INT8S AddDevice(INT16U devadr, INT8S dtype, INT8U *name);
INT8S DelDevice(INT8U nr, INT8S type);
#endif
