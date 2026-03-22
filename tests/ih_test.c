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
 * \file ih_test.c
 * \brief In-House application functions - tests
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains tests for In-House application handling.
 */

#include <check.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "ih.h"
#include "rds_private.h"


/**
 * \brief Test of \ref rds_ih_decode_a function
 *
 * This tests the \ref rds_ih_decode_a function.
 */
START_TEST(test_ih_decode_a)
{
	rds_ih_decode_a(0x12, 0x3456, 0x789a);
}
END_TEST


/**
 * \brief Test of \ref rds_ih_decode_b function
 *
 * This tests the \ref rds_ih_decode_b function.
 */
START_TEST(test_ih_decode_b)
{
	rds_ih_decode_b(0x12, 0x789a);
}
END_TEST


/**
 * \brief Test suite for IH
 *
 * This is the test suite for IH.
 * It adds all tests into the test suite.
 */
Suite *ih_suite(void)
{
	Suite *s = suite_create("IH");

	/* Core test case */
	TCase *tc_core = tcase_create("Core");
	tcase_add_test(tc_core, test_ih_decode_a);
	tcase_add_test(tc_core, test_ih_decode_b);
	suite_add_tcase(s, tc_core);

	return s;
}


/**
 * \brief Main function for IH test suite.
 *
 * This is the main function for IH.
 * It initializes all necessary variables.
 */
int main(void)
{
	int number_failed;
	rds_program_private_t rs;
	rds_program_current = (rds_program_t *) &rs;
	Suite *s = ih_suite();
	SRunner *sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
