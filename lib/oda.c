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
 * \file oda.c
 * \test All related test cases are defined in \ref oda_test.c
 * \brief Open Data Application functions
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains functions for Open Data Application handling.
 */

#include <stdint.h>
#include "oda.h"
#include "rds.h"
#include "oda_ert.h"
#ifdef ODA_IRDS
#include "oda_irds.h"
#endif
#ifdef ODA_RASANT
#include "oda_rasant.h"
#endif
#include "oda_rtp.h"
#ifdef ODA_TMC
#include "oda_tmc.h"
#endif


/**
 * \brief Decode and process ODA assignment message
 *
 * This function decodes and processes ODA assignment messages.
 *
 * \param[in] _agtc Application Group Type Code
 * \param[in] _agtv Application Group Type Version
 * \param[in] _msg  Message bits
 * \param[in] _aid  Application Identification
 */
void rds_oda_decode_assign(uint8_t _agtc, uint8_t _agtv, uint16_t _msg, uint16_t _aid)
{
    /* not carried in associated group */
    if ((_agtc == 0) && (_agtv == 0))
        return;

    /* temporary data fault (encoder status) */
    if ((_agtc == 15) && (_agtv == 1))
        return;

    /* assignment */
    if (rds_program_current->oda[_agtc][_agtv] != _aid)
        rds_program_current->oda[_agtc][_agtv] = _aid;

    /* process data */
    switch (_aid)
    {
#ifdef ODA_TMC
    case 0x0d45: /* RDS-TMC: ALERT-C / EN ISO 14819-1 (for testing use, only) */
        rds_oda_tmc_decode_assign(_msg);
        break;
#endif
    case 0x4bd7: /* RadioText+ / RT+ */
        rds_oda_rtp_decode_assign(_msg);
        break;
    case 0x6552: /* Enhanced RadioText / eRT */
        rds_oda_ert_decode_assign(_msg);
        break;
#ifdef ODA_IRDS
    case 0xc563: /* ID Logic */
        rds_oda_irds_decode_assign(_msg);
        break;
#endif
#ifdef ODA_TMC
    case 0xcd46: /* RDS-TMC: ALERT-C / EN ISO 14819-1 (for service use, only) */
    case 0xcd47: /* RDS-TMC: ALERT-C / EN ISO 14819-1 (for service use, only) */
        rds_oda_tmc_decode_assign(_msg);
        break;
#endif
    default:
        rds_decode_status = RDS_DECODE_STATUS_UNKNOWN_AID;
        break;
    }
}


/** \brief Process ODA type A message
 *
 * This function processes ODA type A messages.
 *
 * \param[in] _gtc Group Type Version
 * \param[in] _x   Message bits in blk 2
 * \param[in] _y   Message bits in blk 3
 * \param[in] _z   Message bits in blk 4
 */
void rds_oda_decode_a(uint8_t _gtc, uint8_t _x, uint16_t _y, uint16_t _z)
{
    uint16_t _aid = rds_program_current->oda[_gtc][0];

    switch(_aid)
    {
#ifdef ODA_TMC
    case 0x0d45: /* RDS-TMC: ALERT-C / EN ISO 14819-1 (for testing use, only) */
        rds_oda_tmc_decode(_x, _y, _z);
        break;
#endif
#ifdef ODA_RASANT
    case 0x4aa1: /* RASANT */
        rds_oda_rasant_decode(_x, _y, _z);
        break;
#endif
    case 0x4bd7: /* RadioText+ / RT+ */
        rds_oda_rtp_decode(_x, _y, _z);
        break;
    case 0x6552: /* Enhanced RadioText / eRT */
        rds_oda_ert_decode(_x, _y, _z);
        break;
#ifdef ODA_IRDS
    case 0xc563: /* ID Logic */
        rds_oda_irds_decode(_x, _y, _z);
        break;
#endif
#ifdef ODA_TMC
    case 0xcd46: /* RDS-TMC: ALERT-C / EN ISO 14819-1 (for service use, only) */
    case 0xcd47: /* RDS-TMC: ALERT-C / EN ISO 14819-1 (for service use, only) */
        rds_oda_tmc_decode(_x, _y, _z);
        break;
#endif
    default:
        rds_decode_status = RDS_DECODE_STATUS_UNKNOWN_AID;
        break;
    }
}


/** \brief Process ODA type B message
 *
 * This function processes ODA type B messages.
 *
 * \param[in] _gtc Group Type Version
 * \param[in] _x   Message bits in blk 2
 * \param[in] _z   Message bits in blk 4
 */
void rds_oda_decode_b(uint8_t _gtc, /*@unused@*/ uint8_t _x, /*@unused@*/ uint16_t _z)
{
    uint16_t aid = rds_program_current->oda[_gtc][1];

    /* no supported ODA is using type B messages yet... */
    switch(aid)
    {
    default:
        rds_decode_status = RDS_DECODE_STATUS_UNKNOWN_AID;
        break;
    }
}
