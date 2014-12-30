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

extern const posix_mountpoint_vmt_t fatfs_vmt;

#define FATFS_DECLARE_MOUNTPOINT(name, path, fatfs) \
	posix_mountpoint_t name = { \
			&fatfs, \
			&fatfs_vmt, \
			path, \
			NULL \
	}

#endif /* POSIX_FATFS_PROVIDER_H_ */
