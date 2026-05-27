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
 * \file oda_tmc.c
 * \test All related test cases are defined in \ref oda_tmc_test.c
 * \brief Traffic Message Channel functions
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains functions for Traffic Message Channel handling.
 */

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <time.h>
#include "af.h"
#include "char_set.h"
#include "ct.h"
#include "ecc.h"
#include "pi.h"
#include "rds_private.h"
#include "oda_tmc.h"


/* splint means that "Identifier used in code has not been declared." */
/** \brief set or add an environment variable */
int setenv(const char *name, const char *value, int overwrite);
/** \brief reset an environment variable */
int unsetenv(const char *name);


/**
 * \brief TMC data service provider list
 *
 * This list contains all information to currently received TMC programs.
 */
static rds_oda_tmc_dsp_t rds_oda_tmc_dsp[4];


/**
 * \brief Current TMC data service provider
 *
 * This points to the current TMC data service provider.
 */
rds_oda_tmc_dsp_t *rds_oda_tmc_dsp_current;


/**
 * \brief Structure for TMC Event List entry type
 *
 * This type contains the language-independent constants of a TMC event list entry.
 */
typedef struct rds_oda_tmc_event_list_entry_t
{
    char        n;      /**< Nature: 'I'=Info 'F'=Forecast 'S'=Silent */
    uint8_t     q;      /**< Quantifier: 0..12 */
    char        t;      /**< Duration Type: 'D/d'=Dynamic 'L/l'=Longer-lasting */
    uint8_t     d;      /**< Directionality: 1..2 */
    char        u;      /**< Urgency: 'N'=Normal 'U'=Urgent 'X'=Extremely Urgent */
    uint8_t     c;      /**< Update Class: 1..31=EL,32..39=FEL */
} rds_oda_tmc_event_list_entry_t;


/**
 * \brief TMC Event List
 *
 * This array contains the language-independent constants of the TMC event list.
 */
static rds_oda_tmc_event_list_entry_t rds_oda_tmc_event_list[2048];


/**
 * \brief Pointer to the TMC Event List database
 *
 * This variable contains the pointer to the Event List database.
 */
static sqlite3 *rds_oda_tmc_db_el;


/**
 * \brief Filename of TMC Event List database
 *
 * This variable contains the filename to the Event List database.
 */
static char tmc_db_el_filename[80]; /* should be enough for DATADIR/tmc_el_xx_XXX.db */


/**
 * \brief Pointer to the TMC Location Code List database
 *
 * This variable contains the pointer to the Location Code List database.
 */
static sqlite3 *rds_oda_tmc_db_lcl;


/**
 * \brief Filename to the TMC Location Code List database
 *
 * This variable contains the filename to the Location Code List database.
 */
static char tmc_db_lcl_filename[80]; /* should be enough for DATADIR/tmc_lcl_xx_x_xx.db */


/**
 * \brief TMC callback function
 *
 * This variable contains the TMC callback function.
 *
 * \param[in] _msg    TMC message
 * \param[in] _action 0=new message,
 *                    1=message updated (by timer or by reception),
 *                    2="problem cleared" (by non-silent cancellation message)
 */
void (*rds_oda_tmc_callback)(rds_oda_tmc_message_t *_msg, uint8_t _action);


/**
 * \brief Open TMC Event List database
 *
 * This function checks if the database is open.
 * In case it's not it opens the database and reads in all language-independent constants.
 *
 * \return Exit status (EXIT_SUCCESS, EXIT_FAILURE)
 */
/*@-globstate@*/
static int tmc_db_el_open(void)
{
    char sql[] = "select CODE,N,Q,T,D,U,C from ___";
    char s[4];
    char filename[80];
    sqlite3_stmt *stmt = NULL;
    rds_oda_tmc_event_list_entry_t *e;
    uint16_t code = 0;

    /* check if it's already open */
    if (rds_oda_tmc_db_el != NULL)
        return EXIT_SUCCESS;

    /** \todo make database configurable */

    /* create database filename */
    (void) snprintf(&filename[0], sizeof(filename), "%s/tmc_el_%s.db",
                    /*@-unrecog@*/ DATADIR /*@+unrecog@*/,
                    "de_DE");

    /* check if location has changed */
    if (strcmp(&tmc_db_el_filename[0], &filename[0]) != 0)
    {
        strncpy(&tmc_db_el_filename[0], &filename[0], sizeof(tmc_db_el_filename));


        /* close database if necessary */
        if (rds_oda_tmc_db_el != NULL)
            (void) sqlite3_close(rds_oda_tmc_db_el);


        /* open database */
        if (sqlite3_open(&tmc_db_el_filename[0], /*@-nullstate@*/ &rds_oda_tmc_db_el /*@+nullstate@*/) != 0)
        {
            if (rds_oda_tmc_db_el != NULL)
                (void) sqlite3_close(rds_oda_tmc_db_el);
            printf("Failed to open eventlist db\n");
            return EXIT_FAILURE;
        }

        /* read in all language-independent constants from Event List */
        (void) snprintf(sql, sizeof(sql), "select CODE,N,Q,T,D,U,C from EL");
        /*@-nullpass@*/
        (void) sqlite3_prepare(rds_oda_tmc_db_el, sql, (int) sizeof(sql), /*@-nullstate@*/ &stmt /*@+nullstate@*/, NULL);
        while (stmt != NULL && (sqlite3_step(stmt) == SQLITE_ROW))
        {
            code = (uint16_t) sqlite3_column_int(stmt, 0);
            e = &rds_oda_tmc_event_list[code];

            /* N(ature): ''=Info 'F'=Forecast 'S'=Silent */
            /*@+ignoresigns@ @-mustfreefresh@*/
            (void) strncpy(&e->n, (const char *)sqlite3_column_text(stmt, 1), 1);
            /*@-ignoresigns@ @+mustfreefresh@*/
            if ((e->n != 'F') && (e->n != 'S'))
                e->n = 'I';

            /* Q(uantifier): 0..12 */
            e->q = (uint8_t) sqlite3_column_int(stmt, 2);

            /* (Duration) T(ype): 'D'=Dynamic 'L'=Longer-lasting '(D)' '(L)' */
            /*@+ignoresigns@ @-mustfreefresh@*/
            (void) strncpy(&s[0], (const char *)sqlite3_column_text(stmt, 3), sizeof(s));
            /*@-ignoresigns@ @+mustfreefresh@*/
            if (strcmp(&s[0], "D") == 0)
            {
                e->t = 'D';
            }
            else if (strcmp(&s[0], "L") == 0)
            {
                e->t = 'L';
            }
            else if (strcmp(&s[0], "(D)") == 0)
            {
                e->t = 'd';
            }
            else if (strcmp(&s[0], "(L)") == 0)
            {
                e->t = 'l';
            }

            /* D(irectionality): 1..2 */
            e->d = (uint8_t) sqlite3_column_int(stmt, 4);

            /* U(rgency): ' ' 'U' 'X' */
            /*@+ignoresigns@ @-mustfreefresh@*/
            (void) strncpy(&e->u, (const char *)sqlite3_column_text(stmt, 5), 1);
            /*@-ignoresigns@ @+mustfreefresh@*/
            if ((e->u != 'U') && (e->u != 'X'))
                e->u = 'N';

            /* (Update) C(lass): 1..31=EL,32..39=FEL */
            e->c = (uint8_t) sqlite3_column_int(stmt, 6);
        }
        (void) sqlite3_finalize(stmt);

        /* read in all language-independent constants from Forecast Event List */
        (void) snprintf(sql, sizeof(sql), "select CODE,N,Q,T,D,U,C from FEL");
        (void) sqlite3_prepare(rds_oda_tmc_db_el, sql, (int) sizeof(sql), /*@-nullstate@*/ &stmt /*@+nullstate@*/, NULL);
        while (stmt != NULL && (sqlite3_step(stmt) == SQLITE_ROW))
        {
            code = (uint16_t) sqlite3_column_int(stmt, 0);
            e = &rds_oda_tmc_event_list[code];

            /* N(ature): ''=Info 'F'=Forecast 'S'=Silent */
            /*@+ignoresigns@ @-mustfreefresh@*/
            (void) strncpy(&e->n, (const char *)sqlite3_column_text(stmt, 1), 1);
            /*@-ignoresigns@ @+mustfreefresh@*/
            if ((e->n != 'F') && (e->n != 'S'))
                e->n = 'I';

            /* Q(uantifier): 0..12 */
            e->q = (uint8_t) sqlite3_column_int(stmt, 2);

            /* (Duration) T(ype): 'D'=Dynamic 'L'=Longer-lasting '(D)' '(L)' */
            /*@+ignoresigns@ @-mustfreefresh@*/
            (void) strncpy(s, (const char *)sqlite3_column_text(stmt, 3), sizeof(s));
            /*@-ignoresigns@ @+mustfreefresh@*/
            if (strcmp(s, "D") == 0)
            {
                e->t = 'D';
            }
            else if (strcmp(s, "L") == 0)
            {
                e->t = 'L';
            }
            else if (strcmp(s, "(D)") == 0)
            {
                e->t = 'd';
            }
            else if (strcmp(s, "(L)") == 0)
            {
                e->t = 'l';
            }

            /* D(irectionality): 1..2 */
            e->d = (uint8_t) sqlite3_column_int(stmt, 4);

            /* U(rgency): ' ' 'U' 'X' */
            /*@+ignoresigns@ @-mustfreefresh@*/
            (void) strncpy(&e->u, (const char *)sqlite3_column_text(stmt, 5), 1);
            /*@-ignoresigns@ @+mustfreefresh@*/
            if ((e->u != 'U') && (e->u != 'X'))
                e->u = 'N';

            /* (Update) C(lass): 1..31=EL,32..39=FEL */
            e->c = (uint8_t) sqlite3_column_int(stmt, 6);
        }
        (void) sqlite3_finalize(stmt);
        /*@+nullpass@*/
    }

    return EXIT_SUCCESS;
}
/*@+globstate@*/


/**
 * \brief Get back Event or Phrase Code string
 *
 * This function returns the Phrase (A..Y) or Supplementary Info (Z) string.
 *
 * \param[out] _str  Return Event or Phrase Code string
 * \param[in]  _size Return string maximum length
 * \param[in]  _l    Letter (A..Y=Phrase Code) (Z=Phrase Code)
 * \param[in]  _n    Number in case of _l='Z', Event Code otherwise
 * \param[in]  _q    Number of quantifiers
 */
void rds_oda_tmc_get_phrase(char *_str, size_t _size, char _l, uint16_t _n, uint8_t _q)
{
    char sql[] = "select TEXT,TEXT_Q0,TEXT_Q1,TEXT_QN from SIL where L='_' and N=___";
    sqlite3_stmt *stmt = NULL;

    /* clear buffer */
    memset(_str, 0, _size);

    /* open TMC Event List database */
    if (tmc_db_el_open() != EXIT_SUCCESS)
        return;

    /* prepare string */
    if (_l == 'Z')
    {
        (void) snprintf(sql, sizeof(sql), "select TEXT,TEXT_Q0,TEXT_Q1,TEXT_QN from SIL where L='%c' and N=%hu", _l, (unsigned short int) _n);
    }
    else
    {
        uint8_t c = rds_oda_tmc_event_list[_n].c;
        if ((c >= 1) && (c <= 31))
        {
            (void) snprintf(sql, sizeof(sql), "select TEXT,TEXT_Q0,TEXT_Q1,TEXT_QN from EL where CODE=%hu", (unsigned short int) _n);
        }
        else if ((c >= 32) && (c <= 39))
        {
            (void) snprintf(sql, sizeof(sql), "select TEXT,TEXT_Q0,TEXT_Q1,TEXT_QN from FEL where CODE=%hu", (unsigned short int) _n);
        }
        else
        {
            return;
        }
    }
    /*@-nullpass@*/
    (void) sqlite3_prepare(rds_oda_tmc_db_el, sql, (int) sizeof(sql), /*@-nullstate@*/ &stmt /*@-nullstate@*/, NULL);

    /* retrieve SQL sentence */
    if (stmt != NULL && sqlite3_step(stmt) == SQLITE_ROW)
    {
        /*@+ignoresigns@ @-mustfreefresh@*/
        switch(_q)
        {
        case 0:
            /* specific TEXT_Q0 */
            (void) strncpy(_str, (const char *)sqlite3_column_text(stmt, 1), _size);
            break;
        case 1:
            /* specific TEXT_Q1 */
            (void) strncpy(_str, (const char *)sqlite3_column_text(stmt, 2), _size);
            break;
        default:
            /* specific TEXT_QN */
            (void) strncpy(_str, (const char *)sqlite3_column_text(stmt, 3), _size);
            break;
        }

        /* fallback to default TEXT */
        if (strlen(_str) == 0)
            (void) strncpy(_str, (const char *)sqlite3_column_text(stmt, 0), _size);
        /*@-ignoresigns@ @+mustfreefresh@*/
    }

    (void) sqlite3_finalize(stmt);
    /*@+nullpass@*/
    convert_rds_charset_to_ascii(_str);
}


/**
 * \brief Open TMC Location Code List database
 *
 * This function checks if the database is open.
 * In case it's not it opens the database.
 *
 * \return Exit status (EXIT_SUCCESS, EXIT_FAILURE)
 */
/*@-globstate@*/
static int tmc_db_lcl_open(void)
{
    char filename[80];
    char sql[] = "select LID from LANGUAGES where CID=___";
    sqlite3_stmt *stmt = NULL;

    /* check if it's already open */
    if (rds_oda_tmc_db_lcl != NULL)
        return EXIT_SUCCESS;

    /* create database filename */

    (void) snprintf(&filename[0], sizeof(filename), "%s/tmc_lcl_%2.2X_%1.1X_%2.2X.db",
                    /*@-unrecog@*/ DATADIR /*@+unrecog@*/,
                    (unsigned short int) 0xE0,//rds_program_current->ecc, TODO: Why ECC is not correct?
                    (unsigned short int) rds_pi_cc(rds_program_current->pi),
                    (unsigned short int) rds_oda_tmc_dsp_current->ltn);

    /* check if location has changed */
    if (strcmp(&tmc_db_lcl_filename[0], &filename[0]) != 0)
    {
        strncpy(&tmc_db_lcl_filename[0], &filename[0], sizeof(tmc_db_lcl_filename));

        /* close database if necessary */
        if (rds_oda_tmc_db_lcl != NULL)
            (void) sqlite3_close(rds_oda_tmc_db_lcl);

        /* open database */
        /*@-nullpass@*/
        if (sqlite3_open(&tmc_db_lcl_filename[0], &rds_oda_tmc_db_lcl) != 0)
        {
            (void) sqlite3_close(rds_oda_tmc_db_lcl);
            printf("Failed to open lcl db\n");
            return EXIT_FAILURE;
        }

        /* read language ID */
        (void) snprintf(&sql[0], sizeof(sql), "select LID from LANGUAGES where CID=%hu", 58);
           //             (unsigned short int) rds_program_current->oda_tmc_cid);
        (void) sqlite3_prepare(rds_oda_tmc_db_lcl, sql, (int) sizeof(sql), &stmt, NULL);
        if(stmt == NULL) {
            printf("Not found CID: %u -- %s\n %i",(unsigned short int) rds_program_current->oda_tmc_cid, sql, sqlite3_prepare(rds_oda_tmc_db_lcl, sql, (int) sizeof(sql), &stmt, NULL));
        }
        /* retrieve SQL sentence */
        if (stmt != NULL && sqlite3_step(stmt) == SQLITE_ROW)
        {
            rds_program_current->oda_tmc_lid = (uint8_t) sqlite3_column_int(stmt, 0);
        }
        /*@+nullpass@*/
    }

    return EXIT_SUCCESS;
}
/*@+globstate@*/


/**
 * \brief Get TMC location information
 *
 * This function returns a TMC location information.
 *
 * \param[in]  _lcd Location Code
 * \param[out] _l   Location Information
 */
void rds_oda_tmc_lcl_get_location(uint16_t _lcd, rds_oda_tmc_lcl_location_t *_l)
{
    rds_ecc_get_iso_itu_cid();

    char sql[] = "select CLASS,TCD,STCD,JUNCTIONNUMBER,RNID,N1ID,N2ID,POL_LCD,OTH_LCD,SEG_LCD,ROA_LCD,INPOS,INNEG,OUTPOS,OUTNEG,PRESENTPOS,PRESENTNEG,DIVERSIONPOS,DIVERSIONNEG,XCOORD,YCOORD,INTERRUPTSROAD,URBAN from POINTS where CID=__ and TABCD=_ and LCD=_____";
    sqlite3_stmt *stmt = NULL;

    /* clear location */
    memset(_l, 0, sizeof(rds_oda_tmc_lcl_location_t));

    /* open LCL database */
    if (tmc_db_lcl_open() != EXIT_SUCCESS)
        return;

    /* preset identical values from columns CID, TABCD and LCD */
    _l->cid = rds_program_current->oda_tmc_cid;
    _l->tabcd = rds_oda_tmc_dsp_current->ltn;
    _l->lcd = _lcd;

    /* 1: Countries */
    /* 2: LocationDataSets */
    /* 3: Locationcodes */
    /* 4: Classes */
    /* 5: Types */
    /* 6: Subtypes */
    /* 7: Languages */
    /* 8: EuroRoadNo */
    /* 9: Names */
    /* 10: NameTranslations */
    /* 11: SubtypeTranslations */
    /* 12: ERNo_belongs_to_country */

    /* 13: AdministrativeAreas */
    if (_l->tclass == (char) 0)
    {
//        memset(sql, 0, sizeof(sql));
        (void) snprintf(sql, sizeof(sql), "select CLASS,TCD,STCD,NID,POL_LCD from ADMINISTRATIVEAREA where CID=%hu and TABCD=%hu and LCD=%hu",
                        (unsigned short int) _l->cid,
                        (unsigned short int) _l->tabcd,
                        (unsigned short int) _l->lcd);
        /*@-nullpass@*/
        (void) sqlite3_prepare(rds_oda_tmc_db_lcl, sql, (int) sizeof(sql), &stmt, NULL);
        if(stmt == NULL) {
            printf("%s\n %i", sql, sqlite3_prepare(rds_oda_tmc_db_lcl, sql, (int) sizeof(sql), &stmt, NULL));
        }
        if (stmt != NULL && sqlite3_step(stmt) == SQLITE_ROW)
        {
            /*@+ignoresigns@ @-mustfreefresh@*/
            strncpy(&_l->tclass, (const char *)sqlite3_column_text(stmt, 0), 1);
            /*@-ignoresigns@ @+mustfreefresh@*/
            _l->tcd = (uint16_t) sqlite3_column_int(stmt, 1);
            _l->stcd = (uint16_t) sqlite3_column_int(stmt, 2);
            _l->nid = (uint16_t) sqlite3_column_int(stmt, 3);
            _l->pol_lcd = (uint16_t) sqlite3_column_int(stmt, 4);
        }
        (void) sqlite3_finalize(stmt);
        /*@+nullpass@*/
    }

    /* 14: OtherAreas */
    if (_l->tclass == (char) 0)
    {
        (void) snprintf(sql, sizeof(sql), "select CLASS,TCD,STCD,NID,POL_LCD from OTHERAREAS where CID=%hu and TABCD=%hu and LCD=%hu",
                        (unsigned short int) _l->cid,
                        (unsigned short int) _l->tabcd,
                        (unsigned short int) _l->lcd);
        /*@-nullpass@*/
        (void) sqlite3_prepare(rds_oda_tmc_db_lcl, sql, (int) sizeof(sql), &stmt, NULL);
        if(stmt == NULL) {
            printf("%s\n %i", sql, sqlite3_prepare(rds_oda_tmc_db_lcl, sql, (int) sizeof(sql), &stmt, NULL));
        }
        if (stmt != NULL && sqlite3_step(stmt) == SQLITE_ROW)
        {
            /*@+ignoresigns@ @-mustfreefresh@*/
            strncpy(&_l->tclass, (const char *)sqlite3_column_text(stmt, 0), 1);
            /*@-ignoresigns@ @+mustfreefresh@*/
            _l->tcd = (uint16_t) sqlite3_column_int(stmt, 1);
            _l->stcd = (uint16_t) sqlite3_column_int(stmt, 2);
            _l->nid = (uint16_t) sqlite3_column_int(stmt, 3);
            _l->pol_lcd = (uint16_t) sqlite3_column_int(stmt, 4);
        }
        (void) sqlite3_finalize(stmt);
        /*@+nullpass@*/
    }

    /* 15: Roads */
    if (_l->tclass == (char) 0)
    {
        (void) snprintf(sql, sizeof(sql), "select CLASS,TCD,STCD,ROADNUMBER,RNID,N1ID,N2ID,POL_LCD,PES_LEV from ROADS where CID=%hu and TABCD=%hu and LCD=%hu",
                        (unsigned short int) _l->cid,
                        (unsigned short int) _l->tabcd,
                        (unsigned short int) _l->lcd);
        /*@-nullpass@*/
        (void) sqlite3_prepare(rds_oda_tmc_db_lcl, sql, (int) sizeof(sql), &stmt, NULL);
        if(stmt == NULL) {
            printf("%s\n %i", sql, sqlite3_prepare(rds_oda_tmc_db_lcl, sql, (int) sizeof(sql), &stmt, NULL));
        }
        if (stmt != NULL && sqlite3_step(stmt) == SQLITE_ROW)
        {
            /*@+ignoresigns@ @-mustfreefresh@*/
            strncpy(&_l->tclass, (const char *)sqlite3_column_text(stmt, 0), 1);
            /*@-ignoresigns@ @+mustfreefresh@*/
            _l->tcd = (uint16_t) sqlite3_column_int(stmt, 1);
            _l->stcd = (uint16_t) sqlite3_column_int(stmt, 2);
            /*@+ignoresigns@ @-mustfreefresh@*/
            strncpy(&_l->roadnumber[0], (const char *)sqlite3_column_text(stmt, 3), 11);
            /*@-ignoresigns@ @+mustfreefresh@*/
            _l->rnid = (uint16_t) sqlite3_column_int(stmt, 4);
            _l->n1id = (uint16_t) sqlite3_column_int(stmt, 5);
            _l->n2id = (uint16_t) sqlite3_column_int(stmt, 6);
            _l->pol_lcd = (uint16_t) sqlite3_column_int(stmt, 7);
            _l->pes_lev = (uint8_t) sqlite3_column_int(stmt, 8);
        }
        (void) sqlite3_finalize(stmt);
        /*@+nullpass@*/
    }

    /* 16: Road_network_level_types */

    /* 17: Segments */
    if (_l->tclass == (char) 0)
    {
        (void) snprintf(sql, sizeof(sql), "select CLASS,TCD,STCD,ROADNUMBER,RNID,N1ID,N2ID,ROA_LCD,SEG_LCD,POL_LCD from SEGMENTS where CID=%hu and TABCD=%hu and LCD=%hu",
                        (unsigned short int) _l->cid,
                        (unsigned short int) _l->tabcd,
                        (unsigned short int) _l->lcd);
        /*@-nullpass@*/
        (void) sqlite3_prepare(rds_oda_tmc_db_lcl, sql, (int) sizeof(sql), &stmt, NULL);
        if(stmt == NULL) {
            printf("%s\n %i", sql, sqlite3_prepare(rds_oda_tmc_db_lcl, sql, (int) sizeof(sql), &stmt, NULL));
        }
        if (stmt != NULL && sqlite3_step(stmt) == SQLITE_ROW)
        {
            /*@+ignoresigns@ @-mustfreefresh@*/
            strncpy(&_l->tclass, (const char *)sqlite3_column_text(stmt, 0), 1);
            /*@-ignoresigns@ @+mustfreefresh@*/
            _l->tcd = (uint16_t) sqlite3_column_int(stmt, 1);
            _l->stcd = (uint16_t) sqlite3_column_int(stmt, 2);
            /*@+ignoresigns@ @-mustfreefresh@*/
            strncpy(&_l->roadnumber[0], (const char *)sqlite3_column_text(stmt, 3), 11);
            /*@-ignoresigns@ @+mustfreefresh@*/
            _l->rnid = (uint16_t) sqlite3_column_int(stmt, 4);
            _l->n1id = (uint16_t) sqlite3_column_int(stmt, 5);
            _l->n2id = (uint16_t) sqlite3_column_int(stmt, 6);
            _l->roa_lcd = (uint16_t) sqlite3_column_int(stmt, 7);
            _l->seg_lcd = (uint16_t) sqlite3_column_int(stmt, 8);
            _l->pol_lcd = (uint16_t) sqlite3_column_int(stmt, 9);
        }
        (void) sqlite3_finalize(stmt);
        /*@+nullpass@*/
    }

    /* 18: Soffsets */
    if ((_l->neg_off_lcd == 0) && (_l->pos_off_lcd == 0))
    {
        (void) snprintf(sql, sizeof(sql), "select NEG_OFF_LCD,POS_OFF_LCD from SOFFSETS where CID=%hu and TABCD=%hu and LCD=%hu",
                        (unsigned short int) _l->cid,
                        (unsigned short int) _l->tabcd,
                        (unsigned short int) _l->lcd);
        /*@-nullpass@*/
        (void) sqlite3_prepare(rds_oda_tmc_db_lcl, sql, (int) sizeof(sql), &stmt, NULL);
        if(stmt == NULL) {
            printf("%s\n %i", sql, sqlite3_prepare(rds_oda_tmc_db_lcl, sql, (int) sizeof(sql), &stmt, NULL));
        }
        if (stmt != NULL && sqlite3_step(stmt) == SQLITE_ROW)
        {
            _l->neg_off_lcd = (uint16_t) sqlite3_column_int(stmt, 0);
            _l->pos_off_lcd = (uint16_t) sqlite3_column_int(stmt, 1);
        }
        (void) sqlite3_finalize(stmt);
        /*@+nullpass@*/
    }

    /* 19: Seg_has_ERNo */

    /* 20: Points */
    if (_l->tclass == (char) 0)
    {
        (void) snprintf(sql, sizeof(sql), "select CLASS,TCD,STCD,JUNCTIONNUMBER,RNID,N1ID,N2ID,POL_LCD,OTH_LCD,SEG_LCD,ROA_LCD,INPOS,INNEG,OUTPOS,OUTNEG,PRESENTPOS,PRESENTNEG,DIVERSIONPOS,DIVERSIONNEG,XCOORD,YCOORD,INTERRUPTSROAD,URBAN from POINTS where CID=%hu and TABCD=%hu and LCD=%hu",
                        (unsigned short int) _l->cid,
                        (unsigned short int) _l->tabcd,
                        (unsigned short int) _l->lcd);
        /*@-nullpass@*/
        (void) sqlite3_prepare(rds_oda_tmc_db_lcl, sql, (int) sizeof(sql), &stmt, NULL);
        if(stmt == NULL) {
            printf("%s\n %i", sql, sqlite3_prepare(rds_oda_tmc_db_lcl, sql, (int) sizeof(sql), &stmt, NULL));
        }
        if (stmt != NULL && sqlite3_step(stmt) == SQLITE_ROW)
        {
            /*@+ignoresigns@ @-mustfreefresh@*/
            strncpy(&_l->tclass, (const char *)sqlite3_column_text(stmt, 0), 1);
            /*@-ignoresigns@ @+mustfreefresh@*/
            _l->tcd = (uint16_t) sqlite3_column_int(stmt, 1);
            _l->stcd = (uint16_t) sqlite3_column_int(stmt, 2);
            /*@+ignoresigns@ @-mustfreefresh@*/
            strncpy(&_l->junctionnumber[0], (const char *)sqlite3_column_text(stmt, 3), 11);
            /*@-ignoresigns@ @+mustfreefresh@*/
            _l->rnid = (uint16_t) sqlite3_column_int(stmt, 4);
            _l->n1id = (uint16_t) sqlite3_column_int(stmt, 5);
            _l->n2id = (uint16_t) sqlite3_column_int(stmt, 6);
            _l->pol_lcd = (uint16_t) sqlite3_column_int(stmt, 7);
            _l->oth_lcd = (uint16_t) sqlite3_column_int(stmt, 8);
            _l->seg_lcd = (uint16_t) sqlite3_column_int(stmt, 9);
            _l->roa_lcd = (uint16_t) sqlite3_column_int(stmt, 10);
            _l->inpos = (uint8_t) sqlite3_column_int(stmt, 11);
            _l->inneg = (uint8_t) sqlite3_column_int(stmt, 12);
            _l->outpos = (uint8_t) sqlite3_column_int(stmt, 13);
            _l->outneg = (uint8_t) sqlite3_column_int(stmt, 14);
            _l->presentpos = (uint8_t) sqlite3_column_int(stmt, 15);
            _l->presentneg = (uint8_t) sqlite3_column_int(stmt, 16);
            /*@+ignoresigns@ @-mustfreefresh@*/
            strncpy(&_l->diversionpos[0], (const char *)sqlite3_column_text(stmt, 17), 11);
            strncpy(&_l->diversionneg[0], (const char *)sqlite3_column_text(stmt, 18), 11);
            strncpy(&_l->xcoord[0], (const char *)sqlite3_column_text(stmt, 19), 10);
            strncpy(&_l->ycoord[0], (const char *)sqlite3_column_text(stmt, 20), 9);
            /*@-ignoresigns@ @+mustfreefresh@*/
            _l->interruptsroad = (uint8_t) sqlite3_column_int(stmt, 21);
            _l->urban = (uint8_t) sqlite3_column_int(stmt, 22);
        }
        (void) sqlite3_finalize(stmt);
        /*@+nullpass@*/
    }

    /* 21: Poffsets */
    if ((_l->neg_off_lcd == 0) && (_l->pos_off_lcd == 0))
    {
        (void) snprintf(sql, sizeof(sql), "select NEG_OFF_LCD,POS_OFF_LCD from POFFSETS where CID=%hu and TABCD=%hu and LCD=%hu",
                        (unsigned short int) _l->cid,
                        (unsigned short int) _l->tabcd,
                        (unsigned short int) _l->lcd);
        /*@-nullpass@*/
        (void) sqlite3_prepare(rds_oda_tmc_db_lcl, sql, (int) sizeof(sql), &stmt, NULL);
        if(stmt == NULL) {
            printf("%s\n %i", sql, sqlite3_prepare(rds_oda_tmc_db_lcl, sql, (int) sizeof(sql), &stmt, NULL));
        }
        if (stmt != NULL && sqlite3_step(stmt) == SQLITE_ROW)
        {
            _l->neg_off_lcd = (uint16_t) sqlite3_column_int(stmt, 0);
            _l->pos_off_lcd = (uint16_t) sqlite3_column_int(stmt, 1);
        }
        (void) sqlite3_finalize(stmt);
        /*@+nullpass@*/
    }

    /* 22: Intersections */
}


/**
 * \brief Get TMC location name
 *
 * This function returns a TMC location name.
 *
 * \param[out] _str  Return Name string
 * \param[in]  _size Return string maximum length
 * \param[in]  _nid  Name ID
 */
void rds_oda_tmc_lcl_get_name(char *_str, size_t _size, uint16_t _nid)
{
    char sql[] = "select NAME from NAMES where CID=__ and LID=__ and NID=_____";
    sqlite3_stmt *stmt = NULL;

    /* clear name */
    memset(_str, 0, _size);

    /* open LCL database */
    if (tmc_db_lcl_open() != EXIT_SUCCESS)
        return;

    /* prepare SQL */
    (void) snprintf(sql, sizeof(sql), "select NAME from NAMES where CID=%hu and LID=%hu and NID=%hu",
                    (unsigned short int) rds_program_current->oda_tmc_cid,
                    (unsigned short int) rds_program_current->oda_tmc_lid,
                    (unsigned short int) _nid);
    /*@-nullpass@*/
    (void) sqlite3_prepare(rds_oda_tmc_db_lcl, sql, (int) sizeof(sql), &stmt, NULL);

    /* execute SQL */
    if (stmt != NULL && sqlite3_step(stmt) == SQLITE_ROW)
    {
        /*@+ignoresigns@ @-mustfreefresh@*/
        (void) strncpy(_str, (const char *)sqlite3_column_text(stmt, 0), _size);
        /*@-ignoresigns@ @+mustfreefresh@*/
    }
    (void) sqlite3_finalize(stmt);
    /*@+nullpass@*/
    convert_rds_charset_to_ascii(_str);
}


/**
 * \brief Get TMC (sub)type name
 *
 * This function returns a TMC (sub)type name.
 *
 * \param[out] _str    Return (Sub)Type Name string
 * \param[in]  _size   Return string maximum length
 * \param[in]  _tclass Type Class
 * \param[in]  _tcd    Type Code
 * \param[in]  _stcd   SubType Code
 */
void rds_oda_tmc_lcl_get_type_name(char *_str, size_t _size, char _tclass, uint8_t _tcd, uint8_t _stcd)
{
    char sql[] = "select SDESC,SNATCODE,SNATDESC from SUBTYPES where CLASS='_' and TCD=__ and STCD=__";
    sqlite3_stmt *stmt = NULL;

    /* clear name */
    memset(_str, 0, _size);

    /* open LCL database */
    if (tmc_db_lcl_open() != EXIT_SUCCESS)
        return;

    /* 5: Types */
    /* 6: Subtypes */
    if (_stcd == 0)
    {
        (void) snprintf(sql, sizeof(sql), "select TDESC,TNATCD,TNATDESC from TYPES where CLASS='%c' and TCD=%hu", _tclass, (unsigned short int) _tcd);
    }
    else
    {
        (void) snprintf(sql, sizeof(sql), "select SDESC,SNATCODE,SNATDESC from SUBTYPES where CLASS='%c' and TCD=%hu and STCD=%hu", _tclass, (unsigned short int) _tcd, (unsigned short int) _stcd);
    }
    /*@-nullpass@*/
    (void) sqlite3_prepare(rds_oda_tmc_db_lcl, sql, (int) sizeof(sql), &stmt, NULL);

    if (stmt != NULL && sqlite3_step(stmt) == SQLITE_ROW)
    {
        /*@+ignoresigns@ @-mustfreefresh@*/
        (void) strncpy(_str, (const char *)sqlite3_column_text(stmt, 2), _size);
        /*@-ignoresigns@ @+mustfreefresh@*/
    }

    (void) sqlite3_finalize(stmt);
    /*@+nullpass@*/
    convert_rds_charset_to_ascii(_str);
}


/**
 * \brief Save TMC data service provider
 *
 * This function saves a TMC DSP to persistent storage.
 *
 * \param[in] _dsp TMC data service provider
 */
static void rds_oda_tmc_dsp_save(rds_oda_tmc_dsp_t *_dsp)
{
    char buf[256];
    FILE *file;

    /* open DSP */
    /** \todo dsp directory does not exists */
    (void) snprintf(&buf[0], sizeof(buf),
                    "/var/lib/rds/tmc/dsp/%2.2hX_%2.2hX_%2.2hX",
                    (unsigned short int) _dsp->ecc, (unsigned short int) _dsp->cc, (unsigned short int) _dsp->sid);
    file = fopen(&buf[0], "w");
    if (file == (FILE *) 0)
        return;

    /* save DSP */
    (void) fwrite(_dsp, sizeof(rds_oda_tmc_dsp_t), 1, file);
    /* either the dsp data is write or not... */
    (void) fclose(file);
}


/**
 * \brief Save all TMC data service providers
 *
 * This function saves all TMC DSP to persistent storage.
 */
/*@unused@ */ static void rds_oda_tmc_dsp_save_all(void)
{
    uint8_t i;
    for (i = 0; i < 4; i++)
        if (rds_oda_tmc_dsp[i].sid != 0)
            rds_oda_tmc_dsp_save(&rds_oda_tmc_dsp[i]);
}


/**
 * \brief Load TMC data service provider
 *
 * This function loads a TMC DSP from the persistent storage.
 *
 * \param[in] _ecc ECC Code
 * \param[in] _cc  CC Code
 * \param[in] _sid SID Code
 * \return         Pointer to TMC DSP structure
 */
/*@-globstate@*/
static rds_oda_tmc_dsp_t *rds_oda_tmc_dsp_load(uint8_t _ecc, uint8_t _cc, uint8_t _sid)
{
    int8_t i;
    int8_t first_free_index = -1;
    int8_t oldest_index = -1;
    time_t oldest_update = 0;
    char buf[256];
    FILE *file;

    /* check if this is the current DSP */
    if ((rds_oda_tmc_dsp_current != NULL) &&
            (rds_oda_tmc_dsp_current->ecc == _ecc) &&
            (rds_oda_tmc_dsp_current->cc == _cc) &&
            (rds_oda_tmc_dsp_current->sid == _sid))
    {
        rds_oda_tmc_dsp_current->last_update = time(NULL);
        /*@-immediatetrans@@-unqualifiedtrans@*/
        return rds_oda_tmc_dsp_current;
        /*@+immediatetrans@@+unqualifiedtrans@*/
    }

    /* search if DSP is already loaded */
    for (i = 0; i < 4; i++)
    {
        /* check if this is the correct DSP */
        if ((rds_oda_tmc_dsp[i].ecc == _ecc) &&
                (rds_oda_tmc_dsp[i].cc == _cc) &&
                (rds_oda_tmc_dsp[i].sid == _sid))
        {
            rds_oda_tmc_dsp[i].last_update = time(NULL);
            /*@-immediatetrans@@*/
            return &rds_oda_tmc_dsp[i];
            /*@+immediatetrans@*/
        }

        /* check if this entry is free */
        else if (rds_oda_tmc_dsp[i].sid == 0)
        {
            /* remember first free index */
            if (first_free_index < 0)
                first_free_index = i;
        }

        /* check if this is the oldest DSP */
        else if ((oldest_index < 0) || (rds_oda_tmc_dsp[i].last_update < oldest_update))
        {
            oldest_index = i;
            oldest_update = rds_oda_tmc_dsp[i].last_update;
        }
    }
    /* if we reach this point, the DSP was not found */

    /* load new DSP */
    if (first_free_index >= 0)
    {
        /* use first free index to load DSP */
        i = first_free_index;
    }
    else
    {
        /* all indices in use, replace oldest DSP */
        i = oldest_index;
        rds_oda_tmc_dsp_save(&rds_oda_tmc_dsp[i]);
        memset(&rds_oda_tmc_dsp[i], 0, sizeof(rds_oda_tmc_dsp_t));
    }

    /* open DSP */
    /** \todo dsp directory does not exists */
    (void) snprintf(&buf[0], sizeof(buf),
                    "/var/lib/rds/tmc/dsp/%2.2hX_%2.2hX_%2.2hX",
                    (unsigned short int) _ecc, (unsigned short int) _cc, (unsigned short int) _sid);
    file = fopen(&buf[0], "r");

    /* is DSP found? */
    if (file != (FILE *) 0)
    {
        /* load DSP */
        (void) fread(&rds_oda_tmc_dsp[i], sizeof(rds_oda_tmc_dsp_t), 1, file);
        /* either the dsp data is read or not... */
        (void) fclose(file);
    }
    else
    {
        rds_oda_tmc_dsp[i].ecc = _ecc;
        rds_oda_tmc_dsp[i].cc = _cc;
        rds_oda_tmc_dsp[i].sid = _sid;
    }

    /*@-immediatetrans@@*/
    return &rds_oda_tmc_dsp[i];
    /*@+immediatetrans@@*/
}
/*@+globstate@*/


/**
 * \brief Set encryption table ID
 *
 * This function sets the encryption table ID.
 *
 * \param[in] _tabid Encryption table ID
 */
/*@-globstate@*/
void rds_oda_tmc_set_enc_tabid(uint8_t _tabid)
{
    if (rds_oda_tmc_dsp_current != NULL)
        rds_oda_tmc_dsp_current->enc_tabid = _tabid;
}
/*@+globstate@*/


/**
 * \brief Delete message from message memory
 *
 * This function deletes an entry in the message memory.
 *
 * \param[in] _i Message memory index
 */
static void tmc_delete_message(uint16_t _i)
{
    /* execute callback with 2="problem cleared" */
    if (rds_oda_tmc_callback != NULL)
        rds_oda_tmc_callback(&rds_oda_tmc_dsp_current->msg, 2);

    /* delete */
    rds_oda_tmc_dsp_current->message_memory_cnt--;
    if (rds_oda_tmc_dsp_current->message_memory_cnt > 0)
    {
        memcpy(&rds_oda_tmc_dsp_current->message_memory[_i], &rds_oda_tmc_dsp_current->message_memory[rds_oda_tmc_dsp_current->message_memory_cnt], sizeof(rds_oda_tmc_message_t));
        memset(&rds_oda_tmc_dsp_current->message_memory[rds_oda_tmc_dsp_current->message_memory_cnt], 0, sizeof(rds_oda_tmc_message_t));
    }
    else
    {
        memset(&rds_oda_tmc_dsp_current->message_memory[_i], 0, sizeof(rds_oda_tmc_message_t));
    }
}


/**
 * \brief Automatically delete message from message memory
 *
 * This function handles expiry, stop and decrement times.
 */
void rds_oda_tmc_message_memory_autoclean(void)
{
    uint16_t i;
    time_t time_now;

    if (rds_program_current == NULL)
        return;
    if (rds_oda_tmc_dsp_current == NULL)
        rds_oda_tmc_dsp_current = rds_oda_tmc_dsp_load(rds_program_current->ecc, rds_pi_cc(rds_program_current->pi), rds_program_current->oda_tmc_sid);

    time_now = rds_program_current->ct;

    for (i = 0; i < rds_oda_tmc_dsp_current->message_memory_cnt; i++)
    {
        /* expiry time */
        if ((rds_oda_tmc_dsp_current->message_memory[i].expiry_time > 0) &&
                (rds_oda_tmc_dsp_current->message_memory[i].expiry_time <= time_now))
        {
            tmc_delete_message(i);
            i--;    /* check location again */
        }
        else

            /* stop time */
            if ((rds_oda_tmc_dsp_current->message_memory[i].stop_time > 0) &&
                    (rds_oda_tmc_dsp_current->message_memory[i].stop_time <= time_now))
            {
                tmc_delete_message(i);
                i--;    /* check location again */
            }
            else

                /* silent cancellation message */
                if (rds_oda_tmc_dsp_current->message_memory[i].dir_ub == 0)
                {
                    /* silent cancellation message (no directionality set) */
                    tmc_delete_message(i);  /** \todo do it silent */
                    i--;    /* check location again */
                }
                else

                    /* decrement time */
                    if ((rds_oda_tmc_dsp_current->message_memory[i].decrement_time > 0) &&
                            (rds_oda_tmc_dsp_current->message_memory[i].decrement_time <= time_now))
                    {
                        if (rds_oda_tmc_dsp_current->message_memory[i].dur > 0)
                            rds_oda_tmc_dsp_current->message_memory[i].dur--;   /* decrement by one */
                        rds_oda_tmc_dsp_current->message_memory[i].decrement_time = 0;  /* default, no new decrement time */

                        /* set decrement time */
                        if (((char) toupper(rds_oda_tmc_dsp_current->message_memory[i].dur_dl) == 'D') &&
                                ((rds_oda_tmc_dsp_current->message_memory[i].nat     == 'I') ||
                                 (rds_oda_tmc_dsp_current->message_memory[i].nat     == 'F')))
                        {
                            /* D: dynamic, I: information */
                            /* D: dynamic, F: forecast */
                            switch(rds_oda_tmc_dsp_current->message_memory[i].dur)
                            {
                            case 2:
                                /* 15 minutes */
                                rds_oda_tmc_dsp_current->message_memory[i].decrement_time += 15*60;
                                break;
                            case 3:
                                /* 30 minutes */
                                rds_oda_tmc_dsp_current->message_memory[i].decrement_time += 30*60;
                                break;
                            case 4:
                            case 5:
                            case 6:
                                /* 1 hour */
                                rds_oda_tmc_dsp_current->message_memory[i].decrement_time += 1*60*60;
                                break;
                            default: /* case 0, 1, 7 */
                                break;
                            }
                        }
                        else if (((char) toupper(rds_oda_tmc_dsp_current->message_memory[i].dur_dl) == 'L') &&
                                 (rds_oda_tmc_dsp_current->message_memory[i].nat     == 'I'))
                        {
                            /* L: longer-lasting, I: information */
                            switch(rds_oda_tmc_dsp_current->message_memory[i].dur)
                            {
                            case 3:
                                /* midnight */
                                rds_oda_tmc_dsp_current->message_memory[i].decrement_time =
                                    rds_ct_start_of_day(rds_oda_tmc_dsp_current->message_memory[i].decrement_time, 1);
                                break;
                            case 4:
                                /* Friday midnight, i.e. 00:00 Saturday */
                                rds_oda_tmc_dsp_current->message_memory[i].decrement_time =
                                    rds_ct_midnight(rds_oda_tmc_dsp_current->message_memory[i].decrement_time, 5);
                                break;
                            case 5:
                                /* Sunday midnight, i.e. 00:00 Monday */
                                rds_oda_tmc_dsp_current->message_memory[i].decrement_time =
                                    rds_ct_midnight(rds_oda_tmc_dsp_current->message_memory[i].decrement_time, 7);
                                break;
                            default: /* case 0, 1, 2, 6, 7 */
                                break;
                            }
                        }
                        else if (((char) toupper(rds_oda_tmc_dsp_current->message_memory[i].dur_dl) == 'L') &&
                                 (rds_oda_tmc_dsp_current->message_memory[i].nat     == 'F'))
                        {
                            /* L: longer-lasting, F: forecast */
                            switch(rds_oda_tmc_dsp_current->message_memory[i].dur)
                            {
                            case 3:
                            case 4:
                                /* midnight */
                                rds_oda_tmc_dsp_current->message_memory[i].decrement_time =
                                    rds_ct_start_of_day(rds_oda_tmc_dsp_current->message_memory[i].decrement_time, 1);
                                break;
                            default: /* case 0, 1, 2, 5, 6, 7 */
                                break;
                            }
                        }

                        /* inform about update */
                        if (rds_oda_tmc_callback != NULL)
                            rds_oda_tmc_callback(&rds_oda_tmc_dsp_current->message_memory[i], 1);
                    }
    }
}


/**
 * \brief Return start/stop time based on free format data
 *
 * Returns the start/stop time based on free format data.
 *
 * \param[in] _time Timebase for calculation
 * \param[in] _data Free format data for start/stop time
 * \return          Unix time
 */
static time_t tmc_start_stop_time(time_t _time, uint8_t _data)
{
    struct tm *tm;
    char *tz;

    /* save current time zone */
    tzset();
    tz = getenv("TZ");

    /* set time zone to UTC */
    (void) setenv("TZ", "", 1);
    tzset();

    /* get time structure */
    tm = gmtime(&_time);

    if (_data <= 95)    /* 0..95 */
    {
        /* 00:00 to 23:45 (15 minute interval) */
        /* spoken as "today at &&:&& UTC" */
        tm->tm_hour = (int) ((_data * 15) / 60);
        tm->tm_min = (int) ((_data * 15) % 60);
        tm->tm_sec = 0;
        tm->tm_isdst = -1;
        _time = mktime(tm);

    }
    else if ((_data >= 96) && (_data <= 200))       /* 96..200 */
    {
        /* Hour and day, starting at midnight following message receipt (1 hour interval) */
        /* spoken as "in %% days at %%:00 UTC" */
        tm->tm_mday += (_data - 95) / 24;
        if (tm->tm_hour >= (int) ((_data - 96) % 24))
            tm->tm_mday++;
        tm->tm_hour = (int) ((_data - 96) % 24);
        tm->tm_min = 0;
        tm->tm_sec = 0;
        tm->tm_isdst = -1;
        _time = mktime(tm);

    }
    else if ((_data >= 201) && (_data <= 231))      /* 201..231 */
    {
        /* 1st to 31st day of the month (1 day interval) */
        /* spoken as "in %% days" */
        if (tm->tm_mday >= (int) (_data - 200))
            tm->tm_mon++;
        tm->tm_mday = (int) (_data - 200);
        tm->tm_hour = 12;
        tm->tm_min = 0;
        tm->tm_sec = 0;
        tm->tm_isdst = -1;
        _time = mktime(tm);

    }
    else     /* 232..255 */
    {
        /* 15th January to 31st December (half month interval) */
        /* spoken as "mid/end of %%.month" */
        if (tm->tm_mon >= (int) (_data - 232))
            tm->tm_year++;
        tm->tm_mon = (int) ((_data - 231) / 2);
        tm->tm_mday = (((_data - 231) % 2) == 1) ? 15 : 0;
        tm->tm_hour = 12;
        tm->tm_min = 0;
        tm->tm_sec = 0;
        tm->tm_isdst = -1;
        _time = mktime(tm);
    }

    /* reset time zone */
    if (tz)
        (void) setenv("TZ", tz, 1);
    else
        (void) unsetenv("TZ");
    tzset();

    return _time;
}


/**
 * \brief Handle a TMC message
 *
 * This function handles the current TMC message.
 */
static void tmc_handle_message(void)
{
    uint16_t i;
    uint8_t q;

    /* set receive time */
    rds_oda_tmc_dsp_current->msg.receive_time = rds_program_current->ct;

    /* don't handle encrypted messages without knowing the encryption key */
    if ((rds_oda_tmc_dsp_current->ltn == 0) && (rds_oda_tmc_dsp_current->enc_ltnbe == 0))
    {
        /* clear message */
        memset(&rds_oda_tmc_dsp_current->msg, 0, sizeof(rds_oda_tmc_message_t));
        return;
    }

    /* fill in receiver information */
    rds_oda_tmc_dsp_current->msg.ecc = rds_program_current->ecc;
    rds_oda_tmc_dsp_current->msg.cc  = rds_pi_cc(rds_program_current->pi);
    rds_oda_tmc_dsp_current->msg.ltn = rds_oda_tmc_dsp_current->ltn;
    rds_oda_tmc_dsp_current->msg.sid = rds_oda_tmc_dsp_current->sid;

    /* decrypt location */
    if (rds_oda_tmc_dsp_current->ltn == 0)
    {
        uint8_t xor_value   = 0;
        uint8_t start_bit   = 0;
        uint8_t rotate_left = 0;

        rds_oda_tmc_dsp_current->msg.ltn = rds_oda_tmc_dsp_current->enc_ltnbe;

        switch (rds_oda_tmc_dsp_current->enc_test)
        {
        case 0:
            /* no encryption */
            break;
        case 1:
            /* encrypt using encryption parameters pre-advised by the service provider */
            /* the values from the example in DIN_EN_ISO_14819-6 are: xor value = 0x39, start bit = 8, rotate left = 1 */
            xor_value   = rds_oda_tmc_dsp_current->enc_key_table_pre_adviced[0];
            start_bit   = rds_oda_tmc_dsp_current->enc_key_table_pre_adviced[1];
            rotate_left = rds_oda_tmc_dsp_current->enc_key_table_pre_adviced[2];
            break;
        case 3:
            /* full encryption */
            xor_value   = rds_oda_tmc_dsp_current->enc_key_table[rds_oda_tmc_dsp_current->enc_tabid][rds_oda_tmc_dsp_current->enc_encid][0];
            start_bit   = rds_oda_tmc_dsp_current->enc_key_table[rds_oda_tmc_dsp_current->enc_tabid][rds_oda_tmc_dsp_current->enc_encid][1];
            rotate_left = rds_oda_tmc_dsp_current->enc_key_table[rds_oda_tmc_dsp_current->enc_tabid][rds_oda_tmc_dsp_current->enc_encid][2];
            break;
        default:
            /* 2 is reserved for future use */
            rds_decode_status = RDS_DECODE_STATUS_NOT_ASSIGNED;
            break;
        }

        /* decrypt */
        rds_oda_tmc_dsp_current->msg.loc ^= (xor_value << start_bit);
        rds_oda_tmc_dsp_current->msg.loc = (rds_oda_tmc_dsp_current->msg.loc << rotate_left) | (rds_oda_tmc_dsp_current->msg.loc >> (16-rotate_left));
    }

    /* count optional message content qualifiers */
    q = 0;
    for (i = 0; i < rds_oda_tmc_dsp_current->msg.opt_cnt; i++)
    {
        //uint16_t data = rds_oda_tmc_dsp_current->msg.opt[i].data;

        switch (rds_oda_tmc_dsp_current->msg.opt[i].label)
        {
        case 4:
        case 5:
            /* Quantifier */
            q++;
            break;
        default:
            break;
        }
    }

    /* open TMC Event List database */
    if (tmc_db_el_open() != EXIT_SUCCESS)
        return;

    /* fill in implicit information from event list */
    rds_oda_tmc_dsp_current->msg.nat    = rds_oda_tmc_event_list[rds_oda_tmc_dsp_current->msg.evt].n;   /* N(ature): 'I'=Info 'F'=Forecast 'S'=Silent */
    rds_oda_tmc_dsp_current->msg.qnt    = rds_oda_tmc_event_list[rds_oda_tmc_dsp_current->msg.evt].q;   /* Q(uantifier): 0..12 */
    rds_oda_tmc_dsp_current->msg.dur_dl = rds_oda_tmc_event_list[rds_oda_tmc_dsp_current->msg.evt].t;   /* (Duration) T(ype): 'D'=Dynamic 'L'=Longer-lasting 'd' 'l' */
    rds_oda_tmc_dsp_current->msg.dir_ub = rds_oda_tmc_event_list[rds_oda_tmc_dsp_current->msg.evt].d;   /* D(irectionality): 1..2 */
    rds_oda_tmc_dsp_current->msg.urg    = rds_oda_tmc_event_list[rds_oda_tmc_dsp_current->msg.evt].u;   /* U(rgency): 'N' 'U' 'X' */
    rds_oda_tmc_dsp_current->msg.upc    = rds_oda_tmc_event_list[rds_oda_tmc_dsp_current->msg.evt].c;   /* (Update) C(lass): 1..31=EL,32..39=FEL */

    /* process optional message content */
    for (i = 0; i < rds_oda_tmc_dsp_current->msg.opt_cnt; i++)
    {
        uint16_t data = rds_oda_tmc_dsp_current->msg.opt[i].data;

        switch (rds_oda_tmc_dsp_current->msg.opt[i].label)
        {
        case 0:
            /* Duration */
            if (data == 0)
            {
                /* 0 is not allowed in multi-group messages */
                /* we use it here to terminate the message */
                rds_oda_tmc_dsp_current->msg.opt_cnt = i;
            }
            else
                rds_oda_tmc_dsp_current->msg.dur = data;
            break;
        case 1:
            /* Control code */
            switch (data)
            {
            case 0:
                /* Default urgency increased by one level */
                if (rds_oda_tmc_dsp_current->msg.urg == 'N')
                    rds_oda_tmc_dsp_current->msg.urg = 'U';
                else if (rds_oda_tmc_dsp_current->msg.urg == 'U')
                    rds_oda_tmc_dsp_current->msg.urg = 'X';
                else
                    rds_oda_tmc_dsp_current->msg.urg = 'N';
                break;
            case 1:
                /* Default urgency reduced by one level */
                if (rds_oda_tmc_dsp_current->msg.urg == 'X')
                    rds_oda_tmc_dsp_current->msg.urg = 'U';
                else if (rds_oda_tmc_dsp_current->msg.urg == 'U')
                    rds_oda_tmc_dsp_current->msg.urg = 'N';
                else
                    rds_oda_tmc_dsp_current->msg.urg = 'X';
                break;
            case 2:
                /* Default directionality of message changed */
                if (rds_oda_tmc_dsp_current->msg.dir_ub == 1)
                    rds_oda_tmc_dsp_current->msg.dir_ub = 2;
                else
                    rds_oda_tmc_dsp_current->msg.dir_ub = 1;
                break;
            case 3:
                /* Default dynamic or longer-lasting provision interchanged */
                if (rds_oda_tmc_dsp_current->msg.dur_dl == 'D')
                    rds_oda_tmc_dsp_current->msg.dur_dl = 'L';
                else if (rds_oda_tmc_dsp_current->msg.dur_dl == 'L')
                    rds_oda_tmc_dsp_current->msg.dur_dl = 'D';
                else if (rds_oda_tmc_dsp_current->msg.dur_dl == 'd')
                    rds_oda_tmc_dsp_current->msg.dur_dl = 'l';
                else if (rds_oda_tmc_dsp_current->msg.dur_dl == 'l')
                    rds_oda_tmc_dsp_current->msg.dur_dl = 'd';
                break;
            case 4:
                /* Default spoken or unspoken duration interchanged */
                if (isupper(rds_oda_tmc_dsp_current->msg.dur_dl)) // aka isspoken(..)
                    rds_oda_tmc_dsp_current->msg.dur_dl = tolower(rds_oda_tmc_dsp_current->msg.dur_dl);
                else
                    rds_oda_tmc_dsp_current->msg.dur_dl = toupper(rds_oda_tmc_dsp_current->msg.dur_dl);
                break;
            case 5:
                /* Equivalent of diversion bit set to "1" */
                rds_oda_tmc_dsp_current->msg.div = 1;
                break;
            case 6:
                /* Increase the number of steps in the problem extent by eight */
                rds_oda_tmc_dsp_current->msg.ext += 8;
                break;
            case 7:
                /* Increase the number of steps in the problem extent by sixteen */
                rds_oda_tmc_dsp_current->msg.ext += 16;
                break;
            }
            break;
        case 2:
            /* Length of route affected */
            break;
        case 3:
            /* Speed limit advice */
            break;
        case 4:
            /* Quantifier */
            break;
        case 5:
            /* Quantifier */
            break;
        case 6:
            /* Supplementary information code */
            break;
        case 7:
            /* Explicit start time */
            rds_oda_tmc_dsp_current->msg.start_time =
                tmc_start_stop_time(rds_oda_tmc_dsp_current->msg.receive_time, data);
            break;
        case 8:
            /* Explicit stop time */
            rds_oda_tmc_dsp_current->msg.stop_time =
                tmc_start_stop_time(rds_oda_tmc_dsp_current->msg.receive_time, data);
            break;
        case 9:
            /* Additional event */
            /* use highest possible urgency */
            /* HINT: an arithmetic comparison can be used because ('N'<'U'<'X') */
            if (rds_oda_tmc_event_list[data].u > rds_oda_tmc_dsp_current->msg.urg)
                rds_oda_tmc_dsp_current->msg.urg = rds_oda_tmc_event_list[data].u;
            /* set to unidirectional if any is unidirectional */
            if (rds_oda_tmc_event_list[data].d == 1)
                rds_oda_tmc_dsp_current->msg.dir_ub = 1;
            break;
        case 10:
            /* Detailed diversion instructions */
            break;
        case 11:
            /* Destination */
            break;
        case 12:
            /* Reserved for future use */
            rds_decode_status = RDS_DECODE_STATUS_NOT_ASSIGNED;
            break;
        case 13:
            /* Cross linkage to source of problem, or another route */
            break;
        case 14:
            /* Separator */
            break;
        case 15:
            /* Reserved for future use */
            rds_decode_status = RDS_DECODE_STATUS_NOT_ASSIGNED;
            break;
        }
    }

    /* loc = 0: reserved */
    /* loc = 1..63487: regular locations */
    /* loc = 63488..64511: special use */
    /* loc = 64512 (0xFC00)..65532: INTER-ROAD (foreign location table) */
    /* loc = 65533: '(message) for all users' */
    /* loc = 65534: 'silent' location code */
    /* loc = 65535: updating or cancelling of messages */
    /** \todo handle silent cancellation message */

    /* handle null messages (only for msgs from same RDS-TMC service */
    if ((rds_oda_tmc_dsp_current->msg.evt == 2047) && (rds_oda_tmc_dsp_current->msg.loc == 65535))
    {
        /* clear all message from the same RDS-TMC service */
        for (i = 0; i < rds_oda_tmc_dsp_current->message_memory_cnt; i++)
        {
            if ((rds_oda_tmc_dsp_current->message_memory[i].ecc == rds_oda_tmc_dsp_current->msg.ecc) &&
                    (rds_oda_tmc_dsp_current->message_memory[i].cc  == rds_oda_tmc_dsp_current->msg.cc) &&
                    (rds_oda_tmc_dsp_current->message_memory[i].ltn == rds_oda_tmc_dsp_current->msg.ltn) &&
                    (rds_oda_tmc_dsp_current->message_memory[i].sid == rds_oda_tmc_dsp_current->msg.sid))
            {
                tmc_delete_message(i);
                i--; /* check location again */
            }
        }
        return;
    }

    /* clear all message with same location code */
    if (rds_oda_tmc_dsp_current->msg.evt == 2047)
    {
        for (i = 0; i < rds_oda_tmc_dsp_current->message_memory_cnt; i++)
        {
            if ((rds_oda_tmc_dsp_current->message_memory[i].ecc == rds_oda_tmc_dsp_current->msg.ecc) &&
                    (rds_oda_tmc_dsp_current->message_memory[i].cc  == rds_oda_tmc_dsp_current->msg.cc) &&
                    (rds_oda_tmc_dsp_current->message_memory[i].ltn == rds_oda_tmc_dsp_current->msg.ltn) &&
                    (rds_oda_tmc_dsp_current->message_memory[i].sid == rds_oda_tmc_dsp_current->msg.sid) &&
                    (rds_oda_tmc_dsp_current->message_memory[i].loc == rds_oda_tmc_dsp_current->msg.loc))
            {
                tmc_delete_message(i);
                i--; /* check location again */
            }
        }
        return;
    }

    /* clear all message with the same update class */
    if ((rds_oda_tmc_dsp_current->msg.dir_ub == 0) && (rds_oda_tmc_dsp_current->msg.loc == 65535))   /* dir_ub=0 for silent cancellation events */
    {
        for (i = 0; i < rds_oda_tmc_dsp_current->message_memory_cnt; i++)
        {
            if ((rds_oda_tmc_dsp_current->message_memory[i].ecc == rds_oda_tmc_dsp_current->msg.ecc) &&
                    (rds_oda_tmc_dsp_current->message_memory[i].cc  == rds_oda_tmc_dsp_current->msg.cc) &&
                    (rds_oda_tmc_dsp_current->message_memory[i].ltn == rds_oda_tmc_dsp_current->msg.ltn) &&
                    (rds_oda_tmc_dsp_current->message_memory[i].sid == rds_oda_tmc_dsp_current->msg.sid) &&
                    (rds_oda_tmc_dsp_current->message_memory[i].upc == rds_oda_tmc_dsp_current->msg.upc))
            {
                tmc_delete_message(i);
                i--; /* check location again */
            }
        }
        return;
    }
    /** \todo INTER-ROAD cancellation, see 6.7.3, usually with msg.loc_ir = 65535 */
    if (rds_oda_tmc_dsp_current->msg.loc >= 64512)
        return; /** \todo currently don't handle Inter-Road messages */

    /* look if we already have the message (search for expired messages) */
    for (i = 0; i < rds_oda_tmc_dsp_current->message_memory_cnt; i++)
    {
        uint8_t any_upc_different;

        /* location and direction must be identical */
        if ((rds_oda_tmc_dsp_current->message_memory[i].loc != rds_oda_tmc_dsp_current->msg.loc) || (rds_oda_tmc_dsp_current->message_memory[i].dir != rds_oda_tmc_dsp_current->msg.dir))
            continue;

        /* update class must be identical for any event's update class */
        any_upc_different = 1;
        if (rds_oda_tmc_dsp_current->message_memory[i].upc == rds_oda_tmc_dsp_current->msg.upc)
        {
            any_upc_different = 0;
        }
        else
        {
            uint8_t o; /* index for optional content */
            for (o = 0; o < rds_oda_tmc_dsp_current->msg.opt_cnt; o++)
            {
                if (rds_oda_tmc_dsp_current->msg.opt[o].label != 9)
                    continue;

                /* check event's update class */
                if (rds_oda_tmc_dsp_current->message_memory[i].upc == rds_oda_tmc_event_list[rds_oda_tmc_dsp_current->msg.opt[o].data].c)
                {
                    any_upc_different = 0;
                    break;
                }
            }
        }
        if (any_upc_different == 1)
            continue;

        /* same RDS-TMC service */
        if (rds_oda_tmc_dsp_current->message_memory[i].ecc != rds_oda_tmc_dsp_current->msg.ecc)
            continue;
        if (rds_oda_tmc_dsp_current->message_memory[i].cc != rds_oda_tmc_dsp_current->msg.cc)
            continue;
        if (rds_oda_tmc_dsp_current->message_memory[i].ltn != rds_oda_tmc_dsp_current->msg.ltn)
            continue;
        if (rds_oda_tmc_dsp_current->message_memory[i].sid != rds_oda_tmc_dsp_current->msg.sid)
            continue;

        /* if forecast event, same duration */
        if ((rds_oda_tmc_dsp_current->msg.upc >= 32) && (rds_oda_tmc_dsp_current->msg.upc >= 39) && (rds_oda_tmc_dsp_current->message_memory[i].dur != rds_oda_tmc_dsp_current->msg.dur))
            continue;

        /* all conditions passed, message found */
        break;
    }
    /* i == message_memory_cnt, means that no updateable message was found */
    if (i == rds_oda_tmc_dsp_current->message_memory_cnt)   /* new message */
    {
        rds_oda_tmc_dsp_current->message_memory_cnt++;
    }
    else     /* existing message */
    {
        if (rds_oda_tmc_dsp_current->msg.incomplete == 1) /* don't update with incomplete message */
            return;
    }
    /* overwrite old message with new one */
    memcpy(&rds_oda_tmc_dsp_current->message_memory[i], &rds_oda_tmc_dsp_current->msg, sizeof(rds_oda_tmc_message_t));

    /* set expiry times */
    if ((char) toupper(rds_oda_tmc_dsp_current->msg.dur_dl) == 'D')
    {
        /* D: dynamic event */ /** \todo Persistence period (after last receipt of the message) */
        switch(rds_oda_tmc_dsp_current->msg.dur)
        {
        case 0:
        case 1:
            /* 15 minutes (no message to end-user) */ /** \todo no message to end-user */
            rds_oda_tmc_dsp_current->msg.expiry_time =
                rds_oda_tmc_dsp_current->msg.receive_time + 15*60;
            break;
        case 2:
            /* 30 minutes (with message to end-user) */ /** \todo with message to end-user */
            rds_oda_tmc_dsp_current->msg.expiry_time =
                rds_oda_tmc_dsp_current->msg.receive_time + 30*60;
            break;
        case 3:
            /* 1 hour */
            rds_oda_tmc_dsp_current->msg.expiry_time =
                rds_oda_tmc_dsp_current->msg.receive_time + 1*60*60;
            break;
        case 4:
            /* 2 hours */
            rds_oda_tmc_dsp_current->msg.expiry_time =
                rds_oda_tmc_dsp_current->msg.receive_time + 2*60*60;
            break;
        case 5:
            /* 3 hours */
            rds_oda_tmc_dsp_current->msg.expiry_time =
                rds_oda_tmc_dsp_current->msg.receive_time + 3*60*60;
            break;
        case 6:
            /* 4 hours */
            rds_oda_tmc_dsp_current->msg.expiry_time =
                rds_oda_tmc_dsp_current->msg.receive_time + 4*60*60;
            break;
        default: /* case 7 */
            /* until midnight on the day of message receipt */
            rds_oda_tmc_dsp_current->msg.expiry_time =
                rds_ct_start_of_day(rds_oda_tmc_dsp_current->msg.receive_time, 1);
            break;
        }
    }
    else
    {
        /* L: longer-lasting event */ /** \todo Persistence period or Persistence of message */
        switch (rds_oda_tmc_dsp_current->msg.dur)
        {
        case 0:
            /* 1 hour */
            rds_oda_tmc_dsp_current->msg.expiry_time =
                rds_oda_tmc_dsp_current->msg.receive_time + 1*60*60;
            break;
        case 1:
            /* 2 hours */
            rds_oda_tmc_dsp_current->msg.expiry_time =
                rds_oda_tmc_dsp_current->msg.receive_time + 2*60*60;
            break;
        case 2:
            /* until midnight on the day of message receipt */
            rds_oda_tmc_dsp_current->msg.expiry_time =
                rds_ct_start_of_day(rds_oda_tmc_dsp_current->msg.receive_time, 1);
            break;
        default: /* case 3..7 */
            /* until midnight on the day after message receipt */
            rds_oda_tmc_dsp_current->msg.expiry_time =
                rds_ct_start_of_day(rds_oda_tmc_dsp_current->msg.receive_time, 2);
            break;
        }
    }

    /* set decrement time */
    if (((char) toupper(rds_oda_tmc_dsp_current->msg.dur_dl) == 'D') &&
            ((rds_oda_tmc_dsp_current->msg.nat     == 'I') ||
             (rds_oda_tmc_dsp_current->msg.nat     == 'F')))
    {
        /* D: dynamic, I: information */
        /* D: dynamic, F: forecase */
        switch(rds_oda_tmc_dsp_current->msg.dur)
        {
        case 2:
            /* 15 minutes */
            rds_oda_tmc_dsp_current->msg.decrement_time =
                rds_oda_tmc_dsp_current->msg.receive_time + 15*60;
            break;
        case 3:
            /* 30 minutes */
            rds_oda_tmc_dsp_current->msg.decrement_time =
                rds_oda_tmc_dsp_current->msg.receive_time + 30*60;
            break;
        case 4:
        case 5:
        case 6:
            /* 1 hour */
            rds_oda_tmc_dsp_current->msg.decrement_time =
                rds_oda_tmc_dsp_current->msg.receive_time + 1*60*60;
            break;
        default: /* case 0, 1, 7 */
            /* do not decrement */
            /* leave msg.decrement_time = 0 */
            break;
        }
    }
    else if (((char) toupper(rds_oda_tmc_dsp_current->msg.dur_dl) == 'L') &&
             (rds_oda_tmc_dsp_current->msg.nat     == 'I'))
    {
        /* L: longer-lasting, I: information */
        switch(rds_oda_tmc_dsp_current->msg.dur)
        {
        case 3:
            /* midnight */
            rds_oda_tmc_dsp_current->msg.decrement_time =
                rds_ct_start_of_day(rds_oda_tmc_dsp_current->msg.receive_time, 1);
            break;
        case 4:
            /* Friday midnight, i.e. 00:00 Saturday */
            rds_oda_tmc_dsp_current->msg.decrement_time =
                rds_ct_midnight(rds_oda_tmc_dsp_current->msg.receive_time, 5);
            break;
        case 5:
            /* Sunday midnight, i.e. 00:00 Monday */
            rds_oda_tmc_dsp_current->msg.decrement_time =
                rds_ct_midnight(rds_oda_tmc_dsp_current->msg.receive_time, 7);
            break;
        default: /* case 0..2, 6..7 */
            /* do not decrement */
            /* leave msg.decrement_time = 0 */
            break;
        }
    }
    else if (((char) toupper(rds_oda_tmc_dsp_current->msg.dur_dl) == 'L') &&
             (rds_oda_tmc_dsp_current->msg.nat     == 'F'))
    {
        /* L: longer-lasting, F: forecast */
        switch(rds_oda_tmc_dsp_current->msg.dur)
        {
        case 3:
        case 4:
            /* decrement at midnight */
            rds_oda_tmc_dsp_current->msg.decrement_time =
                rds_ct_start_of_day(rds_oda_tmc_dsp_current->msg.receive_time, 1);
            break;
        default: /* case 0..2, 5..7 */
            /* do not decrement */
            /* leave msg.decrement_time = 0 */
            break;
        }
    }

    /* execute callback with 0=message update */
    if (rds_oda_tmc_callback != NULL)
        rds_oda_tmc_callback(&rds_oda_tmc_dsp_current->msg, 0);

    /* clear message */
    memset(&rds_oda_tmc_dsp_current->msg, 0, sizeof(rds_oda_tmc_message_t));
}


/**
 * \brief Decoding of single group TMC messages
 *
 * This function decodes single group TMC messages.
 *
 * \param[in] _dp  direction and persistance
 * \param[in] _div diversion advice
 * \param[in] _dir direction
 * \param[in] _ext extent
 * \param[in] _evt event
 * \param[in] _loc location
 */
static void tmc_decode_single(uint8_t _dp, uint8_t _div, uint8_t _dir, uint8_t _ext, uint16_t _evt, uint16_t _loc)
{
    /* handle previously received incomplete message */
    if (rds_oda_tmc_dsp_current->msg.incomplete == 1)
        tmc_handle_message();

    /* set data */
    rds_oda_tmc_dsp_current->msg.dur = _dp;
    rds_oda_tmc_dsp_current->msg.div = _div;
    rds_oda_tmc_dsp_current->msg.dir = _dir;
    rds_oda_tmc_dsp_current->msg.ext = _ext;
    rds_oda_tmc_dsp_current->msg.evt = _evt;
    rds_oda_tmc_dsp_current->msg.loc = _loc;

    /* handle message */
    tmc_handle_message();
}


/**
 * \brief Decoding of first group of multi-group TMC messages
 *
 * This function decodes first group of multi-group TMC messages.
 *
 * \param[in] _ci  continuity index
 * \param[in] _dir direction
 * \param[in] _ext extent
 * \param[in] _evt event
 * \param[in] _loc location
 */
static void rds_oda_tmc_decode_multi_first(uint8_t _ci, uint8_t _dir, uint8_t _ext, uint16_t _evt, uint16_t _loc)
{
    /* check for new continuity index */
    if (rds_oda_tmc_dsp_current->last_ci == _ci)
        return;
    rds_oda_tmc_dsp_current->last_ci = _ci;

    /* handle previously received incomplete message */
    if (rds_oda_tmc_dsp_current->msg.incomplete == 1)
        tmc_handle_message();

    /* set data */
    rds_oda_tmc_dsp_current->msg.dir = _dir;
    rds_oda_tmc_dsp_current->msg.ext = _ext;
    rds_oda_tmc_dsp_current->msg.evt = _evt;
    rds_oda_tmc_dsp_current->msg.loc = _loc;
    rds_oda_tmc_dsp_current->msg.incomplete = 1;
    rds_oda_tmc_dsp_current->msg.dur = 7;   /* set persistence period to midnight on the day after message receipt */
}


/**
 * \brief Decoding of subsequent groups of multi-group TMC messages
 *
 * This function decodes subsequent groups of multi-group TMC messages.
 *
 * \param[in] _ci  continuity index
 * \param[in] _sgi second group indicator
 * \param[in] _gsi group sequence identifier
 * \param[in] _ff  free format in blk 3 and blk 4 (28 bits from y11..z0)
 */
static void rds_oda_tmc_decode_multi_subsequent(uint8_t _ci, uint8_t _sgi, uint8_t _gsi, uint64_t _ff)
{
    /* content length */
    const uint8_t tmc_optional_message_content_length[16] = {3, 3, 5, 5, 5, 8, 8, 8, 8, 11, 16, 16, 16, 16, 0, 0};

    /* get access to private data */
    rds_program_private_t *rds_program_current_private =
        (rds_program_private_t *) rds_program_current;


    /* ci should be identical to the ci of the first message */
    if (rds_oda_tmc_dsp_current->last_ci != _ci)
        return;

    /* possible values of sgi and gsi for N=5..2: */
    /* N=5: MF, MS(sgi=1, gsi=3), MS(sgi=0, gsi=2), MS(sgi=0, gsi=1), MS(sgi=0, gsi=0) */
    /* N=4: MF, MS(sgi=1, gsi=2), MS(sgi=0, gsi=1), MS(sgi=0, gsi=0) */
    /* N=3: MF, MS(sgi=1, gsi=1), MS(sgi=0, gsi=0) */
    /* N=2: MF, MS(sgi=1, gsi=0) */
    if (_sgi == 1)   /* second group */
    {
        /* empty fifo */
        rds_program_current_private->oda_tmc_fifo = 0;
        rds_program_current_private->oda_tmc_fifo_cnt = 0;

        /* process free format (y11..z0) */
        rds_program_current_private->oda_tmc_fifo |= _ff << (64 - rds_program_current_private->oda_tmc_fifo_cnt - 28);
        rds_program_current_private->oda_tmc_fifo_cnt += 28;

        /* inter-road message have the primary location 16 bits coded in (y11..z12) */
        /* only the remaining 12 bits (z11..z0) are then used for free format */
        if ((rds_oda_tmc_dsp_current->msg.loc >= 64512) && (rds_oda_tmc_dsp_current->msg.loc <= 65532))
        {
            rds_oda_tmc_dsp_current->msg.loc_ir = rds_program_current_private->oda_tmc_fifo >> (64-16);
            rds_program_current_private->oda_tmc_fifo <<= 16;   // shift away data
            rds_program_current_private->oda_tmc_fifo_cnt -= 16;
        }
    }
    else if (((64 - rds_program_current_private->oda_tmc_fifo_cnt - 28)>=0) && rds_program_current_private->oda_tmc_last_gsi - 1 == _gsi)     /* groups 3..5 */
    {
        /* process free format (y11..z0) */
        rds_program_current_private->oda_tmc_fifo |= _ff << (64 - rds_program_current_private->oda_tmc_fifo_cnt - 28); //TODO: shift exponent might get negative
        rds_program_current_private->oda_tmc_fifo_cnt += 28;
    }
    rds_program_current_private->oda_tmc_last_gsi = _gsi;

    /* process free format (optional content has max. length of 16 bits) */
    while (rds_program_current_private->oda_tmc_fifo_cnt >= 4)      /* label size is 4 bits */
    {
        uint8_t label = rds_program_current_private->oda_tmc_fifo >> (64-4);
        uint8_t data_length = tmc_optional_message_content_length[label];

        if (data_length > 0 && (rds_program_current_private->oda_tmc_fifo_cnt >= 4 + data_length))
        {
            rds_oda_tmc_dsp_current->msg.opt[rds_oda_tmc_dsp_current->msg.opt_cnt].label = label;
            rds_program_current_private->oda_tmc_fifo <<= 4;    /* shift away label */
            rds_program_current_private->oda_tmc_fifo_cnt -= 4;

            rds_oda_tmc_dsp_current->msg.opt[rds_oda_tmc_dsp_current->msg.opt_cnt].data =
                rds_program_current_private->oda_tmc_fifo >> (64-data_length);
            rds_program_current_private->oda_tmc_fifo <<= data_length;  /* shift away data */
            rds_program_current_private->oda_tmc_fifo_cnt -= data_length;

            rds_oda_tmc_dsp_current->msg.opt_cnt++;
        }
        else
            break;
    }

    /* handle message */
    if (_gsi == 0)
    {
        rds_oda_tmc_dsp_current->msg.incomplete = 0;
        tmc_handle_message();
    }
}


/**
 * \brief Decoding of encryption administration group
 *
 * This function handles Encryption Administration group messages.
 *
 * \param[in] _test  Test Mode
 * \param[in] _sid   Service identifier
 * \param[in] _encid Encryption identifier
 * \param[in] _ltnbe Location Table Number before encryption
 */
static void tmc_decode_encrypted_eag(uint8_t _test, uint8_t _sid, uint8_t _encid, uint8_t _ltnbe)
{
    /* save information */
    rds_oda_tmc_dsp_current->enc_test   = _test;
    rds_oda_tmc_dsp_current->sid        = _sid;
    rds_oda_tmc_dsp_current->enc_encid  = _encid;
    rds_oda_tmc_dsp_current->enc_ltnbe  = _ltnbe;
}


/**
 * \brief Decoding of TMC message
 *
 * \param[in] _x Block 2 data
 * \param[in] _y Block 3 data
 * \param[in] _z Block 4 data
 */
void rds_oda_tmc_decode(uint8_t _x, uint16_t _y, uint16_t _z)
{
    uint8_t t, f, dp_ci, div, dir_sgi, ext, gsi;
    uint16_t evt;

    /* only process if TMC DSP information is loaded */
    if (rds_oda_tmc_dsp_current == NULL)
        return;

    /* validation (identical reception at least twice) */
    if ((rds_oda_tmc_dsp_current->decode_last_x != _x) ||
            (rds_oda_tmc_dsp_current->decode_last_y != _y) ||
            (rds_oda_tmc_dsp_current->decode_last_z != _z))
    {
        rds_oda_tmc_dsp_current->decode_last_x = _x;
        rds_oda_tmc_dsp_current->decode_last_y = _y;
        rds_oda_tmc_dsp_current->decode_last_z = _z;
        rds_oda_tmc_dsp_current->decode_cnt = 1;
        return;
    }
    rds_oda_tmc_dsp_current->decode_cnt++;
    if (rds_oda_tmc_dsp_current->decode_cnt != 2)
        return;

    t = (_x >> 4) & 1;      /* tuning information */
    f = (_x >> 3) & 1;      /* single-group message */
    dp_ci = _x & 7;         /* duration/persistence or continuity index */
    div = _y >> 15;         /* diversion advice */
    dir_sgi = (_y >> 14) & 1;   /* direction or second group indicator */
    ext = (_y >> 11) & 7;       /* extent */
    evt = _y & 0x7ff;       /* event */
    gsi = (_y >> 12) & 3;       /* group sequence identifier */

    if (t == 0)   /* X4=T=0 */
    {
        if (f == 0)   /* X3=F=0 */
        {
            if (dp_ci == 0)     /* encrypted RDS-TMC */
            {
                uint8_t variant_code = (_y >> 13);
                if (variant_code == 0)
                {
                    /* uint16_t rfu = z & 0x03ff; */
                    tmc_decode_encrypted_eag((_y>>11) & 3, (_y >> 5) & 0x3f, _y & 0x1f, _z >> 10);
                }
                else
                {
                    /* encrypted and rfu */
                }
            }
            else if ((dp_ci >= 1) && (dp_ci <= 6))      /* multi-group messages */
            {
                if ((_y >> 15) == 1)    /* first group */
                {
                    rds_oda_tmc_decode_multi_first(dp_ci, dir_sgi, ext, evt, _z);
                }
                else        /* subsequent groups */
                {
                    rds_oda_tmc_decode_multi_subsequent(dp_ci, dir_sgi, gsi, ((_y & 0xfff) << 16 | _z));
                }
            }
            else
            {
                /* 7 is reserved for future use */
                rds_decode_status = RDS_DECODE_STATUS_NOT_ASSIGNED;
            }
        }
        else     /* X3=F=1 */
        {
            /* single-group user message */
            tmc_decode_single(dp_ci, div, dir_sgi, ext, evt, _z);
        }
    }
    else     /* X4=T=1: Tuning Information */
    {

        switch (_x & 0xf)   /* variant/address */
        {
        case 4:
            /* character 1..4 */
            rds_oda_tmc_dsp_current->spn[0] = rds_to_wchar[((_y >> 8) & 0xff)];
            rds_oda_tmc_dsp_current->spn[1] = rds_to_wchar[((_y >> 0) & 0xff)];
            rds_oda_tmc_dsp_current->spn[2] = rds_to_wchar[((_z >> 8) & 0xff)];
            rds_oda_tmc_dsp_current->spn[3] = rds_to_wchar[((_z >> 0) & 0xff)];
            break;
        case 5:
            /* character 5..8 */
            rds_oda_tmc_dsp_current->spn[4] = rds_to_wchar[((_y >> 8) & 0xff)];
            rds_oda_tmc_dsp_current->spn[5] = rds_to_wchar[((_y >> 0) & 0xff)];
            rds_oda_tmc_dsp_current->spn[6] = rds_to_wchar[((_z >> 8) & 0xff)];
            rds_oda_tmc_dsp_current->spn[7] = rds_to_wchar[((_z >> 0) & 0xff)];
            break;
        case 6:
            /* AF (ON), AF (ON), PI (ON) */
#if 0
            rds_af_handle(&rds_oda_tmc_dsp_current->af, 2, (_y >> 8) & 0xff, _y & 0xff);
#endif
            /** \todo re-add TMC AF */
            /** \todo save PI(ON)=_z */
            break;
        case 7:
            /* Tuning freq (TN), Mapped freq (ON), PI (ON) */
            rds_oda_tmc_dsp_current->tn[rds_oda_tmc_dsp_current->tnmfpi_cnt] = _y >> 8;
            rds_oda_tmc_dsp_current->mf[rds_oda_tmc_dsp_current->tnmfpi_cnt] = _y & 0xff;
            rds_oda_tmc_dsp_current->pi[rds_oda_tmc_dsp_current->tnmfpi_cnt] = _z;
            rds_oda_tmc_dsp_current->tnmfpi_cnt++;
            rds_oda_tmc_dsp_current->tnmfpi_cnt%=5;
            break;
        case 8:
            /* PI (ON with AFI=1), PI (ON with AFI=1) */
            rds_oda_tmc_dsp_current->pi_afi[rds_oda_tmc_dsp_current->pi_afi_cnt] = _y;
            rds_oda_tmc_dsp_current->pi_afi_cnt++;
            rds_oda_tmc_dsp_current->pi_afi_cnt%=5;
            rds_oda_tmc_dsp_current->pi_afi[rds_oda_tmc_dsp_current->pi_afi_cnt] = _z;
            rds_oda_tmc_dsp_current->pi_afi_cnt++;
            rds_oda_tmc_dsp_current->pi_afi_cnt%=5;
            break;
        case 9:
            /* LTN (ON), MGS (ON), SID (ON), PI (ON with AFI=1) */
            /*@-mustfreefresh@*/
        {
            rds_program_t *rds_program_on;
            rds_oda_tmc_dsp_t *rds_oda_tmc_dsp_on;
            rds_program_on = rds_program_load(rds_program_current->ecc, _z);
            rds_program_on->oda_tmc_sid = _y & 0x3f;
            rds_oda_tmc_dsp_on = rds_oda_tmc_dsp_load(rds_program_on->ecc, rds_pi_cc(_z), rds_program_on->oda_tmc_sid);
            rds_oda_tmc_dsp_on->sid = _y & 0x3f;
            rds_oda_tmc_dsp_on->ltn = _y >> 10;
            rds_oda_tmc_dsp_on->mgs = (_y >> 6) & 0xf;
            rds_oda_tmc_dsp_current->pi_afi[rds_oda_tmc_dsp_current->pi_afi_cnt] = _z;
            rds_oda_tmc_dsp_current->pi_afi_cnt++;
            rds_oda_tmc_dsp_current->pi_afi_cnt%=5;
        }
        /*@+mustfreefresh@*/
        break;
        default:
            /* cases 0..3 and 10..15: reserved for future use */
            rds_decode_status = RDS_DECODE_STATUS_NOT_ASSIGNED;
            break;
        }
    }
}


/**
 * \brief Decoding of system message containing location table number
 *
 * This function decodes system messages containing the location table number.
 *
 * \param[in] _ltn Location Table Number
 * \param[in] _afi Alternative Frequency Indicator
 * \param[in] _m   Mode (0=basic 1=enhanced)
 * \param[in] _mgs Message Geographical Scope
 */
static void tmc_decode_system_ltn(uint8_t _ltn, uint8_t _afi, uint8_t _m, uint8_t _mgs)
{
    /* only process if TMC DSP information is loaded */
    if (rds_oda_tmc_dsp_current == NULL)
        return;

    /* save information */
    rds_oda_tmc_dsp_current->ltn = _ltn;
    rds_oda_tmc_dsp_current->afi = _afi;
    rds_oda_tmc_dsp_current->mode = _m;
    rds_oda_tmc_dsp_current->mgs = _mgs;

    /* change location code list */
    /*rds_oda_tmc_lcl_set_tabcd(_ltn);*/    /** \todo is this really necessary? */
}


/**
 * \brief Decoding of system message containing service identifier
 *
 * This function decodes system messages containing the service identifier.
 *
 * \param[in] _g   gap (groups)
 * \param[in] _sid service identifier
 * \param[in] _ta  activity time
 * \param[in] _tw  window time
 * \param[in] _td  delay time
 */
static void tmc_decode_system_sid(uint8_t _g, uint8_t _sid, uint8_t _ta, uint8_t _tw, uint8_t _td)
{
    /* service identifier */
    rds_program_current->oda_tmc_sid = _sid;
    rds_oda_tmc_dsp_current = rds_oda_tmc_dsp_load(rds_program_current->ecc, rds_pi_cc(rds_program_current->pi), _sid);

    /* gap (groups), default: 8 to 14 */
    switch(_g)
    {
    case 0:
        rds_oda_tmc_dsp_current->gap = 3;
        break;
    case 1:
        rds_oda_tmc_dsp_current->gap = 5;
        break;
    case 2:
        rds_oda_tmc_dsp_current->gap = 8;
        break;
    default: /* case 3 */
        rds_oda_tmc_dsp_current->gap = 11;
        break;
    }

    /* enhanced mode */
    if (rds_oda_tmc_dsp_current->mode == 1)
    {
        rds_oda_tmc_dsp_current->delay_time    = (uint8_t) _td;
        rds_oda_tmc_dsp_current->activity_time = (uint8_t) (2<<_ta);
        rds_oda_tmc_dsp_current->window_time   = (uint8_t) (2<<_tw);
    }
}


/**
 * \brief Decoding of ODA assign message information
 *
 * This function decodes the RDS ODA assign message.
 *
 * \param[in] _msg Message content
 */
void rds_oda_tmc_decode_assign(uint16_t _msg)
{
    uint8_t variant_code = (_msg >> 14) & 3;

    if (variant_code == 0)
    {
        tmc_decode_system_ltn((_msg >> 6) & 0x3f, (_msg >> 5) & 1, (_msg >> 4) & 1, _msg & 0xf);
    }
    else if (variant_code == 1)
    {
        tmc_decode_system_sid((_msg >> 12) & 3, (_msg >> 6) & 0x3f, (_msg >> 4) & 3, (_msg >> 2) & 3, _msg & 3);
    }
    else if (variant_code == 2)
    {
        rds_program_current->ecc=_msg & 255;
    }
    else
    {
        /* nothing is documented for the variant code 3 */
        rds_decode_status = RDS_DECODE_STATUS_NOT_ASSIGNED;
    }
}

char * stringReplace(char *search, char *replace, char *string) {
    char *tempString, *searchStart;
    long len=0;
    
    
    // preuefe ob Such-String vorhanden ist
    searchStart = strstr(string, search);
    if(searchStart == NULL) {
        return string;
    }
    
    // Speicher reservieren
    tempString = (char*) malloc(strlen(string) * sizeof(wchar_t));
    if(tempString == NULL) {
        return NULL;
    }
    
    // temporaere Kopie anlegen
    strcpy(tempString, string);
    
    // ersten Abschnitt in String setzen
    len = searchStart - string;
    string[len] = '\0';
    
    // zweiten Abschnitt anhaengen
    strcat(string, replace);
    
    // dritten Abschnitt anhaengen
    len += strlen(search);
    strcat(string, (char*)tempString+len);
    
    // Speicher freigeben
    free(tempString);
    
    return string;
}

void  convert_rds_charset_to_ascii(char* str) {
    
//    RDSCharset.decodermapping.put (Character.valueOf((char) 0x04),  Character.valueOf((char) 0x20)); // SPACE
//    RDSCharset.decodermapping.put (Character.valueOf((char) 0x91),  Character.valueOf((char) 0xE4)); // ä
//    RDSCharset.decodermapping.put (Character.valueOf((char) 0x97),  Character.valueOf((char) 0xF6)); // ö
//    RDSCharset.decodermapping.put (Character.valueOf((char) 0x99),  Character.valueOf((char) 0xFC)); // ü
//    RDSCharset.decodermapping.put (Character.valueOf((char) 0x8D),  Character.valueOf((char) 0xCF)); // ß
//    RDSCharset.decodermapping.put (Character.valueOf((char) 0xD1),  Character.valueOf((char) 0xC4)); // Ä
//    RDSCharset.decodermapping.put (Character.valueOf((char) 0xD7),  Character.valueOf((char) 0xD6)); // Ö
//    RDSCharset.decodermapping.put (Character.valueOf((char) 0xD9),  Character.valueOf((char) 0xDC)); // Ü

    // {{0x9f, 0xE1},{0x04, 0x20}, {0x91, 0xE4}, {0x97, 0xF6}, {0x99, 0xFC}, {0x8D, 0xCF}, {0xD1, 0xC4}, {0xD7, 0xD6}, {0xD9, 0xDC}};
    

        
    unsigned char *src = (unsigned char*)str;
    unsigned char *dst = (unsigned char*)str;
    
    while (*src) {
        if (*src == 0xC3 && *(src + 1) >= 0x80) {
            // Konvertierungsschema: (Byte1 & 0x03) << 6 | (Byte2 & 0x3F)
            // Speziell für den C3-Block: Ergebnis = zweites Byte + 0x40
            *dst++ = *(src + 1) + 0x40;
            src += 2;
        } else if (*src == 0xC2 && *(src + 1) >= 0x80) {
            // Für den C2-Block (z.B. ©, ®): Ergebnis = zweites Byte
            *dst++ = *(src + 1);
            src += 2;
        } else {
            *dst++ = *src++;
        }
    }
    *dst = '\0';
    str=str;
}
