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
 * \file eon_test.c
 * \brief Enhanced Other Networks functions - tests
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains tests for Enhanced Other Networks handling.
 */

#include <check.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "eon.h"
#include "rds_private.h"


/**
 * \brief Test of \ref rds_eon_decode_pi function
 *
 * This tests the \ref rds_eon_decode_pi function.
 */
START_TEST(test_eon_decode_pi)
{
	rds_program_private_t rs;
	rds_program_current = (rds_program_t *) &rs;

	/* PI */
	rds_eon_decode_pi(0x1234);
//	fail_unless(rds_program_current->eon_pi[0] == 0x1234);
//	fail_unless(rds_program_current->eon_pi_cnt[0] == 1);
	rds_eon_decode_pi(0x1234);
//	fail_unless(rds_program_current->eon_pi_cnt[0] == 2);
}
END_TEST


/**
 * \brief Test of \ref rds_eon_decode_tp_on function
 *
 * This tests the \ref rds_eon_decode_tp_on function.
 */
START_TEST(test_eon_decode_tp_on)
{
	rds_program_private_t rs;
	rds_program_current = (rds_program_t *) &rs;

	/* TP */
	rds_eon_decode_tp_on(rds_program_current, 1);
	fail_unless(rds_program_current->tp == 1);
}
END_TEST


/**
 * \brief Test of \ref rds_eon_decode_ta_on function
 *
 * This tests the \ref rds_eon_decode_ta_on function.
 */
START_TEST(test_eon_decode_ta_on)
{
	rds_program_private_t rs;
	rds_program_current = (rds_program_t *) &rs;

	/* TA */
	rds_eon_decode_ta_on(rds_program_current, 1);
	fail_unless(rds_program_current->ta == 1);
}
END_TEST


/**
 * \brief Test of \ref rds_eon_decode function
 *
 * This tests the \ref rds_eon_decode function.
 */
START_TEST(test_eon_decode)
{
	rds_program_private_t rs;
	rds_program_current = (rds_program_t *) &rs;

	/* PS */
	rds_eon_decode(rds_program_current, 0, 0x3132);
	rds_eon_decode(rds_program_current, 1, 0x3334);
	rds_eon_decode(rds_program_current, 2, 0x3536);
	rds_eon_decode(rds_program_current, 3, 0x3738);
	fail_unless(rds_program_current->ps[0] == 0x0031);
	fail_unless(rds_program_current->ps[1] == 0x0032);
	fail_unless(rds_program_current->ps[2] == 0x0033);
	fail_unless(rds_program_current->ps[3] == 0x0034);
	fail_unless(rds_program_current->ps[4] == 0x0035);
	fail_unless(rds_program_current->ps[5] == 0x0036);
	fail_unless(rds_program_current->ps[6] == 0x0037);
	fail_unless(rds_program_current->ps[7] == 0x0038);

	/* AF(ON), AF(ON)*/
	rds_eon_decode(rds_program_current, 4, (224 << 8) | 205);
	rds_eon_decode(rds_program_current, 4, (224 << 8) | 205);

	/* Tuning freq. (TN), Mapped FM freq. x (ON) */
	rds_eon_decode(rds_program_current, 5, (1 << 8) | 5);
	fail_unless(rds_program_current->eon_mf[0].tn == 1);
	fail_unless(rds_program_current->eon_mf[0].mf == 5);
	rds_eon_decode(rds_program_current, 6, (2 << 8) | 6);
	fail_unless(rds_program_current->eon_mf[1].tn == 2);
	fail_unless(rds_program_current->eon_mf[1].mf == 6);
	rds_eon_decode(rds_program_current, 7, (3 << 8) | 7);
	fail_unless(rds_program_current->eon_mf[2].tn == 3);
	fail_unless(rds_program_current->eon_mf[2].mf == 7);
	rds_eon_decode(rds_program_current, 8, (4 << 8) | 8);
	fail_unless(rds_program_current->eon_mf[3].tn == 4);
	fail_unless(rds_program_current->eon_mf[3].mf == 8);

	/* Tuning freq. (TN), Mapped AM freq. (ON) */
	rds_eon_decode(rds_program_current, 9, (10 << 8) | 11);
	fail_unless(rds_program_current->eon_mf[4].tn == 10);
	fail_unless(rds_program_current->eon_mf[4].mf == 11);

	/* Unallocated */
	rds_eon_decode(rds_program_current, 10, 0);
	fail_unless(rds_decode_status == RDS_DECODE_STATUS_NOT_ASSIGNED);
	rds_eon_decode(rds_program_current, 11, 0);
	fail_unless(rds_decode_status == RDS_DECODE_STATUS_NOT_ASSIGNED);

	/* Linkage information: set by VC12 */
	fail_unless(rds_program_current->la == 0);
	rds_eon_decode(rds_program_current, 12, (1<<15) | (1<<13)); // (1<<13) is only to trigger rfu
	fail_unless(rds_decode_status == RDS_DECODE_STATUS_NOT_ASSIGNED);
	fail_unless(rds_program_current->eon_li[0].la == 1);
	fail_unless(rds_program_current->la == 1);
	/* Linkage information: clear by VC12 */
	rds_eon_decode(rds_program_current, 12, (0<<15));
	fail_unless(rds_program_current->eon_li[0].la == 0);
	fail_unless(rds_program_current->la == 0);
	/* Linkage information: set by VC12 */
	rds_eon_decode(rds_program_current, 12, (1<<15));
	fail_unless(rds_program_current->eon_li[0].la == 1);
	fail_unless(rds_program_current->la == 1);
	/* Linkage information: clear by generic LA */
	rds_eon_decode_la(0);
	fail_unless(rds_program_current->eon_li[0].la == 0);
	fail_unless(rds_program_current->la == 0);
	/* Linkage information: set only generic LA */
	rds_eon_decode_la(1);
	fail_unless(rds_program_current->eon_li[0].la == 0);
	fail_unless(rds_program_current->la == 1);

	/* PTY, TA */
	rds_eon_decode(rds_program_current, 13, (31 << 11) | 1);
	fail_unless(rds_program_current->pty == 31);
	fail_unless(rds_program_current->ta == 1);

	/* PIN */
	rds_eon_decode(rds_program_current, 14, (1 << 11) | (2 << 6) | 3);
	fail_unless(rds_program_current->pin.day == 1);
	fail_unless(rds_program_current->pin.hour == 2);
	fail_unless(rds_program_current->pin.minute == 3);
	rds_eon_decode(rds_program_current, 14, (0 << 11) | (4 << 6) | 5);
	fail_unless(rds_program_current->pin.day == 1);
	fail_unless(rds_program_current->pin.hour == 2);
	fail_unless(rds_program_current->pin.minute == 3);

	/* Reserved for broadcasters use */
	rds_eon_decode(rds_program_current, 15, 0);
}
END_TEST


/**
 * \brief Test suite for EON
 *
 * This is the test suite for EON.
 * It adds all tests into the test suite.
 */
Suite *eon_suite(void)
{
	Suite *s = suite_create("EON");

	/* Core test case */
	TCase *tc_core = tcase_create("Core");
	tcase_add_test(tc_core, test_eon_decode_pi);
	tcase_add_test(tc_core, test_eon_decode_tp_on);
	tcase_add_test(tc_core, test_eon_decode_ta_on);
	tcase_add_test(tc_core, test_eon_decode);
	suite_add_tcase(s, tc_core);

	return s;
}


/**
 * \brief Main function for EON test suite.
 *
 * This is the main function for EON.
 * It initializes all necessary variables.
 */
int main(void)
{
	int number_failed;
	Suite *s = eon_suite();
	SRunner *sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
