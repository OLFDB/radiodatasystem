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
 * \file ct.h
 * \brief Clock Time functions - headers
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains functions for Clock Time handling.
 */

#ifndef CT_H
#define CT_H

#include <stdint.h>
#include <time.h>


/**
 * \brief Calculate CT time from computer time.
 *
 * This functions uses the computer time to set the RDS CT time.
 *
 * \return Current time
 */
time_t rds_ct_from_gmtime(void);


/**
 * \brief Decodes and handles received CT messages
 *
 * This function decodes and handles received CT messages
 *
 * \param[in] _mjd  Modified Julian Day (range 0-99999)
 * \param[in] _hour Hour (range 0-23)
 * \param[in] _minute   Minute (range 0-59)
 * \param[in] _slto Sense of local time offset (0 = positive offset and 1 = negative offset)
 * \param[in] _lto  Local time offset (expressed in multiples of half hours within the  range -15.5h to +15.5h)
 */
void rds_ct_decode(uint32_t _mjd, uint8_t _hour, uint8_t _minute, uint8_t _slto, uint8_t _lto);


/**
 * \brief Return start of day and add number of days
 *
 * Returns the start of day and adds additional number of days.
 *
 * \param[in] _time Timebase for calculation
 * \param[in] _day_inc  Day increment (0=today, 1=tomorrow=midnight today, 2=...)
 * \return      Unix time
 */
time_t rds_ct_start_of_day(time_t _time, int8_t _day_inc);


/**
 * \brief Return next weekdays midnight time
 *
 * Returns next weekdays midnight time.
 *
 * \param[in] _time Timebase for calculation
 * \param[in] _wd   0 meens todays midnight. 1=mon..7=sun means next midnight of given weekday.
 * \return      Unix time
 */
time_t rds_ct_midnight(time_t _time, uint8_t _wd);


#endif /* CT_H */
