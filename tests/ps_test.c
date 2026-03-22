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
 * \file ps_test.c
 * \brief Programme Service name functions - tests
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains tests for Programme Service name handling.
 */

#include <check.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "ps.h"
#include "rds_private.h"


/**
 * \brief Test of \ref rds_ps_decode function
 *
 * This tests the \ref rds_ps_decode function.
 */
START_TEST(test_ps_decode)
{
	rds_ps_decode(0, 0x31, 0x32);
	rds_ps_decode(1, 0x33, 0x34);
	rds_ps_decode(2, 0x35, 0x36);
	rds_ps_decode(3, 0x37, 0x38);

	fail_unless(rds_program_current->ps[0] == 0x0031);
	fail_unless(rds_program_current->ps[1] == 0x0032);
	fail_unless(rds_program_current->ps[2] == 0x0033);
	fail_unless(rds_program_current->ps[3] == 0x0034);
	fail_unless(rds_program_current->ps[4] == 0x0035);
	fail_unless(rds_program_current->ps[5] == 0x0036);
	fail_unless(rds_program_current->ps[6] == 0x0037);
	fail_unless(rds_program_current->ps[7] == 0x0038);
}
END_TEST


/**
 * \brief Test suite for PS
 *
 * This is the test suite for PS.
 * It adds all tests into the test suite.
 */
Suite *ps_suite(void)
{
	Suite *s = suite_create("PS");

	/* Core test case */
	TCase *tc_core = tcase_create("Core");
	tcase_add_test(tc_core, test_ps_decode);
	suite_add_tcase(s, tc_core);

	return s;
}


/**
 * \brief Main function for PS test suite.
 *
 * This is the main function for PS.
 * It initializes all necessary variables.
 */
int main(void)
{
	int number_failed;
	rds_program_private_t rs;
	rds_program_current = (rds_program_t *) &rs;
	Suite *s = ps_suite();
	SRunner *sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
