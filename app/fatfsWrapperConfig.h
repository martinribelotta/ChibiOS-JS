/**
 * @file fatfsWrapperConfig.hpp
 * @author Matthias Blaicher
 *
 * @brief Configuration file for FatFS wrapper
 */

#ifndef FATFSWRAPPERCONFIG_HPP_
#define FATFSWRAPPERCONFIG_HPP_

/**
 * @brief Worker area size.
 */
#if !defined(FATFS_WRP_WORKER_SIZE) || defined(__DOXYGEN__)
#define FATFS_WRP_WORKER_SIZE                 2048
#endif

/**
 * @brief Enable wrapped calls to f_mount.
 */
#if !defined(FATFS_WRP_ENABLE_MOUNT) || defined(__DOXYGEN__)
#define FATFS_WRP_ENABLE_MOUNT                 1
#endif

/**
 * @brief Enable wrapped calls to f_open.
 */
#if !defined(FATFS_WRP_ENABLE_OPEN) || defined(__DOXYGEN__)
#define FATFS_WRP_ENABLE_OPEN                 1
#endif

/**
 * @brief Enable wrapped calls to f_read.
 */
#if !defined(FATFS_WRP_ENABLE_READ) || defined(__DOXYGEN__)
#define FATFS_WRP_ENABLE_READ                 1
#endif

/**
 * @brief Enable wrapped calls to f_lseek.
 */
#if !defined(FATFS_WRP_ENABLE_LSEEK) || defined(__DOXYGEN__)
#define FATFS_WRP_ENABLE_LSEEK                 1
#endif

/**
 * @brief Enable wrapped calls to f_close.
 */
#if !defined(FATFS_WRP_ENABLE_CLOSE) || defined(__DOXYGEN__)
#define FATFS_WRP_ENABLE_CLOSE                 1
#endif

/**
 * @brief Enable wrapped calls to f_opendir.
 */
#if !defined(FATFS_WRP_ENABLE_OPENDIR) || defined(__DOXYGEN__)
#define FATFS_WRP_ENABLE_OPENDIR                 1
#endif

/**
 * @brief Enable wrapped calls to f_readdir.
 */
#if !defined(FATFS_WRP_ENABLE_READDIR) || defined(__DOXYGEN__)
#define FATFS_WRP_ENABLE_READDIR                 1
#endif

/**
 * @brief Enable wrapped calls to f_stat.
 */
#if !defined(FATFS_WRP_ENABLE_STAT) || defined(__DOXYGEN__)
#define FATFS_WRP_ENABLE_STAT                 1
#endif

/**
 * @brief Enable wrapped calls to f_write.
 */
#if !defined(FATFS_WRP_ENABLE_WRITE) || defined(__DOXYGEN__)
#define FATFS_WRP_ENABLE_WRITE                 1
#endif

/**
 * @brief Enable wrapped calls to f_getfree.
 */
#if !defined(FATFS_WRP_ENABLE_GETFREE) || defined(__DOXYGEN__)
#define FATFS_WRP_ENABLE_GETFREE                 1
#endif

/**
 * @brief Enable wrapped calls to f_truncate.
 */
#if !defined(FATFS_WRP_ENABLE_TRUNKATE) || defined(__DOXYGEN__)
#define FATFS_WRP_ENABLE_TRUNKATE                 1
#endif

/**
 * @brief Enable wrapped calls to f_sync.
 */
#if !defined(FATFS_WRP_ENABLE_SYNC) || defined(__DOXYGEN__)
#define FATFS_WRP_ENABLE_SYNC                 1
#endif

/**
 * @brief Enable wrapped calls to f_unlink.
 */
#if !defined(FATFS_WRP_ENABLE_UNLINK) || defined(__DOXYGEN__)
#define FATFS_WRP_ENABLE_UNLINK                 1
#endif

/**
 * @brief Enable wrapped calls to f_mkdir.
 */
#if !defined(FATFS_WRP_ENABLE_MKDIR) || defined(__DOXYGEN__)
#define FATFS_WRP_ENABLE_MKDIR                 1
#endif

/**
 * @brief Enable wrapped calls to f_chmod.
 */
#if !defined(FATFS_WRP_ENABLE_CHMOD) || defined(__DOXYGEN__)
#define FATFS_WRP_ENABLE_CHMOD                 1
#endif

/**
 * @brief Enable wrapped calls to f_utime.
 */
#if !defined(FATFS_WRP_ENABLE_UTIME) || defined(__DOXYGEN__)
#define FATFS_WRP_ENABLE_UTIME                 1
#endif

/**
 * @brief Enable wrapped calls to f_rename.
 */
#if !defined(FATFS_WRP_ENABLE_RENAME) || defined(__DOXYGEN__)
#define FATFS_WRP_ENABLE_RENAME                 1
#endif

/**
 * @brief Enable wrapped calls to f_chdrive.
 */
#if !defined(FATFS_WRP_ENABLE_CHDRIVE) || defined(__DOXYGEN__)
#define FATFS_WRP_ENABLE_CHDRIVE                 1
#endif

/**
 * @brief Enable wrapped calls to f_chdir.
 */
#if !defined(FATFS_WRP_ENABLE_CHDIR) || defined(__DOXYGEN__)
#define FATFS_WRP_ENABLE_CHDIR                 1
#endif

/**
 * @brief Enable wrapped calls to f_getcwd.
 */
#if !defined(FATFS_WRP_ENABLE_GETCWD) || defined(__DOXYGEN__)
#define FATFS_WRP_ENABLE_GETCWD                 1
#endif

/**
 * @brief Enable wrapped calls to f_forward.
 */
#if !defined(FATFS_WRP_ENABLE_FORWARD) || defined(__DOXYGEN__)
#define FATFS_WRP_ENABLE_FORWARD                 1
#endif

/**
 * @brief Enable wrapped calls to f_mkfs.
 */
#if !defined(FATFS_WRP_ENABLE_MKDS) || defined(__DOXYGEN__)
#define FATFS_WRP_ENABLE_MKFS                 1
#endif

/**
 * @brief Enable wrapped calls to f_fdisk.
 */
#if !defined(FATFS_WRP_ENABLE_FDISK) || defined(__DOXYGEN__)
#define FATFS_WRP_ENABLE_FDISK                 1
#endif

/**
 * @brief Enable wrapped calls to f_putc.
 */
#if !defined(FATFS_WRP_ENABLE_PUTC) || defined(__DOXYGEN__)
#define FATFS_WRP_ENABLE_PUTC                 1
#endif

/**
 * @brief Enable wrapped calls to f_puts.
 */
#if !defined(FATFS_WRP_ENABLE_PUTS) || defined(__DOXYGEN__)
#define FATFS_WRP_ENABLE_PUTS                 1
#endif

/**
 * @brief Enable wrapped calls to f_printf.
 */
#if !defined(FATFS_WRP_ENABLE_PRINTF) || defined(__DOXYGEN__)
#define FATFS_WRP_ENABLE_PRINTF                 1
#endif

/**
 * @brief Enable wrapped calls to f_gets.
 */
#if !defined(FATFS_WRP_ENABLE_GETS) || defined(__DOXYGEN__)
#define FATFS_WRP_ENABLE_GETS                 1
#endif


#endif /* FATFSWRAPPERCONFIG_HPP_ */
