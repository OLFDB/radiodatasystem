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
 * \file lic.c
 * \test All related test cases are defined in \ref lic_test.c
 * \brief Language Identification Code functions
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains functions for Language Identification Code handling.
 */

#include <sqlite3.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lic.h"
#include "rds_private.h"


/**
 * \brief Return LIC string
 *
 * This function returns a LIC string.
 *
 * \param[out] _str  Return LIC string
 * \param[in]  _size String size
 * \param[in]  _lic  LI code
 */
void rds_lic_get_str(char *_str, size_t _size, uint8_t _lic)
{
    char sql[] = "select NAME from LIC where LIC=___";
    sqlite3_stmt *stmt = NULL;

    /* clear buffer */
    memset(_str, 0, _size);

    /* open database */
    if (rds_db_lang_open(NULL) != EXIT_SUCCESS)
        return;

    /* SQL request */
    (void) snprintf(&sql[0], sizeof(sql), "select NAME from LIC where LIC=%hu",
                    (unsigned short int) _lic);
    /*@-nullpass@ */
    (void) sqlite3_prepare(rds_db_lang, sql, (int) sizeof(sql), /*@-nullstate@*/ &stmt /*@+nullstate@*/, NULL);

    /* evaluate SQL results */
    if (stmt != NULL && sqlite3_step(stmt) == SQLITE_ROW)
    {
        /*@+ignoresigns@ @-mustfreefresh@*/
        (void) strncpy(_str, (char *) sqlite3_column_text(stmt, 0), _size);
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
 * \brief Decodes and handles received LI Codes
 *
 * This function decodes and handles received LI Codes.
 *
 * \param[in] _lic LI Code
 */
void rds_lic_decode(uint8_t _lic)
{
    rds_program_current->lic = _lic;
}
