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
 * \file oda_irds_test.c
 * \brief In-Receiver Database System functions - tests
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains tests for In-Receiver Database System handling.
 */

#include <check.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "rds_private.h"
#include "oda_irds.h"


/**
 * \brief Test of \ref rds_oda_decode_assign function specific to IRDS
 *
 * This tests the \ref rds_oda_decode_assign function specific to IRDS.
 */
START_TEST(test_irds_decode_assign)
{
	rds_oda_decode_assign(11, 0, 0x1234, 0xc563);
}
END_TEST


/**
 * \brief Test of \ref rds_oda_decode_a function specific to IRDS
 *
 * This tests the \ref rds_oda_decode_a function specific to IRDS.
 */
START_TEST(test_irds_decode_a)
{
	uint8_t ac;

	for (ac = 0; ac < 32; ac++)
		rds_oda_decode_a(11, ac, 0x1234, 0x5678);
}
END_TEST


/**
 * \brief Test suite for ODA IRDS
 *
 * This is the test suite for ODA IRDS.
 * It adds all tests into the test suite.
 */
Suite *irds_suite(void)
{
	Suite *s = suite_create("ODA.IRDS");

	/* Core test case */
	TCase *tc_core = tcase_create("Core");
	tcase_add_test(tc_core, test_irds_decode_assign);
	tcase_add_test(tc_core, test_irds_decode_a);
	suite_add_tcase(s, tc_core);

	return s;
}


/**
 * \brief Main function for ODA IRDS test suite.
 *
 * This is the main function for ODA IRDS.
 * It initializes all necessary variables.
 */
int main(void)
{
	int number_failed;
	rds_program_private_t rs;
	rds_program_current = (rds_program_t *) &rs;
	Suite *s = irds_suite();
	SRunner *sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
