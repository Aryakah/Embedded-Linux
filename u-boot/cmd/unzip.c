// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2000
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 */

#include <command.h>
#include <env.h>
#include <gzip.h>
#include <mapmem.h>
#include <part.h>
#include <vsprintf.h>

static int do_unzip(struct cmd_tbl *cmdtp, int flag, int argc,
		    char *const argv[])
{
	unsigned long src, dst;
	unsigned long src_len = ~0UL, dst_len = ~0UL;

	switch (argc) {
		case 4:
			dst_len = hextoul(argv[3], NULL);
			/* fall through */
		case 3:
			src = hextoul(argv[1], NULL);
			dst = hextoul(argv[2], NULL);
			break;
		default:
			return CMD_RET_USAGE;
	}

	if (gunzip(map_sysmem(dst, dst_len), dst_len, map_sysmem(src, 0),
		   &src_len) != 0)
		return 1;

	printf("Uncompressed size: %lu = 0x%lX\n", src_len, src_len);
	env_set_hex("filesize", src_len);

	return 0;
}

U_BOOT_CMD(
	unzip,	4,	1,	do_unzip,
	"unzip a memory region",
	"srcaddr dstaddr [dstsize]"
);

static int do_gzwrite(struct cmd_tbl *cmdtp, int flag,
		      int argc, char *const argv[])
{
	struct blk_desc *bdev;
	int ret;
	unsigned char *addr;
	unsigned long length;
	unsigned long writebuf = 1<<20;
	u64 startoffs = 0;
	u64 szexpected = 0;

	if (argc < 5)
		return CMD_RET_USAGE;
	ret = blk_get_device_by_str(argv[1], argv[2], &bdev);
	if (ret < 0)
		return CMD_RET_FAILURE;

	addr = (unsigned char *)hextoul(argv[3], NULL);
	length = hextoul(argv[4], NULL);

	if (5 < argc) {
		writebuf = hextoul(argv[5], NULL);
		if (6 < argc) {
			startoffs = simple_strtoull(argv[6], NULL, 16);
			if (7 < argc)
				szexpected = simple_strtoull(argv[7],
							     NULL, 16);
		}
	}

	ret = gzwrite(addr, length, bdev, writebuf, startoffs, szexpected);

	return ret ? CMD_RET_FAILURE : CMD_RET_SUCCESS;
}

U_BOOT_CMD(
	gzwrite, 8, 0, do_gzwrite,
	"unzip and write memory to block device",
	"<interface> <dev> <addr> length [wbuf=1M [offs=0 [outsize=0]]]\n"
	"\twbuf is the size in bytes (hex) of write buffer\n"
	"\t\tand should be padded to erase size for SSDs\n"
	"\toffs is the output start offset in bytes (hex)\n"
	"\toutsize is the size of the expected output (hex bytes)\n"
	"\t\tand is required for files with uncompressed lengths\n"
	"\t\t4 GiB or larger\n"
);
