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
 * \file oda_ert_test.c
 * \brief Enhanced Radio Text functions - tests
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains tests for Enhanced Radio Text handling.
 */

#include <check.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "rds_private.h"
#include "oda_ert.h"


/**
 * \brief Test of \ref rds_oda_decode_assign and \ref rds_oda_decode_a functions specific to ERT
 *
 * This tests the \ref rds_oda_decode_assign and \ref rds_oda_decode_a functions specific to ERT.
 */
START_TEST(test_ert_utf16)
{
	/* rds_oda_decode_assign */
	rds_oda_decode_assign(11, 0, 0, 0x6552);
	fail_unless(rds_program_current->oda[11][0] == 0x6552);
//	fail_unless(rds_program_current->oda_ert_ctid == 0);
//	fail_unless(rds_program_current->oda_ert_dtfd == 0);
//	fail_unless(rds_program_current->oda_ert_utf8 == 0);

	/* rds_oda_decode_a UTF-16 */
	rds_oda_decode_a(11, 0, 0x3000, 0x3100);
//	fail_unless(rds_program_current->oda_ert_str[0] == 0x30);
//	fail_unless(rds_program_current->oda_ert_str[1] == 0x00);
//	fail_unless(rds_program_current->oda_ert_str[2] == 0x31);
//	fail_unless(rds_program_current->oda_ert_str[3] == 0x00);
	fail_unless(rds_program_current->oda_ert[0] == 0x0030);
	fail_unless(rds_program_current->oda_ert[1] == 0x0031);
}
END_TEST


/**
 * \brief Test of \ref rds_oda_decode_assign and \ref rds_oda_decode_a functions specific to ERT
 *
 * This tests the \ref rds_oda_decode_assign and \ref rds_oda_decode_a functions specific to ERT.
 */
START_TEST(test_ert_utf8)
{
	/* rds_oda_decode_assign */
	rds_oda_decode_assign(11, 0, (1<<6) | 1, 0x6552); // (1<<6) is only to trigger rfu
	fail_unless(rds_decode_status == RDS_DECODE_STATUS_NOT_ASSIGNED);
	fail_unless(rds_program_current->oda[11][0] == 0x6552);
//	fail_unless(rds_program_current->oda_ert_ctid == 0);
//	fail_unless(rds_program_current->oda_ert_dtfd == 0);
//	fail_unless(rds_program_current->oda_ert_utf8 == 0);

	/* rds_oda_decode_a UTF-8 */
	rds_oda_decode_a(11, 0, 0x3031, 0x3233);
//	fail_unless(rds_program_current->oda_ert_str[0] == 0x30);
//	fail_unless(rds_program_current->oda_ert_str[1] == 0x31);
//	fail_unless(rds_program_current->oda_ert_str[2] == 0x32);
//	fail_unless(rds_program_current->oda_ert_str[3] == 0x33);
	fail_unless(rds_program_current->oda_ert[0] == 0x0030);
	fail_unless(rds_program_current->oda_ert[1] == 0x0031);
	fail_unless(rds_program_current->oda_ert[2] == 0x0032);
	fail_unless(rds_program_current->oda_ert[3] == 0x0033);
}
END_TEST


/**
 * \brief Test suite for ODA ERT
 *
 * This is the test suite for ODA ERT.
 * It adds all tests into the test suite.
 */
Suite *ert_suite(void)
{
	Suite *s = suite_create("ODA.ERT");

	/* Core test case */
	TCase *tc_core = tcase_create("Core");
	tcase_add_test(tc_core, test_ert_utf16);
	tcase_add_test(tc_core, test_ert_utf8);
	suite_add_tcase(s, tc_core);

	return s;
}


/**
 * \brief Test suite for ODA ERT
 *
 * This is the test suite for ODA ERT.
 * It adds all tests into the test suite.
 */
int main(void)
{
	int number_failed;
	rds_program_private_t rs;
	rds_program_current = (rds_program_t *) &rs;
	Suite *s = ert_suite();
	SRunner *sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
