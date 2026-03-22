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
 * \file tdc_test.c
 * \brief Transparent Data Channel functions - tests
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains tests for Transparent Data Channel handling.
 */

#include <check.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "rds_private.h"
#include "tdc.h"


static uint8_t addr;
static uint16_t data;

static void tdc_callback(uint8_t _addr, uint16_t _data)
{
	addr = _addr;
	data = _data;
}


/**
 * \brief Test of \ref rds_tdc_decode function
 *
 * This tests the \ref rds_tdc_decode function.
 */
START_TEST(test_tdc_decode)
{
	rds_tdc_decode(0x12, 0x4567);
	fail_unless(addr == 0x12);
	fail_unless(data == 0x4567);
}
END_TEST


/**
 * \brief Test suite for TDC
 *
 * This is the test suite for TDC.
 * It adds all tests into the test suite.
 */
Suite *tdc_suite(void)
{
	Suite *s = suite_create("TDC");

	/* Core test case */
	TCase *tc_core = tcase_create("Core");
	tcase_add_test(tc_core, test_tdc_decode);
	suite_add_tcase(s, tc_core);

	return s;
}


/**
 * \brief Main function for TDC test suite.
 *
 * This is the main function for TDC.
 * It initializes all necessary variables.
 */
int main(void)
{
	int number_failed;
	rds_program_private_t rs;
	rds_program_current = (rds_program_t *) &rs;
	rds_tdc_callback = &tdc_callback;
	Suite *s = tdc_suite();
	SRunner *sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
