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
 * \file rdsdump.c
 * \brief Dumps received RDS data.
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This program dumps all received data from a RDS stream.
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
#include "print.h"


static void callback(rds_program_t *new, rds_program_t *old)
{
	uint8_t agtc, agtv;

	rds_program_save(new);

	/* only report for current program, not EON programs */
	if (rds_program_current != new)
		return;
//	if (memcmp(&new->af.af, &old->af.af, new->af.af[0][0] - 224 + 1) != 0)
//		af_print();
	if (memcmp(&new->ct, &old->ct, sizeof(new->ct)) != 0)
		ct_print();
	if (new->di_st != old->di_st)
		di_st_print();
	if (new->di_ah != old->di_ah)
		di_ah_print();
	if (new->di_co != old->di_co)
		di_co_print();
	if (new->di_dp != old->di_dp)
		di_dp_print();
	if (new->ecc != old->ecc)
		ecc_print();
	if (memcmp(&new->eon_li, &old->eon_li, sizeof(rds_eon_li_t)) != 0)
		eon_print();
	if (memcmp(&new->eon_mf, &old->eon_mf, sizeof(rds_eon_mf_t)) != 0)
		eon_tn_mf_print();
	if (memcmp(&new->oda_ert, &old->oda_ert, sizeof(new->oda_ert)) != 0)
		ert_print();
	if (new->lic != old->lic) {
		lic_print();
		/* here we have the chance to change the RDS language database */
		/* rds_db_lang_open("de_DE"); */
	}
	if (new->ms != old->ms)
		ms_print();
	for (agtc = 0; agtc < 16; agtc++)
		for (agtv = 0; agtv < 2; agtv++)
			if (new->oda[agtc][agtv] != old->oda[agtc][agtv])
				oda_print(agtc, agtv, new->oda[agtc][agtv]);
	if (new->pi != old->pi)
		pi_print();
	if (memcmp(&new->pin, &old->pin, sizeof(new->pin)) != 0)
		pin_print();
	if (memcmp(&new->ps, &old->ps, sizeof(new->ps)) != 0)
		ps_print();
	if (new->pty != old->pty)
		pty_print();
	if (memcmp(&new->ptyn, &old->ptyn, sizeof(new->ptyn)) != 0)
		ptyn_print();
	if (memcmp(&new->rt, &old->rt, sizeof(new->rt)) != 0)
		rt_print();
	if (memcmp(&new->oda_rtp, &old->oda_rtp, sizeof(new->oda_rtp)) != 0) {
		rtp_print(new->oda_rtp_ct[0]);
		rtp_print(new->oda_rtp_ct[1]);
	}
	if ((new->tp != old->tp) || (new->ta != old->ta))
		tp_ta_print();

	if (rds_decode_status != 0)
		printf("RDS decode status: %i\n", rds_decode_status);
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
	if ((filter == -1) || ((argc - optind) != 1))
		usage = 1;
	if (usage == 1) {
		printf("Usage: %s [-f filter] filename\n", argv[0]);
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

	/* set callbacks */
	rds_callback = &callback;
	rds_ih_callback = &ih_print;
	rds_tdc_callback = &tdc_print;

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

	return EXIT_SUCCESS;
}
