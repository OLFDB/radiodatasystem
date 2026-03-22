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
 * \file rds_private.h
 * \brief Radio Data System functions - headers
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains private functions for Radio Data System handling.
 */

#ifndef RDS_PRIVATE_H
#define RDS_PRIVATE_H

#include "af.h"
#include "rds.h"


/**
 * \brief Language specific database
 *
 * This database contains language specific information.
 */
extern sqlite3 *rds_db_lang;


/**
 * \brief Structure for RDS program information including private variables
 *
 * This structure contains all information corresponding to one RDS program (same PI code).
 */
typedef struct rds_program_private_t
{
    rds_program_t   public;                 /**< this is the only public data, the rest is private */
    time_t      last_update;                /**< system time of last RDS load program during update */

    /* Type 0 groups: Basic tuning and switching information */
    rds_af_t    af;                         /**< Alternative Frequency (AF) filter */

    /* Type 1A groups: Enhanced Paging */
    uint8_t     erp_opc;                    /**< ERP OPerator Code */
    uint8_t     erp_pac;                    /**< ERP Paging Area Code */
    uint8_t     erp_ccf;                    /**< ERP Current Carrier Frequency */

    /* Type 2 groups: RadioText */
    uint8_t     rt_last_ab;                 /**< RT last A/B flag */

    /* Type 7A groups: Radio Paging */
    uint8_t     rp_last_ab;                 /**< RP last A/B flag */
    uint8_t     rp_last_sa;                 /**< RP last Paging segment address code */
    uint8_t     rp_type;                    /**< RP type of additional message */
    /* t=0: No additional message */
    /* t=1: 10 digit numeric message */
    /* t=2: 18 digit numeric message */
    /* t=3: alphanumeric message */
    /* t=4: 15 digit numeric message in international paging */
    /* t=5: functions message in international paging */
    uint8_t     rp_y[2];                     /* Y1..Y2 : group code */
    uint8_t     rp_z[4];                     /* Z1..Z4 : individual code within group */
    uint8_t     rp_a[18];                    /* A1..A18: numeric message */
    uint8_t     rp_c[24];                    /* C1..C24: message characters */
    uint8_t     rp_x[3];                     /* X1..X3 : country code according to ITU-T Rec. E212 */
    uint8_t     rp_f[7];                     /* F1..F7 : functions message in international paging */

    /* Type 10A groups: Programme Type Name */
    uint8_t     ptyn_last_ab;                /**< PTYN last A/B flag */

    /* Type 13A groups: Enhanced Paging */
    uint8_t     erp_cs;                      /**< ERP Cycle Selection */
    /* cs = 0: 1 minute cycle */
    /* cs = 1: reserved for future use */
    /* cs = 2: 2 minutes cycle or mixed (even) */
    /* cs = 3: 2 minutes cycle or mixed (odd) */
    uint8_t     erp_it;                     /**< ERP paging InTerval numbering */
    uint8_t     erp_s;                      /**< ERP message Sorting */
    /* s = 0: not sorted */
    /* s = 1: reserved for future use */
    /* s = 2: sorted in ascending order */
    /* s = 3: sorted in descending order */
    uint32_t    erp_anb25;                  /**< ERP address notification bits 24..0, when only 25 bits are used */
    uint32_t    erp_anb50a;                 /**< ERP address notification bits 24..0, when 50 bits are used */
    uint32_t    erp_anb50b;                 /**< ERP address notification bits 49..25, when 50 bits are used */

    /* Type 14 groups: Enhanced Other Networks information */
    uint8_t     eon_li_cnt[rds_eon_li_max]; /**< EON LI reception counter */

#ifdef ODA_TMC
    /* ODA 0x0d45: RDS-TMC: ALERT-C / EN ISO 14819-1 (for testing use, only) */
    uint8_t     oda_tmc_last_gsi;           /**< TMC last received gsi */
    uint64_t    oda_tmc_fifo;               /**< TMC 64 bit fifo: data (msb aligned) */
    uint8_t     oda_tmc_fifo_cnt;           /**< TMC 64 bit fifo: count */
#endif

    /* ODA 0x4bd7: RadioText+ / RT+ */
    uint8_t     oda_rtp_ert;                /**< RT+: RT+ eRT flag (0=RT 1=eRT) */

    /* ODA 0x6552: Enhanced RadioText / eRT */
    uint8_t     oda_ert_utf8;               /**< eRT: UTF-8 flag (0=UTF-16, 1=UTF-8) */
    uint8_t     oda_ert_ctid;               /**< eRT: Character Table ID */
    uint8_t     oda_ert_dtfd;               /**< eRT: Default Text Formatting Direction */
    uint8_t     oda_ert_str[128];           /**< eRT: String */

#ifdef ODA_TMC
    /* ODA 0xcd46: RDS-TMC: ALERT-C / EN ISO 14819-1 (for service use, only) */
    /* uses ODA 0x0d45 variables */
#endif
} rds_program_private_t;


#endif /* RDS_PRIVATE_H */
