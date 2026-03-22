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
 * \file ct_test.c
 * \brief Clock Time functions - tests
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains tests for Clock Time handling.
 */

#include <check.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "ct.h"
#include "rds_private.h"


/**
 * \brief Test of \ref rds_ct_decode function
 *
 * This tests the \ref rds_ct_decode function.
 */
START_TEST(test_ct_decode)
{
	/* MJD = 45218, Y=(19)82 M=9 (September) D=6 W=4315 WY=(19)82 WN=36 WD=1 (Monday) */

	/* 1982-09-06 00:00:00 UTC Monday */
	rds_ct_decode(45218, 0, 0, 0, 0); 
	fail_unless(rds_program_current->ct == 400118400);

	/* 1982-09-06 12:34:00 UTC Monday */
	rds_ct_decode(45218, 12, 34, 0, 0);
	fail_unless(rds_program_current->ct == 400163640);

	/* 1982-09-06 12:34:00 CEST Monday */
	rds_ct_decode(45218, 12-2, 34, 0, 4);
	fail_unless(rds_program_current->ct == 400156440);

	/* different failure situations should not change the old time */
	rds_ct_decode(100000,  0,  0, 0, 0); // mjd>99999
	fail_unless(rds_program_current->ct == 400156440);
	rds_ct_decode( 45218, 24,  0, 0, 0); // hour>23
	fail_unless(rds_program_current->ct == 400156440);
	rds_ct_decode( 45218,  0, 60, 0, 0); // minute>59
	fail_unless(rds_program_current->ct == 400156440);

	/* this gets the local PC time */
	rds_ct_decode(     0,  0,  0, 0, 0); // mjd=0
}
END_TEST


/**
 * \brief Test of \ref rds_ct_start_of_day function
 *
 * This tests the \ref rds_ct_start_of_day function.
 */
START_TEST(test_ct_start_of_day)
{
	time_t t;

	rds_ct_decode(45217, 22, 0, 0, 4); /* 1982-09-06 00:00:00 CEST Monday */
	t = rds_program_current->ct;
	fail_unless(t == 400111200);

	fail_unless(rds_ct_start_of_day(t-1,  0) == 400024800);	/* -1 second */
	fail_unless(rds_ct_start_of_day(t  ,  0) == 400111200);
	fail_unless(rds_ct_start_of_day(t+1,  0) == 400111200);	/* +1 second */
	fail_unless(rds_ct_start_of_day(t  , -1) == 400024800);	/* -1 day */
	fail_unless(rds_ct_start_of_day(t  , +1) == 400197600);	/* +1 day */
}
END_TEST


/**
 * \brief Test of \ref rds_ct_midnight function
 *
 * This tests the \ref rds_ct_midnight function.
 */
START_TEST(test_ct_midnight)
{
	time_t t;

	rds_ct_decode(45217, 22, 0, 0, 4); /* 1982-09-06 00:00:00 CEST Monday */
	t = rds_program_current->ct;
	fail_unless(t == 400111200);

	t = rds_ct_midnight(t-1, 0);

	fail_unless(rds_ct_midnight(t-1, 0) == 400111200); /* today -1 second */
	fail_unless(rds_ct_midnight(t  , 0) == 400197600); /* today +0 second */
	fail_unless(rds_ct_midnight(t+1, 0) == 400197600); /* today +1 second */

	fail_unless(rds_ct_midnight(t-1, 1) == 400197600); /* monday */
	fail_unless(rds_ct_midnight(t  , 1) == 400197600);
	fail_unless(rds_ct_midnight(t+1, 1) == 400197600);
	fail_unless(rds_ct_midnight(t-1, 2) == 400284000); /* tuesday */
	fail_unless(rds_ct_midnight(t  , 2) == 400284000);
	fail_unless(rds_ct_midnight(t+1, 2) == 400284000);
	fail_unless(rds_ct_midnight(t-1, 3) == 400370400); /* wednesday */
	fail_unless(rds_ct_midnight(t  , 3) == 400370400);
	fail_unless(rds_ct_midnight(t+1, 3) == 400370400);
	fail_unless(rds_ct_midnight(t-1, 4) == 400456800); /* thursday */
	fail_unless(rds_ct_midnight(t  , 4) == 400456800);
	fail_unless(rds_ct_midnight(t+1, 4) == 400456800);
	fail_unless(rds_ct_midnight(t-1, 5) == 400543200); /* friday */
	fail_unless(rds_ct_midnight(t  , 5) == 400543200);
	fail_unless(rds_ct_midnight(t+1, 5) == 400543200);
	fail_unless(rds_ct_midnight(t-1, 6) == 400629600); /* saturday */
	fail_unless(rds_ct_midnight(t  , 6) == 400629600);
	fail_unless(rds_ct_midnight(t+1, 6) == 400629600);
	fail_unless(rds_ct_midnight(t-1, 7) == 400716000); /* sunday */
	fail_unless(rds_ct_midnight(t  , 7) == 400716000);
	fail_unless(rds_ct_midnight(t+1, 7) == 400716000);
}
END_TEST


/**
 * \brief Test suite for CT
 *
 * This is the test suite for CT.
 * It adds all tests into the test suite.
 */
Suite *ct_suite(void)
{
	Suite *s = suite_create("CT");

	/* Core test case */
	TCase *tc_core = tcase_create("Core");
	tcase_add_test(tc_core, test_ct_decode);
	tcase_add_test(tc_core, test_ct_start_of_day);
	tcase_add_test(tc_core, test_ct_midnight);
	suite_add_tcase(s, tc_core);

	return s;
}


/**
 * \brief Main function for CT test suite.
 *
 * This is the main function for CT.
 * It initializes all necessary variables.
 */
int main(void)
{
	int number_failed;
	rds_program_private_t rs;
	rds_program_current = (rds_program_t *) &rs;
	Suite *s = ct_suite();
	SRunner *sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
