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
 * \file oda_test.c
 * \brief Open Data Application functions - tests
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains tests for Open Data Application handling.
 */

#include <check.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "oda.h"
#include "rds_private.h"


/**
 * \brief Test of \ref rds_oda_decode_assign function
 *
 * This tests the \ref rds_oda_decode_assign function.
 */
START_TEST(test_oda_decode_assign)
{
	/* oda assignments */
	rds_oda_decode_assign(0, 0, 0, 0);		// no assignment
	rds_oda_decode_assign(15, 1, 0, 0);		// temporary data fault
	rds_oda_decode_assign(11, 0, 0x1234, 0xABCD);	// 11A msg=0x1234 aid=0xABCD
	rds_oda_decode_assign(11, 1, 0x5678, 0xABCD);	// 11A msg=0x5678 aid=0xABCD
	fail_unless(rds_program_current->oda[11][0] == 0xABCD);
	fail_unless(rds_program_current->oda[11][1] == 0xABCD);

	/** \note specific oda assignments should be tested in the corresponding tests */
}
END_TEST


/**
 * \brief Test of \ref rds_oda_decode_a function
 *
 * This tests the \ref rds_oda_decode_a function.
 */
START_TEST(test_oda_decode_a)
{
	rds_oda_decode_a(11, 0x12, 0x3456, 0x789A);

	/** \note specific oda decodes should be tested in the corresponding tests */
}
END_TEST


/**
 * \brief Test of \ref rds_oda_decode_b function
 *
 * This tests the \ref rds_oda_decode_b function.
 */
START_TEST(test_oda_decode_b)
{
	rds_oda_decode_b(11, 0x12, 0x789A);

	/** \note specific oda decodes should be tested in the corresponding tests */
}
END_TEST


/**
 * \brief Test suite for ODA
 *
 * This is the test suite for ODA.
 * It adds all tests into the test suite.
 */
Suite *oda_suite(void)
{
	Suite *s = suite_create("ODA");

	/* Core test case */
	TCase *tc_core = tcase_create("Core");
	tcase_add_test(tc_core, test_oda_decode_assign);
	tcase_add_test(tc_core, test_oda_decode_a);
	tcase_add_test(tc_core, test_oda_decode_b);
	suite_add_tcase(s, tc_core);

	return s;
}


/**
 * \brief Main function for ODA test suite.
 *
 * This is the main function for ODA.
 * It initializes all necessary variables.
 */
int main(void)
{
	int number_failed;
	rds_program_private_t rs;
	rds_program_current = (rds_program_t *) &rs;
	Suite *s = oda_suite();
	SRunner *sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
