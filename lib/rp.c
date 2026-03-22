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
 * \file rp.c
 * \test All related test cases are defined in \ref rp_test.c
 * \brief Radio Paging functions
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains functions for Radio Paging handling.
 *
 * \todo No implementation yet.
 */

#include <stdint.h>
#include "rds_private.h"
#include "rp.h"


/** \brief Process RP message
 *
 * This function processes RP messages.
 *
 * \param[in] _ab Paging A/B
 * \param[in] _sa Paging segment address code
 * \param[in] _p1 Paging data in blk 3
 * \param[in] _p2 Paging data in blk 4
 */
void rds_rp_decode(/*@unused@*/ uint8_t _ab, uint8_t _sa, uint16_t _p1, uint16_t _p2)
{
    /* get access to private data */
    rds_program_private_t *rds_program_current_private =
        (rds_program_private_t *) rds_program_current;

    /* check if A/B has changed */
    if (rds_program_current_private->rp_last_ab != _ab)
        rds_program_current_private->rp_type = 0;

    /* Additional message content */
    switch (_sa)
    {
    case 0:
        /* No additional message */
        rds_program_current_private->rp_type = 0;
        rds_program_current_private->rp_y[0] = (_p1 >> 12) & 0xf;   /* Y1 */
        rds_program_current_private->rp_y[1] = (_p1 >>  8) & 0xf;   /* Y2 */
        rds_program_current_private->rp_z[0] = (_p1 >>  4) & 0xf;   /* Z1 */
        rds_program_current_private->rp_z[1] = (_p1 >>  0) & 0xf;   /* Z2 */
        rds_program_current_private->rp_z[2] = (_p2 >> 12) & 0xf;   /* Z3 */
        rds_program_current_private->rp_z[3] = (_p2 >>  8) & 0xf;   /* Z4 */
        rds_program_current_private->rp_x[0] = (_p2 >>  4) & 0xf;   /* X1 (ERP) */
        rds_program_current_private->rp_x[1] = (_p2 >>  0) & 0xf;   /* X2 (ERP) */
        /** \todo process here */
        break;
    case 1:
        /* part of functions message */
        rds_program_current_private->rp_type = 5;
        rds_program_current_private->rp_y[0] = (_p1 >> 12) & 0xf;   /* Y1 */
        rds_program_current_private->rp_y[1] = (_p1 >>  8) & 0xf;   /* Y2 */
        rds_program_current_private->rp_z[0] = (_p1 >>  4) & 0xf;   /* Z1 */
        rds_program_current_private->rp_z[1] = (_p1 >>  0) & 0xf;   /* Z2 */
        rds_program_current_private->rp_z[2] = (_p2 >> 12) & 0xf;   /* Z3 */
        rds_program_current_private->rp_z[3] = (_p2 >>  8) & 0xf;   /* Z4 */
        rds_program_current_private->rp_x[0] = (_p2 >>  4) & 0xf;   /* X1 */
        rds_program_current_private->rp_x[1] = (_p2 >>  0) & 0xf;   /* X2 */
        break;
    case 2:
        /* 10 digit numeric message */
        rds_program_current_private->rp_type = 1;
        rds_program_current_private->rp_y[0] = (_p1 >> 12) & 0xf;   /* Y1 */
        rds_program_current_private->rp_y[1] = (_p1 >>  8) & 0xf;   /* Y2 */
        rds_program_current_private->rp_z[0] = (_p1 >>  4) & 0xf;   /* Z1 */
        rds_program_current_private->rp_z[1] = (_p1 >>  0) & 0xf;   /* Z2 */
        rds_program_current_private->rp_z[2] = (_p2 >> 12) & 0xf;   /* Z3 */
        rds_program_current_private->rp_z[3] = (_p2 >>  8) & 0xf;   /* Z4 */
        rds_program_current_private->rp_a[0] = (_p2 >>  4) & 0xf;   /* A1 */
        rds_program_current_private->rp_a[1] = (_p2 >>  0) & 0xf;   /* A2 */
        break;
    case 3:
        if ((rds_program_current_private->rp_type == 1) && (rds_program_current_private->rp_last_sa == 2))
        {
            /* 10 digit numeric message */
            rds_program_current_private->rp_a[2] = (_p1 >> 12) & 0xf;   /* A3 */
            rds_program_current_private->rp_a[3] = (_p1 >>  8) & 0xf;   /* A4 */
            rds_program_current_private->rp_a[4] = (_p1 >>  4) & 0xf;   /* A5 */
            rds_program_current_private->rp_a[5] = (_p1 >>  0) & 0xf;   /* A6 */
            rds_program_current_private->rp_a[6] = (_p2 >> 12) & 0xf;   /* A7 */
            rds_program_current_private->rp_a[7] = (_p2 >>  8) & 0xf;   /* A8 */
            rds_program_current_private->rp_a[8] = (_p2 >>  4) & 0xf;   /* A9 */
            rds_program_current_private->rp_a[9] = (_p2 >>  0) & 0xf;   /* A10 */
            /** \todo process here */
        }
        else if ((rds_program_current_private->rp_type == 5) && (rds_program_current_private->rp_last_sa == 1))
        {
            /* part of functions message */
            rds_program_current_private->rp_x[2] = (_p1 >> 12) & 0xf;   /* X3 */
            rds_program_current_private->rp_f[0] = (_p1 >>  8) & 0xf;   /* F1 */
            rds_program_current_private->rp_f[1] = (_p1 >>  4) & 0xf;   /* F2 */
            rds_program_current_private->rp_f[2] = (_p1 >>  0) & 0xf;   /* F3 */
            rds_program_current_private->rp_f[3] = (_p2 >> 12) & 0xf;   /* F4 */
            rds_program_current_private->rp_f[4] = (_p2 >>  8) & 0xf;   /* F5 */
            rds_program_current_private->rp_f[5] = (_p2 >>  4) & 0xf;   /* F6 */
            rds_program_current_private->rp_f[6] = (_p2 >>  0) & 0xf;   /* F7 */
            /** \todo process here */
        }
        break;
    case 4:
        /* 18 digit numeric message */
        rds_program_current_private->rp_type = 2;
        rds_program_current_private->rp_y[0] = (_p1 >> 12) & 0xf;   /* Y1 */
        rds_program_current_private->rp_y[1] = (_p1 >>  8) & 0xf;   /* Y2 */
        rds_program_current_private->rp_z[0] = (_p1 >>  4) & 0xf;   /* Z1 */
        rds_program_current_private->rp_z[1] = (_p1 >>  0) & 0xf;   /* Z2 */
        rds_program_current_private->rp_z[2] = (_p2 >> 12) & 0xf;   /* Z3 */
        rds_program_current_private->rp_z[3] = (_p2 >>  8) & 0xf;   /* Z4 */
        rds_program_current_private->rp_a[0] = (_p2 >>  4) & 0xf;   /* A1 */
        rds_program_current_private->rp_a[1] = (_p2 >>  0) & 0xf;   /* A2 */
        break;
    case 5:
        if ((rds_program_current_private->rp_type == 2) && (rds_program_current_private->rp_last_sa = 4))
        {
            /* 18 digit numeric message */
            rds_program_current_private->rp_a[2] = (_p1 >> 12) & 0xf;   /* A3 */
            rds_program_current_private->rp_a[3] = (_p1 >>  8) & 0xf;   /* A4 */
            rds_program_current_private->rp_a[4] = (_p1 >>  4) & 0xf;   /* A5 */
            rds_program_current_private->rp_a[5] = (_p1 >>  0) & 0xf;   /* A6 */
            rds_program_current_private->rp_a[6] = (_p2 >> 12) & 0xf;   /* A7 */
            rds_program_current_private->rp_a[7] = (_p2 >>  8) & 0xf;   /* A8 */
            rds_program_current_private->rp_a[8] = (_p2 >>  4) & 0xf;   /* A9 */
            rds_program_current_private->rp_a[9] = (_p2 >>  0) & 0xf;   /* A10 */
        }
        else if ((rds_program_current_private->rp_type == 4) && (rds_program_current_private->rp_last_sa = 7))
        {
            /* 15 digit numeric message in international paging */
            rds_program_current_private->rp_x[2] = (_p1 >> 12) & 0xf;   /* X3 */
            rds_program_current_private->rp_a[0] = (_p1 >>  8) & 0xf;   /* A1 */
            rds_program_current_private->rp_a[1] = (_p1 >>  4) & 0xf;   /* A2 */
            rds_program_current_private->rp_a[2] = (_p1 >>  0) & 0xf;   /* A3 */
            rds_program_current_private->rp_a[3] = (_p2 >> 12) & 0xf;   /* A4 */
            rds_program_current_private->rp_a[4] = (_p2 >>  8) & 0xf;   /* A5 */
            rds_program_current_private->rp_a[5] = (_p2 >>  4) & 0xf;   /* A6 */
            rds_program_current_private->rp_a[6] = (_p2 >>  0) & 0xf;   /* A7 */
        }
        break;
    case 6:
        if ((rds_program_current_private->rp_type == 2) && (rds_program_current_private->rp_last_sa = 5))
        {
            /* 18 digit numeric message */
            rds_program_current_private->rp_a[10] = (_p1 >> 12) & 0xf;  /* A11 */
            rds_program_current_private->rp_a[11] = (_p1 >>  8) & 0xf;  /* A12 */
            rds_program_current_private->rp_a[12] = (_p1 >>  4) & 0xf;  /* A13 */
            rds_program_current_private->rp_a[13] = (_p1 >>  0) & 0xf;  /* A14 */
            rds_program_current_private->rp_a[14] = (_p2 >> 12) & 0xf;  /* A15 */
            rds_program_current_private->rp_a[15] = (_p2 >>  8) & 0xf;  /* A16 */
            rds_program_current_private->rp_a[16] = (_p2 >>  4) & 0xf;  /* A17 */
            rds_program_current_private->rp_a[17] = (_p2 >>  0) & 0xf;  /* A18 */
            /** \todo process here */
        }
        else if ((rds_program_current_private->rp_type == 4) && (rds_program_current_private->rp_last_sa = 5))
        {
            /* 15 digit numeric message in international paging */
            rds_program_current_private->rp_a[ 7] = (_p1 >> 12) & 0xf;  /* A8 */
            rds_program_current_private->rp_a[ 8] = (_p1 >>  8) & 0xf;  /* A9 */
            rds_program_current_private->rp_a[ 9] = (_p1 >>  4) & 0xf;  /* A10 */
            rds_program_current_private->rp_a[10] = (_p1 >>  0) & 0xf;  /* A11 */
            rds_program_current_private->rp_a[11] = (_p2 >> 12) & 0xf;  /* A12 */
            rds_program_current_private->rp_a[12] = (_p2 >>  8) & 0xf;  /* A13 */
            rds_program_current_private->rp_a[13] = (_p2 >>  4) & 0xf;  /* A14 */
            rds_program_current_private->rp_a[14] = (_p2 >>  0) & 0xf;  /* A15 */
            /** \todo process here */
        }
        break;
    case 7:
        /* 15 digit numeric message in international paging */
        rds_program_current_private->rp_type = 4;
        rds_program_current_private->rp_y[0] = (_p1 >> 12) & 0xf;   /* Y1 */
        rds_program_current_private->rp_y[1] = (_p1 >>  8) & 0xf;   /* Y2 */
        rds_program_current_private->rp_z[0] = (_p1 >>  4) & 0xf;   /* Z1 */
        rds_program_current_private->rp_z[1] = (_p1 >>  0) & 0xf;   /* Z2 */
        rds_program_current_private->rp_z[2] = (_p2 >> 12) & 0xf;   /* Z3 */
        rds_program_current_private->rp_z[3] = (_p2 >>  8) & 0xf;   /* Z4 */
        rds_program_current_private->rp_x[0] = (_p2 >>  4) & 0xf;   /* X1 */
        rds_program_current_private->rp_x[1] = (_p2 >>  0) & 0xf;   /* X2 */
        break;
    case 8:
        /* Alphanumeric message */
        rds_program_current_private->rp_type = 3;
        /* Group and individual code */
        rds_program_current_private->rp_y[0] = (_p1 >> 12) & 0xf;   /* Y1 */
        rds_program_current_private->rp_y[1] = (_p1 >>  8) & 0xf;   /* Y2 */
        rds_program_current_private->rp_z[0] = (_p1 >>  4) & 0xf;   /* Z1 */
        rds_program_current_private->rp_z[1] = (_p1 >>  0) & 0xf;   /* Z2 */
        rds_program_current_private->rp_z[2] = (_p2 >> 12) & 0xf;   /* Z3 */
        rds_program_current_private->rp_z[3] = (_p2 >>  8) & 0xf;   /* Z4 */
        rds_program_current_private->rp_x[0] = (_p2 >>  4) & 0xf;   /* X1 (ERP) */
        rds_program_current_private->rp_x[1] = (_p2 >>  0) & 0xf;   /* X2 (ERP) */
        break;
    case 9:
        /* Alphanumeric message */
        if ((rds_program_current_private->rp_type == 3) && (rds_program_current_private->rp_last_sa == 8))
        {
            /* Message characters */
            rds_program_current_private->rp_c[0] = _p1 >> 8;
            rds_program_current_private->rp_c[1] = _p1 & 0xf;
            rds_program_current_private->rp_c[2] = _p2 >> 8;
            rds_program_current_private->rp_c[3] = _p2 & 0xf;
            /** \todo process here (possible for ERP depending on X1X2) */
        }
        break;
    case 10:
        /* Alphanumeric message */
        if ((rds_program_current_private->rp_type == 3) && (rds_program_current_private->rp_last_sa == 9))
        {
            /* Message characters */
            rds_program_current_private->rp_c[4] = _p1 >> 8;
            rds_program_current_private->rp_c[5] = _p1 & 0xf;
            rds_program_current_private->rp_c[6] = _p2 >> 8;
            rds_program_current_private->rp_c[7] = _p2 & 0xf;
        }
        break;
    case 11:
        /* Alphanumeric message */
        if ((rds_program_current_private->rp_type == 3) && (rds_program_current_private->rp_last_sa == 10))
        {
            /* Message characters */
            rds_program_current_private->rp_c[ 8] = _p1 >> 8;
            rds_program_current_private->rp_c[ 9] = _p1 & 0xf;
            rds_program_current_private->rp_c[10] = _p2 >> 8;
            rds_program_current_private->rp_c[11] = _p2 & 0xf;
        }
        break;
    case 12:
        /* Alphanumeric message */
        if ((rds_program_current_private->rp_type == 3) && (rds_program_current_private->rp_last_sa == 11))
        {
            /* Message characters */
            rds_program_current_private->rp_c[12] = _p1 >> 8;
            rds_program_current_private->rp_c[13] = _p1 & 0xf;
            rds_program_current_private->rp_c[14] = _p2 >> 8;
            rds_program_current_private->rp_c[15] = _p2 & 0xf;
        }
        break;
    case 13:
        /* Alphanumeric message */
        if ((rds_program_current_private->rp_type == 3) && (rds_program_current_private->rp_last_sa == 12))
        {
            /* Message characters */
            rds_program_current_private->rp_c[16] = _p1 >> 8;
            rds_program_current_private->rp_c[17] = _p1 & 0xf;
            rds_program_current_private->rp_c[18] = _p2 >> 8;
            rds_program_current_private->rp_c[19] = _p2 & 0xf;
        }
        break;
    case 14:
        /* Alphanumeric message */
        if ((rds_program_current_private->rp_type == 3) && (rds_program_current_private->rp_last_sa == 13))
        {
            /* Message characters */
            rds_program_current_private->rp_c[20] = _p1 >> 8;
            rds_program_current_private->rp_c[21] = _p1 & 0xf;
            rds_program_current_private->rp_c[22] = _p2 >> 8;
            rds_program_current_private->rp_c[23] = _p2 & 0xf;
        }
        break;
    case 15:
        /* Alphanumeric message */
        if ((rds_program_current_private->rp_type == 3) && (rds_program_current_private->rp_last_sa >= 8) && (rds_program_current_private->rp_last_sa <= 14))
        {
            /* End of alphanumeric message: last four or fewer message characters */
            rds_program_current_private->rp_c[(rds_program_current_private->rp_last_sa-8)*4+0] = _p1 >> 8;
            rds_program_current_private->rp_c[(rds_program_current_private->rp_last_sa-8)*4+1] = _p1 & 0xf;
            rds_program_current_private->rp_c[(rds_program_current_private->rp_last_sa-8)*4+2] = _p2 >> 8;
            rds_program_current_private->rp_c[(rds_program_current_private->rp_last_sa-8)*4+3] = _p2 & 0xf;
            /** \todo process here (differently for RP and ERP depending on X1X2) */
        }
        break;
    }
    rds_program_current_private->rp_last_ab = _ab;
    rds_program_current_private->rp_last_sa = _sa;

    rds_decode_status = RDS_DECODE_STATUS_NOT_IMPLEMENTED_YET;
}


/** \brief Process Paging Identification
 *
 * This function processes Paging Identification.
 *
 * \param[in] _id Paging Identification
 */
void rds_rp_decode_ident(/*@unused@*/ uint16_t _id)
{
    rds_decode_status = RDS_DECODE_STATUS_NOT_IMPLEMENTED_YET;
}


/** \brief Process Radio Paging Code
 *
 * This function processes Radio Paging Codes.
 *
 * \param[in] _rpc Ragio Paging Code
 */
void rds_rp_decode_rpc(uint8_t _rpc)
{
    uint8_t pgc;    /* pager group codes */

    /* network group designation */
    switch ((_rpc >> 2) & 0x7)
    {
    case 0:
        /* No basic paging on channel */
        break;
    case 1:
        /* Group codes: 00-99 (100 codes) */
        break;
    case 2:
        /* Group codes: 00-39 (40 codes) */
        break;
    case 3:
        /* Group codes: 40-99 (60 codes) */
        break;
    case 4:
        /* Group codes: 40-69 (30 codes) */
        break;
    case 5:
        /* Group codes: 70-99 (30 codes) */
        break;
    case 6:
        /* Group codes: 00-19 (20 codes) */
        break;
    case 7:
        /* Group codes: 20-39 (20 codes) */
        break;
    }

    /* battery saving interval synchronization and identification */
    switch (_rpc & 0x3)
    {
    }

    rds_decode_status = RDS_DECODE_STATUS_NOT_IMPLEMENTED_YET;
}
