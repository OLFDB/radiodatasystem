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
 * \file rds_test.c
 * \brief Radio Data System functions - tests
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains tests for Radio Data System handling.
 */

#include <check.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "rds_private.h"


/**
 * \brief Test of \ref rds_decode function
 *
 * This tests the \ref rds_decode function.
 */
START_TEST(test_rds_decode)
{
#if 0
	rds_decode(0xd000, ( 0<<12) | (0<<11), 0     , 0); /* TYPE A */
	rds_decode(0xd000, ( 0<<12) | (1<<11), 0xd000, 0); /* TYPE B */

	rds_decode(0xd000, ( 1<<12) | (0<<11), 0     , 0); /* TYPE A */
	rds_decode(0xd000, ( 1<<12) | (1<<11), 0xd000, 0); /* TYPE B */

	rds_decode(0xd000, ( 2<<12) | (0<<11), 0     , 0); /* TYPE A */
	rds_decode(0xd000, ( 2<<12) | (1<<11), 0xd000, 0); /* TYPE B */

	rds_decode(0xd000, ( 3<<12) | (0<<11), 0     , 0); /* TYPE A */
	rds_decode(0xd000, ( 3<<12) | (1<<11), 0xd000, 0); /* TYPE B */

	rds_decode(0xd000, ( 4<<12) | (0<<11), 0     , 0); /* TYPE A */
	rds_decode(0xd000, ( 4<<12) | (1<<11), 0xd000, 0); /* TYPE B */

	rds_decode(0xd000, ( 5<<12) | (0<<11), 0     , 0); /* TYPE A */
	rds_decode(0xd000, ( 5<<12) | (1<<11), 0xd000, 0); /* TYPE B */

	rds_decode(0xd000, ( 6<<12) | (0<<11), 0     , 0); /* TYPE A */
	rds_decode(0xd000, ( 6<<12) | (1<<11), 0xd000, 0); /* TYPE B */

	rds_decode(0xd000, ( 7<<12) | (0<<11), 0     , 0); /* TYPE A */
	rds_decode(0xd000, ( 7<<12) | (1<<11), 0xd000, 0); /* TYPE B */

	rds_decode(0xd000, ( 8<<12) | (0<<11), 0     , 0); /* TYPE A */
	rds_decode(0xd000, ( 8<<12) | (1<<11), 0xd000, 0); /* TYPE B */

	rds_decode(0xd000, ( 9<<12) | (0<<11), 0     , 0); /* TYPE A */
	rds_decode(0xd000, ( 9<<12) | (1<<11), 0xd000, 0); /* TYPE B */

	rds_decode(0xd000, (10<<12) | (0<<11), 0     , 0); /* TYPE A */
	rds_decode(0xd000, (10<<12) | (1<<11), 0xd000, 0); /* TYPE B */

	rds_decode(0xd000, (11<<12) | (0<<11), 0     , 0); /* TYPE A */
	rds_decode(0xd000, (11<<12) | (1<<11), 0xd000, 0); /* TYPE B */

	rds_decode(0xd000, (12<<12) | (0<<11), 0     , 0); /* TYPE A */
	rds_decode(0xd000, (12<<12) | (1<<11), 0xd000, 0); /* TYPE B */

	rds_decode(0xd000, (13<<12) | (0<<11), 0     , 0); /* TYPE A */
	rds_decode(0xd000, (13<<12) | (1<<11), 0xd000, 0); /* TYPE B */

	rds_decode(0xd000, (14<<12) | (0<<11), 0     , 0); /* TYPE A */
	rds_decode(0xd000, (14<<12) | (1<<11), 0xd000, 0); /* TYPE B */

	rds_decode(0xd000, (15<<12) | (0<<11), 0     , 0); /* TYPE A */
	rds_decode(0xd000, (15<<12) | (1<<11), 0xd000, 0); /* TYPE B */
#endif
}
END_TEST


/**
 * \brief Test suite for RDS
 *
 * This is the test suite for RDS.
 * It adds all tests into the test suite.
 */
Suite *rds_suite(void)
{
	Suite *s = suite_create("RDS");

	/* Core test case */
	TCase *tc_core = tcase_create("Core");
	tcase_add_test(tc_core, test_rds_decode);
	suite_add_tcase(s, tc_core);

	return s;
}


/**
 * \brief Main function for RDS test suite.
 *
 * This is the main function for RDS.
 * It initializes all necessary variables.
 */
int main(void)
{
	int number_failed;
	rds_program_private_t rs;
	rds_program_current = (rds_program_t *) &rs;
	rds_program_current->pi = 0xd000;
	sqlite3_open("../db_lang/en_CEN.db", &rds_db_lang);
	Suite *s = rds_suite();
	SRunner *sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
