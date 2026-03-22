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
 * \file pty.c
 * \test All related test cases are defined in \ref pty_test.c
 * \brief Programme Type code functions
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains functions for Programme Type code handling.
 */

#include <sqlite3.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pty.h"
#include "rds_private.h"


/**
 * \brief Return PTY string
 *
 * This function returns a PTY string.
 *
 * \param[out] _str            Return PTY string
 * \param[in]  _display_length Display length (<=8, <=16 or more)
 * \param[in]  _pty            PTY code
 * \param[in]  _rbds           Selector RDS=0 (othen then ITU region 2), RBDS=1 (ITU region 2)
 */
void rds_pty_get_str(char *_str, size_t _display_length, uint8_t _pty, uint8_t _rbds)
{
    char sql[] = "select RBDS16 from PTY where PTY=__";
    sqlite3_stmt *stmt = NULL;

    /* clear buffer */
    memset(_str, 0, _display_length);

    /* open database */
    if (rds_db_lang_open(NULL) != EXIT_SUCCESS)
        return;

    /* SQL request */
    if (_rbds == 0)
    {
        if (_display_length <= 8)
        {
            (void) snprintf(&sql[0], sizeof(sql), "select RDS8 from PTY where PTY=%hu", (unsigned short int) _pty);
        }
        else if (_display_length <= 16)
        {
            (void) snprintf(&sql[0], sizeof(sql), "select RDS16 from PTY where PTY=%hu", (unsigned short int) _pty);
        }
        else
        {
            (void) snprintf(&sql[0], sizeof(sql), "select RDS from PTY where PTY=%hu", (unsigned short int) _pty);
        }
    }
    else
    {
        if (_display_length <= 8)
        {
            (void) snprintf(&sql[0], sizeof(sql), "select RBDS8 from PTY where PTY=%hu", (unsigned short int) _pty);
        }
        else if (_display_length <= 16)
        {
            (void) snprintf(&sql[0], sizeof(sql), "select RBDS16 from PTY where PTY=%hu", (unsigned short int) _pty);
        }
        else
        {
            (void) snprintf(&sql[0], sizeof(sql), "select RBDS from PTY where PTY=%hu", (unsigned short int) _pty);
        }
    }
    /*@-nullpass@*/
    (void) sqlite3_prepare(rds_db_lang, sql, (int) sizeof(sql), /*@-nullstate@*/ &stmt /*@+nullstate@*/, NULL);

    /* evaluate SQL results */
    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        /*@+ignoresigns@ @-mustfreefresh@*/
        (void) strncpy(_str, sqlite3_column_text(stmt, 0), _display_length);
        /*@-ignoresigns@ @+mustfreefresh@*/
    }
    else
    {
        (void) strncpy(_str, "", _display_length);
    }

    /* finish SQL */
    (void) sqlite3_finalize(stmt);
    /*@+nullpass@*/
}


/**
 * \brief Decodes and handles received PTY codes
 *
 * This function decodes and handles received PTY codes.
 *
 * \param[in] _pty Programme Type code
 */
void rds_pty_decode(uint8_t _pty)
{
    rds_program_current->pty = _pty;
}
