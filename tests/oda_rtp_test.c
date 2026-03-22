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
 * \file oda_rtp_test.c
 * \brief Radio Text Plus functions - tests
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains tests for Radio Text Plus handling.
 */

#include <check.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "rds_private.h"
#include "oda_rtp.h"


/**
 * \brief Test of \ref rds_rt_decode_a, \ref rds_oda_decode_assign and \ref rds_oda_decode_a functions specific to RTP
 *
 * This tests the \ref rds_rt_decode_a, \ref rds_oda_decode_assign and \ref rds_oda_decode_a functions specific to RTP.
 */
START_TEST(test_rtp_rt)
{
	/* You are listening to 'House of the rising sun' by Eric Burdon    */
	/* 0----0----1----1----2----2----3----3----4----4----5----5----6--- */
	/* 0----5----0----5----0----5----0----5----0----5----0----5----0--- */
	rds_rt_decode_a(0,  0, 'Y', 'o', 'u', ' ');
	rds_rt_decode_a(0,  1, 'a', 'r', 'e', ' ');
	rds_rt_decode_a(0,  2, 'l', 'i', 's', 't');
	rds_rt_decode_a(0,  3, 'e', 'n', 'i', 'n');
	rds_rt_decode_a(0,  4, 'g', ' ', 't', 'o');
	rds_rt_decode_a(0,  5, ' ', '\'', 'H', 'o');
	rds_rt_decode_a(0,  6, 'u', 's', 'e', ' ');
	rds_rt_decode_a(0,  7, 'o', 'f', ' ', 't');
	rds_rt_decode_a(0,  8, 'h', 'e', ' ', 'r');
	rds_rt_decode_a(0,  9, 'i', 's', 'i', 'n');
	rds_rt_decode_a(0, 10, 'g', ' ', 's', 'u');
	rds_rt_decode_a(0, 11, 'n', '\'', ' ', 'b');
	rds_rt_decode_a(0, 12, 'y', ' ', 'E', 'r');
	rds_rt_decode_a(0, 13, 'i', 'c', ' ', 'B');
	rds_rt_decode_a(0, 14, 'u', 'r', 'd', 'o');
	rds_rt_decode_a(0, 15, 'n', ' ', ' ', ' ');

	/* rds_oda_rtp_decode_assign */
	rds_oda_decode_assign(11, 0, 0, 0x4bd7);
//	fail_unless(rds_program_current->oda_rtp_ert == 0);
	fail_unless(rds_program_current->oda_rtp_cb == 0);
	fail_unless(rds_program_current->oda_rtp_scb == 0);
	fail_unless(rds_program_current->oda_rtp_tn == 0);

	/* rds_oda_rtp_decode */
	rds_oda_decode_a(11, 0,
		(1<<13)|(22<<7)|(22<<1),	/* content=ITEM.TITLE=1 start=22 length=22 */
		(4<<11)|(50<<5)|10);		/* content=ITEM.ARTIST=4 start=50 length=10 */

	/* check ITEM.TITLE */
	fail_unless(rds_program_current->oda_rtp[1][ 0] == 'H');
	fail_unless(rds_program_current->oda_rtp[1][ 1] == 'o');
	fail_unless(rds_program_current->oda_rtp[1][ 2] == 'u');
	fail_unless(rds_program_current->oda_rtp[1][ 3] == 's');
	fail_unless(rds_program_current->oda_rtp[1][ 4] == 'e');
	fail_unless(rds_program_current->oda_rtp[1][ 5] == ' ');
	fail_unless(rds_program_current->oda_rtp[1][ 6] == 'o');
	fail_unless(rds_program_current->oda_rtp[1][ 7] == 'f');
	fail_unless(rds_program_current->oda_rtp[1][ 8] == ' ');
	fail_unless(rds_program_current->oda_rtp[1][ 9] == 't');
	fail_unless(rds_program_current->oda_rtp[1][10] == 'h');
	fail_unless(rds_program_current->oda_rtp[1][11] == 'e');
	fail_unless(rds_program_current->oda_rtp[1][12] == ' ');
	fail_unless(rds_program_current->oda_rtp[1][13] == 'r');
	fail_unless(rds_program_current->oda_rtp[1][14] == 'i');
	fail_unless(rds_program_current->oda_rtp[1][15] == 's');
	fail_unless(rds_program_current->oda_rtp[1][16] == 'i');
	fail_unless(rds_program_current->oda_rtp[1][17] == 'n');
	fail_unless(rds_program_current->oda_rtp[1][18] == 'g');
	fail_unless(rds_program_current->oda_rtp[1][19] == ' ');
	fail_unless(rds_program_current->oda_rtp[1][20] == 's');
	fail_unless(rds_program_current->oda_rtp[1][21] == 'u');
	fail_unless(rds_program_current->oda_rtp[1][22] == 'n');
	fail_unless(rds_program_current->oda_rtp[1][23] == '\0');

	/* check ITEM.ARTIST */
	fail_unless(rds_program_current->oda_rtp[4][ 0] == 'E');
	fail_unless(rds_program_current->oda_rtp[4][ 1] == 'r');
	fail_unless(rds_program_current->oda_rtp[4][ 2] == 'i');
	fail_unless(rds_program_current->oda_rtp[4][ 3] == 'c');
	fail_unless(rds_program_current->oda_rtp[4][ 4] == ' ');
	fail_unless(rds_program_current->oda_rtp[4][ 5] == 'B');
	fail_unless(rds_program_current->oda_rtp[4][ 6] == 'u');
	fail_unless(rds_program_current->oda_rtp[4][ 7] == 'r');
	fail_unless(rds_program_current->oda_rtp[4][ 8] == 'd');
	fail_unless(rds_program_current->oda_rtp[4][ 9] == 'o');
	fail_unless(rds_program_current->oda_rtp[4][10] == 'n');
	fail_unless(rds_program_current->oda_rtp[4][11] == '\0');
}
END_TEST


/**
 * \brief Test of \ref rds_oda_ert_decode_assign, \ref rds_oda_ert_decode, \ref rds_oda_decode_assign and \ref rds_oda_decode_a functions specific to RTP
 *
 * This tests the \ref rds_oda_ert_decode_assign, \ref rds_oda_ert_decode, \ref rds_oda_decode_assign and \ref rds_oda_decode_a functions specific to RTP.
 */
START_TEST(test_rtp_ert)
{
	/* You are listening to 'House of the rising sun' by Eric Burdon    */
	/* 0----0----1----1----2----2----3----3----4----4----5----5----6--- */
	/* 0----5----0----5----0----5----0----5----0----5----0----5----0--- */
	rds_oda_ert_decode_assign(0);
	rds_oda_ert_decode( 0, 'Y'<<8, 'o'<<8);
	rds_oda_ert_decode( 1, 'u'<<8, ' '<<8);
	rds_oda_ert_decode( 2, 'a'<<8, 'r'<<8);
	rds_oda_ert_decode( 3, 'e'<<8, ' '<<8);
	rds_oda_ert_decode( 4, 'l'<<8, 'i'<<8);
	rds_oda_ert_decode( 5, 's'<<8, 't'<<8);
	rds_oda_ert_decode( 6, 'e'<<8, 'n'<<8);
	rds_oda_ert_decode( 7, 'i'<<8, 'n'<<8);
	rds_oda_ert_decode( 8, 'g'<<8, ' '<<8);
	rds_oda_ert_decode( 9, 't'<<8, 'o'<<8);
	rds_oda_ert_decode(10, ' '<<8, '\''<<8);
	rds_oda_ert_decode(11, 'H'<<8, 'o'<<8);
	rds_oda_ert_decode(12, 'u'<<8, 's'<<8);
	rds_oda_ert_decode(13, 'e'<<8, ' '<<8);
	rds_oda_ert_decode(14, 'o'<<8, 'f'<<8);
	rds_oda_ert_decode(15, ' '<<8, 't'<<8);
	rds_oda_ert_decode(16, 'h'<<8, 'e'<<8);
	rds_oda_ert_decode(17, ' '<<8, 'r'<<8);
	rds_oda_ert_decode(18, 'i'<<8, 's'<<8);
	rds_oda_ert_decode(19, 'i'<<8, 'n'<<8);
	rds_oda_ert_decode(20, 'g'<<8, ' '<<8);
	rds_oda_ert_decode(21, 's'<<8, 'u'<<8);
	rds_oda_ert_decode(22, 'n'<<8, '\''<<8);
	rds_oda_ert_decode(23, ' '<<8, 'b'<<8);
	rds_oda_ert_decode(24, 'y'<<8, ' '<<8);
	rds_oda_ert_decode(25, 'E'<<8, 'r'<<8);
	rds_oda_ert_decode(26, 'i'<<8, 'c'<<8);
	rds_oda_ert_decode(27, ' '<<8, 'B'<<8);
	rds_oda_ert_decode(28, 'u'<<8, 'r'<<8);
	rds_oda_ert_decode(29, 'd'<<8, 'o'<<8);
	rds_oda_ert_decode(30, 'n'<<8,  0 <<8);

	/* rds_oda_rtp_decode_assign */
	rds_oda_decode_assign(11, 0, 1<<13, 0x4bd7);
//	fail_unless(rds_program_current->oda_rtp_ert == 1);
	fail_unless(rds_program_current->oda_rtp_cb == 0);
	fail_unless(rds_program_current->oda_rtp_scb == 0);
	fail_unless(rds_program_current->oda_rtp_tn == 0);

	/* rds_oda_rtp_decode */
	rds_oda_decode_a(11, 0,
		(1<<13)|(22<<7)|(22<<1),	/* content=ITEM.TITLE=1 start=22 length=22 */
		(4<<11)|(50<<5)|10);		/* content=ITEM.ARTIST=4 start=50 length=10 */

	/* check ITEM.TITEL */
	fail_unless(rds_program_current->oda_rtp[1][ 0] == 'H');
	fail_unless(rds_program_current->oda_rtp[1][ 1] == 'o');
	fail_unless(rds_program_current->oda_rtp[1][ 2] == 'u');
	fail_unless(rds_program_current->oda_rtp[1][ 3] == 's');
	fail_unless(rds_program_current->oda_rtp[1][ 4] == 'e');
	fail_unless(rds_program_current->oda_rtp[1][ 5] == ' ');
	fail_unless(rds_program_current->oda_rtp[1][ 6] == 'o');
	fail_unless(rds_program_current->oda_rtp[1][ 7] == 'f');
	fail_unless(rds_program_current->oda_rtp[1][ 8] == ' ');
	fail_unless(rds_program_current->oda_rtp[1][ 9] == 't');
	fail_unless(rds_program_current->oda_rtp[1][10] == 'h');
	fail_unless(rds_program_current->oda_rtp[1][11] == 'e');
	fail_unless(rds_program_current->oda_rtp[1][12] == ' ');
	fail_unless(rds_program_current->oda_rtp[1][13] == 'r');
	fail_unless(rds_program_current->oda_rtp[1][14] == 'i');
	fail_unless(rds_program_current->oda_rtp[1][15] == 's');
	fail_unless(rds_program_current->oda_rtp[1][16] == 'i');
	fail_unless(rds_program_current->oda_rtp[1][17] == 'n');
	fail_unless(rds_program_current->oda_rtp[1][18] == 'g');
	fail_unless(rds_program_current->oda_rtp[1][19] == ' ');
	fail_unless(rds_program_current->oda_rtp[1][20] == 's');
	fail_unless(rds_program_current->oda_rtp[1][21] == 'u');
	fail_unless(rds_program_current->oda_rtp[1][22] == 'n');
	fail_unless(rds_program_current->oda_rtp[1][23] == '\0');

	/* check ITEM.ARTIST */
	fail_unless(rds_program_current->oda_rtp[4][ 0] == 'E');
	fail_unless(rds_program_current->oda_rtp[4][ 1] == 'r');
	fail_unless(rds_program_current->oda_rtp[4][ 2] == 'i');
	fail_unless(rds_program_current->oda_rtp[4][ 3] == 'c');
	fail_unless(rds_program_current->oda_rtp[4][ 4] == ' ');
	fail_unless(rds_program_current->oda_rtp[4][ 5] == 'B');
	fail_unless(rds_program_current->oda_rtp[4][ 6] == 'u');
	fail_unless(rds_program_current->oda_rtp[4][ 7] == 'r');
	fail_unless(rds_program_current->oda_rtp[4][ 8] == 'd');
	fail_unless(rds_program_current->oda_rtp[4][ 9] == 'o');
	fail_unless(rds_program_current->oda_rtp[4][10] == 'n');
	fail_unless(rds_program_current->oda_rtp[4][11] == '\0');
}
END_TEST


/**
 * \brief Test of \ref rds_oda_rtp_get_class function
 *
 * This tests the \ref rds_oda_rtp_get_class function.
 */
START_TEST(test_rtp_get_class)
{
	char s[60];

	/* 27;Info;INFO.ALARM */
	rds_oda_rtp_get_class(&s[0], sizeof(s), 27);
	fail_unless(strcmp(&s[0], "INFO.ALARM") == 0);
}
END_TEST


/**
 * \brief Test suite for ODA RTP
 *
 * This is the test suite for ODA RTP.
 * It adds all tests into the test suite.
 */
Suite *rtp_suite(void)
{
	Suite *s = suite_create("ODA.RTP");

	/* Core test case */
	TCase *tc_core = tcase_create("Core");
	tcase_add_test(tc_core, test_rtp_rt);
	tcase_add_test(tc_core, test_rtp_ert);
	tcase_add_test(tc_core, test_rtp_get_class);
	suite_add_tcase(s, tc_core);

	return s;
}


/**
 * \brief Main function for ODA RTP test suite.
 *
 * This is the main function for ODA RTP.
 * It initializes all necessary variables.
 */
int main(void)
{
	int number_failed;
	rds_program_private_t rs;
	rds_program_current = (rds_program_t *) &rs;
	sqlite3_open("../db/rds_lang/rds_lang_en_CEN.db", &rds_db_lang);
	Suite *s = rtp_suite();
	SRunner *sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
