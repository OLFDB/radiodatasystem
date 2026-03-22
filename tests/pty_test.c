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
 * \file pty_test.c
 * \brief Programme Type code functions - tests
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains tests for Programme Type code handling.
 */

#include <check.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "pty.h"
#include "rds_private.h"


/**
 * \brief Test of \ref rds_pty_decode function
 *
 * This tests the \ref rds_pty_decode function.
 */
START_TEST(test_pty_decode)
{
	rds_pty_decode(0);
	fail_unless(rds_program_current->pty == 0);
	rds_pty_decode(1);
	fail_unless(rds_program_current->pty == 1);
}
END_TEST


/**
 * \brief Test of \ref rds_pty_get_str function
 *
 * This tests the \ref rds_pty_get_str function.
 */
START_TEST(test_pty_get_str)
{
	char s[60];

	/* RDS: 1;Nachrichtendienst;Nachrich;Nachrichten */
	memset(&s, 0, sizeof(s));
	rds_pty_get_str(&s[0], 8, 1, 0);
	fail_unless(strncmp(&s[0], "News", 8) == 0);
	rds_pty_get_str(&s[0], 16, 1, 0);
	fail_unless(strncmp(&s[0], "News", 16) == 0);
	rds_pty_get_str(&s[0], sizeof(s), 1, 0);
	fail_unless(strncmp(&s[0], "News", sizeof(s)) == 0);

	/* RBDS: 1;News;News;News */
	memset(&s, 0, sizeof(s));
	rds_pty_get_str(&s[0], 8, 1, 1);
	fail_unless(strcmp(&s[0], "News") == 0);
	rds_pty_get_str(&s[0], 16, 1, 1);
	fail_unless(strcmp(&s[0], "News") == 0);
	rds_pty_get_str(&s[0], sizeof(s), 1, 1);
	fail_unless(strcmp(&s[0], "News") == 0);

	/* these should not exists */
	rds_pty_get_str(&s[0], sizeof(s), 32, 0);
	rds_pty_get_str(&s[0], sizeof(s), 0, 2);
	rds_pty_get_str(&s[0], sizeof(s), 32, 2);
}
END_TEST


/**
 * \brief Test suite for PTY
 *
 * This is the test suite for PTY.
 * It adds all tests into the test suite.
 */
Suite *pty_suite(void)
{
	Suite *s = suite_create("PTY");

	/* Core test case */
	TCase *tc_core = tcase_create("Core");
	tcase_add_test(tc_core, test_pty_decode);
	tcase_add_test(tc_core, test_pty_get_str);
	suite_add_tcase(s, tc_core);

	return s;
}


/**
 * \brief Main function for PTY test suite.
 *
 * This is the main function for PTY.
 * It initializes all necessary variables.
 */
int main(void)
{
	int number_failed;
	rds_program_private_t rs;
	rds_program_current = (rds_program_t *) &rs;
	sqlite3_open("../db/rds_lang/rds_lang_en_CEN.db", &rds_db_lang);
	Suite *s = pty_suite();
	SRunner *sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
