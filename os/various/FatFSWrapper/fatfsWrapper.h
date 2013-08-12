/**
 * @file fatfsWrapper.h
 * @author Matthias Blaicher
 *
 * @brief Wrapper around FatFS to localize all access over one single thread.
 *
 * @brief This greatly reduces stack usage when you use FatFS from several
 *        threads.
 */

#ifndef FATFSWRAPPER_H_
#define FATFSWRAPPER_H_

#include "ff.h"

#ifdef __cplusplus
extern "C" {
#endif

void wf_init (tprio_t priority);

FRESULT wf_mount (BYTE, FATFS*);                     /* Mount/Unmount a logical drive */
FRESULT wf_open (FIL*, const TCHAR*, BYTE);          /* Open or create a file */
FRESULT wf_read (FIL*, void*, UINT, UINT*);          /* Read data from a file */
FRESULT wf_lseek (FIL*, DWORD);                      /* Move file pointer of a file object */
FRESULT wf_close (FIL*);                             /* Close an open file object */
FRESULT wf_opendir (DIR*, const TCHAR*);             /* Open an existing directory */
FRESULT wf_readdir (DIR*, FILINFO*);                 /* Read a directory item */
FRESULT wf_stat (const TCHAR*, FILINFO*);            /* Get file status */
FRESULT wf_write (FIL*, const void*, UINT, UINT*);   /* Write data to a file */
FRESULT wf_getfree (const TCHAR*, DWORD*, FATFS**);  /* Get number of free clusters on the drive */
FRESULT wf_truncate (FIL*);                          /* Truncate file */
FRESULT wf_sync (FIL*);                              /* Flush cached data of a writing file */
FRESULT wf_unlink (const TCHAR*);                    /* Delete an existing file or directory */
FRESULT wf_mkdir (const TCHAR*);                     /* Create a new directory */
FRESULT wf_chmod (const TCHAR*, BYTE, BYTE);         /* Change attriburte of the file/dir */
FRESULT wf_utime (const TCHAR*, const FILINFO*);     /* Change timestamp of the file/dir */
FRESULT wf_rename (const TCHAR*, const TCHAR*);      /* Rename/Move a file or directory */
FRESULT wf_chdrive (BYTE);                           /* Change current drive */
FRESULT wf_chdir (const TCHAR*);                     /* Change current directory */
FRESULT wf_getcwd (TCHAR*, UINT);                    /* Get current directory */
FRESULT wf_forward (FIL*, UINT(*)(const BYTE*,UINT), UINT, UINT*);   /* Forward data to the stream */
FRESULT wf_mkfs (BYTE, BYTE, UINT);                  /* Create a file system on the drive */
FRESULT wf_fdisk (BYTE, const DWORD[], void*);       /* Divide a physical drive into some partitions */
int wf_putc (TCHAR, FIL*);                           /* Put a character to the file */
int wf_puts (const TCHAR*, FIL*);                    /* Put a string to the file */
int wf_printf (FIL*, const TCHAR*, ...);             /* Put a formatted string to the file */
TCHAR* wf_gets (TCHAR*, int, FIL*);                  /* Get a string from the file */

#define wf_eof(fp) f_eof(fp)
#define wf_error(fp) f_error(fp)
#define wf_tell(fp) f_tell(fp)
#define wf_size(fp) f_size(fp)

const char *wf_strerror(FRESULT result);

#ifdef __cplusplus
}
#endif


#endif /* FATFSWRAPPER_H_ */
