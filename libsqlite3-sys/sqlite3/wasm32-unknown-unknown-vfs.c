/*
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
*/
#include "sqlite3.h"
#include <string.h>
#include <assert.h>


/*
** Forward declaration of objects used by this utility
*/
typedef struct sqlite3_vfs NoopVfs;

/*
** Methods for NoopVfs
*/
static int noopOpen(sqlite3_vfs*, const char *, sqlite3_file*, int , int *);
static int noopDelete(sqlite3_vfs*, const char *zName, int syncDir);
static int noopAccess(sqlite3_vfs*, const char *zName, int flags, int *);
static int noopFullPathname(sqlite3_vfs*, const char *zName, int, char *zOut);
static void *noopDlOpen(sqlite3_vfs*, const char *zFilename);
static void noopDlError(sqlite3_vfs*, int nByte, char *zErrMsg);
static void (*noopDlSym(sqlite3_vfs *pVfs, void *p, const char*zSym))(void);
static void noopDlClose(sqlite3_vfs*, void*);
static int noopRandomness(sqlite3_vfs*, int nByte, char *zOut);
static int noopSleep(sqlite3_vfs*, int microseconds);
static int noopCurrentTime(sqlite3_vfs*, double*);
static int noopGetLastError(sqlite3_vfs*, int, char *);

static sqlite3_vfs noop_vfs = {
  1,                           /* iVersion */
  0,                           /* szOsFile (set when registered) */
  1024,                        /* mxPathname */
  0,                           /* pNext */
  "noopvfs",                    /* zName */
  0,                           /* pAppData (set when registered) */
  noopOpen,                     /* xOpen */
  noopDelete,                   /* xDelete */
  noopAccess,                   /* xAccess */
  noopFullPathname,             /* xFullPathname */
  noopDlOpen,                   /* xDlOpen */
  noopDlError,                  /* xDlError */
  noopDlSym,                    /* xDlSym */
  noopDlClose,                  /* xDlClose */
  noopRandomness,               /* xRandomness */
  noopSleep,                    /* xSleep */
  noopCurrentTime,              /* xCurrentTime */
  noopGetLastError,             /* xGetLastError */
  0                             /* xCurrentTimeInt64 */
};

static int noopOpen(
  sqlite3_vfs *pVfs,
  const char *zName,
  sqlite3_file *pFile,
  int flags,
  int *pOutFlags
){
  return SQLITE_CANTOPEN;
}

static int noopDelete(sqlite3_vfs *pVfs, const char *zPath, int dirSync){
  return SQLITE_IOERR_DELETE;
}

static int noopAccess(
  sqlite3_vfs *pVfs,
  const char *zPath,
  int flags,
  int *pResOut
){
  *pResOut = 0;
  return SQLITE_OK;
}

static int noopFullPathname(
  sqlite3_vfs *pVfs,
  const char *zPath,
  int nOut,
  char *zOut
){
  sqlite3_snprintf(nOut, zOut, "%s", zPath);
  return SQLITE_OK;
}

static void *noopDlOpen(sqlite3_vfs *pVfs, const char *zPath){
  return 0;
}

static void noopDlError(sqlite3_vfs *pVfs, int nByte, char *zErrMsg){
  sqlite3_snprintf(nByte, zErrMsg, "Loadable extensions are not supported");
  zErrMsg[nByte-1] = '\0';
}

static void (*noopDlSym(sqlite3_vfs *pVfs, void *p, const char *zSym))(void){
  return 0;
}

static void noopDlClose(sqlite3_vfs *pVfs, void *pHandle){
  return;
}

static int noopRandomness(sqlite3_vfs *pVfs, int nByte, char *zBufOut){
  // TODO: implement?
  return SQLITE_OK;
}

static int noopSleep(sqlite3_vfs *pVfs, int nMicro){
  return nMicro;
}

static int noopCurrentTime(sqlite3_vfs *pVfs, double *pTimeOut){
  return 0;
}

static int noopGetLastError(sqlite3_vfs *pVfs, int a, char *b){
  return 0;
}

int sqlite3_os_init()
{
    noop_vfs.szOsFile = sizeof(sqlite3_file);
    sqlite3_vfs_register(&noop_vfs, 1);
    return 0;
}

int sqlite3_os_end()
{
    return 0;
}
