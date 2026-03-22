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
 * \file lic_test.c
 * \brief Language Identification Code functions - tests
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains tests for Language Identification Code handling.
 */

#include <check.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "lic.h"
#include "rds_private.h"


/**
 * \brief Test of \ref rds_lic_decode function
 *
 * This tests the \ref rds_lic_decode function.
 */
START_TEST(test_lic_decode)
{
	uint8_t lic;

	for (lic = 0; lic < 128; lic++) {
		rds_lic_decode(lic);
		fail_unless(rds_program_current->lic == lic);
	}
}
END_TEST


/**
 * \brief Test of \ref rds_lic_get_str function
 *
 * This tests the \ref rds_lic_get_str function.
 */
START_TEST(test_lic_get_str)
{
	char s[60];

	/* 8;Deutsch */
	rds_lic_get_str(&s[0], sizeof(s), 8);
	fail_unless(strcmp(&s[0], "German") == 0);
}
END_TEST


/**
 * \brief Test suite for LIC
 *
 * This is the test suite for LIC.
 * It adds all tests into the test suite.
 */
Suite *lic_suite(void)
{
	Suite *s = suite_create("LIC");

	/* Core test case */
	TCase *tc_core = tcase_create("Core");
	tcase_add_test(tc_core, test_lic_decode);
	tcase_add_test(tc_core, test_lic_get_str);
	suite_add_tcase(s, tc_core);

	return s;
}


/**
 * \brief Main function for LIC test suite.
 *
 * This is the main function for LIC.
 * It initializes all necessary variables.
 */
int main(void)
{
	int number_failed;
	rds_program_private_t rs;
	rds_program_current = (rds_program_t *) &rs;
	sqlite3_open("../db/rds_lang/rds_lang_en_CEN.db", &rds_db_lang);
	Suite *s = lic_suite();
	SRunner *sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
