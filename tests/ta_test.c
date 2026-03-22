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
 * \file ta_test.c
 * \brief Traffic Announcement functions - tests
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains tests for Traffic Announcement handling.
 */

#include <check.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "rds_private.h"
#include "ta.h"


/**
 * \brief Test of \ref rds_ta_decode function
 *
 * This tests the \ref rds_ta_decode function.
 */
START_TEST(test_ta_decode)
{
	rds_ta_decode(0);
	fail_unless(rds_program_current->ta == 0);
	rds_ta_decode(1);
	fail_unless(rds_program_current->ta == 1);
}
END_TEST


/**
 * \brief Test suite for TA
 *
 * This is the test suite for TA.
 * It adds all tests into the test suite.
 */
Suite *ta_suite(void)
{
	Suite *s = suite_create("TA");

	/* Core test case */
	TCase *tc_core = tcase_create("Core");
	tcase_add_test(tc_core, test_ta_decode);
	suite_add_tcase(s, tc_core);

	return s;
}


/**
 * \brief Main function for TA test suite.
 *
 * This is the main function for TA.
 * It initializes all necessary variables.
 */
int main(void)
{
	int number_failed;
	rds_program_private_t rs;
	rds_program_current = (rds_program_t *) &rs;
	Suite *s = ta_suite();
	SRunner *sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
