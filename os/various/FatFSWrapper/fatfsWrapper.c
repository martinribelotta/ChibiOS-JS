/**
 * @file fatfsWrapper.c
 * @author Matthias Blaicher
 *
 */

#include "ff.h"
#include "ffconf.h"

#include "fatfsWrapperConfig.h"

/*
 * Defines for which functions need to be included in the wrapper.
 */
#define HAS_MOUNT       (FATFS_WRP_ENABLE_MOUNT)
#define HAS_OPEN        (FATFS_WRP_ENABLE_OPEN)
#define HAS_READ        (FATFS_WRP_ENABLE_READ)
#define HAS_CLOSE       (FATFS_WRP_ENABLE_CLOSE)

#define HAS_WRITE       (FATFS_WRP_ENABLE_WRITE    && !_FS_READONLY)
#define HAS_SYNC        (FATFS_WRP_ENABLE_SYNC     && !_FS_READONLY)

#define HAS_CHDRIVE     (FATFS_WRP_ENABLE_CHDRIVE  && (_FS_RPATH >= 1))
#define HAS_CHDIR       (FATFS_WRP_ENABLE_CHDIR    && (_FS_RPATH >= 1))
#define HAS_GETCWD      (FATFS_WRP_ENABLE_GETCWD   && (_FS_RPATH >= 2))


#define HAS_LSEEK       (FATFS_WRP_ENABLE_LSEEK    && (_FS_MINIMIZE <= 2))
#define HAS_OPENDIR     (FATFS_WRP_ENABLE_OPENDIR  && (_FS_MINIMIZE <= 1))
#define HAS_READDIR     (FATFS_WRP_ENABLE_READDIR  && (_FS_MINIMIZE <= 1))
#define HAS_STAT        (FATFS_WRP_ENABLE_STAT     && (_FS_MINIMIZE == 0))
#define HAS_GETFREE     (FATFS_WRP_ENABLE_GETFREE  && (_FS_MINIMIZE == 0) && !_FS_READONLY)
#define HAS_TRUNCATE    (FATFS_WRP_ENABLE_TRUNKATE && (_FS_MINIMIZE == 0) && !_FS_READONLY)
#define HAS_UNLINK      (FATFS_WRP_ENABLE_UNLINK   && (_FS_MINIMIZE == 0) && !_FS_READONLY)
#define HAS_MKDIR       (FATFS_WRP_ENABLE_MKDIR    && (_FS_MINIMIZE == 0) && !_FS_READONLY)
#define HAS_CHMOD       (FATFS_WRP_ENABLE_CHMOD    && (_FS_MINIMIZE == 0) && !_FS_READONLY)
#define HAS_UTIME       (FATFS_WRP_ENABLE_UTIME    && (_FS_MINIMIZE == 0) && !_FS_READONLY)
#define HAS_RENAME      (FATFS_WRP_ENABLE_RENAME   && (_FS_MINIMIZE == 0) && !_FS_READONLY)


#define HAS_FORWARD     (FATFS_WRP_ENABLE_FORWARD  && _USE_FORWARD && _FS_TINY)
#define HAS_MKFS        (FATFS_WRP_ENABLE_MKFS     && _USE_MKFS && !_FS_READONLY)
#define HAS_FDISK       (FATFS_WRP_ENABLE_FDISK    && _USE_MKFS && !_FS_READONLY && (_MULTI_PARTITION == 2))

#define HAS_GETS        (FATFS_WRP_ENABLE_GETS     && _USE_STRFUNC)
#define HAS_PUTC        (FATFS_WRP_ENABLE_PUTC     && _USE_STRFUNC && !_FS_READONLY)
#define HAS_PUTS        (FATFS_WRP_ENABLE_PUTS     && _USE_STRFUNC && !_FS_READONLY)
#define HAS_PRINTF      (FATFS_WRP_ENABLE_PRINTF   && _USE_STRFUNC && !_FS_READONLY)

/**
 * @brief Enumeration of all possible actions depending on activated functions.
 */
enum wrapper_action {
#if HAS_MOUNT
    eFMOUNT,
#endif
#if HAS_OPEN
    eFOPEN,
#endif
#if HAS_READ
    eFREAD,
#endif
#if HAS_WRITE
    eFWRITE,
#endif
#if HAS_SYNC
    eFSYNC,
#endif
#if HAS_CHDRIVE
    eFCHDRIVE,
#endif
#if HAS_CHDIR
    eFCHDIR,
#endif
#if HAS_GETCWD
    eFGETCWD,
#endif
#if HAS_LSEEK
    eFLSEEK,
#endif
#if HAS_CLOSE
    eFCLOSE,
#endif
#if HAS_OPENDIR
    eFOPENDIR,
#endif
#if HAS_READDIR
    eFREADDIR,
#endif
#if HAS_STAT
    eFSTAT,
#endif
#if HAS_GETFREE
    eFGETFREE,
#endif
#if HAS_TRUNCATE
    eFTRUNCATE,
#endif
#if HAS_UNLINK
    eFUNLINK,
#endif
#if HAS_MKDIR
    eFMKDIR,
#endif
#if HAS_CHMOD
    eFCHMOD,
#endif
#if HAS_UTIME
    eFUTIME,
#endif
#if HAS_RENAME
    eFRENAME,
#endif
#if HAS_MKFS
    eFMKFS,
#endif
#if HAS_FDISK
    eFFDISK,
#endif
#if HAS_GETS
    eFGETS,
#endif
#if HAS_PUTC
    eFPUTC,
#endif
#if HAS_PUTS
    eFPUTS,
#endif
};


#define _wrapper_msg_base                     \
    enum wrapper_action action;               \
    FRESULT result;

struct wrapper_msg_base {
  _wrapper_msg_base
};


struct wrapper_msg_vBYTEpFATFS {
  _wrapper_msg_base

  BYTE byte;
  FATFS* fatfsp;
};

struct wrapper_msg_pFILpTCHARvBYTE {
  _wrapper_msg_base

  FIL* filep;
  TCHAR* string;
  BYTE byte;
};

struct wrapper_msg_pFILpVOIDvUINTpUINT {
  _wrapper_msg_base

  FIL* filep;
  void* voidp;
  UINT uint;
  UINT* uintp;
};

struct wrapper_msg_pFILvDWORD {
  _wrapper_msg_base

  FIL* filep;
  DWORD dword;
};

struct wrapper_msg_pFIL {
  _wrapper_msg_base

  FIL* filep;
};

struct wrapper_msg_pDIRpTCHAR {
  _wrapper_msg_base

  DIR* dirp;
  TCHAR* string;
};

struct wrapper_msg_pDIRpFILINFO {
  _wrapper_msg_base

  DIR* dirp;
  FILINFO* filinfop;
};


struct wrapper_msg_pTCHARpFILINFO {
  _wrapper_msg_base

  TCHAR* string;
  FILINFO* filinfop;
};


struct wrapper_msg_pTCHARpDWORDppFATFS {
  _wrapper_msg_base

  TCHAR* string;
  DWORD* dwordp;
  FATFS** fatfspp;
};

struct wrapper_msg_pTCHAR {
  _wrapper_msg_base

  TCHAR* string;
};


struct wrapper_msg_pTCHARvBYTEvBYTE {
  _wrapper_msg_base

  TCHAR* string;
  BYTE byte1;
  BYTE byte2;
};

struct wrapper_msg_pTCHARpTCHAR {
  _wrapper_msg_base

  TCHAR* string1;
  TCHAR* string2;
};

struct wrapper_msg_vBYTE {
  _wrapper_msg_base

  BYTE byte;
};

struct wrapper_msg_pTCHARvUINT {
  _wrapper_msg_base

  TCHAR* string;
  UINT uint;
};

struct wrapper_msg_vBYTEvBYTEvUINT {
  _wrapper_msg_base

  BYTE byte1;
  BYTE byte2;
  UINT uint;
};

struct wrapper_msg_vBYTEpDWORDpVOID {
  _wrapper_msg_base

  BYTE byte;
  DWORD* dwordp;
  void* voidp;
};

struct wrapper_msg_vTCHARpFIL {
  _wrapper_msg_base

  TCHAR tchar;
  FIL* filep;
};

struct wrapper_msg_pTCHARpFIL {
  _wrapper_msg_base

  TCHAR* string;
  FIL* filep;
};

struct wrapper_msg_pTCHARvINTpFILpTCHAR {
  _wrapper_msg_base

  TCHAR* string;
  int n;
  FIL* filep;

  TCHAR* string2;
};


/**
 * @brief Pointer to worker thread for synchronous communication.
 */
static Thread* workerThread = NULL;

static WORKING_AREA(waFatFSWorkerThread, FATFS_WRP_WORKER_SIZE);

static msg_t ThreadFatFSWorker(void *arg) {
  (void)arg;

  Thread* p;

  chRegSetThreadName("fatfsWorker");
  while (TRUE) {

    /* Wait for msg with work to do. */
    p = chMsgWait();


    struct wrapper_msg_base* msg = (struct wrapper_msg_base*) chMsgGet(p);

    msg->result = FR_INVALID_PARAMETER;

    switch(msg->action) {

#if HAS_MOUNT
    case eFMOUNT: {
      const struct wrapper_msg_vBYTEpFATFS* exmsg = \
                                    (const struct wrapper_msg_vBYTEpFATFS*) msg;
      msg->result = f_mount(exmsg->byte, exmsg->fatfsp);
      break;
    }
#endif /* HAS_MOUNT */

#if HAS_OPEN
    case eFOPEN: {
      const struct wrapper_msg_pFILpTCHARvBYTE* exmsg = \
                                (const struct wrapper_msg_pFILpTCHARvBYTE*) msg;

      msg->result = f_open(exmsg->filep, exmsg->string, exmsg->byte);
      break;
    }
#endif /* HAS_OPEN */

#if HAS_READ
    case eFREAD: {
      const struct wrapper_msg_pFILpVOIDvUINTpUINT* exmsg = \
                            (const struct wrapper_msg_pFILpVOIDvUINTpUINT*) msg;

      msg->result = f_read(exmsg->filep, exmsg->voidp, exmsg->uint,
                           exmsg->uintp);
      break;
    }
#endif /* HAS_READ */

#if HAS_WRITE
    case eFWRITE: {
      const struct wrapper_msg_pFILpVOIDvUINTpUINT* exmsg = \
                            (const struct wrapper_msg_pFILpVOIDvUINTpUINT*) msg;
      msg->result = f_write(exmsg->filep, exmsg->voidp, exmsg->uint,
                            exmsg->uintp);
      break;
    }
#endif /* HAD_WRITE */

#if HAS_SYNC
    case eFSYNC: {
      const struct wrapper_msg_pFIL* exmsg = \
                                          (const struct wrapper_msg_pFIL*) msg;
      msg->result = f_sync(exmsg->filep);
      break;
    }
#endif /* HAS_SYNC */

#if HAS_CHDRIVE
    case eFCHDRIVE: {
      const struct wrapper_msg_vBYTE* exmsg = \
                                          (const struct wrapper_msg_vBYTE*) msg;
      msg->result = f_chdrive(exmsg->byte);
      break;
    }
#endif

#if HAS_CHDIR
    case eFCHDIR: {
      const struct wrapper_msg_pTCHAR* exmsg = \
                                          (const struct wrapper_msg_pTCHAR*) msg;

      msg->result = f_chdir(exmsg->string);
      break;
    }
#endif /* HAS_CHDIR */

#if HAS_GETCWD
    case eFGETCWD: {
      const struct wrapper_msg_pTCHARvUINT* exmsg = \
                                  (const struct wrapper_msg_pTCHARvUINT*) msg;
      msg->result = f_getcwd(exmsg->string, exmsg->uint);
      break;
    }
#endif

#if HAS_LSEEK
    case eFLSEEK: {
      const struct wrapper_msg_pFILvDWORD* exmsg = \
                                  (const struct wrapper_msg_pFILvDWORD*) msg;
      msg->result = f_lseek(exmsg->filep, exmsg->dword);
      break;
    }
#endif /* HAS_LSEEK */

#if HAS_CLOSE
    case eFCLOSE: {
      const struct wrapper_msg_pFIL* exmsg = \
                                  (const struct wrapper_msg_pFIL*) msg;
      msg->result = f_close(exmsg->filep);
      break;
    }
#endif /* HAS_CLOSE */

#if HAS_OPENDIR
    case eFOPENDIR: {
      const struct wrapper_msg_pDIRpTCHAR* exmsg = \
                                  (const struct wrapper_msg_pDIRpTCHAR*) msg;
      msg->result = f_opendir(exmsg->dirp, exmsg->string);
      break;
    }
#endif /* HAD_OPENDIR */

#if HAS_READDIR
    case eFREADDIR: {
      const struct wrapper_msg_pDIRpFILINFO* exmsg = \
                                  (const struct wrapper_msg_pDIRpFILINFO*) msg;
      msg->result = f_readdir(exmsg->dirp, exmsg->filinfop);
      break;
    }
#endif /* HAS_READDIR */

#if HAS_STAT
    case eFSTAT: {
      const struct wrapper_msg_pTCHARpFILINFO* exmsg = \
                                  (const struct wrapper_msg_pTCHARpFILINFO*) msg;
      msg->result = f_stat(exmsg->string, exmsg->filinfop);
      break;
    }
#endif /* HAS_STAT */

#if HAS_GETFREE
    case eFGETFREE: {
      const struct wrapper_msg_pTCHARpDWORDppFATFS* exmsg = \
                            (const struct wrapper_msg_pTCHARpDWORDppFATFS*) msg;
      msg->result = f_getfree(exmsg->string, exmsg->dwordp, exmsg->fatfspp);
      break;
    }
#endif /* HAS_GETFREE */

#if HAS_TRUNCATE
    case eFTRUNCATE: {
      const struct wrapper_msg_pFIL* exmsg = \
                                          (const struct wrapper_msg_pFIL*) msg;
      msg->result = f_truncate(exmsg->filep);
      break;
    }
#endif /* HAS_TRUNCATE */

#if HAS_UNLINK
    case eFUNLINK: {
      const struct wrapper_msg_pTCHAR* exmsg = \
                                        (const struct wrapper_msg_pTCHAR*) msg;
      msg->result = f_unlink(exmsg->string);
      break;
    }
#endif /* HAS_UNLINK */

#if HAS_MKDIR
    case eFMKDIR: {
      const struct wrapper_msg_pTCHAR* exmsg = \
                                        (const struct wrapper_msg_pTCHAR*) msg;
      msg->result = f_mkdir(exmsg->string);
      break;
    }
#endif /* HAS_MKDIR */

#if HAS_CHMOD
    case eFCHMOD: {
      const struct wrapper_msg_pTCHARvBYTEvBYTE* exmsg = \
                              (const struct wrapper_msg_pTCHARvBYTEvBYTE*) msg;
      msg->result = f_chmod(exmsg->string, exmsg->byte1, exmsg->byte2);
      break;
    }
#endif /* HAS_CHMOD */

#if HAS_UTIME
    case eFUTIME: {
      const struct wrapper_msg_pTCHARpFILINFO* exmsg = \
                              (const struct wrapper_msg_pTCHARpFILINFO*) msg;
      msg->result = f_utime(exmsg->string, exmsg->filinfop);
      break;
    }
#endif /* HAD_UTIME */

#if HAS_RENAME
    case eFRENAME: {
      const struct wrapper_msg_pTCHARpTCHAR* exmsg = \
                                  (const struct wrapper_msg_pTCHARpTCHAR*) msg;

      msg->result = f_rename(exmsg->string1, exmsg->string2);
      break;
    }
#endif /* HAS_RENAME */

#if HAS_MKFS
    case eFMKFS: {
      const struct wrapper_msg_vBYTEvBYTEvUINT* exmsg = \
                              (const struct wrapper_msg_vBYTEvBYTEvUINT*) msg;
      msg->result = f_mkfs(exmsg->byte1, exmsg->byte2, exmsg->uint);
      break;
    }
#endif /* HAS_MKFS */

#if HAS_FDISK
    case eFFDISK: {
      const struct wrapper_msg_vBYTEpDWORDpVOID* exmsg = \
                              (const struct wrapper_msg_vBYTEpDWORDpVOID*) msg;
      msg->result = f_fdisk(exmsg->byte, exmsg->dwordp, exmsg->voidp);
      break;
    }
#endif /* HAS_FDISK */

#if HAS_GETS
    case eFGETS: {
      struct wrapper_msg_pTCHARvINTpFILpTCHAR* exmsg = \
                                (struct wrapper_msg_pTCHARvINTpFILpTCHAR*) msg;

      exmsg->string2 = f_gets(exmsg->string, exmsg->n, exmsg->filep);
      break;
    }
#endif /* HAS_GETS */

#if HAS_PUTC
    case eFPUTC: {
      const struct wrapper_msg_vTCHARpFIL* exmsg = \
                                  (const struct wrapper_msg_vTCHARpFIL*) msg;

      msg->result = f_putc(exmsg->tchar, exmsg->filep);
      break;
    }
#endif /* HAS_PUTC */

#if HAS_PUTS
    case eFPUTS: {
      const struct wrapper_msg_pTCHARpFIL* exmsg = \
                                  (const struct wrapper_msg_pTCHARpFIL*) msg;

      msg->result = f_puts(exmsg->string, exmsg->filep);
      break;
    }
#endif /* HAS_PUTS */

    }

    /* Done, release msg again. */
    chMsgRelease(p, 0);
  }

  return 0;
}


void wf_init (tprio_t priority) {
  workerThread = chThdCreateStatic(waFatFSWorkerThread,
                                   sizeof(waFatFSWorkerThread),
                                   priority, ThreadFatFSWorker, NULL);
}



#if HAS_MOUNT || defined(__DOXYGEN__)
/**
 * @brief Mount/Unmount a logical Drive
 * @param vol   Logical drive number to be mounted/unmounted.
 * @param fs    Pointer to new file system object (NULL for unmount).
 * @return
 */
FRESULT wf_mount (BYTE vol, FATFS* fs) {
  struct wrapper_msg_vBYTEpFATFS msg;

  msg.action = eFMOUNT;
  msg.byte = vol;
  msg.fatfsp = fs;

  chMsgSend(workerThread, (msg_t) &msg);
  return msg.result;
}
#endif /* HAS_MOUNT */


#if HAS_OPEN || defined(__DOXYGEN__)
/**
 * @brief Open or Create a File.
 *
 * @param fp    Pointer to the blank file object.
 * @param path  Pointer to the file name.
 * @param mode  Access mode and file open mode flags.
 * @return
 */
FRESULT wf_open (FIL* fp, const TCHAR* path, BYTE mode) {
  struct wrapper_msg_pFILpTCHARvBYTE msg;

  msg.action = eFOPEN;
  msg.filep = fp;
  msg.string = (TCHAR*) path;
  msg.byte = mode;

  chMsgSend(workerThread, (msg_t) &msg);
  return msg.result;
}
#endif /* HAS_OPEN */

#if HAS_READ || defined(__DOXYGEN__)
/**
 * @brief Read File
 *
 * @param fp    Pointer to the file object.
 * @param buff  Pointer to data buffer.
 * @param btr   Number of bytes to read.
 * @param br    Pointer to number of bytes read.
 * @return
 */
FRESULT wf_read (FIL* fp, void* buff, UINT btr, UINT* br) {
  struct wrapper_msg_pFILpVOIDvUINTpUINT msg;

  msg.action = eFREAD;
  msg.filep = fp;
  msg.voidp = buff;
  msg.uint = btr;
  msg.uintp = br;

  chMsgSend(workerThread, (msg_t) &msg);
  return msg.result;
}
#endif /* HAS_READ */


#if HAS_WRITE || defined(__DOXYGEN__)
/**
 * @brief Write File.
 *
 * @param fp    Pointer to the file object.
 * @param buff  Pointer to the data to be written.
 * @param btw   Number of bytes to write.
 * @param bw    Pointer to number of bytes written.
 * @return
 */
FRESULT wf_write (FIL* fp, const void* buff, UINT btw, UINT* bw) {
  struct wrapper_msg_pFILpVOIDvUINTpUINT msg;

  msg.action = eFWRITE;
  msg.filep = fp;
  msg.voidp = (void*) buff;
  msg.uint = btw;
  msg.uintp = bw;

  chMsgSend(workerThread, (msg_t) &msg);
  return msg.result;
}
#endif /* HAS_WRITE */

#if HAS_SYNC || defined(__DOXYGEN__)
/**
 * @brief Synchronize the File Object
 *
 * @param fp    Pointer to the file object.
 * @return
 */
FRESULT wf_sync (FIL* fp) {
  struct wrapper_msg_pFIL msg;

  msg.action = eFSYNC;
  msg.filep = fp;

  chMsgSend(workerThread, (msg_t) &msg);
  return msg.result;
}
#endif /* HAS_SYNC */


#if HAS_CHDRIVE || defined(__DOXYGEN__)
/**
 * @brief Change current drive.
 * @param drv    Drive number.
 * @return
 */
FRESULT wf_chdrive (BYTE drv) {
  struct wrapper_msg_vBYTE msg;

  msg.action = eFCHDRIVE;
  msg.byte = drv;

  chMsgSend(workerThread, (msg_t) &msg);
  return msg.result;
}
#endif /* HAS_CHDRIVE */

#if HAS_CHDIR || defined(__DOXYGEN__)
/**
 * @brief Change current path.
 * @param path  Pointer to the directory path.
 * @return
 */
FRESULT wf_chdir (const TCHAR* path) {
  struct wrapper_msg_pTCHAR msg;

  msg.action = eFCHDIR;
  msg.string = (TCHAR*) path;

  chMsgSend(workerThread, (msg_t) &msg);
  return msg.result;
}
#endif /* HAS_CHDIR */

#if HAS_GETCWD || defined(__DOXYGEN__)
/**
 * @brief Get current working directory.
 * @param path      Pointer to the directory path.
 * @param sz_path   Size of path.
 * @return
 */
FRESULT wf_getcwd (TCHAR* path, UINT sz_path) {
  struct wrapper_msg_pTCHARvUINT msg;

  msg.action = eFGETCWD;
  msg.string = (TCHAR*) path;
  msg.uint = sz_path;

  chMsgSend(workerThread, (msg_t) &msg);
  return msg.result;
}
#endif /* HAS_GETCWD */

#if HAS_LSEEK || defined(__DOXYGEN__)
/**
 * @brief Seek File R/W Pointer.
 *
 * @param fp    Pointer to the file object.
 * @param ofs   File pointer from top of file.
 * @return
 */
FRESULT wf_lseek (FIL* fp, DWORD ofs) {
  struct wrapper_msg_pFILvDWORD msg;

  msg.action = eFLSEEK;
  msg.filep = fp;
  msg.dword = ofs;

  chMsgSend(workerThread, (msg_t) &msg);
  return msg.result;
}
#endif /* HAS_LSEEK */

#if HAS_CLOSE || defined(__DOXYGEN__)
/**
 * @brief Close File.
 *
 * @param fp    Pointer to the file object to be closed.
 * @return
 */
FRESULT wf_close (FIL* fp) {
  struct wrapper_msg_pFIL msg;

  msg.action = eFCLOSE;
  msg.filep = fp;

  chMsgSend(workerThread, (msg_t) &msg);
  return msg.result;
}
#endif /* HAS_CLOSE */

#if HAS_OPENDIR || defined(__DOXYGEN__)
/**
 * @brief Create a Directory Object
 *
 * @param dj    Pointer to directory object to create.
 * @param path  Pointer to the directory path.
 * @return
 */
FRESULT wf_opendir (DIR* dj, const TCHAR* path) {
  struct wrapper_msg_pDIRpTCHAR msg;

  msg.action = eFOPENDIR;
  msg.dirp = dj;
  msg.string = (TCHAR*) path;

  chMsgSend(workerThread, (msg_t) &msg);
  return msg.result;
}
#endif /* HAS_OPENDIR */

#if HAS_READDIR || defined(__DOXYGEN__)
/**
 * @brief Read Directory Entry in Sequence
 * @param dj    Pointer to the open directory object.
 * @param fno   Pointer to file information to return.
 * @return
 */
FRESULT wf_readdir (DIR* dj, FILINFO* fno) {
  struct wrapper_msg_pDIRpFILINFO msg;

  msg.action = eFREADDIR;
  msg.dirp = dj;
  msg.filinfop = fno;

  chMsgSend(workerThread, (msg_t) &msg);
  return msg.result;
}
#endif /* HAS_READDIR */

#if HAS_STAT || defined(__DOXYGEN__)
/**
 * @brief Get File Status.
 *
 * @param path  Pointer to the file path.
 * @param fno   Pointer to file information to return.
 * @return
 */
FRESULT wf_stat (const TCHAR* path, FILINFO* fno) {
  struct wrapper_msg_pTCHARpFILINFO msg;

  msg.action = eFSTAT;
  msg.string = (TCHAR*) path;
  msg.filinfop = fno;

  chMsgSend(workerThread, (msg_t) &msg);
  return msg.result;
}
#endif /* HAS_STAT */

#if HAS_GETFREE || defined(__DOXYGEN__)
/**
 * @brief Get Number of Free Clusters.
 *
 * @param path  Pointer to the logical drive number (root dir).
 * @param nclst Pointer to the variable to return number of free clusters,
 * @param fatfs Pointer to pointer to corresponding file system object to
 *              return.
 * @return
 */
FRESULT wf_getfree (const TCHAR* path, DWORD*nclst, FATFS** fatfs) {
  struct wrapper_msg_pTCHARpDWORDppFATFS msg;

  msg.action = eFGETFREE;
  msg.string = (TCHAR*) path;
  msg.dwordp = nclst;
  msg.fatfspp = fatfs;

  chMsgSend(workerThread, (msg_t) &msg);
  return msg.result;
}
#endif /* HAS_GETFREE */

#if HAS_TRUNCATE || defined(__DOXYGEN__)
/**
 * @brief Truncate File.
 *
 * @param fp    Pointer to the file object.
 * @return
 */
FRESULT wf_truncate (FIL* fp){
  struct wrapper_msg_pFIL msg;

  msg.action = eFTRUNCATE;
  msg.filep = fp;

  chMsgSend(workerThread, (msg_t) &msg);
  return msg.result;
}
#endif /* HAS_TRUNCATE */

#if HAS_UNLINK || defined(__DOXYGEN__)
/**
 * @brief Delete a File or Directory.
 * @param path  Pointer to the file or directory path.
 * @return
 */
FRESULT wf_unlink (const TCHAR* path) {
  struct wrapper_msg_pTCHAR msg;

  msg.action = eFUNLINK;
  msg.string = (TCHAR*) path;

  chMsgSend(workerThread, (msg_t) &msg);
  return msg.result;
}
#endif /* HAS_UNLINK */

#if HAS_MKDIR || defined(__DOXYGEN__)
/**
 * @brief Create a Directory.
 * @param path  Pointer to the directory path.
 * @return
 */
FRESULT wf_mkdir (const TCHAR* path) {
  struct wrapper_msg_pTCHAR msg;

  msg.action = eFMKDIR;
  msg.string = (TCHAR*) path;

  chMsgSend(workerThread, (msg_t) &msg);
  return msg.result;
}
#endif /* HAS_MKDIR */

#if HAS_CHMOD || defined(__DOXYGEN__)
/**
 * @brief Change Attribute.
 *
 * @param path  Pointer to the file path.
 * @param value Attribute bits.
 * @param mask  Attribute mask to change.
 * @return
 */
FRESULT wf_chmod (const TCHAR* path, BYTE value, BYTE mask) {
  struct wrapper_msg_pTCHARvBYTEvBYTE msg;

  msg.action = eFCHMOD;
  msg.string = (TCHAR*) path;
  msg.byte1 = value;
  msg.byte2 = mask;

  chMsgSend(workerThread, (msg_t) &msg);
  return msg.result;
}
#endif /* HAS_CHMOD */

#if HAS_UTIME || defined(__DOXYGEN__)
/**
 * @brief Change Timestamp.
 *
 * @param path  Pointer to the file/directory name.
 * @param fno   Pointer to the time stamp to be set.
 * @return
 */
FRESULT wf_utime (const TCHAR* path, const FILINFO* fno) {
  struct wrapper_msg_pTCHARpFILINFO msg;

  msg.action = eFUTIME;
  msg.string = (TCHAR*) path;
  msg.filinfop = (FILINFO*) fno;

  chMsgSend(workerThread, (msg_t) &msg);
  return msg.result;
}
#endif /* HAS_UTIME */

#if HAS_RENAME || defined(__DOXYGEN__)
/**
 * @brief Rename File/Directory.
 * @param path_old  Pointer to the old name.
 * @param path_new  Pointer to the new name
 * @return
 */
FRESULT wf_rename (const TCHAR* path_old, const TCHAR* path_new) {
  struct wrapper_msg_pTCHARpTCHAR msg;

  msg.action = eFRENAME;
  msg.string1 = (TCHAR*) path_old;
  msg.string2 = (TCHAR*) path_new;

  chMsgSend(workerThread, (msg_t) &msg);
  return msg.result;
}
#endif /* HAS_RENAME */

#if HAS_MKFS || defined(__DOXYGEN__)
/**
 * @brief Create File System on the Drive.
 * @param drv   Logical drive number.
 * @param sfd   Partitioning rule 0:FDISK, 1:SFD.
 * @param au    Allocation unit size [bytes].
 * @return
 */
FRESULT wf_mkfs (BYTE drv, BYTE sfd, UINT au) {
  struct wrapper_msg_vBYTEvBYTEvUINT msg;

  msg.action = eFMKFS;
  msg.byte1 = drv;
  msg.byte2 = sfd;
  msg.uint = au;

  chMsgSend(workerThread, (msg_t) &msg);
  return msg.result;
}
#endif /* HAS_MKFS */

#if HAS_FDISK || defined(__DOXYGEN__)
/**
 * @brief Divide Physical Drive
 * @param pdrv  Physical drive number.
 * @param szt   Pointer to the size table for each partitions.
 * @param work  Pointer to the working buffer.
 * @return
 */
FRESULT wf_fdisk (BYTE pdrv, const DWORD szt[], void* work) {
  struct wrapper_msg_vBYTEpDWORDpVOID msg;

  msg.action = eFFDISK;
  msg.byte = pdrv;
  msg.dwordp = (DWORD*) szt;
  msg.voidp = work;

  chMsgSend(workerThread, (msg_t) &msg);
  return msg.result;
}
#endif /* HAS_FDISK */

#if HAS_GETS || defined(__DOXYGEN__)
/**
 * @brief Get a string from the file.
 * @param buff  Pointer to the string buffer to read.
 * @param len   Size of string buffer (characters).
 * @param fil   Pointer to the file object.
 * @return
 */
TCHAR* wf_gets (TCHAR* buff, int len, FIL* fil) {
  struct wrapper_msg_pTCHARvINTpFILpTCHAR msg;

  msg.action = eFGETS;
  msg.string = buff;
  msg.n = len;
  msg.filep = fil;

  chMsgSend(workerThread, (msg_t) &msg);
  return msg.string2;
}
#endif /* HAS_GETS */

#if HAS_PUTC || defined(__DOXYGEN__)
/**
 * @brief Put a character to the file.
 * @param c     A character to be output.
 * @param fil   Pointer to the file object.
 * @return
 */
int wf_putc (TCHAR c, FIL* fil) {
  struct wrapper_msg_vTCHARpFIL msg;

  msg.action = eFPUTC;
  msg.tchar = c;
  msg.filep = fil;


  chMsgSend(workerThread, (msg_t) &msg);
  return msg.result;
}
#endif /* HAS_PUTC */

#if HAS_PUTS || defined(__DOXYGEN__)
/**
 * @brief Put a string to the file.
 * @param str   Pointer to the string to be output.
 * @param fil   Pointer to the file object.
 * @return
 */
int wf_puts (const TCHAR* str, FIL* fil) {
  struct wrapper_msg_pTCHARpFIL msg;

  msg.action = eFPUTS;
  msg.string = (TCHAR*) str;
  msg.filep = fil;

  chMsgSend(workerThread, (msg_t) &msg);
  return msg.result;
}
#endif /* HAS_PUTS */

const char *wf_strerror(FRESULT r) {
	switch(r) {
	case FR_OK: return "Succeeded";
	case FR_DISK_ERR: return "Error occured on low level disk I/O layer";
	case FR_INT_ERR: return "Assertion failed";
	case FR_NOT_READY: return "The physical drive cannot work";
	case FR_NO_FILE: return "Could not find the file";
	case FR_NO_PATH: return "Could not find the path";
	case FR_INVALID_NAME: return "The path name format is invalid";
	case FR_DENIED: return "Acces denied or directory full";
	case FR_EXIST: return "Acces denied";
	case FR_INVALID_OBJECT: return "The file/directory object is invalid";
	case FR_WRITE_PROTECTED: return "The physical drive is write protected";
	case FR_INVALID_DRIVE: return "The logical drive number is invalid";
	case FR_NOT_ENABLED: return "The volume has no work area";
	case FR_NO_FILESYSTEM: return "There is no valid FAT volume";
	case FR_MKFS_ABORTED: return "f_mkfs() aborted due to any parameter error";
	case FR_TIMEOUT: return "Volume access timeout";
	case FR_LOCKED: return "File locked";
	case FR_NOT_ENOUGH_CORE: return "LFN working buffer can not allocated";
	case FR_TOO_MANY_OPEN_FILES: return "Number of open files > _FS_SHARE";
	case FR_INVALID_PARAMETER: return "Given parameter is invalid";
	default: return "Unknow error";
	}
}
