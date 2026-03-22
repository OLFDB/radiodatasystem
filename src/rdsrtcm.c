/*
 * Copyright © 2009-2011 Tobias Lorenz
 *
 * This file is part of librds.
 *
 * librds is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * librds is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with librds.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * \file rdsrtcm.c
 * \brief Dumps received RTCM data.
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This program dumps all received RTCM data from a RDS stream.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "rds.h"
#include "filters.h"
#include "oda_rasant.h"
#include "print.h"


FILE *file;

/* Rasant RTCM binary output and suitable for gpsd (75% done, untested) */

static void rasant_print(rds_program_t *rs, uint8_t data)
{
	fwrite(&data, 1, 1, file);
}


int main(int argc, char *argv[])
{
	FILE *fd;
	int opt;
	int filter = 0;
	int usage  = 0;
	int retval = 0;

	while ((opt = getopt(argc, argv, "f:")) != -1) {
		switch (opt) {
		case 'f':
			if (strcmp(optarg, "v4l") == 0) {
				filter = 0;
			} else
			if (strcmp(optarg, "raw") == 0) {
				filter = 1;
			} else
			if (strcmp(optarg, "csv") == 0) {
				filter = 2;
			} else
			if (strcmp(optarg, "smp") == 0) {
				filter = 3;
			} else
				filter = -1;
			break;
		default: /* '?' */
			usage = 1;
		}
	}

	/* safety checks */
	if ((filter == -1) || ((argc - optind) != 2))
		usage = 1;
	if (usage == 1) {
		printf("Usage: %s [-f filter] filename rtcmout\n", argv[0]);
		return EXIT_FAILURE;
	}

	/* open file */
	if (strcmp(argv[optind], "-") == 0) {
		fd = stdin;
	} else {
		fd = fopen(argv[optind], "r");
		if (fd <= 0) {
			printf("Unable to open file %s\n", argv[optind]);
			return EXIT_FAILURE;
		}
	}
	optind++;

	/* open RTCM output file */
	if (strcmp(argv[optind], "-") == 0) {
		fd = stdout;
	} else {
		fd = fopen(argv[optind], "w");
		if (fd <= 0) {
			printf("Unable to open file %s\n", argv[optind]);
			return EXIT_FAILURE;
		}
	}

	/* set callbacks */
	rds_oda_rasant_callback = &rasant_print;

	/* save RDS program on change */
	rds_program_save_config = RDS_PROGRAM_SAVE_CONFIG_ON_CHANGE;

	/* read in stream */
	do {
		switch (filter) {
		case 0:
			retval = rds_decode_v4l(fd);
			break;
		case 1:
			retval = rds_decode_raw(fd);
			break;
		case 2:
			retval = rds_decode_csv(fd);
			break;
		case 3:
			retval = rds_decode_smp(fd);
			break;
		}
	} while (retval == EXIT_SUCCESS);

	/* close file */
	(void) fclose(fd);

	/* save all RDS programs */
	rds_program_save_all();

	/* close RTCM output file */
	(void) fclose(file);

	return EXIT_SUCCESS;
}
