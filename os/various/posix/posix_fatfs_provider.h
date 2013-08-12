/*
 * posix_fatfs_provider.h
 *
 *  Created on: 11/08/2013
 *      Author: mribelotta
 */

#ifndef POSIX_FATFS_PROVIDER_H_
#define POSIX_FATFS_PROVIDER_H_

#include "posix.h"
#include <ff.h>

extern FRESULT posix_fatfs_provider_init(posix_file_provider_t *p, FATFS *fs, int vol);

#endif /* POSIX_FATFS_PROVIDER_H_ */
