/*
 * Phoenix-RTOS
 *
 * Meterfs test
 *
 * Copyright 2018 Phoenix Systems
 * Author: Aleksander Kaminski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <sys/msg.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "../meterfs.h"


struct {
	oid_t meterfs_oid;
	msg_t msg;
} test_common;


void test_hexdump(char *buff, size_t bufflen)
{
	int i, j;

	for (i = 0; i < bufflen; i += 16) {
		printf("\t");
		for (j = i; j < 16 && i + j < bufflen; ++j)
			printf("0x%02x ", buff[i + j]);
		printf("\n");
	}
}


int test_allocate(const char *name, size_t sectors, size_t filesz, size_t recordsz)
{
	meterfs_i_devctl_t *i = (meterfs_i_devctl_t *)&test_common.msg.i.raw;
	meterfs_o_devctl_t *o = (meterfs_o_devctl_t *)&test_common.msg.o.raw;
	int err;

	test_common.msg.type = mtDevCtl;
	test_common.msg.i.data = NULL;
	test_common.msg.i.size = 0;
	test_common.msg.o.data = NULL;
	test_common.msg.i.size = 0;
	i->type = meterfs_allocate;
	strncpy(i->allocate.name, name, sizeof(i->allocate.name));
	i->allocate.sectors = sectors;
	i->allocate.filesz = filesz;
	i->allocate.recordsz = recordsz;

	printf("test: Allocating file \"%s\": %zu sectors, file size %zu, record size %zu\n", name, sectors, filesz, recordsz);

	err = msgSend(test_common.meterfs_oid.port, &test_common.msg);

	err = (err < 0) ? err : o->err;

	printf("test: (%d)\n", err);

	return err;
}


int test_resize(oid_t *oid, size_t filesz, size_t recordsz)
{
	meterfs_i_devctl_t *i = (meterfs_i_devctl_t *)&test_common.msg.i.raw;
	meterfs_o_devctl_t *o = (meterfs_o_devctl_t *)&test_common.msg.o.raw;
	int err;

	test_common.msg.type = mtDevCtl;
	test_common.msg.i.data = NULL;
	test_common.msg.i.size = 0;
	test_common.msg.o.data = NULL;
	test_common.msg.i.size = 0;
	i->type = meterfs_resize;
	i->resize.oid = *oid;
	i->resize.filesz = filesz;
	i->resize.recordsz = recordsz;

	printf("test: Resizing file #%u: new file size %zu, new record size %zu\n", oid->id, filesz, recordsz);

	err = msgSend(test_common.meterfs_oid.port, &test_common.msg);

	err = (err < 0) ? err : o->err;

	printf("test: (%d)\n", err);

	return err;
}


int test_chiperase(void)
{
	meterfs_i_devctl_t *i = (meterfs_i_devctl_t *)&test_common.msg.i.raw;
	meterfs_o_devctl_t *o = (meterfs_o_devctl_t *)&test_common.msg.o.raw;
	int err;

	test_common.msg.type = mtDevCtl;
	test_common.msg.i.data = NULL;
	test_common.msg.i.size = 0;
	test_common.msg.o.data = NULL;
	test_common.msg.i.size = 0;
	i->type = meterfs_chiperase;

	printf("test: Performing chip erase\n");

	err = msgSend(test_common.meterfs_oid.port, &test_common.msg);

	err = (err < 0) ? err : o->err;

	printf("test: (%d)\n", err);

	return err;
}


int test_fileinfo(oid_t *oid, struct _info *info)
{
	meterfs_i_devctl_t *i = (meterfs_i_devctl_t *)&test_common.msg.i.raw;
	meterfs_o_devctl_t *o = (meterfs_o_devctl_t *)&test_common.msg.o.raw;
	int err;

	test_common.msg.type = mtDevCtl;
	test_common.msg.i.data = NULL;
	test_common.msg.i.size = 0;
	test_common.msg.o.data = NULL;
	test_common.msg.i.size = 0;
	i->type = meterfs_info;
	i->oid = *oid;

	err = msgSend(test_common.meterfs_oid.port, &test_common.msg);

	if (info != NULL)
		memcpy(info, &o->info, sizeof(*info));

	printf("test: Got file #%u info: %zu sectors, %zu records, file size %zu, record size %zu\n",
		oid->id, o->info.sectors, o->info.recordcnt, o->info.filesz, o->info.recordsz);

	err = (err < 0) ? err : o->err;

	printf("test: (%d)\n", err);

	return err;
}


int test_write(oid_t *oid, void *buff, size_t len)
{
	int err;

	test_common.msg.type = mtWrite;
	test_common.msg.i.io.oid = *oid;
	test_common.msg.i.io.offs = 0;
	test_common.msg.i.io.len = len;
	test_common.msg.i.io.mode = 0;
	test_common.msg.i.data = NULL;
	test_common.msg.i.size = 0;
	test_common.msg.o.data = buff;
	test_common.msg.o.size = len;

	err = msgSend(test_common.meterfs_oid.port, &test_common.msg);

	printf("test: Write to file #%u len %zu\n", oid->id, len);
	test_hexdump(buff, len);

	err = (err < 0) ? err : test_common.msg.o.io.err;

	printf("test: (%d)\n", err);

	return err;
}


int test_read(oid_t *oid, offs_t offs, void *buff, size_t len)
{
	int err;

	test_common.msg.type = mtRead;
	test_common.msg.i.io.oid = *oid;
	test_common.msg.i.io.offs = offs;
	test_common.msg.i.io.len = len;
	test_common.msg.i.io.mode = 0;
	test_common.msg.i.data = buff;
	test_common.msg.i.size = len;
	test_common.msg.o.data = NULL;
	test_common.msg.o.size = 0;

	err = msgSend(test_common.meterfs_oid.port, &test_common.msg);

	printf("test: Read from file #%u len %zu @offset %zu\n", oid->id, len, (size_t)offs);
	test_hexdump(buff, len);

	err = (err < 0) ? err : test_common.msg.o.io.err;

	printf("test: (%d)\n", err);

	return err;
}


int test_open(const char *name, oid_t *oid)
{
	int err;

	printf("test: lookup of file \"%s\" ", name);

	if ((err = lookup(name, oid)) < 0) {
		printf(" failed (%d)\n", err);
		return err;
	}

	printf(" found id %u\n", oid->id);

	test_common.msg.type = mtOpen;
	test_common.msg.i.openclose.oid = *oid;
	test_common.msg.i.openclose.flags = 0;
	test_common.msg.i.data = NULL;
	test_common.msg.i.size = 0;
	test_common.msg.o.data = NULL;
	test_common.msg.i.size = 0;

	printf("test: Open\n");

	err = msgSend(test_common.meterfs_oid.port, &test_common.msg);

	err = (err < 0) ? err : test_common.msg.o.io.err;

	printf("test: (%d)\n", err);

	return err;
}


int test_close(oid_t *oid)
{
	int err;

	test_common.msg.type = mtClose;
	test_common.msg.i.openclose.oid = *oid;
	test_common.msg.i.openclose.flags = 0;
	test_common.msg.i.data = NULL;
	test_common.msg.i.size = 0;
	test_common.msg.o.data = NULL;
	test_common.msg.i.size = 0;

	printf("test: Close id %u\n", oid->id);

	err = msgSend(test_common.meterfs_oid.port, &test_common.msg);

	err = (err < 0) ? err : test_common.msg.o.io.err;

	printf("test: (%d)\n", err);

	return err;
}


int main(void)
{
	oid_t oid;
	char buff[20];

	while (lookup("/", &test_common.meterfs_oid) < 0)
		usleep(100000);

	test_chiperase();
	test_allocate("test1", 2, 2000, 20);
	test_open("/test1", &oid);
	test_write(&oid, "test", 5);
	test_fileinfo(&oid, NULL);
	test_read(&oid, 0, buff, 5);

	return 0;
}