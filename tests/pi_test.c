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
 * \file pi_test.c
 * \brief Programme Identification functions - tests
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains tests for Programme Identification handling.
 */

#include <check.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "pi.h"
#include "rds_private.h"


/**
 * \brief Test of \ref rds_pi_decode function
 *
 * This tests the \ref rds_pi_decode function.
 */
START_TEST(test_pi_decode)
{
	/* Reflexion 166 - Beispiele - Vierte Ziffer */
	rds_pi_decode(0xD361);	/* hr 1 */
	fail_unless(rds_program_current->pi == 0xd361);
	rds_pi_decode(0xD3A8);	/* RPR Eins */
	fail_unless(rds_program_current->pi == 0xd3a8);
	rds_pi_decode(0x1092);	/* R. Essen */
	fail_unless(rds_program_current->pi == 0x1092);

	/* Reflexion 166 - Beispiele - Zweite Ziffer */
	rds_pi_decode(0x100A);	/* Stadtradio Stuttgart */
	fail_unless(rds_program_current->pi == 0x100a);
	rds_pi_decode(0xD210);	/* DLF */
	fail_unless(rds_program_current->pi == 0xd210);
	rds_pi_decode(0xD301);	/* SWR1 BW */
	fail_unless(rds_program_current->pi == 0xd301);
	rds_pi_decode(0xDC04);	/* SWR4 MA */
	fail_unless(rds_program_current->pi == 0xdc04);

	/* Reflexion 166 - Beispiele - Dritte Ziffer */
	rds_pi_decode(0xD313);	/* Bayern 3 */
	fail_unless(rds_program_current->pi == 0xd313);
	rds_pi_decode(0xD368);	/* FFH */
	fail_unless(rds_program_current->pi == 0xd368);
	rds_pi_decode(0xD3A1);	/* SWR1 RP */
	fail_unless(rds_program_current->pi == 0xd3a1);
	rds_pi_decode(0xD220);	/* DRadio */
	fail_unless(rds_program_current->pi == 0xd220);

	/* Reflexion 166 - Beispiele - Vierte Ziffer */
	rds_pi_decode(0xD3A3);	/* SWR3 */
	fail_unless(rds_program_current->pi == 0xd3a3);
	rds_pi_decode(0xD3A5);	/* DASDING */
	fail_unless(rds_program_current->pi == 0xd3a5);
	rds_pi_decode(0xD388);	/* FFN */
	fail_unless(rds_program_current->pi == 0xd388);
	rds_pi_decode(0xD3A9);	/* RPR Zwei */
	fail_unless(rds_program_current->pi == 0xd3a9);
	rds_pi_decode(0xD3AA);	/* Rockland R. */
	fail_unless(rds_program_current->pi == 0xd3aa);
}
END_TEST


/**
 * \brief Test of \ref rds_pi_cc function
 *
 * This tests the \ref rds_pi_cc function.
 */
START_TEST(test_pi_cc)
{
	uint8_t cc;

	/* simple test of rds_pi_cc */
	for (cc = 0; cc < 0x10; cc++)
		fail_unless(rds_pi_cc(cc << 12) == cc);
}
END_TEST


/**
 * \brief Test suite for PI
 *
 * This is the test suite for PI.
 * It adds all tests into the test suite.
 */
Suite *pi_suite(void)
{
	Suite *s = suite_create("PI");

	/* Core test case */
	TCase *tc_core = tcase_create("Core");
	tcase_add_test(tc_core, test_pi_decode);
	tcase_add_test(tc_core, test_pi_cc);
	suite_add_tcase(s, tc_core);

	return s;
}


/**
 * \brief Main function for PI test suite.
 *
 * This is the main function for PI.
 * It initializes all necessary variables.
 */
int main(void)
{
	int number_failed;
	rds_program_private_t rs;
	rds_program_current = (rds_program_t *) &rs;
	sqlite3_open("../db_lang/en_CEN.db", &rds_db_lang);
	Suite *s = pi_suite();
	SRunner *sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
