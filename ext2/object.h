/*
 * Phoenix-RTOS
 *
 * ext2
 *
 * object.h
 *
 * Copyright 2017 Phoenix Systems
 * Author: Kamil Amanowicz
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _OBJECT_H_
#define _OBJECT_H_ /* object.h */


extern ext2_object_t *object_get(unsigned int id);


extern void object_put(ext2_object_t *o);


extern void object_sync(ext2_object_t *o);


extern ext2_object_t *object_create(id_t id, ext2_inode_t **inode, int mode);


extern int object_destroy(ext2_object_t *o);


extern void object_init(void);

#endif /* object.h */
