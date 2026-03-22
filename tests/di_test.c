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
 * \file di_test.c
 * \brief Decoder Identification functions - tests
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains tests for Decoder Identification handling.
 */

#include <check.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "di.h"
#include "rds_private.h"


/**
 * \brief Test of \ref rds_di_decode function
 *
 * This tests the \ref rds_di_decode function.
 */
START_TEST(test_di_decode)
{
	rds_di_decode(0, 0);
	fail_unless(rds_program_current->di_st == 0);
	rds_di_decode(0, 1);
	fail_unless(rds_program_current->di_st == 1);

	rds_di_decode(1, 0);
	fail_unless(rds_program_current->di_ah == 0);
	rds_di_decode(1, 1);
	fail_unless(rds_program_current->di_ah == 1);

	rds_di_decode(2, 0);
	fail_unless(rds_program_current->di_co == 0);
	rds_di_decode(2, 1);
	fail_unless(rds_program_current->di_co == 1);

	rds_di_decode(3, 0);
	fail_unless(rds_program_current->di_dp == 0);
	rds_di_decode(3, 1);
	fail_unless(rds_program_current->di_dp == 1);
}
END_TEST


/**
 * \brief Test suite for DI
 *
 * This is the test suite for DI.
 * It adds all tests into the test suite.
 */
Suite *di_suite(void)
{
	Suite *s = suite_create("DI");

	/* Core test case */
	TCase *tc_core = tcase_create("Core");
	tcase_add_test(tc_core, test_di_decode);
	suite_add_tcase(s, tc_core);

	return s;
}


/**
 * \brief Main function for DI test suite.
 *
 * This is the main function for DI.
 * It initializes all necessary variables.
 */
int main(void)
{
	int number_failed;
	rds_program_private_t rs;
	rds_program_current = (rds_program_t *) &rs;
	Suite *s = di_suite();
	SRunner *sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
