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
 * \file ecc_test.c
 * \brief Extended Country Code functions - tests
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains tests for Extended Country Code handling.
 */

#include <check.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "ecc.h"
#include "pi.h"
#include "rds_private.h"


/**
 * \brief Test of \ref rds_ecc_decode function
 *
 * This tests the \ref rds_ecc_decode function.
 */
START_TEST(test_ecc_decode)
{
	char *s;

	rds_pi_decode(0xd3a3);
	rds_ecc_decode(0xE0);
	rds_pi_decode(0xd3a3);
	rds_ecc_decode(0xE0);

	fail_unless(rds_program_current->ecc == 0xE0);
	fail_unless(strncmp(rds_program_current->iso, "DE", 2) == 0);
	fail_unless(rds_program_current->itu == 1);
	fail_unless(rds_program_current->oda_tmc_cid == 58);
}
END_TEST


/**
 * \brief Test of \ref rds_ecc_get_name function
 *
 * This tests the \ref rds_ecc_get_name function.
 */
START_TEST(test_ecc_get_name)
{
	char buf[128];

	rds_ecc_get_name(&buf[0], sizeof(buf), 0xe0, 0xd);
	fail_unless(strncmp(&buf[0], "Germany", sizeof(buf)) == 0);
}
END_TEST


/**
 * \brief Test suite for ECC
 *
 * This is the test suite for ECC.
 * It adds all tests into the test suite.
 */
Suite *ecc_suite(void)
{
	Suite *s = suite_create("ECC");

	/* Core test case */
	TCase *tc_core = tcase_create("Core");
	tcase_add_test(tc_core, test_ecc_decode);
	tcase_add_test(tc_core, test_ecc_get_name);
	suite_add_tcase(s, tc_core);

	return s;
}


/**
 * \brief Main function for ECC test suite.
 *
 * This is the main function for ECC.
 * It initializes all necessary variables.
 */
int main(void)
{
	int number_failed;
	rds_program_private_t rs;
	rds_program_current = (rds_program_t *) &rs;
	sqlite3_open("../db/rds_lang/rds_lang_en_CEN.db", &rds_db_lang);
	Suite *s = ecc_suite();
	SRunner *sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
