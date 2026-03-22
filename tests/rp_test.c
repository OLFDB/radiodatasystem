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
 * \file rp_test.c
 * \brief Radio Paging functions - tests
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains tests for Radio Paging handling.
 */

#include <check.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "rds_private.h"
#include "rp.h"


/**
 * \brief Test of \ref rds_rp_decode function
 *
 * This tests the \ref rds_rp_decode function.
 */
START_TEST(test_rp_decode)
{
	rds_rp_decode(0x12, 0x34, 0x5678, 0x9abc);
}
END_TEST


/**
 * \brief Test of \ref rds_rp_decode_ident function
 *
 * This tests the \ref rds_rp_decode_ident function.
 */
START_TEST(test_rp_decode_ident)
{
	rds_rp_decode_ident(0x1234);
}
END_TEST


/**
 * \brief Test of \ref rds_rp_decode_rpc function
 *
 * This tests the \ref rds_rp_decode_rpc function.
 */
START_TEST(test_rp_decode_rpc)
{
	rds_rp_decode_rpc(0x12);
}
END_TEST


/**
 * \brief Test suite for RP
 *
 * This is the test suite for RP.
 * It adds all tests into the test suite.
 */
Suite *rp_suite(void)
{
	Suite *s = suite_create("RP");

	/* Core test case */
	TCase *tc_core = tcase_create("Core");
	tcase_add_test(tc_core, test_rp_decode);
	tcase_add_test(tc_core, test_rp_decode_ident);
	tcase_add_test(tc_core, test_rp_decode_rpc);
	suite_add_tcase(s, tc_core);

	return s;
}


/**
 * \brief Main function for RP test suite.
 *
 * This is the main function for RP.
 * It initializes all necessary variables.
 */
int main(void)
{
	int number_failed;
	rds_program_private_t rs;
	rds_program_current = (rds_program_t *) &rs;
	Suite *s = rp_suite();
	SRunner *sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
