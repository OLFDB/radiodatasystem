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
 * \file ct.c
 * \test All related test cases are defined in \ref ct_test.c
 * \brief Clock Time functions
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains functions for Clock Time handling.
 */

#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include "ct.h"
#include "rds.h"
#ifdef ODA_TMC
#include "oda_tmc.h"
#endif


/* splint means that "Identifier used in code has not been declared." */
/** \brief set or add an environment variable */
int setenv(const char *name, const char *value, int overwrite);
/** \brief reset an environment variable */
int unsetenv(const char *name);


/**
 * \brief Conversion from Modified Julian Date to Year, Month, Day
 *
 * This function converts from Modified Julian Date to Year, Month and Day.
 *
 * \param[out] _mjd Modified Julian Day
 * \param[in]  _y   Year from 1900 (e.g. for 2003, Y=103)
 * \param[in]  _m   Month from January (=1) to December (=12)
 * \param[in]  _d   Day of month from 1 to 31
 */
static void mjd2ymd(int _mjd, /*@out@*/ int *_y, /*@out@*/ int *_m, /*@out@*/ int *_d)
{
    int k;

    *_y = (int) ((_mjd - 15078.2) / 365.25);
    *_m = (int) ((_mjd - 14956.1 - (int) (*_y * 365.25)) / 30.6001);
    *_d = _mjd - 14956 - (int) (*_y * 365.25) - (int) (*_m * 30.6001);

    k = (((*_m == 14) || (*_m == 15)) ? 1 : 0);
    *_y = *_y + k;
    *_m = *_m - 1 - k * 12;
}


#if 0
/**
 * \brief Conversion from Year, Month, Day to Modified Julian Date
 *
 * This function converts from Year, Month and Day to Modified Julian Date.
 *
 * \param[in] _y Year from 1900 (e.g. for 2003, Y=103)
 * \param[in] _m Month from January (=1) to December (=12)
 * \param[in] _d Day of month from 1 to 31
 * \return       Modified Julian Day
 */
/*@unused@*/ static uint16_t ymd2mjd(uint16_t _y, uint8_t _m, uint8_t _d)
{
    int l = ((_m == 1) || (_m==2)) ? 1 : 0;

    return 14956 + _d + (int) ((_y-l) * 365.25) + (int) ((_m + 1 + l * 12) * 30.6001);
}
#endif


#if 0
/**
 * \brief Conversion from Modified Julian Date to Week Day
 *
 * This function converts from Modified Julian Date to Week Day.
 *
 * \param[in] _mjd Modified Julian Day
 * \return         Day of week from Monday (=1) to Sunday (=7)
 */
/*@unused@*/ static uint8_t mjd2wd(uint16_t _mjd)
{
    return ((_mjd + 2) % 7) + 1;
}
#endif


#if 0
/**
 * \brief Conversion from Week Year, Number, Day to Modified Julian Date
 *
 * This function converts from Week Year, Number and Day to Modified Julian Date.
 *
 * \param[in] _wy "Week number" Year from 1900
 * \param[in] _wn Week number according to ISO 2015
 * \param[in] _wd Day of week from Monday (=1) to Sunday (=7)
 * \return        Modified Julian Day
 */
/*@unused@*/ static uint16_t wywnwd2mjd(uint16_t _wy, uint8_t _wn, uint8_t _wd)
{
    return 15012 + _wd + 7 * (_wn + (int) ((_wy * 1461 / 28) + 0.41));
}
#endif


#if 0
/**
 * \brief Conversion from Modified Julian Date to Week Year, Number
 *
 * This function converts from Modified Julian Date to Week Year and Number.
 *
 * \param[in]  _mjd Modified Julian Day
 * \param[out] _wy  "Week number" Year from 1900
 * \param[out] _wn  Week number according to ISO 2015
 */
/*@unused@*/ static void mjd2wywn(uint16_t _mjd, /*@out@*/ uint16_t *_wy, /*@out@*/ uint8_t *_wn)
{
    int w = (int) ((_mjd / 7) - 2144.64);
    *_wy = (uint16_t) ((w * 28 / 1461) - 0.0079);
    *_wn = (uint8_t) (w - (int) ((*_wy * 1461 / 28) + 0.41));
}
#endif


/**
 * \brief Calculate CT time from computer time.
 *
 * This functions uses the computer time to set the RDS CT time.
 *
 * \return Current time
 */
time_t rds_ct_from_gmtime(void)
{
    time_t t;
    struct tm *tm;
    char *tz;

    /* save current time zone */
    tzset();
    tz = getenv("TZ");

    /* set time zone to UTC */
    (void) setenv("TZ", "", 1);
    tzset();

    /* get time structure */
    t = time(NULL);
    tm = gmtime(&t);

    /* call mktime */
    tm->tm_sec = 0;
    t = mktime(tm);

    /* reset time zone */
    if (tz)
        (void) setenv("TZ", tz, 1);
    else
        (void) unsetenv("TZ");
    tzset();

    return t;
}


/**
 * \brief Decodes and handles received CT messages
 *
 * This function decodes and handles received CT messages
 *
 * \param[in] _mjd    Modified Julian Day (range 0-99999)
 * \param[in] _hour   Hour (range 0-23)
 * \param[in] _minute Minute (range 0-59)
 * \param[in] _slto   Sense of local time offset (0 = positive offset and 1 = negative offset)
 * \param[in] _lto    Local time offset (expressed in multiples of half hours within the  range -15.5h to +15.5h)
 */
void rds_ct_decode(uint32_t _mjd, uint8_t _hour, uint8_t _minute, uint8_t _slto, uint8_t _lto)
{
    struct tm tm;
    char *tz;

    /* Do not update. Don't use this information directly. */
    /* The conversion to local time and date will be made in the receiver's circuitry. */
    if (_mjd == 0)
    {
        rds_program_current->ct = rds_ct_from_gmtime();
        return;
    }

    /* validity checks */
    if (_mjd > 99999)
        return;
    if (_hour > 23)
        return;
    if (_minute > 59)
        return;

    /* save information */
    mjd2ymd((int) _mjd,
            &tm.tm_year,
            &tm.tm_mon,
            &tm.tm_mday);
    tm.tm_mon--;
    tm.tm_wday = 0;
    tm.tm_yday = 0;
    tm.tm_hour = (int) _hour;
    tm.tm_min = (int) _minute;
    tm.tm_sec = 0;

    /* save current time zone */
    tzset();
    tz = getenv("TZ");

    /* set time zone to UTC */
    (void) setenv("TZ", "", 1);
    tzset();

    /* call mktime */
    tm.tm_isdst = 0;
    rds_program_current->ct = mktime(&tm);

    /* reset time zone */
    if (tz)
        (void) setenv("TZ", tz, 1);
    else
        (void) unsetenv("TZ");
    tzset();

    /* save local time offset */
    rds_program_current->ct_lto = (_slto == 0 ? 1 : -1) * (int8_t) _lto;

#ifdef ODA_TMC
    /* check for outdated TMC messages */
    rds_oda_tmc_message_memory_autoclean();
#endif
}


/**
 * \brief Return start of day and add number of days
 *
 * Returns the start of day and adds additional number of days.
 *
 * \param[in] _time    Timebase for calculation
 * \param[in] _day_inc Day increment (0=today, 1=tomorrow=midnight today, 2=...)
 * \return             Unix time
 */
time_t rds_ct_start_of_day(time_t _time, int8_t _day_inc)
{
    /** \todo if only tmc is using this function, move it there */
    /** \todo is it better to use gmtime and mktime here? */
    time_t t;

    /* map local time zone to utc time zone */
    t = _time + rds_program_current->ct_lto * 1800;

    /* go to start of day */
    t -= t % 86400;

    /* respect day increment */
    t += _day_inc * 86400;

    /* map utc time zone back to local time */
    return t - rds_program_current->ct_lto * 1800;
}


/**
 * \brief Return next weekdays midnight time
 *
 * Returns next weekdays midnight time.
 *
 * \param[in] _time Timebase for calculation
 * \param[in] _wd   0 meens todays midnight. 1=mon..7=sun means next midnight of given weekday.
 * \return          Unix time
 */
time_t rds_ct_midnight(time_t _time, uint8_t _wd)
{
    /** \todo if only tmc is using this function, move it there */
    /** \todo is it better to use gmtime and mktime here? */
    struct tm *tm;
    char *tz;
    time_t t;

    /* save current time zone */
    tzset();
    tz = getenv("TZ");

    /* set time zone to UTC */
    (void) setenv("TZ", "", 1);
    tzset();

    /* map local time zone to utc time zone */
    t = _time + rds_program_current->ct_lto * 1800;

    /* go to start of day */
    t -= t % 86400;

    /* get time structure */
    tm = gmtime(&t);

    /* reset time zone */
    if (tz)
        (void) setenv("TZ", tz, 1);
    else
        (void) unsetenv("TZ");
    tzset();

    /* increment to midnight */
    t += 86400;

    /* increase by days depending on selected weekday */
    if (_wd > 0)
        t += (_wd - tm->tm_wday) * 86400;

    /* map utc time zone back to local time */
    return t - rds_program_current->ct_lto * 1800;
}
