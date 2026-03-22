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
 * \file pin.c
 * \test All related test cases are defined in \ref pin_test.c
 * \brief Programme Item Number functions
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains functions for Programme Item Number handling.
 */

#include <stdint.h>
#include "pin.h"
#include "rds.h"


/**
 * \brief Decodes and handles received PIN codes
 *
 * This function decodes and handles received PIN codes.
 *
 * \param[in] _day    Day
 * \param[in] _hour   Hour
 * \param[in] _minute Minute
 */
void rds_pin_decode(uint8_t _day, uint8_t _hour, uint8_t _minute)
{
    /* ignore the rest of the message */
    if (_day == 0)
        return;

    /* handle components */
    rds_program_current->pin.day = _day;
    rds_program_current->pin.hour = _hour;
    rds_program_current->pin.minute = _minute;
}
