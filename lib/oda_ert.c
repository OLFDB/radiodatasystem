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
 * \file oda_ert.c
 * \test All related test cases are defined in \ref oda_ert_test.c
 * \brief Enhanced Radio Text functions
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains functions for Enhanced Radio Text handling.
 */

#include <iconv.h>
#include <stdint.h>
#include <string.h>
#include <wchar.h>
#include "rds_private.h"
#include "oda_ert.h"


/**
 * \brief Conversion descriptor for UTF-8 to WCHAR
 *
 * This variable contains the conversion descriptor for the UTF-8 to WCHAR convesion.
 */
static iconv_t cd_utf8_to_wchar;


/**
 * \brief Conversion descriptor for UTF-16 to WCHAR
 *
 * This variable contains the conversion descriptor for the UTF-16 to WCHAR convesion.
 */
static iconv_t cd_utf16_to_wchar;


/**
 * \brief Decodes and handles received eRT information
 *
 * This function decodes and handles received eRT information.
 *
 * \param[in] _bpac eRT byte pair address code
 * \param[in] _y    eRT byte numbers (block 3)
 * \param[in] _z    eRT byte numbers (block 4)
 */
void rds_oda_ert_decode(uint8_t _bpac, uint16_t _y, uint16_t _z)
{
    uint8_t change = 0;

    /* get access to private data */
    rds_program_private_t *rds_program_current_private =
        (rds_program_private_t *) rds_program_current;

    /* process characters */
    if (rds_program_current_private->oda_ert_str[4*_bpac+0] != _y >> 8)
    {
        rds_program_current_private->oda_ert_str[4*_bpac+0] = _y >> 8;
        change = 1;
    }
    if (rds_program_current_private->oda_ert_str[4*_bpac+1] != _y & 0xff)
    {
        rds_program_current_private->oda_ert_str[4*_bpac+1] = _y & 0xff;
        change = 1;
    }
    if (rds_program_current_private->oda_ert_str[4*_bpac+2] != _z >> 8)
    {
        rds_program_current_private->oda_ert_str[4*_bpac+2] = _z >> 8;
        change = 1;
    }
    if (rds_program_current_private->oda_ert_str[4*_bpac+3] != _z & 0xff)
    {
        rds_program_current_private->oda_ert_str[4*_bpac+3] = _z & 0xff;
        change = 1;
    }

    if (change == 1)
    {
        char *inbuf  = (char *) &rds_program_current_private->oda_ert_str;
        char *outbuf = (char *) &rds_program_current->oda_ert;
        size_t inbytesleft  = sizeof(rds_program_current_private->oda_ert_str);
        size_t outbytesleft = sizeof(rds_program_current->oda_ert);
        if (rds_program_current_private->oda_ert_utf8 == 0)
        {
            if (cd_utf16_to_wchar <= (iconv_t) 0)
            {
                /*@-onlytrans@*/
                cd_utf16_to_wchar = iconv_open("WCHAR_T", "UTF-16");
                /*@+onlytrans@*/
                if (cd_utf16_to_wchar <= (iconv_t) 0)
                {
                    /* this is an error */
                    rds_decode_status = RDS_DECODE_STATUS_ICONV_ERROR;
                    return;
                }
            }
            (void) iconv(cd_utf16_to_wchar, &inbuf, &inbytesleft, &outbuf, &outbytesleft);
        }
        else     /* UTF-16 */
        {
            if (cd_utf8_to_wchar <= (iconv_t) 0)
            {
                /*@-onlytrans@*/
                cd_utf8_to_wchar = iconv_open("WCHAR_T", "UTF-8");
                /*@+onlytrans@*/
                if (cd_utf8_to_wchar <= (iconv_t) 0)
                {
                    /* this is an error */
                    rds_decode_status = RDS_DECODE_STATUS_ICONV_ERROR;
                    return;
                }
            }
            (void) iconv(cd_utf8_to_wchar, &inbuf, &inbytesleft, &outbuf, &outbytesleft);
        }
    }
}


/**
 * \brief Decodes and handles received eTR assign message
 *
 * This function decodes and handles received eRT assign messages.
 *
 * \param[in] _msg eRT byte pair address
 */
void rds_oda_ert_decode_assign(uint8_t _msg)
{
    /* get access to private data */
    rds_program_private_t *rds_program_current_private =
        (rds_program_private_t *) rds_program_current;

    uint16_t rfu = (_msg >> 6) & 0x3ff; /* reserved for future use */
    if (rfu != 0)
        rds_decode_status = RDS_DECODE_STATUS_NOT_ASSIGNED;
    rds_program_current_private->oda_ert_ctid = (_msg >> 2) & 0xf;
    rds_program_current_private->oda_ert_dtfd = (_msg >> 1) & 1;
    rds_program_current_private->oda_ert_utf8 =  _msg       & 1;
}
