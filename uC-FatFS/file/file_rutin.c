/*
*********************************************************************************************************
*                                        FILE MODULE SOURCE FILE
*
* Filename      : file.c
* Version       : V0.1
* Programmer(s) : Puleo
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                              INCLUDE FILES
*********************************************************************************************************
*/
#define  FILE_RUTIN_GLOBALS
#include <includes.h>

#if FS_MODULE == DEF_ENABLED

/*
*********************************************************************************************************
*                                            LOCAL CONSTANTS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            LOCAL VARIABLES
*********************************************************************************************************
*/

 
 INT8U name[30];
 INT8U name1[30];
 INT8U Fstr[20];
 
 INT8U logCount = 1;
extern INT8U time_buffer[20];
/*
*********************************************************************************************************
*                                        LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/




/*
*********************************************************************************************************
*
*                                        write_file
*
*                      Create and write to a file using the file system.
*********************************************************************************************************
*/

INT8U Write_File(INT8U *name, INT8U *txt) 
{
  INT8U x;
  
  /* create file */
  pFile = FS_FOpen((char const*)name,"w");
  if (pFile) {
    /* write to file */
    x = FS_FWrite(txt,1,strlen((char const*)txt),pFile);
    /* all data written ? */
    if (x!=(int)strlen((char const*)txt)) {
      /* check, why not all data was written */
      x = FS_FError(pFile);
      sprintf((char*)FileBuffer,"Not all bytes written because of error %d.\n",x);
      FS_ClearErr (pFile);
//      _error(mybuffer);
      return -1;
    }
    /* close file */
    FS_FClose(pFile);
    return 0;
  }
  else {
    sprintf((char*)FileBuffer,"Unable to create file %s\n",name);
//   _error(mybuffer);
    return -1;
  }
}


/*
*********************************************************************************************************
*
*                                        Dump_File
*
*                          Open and read from a file using the file system.
*********************************************************************************************************
*/

INT8U Read_File(INT8U *name)
{
  int x;

  /* open file */
  pFile = FS_FOpen((char const*)name,"r");
  if (pFile) {
    /* read until EOF has been reached */
    do {
      x = FS_FRead(FileBuffer,1,sizeof(FileBuffer)-1,pFile);
      FileBuffer[x]=0;
      if(x < 512){
        x = 0;
      }
    } while (x);
    /* check, if there is no more data, because of EOF */
    x = FS_FError(pFile);
    if (x!=FS_ERR_EOF) {
      /* there was a problem during read operation */
      sprintf((char*)FileBuffer,"Error %d during read operation.\n",x);
      FS_ClearErr (pFile);
//      _error(mybuffer);
      return -1;
    }
    /* close file */
    FS_FClose(pFile);
    return 0;
  }
  else {
    sprintf((char*)FileBuffer,"Unable to open file %s.\n",name);
//    _error(mybuffer);
    return -1;
  }
}

/*
*********************************************************************************************************
*
*                                        Delete_File
*
*                          Open and delete from a file using the file system.
*********************************************************************************************************
*/

INT8U Delete_File(INT8U *name)
{
  /* open file */
  pFile = FS_FOpen((char const*)name,"r");
  if (pFile) {
    /* close file */
    FS_FClose(pFile);
    if (FS_Remove((char const*)name)!= 0){
      sprintf((char*)FileBuffer,"Unable to delete file %s.\n",name);
      return -1;
    }
    return 0;
  }
  else {
    sprintf((char*)FileBuffer,"Unable to open file %s.\n",name);
    return -1;
  }
}

/*
*********************************************************************************************************
*
*                                        Rename_File
*
*                          Read, delete and write from a file using the file system.
*********************************************************************************************************
*/

INT8U Rename_File(INT8U *oldname, INT8U *newname)
{
  /* read file */
  if (Read_File(oldname)== 0){
    /* delete file */
    if (Delete_File(oldname) == 0){
      /* write file */
      if(Write_File(newname, FileBuffer) != 0){
        sprintf((char*)FileBuffer,"Unable to rename file %s\n",newname);
        return -1;
      }
      return 0;
    }else{
      sprintf((char*)FileBuffer,"Unable to delete file %s.\n",oldname);
      return -1;
    }
  }else {
    sprintf((char*)FileBuffer,"Unable to read file %s.\n",oldname);
    return -1;
  }
}

/*
*********************************************************************************************************
*
*                                        Log_File
*
*                            Create,update log file or error file
*********************************************************************************************************
*/

INT8U Log_File(INT8U *txt)
{ 
  INT8U i, j;  
  INT8U patch[] = "mmc:0:\\log\\";
  
  FileBuffer[0] = '\0';
  Get_Time_Now();
  //Set Date string
  j = strlen((char const*)patch);
  sprintf((char*)name, "%s",patch);
  
  name[0 + j] = time_buffer[2];
  name[1 + j] = time_buffer[3];
  name[2 + j] = time_buffer[5];
  name[3 + j] = time_buffer[6];
  name[4 + j] = time_buffer[8];
  name[5 + j] = time_buffer[9];
  name[6 + j] = '_';              
  name[7 + j] = '0';
  name[8 + j] = '.';
  strcat((char*)name,(char const*)"log");
  
  //Set Time string
  Fstr[0] = time_buffer[11];
  Fstr[1] = time_buffer[12];
  Fstr[2] = ':';
  Fstr[3] = time_buffer[14];
  Fstr[4] = time_buffer[15];
  Fstr[5] = ':';
  Fstr[6] = time_buffer[17];
  Fstr[7] = time_buffer[18];
  Fstr[8] = ' ';
  
  strcpy((char*)FileBuffer,(char const*)Fstr);
  strcat((char*)FileBuffer,(char const*)txt);
  strcat((char*)FileBuffer,(char const*)"\r\n");
  
  i = strlen((char const*)FileBuffer);
  
  /* read file */
  if (Read_File(name)!= 0){
    if (strlen((char const*)FileBuffer) + i > 512){
      for(i=logCount; i > 0; i--){
        sprintf((char*)name[7 + j],"%d",logCount);    // xxx_0.log -> xxx_1.log
        strcpy((char*)name1,(char const*)name);
        sprintf((char*)name[7 + j],"%d",logCount-1);
        
        
        Rename_File(name, name1);
      }
      logCount++;
    }
  }else{
//    FileBuffer[0] = '\0';
  }
  
  strcat((char*)FileBuffer,(char const*)Fstr);   // Date string
  strcat((char*)FileBuffer,(char const*)txt);    // Message string
  strcat((char*)FileBuffer,(char const*)"\r\n");
  /* write file */
  
  if(Write_File(name, FileBuffer) != 0){
    sprintf((char*)FileBuffer,"Unable to write file %s\n",name);
    return -1;
  }
  return 0;
}

/*
*********************************************************************************************************
*
*                                           Create_Directory
*
*                                          Create a directory.
*********************************************************************************************************
*/

#if FS_POSIX_DIR_SUPPORT

void Create_Directory(INT8U *name) 
{
  pDir = FS_OpenDir((char const*)name);
  if (!pDir) {
    if (FS_MkDir((char const*)name)!= 0){
      sprintf((char*)FileBuffer,"Unable to create directory %s.\n",name);
    }
  }else {
    sprintf((char*)FileBuffer,"Unable to open directory %s.\n",name);
  }
}
#endif /* FS_POSIX_DIR_SUPPORT */

/*
*********************************************************************************************************
*
*                                           Del_Directory
*
*                                          Delete a directory.
*********************************************************************************************************
*/

#if FS_POSIX_DIR_SUPPORT

void Del_Directory(INT8U *name) 
{
  pDir = FS_OpenDir((char const*)name);
  if (pDir) {
    FS_CloseDir(pDir);
    if (FS_RmDir((char const*)name)!= 0){
      sprintf((char*)FileBuffer,"Unable to delete directory %s.\n",name);
    }
  }
  else {
    sprintf((char*)FileBuffer,"Unable to open directory %s.\n",name);
  }
}
#endif /* FS_POSIX_DIR_SUPPORT */

/*
*********************************************************************************************************
*
*                                           Read_Directory
*
*                                          Read a directory.
*********************************************************************************************************
*/

#if FS_POSIX_DIR_SUPPORT

void Read_Directory(INT8U *name) 
{  
  FileBuffer[0] = '\0';
  pDir = FS_OpenDir((char const*)name);
  if (pDir) {
    do {
      pDirEnt = FS_ReadDir(pDir);
      if (pDirEnt) {
        
        sprintf((char*)Fstr,"%s\n",pDirEnt->d_name);
        FileBuffer[strlen((char const *)FileBuffer) +1] = '\0';
        strcat((char*)FileBuffer,(char const *)Fstr);
        Fstr[0] = '\0';
      }
    } while (pDirEnt);
    FS_CloseDir(pDir);
    printf((char*)FileBuffer);
  }
  else {
//    _error("Unable to open directory\n");
  }
}
#endif /* FS_POSIX_DIR_SUPPORT */


/*
*********************************************************************************************************
*
*                                             Show_Free
*
*                                     Read disk space information.
*********************************************************************************************************
*/

void Show_Free(INT8U *device) 
{
  FS_DISKFREE_T disk_data;
  INT8U x;

  x = FS_IoCtl((char const*)device,FS_CMD_GET_DISKFREE,0,(void*) &disk_data);
  if (x==0) {
    sprintf((char*)FileBuffer,"total clusters     : %lu\navailable clusters : %lu\nsectors/cluster    : %u\nbytes per sector   : %u\n",
          disk_data.total_clusters, disk_data.avail_clusters, disk_data.sectors_per_cluster, disk_data.bytes_per_sector);
//    _log(mybuffer);
  } 
  else {
//    _error("Invalid drive specified\n");
  }
}

#endif
