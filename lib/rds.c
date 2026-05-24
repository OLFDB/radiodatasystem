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
 * \file rds.c
 * \test All related test cases are defined in \ref rds_test.c
 * \brief Radio Data System functions
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains functions for Radio Data System handling.
 */

#include <sqlite3.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define _XOPEN_SOURCE /* glibc2 needs this for strptime in time.h */
#include <time.h>
#include <wchar.h>
#include "af.h"
#include "ct.h"
#include "di.h"
#include "ecc.h"
#include "eon.h"
#include "ews.h"
#include "ih.h"
#include "lic.h"
#include "ms.h"
#include "oda.h"
#include "pi.h"
#include "pin.h"
#include "ps.h"
#include "pty.h"
#include "ptyn.h"
#include "rds_private.h"
#include "rp.h"
#include "rt.h"
#include "ta.h"
#include "tdc.h"
#include "tp.h"
#include "erp.h"
#ifdef ODA_TMC
#include "oda_tmc.h"
#endif


/**
 * \brief Statistics counter
 *
 * This variable contains a statistics counter for received RDS groups and versions
 */
uint32_t rds_group_type_cnt[16][2];


/**
 * \brief Load RDS program configuration
 *
 * This parameter configures when RDS programs will be loaded.
 * Default is \ref RDS_PROGRAM_LOAD_CONFIG_ON_NEW_PROGRAM.
 */
rds_program_load_config_t rds_program_load_config = RDS_PROGRAM_LOAD_CONFIG_ON_NEW_PROGRAM;


/**
 * \brief Save RDS program configuration
 *
 * This parameter configures when RDS programs will be saved.
 * Default is \ref RDS_PROGRAM_SAVE_CONFIG_OLDEST.
 */
rds_program_save_config_t rds_program_save_config = RDS_PROGRAM_SAVE_CONFIG_OLDEST;


/**
 * \brief Current RDS program
 *
 * This points to the current RDS program.
 */
rds_program_t *rds_program_current;


/**
 * \brief RDS program list
 *
 * This list contains all information to currently received RDS programs.
 */
static rds_program_private_t rds_program[64];


/**
 * \brief Language specific database
 *
 * This database contains language specific information.
 */
sqlite3 *rds_db_lang;


/**
 * \brief RDS program database
 *
 * This database contains RDS program information.
 */
static sqlite3 *rds_db_program;


/**
 * \brief Callback function
 *
 * This variable contains the callback function.
 *
 * \param[out] _new New program data
 * \param[out] _old Old program data
 */
void (*rds_callback)(rds_program_t *_new, rds_program_t *_old);


/**
 * \brief RDS decode status from the last decoded message.
 *
 * This variable contains RDS decode status from the last rds_decode operation.
 * The status is reset at the beginning of every rds_decode operation.
 */
rds_decode_status_t rds_decode_status = RDS_DECODE_STATUS_OK;


/**
 * \brief Decode and handle RDS message
 *
 * This function decodes and handles RDS messages.
 *
 * \param[in] _pi   Block 1 data (contains PI code)
 * \param[in] _blk2 Block 2 data
 * \param[in] _blk3 Block 3 data (contains PI code in type B)
 * \param[in] _blk4 Block 4 data
 */
/*@-mustfreefresh@@-mustfreeonly@*/
void rds_decode(uint16_t _pi, uint16_t _blk2, uint16_t _blk3, uint16_t _blk4)
{
    uint8_t ecc = 0;
    uint8_t gtc;    /* Group Type Code */
    uint8_t gtv;    /* Group Type Version: 0=A 1=B */
    static rds_program_t rds_program_current_old;
    static rds_program_t rds_program_eon_old;
    rds_program_t *rds_program_eon = NULL;
    rds_program_private_t *rds_program_current_private;
    uint8_t change = 0;
   
    /* reset status */
    rds_decode_status = RDS_DECODE_STATUS_OK;

    /* save data from previous reception */
    if (rds_program_current != NULL)
    {
        /* save old program data for comparison */
        memcpy(&rds_program_current_old, rds_program_current, sizeof(rds_program_t));

        /* (assume same ECC for program load) */
        ecc = rds_program_current->ecc;
    }

    /* load program */
    rds_program_current = rds_program_load(ecc, _pi);
    
    
    
    rds_program_current_private = (rds_program_private_t *) rds_program_current;

    /* decode components in every frame */
    rds_pi_decode(_pi);
    gtc = (_blk2 >> 12) & 0xf;
    gtv = (_blk2 >> 11) & 1;
    rds_tp_decode((_blk2 >> 10) & 1);
    rds_pty_decode((_blk2 >> 5) & 0x1f);
    if (gtv == 1)
        rds_pi_decode(_blk3);

    /* do statistics */
    rds_group_type_cnt[gtc][gtv]++;

    /* decode groups */
    switch (gtc)
    {
    case 0: /* Basic tuning and switching information */
            
            
        rds_ta_decode((_blk2 >> 4) & 1);
        rds_ms_decode((_blk2 >> 3) & 1);
        rds_di_decode(_blk2 & 0x3, (_blk2 >> 2) & 0x1);
        if (gtv == 0)
            rds_af_decode((_blk3 >> 8) & 0xff, (_blk3 >> 0) & 0xff);
        rds_ps_decode(_blk2 & 0x3, (_blk4 >> 8) & 0xff, (_blk4 >> 0) & 0xff);
        break;
    case 1: /* Programme Item Number and slow labelling codes */
            
        rds_pin_decode((_blk4 >> 11) & 0x1f, (_blk4 >> 6) & 0x1f, (_blk4 >> 0) & 0x2f);
        if (gtv == 0)
        {
            rds_eon_decode_la(_blk3 >> 15); /* Linkage Actuator */
            rds_rp_decode_rpc(_blk2 & 0x1f); /* Radio Paging Codes */
            switch ((_blk3 >> 12) & 0x7)   /* Variant Code */
            {
            case 0: /* Paging, Extended Country Code */
                rds_erp_decode_opc((_blk3 >> 8) & 0xf); /* OPerator Code */
                rds_ecc_decode(_blk3 & 0xff);
                if (((_blk4 >> 11) & 0x1f) == 0)   /* no valid PIN */
                {
                    if (((_blk4 >> 10) & 0x1) == 0)
                    {
                        /* sub type 0 */
                        rds_erp_decode_pac((_blk4 >> 4) & 0x3f); /* Paging Area Code */
                        rds_erp_decode_opc(_blk4 & 0xf); /* OPerator Code */
                    }
                    else
                    {
                        /* sub type 1 */
                        switch ((_blk4 >> 8) & 0x3)   /* sub usage code */
                        {
                        case 0:
                            rds_ecc_decode(_blk4 & 0xff);
                            break;
                        case 1:
                        case 2:
                            /* for future use */
                            rds_decode_status = RDS_DECODE_STATUS_NOT_ASSIGNED;
                            break;
                        case 3:
                            rds_erp_decode_ccf(_blk4 & 0xff); /* Current Carrier Frequency */
                            break;
                        }
                    }
                }
                break;
            case 1: /* not assigned */
                /* was once used for TMC identification, when ODA is not used */
                rds_decode_status = RDS_DECODE_STATUS_NOT_ASSIGNED;
                break;
            case 2: /* Paging identification */
                rds_erp_decode_opc((_blk3 >> 8) & 0xf); /* OPerator Code */
                if ((_blk3 & 0xc0) != 0)
                    rds_decode_status = RDS_DECODE_STATUS_NOT_ASSIGNED;
                rds_erp_decode_pac(_blk3 & 0x3f); /* Paging Area Code */
                if (((_blk4 >> 11) & 0x1f) == 0)   /* no valid PIN */
                {
                    if (((_blk4 >> 10) & 0x1) == 0)
                    {
                        /* sub type 0 */
                        rds_erp_decode_pac((_blk4 >> 4) & 0x3f); /* Paging Area Code */
                        rds_erp_decode_opc(_blk4 & 0xf); /* OPerator Code */
                    }
                    else
                    {
                        /* sub type 1 */
                        switch ((_blk4 >> 8) & 0x3)   /* sub usage code */
                        {
                        case 0:
                            rds_ecc_decode(_blk4 & 0xff);
                            break;
                        case 1:
                        case 2:
                            /* for future use */
                            rds_decode_status = RDS_DECODE_STATUS_NOT_ASSIGNED;
                            break;
                        case 3:
                            rds_erp_decode_ccf(_blk4 & 0xff); /* Current Carrier Frequency */
                            break;
                        }
                    }
                }
                break;
            case 3: /* Language identification codes */
                rds_lic_decode(_blk3 & 0xfff);
                break;
            case 4: /* not assigned */
            case 5: /* not assigned */
                rds_decode_status = RDS_DECODE_STATUS_NOT_ASSIGNED;
                break;
            case 6: /* For use by broadcasters */
                rds_decode_status = RDS_DECODE_STATUS_FOR_BROADCASTERS_USE;
                break;
            case 7: /* Identification of EWS channel */
                rds_ews_decode_ident(_blk3 & 0x0fff);
                break;
            }
        }
        else
        {
            /* _blk2 & 0x1f: Spare bits */
            if ((_blk2 & 0x1f) != 0)
                rds_decode_status = RDS_DECODE_STATUS_NOT_ASSIGNED;
        }
        break;
    case 2: /* RadioText */
            
        if (gtv == 0)
        {
            rds_rt_decode_a((_blk2 >> 5) & 1, (_blk2 & 0x0f),
                            (_blk3 >> 8) & 0xff,
                            (_blk3 >> 0) & 0xff,
                            (_blk4 >> 8) & 0xff,
                            (_blk4 >> 0) & 0xff);
        }
        else
        {
            rds_rt_decode_b((_blk2 >> 5) & 1, (_blk2 & 0x0f),
                            (_blk4 >> 8) & 0xff,
                            (_blk4 >> 0) & 0xff);
        }
        break;
    case 3:
            
        if (gtv == 0)   /* Application identification for Open Data */
        {
            rds_oda_decode_assign((_blk2 >> 1) & 0xf, _blk2 & 1, _blk3, _blk4);
        }
        else     /* Open Data Application */
        {
            rds_oda_decode_b(gtc, _blk2 & 0x1f, _blk4);
        }
        break;
    case 4:
            
        if (gtv == 0)   /* Clock-time and date */
        {
            rds_ct_decode(((_blk2 & 3) << 15) | (_blk3 >> 1),
                          ((_blk3 & 1) << 4) | ((_blk4 >> 12) & 0xf),
                          (_blk4 >> 6) & 0x3f,
                          (_blk4 >> 5) & 1,
                          _blk4 & 0x1f);

            /* save program */
            if (rds_program_save_config == 2)
                rds_program_save(rds_program_current);
        }
        else     /* Open data application */
        {
            rds_oda_decode_b(gtc, _blk2 & 0x1f, _blk4);
        }
        break;
    case 5: /* Transparent data channels or ODA */
        if (gtv == 0)
        {
            if (rds_program_current->oda[gtc][gtv] != 0)
            {
                rds_oda_decode_a(gtv, _blk2 & 0x1f, _blk3, _blk4);
            }
            else
            {
                rds_tdc_decode(_blk2 & 0x1f, _blk3);
                rds_tdc_decode(_blk2 & 0x1f, _blk4);
            }
        }
        else
        {
            if (rds_program_current->oda[gtc][gtv] != 0)
            {
                rds_oda_decode_b(gtc, _blk2 & 0x1f, _blk4);
            }
            else
            {
                rds_tdc_decode(_blk2 & 0x1f, _blk4);
            }
        }
        break;
    case 6: /* In-house applications or ODA */
        if (gtv == 0)
        {
            if (rds_program_current->oda[gtc][gtv] != 0)
            {
                rds_oda_decode_a(gtc, _blk2 & 0x1f, _blk3, _blk4);
            }
            else
            {
                rds_ih_decode_a(_blk2 & 0x1f, _blk3, _blk4);
            }
        }
        else
        {
            if (rds_program_current->oda[gtc][gtv] != 0)
            {
                rds_oda_decode_b(gtc, _blk2 & 0x1f, _blk4);
            }
            else
            {
                rds_ih_decode_b(_blk2 & 0x1f, _blk4);
            }
        }
        break;
    case 7:
        if (gtv == 0)   /* Radio paging or ODA */
        {
            if (rds_program_current->oda[gtc][gtv] != 0)
            {
                rds_oda_decode_a(gtc, _blk2 & 0x1f, _blk3, _blk4);
            }
            else
            {
                rds_rp_decode((_blk2 >> 4) & 1, _blk2 & 0xf, _blk3, _blk4);
            }
        }
        else     /* Open data application */
        {
            rds_oda_decode_b(gtc, _blk2 & 0x1f, _blk4);
        }
        break;
    case 8: /* Traffic Message Channel or ODA */
            
        if (gtv == 0)
        {
#ifdef ODA_TMC
            if (rds_program_current->oda[gtc][gtv] != 0)
            {
#endif
                rds_oda_decode_a(gtc, _blk2 & 0x1f, _blk3, _blk4);
#ifdef ODA_TMC
            }
            else
            {
                rds_oda_tmc_decode(_blk2 & 0x1f, _blk3, _blk4);
            }
#endif
        }
        else
        {
            rds_oda_decode_b(gtc, _blk2 & 0x1f, _blk4);
        }
        break;
    case 9: /* Emergency warning systems or ODA */
        if (gtv == 0)
        {
            if (rds_program_current->oda[gtc][gtv] != 0)
            {
                rds_oda_decode_a(gtc, _blk2 & 0x1f, _blk3, _blk4);
            }
            else
            {
                rds_ews_decode(_blk2 & 0x1f, _blk3, _blk4);
            }
        }
        else
        {
            rds_oda_decode_b(gtc, _blk2 & 0x1f, _blk4);
        }
        break;
    case 10:
        if (gtv == 0)   /* Programme Type Name */
        {
            rds_ptyn_decode((_blk2 >> 4) & 1, _blk2 & 1,
                            (_blk3 >> 8) & 0xff,
                            (_blk3 >> 0) & 0xff,
                            (_blk4 >> 8) & 0xff,
                            (_blk4 >> 0) & 0xff);
        }
        else     /* Open Data Application */
        {
            rds_oda_decode_b(gtc, _blk2 & 0x1f, _blk4);
        }
        break;
    case 11: /* Open Data Application */
        if (gtv == 0)
        {
            rds_oda_decode_a(gtc, _blk2 & 0x1f, _blk3, _blk4);
        }
        else
        {
            rds_oda_decode_b(gtc, _blk2 & 0x1f, _blk4);
        }
        break;
    case 12: /* Open Data Application */
        if (gtv == 0)
        {
            rds_oda_decode_a(gtc, _blk2 & 0x1f, _blk3, _blk4);
        }
        else
        {
            rds_oda_decode_b(gtc, _blk2 & 0x1f, _blk4);
        }
        break;
    case 13:
        if (gtv == 0)   /* Enhanced Radio Paging or ODA */
        {
            if (rds_program_current->oda[gtc][gtv] != 0)
            {
                rds_oda_decode_a(gtc, _blk2 & 0x1f, _blk3, _blk4);
            }
            else
            {
                rds_erp_decode(_blk2 & 7, (_blk2 >> 3) & 3, _blk3, _blk4);
            }
        }
        else     /* Open Data Application */
        {
            rds_oda_decode_b(gtc, _blk2 & 0x1f, _blk4);
        }
        break;
    case 14: /* Enhanced Other Networks information */
        /* load program */
        rds_program_eon = rds_program_load(rds_program_current->ecc, _blk4);
        memcpy(&rds_program_eon_old, rds_program_eon, sizeof(rds_program_t));
        rds_eon_decode_pi(_blk4);
        rds_eon_decode_tp_on(rds_program_eon, (_blk2 >> 4) & 1);
        if (gtv == 0)
        {
            rds_eon_decode(rds_program_eon, _blk2 & 0xf, _blk3);
        }
        else
        {
            rds_eon_decode_ta_on(rds_program_eon, (_blk2 >> 3) & 1);
        }
        break;
    default: /* case 15 */
        if (gtv == 0)   /* former US NRSC RDBS, now ODA */
        {
            rds_oda_decode_a(gtc, _blk2 & 0x1f, _blk3, _blk4);
        }
        else     /* Fast basic tuning and switching information */
        {
            rds_tp_decode((_blk4 >> 10) & 1);
            rds_pty_decode((_blk4 >> 5) & 0x1f);
            rds_ta_decode((_blk4 >> 4) & 1);
            rds_ms_decode((_blk4 >> 3) & 1);
            rds_di_decode(_blk4 & 0x3, (_blk4 >> 2) & 0x1);
        }
        break;
    }

    /* check for changes */
    if (memcmp(rds_program_current, &rds_program_current_old, sizeof(rds_program_t)) != 0)
        change = 1;

    /* save program */
    if (((rds_program_save_config == 3) && (change == 1)) ||
            (rds_program_save_config == 4))
        rds_program_save(rds_program_current);

    /* execute callback */
    if (rds_callback != NULL)
    {
        if (change == 1)
            rds_callback(rds_program_current, &rds_program_current_old);
        if ((gtc == 14) && (rds_program_eon != NULL) && (memcmp(rds_program_eon, &rds_program_eon_old, sizeof(rds_program_t)) != 0))
            rds_callback(rds_program_eon, &rds_program_eon_old);
    }
}
/*@+mustfreefresh@@+mustfreeonly@*/


/**
 * \brief Open language database
 *
 * This function opens the database for the given language.
 * If _lang is NULL, it continues using the open database.
 * If there is non open or the new language could not be loaded, it opens the default database.
 *
 * \param[in] _lang Language (e.g. "de_DE", "", NULL)
 * \return          Exit status (EXIT_SUCCESS, EXIT_FAILURE)
 */
/*@-globstate@*/
int rds_db_lang_open(/*@null@*/ char *_lang)
{
    char filename[80];

    /* reset variables */
    memset(&filename, 0, sizeof(filename));

    /* check if _lang says continue or default */
    if ((_lang == NULL) || (strlen(_lang) == 0))
    {
        if (rds_db_lang != NULL)
        {
            /* continue using the existing database or open default database */
            return EXIT_SUCCESS;
        }
        else
        {
            /* use default database */
            (void) snprintf(&filename[0], sizeof(filename),
                            DATADIR "/rds_lang_en_CEN.db");
        }
    }
    else
    {
        /* use supplied database */
        (void) snprintf(&filename[0], sizeof(filename),
                        DATADIR "/rds_lang_%s.db", _lang);
    }

    /* open specific database */
    if (sqlite3_open(&filename[0], /*@-nullstate@*/ &rds_db_lang /*@+nullstate@*/) == 0)
        return EXIT_SUCCESS;

    /* clean up */
    if (rds_db_lang != NULL)
        (void) sqlite3_close(rds_db_lang);

    /* open default database */
    if (sqlite3_open(DATADIR "/rds_lang_en_CEN.db", /*@-nullstate@*/ &rds_db_lang /*@+nullstate@*/) == 0)
        return EXIT_SUCCESS;

    /* clean up */
    if (rds_db_lang != NULL)
        (void) sqlite3_close(rds_db_lang);

    return EXIT_FAILURE;
}
/*@+globstate@*/


/**
 * \brief Open program database
 *
 * This function checks if the database is open.
 * In case it's not it opens the database.
 *
 * \return Exit status (EXIT_SUCCESS, EXIT_FAILURE)
 */
/*@-globstate@*/
static int rds_db_program_open(void)
{
    char filename[80];

    /* check if it's already open */
    if (rds_db_program)
        return EXIT_SUCCESS;

    /* create database filename */
    (void) snprintf(&filename[0], sizeof(filename), "%s/rds_program.db",
                    /*@-unrecog@*/ SHAREDSTATEDIR /*@+unrecog@*/);

    /* open database */
    if (sqlite3_open(&filename[0], /*@-nullstate@*/ &rds_db_program /*@+nullstate@*/) != 0)
    {
        if (rds_db_program != NULL)
            (void) sqlite3_close(rds_db_program);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
/*@+globstate@*/


/**
 * \brief Load RDS program
 *
 * This function loads a RDS program from the persistent storage.
 * Assume that the handle is only valid until the next RDS decode.
 * The function only checks for PI, not for ECC as it may not be received yet.
 *
 * \param[in] _ecc ECC Code
 * \param[in] _pi  PI Code
 * \return         Pointer to RDS program
 */
/*@-globstate@*/
rds_program_t *rds_program_load(uint8_t _ecc, uint16_t _pi)
{
    int8_t i;
    int8_t first_free_index = -1;
    int8_t oldest_index = -1;
    time_t oldest_update = 0;

    /* get access to private data */
    rds_program_private_t *rds_program_current_private =
        (rds_program_private_t *) rds_program_current;

    /* check if this is the current program */
    if ((rds_program_current != NULL) &&
            (rds_program_current->pi == _pi))
    {
        rds_program_current_private->last_update = time(NULL);
        /*@-immediatetrans@@-unqualifiedtrans@*/
        return rds_program_current;
        /*@+immediatetrans@@+unqualifiedtrans@*/
    }

    /* find appropriate entry */
    for (i = 0; i < 64; i++)
    {
        /* check if this is the correct program */
        if (rds_program[i].public.pi == _pi)
        {
            rds_program[i].last_update = time(NULL);
            /*@-immediatetrans@*/
            return &rds_program[i].public;
            /*@+immediatetrans@*/
        }

        /* check if this entry is free */
        else if (rds_program[i].public.pi == 0)
        {
            /* remember first free index */
            if (first_free_index < 0)
                first_free_index = i;
        }

        /* check if this is the oldest program */
        else if ((oldest_index < 0) || (rds_program[i].last_update < oldest_update))
        {
            oldest_index = i;
            oldest_update = rds_program[i].last_update;
        }
    }
    /* if we reach this point, the program was not found */

    /* select new entry */
    if (first_free_index >= 0)
    {
        /* use first free index to load program */
        i = first_free_index;
    }
    else
    {
        /* all indices in use, replace oldest program */
        i = oldest_index;
        if (rds_program_save_config == 1)
            rds_program_save(&rds_program[i].public);
    }

    /* init free entry */
    memset(&rds_program[i], 0, sizeof(rds_program_private_t));
    rds_program[i].public.pi = _pi;
    rds_program[i].public.ecc = _ecc;

    /* load only if configured */
    if (rds_program_load_config == RDS_PROGRAM_LOAD_CONFIG_ON_NEW_PROGRAM)
    {
        if (rds_db_program_open() == EXIT_SUCCESS)
        {
            char sql[300];
            sqlite3_stmt *stmt;
            int row;
            int j;

            /* prepare SQL */
            (void) snprintf(&sql[0], sizeof(sql),
                            "select"
                            " PS,PTY,TP,AF,TA"
                            ",DI_ST,DI_AH,DI_CO,DI_DP,MS"
                            ",PIN,ISO,ITU,LIC,LA"
                            ",RT,ODA,CT,PTYN"
                            ",EON_MF"
                            ",ODA_RTP_CB,ODA_RTP_SCB,ODA_RTP_TN,ODA_RTP_ITB,ODA_RTP_IRB,ODA_RTP_CT1,ODA_RTP_CT2"
                            ",ODA_ERT"
#ifdef ODA_TMC
                            ",ODA_TMC_CID,ODA_TMC_LID,ODA_TMC_SID"
#endif
                            " from RDS where lower(ECC)='%2.2x' and lower(PI)='%4.4x'",
                            (unsigned int) rds_program[i].public.ecc,
                            (unsigned int) rds_program[i].public.pi);
            stmt = NULL;
            /*@-nullpass@*/
            (void) sqlite3_prepare(rds_db_program, sql, (int) sizeof(sql), /*@-nullstate@*/ &stmt /*@+nullstate@*/, NULL);

            /* get results */
            if (sqlite3_step(stmt) == SQLITE_ROW)
            {
                char af_str[512];
                uint8_t af[2];
                char ct_str[25];
                struct tm tm;
                row = 0;
                wcsncpy(&rds_program[i].public.ps[0],
                        /*@-mustfreefresh@*/
                        sqlite3_column_text16(stmt, row++),
                        /*@+mustfreefresh@*/
                        sizeof(rds_program[i].public.ps)/sizeof(wchar_t));
                rds_program[i].public.pty         = (uint8_t) sqlite3_column_int(stmt, row++);
                rds_program[i].public.tp          = (uint8_t) sqlite3_column_int(stmt, row++);
                /*@-mustfreefresh@*/
                strncpy(&af_str[0],
                        (char *) sqlite3_column_text(stmt, row++),
                        sizeof(af_str));
                /*@+mustfreefresh@*/
                rds_program[i].public.ta          = (uint8_t) sqlite3_column_int(stmt, row++);
                rds_program[i].public.di_st       = (uint8_t) sqlite3_column_int(stmt, row++);
                rds_program[i].public.di_ah       = (uint8_t) sqlite3_column_int(stmt, row++);
                rds_program[i].public.di_co       = (uint8_t) sqlite3_column_int(stmt, row++);
                rds_program[i].public.di_dp       = (uint8_t) sqlite3_column_int(stmt, row++);
                rds_program[i].public.ms          = (uint8_t) sqlite3_column_int(stmt, row++);
                /*@-mustfreefresh@*/
                (void) sscanf((char *) sqlite3_column_text(stmt, row++),
                              "%u:%u:%u",
                              (unsigned int *) &rds_program[i].public.pin.day,
                              (unsigned int *) &rds_program[i].public.pin.hour,
                              (unsigned int *) &rds_program[i].public.pin.minute);
                /*@+mustfreefresh@*/
                /*@-mustfreefresh@*/
                strncpy(&rds_program[i].public.iso[0],
                        (char *) sqlite3_column_text(stmt, row++),
                        sizeof(rds_program[i].public.iso));
                /*@+mustfreefresh@*/
                rds_program[i].public.itu         = (uint8_t) sqlite3_column_int(stmt, row++);
                rds_program[i].public.lic         = (uint8_t) sqlite3_column_int(stmt, row++);
                rds_program[i].public.la          = (uint8_t) sqlite3_column_int(stmt, row++);
                /*@-mustfreefresh@*/
                wcsncpy(&rds_program[i].public.rt[0],
                        sqlite3_column_text16(stmt, row++),
                        sizeof(rds_program[i].public.rt)/sizeof(wchar_t));
                /*@+mustfreefresh@*/
                /*@-mustfreefresh@*/
                (void) sscanf((char *) sqlite3_column_text(stmt, row++),
                              "%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u",
                              (unsigned int *) &rds_program[i].public.oda[ 0][0], (unsigned int *) &rds_program[i].public.oda[ 0][1],
                              (unsigned int *) &rds_program[i].public.oda[ 1][0], (unsigned int *) &rds_program[i].public.oda[ 1][1],
                              (unsigned int *) &rds_program[i].public.oda[ 2][0], (unsigned int *) &rds_program[i].public.oda[ 2][1],
                              (unsigned int *) &rds_program[i].public.oda[ 3][0], (unsigned int *) &rds_program[i].public.oda[ 3][1],
                              (unsigned int *) &rds_program[i].public.oda[ 4][0], (unsigned int *) &rds_program[i].public.oda[ 4][1],
                              (unsigned int *) &rds_program[i].public.oda[ 5][0], (unsigned int *) &rds_program[i].public.oda[ 5][1],
                              (unsigned int *) &rds_program[i].public.oda[ 6][0], (unsigned int *) &rds_program[i].public.oda[ 6][1],
                              (unsigned int *) &rds_program[i].public.oda[ 7][0], (unsigned int *) &rds_program[i].public.oda[ 7][1],
                              (unsigned int *) &rds_program[i].public.oda[ 8][0], (unsigned int *) &rds_program[i].public.oda[ 8][1],
                              (unsigned int *) &rds_program[i].public.oda[ 9][0], (unsigned int *) &rds_program[i].public.oda[ 9][1],
                              (unsigned int *) &rds_program[i].public.oda[10][0], (unsigned int *) &rds_program[i].public.oda[10][1],
                              (unsigned int *) &rds_program[i].public.oda[11][0], (unsigned int *) &rds_program[i].public.oda[11][1],
                              (unsigned int *) &rds_program[i].public.oda[12][0], (unsigned int *) &rds_program[i].public.oda[12][1],
                              (unsigned int *) &rds_program[i].public.oda[13][0], (unsigned int *) &rds_program[i].public.oda[13][1],
                              (unsigned int *) &rds_program[i].public.oda[14][0], (unsigned int *) &rds_program[i].public.oda[14][1],
                              (unsigned int *) &rds_program[i].public.oda[15][0], (unsigned int *) &rds_program[i].public.oda[15][1]);
                /*@+mustfreefresh@*/
                /*@-mustfreefresh@*/
                strncpy(&ct_str[0],
                        (char *) sqlite3_column_text(stmt, row++),
                        sizeof(ct_str));
                /*@+mustfreefresh@*/
                /*@-mustfreefresh@*/
                wcsncpy(&rds_program[i].public.ptyn[0],
                        sqlite3_column_text16(stmt, row++),
                        sizeof(rds_program[i].public.ptyn)/sizeof(wchar_t));
                /*@+mustfreefresh@*/
                /*@-mustfreefresh@*/
                (void) sscanf((char *) sqlite3_column_text(stmt, row++),
                              "%u>%u,%u>%u,%u>%u,%u>%u,%u>%u",
                              (unsigned int *) &rds_program[i].public.eon_mf[0].tn, (unsigned int *) &rds_program[i].public.eon_mf[0].mf,
                              (unsigned int *) &rds_program[i].public.eon_mf[1].tn, (unsigned int *) &rds_program[i].public.eon_mf[1].mf,
                              (unsigned int *) &rds_program[i].public.eon_mf[2].tn, (unsigned int *) &rds_program[i].public.eon_mf[2].mf,
                              (unsigned int *) &rds_program[i].public.eon_mf[3].tn, (unsigned int *) &rds_program[i].public.eon_mf[3].mf,
                              (unsigned int *) &rds_program[i].public.eon_mf[4].tn, (unsigned int *) &rds_program[i].public.eon_mf[4].mf);
                /*@+mustfreefresh@*/
                rds_program[i].public.oda_rtp_cb = (uint8_t) sqlite3_column_int(stmt, row++);
                rds_program[i].public.oda_rtp_scb = (uint8_t) sqlite3_column_int(stmt, row++);
                rds_program[i].public.oda_rtp_tn = (uint8_t) sqlite3_column_int(stmt, row++);
                rds_program[i].public.oda_rtp_itb = (uint8_t) sqlite3_column_int(stmt, row++);
                rds_program[i].public.oda_rtp_irb = (uint8_t) sqlite3_column_int(stmt, row++);
                rds_program[i].public.oda_rtp_ct[0] = (uint8_t) sqlite3_column_int(stmt, row++);
                rds_program[i].public.oda_rtp_ct[1] = (uint8_t) sqlite3_column_int(stmt, row++);
                /*@-mustfreefresh@*/
                wcsncpy(&rds_program[i].public.oda_ert[0],
                        sqlite3_column_text16(stmt, row++),
                        sizeof(rds_program[i].public.oda_ert)/sizeof(wchar_t));
                /*@+mustfreefresh@*/
#ifdef ODA_TMC
                rds_program[i].public.oda_tmc_cid = (uint8_t) sqlite3_column_int(stmt, row++);
                rds_program[i].public.oda_tmc_lid = (uint8_t) sqlite3_column_int(stmt, row++);
                rds_program[i].public.oda_tmc_sid = (uint8_t) sqlite3_column_int(stmt, row++);
#endif

                /* handle AF */
                j = 0;
                while (af_str[j] != '\0')
                {
                    if (sscanf(&af_str[j], "%u,%u", (unsigned int *) &af[0], (unsigned int *) &af[1]) == 2)
                    {
                        /* do something with it */
                        rds_af_handle(&rds_program[i].public.af[0], &rds_program[i].af, af[0], af[1]);
                        /* overstep first comma */
                        while (af_str[j] != ',')
                            j++;
                        j++;
                        /* overstep second comma */
                        while ((af_str[j] != ',') && (af_str[j] != '\0'))
                            j++;
                        if (af_str[j] != '\0')
                            j++;
                    };
                }

                /* handle CT */
                memmove(&ct_str[22], &ct_str[23], 3);
                memset(&tm, 0, sizeof(tm));
                /*@-unrecog@*/
                (void) strptime(&ct_str[0], "%Y-%m-%dT%H:%M:%S%z", &tm);
                /*@+unrecog@*/
                rds_program[i].public.ct = mktime(&tm);
                /*@-type@*/
                rds_program[i].public.ct_lto = tm.tm_gmtoff / 60 / 30; /* convert to half hours */
                /*@+type@*/
            }

            /* finish SQL */
            (void) sqlite3_finalize(stmt);
            /*@+nullpass@*/

            /* prepare SQL */
            (void) snprintf(&sql[0], sizeof(sql),
                            "select EON_PI,EON_LA,EON_EG,EON_ILS,EON_LSN "
                            "from EON_LI where lower(ECC)='%2.2x' and lower(PI)='%4.4x'",
                            (unsigned int) rds_program[i].public.ecc,
                            (unsigned int) rds_program[i].public.pi);
            stmt = NULL;
            /*@-nullpass@*/
            (void) sqlite3_prepare(rds_db_program, sql, (int) sizeof(sql), /*@-nullstate@*/ &stmt /*@+nullstate@*/, NULL);

            /* get results */
            j = 0;
            while ((j < rds_eon_li_max) && (sqlite3_step(stmt) == SQLITE_ROW))
            {
                row = 0;
                /*@-mustfreefresh@*/
                (void) sscanf((char *) sqlite3_column_text(stmt, row++),
                              "%x",
                              (unsigned int *) &rds_program[i].public.eon_li[j].pi);
                /*@+mustfreefresh@*/
                rds_program[i].public.eon_li[j].la = (uint8_t) sqlite3_column_int(stmt, row++);
                rds_program[i].public.eon_li[j].eg = (uint8_t) sqlite3_column_int(stmt, row++);
                rds_program[i].public.eon_li[j].ils = (uint8_t) sqlite3_column_int(stmt, row++);
                rds_program[i].public.eon_li[j].lsn = (uint8_t) sqlite3_column_int(stmt, row++);
                j++;
            }

            /* finish SQL */
            (void) sqlite3_finalize(stmt);
            /*@+nullpass@*/

            /* prepare SQL */
            (void) snprintf(&sql[0], sizeof(sql),
                            "select CT,RTP"
                            "from ODA_RTP where lower(ECC)='%2.2x' and lower(PI)='%4.4x'",
                            (unsigned int) rds_program[i].public.ecc,
                            (unsigned int) rds_program[i].public.pi);
            stmt = NULL;
            /*@-nullpass@*/
            (void) sqlite3_prepare(rds_db_program, sql, (int) sizeof(sql), /*@-nullstate@*/ &stmt /*@+nullstate@*/, NULL);

            /* get results */
            while (sqlite3_step(stmt) == SQLITE_ROW)
            {
                unsigned int ct;

                row = 0;
                ct = (unsigned int) sqlite3_column_int(stmt, row++);
                /*@-mustfreefresh@*/
                wcsncpy(&rds_program[i].public.oda_rtp[ct][0],
                        sqlite3_column_text16(stmt, row++),
                        sizeof(rds_program[i].public.oda_rtp[ct])/sizeof(wchar_t));
                /*@+mustfreefresh@*/
            }

            /* finish SQL */
            (void) sqlite3_finalize(stmt);
            /*@+nullpass@*/
        }
    }

    /* update CT */
    rds_program[i].public.ct = rds_ct_from_gmtime();

    /*@-immediatetrans@*/
    return &rds_program[i].public;
    /*@+immediatetrans@*/
}
/*@+globstate@*/


/**
 * \brief Save RDS program
 *
 * This function saves a RDS program to persistent storage.
 * The data is saved to the tables RDS, EON_LI and ODA_RTP.
 *
 * \param[in] _rs RDS program
 */
void rds_program_save(rds_program_t *_rs)
{
    char sql[500];
    sqlite3_stmt *stmt = NULL;
    uint16_t i, j;
    char af_str[512];
    time_t t;
    struct tm *tm;
    char ct_str[26];

    if (rds_db_program_open() != EXIT_SUCCESS)
        return;

    /* delete old entry */
    (void) snprintf(&sql[0], sizeof(sql),
                    "delete from RDS where lower(ECC)='%2.2x' and lower(PI)='%4.4x'",
                    (unsigned int) _rs->ecc,
                    (unsigned int) _rs->pi);

    /* execute SQL */
    /*@-nullpass@*/
    stmt = NULL;
    (void) sqlite3_prepare(rds_db_program, sql, (int) sizeof(sql), /*@-nullstate@*/ &stmt /*@+nullstate@*/, NULL);
    (void) sqlite3_step(stmt);
    (void) sqlite3_finalize(stmt);
    /*@+nullpass@*/

    /* prepare AF */
    memset(&af_str[0], 0, sizeof(af_str));
    j = 0;
    for (i = 0; (i < rds_af_max) && (_rs->af[i] != 0); i++)
    {
        if (i == 0)
        {
            j += snprintf(&af_str[j], sizeof(af_str) - j, "%u",
                          (unsigned int) _rs->af[i]);
        }
        else
        {
            j--; /* overwrite \n with , */
            j += snprintf(&af_str[j], sizeof(af_str) - j, ",%u",
                          (unsigned int) _rs->af[i]);
        }
    }

    /* prepare CT */
    t = _rs->ct;
    tm = localtime(&t);
    (void) strftime(&ct_str[0], sizeof(ct_str), "%Y-%m-%dT%H:%M:%S%z", tm);
    /* remove : in time zone */
    memmove(&ct_str[23], &ct_str[22], 3);
    ct_str[22] = ':';

    /* insert new entry */
    (void) snprintf(&sql[0], sizeof(sql),
                    "insert into RDS"
                    " (PI,PS,PTY,TP,AF,TA"
                    ",DI_ST,DI_AH,DI_CO,DI_DP,MS"
                    ",PIN,ECC,ISO,ITU,LIC,LA"
                    ",RT"
                    ",ODA"
                    ",CT,PTYN"
                    ",EON_MF"
                    ",ODA_RTP_CB,ODA_RTP_SCB,ODA_RTP_TN,ODA_RTP_ITB,ODA_RTP_IRB,ODA_RTP_CT1,ODA_RTP_CT2"
                    ",ODA_ERT"
#ifdef ODA_TMC
                    ",ODA_TMC_CID,ODA_TMC_LID,ODA_TMC_SID"
#endif
                    ") values ("
                    "'%4.4x','%ls',%u,%u,'%s',%u"
                    ",%u,%u,%u,%u,%u"
                    ",'%2.2u:%2.2u:%2.2u','%2.2x','%s',%u,%u,%u"
                    ",'%ls'"
                    ",'%4.4x,%4.4x,%4.4x,%4.4x,%4.4x,%4.4x,%4.4x,%4.4x,%4.4x,%4.4x,%4.4x,%4.4x,%4.4x,%4.4x,%4.4x,%4.4x"
                    ",%4.4x,%4.4x,%4.4x,%4.4x,%4.4x,%4.4x,%4.4x,%4.4x,%4.4x,%4.4x,%4.4x,%4.4x,%4.4x,%4.4x,%4.4x,%4.4x'"
                    ",'%s','%ls'"
                    ",'%u>%u,%u>%u,%u>%u,%u>%u,%u>%u'"
                    ",%u,%u,%u,%u,%u,%u,%u"
                    ",'%ls'"
#ifdef ODA_TMC
                    ",%u,%u,%u"
#endif
                    ")",
                    (unsigned int) _rs->pi, (char *) _rs->ps, (unsigned int) _rs->pty, (unsigned int) _rs->tp, &af_str[0], (unsigned int) _rs->ta,
                    (unsigned int) _rs->di_st, (unsigned int) _rs->di_ah, (unsigned int) _rs->di_co, (unsigned int) _rs->di_dp, (unsigned int) _rs->ms,
                    (unsigned int) _rs->pin.day, (unsigned int) _rs->pin.hour, (unsigned int) _rs->pin.minute, (unsigned int) _rs->ecc, (char *) _rs->iso, (unsigned int) _rs->itu, (unsigned int) _rs->lic, (unsigned int) _rs->la,
                    (char *) _rs->rt,
                    (unsigned int) _rs->oda[ 0][0], (unsigned int) _rs->oda[ 0][1],
                    (unsigned int) _rs->oda[ 1][0], (unsigned int) _rs->oda[ 1][1],
                    (unsigned int) _rs->oda[ 2][0], (unsigned int) _rs->oda[ 2][1],
                    (unsigned int) _rs->oda[ 3][0], (unsigned int) _rs->oda[ 3][1],
                    (unsigned int) _rs->oda[ 4][0], (unsigned int) _rs->oda[ 4][1],
                    (unsigned int) _rs->oda[ 5][0], (unsigned int) _rs->oda[ 5][1],
                    (unsigned int) _rs->oda[ 6][0], (unsigned int) _rs->oda[ 6][1],
                    (unsigned int) _rs->oda[ 7][0], (unsigned int) _rs->oda[ 7][1],
                    (unsigned int) _rs->oda[ 8][0], (unsigned int) _rs->oda[ 8][1],
                    (unsigned int) _rs->oda[ 9][0], (unsigned int) _rs->oda[ 9][1],
                    (unsigned int) _rs->oda[10][0], (unsigned int) _rs->oda[10][1],
                    (unsigned int) _rs->oda[11][0], (unsigned int) _rs->oda[11][1],
                    (unsigned int) _rs->oda[12][0], (unsigned int) _rs->oda[12][1],
                    (unsigned int) _rs->oda[13][0], (unsigned int) _rs->oda[13][1],
                    (unsigned int) _rs->oda[14][0], (unsigned int) _rs->oda[14][1],
                    (unsigned int) _rs->oda[15][0], (unsigned int) _rs->oda[15][1],
                    &ct_str[0], (char *) _rs->ptyn,
                    (unsigned int) _rs->eon_mf[0].tn, (unsigned int) _rs->eon_mf[0].mf,
                    (unsigned int) _rs->eon_mf[1].tn, (unsigned int) _rs->eon_mf[1].mf,
                    (unsigned int) _rs->eon_mf[2].tn, (unsigned int) _rs->eon_mf[2].mf,
                    (unsigned int) _rs->eon_mf[3].tn, (unsigned int) _rs->eon_mf[3].mf,
                    (unsigned int) _rs->eon_mf[4].tn, (unsigned int) _rs->eon_mf[4].mf,
                    (unsigned int) _rs->oda_rtp_cb, (unsigned int) _rs->oda_rtp_scb, (unsigned int) _rs->oda_rtp_tn, (unsigned int) _rs->oda_rtp_itb, (unsigned int) _rs->oda_rtp_irb, (unsigned int) _rs->oda_rtp_ct[0], (unsigned int) _rs->oda_rtp_ct[1],
                    (char *) _rs->oda_ert
#ifdef ODA_TMC
                    , (unsigned int) _rs->oda_tmc_cid, (unsigned int) _rs->oda_tmc_lid, (unsigned int) _rs->oda_tmc_sid
#endif
                   );

    /* execute SQL */
    /*@-nullpass@*/
    stmt = NULL;
    (void) sqlite3_prepare(rds_db_program, sql, (int) sizeof(sql), /*@-nullstate@*/ &stmt /*@+nullstate@*/, NULL);
    (void) sqlite3_step(stmt);
    (void) sqlite3_finalize(stmt);
    /*@+nullpass@*/

    /* delete old entries */
    (void) snprintf(&sql[0], sizeof(sql),
                    "delete from EON_LI where lower(ECC)='%2.2x' and lower(PI)='%4.4x'",
                    (unsigned int) _rs->ecc,
                    (unsigned int) _rs->pi);

    /* execute SQL */
    /*@-nullpass@*/
    stmt = NULL;
    (void) sqlite3_prepare(rds_db_program, sql, (int) sizeof(sql), /*@-nullstate@*/ &stmt /*@+nullstate@*/, NULL);
    (void) sqlite3_step(stmt);
    (void) sqlite3_finalize(stmt);
    /*@+nullpass@*/

    /* insert new entries */
    for (i = 0; (i < rds_eon_li_max) && (_rs->eon_li[i].pi != 0); i++)
    {
        (void) snprintf(&sql[0], sizeof(sql),
                        "insert into EON_LI (PI,ECC,EON_PI,EON_LA,EON_EG,EON_ILS,EON_LSN) "
                        "values ('%4.4x','%2.2x','%4.4x',%u,%u,%u,%u)",
                        (unsigned int) _rs->pi,
                        (unsigned int) _rs->ecc,
                        (unsigned int) _rs->eon_li[i].pi,
                        (unsigned int) _rs->eon_li[i].la,
                        (unsigned int) _rs->eon_li[i].eg,
                        (unsigned int) _rs->eon_li[i].ils,
                        (unsigned int) _rs->eon_li[i].lsn);

        /* execute SQL */
        /*@-nullpass@*/
        stmt = NULL;
        (void) sqlite3_prepare(rds_db_program, sql, (int) sizeof(sql), /*@-nullstate@*/ &stmt /*@+nullstate@*/, NULL);
        (void) sqlite3_step(stmt);
        (void) sqlite3_finalize(stmt);
        /*@+nullpass@*/
    }

    /* delete old entries */
    (void) snprintf(&sql[0], sizeof(sql),
                    "delete from ODA_RTP where lower(ECC)='%2.2x' and lower(PI)='%4.4x'",
                    (unsigned int) _rs->ecc,
                    (unsigned int) _rs->pi);

    /* execute SQL */
    /*@-nullpass@*/
    stmt = NULL;
    (void) sqlite3_prepare(rds_db_program, sql, (int) sizeof(sql), /*@-nullstate@*/ &stmt /*@+nullstate@*/, NULL);
    (void) sqlite3_step(stmt);
    (void) sqlite3_finalize(stmt);
    /*@+nullpass@*/

    /* insert new */
    for (i = 0; i < 64; i++)
    {
        if (wcslen(&_rs->oda_rtp[i][0]) > 0)
        {
            (void) snprintf(&sql[0], sizeof(sql),
                            "insert into ODA_RTP (PI,ECC,CT,RTP) "
                            "values ('%4.4x','%2.2x',%u,'%s')",
                            (unsigned int) _rs->pi,
                            (unsigned int) _rs->ecc,
                            (unsigned int) _rs->oda_rtp_ct[0],
                            (char *) &_rs->oda_rtp[i][0]);

            /* execute SQL */
            /*@-nullpass@*/
            stmt = NULL;
            (void) sqlite3_prepare(rds_db_program, sql, (int) sizeof(sql), /*@-nullstate@*/ &stmt /*@+nullstate@*/, NULL);
            (void) sqlite3_step(stmt);
            (void) sqlite3_finalize(stmt);
            /*@+nullpass@*/
        }
    }

    /* execute SQL */
    /*@-nullpass@*/
    stmt = NULL;
    (void) sqlite3_prepare(rds_db_program, sql, (int) sizeof(sql), /*@-nullstate@*/ &stmt /*@+nullstate@*/, NULL);
    (void) sqlite3_step(stmt);
    (void) sqlite3_finalize(stmt);
    /*@+nullpass@*/
}


/**
 * \brief Save all RDS programs
 *
 * This function saves all RDS programs to persistent storage.
 */
void rds_program_save_all(void)
{
    uint8_t i;

    for (i = 0; i < 64; i++)
        if (rds_program[i].public.pi != 0)
            rds_program_save(&rds_program[i].public);
}
