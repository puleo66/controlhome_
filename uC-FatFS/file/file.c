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

#include "includes.h"

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
*                   This routine demonstrates, how to create and write to a file
*                   using the file system.
*********************************************************************************************************
*/

void Write_File(INT8U *name, INT8U *txt) {
  INT8u x;
  
  /* create file */
  myfile = FS_FOpen(name,"w");
  if (myfile) {
    /* write to file */
    x = FS_FWrite(txt,1,strlen(txt),myfile);
    /* all data written ? */
    if (x!=(int)strlen(txt)) {
      /* check, why not all data was written */
      x = FS_FError(myfile);
      sprintf(mybuffer,"Not all bytes written because of error %d.\n",x);
      _error(mybuffer);
    }
    /* close file */
    FS_FClose(myfile);
  }
  else {
    sprintf(mybuffer,"Unable to create file %s\n",name);
    _error(mybuffer);
  }
}


/*
*********************************************************************************************************
*
*                                        Dump_File
*
*                    This routine demonstrates, how to open and read from a file using 
*                    the file system.
*********************************************************************************************************
*/

void Dump_File(INT8U *name) {
  INT8U x;

  /* open file */
  myfile = FS_FOpen(name,"r");
  if (myfile) {
    /* read until EOF has been reached */
    do {
      x = FS_FRead(mybuffer,1,sizeof(mybuffer)-1,myfile);
      mybuffer[x]=0;
      if (x) {
        _log(mybuffer);
      }
    } while (x);
    /* check, if there is no more data, because of EOF */
    x = FS_FError(myfile);
    if (x!=FS_ERR_EOF) {
      /* there was a problem during read operation */
      sprintf(mybuffer,"Error %d during read operation.\n",x);
      _error(mybuffer);
    }
    /* close file */
    FS_FClose(myfile);
  }
  else {
    sprintf(mybuffer,"Unable to open file %s.\n",name);
    _error(mybuffer);
  }
}


/*
*********************************************************************************************************
*
*                                           Show_Directory
*
*                       This routine demonstrates, how to read a directory.
*********************************************************************************************************
*/

#if FS_POSIX_DIR_SUPPORT

void Show_Directory(INT8U *name) {
  FS_DIR *dirp;
  struct FS_DIRENT *direntp;

  _log("Directory of ");
  _log(name);
  _log("\n");
  dirp = FS_OpenDir(name);
  if (dirp) {
    do {
      direntp = FS_ReadDir(dirp);
      if (direntp) {
        sprintf(mybuffer,"%s\n",direntp->d_name);
        _log(mybuffer);
      }
    } while (direntp);
    FS_CloseDir(dirp);
  }
  else {
    _error("Unable to open directory\n");
  }
}
#endif /* FS_POSIX_DIR_SUPPORT */


/*
*********************************************************************************************************
*
*                                             Show_Free
*
*                     This routine demonstrates, how to read disk space information.
*********************************************************************************************************
*/

void Show_Free(INT8U *device) {
  FS_DISKFREE_T disk_data;
  INT8U x;

  x = FS_IoCtl(device,FS_CMD_GET_DISKFREE,0,(void*) &disk_data);
  if (x==0) {
    sprintf(FileBuffer,"total clusters     : %lu\navailable clusters : %lu\nsectors/cluster    : %u\nbytes per sector   : %u\n",
          disk_data.total_clusters, disk_data.avail_clusters, disk_data.sectors_per_cluster, disk_data.bytes_per_sector);
//    _log(mybuffer);
  } 
  else {
//    _error("Invalid drive specified\n");
  }
}

