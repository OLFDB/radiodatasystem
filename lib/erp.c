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
 * \file erp.c
 * \test All related test cases are defined in \ref erp_test.c
 * \brief Enhanced Radio Paging functions
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains functions for Enhanced Radio Paging handling.
 */

#include <stdint.h>
#include "erp.h"
#include "rds_private.h"


/**
 * \brief Decodes and handles received ERP information
 *
 * This function decodes and handles received ERP information.
 *
 * \param[in] _sty  Sub type
 * \param[in] _cs   Cycle Selection
 * \param[in] _blk3 Information field from blk3
 * \param[in] _blk4 Information field from blk4
 */
void rds_erp_decode(uint8_t _sty, uint8_t _cs, uint16_t _blk3, uint16_t _blk4)
{
    /* get access to private data */
    rds_program_private_t *rds_program_current_private =
        (rds_program_private_t *) rds_program_current;

    /* Cycle Selection */
    rds_program_current_private->erp_cs = _cs;
    if (rds_program_current_private->erp_cs == 1)
        rds_decode_status = RDS_DECODE_STATUS_NOT_ASSIGNED;

    /* paging Interval numbering */
    rds_program_current_private->erp_it = (_blk3 >> 12) & 0xf;

    /* reserved for future use */
    if (((_blk3 >> 9) & 0x1) != 0)
        rds_decode_status = RDS_DECODE_STATUS_NOT_ASSIGNED;

    /* Message sorting */
    rds_program_current_private->erp_s = (_blk3 >> 10) & 0x3;
    if (rds_program_current_private->erp_s == 1)
        rds_decode_status = RDS_DECODE_STATUS_NOT_ASSIGNED;

    /* Evaluate sub type */
    switch (_sty)
    {
    case 0: /* address notification bits 24..0, when only 25 bits are used */
        rds_program_current_private->erp_anb25 = ((_blk3 & 0x1ff) << 16) | _blk4;
        break;
    case 1: /* address notification bits 49..25, when 50 bits are used */
        rds_program_current_private->erp_anb50b = ((_blk3 & 0x1ff) << 16) | _blk4;
        break;
    case 2: /* address notification bits 24..0, when 50 bits are used */
        rds_program_current_private->erp_anb50a = ((_blk3 & 0x1ff) << 16) | _blk4;
        break;
    case 3: /* reserved for Value Added Services (VAS) system information */
        rds_decode_status = RDS_DECODE_STATUS_NOT_IMPLEMENTED_YET;
        break;
    default: /* 4..7: reserved for future use */
        rds_decode_status = RDS_DECODE_STATUS_NOT_ASSIGNED;
        break;
    }
}


/**
 * \brief Process Enhanced Paging Operator Code
 *
 * This function processes Enhanced Paging Operator Code.
 *
 * \param[in] _opc Operator Code
 */
void rds_erp_decode_opc(uint8_t _opc)
{
    /* get access to private data */
    rds_program_private_t *rds_program_current_private =
        (rds_program_private_t *) rds_program_current;

    rds_program_current_private->erp_opc = _opc;    /* OPerator Code */
}


/**
 * \brief Process Enhanced Paging Paging Area Code
 *
 * This function processes Enhanced Paging Paging Area Code.
 *
 * \param[in] _pac Paging Area Code
 */
void rds_erp_decode_pac(uint8_t _pac)
{
    /* get access to private data */
    rds_program_private_t *rds_program_current_private =
        (rds_program_private_t *) rds_program_current;

    rds_program_current_private->erp_pac = _pac;    /* Paging Area Code */
}


/**
 * \brief Process Enhanced Paging Current Carrier Frequency
 *
 * This function processes Enhanced Paging Current Carrier Frequency.
 *
 * \param[in] _ccf Current Carrier Frequency
 */
void rds_erp_decode_ccf(uint8_t _ccf)
{
    /* get access to private data */
    rds_program_private_t *rds_program_current_private =
        (rds_program_private_t *) rds_program_current;

    rds_program_current_private->erp_ccf = _ccf;    /* Current Carrier Frequency */
}
