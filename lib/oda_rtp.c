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
 * \file oda_rtp.c
 * \test All related test cases are defined in \ref oda_rtp_test.c
 * \brief Radio Text Plus functions
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains functions for Radio Text Plus handling.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include "rds_private.h"
#include "rt.h"
#include "oda_ert.h"
#include "oda_rtp.h"


/**
 * \brief Return RT+ class string
 *
 * This function returns a RT+ class string.
 *
 * \param[out] _str  Return RT+ class string
 * \param[in]  _size String size
 * \param[in]  _rtp  RT+ class code
 */
void rds_oda_rtp_get_class(char *_str, size_t _size, uint8_t _rtp)
{
    char sql[] = "select CLASS from RTP where CODE=__";
    sqlite3_stmt *stmt = NULL;

    /* clear buffer */
    memset(_str, 0, _size);

    /* open database */
    if (rds_db_lang_open(NULL) != EXIT_SUCCESS)
        return;

    /* SQL request */
    (void) snprintf(&sql[0], sizeof(sql), "select CLASS from RTP where CODE=%hu",
                    (unsigned short int) _rtp);
    /*@-nullpass@*/
    (void) sqlite3_prepare(rds_db_lang, sql, (int) sizeof(sql), /*@-nullstate@*/ &stmt /*@+nullstate@*/, NULL);

    /* evaluate SQL results */
    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        /*@+ignoresigns@ @-mustfreefresh@*/
        (void) strncpy(_str, sqlite3_column_text(stmt, 0), _size);
        /*@-ignoresigns@ @+mustfreefresh@*/
    }
    else
    {
        (void) strncpy(_str, "", _size);
    }

    /* finish SQL */
    (void) sqlite3_finalize(stmt);
    /*@+nullpass@*/
}


/**
 * \brief Decodes and handles received RT+ information
 *
 * This function decodes and handles received RT+ information.
 *
 * \param[in] _x Block 2 data
 * \param[in] _y Block 3 data
 * \param[in] _z Block 4 data
 */
void rds_oda_rtp_decode(uint8_t _x, uint16_t _y, uint16_t _z)
{
    uint8_t i;
    uint8_t start_marker_1, length_marker_1;
    uint8_t start_marker_2, length_marker_2;

    /* get access to private data */
    rds_program_private_t *rds_program_current_private =
        (rds_program_private_t *) rds_program_current;

    /* item toggle/running bits */
    rds_program_current->oda_rtp_itb = (_x >> 4) & 1;
    rds_program_current->oda_rtp_irb = (_x >> 3) & 1;

    /* marker 1 */
    start_marker_1  = (_y >> 7) & 0x3f;
    length_marker_1 = (_y >> 1) & 0x3f;

    /* marker 2 */
    start_marker_2  = (_z >> 5) & 0x3f;
    length_marker_2 = _z & 0x1f;

    /* content types */
    rds_program_current->oda_rtp_ct[0]  = ((_x & 7) << 3) | (_y >> 13);
    rds_program_current->oda_rtp_ct[1]  = ((_y & 1) << 5) | (_z >> 11);

    /* clear content */
    memset(&rds_program_current->oda_rtp[rds_program_current->oda_rtp_ct[0]], 0, sizeof(rds_program_current->oda_rtp[rds_program_current->oda_rtp_ct[0]]));
    memset(&rds_program_current->oda_rtp[rds_program_current->oda_rtp_ct[1]], 0, sizeof(rds_program_current->oda_rtp[rds_program_current->oda_rtp_ct[1]]));

    /* copy new content */
    if (rds_program_current_private->oda_rtp_ert == 0)
    {
        /* Content Type 1 */
        for(i = 0; i <= length_marker_1; i++)
            rds_program_current->oda_rtp[rds_program_current->oda_rtp_ct[0]][i] = rds_program_current->rt[start_marker_1+i];

        /* Content Type 2 */
        for(i = 0; i <= length_marker_2; i++)
            rds_program_current->oda_rtp[rds_program_current->oda_rtp_ct[1]][i] = rds_program_current->rt[start_marker_2+i];
    }
    else
    {
        /* Content Type 1 */
        for(i = 0; i <= length_marker_1; i++)
            rds_program_current->oda_rtp[rds_program_current->oda_rtp_ct[0]][i] = rds_program_current->oda_ert[start_marker_1+i];

        /* Content Type 2 */
        for(i = 0; i <= length_marker_2; i++)
            rds_program_current->oda_rtp[rds_program_current->oda_rtp_ct[1]][i] = rds_program_current->oda_ert[start_marker_2+i];
    }
}


/**
 * \brief Decode and handles received RT+ assign message
 *
 * This function decodes and handles received RT+ assign messages.
 *
 * \param[in] _msg Message bits
 */
void rds_oda_rtp_decode_assign(uint16_t _msg)
{
    /* get access to private data */
    rds_program_private_t *rds_program_current_private =
        (rds_program_private_t *) rds_program_current;

    uint8_t rfu = _msg >> 14;   /* reserved for future use */
    if (rfu != 0)
        rds_decode_status = RDS_DECODE_STATUS_NOT_ASSIGNED;
    rds_program_current_private->oda_rtp_ert = (_msg >> 13) & 1;
    rds_program_current->oda_rtp_cb  = (_msg >> 12) & 1;
    rds_program_current->oda_rtp_scb = (_msg >>  8) & 0xf;
    rds_program_current->oda_rtp_tn  =  _msg        & 0xff;
}
