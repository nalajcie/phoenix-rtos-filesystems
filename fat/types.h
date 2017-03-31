/*
 * Phoenix-RTOS
 *
 * Misc. utilities - FAT
 *
 * Types and errors
 *
 * Copyright 2012 Phoenix Systems
 * Author: Pawel Pisarczyk
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _MISC_TYPES_H_
#define _MISC_TYPES_H_


#include <stdio.h>
#include <ctype.h>


typedef struct _fat_opt_t {
	const char *dev;
	unsigned int off;
} fat_opt_t;


typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef int s32;


/* errors */

#define EOK     0
#define ENOENT  1
#define EPROTO  2
#define EINVAL  3
#define ENOMEM  4


typedef int fat_dev_t;


#define fatprint_err(...) printf(__VA_ARGS__)


#define min(a, b) ({__typeof__(a) _a = (a); \
	__typeof__(b) _b = (b); \
	_a < _b ? _a : _b;})


#endif
