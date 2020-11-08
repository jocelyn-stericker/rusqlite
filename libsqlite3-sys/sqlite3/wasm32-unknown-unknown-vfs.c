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
typedef struct NoopFile NoopFile;

/* An open file */
struct NoopFile {
  sqlite3_file base;              /* IO methods */
};

/*
** Methods for NoopFile
*/
static int noopClose(sqlite3_file*);
static int noopRead(sqlite3_file*, void*, int iAmt, sqlite3_int64 iOfst);
static int noopWrite(sqlite3_file*,const void*,int iAmt, sqlite3_int64 iOfst);
static int noopTruncate(sqlite3_file*, sqlite3_int64 size);
static int noopSync(sqlite3_file*, int flags);
static int noopFileSize(sqlite3_file*, sqlite3_int64 *pSize);
static int noopLock(sqlite3_file*, int);
static int noopUnlock(sqlite3_file*, int);
static int noopCheckReservedLock(sqlite3_file*, int *pResOut);
static int noopFileControl(sqlite3_file*, int op, void *pArg);
static int noopSectorSize(sqlite3_file*);
static int noopDeviceCharacteristics(sqlite3_file*);
static int noopShmMap(sqlite3_file*, int iPg, int pgsz, int, void volatile**);
static int noopShmLock(sqlite3_file*, int offset, int n, int flags);
static void noopShmBarrier(sqlite3_file*);
static int noopShmUnmap(sqlite3_file*, int deleteFlag);
static int noopFetch(sqlite3_file*, sqlite3_int64 iOfst, int iAmt, void **pp);
static int noopUnfetch(sqlite3_file*, sqlite3_int64 iOfst, void *p);

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
static int noopCurrentTimeInt64(sqlite3_vfs*, sqlite3_int64*);

static sqlite3_vfs noop_vfs = {
  2,                           /* iVersion */
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
  noopCurrentTimeInt64          /* xCurrentTimeInt64 */
};

static const sqlite3_io_methods noop_io_methods = {
  3,                              /* iVersion */
  noopClose,                      /* xClose */
  noopRead,                       /* xRead */
  noopWrite,                      /* xWrite */
  noopTruncate,                   /* xTruncate */
  noopSync,                       /* xSync */
  noopFileSize,                   /* xFileSize */
  noopLock,                       /* xLock */
  noopUnlock,                     /* xUnlock */
  noopCheckReservedLock,          /* xCheckReservedLock */
  noopFileControl,                /* xFileControl */
  noopSectorSize,                 /* xSectorSize */
  noopDeviceCharacteristics,      /* xDeviceCharacteristics */
  noopShmMap,                     /* xShmMap */
  noopShmLock,                    /* xShmLock */
  noopShmBarrier,                 /* xShmBarrier */
  noopShmUnmap,                   /* xShmUnmap */
  noopFetch,                      /* xFetch */
  noopUnfetch                     /* xUnfetch */
};



static int noopClose(sqlite3_file *pFile){
  return SQLITE_OK;
}

static int noopRead(
  sqlite3_file *pFile,
  void *zBuf,
  int iAmt,
  sqlite_int64 iOfst
){
  return SQLITE_IOERR_READ;
}

static int noopWrite(
  sqlite3_file *pFile,
  const void *z,
  int iAmt,
  sqlite_int64 iOfst
){
  return SQLITE_IOERR_WRITE;
}

static int noopTruncate(sqlite3_file *pFile, sqlite_int64 size){
  return SQLITE_IOERR_TRUNCATE;
}

static int noopSync(sqlite3_file *pFile, int flags){
  return SQLITE_OK;
}

static int noopFileSize(sqlite3_file *pFile, sqlite_int64 *pSize){
  return SQLITE_IOERR_FSTAT;
}

static int noopLock(sqlite3_file *pFile, int eLock){
  return SQLITE_OK;
}

static int noopUnlock(sqlite3_file *pFile, int eLock){
  return SQLITE_OK;
}

static int noopCheckReservedLock(sqlite3_file *pFile, int *pResOut){
  *pResOut = 0;
  return SQLITE_OK;
}

static int noopFileControl(sqlite3_file *pFile, int op, void *pArg){
  return SQLITE_NOTFOUND;
}

static int noopSectorSize(sqlite3_file *pFile){
  return 0;
}

static int noopDeviceCharacteristics(sqlite3_file *pFile){
  return 0;
}

static int noopShmMap(
  sqlite3_file *pFile,
  int iPg,
  int pgsz,
  int bExtend,
  void volatile **pp
){
  return SQLITE_IOERR_SHMMAP;
}

static int noopShmLock(sqlite3_file *pFile, int offset, int n, int flags){
  return SQLITE_IOERR_SHMLOCK;
}

static void noopShmBarrier(sqlite3_file *pFile){
  return;
}

static int noopShmUnmap(sqlite3_file *pFile, int deleteFlag){
  return SQLITE_OK;
}

static int noopFetch(
  sqlite3_file *pFile,
  sqlite3_int64 iOfst,
  int iAmt,
  void **pp
){
  *pp = 0;
  return SQLITE_OK;
}

static int noopUnfetch(sqlite3_file *pFile, sqlite3_int64 iOfst, void *pPage){
  return SQLITE_OK;
}

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
  // TODO: implement?
  return 0;
}

static int noopCurrentTime(sqlite3_vfs *pVfs, double *pTimeOut){
  // TODO: implement?
  return 0;
}

static int noopGetLastError(sqlite3_vfs *pVfs, int a, char *b){
  return 0;
}
static int noopCurrentTimeInt64(sqlite3_vfs *pVfs, sqlite3_int64 *p){
  // TODO: implement?
  return 0;
}

int sqlite3_os_init()
{
    sqlite3_vfs_register(&noop_vfs, 0);
    return 0;
}

int sqlite3_os_end()
{
    return 0;
}
