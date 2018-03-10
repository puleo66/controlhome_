/*
*********************************************************************************************************
*                                        FILE RUTIN MODULE SOURCE FILE
*
* Filename      : file_rutin.h
* Version       : V0.1
* Programmer(s) : Puleo
*********************************************************************************************************
*/
#ifdef   FILE_RUTIN_GLOBALS
#define  FILE_RUTIN
#else
#define  FILE_RUTIN  extern
#endif


#ifndef  FILE_RUTIN_H__
#define  FILE_RUTIN_H__
/*
*********************************************************************************************************
*                                               CONSTANTS
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            LOCAL VARIABLES
*********************************************************************************************************
*/

FILE_RUTIN          FS_DIR    *pDir;
FILE_RUTIN          FS_FILE   *pFile;
FILE_RUTIN   struct FS_DIRENT *pDirEnt;
   
FILE_RUTIN   INT8U  FileBuffer[512];


/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/
INT8U         Write_File(INT8U *name, INT8U *txt);
INT8U         Read_File(INT8U *name);
INT8U         Delete_File(INT8U *name);
INT8U         Rename_File(INT8U *oldname, INT8U *newname);
INT8U         Log_File(INT8U *txt);

void          Create_Directory(INT8U *name);
void          Read_Directory(INT8U *name);
void          Del_Directory(INT8U *name);

void          Show_Free(INT8U *device);
/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*                                               OS SPECIFIC
*********************************************************************************************************
*/

#endif                                                          /* End of module include.                               */
