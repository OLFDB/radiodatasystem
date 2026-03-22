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
 * \file rt_test.c
 * \brief Radio Text functions - tests
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains tests for Radio Text handling.
 */

#include <check.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "rds_private.h"
#include "rt.h"


/**
 * \brief Test of \ref rds_rt_decode_a function
 *
 * This tests the \ref rds_rt_decode_a function.
 */
START_TEST(test_rt_decode_a)
{
	/* clear */
	rds_rt_decode_a(1, 0, 0, 0, 0, 0);
	rds_rt_decode_a(0, 0, 0, 0, 0, 0);

	/* set */
	rds_rt_decode_a(0, 0, 0x31, 0x32, 0x33, 0x34);
	rds_rt_decode_a(0, 1, 0x35, 0x36, 0x37, 0x38);

	/* check */
	fail_unless(rds_program_current->rt[0] == 0x0031);
	fail_unless(rds_program_current->rt[1] == 0x0032);
	fail_unless(rds_program_current->rt[2] == 0x0033);
	fail_unless(rds_program_current->rt[3] == 0x0034);
	fail_unless(rds_program_current->rt[4] == 0x0035);
	fail_unless(rds_program_current->rt[5] == 0x0036);
	fail_unless(rds_program_current->rt[6] == 0x0037);
	fail_unless(rds_program_current->rt[7] == 0x0038);
}
END_TEST


/**
 * \brief Test of \ref rds_rt_decode_b function
 *
 * This tests the \ref rds_rt_decode_b function.
 */
START_TEST(test_rt_decode_b)
{
	/* clear */
	rds_rt_decode_b(1, 0, 0x3a, 0x3b);

	/* check */
	fail_unless(rds_program_current->rt[0] == 0x003a);
	fail_unless(rds_program_current->rt[1] == 0x003b);
	fail_unless(rds_program_current->rt[2] == 0x0000);
	fail_unless(rds_program_current->rt[3] == 0x0000);
	fail_unless(rds_program_current->rt[4] == 0x0000);
	fail_unless(rds_program_current->rt[5] == 0x0000);
	fail_unless(rds_program_current->rt[6] == 0x0000);
	fail_unless(rds_program_current->rt[7] == 0x0000);
}
END_TEST


/**
 * \brief Test suite for RT
 *
 * This is the test suite for RT.
 * It adds all tests into the test suite.
 */
Suite *rt_suite(void)
{
	Suite *s = suite_create("RT");

	/* Core test case */
	TCase *tc_core = tcase_create("Core");
	tcase_add_test(tc_core, test_rt_decode_a);
	tcase_add_test(tc_core, test_rt_decode_b);
	suite_add_tcase(s, tc_core);

	return s;
}


/**
 * \brief Main function for RT test suite.
 *
 * This is the main function for RT.
 * It initializes all necessary variables.
 */
int main(void)
{
	int number_failed;
	rds_program_private_t rs;
	rds_program_current = (rds_program_t *) &rs;
	Suite *s = rt_suite();
	SRunner *sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
