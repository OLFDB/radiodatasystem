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
 * \file eon.c
 * \test All related test cases are defined in \ref eon_test.c
 * \brief Enhanced Other Networks functions
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains functions for Enhanced Other Networks handling.
 */

#include <stdint.h>
#include <string.h>
#include <wchar.h>
#include "af.h"
#include "char_set.h"
#include "eon.h"
#include "rds_private.h"


/**
 * \brief Decodes and handles PI on other network
 *
 * This function decodes and handles PI on other network.
 *
 * \param[in] _pi PI code
 */
void rds_eon_decode_pi(uint16_t _pi)
{
    uint8_t i = 0; /* index */
    uint8_t o = 0; /* offset */
    uint8_t m = 0; /* minimum cnt */

    /* get access to private data */
    rds_program_private_t *rds_program_current_private =
        (rds_program_private_t *) rds_program_current;

    /* check if pi is already in the list */
    for (i = 0; (i < rds_eon_li_max) && (rds_program_current_private->eon_li_cnt[i] > 0); i++)
    {
        /* stop searching if the combination was found */
        if (rds_program_current->eon_li[i].pi == _pi)
            break;

        /* remember entry having minimum cnt */
        if (rds_program_current_private->eon_li_cnt[i] > 0)
            m = i;
    }
    /* now i points to the correct combination or to the first entry entry */

    /* check if the combination was not found */
    if ((i >= rds_eon_li_max) || (rds_program_current_private->eon_li_cnt[i] == 0))
    {
        /* check if there is free space left */
        if (i < rds_eon_li_max)
        {
            /* use this entry */
        }
        else
        {
            /* check if other entry is outdated */
            if (rds_program_current_private->eon_li_cnt[m] < 2)
            {
                /* overwrite old entry */
                i = m;
            }
            else
            {
                /* better stop now, there are no entries available */
                return;
            }
        }

        /* set entry */
        rds_program_current->eon_li[i].pi = _pi;
    }

    /* increase the counter for this combination */
    rds_program_current_private->eon_li_cnt[i]++;

    /* check if counter has reached upper limit */
    o = 0;
    if (rds_program_current_private->eon_li_cnt[i] >= 32)
    {
        for (i = 0; (i < rds_eon_li_max) && (rds_program_current_private->eon_li_cnt[i] > 0); i++)
        {
            /* copy higher entries in */
            if (o > 0)
            {
                memcpy(&rds_program_current->eon_li[i], &rds_program_current->eon_li[i+o], sizeof(rds_eon_li_t));
                rds_program_current_private->eon_li_cnt[i] = rds_program_current_private->eon_li_cnt[i+o];
            }

            /* divide by 2 */
            rds_program_current_private->eon_li_cnt[i] >>= 1;

            /* jump over this now empty entry */
            if (rds_program_current_private->eon_li_cnt[i] == 0)
            {
                o++;
                i--;    /* process current index again */
            }
        }

        /* reset freed entries */
        while (o-- > 0)
        {
            rds_program_current->eon_li[i].pi = 0;
            rds_program_current_private->eon_li_cnt[i++] = 0;
        }
    }
}


/**
 * \brief Decodes and handles PS on other network
 *
 * This function decodes and handles PS on other network.
 *
 * \param[in,out] _rs EON RDS program
 * \param[in]     _sa Segment address
 * \param[in]     _c1 Text character 1
 * \param[in]     _c2 Text character 2
 */
static void rds_eon_decode_ps_on(rds_program_t *_rs, uint8_t _sa, uint8_t _c1, uint8_t _c2)
{
    /* process characters */
    _rs->ps[_sa+0] = rds_to_wchar[_c1];
    _rs->ps[_sa+1] = rds_to_wchar[_c2];
}


/**
 * \brief Handles mappings of tuned network to FM mapped frequency
 *
 * This function handles mappings of tuned network to FM mapped frequencies.
 *
 * \param[in,out] _rs    EON RDS program
 * \param[in]     _sa    Segment address
 * \param[in]     _tn    Tuned Network
 * \param[in]     _mf_on Mapped FM frequency on Other Network
 */
static void rds_eon_decode_tn_fm(rds_program_t *_rs, uint8_t _sa, uint8_t _tn, uint8_t _mf_on)
{
    _rs->eon_mf[_sa].tn = _tn;
    _rs->eon_mf[_sa].mf = _mf_on;
}


/**
 * \brief Decodes and handles LA on tuned network.
 *
 * This function decodes and handles LA on tuned network.
 *
 * \param[in] _la Linkage Actuator
 */
void rds_eon_decode_la(uint8_t _la)
{
    /* set general linkage actuator */
    rds_program_current->la = _la;

    /* if generic linkage actuator is cleared, also clear all others */
    if (_la == 0)
    {
        uint8_t i;

        for (i = 0; i < rds_eon_li_max; i++)
            rds_program_current->eon_li[i].la = 0;
    }
}


/**
 * \brief Handles Linkage Information
 *
 * This function decodes and handles received Linkage Information.
 *
 * \param[in,out] _rs  EON RDS program
 * \param[in]     _la  Linkage Actuator
 * \param[in]     _eg  Extended Generic indicator
 * \param[in]     _ils International Linkage Set indicator
 * \param[in]     _lsn Linkage Set Number
 */
static void rds_eon_decode_li(rds_program_t *_rs, uint8_t _la, uint8_t _eg, uint8_t _ils, uint16_t _lsn)
{
    uint8_t i;

    /* search entry */
    for (i = 0; i < rds_eon_li_max; i++)
        if (rds_program_current->eon_li[i].pi == _rs->pi)
            break;

    /* save information */
    rds_program_current->eon_li[i].la = _la;
    rds_program_current->eon_li[i].eg = _eg;
    rds_program_current->eon_li[i].ils = _ils;
    rds_program_current->eon_li[i].lsn = _lsn;

    /* activate general linkage actuator */
    if (_la == 1)
    {
        rds_program_current->la = _la;
    }
    else
    {
        /* if all linkage actuators are cleared, also clear generic */
        uint8_t la_cnt = 0;
        for (i = 0; i < rds_eon_li_max; i++)
            la_cnt += rds_program_current->eon_li[i].la;
        if (la_cnt == 0)
            rds_program_current->la = 0;
    }
}


/**
 * \brief Handles PTY on other network
 *
 * This function handles PTY on other network.
 *
 * \param[in,out] _rs     EON RDS program
 * \param[in]     _pty_on Programme Type on Other Network
 */
static void rds_eon_decode_pty_on(rds_program_t *_rs, uint8_t _pty_on)
{
    _rs->pty = _pty_on;
}


/**
 * \brief Handles PIN on other network
 *
 * This function handles PIN on other network.
 *
 * \param[in,out] _rs     EON RDS program
 * \param[in]     _day    Day
 * \param[in]     _hour   Hour
 * \param[in]     _minute Minute
 */
static void rds_eon_decode_pin_on(rds_program_t *_rs, uint8_t _day, uint8_t _hour, uint8_t _minute)
{
    /* ignore the rest of the message */
    if (_day == 0)
        return;

    /* handle components */
    _rs->pin.day = _day;
    _rs->pin.hour = _hour;
    _rs->pin.minute = _minute;
}


/**
 * \brief Handles TP on other network
 *
 * This function handles TP on other network.
 *
 * \param[in,out] _rs    EON RDS program
 * \param[in]     _tp_on Traffic Programme on Other Network
 */
void rds_eon_decode_tp_on(rds_program_t *_rs, uint8_t _tp_on)
{
    _rs->tp = _tp_on;
}


/**
 * \brief Handles TA on other network
 *
 * This function handles TA on other network.
 *
 * \param[in,out] _rs    EON RDS program
 * \param[in]     _ta_on Traffic Announcement on Other Network
 */
void rds_eon_decode_ta_on(rds_program_t *_rs, uint8_t _ta_on)
{
    _rs->ta = _ta_on;
}


/**
 * \brief Decodes and handles received EON message
 *
 * This function decodes and handles received EON messages.
 *
 * \param[in,out] _rs EON RDS program
 * \param[in]     _vc Variant Code
 * \param[in]     _i  Information
 */
void rds_eon_decode(rds_program_t *_rs, uint8_t _vc, uint16_t _i)
{
    /* get access to private data */
    rds_program_private_t *rsp =
        (rds_program_private_t *) _rs;

    switch (_vc)   /* Variant Code */
    {
    case 0:
    case 1:
    case 2:
    case 3: /* char. x, char. x+1 */
        rds_eon_decode_ps_on(_rs, 2 * (_vc & 0xf), _i >> 8, _i & 0xff);
        break;
    case 4: /* AF(ON), AF(ON)*/
        rds_af_handle(&_rs->af[0], &rsp->af, _i >> 8, _i & 0xff);
        break;
    case 5: /* Tuning freq. (TN), Mapped FM freq. 1 (ON) */
    case 6: /* Tuning freq. (TN), Mapped FM freq. 2 (ON) */
    case 7: /* Tuning freq. (TN), Mapped FM freq. 3 (ON) */
    case 8: /* Tuning freq. (TN), Mapped FM freq. 4 (ON) */
    case 9: /* Tuning freq. (TN), Mapped AM freq. (ON) */
        rds_eon_decode_tn_fm(_rs, _vc - 5, _i >> 8, _i & 0xff);
        break;
    case 10:
    case 11: /* variant 10+11 unallocated */
        rds_decode_status = RDS_DECODE_STATUS_NOT_ASSIGNED;
        break;
    case 12: /* Linkage information */
        rds_eon_decode_li(_rs,
                          (_i >> 15) & 1,   /* linkage actuator */
                          (_i >> 14) & 1,   /* extended generic indicator */
                          (_i >> 12) & 1,   /* international linkage set (ILS) indicator */
                          (_i & 0x0fff));   /* linkage set number (LSN) */
        if (((_i >> 13) & 1) != 0) /* reserved */
            rds_decode_status = RDS_DECODE_STATUS_NOT_ASSIGNED;
        break;
    case 13: /* PTY(ON), Reserved, TA(ON) */
        rds_eon_decode_pty_on(_rs, (_i >> 11) & 0x1f);
        rds_eon_decode_ta_on(_rs, _i & 1);
        break;
    case 14: /* PIN(ON) */
        rds_eon_decode_pin_on(_rs, (_i >> 11) & 0x1f, (_i >> 6) & 0x1f, (_i >> 0) & 0x2f);
        break;
    case 15:/* Reserved for broadcasters use */
        rds_decode_status = RDS_DECODE_STATUS_FOR_BROADCASTERS_USE;
        break;
    }
}
