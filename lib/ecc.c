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
 * \file ecc.c
 * \test All related test cases are defined in \ref ecc_test.c
 * \brief Extended Country Code functions
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains functions for Extended Country Code handling.
 */

#include <sqlite3.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ecc.h"
#include "pi.h"
#include "rds_private.h"


/**
 * \brief Set ISO, ITU and CID based on CC and ECC
 *
 * This function sets ISO, ITU and CID based on CC and ECC.
 */
void rds_ecc_get_iso_itu_cid(void)
{
    char sql[] = "select ISO,ITU,CID from CC where ECC='__' and CCD='_'";
    sqlite3_stmt *stmt = NULL;

    /* open database */
    if (rds_db_lang_open(NULL) != EXIT_SUCCESS)
        return;

    /* SQL request */
    (void) snprintf(&sql[0], sizeof(sql), "select ISO,ITU,CID from CC where ECC='%2.2hX' and CCD='%1.1hX'",
                    (unsigned short int) rds_program_current->ecc,
                    (unsigned short int) rds_pi_cc(rds_program_current->pi));
    /*@-nullpass@*/
    (void) sqlite3_prepare(rds_db_lang, sql, (int) sizeof(sql), /*@-nullstate@*/ &stmt /*@+nullstate@*/, NULL);

    /* evaluate SQL results */
    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        /*@+ignoresigns@ @-mustfreefresh@*/
        (void) strncpy(&rds_program_current->iso[0], sqlite3_column_text(stmt, 0), 2);
        /*@-ignoresigns@ @+mustfreefresh@*/
        rds_program_current->itu = (uint8_t) sqlite3_column_int(stmt, 1);
#ifdef ODA_TMC
        rds_program_current->oda_tmc_cid = (uint8_t) sqlite3_column_int(stmt, 2);
#endif
    }

    /* finish SQL */
    (void) sqlite3_finalize(stmt);
    /*@+nullpass@*/
}


/**
 * \brief Return country name
 *
 * This function returns the country name.
 *
 * \param _str  Return country name
 * \param _size String size
 * \param _ecc  Extended country code
 * \param _cc   Country code
 */
void rds_ecc_get_name(char *_str, size_t _size, uint8_t _ecc, uint8_t _cc)
{
    char sql[] = "select NAME from CC where ECC='__' and CCD='_'";
    sqlite3_stmt *stmt = NULL;

    /* clear buffer */
    memset(_str, 0, _size);

    /* open database */
    if (rds_db_lang_open(NULL) != EXIT_SUCCESS)
        return;

    /* SQL request */
    (void) snprintf(&sql[0], sizeof(sql), "select NAME from CC where ECC='%2.2hX' and CCD='%1.1hX'",
                    (unsigned short int) _ecc,
                    (unsigned short int) _cc);
    /*@-nullpass@*/
    (void) sqlite3_prepare(rds_db_lang, sql, (int) sizeof(sql), /*@-nullstate@*/ &stmt /*@+nullstate@*/, NULL);

    /* evaluate SQL results */
    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        /*@+ignoresigns@ @-mustfreefresh@*/
        (void) strncpy(_str, sqlite3_column_text(stmt, 0), _size);
        /*@-ignoresigns@ @+mustfreefresh@*/
    }

    /* finish SQL */
    (void) sqlite3_finalize(stmt);
    /*@+nullpass@*/
}


/**
 * \brief Decodes and handles received EC Codes
 *
 * This function decodes and handles received EC Codes.
 *
 * \param[in] _ecc EC Code
 */
void rds_ecc_decode(uint8_t _ecc)
{
    /* change check */
    if (rds_program_current->ecc == _ecc)
        return;

    /* ECC */
    rds_program_current->ecc = _ecc;

    /* complete ISO and ITU */
    rds_ecc_get_iso_itu_cid();
}
