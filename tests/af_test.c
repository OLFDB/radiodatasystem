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
 * \file af_test.c
 * \brief Alternative Frequency functions - tests
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains tests for Alternative Frequency handling.
 */

#include <check.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "af.h"
#include "rds_private.h"


/**
 * \brief Test of \ref rds_af_decode function
 *
 * This tests the \ref rds_af_decode function using an implementation of IEC 62106 method A example 1.
 */
START_TEST(test_af_decode_a1)
{
	rds_program_private_t rs;
	rds_program_current = (rds_program_t *) &rs;
	memset(&rs, 0, sizeof(rs));

	/* IEC 62106 method A example 1 */
	rds_af_decode(224+5	, 1);		// #5   AF1
	rds_af_decode(2		, 3);		// AF2  AF3
	rds_af_decode(4		, 5);		// AF4  AF5
	/* repeat */
	rds_af_decode(224+5	, 1);		// #5   AF1
	rds_af_decode(2		, 3);		// AF2  AF3
	rds_af_decode(4		, 5);		// AF4  AF5

	/* check list */
	fail_unless(rds_program_current->af[0] == 229);
	fail_unless(rds_program_current->af[1] ==   1);
	fail_unless(rds_program_current->af[2] ==   2);
	fail_unless(rds_program_current->af[3] ==   3);
	fail_unless(rds_program_current->af[4] ==   4);
	fail_unless(rds_program_current->af[5] ==   5);
	fail_unless(rds_program_current->af[6] ==   0);
}
END_TEST


/**
 * \brief Test of \ref rds_af_decode function
 *
 * This tests the \ref rds_af_decode function using an implementation of IEC 62106 method A example 2.
 */
START_TEST(test_af_decode_a2)
{
	rds_program_private_t rs;
	rds_program_current = (rds_program_t *) &rs;
	memset(&rs, 0, sizeof(rs));

	/* IEC 62106 method A example 2 */
	rds_af_decode(224+4	, 1);		// #4   AF1
	rds_af_decode(2		, 3);		// AF2  AF3
	rds_af_decode(4		, 205);		// AF4  Filler
	/* repeat */
	rds_af_decode(224+4	, 1);		// #4   AF1
	rds_af_decode(2		, 3);		// AF2  AF3
	rds_af_decode(4		, 205);		// AF4  Filler

	/* check list */
	fail_unless(rds_program_current->af[0] == 228);
	fail_unless(rds_program_current->af[1] ==   1);
	fail_unless(rds_program_current->af[2] ==   2);
	fail_unless(rds_program_current->af[3] ==   3);
	fail_unless(rds_program_current->af[4] ==   4);
	fail_unless(rds_program_current->af[5] == 205);
	fail_unless(rds_program_current->af[6] ==   0);
}
END_TEST


/**
 * \brief Test of \ref rds_af_decode function
 *
 * This tests the \ref rds_af_decode function using an implementation of IEC 62106 method A example 3.
 */
START_TEST(test_af_decode_a3)
{
	rds_program_private_t rs;
	rds_program_current = (rds_program_t *) &rs;
	memset(&rs, 0, sizeof(rs));

	/* IEC 62106 method A example 3 */
	rds_af_decode(224+4	, 1);		// #4   AF1
	rds_af_decode(2		, 3);		// AF2  AF3
	rds_af_decode(250	, 1);		// LF/MF follows  AF4
	/* repeat */
	rds_af_decode(224+4	, 1);		// #4   AF1
	rds_af_decode(2		, 3);		// AF2  AF3
	rds_af_decode(250	, 1);		// LF/MF follows  AF4

	/* check list */
	fail_unless(rds_program_current->af[0] == 228);
	fail_unless(rds_program_current->af[1] ==   1);
	fail_unless(rds_program_current->af[2] ==   2);
	fail_unless(rds_program_current->af[3] ==   3);
	fail_unless(rds_program_current->af[4] == 250);
	fail_unless(rds_program_current->af[5] ==   1);
	fail_unless(rds_program_current->af[6] ==   0);
}
END_TEST


/**
 * \brief Test of \ref rds_af_decode function
 *
 * This tests the \ref rds_af_decode function using an implementation of IEC 62106 method B example 1.
 */
START_TEST(test_af_decode_b1)
{
	rds_program_private_t rs;
	rds_program_current = (rds_program_t *) &rs;
	memset(&rs, 0, sizeof(rs));

	/* IEC 62106 method B example 1 */
	rds_af_decode(224+11	, 18);		// #11     89.3 (same program = tuning frequency)
	rds_af_decode(18	, 120);		//  89.3   99.5 (same program)
	rds_af_decode(18	, 142);		//  89.3  101.7 (same program)
	rds_af_decode(13	, 18);		//  88.8   89.3 (same program)
	rds_af_decode(151	, 18);		// 102.6   89.3 (regional variant)
	rds_af_decode(18	, 15);		//  89.3   89.0 (regional variant)
	/* repeat */
	rds_af_decode(224+11	, 18);		// #11     89.3 (same program = tuning frequency)
	rds_af_decode(18	, 120);		//  89.3   99.5 (same program)
	rds_af_decode(18	, 142);		//  89.3  101.7 (same program)
	rds_af_decode(13	, 18);		//  88.8   89.3 (same program)
	rds_af_decode(151	, 18);		// 102.6   89.3 (regional variant)
	rds_af_decode(18	, 15);		//  89.3   89.0 (regional variant)

	/* check list */
	fail_unless(rds_program_current->af[ 0] == 235);
	fail_unless(rds_program_current->af[ 1] ==  18);
	fail_unless(rds_program_current->af[ 2] ==  18);
	fail_unless(rds_program_current->af[ 3] == 120);
	fail_unless(rds_program_current->af[ 4] ==  18);
	fail_unless(rds_program_current->af[ 5] == 142);
	fail_unless(rds_program_current->af[ 6] ==  13);
	fail_unless(rds_program_current->af[ 7] ==  18);
	fail_unless(rds_program_current->af[ 8] == 151);
	fail_unless(rds_program_current->af[ 9] ==  18);
	fail_unless(rds_program_current->af[10] ==  18);
	fail_unless(rds_program_current->af[11] ==  15);
	fail_unless(rds_program_current->af[12] ==   0);
}
END_TEST


/**
 * \brief Test of \ref rds_af_decode function
 *
 * This tests the \ref rds_af_decode function using an implementation of IEC 62106 method B example 2.
 */
START_TEST(test_af_decode_b2)
{
	rds_program_private_t rs;
	rds_program_current = (rds_program_t *) &rs;
	memset(&rs, 0, sizeof(rs));

	/* IEC 62106 method B example 2 */
	rds_af_decode(224+9	, 120);		// #9      99.5 (same program = tuning frequency)
	rds_af_decode(18	, 120);		//  89.3   99.5 (same program)
	rds_af_decode(120	, 134);		//  99.5  100.9 (same program)
	rds_af_decode(173	, 120);		// 104.8   99.5 (regional variant)
	rds_af_decode(120	, 16);		//  99.5   89.1 (regional variant)
	/* repeat */
	rds_af_decode(224+9	, 120);		// #9      99.5 (same program = tuning frequency)
	rds_af_decode(18	, 120);		//  89.3   99.5 (same program)
	rds_af_decode(120	, 134);		//  99.5  100.9 (same program)
	rds_af_decode(173	, 120);		// 104.8   99.5 (regional variant)
	rds_af_decode(120	, 16);		//  99.5   89.1 (regional variant)

	/* check list */
	fail_unless(rds_program_current->af[ 0] == 233);
	fail_unless(rds_program_current->af[ 1] == 120);
	fail_unless(rds_program_current->af[ 2] ==  18);
	fail_unless(rds_program_current->af[ 3] == 120);
	fail_unless(rds_program_current->af[ 4] == 120);
	fail_unless(rds_program_current->af[ 5] == 134);
	fail_unless(rds_program_current->af[ 6] == 173);
	fail_unless(rds_program_current->af[ 7] == 120);
	fail_unless(rds_program_current->af[ 8] == 120);
	fail_unless(rds_program_current->af[ 9] ==  16);
	fail_unless(rds_program_current->af[10] ==   0);
}
END_TEST


#if 0
START_TEST(test_af_decode_old)
{
	/* next test */
	rds_program_private_t rs;
	rds_program_current = (rds_program_t *) &rs;
	memset(&rs, 0, sizeof(rs));

	/* test of LF and MF frequencies */
	rds_af_decode(224+3	,  1);		// #3  AF1
	rds_af_decode(250	,  1);		// LF/MF follows  LF1
	rds_af_decode(250	, 16);		// LF/MF follows  MF1

	/* method B with no tuning freq in middle pair (error) */
	rds_af_decode(224+7	, 1);		// #7      AF1
	rds_af_decode(1		, 2);		//  AF1    AF2
	rds_af_decode(2		, 3);		//  AF2    AF3 (error)
	rds_af_decode(1		, 4);		//  AF1    AF4

	/* method B with no tuning freq in last pair (error) */
	rds_af_decode(224+7	, 1);		// #7      AF1
	rds_af_decode(1		, 2);		//  AF1    AF2
	rds_af_decode(1		, 3);		//  AF1    AF3
	rds_af_decode(2		, 4);		//  AF2    AF4 (error)

	/* IEC 62106 method B example 2 with MF/LF freqs */
	rds_af_decode(224+7	, 120);		// #7      99.5 (same program = tuning frequency)
	rds_af_decode(18	, 120);		//  89.3   99.5 (same program)
	rds_af_decode(120	, 134);		//  99.5  100.9 (same program)
	rds_af_decode(250	, 1);		// LF/MF   LF1
	rds_af_decode(250	, 16);		// LF/MF   LF2

	/* No AF exists */
	rds_af_decode(224+0	, 205);		// #0  Filler

	/* 1 AF follows */
	rds_af_decode(224+1	, 1);		// #1  AF1

	/* invalid first AF */
	rds_af_decode(224+1	, 0);		// #1  invalid

	/* invalid follow AF1 */
	rds_af_decode(224+3	, 1);		// #3       AF1
	rds_af_decode(0		, 2);		// invalid  AF2

	/* invalid follow AF2 */
	rds_af_decode(224+3	, 1);		// #3   AF1
	rds_af_decode(2		, 0);		// AF2  invalid

	/* first valid LF index */
	rds_af_decode(224+2	, 1);		// #2     AF1
	rds_af_decode(250	, 1);		// LF/MF  1

	/* last valid LF index */
	rds_af_decode(224+2	, 1);		// #2     AF1
	rds_af_decode(250	, 15);		// LF/MF  15

	/* first valid MF index */
	rds_af_decode(224+2	, 1);		// #2     AF1
	rds_af_decode(250	, 16);		// LF/MF  16

	/* last valid MF index */
	rds_af_decode(224+2	, 1);		// #2     AF1
	rds_af_decode(250	, 135);		// LF/MF  135

	/* invalid LF/MF index */
	rds_af_decode(224+2	, 1);		// #2     AF1
	rds_af_decode(250	, 136);		// LF/MF  invalid

	/* duplicate freq at AF1 */
	rds_af_decode(224+5	, 1);		// #5       AF1
	rds_af_decode(2		, 3);		// AF2      AF3
	rds_af_decode(2		, 5);		// invalid  AF5

	/* duplicate freq at AF2 */
	rds_af_decode(224+5	, 1);		// #5   AF1
	rds_af_decode(2		, 3);		// AF2  AF3
	rds_af_decode(4		, 3);		// AF4  invalid

	/* AF follow in AF2 */
	rds_af_decode(224+1	, 224+2);	// #1  #2

	/* filler in between */
	rds_af_decode(224+3	, 1);		// #3       AF1
	rds_af_decode(205	, 2);		// filler   AF2

	/* filler in between */
	rds_af_decode(224+5	, 1);		// #5       AF1
	rds_af_decode(2		, 205);		// AF2      filler
	rds_af_decode(3		, 4);		// AF3      AF4

	/* double LF/MF frequency */
	rds_af_decode(224+3	, 1);		// #3       AF1
	rds_af_decode(250	, 2);		// LF/MF    2
	rds_af_decode(250	, 2);		// LF/MF    2

	/* double VHF frequency */
	rds_af_decode(224+5	, 2);		// #5       AF2=TF
	rds_af_decode(1		, 2);		// AF1      TF
	rds_af_decode(1		, 2);		// AF1      TF     (double)

	/* double VHF frequency */
	rds_af_decode(224+5	, 2);		// #5       AF2=TF
	rds_af_decode(2		, 3);		// TF       AF3
	rds_af_decode(2		, 3);		// TF2      AF3     (double)

	/* double VHF frequency */
	rds_af_decode(224+5	, 2);		// #5       AF2=TF
	rds_af_decode(3		, 2);		// AF3      TF
	rds_af_decode(3		, 2);		// AF3      TF      (double)

	/* double VHF frequency */
	rds_af_decode(224+5	, 2);		// #5       AF2=TF
	rds_af_decode(2		, 1);		// TF       AF1
	rds_af_decode(2		, 1);		// TF       AF1    (double)

	/* double LF/MF frequency */
	rds_af_decode(224+5	, 1);		// #3       AF1=TF
	rds_af_decode(2		, 1);		// TF       AF1
	rds_af_decode(1		, 1);		// TF       TF

	/* double VHF frequency */
	rds_af_decode(224+5	, 2);		// #5       AF2=TF
	rds_af_decode(1		, 2);		// AF1      TF
	rds_af_decode(250	, 2);		// LF/MF    AF2
	rds_af_decode(250	, 2);		// LF/MF    AF2

	/* double LF/MF frequency */
	rds_af_decode(224+4	, 1);		// #3       AF1=TF
	rds_af_decode(2		, 1);		// TF       AF1
	rds_af_decode(250	, 204);		// LF/MF    invalid
}
END_TEST
#endif


/**
 * \brief Test suite for AF
 *
 * This is the test suite for AF.
 * It adds all tests into the test suite.
 */
Suite *af_suite(void)
{
	Suite *s = suite_create("AF");

	/* Core test case */
	TCase *tc_core = tcase_create("Core");
	tcase_add_test(tc_core, test_af_decode_a1);
	tcase_add_test(tc_core, test_af_decode_a2);
	tcase_add_test(tc_core, test_af_decode_a3);
	tcase_add_test(tc_core, test_af_decode_b1);
	tcase_add_test(tc_core, test_af_decode_b2);
	suite_add_tcase(s, tc_core);

	return s;
}


/**
 * \brief Main function for AF test suite.
 *
 * This is the main function for AF.
 * It initializes all necessary variables.
 */
int main(void)
{
	int number_failed;
	Suite *s = af_suite();
	SRunner *sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
