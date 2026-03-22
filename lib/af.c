/*
 * Copyright © 2009-2011 Tobias Lorenz
 *
 * This file is part of librds.
 *
 * librds is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as _published by
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
 * \file af.c
 * \test All related test cases are defined in \ref af_test.c
 * \brief Alternative Frequency functions
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains functions for Alternative Frequency handling.
 */

/* AF codes */
/* 0: Not to be used */
/* 1..204: Carrier frequency */
/* 205: Filler code */
/* 206..223: Not assigned */
/* 224..249: 0..25 AFs follow */
/* 250: An LF/MF frequency follows */
/* 251..255: Not assigned */

/* ITU regions */
/* 1=mostly europe, middle east: LF/MF code table with  9 kHz spacing */
/* 2=mostly america, pacific   : MF    code table with 10 kHz spacing */
/* 3=mostly asia, oceania      : LF/MF code table with  9 kHz spacing */


#include <stdint.h>
#include <string.h>
#include "af.h"
#include "rds_private.h"


/**
 * \brief Handles received Alternative Frequencies codes
 *
 * This function handles received Alternative Frequencies.
 *
 * \param[in,out] _pub  Public AF data
 * \param[in,out] _priv Private AF data
 * \param[in]     _af1  Alternative Frequency 1
 * \param[in]     _af2  Alternative Frequency 2
 */
void rds_af_handle(uint8_t *_pub, rds_af_t *_priv, uint8_t _af1, uint8_t _af2)
{
    uint8_t  idx    = 0;    /* index */
    uint8_t  off    = 0;    /* offset */
    uint8_t  minidx = 0;    /* minimum counter index */
    uint8_t  maxidx = 0;    /* maximum counter index */
    uint8_t  start  = 0;    /* start index */
    uint8_t  next   = 0;    /* next index */

    uint8_t  cnt    = 0;    /* counter value */
    uint8_t  maxcnt = 0;    /* maximum counter value */

    uint8_t  af1;

    /* shift and insert _af1 and _af2 */
    _priv->rx[0] = _priv->rx[1];        /* oldest entry */
    _priv->rx[1] = _priv->rx[2];
    _priv->rx[2] = (_af1 << 8) | _af2;  /* newest entry */

    /* find index for current combination */
    minidx = 0;
    for (idx = 0; (idx < rds_afp_max) && (_priv->cnt[idx] > 0); idx++)
    {
        /* stop searching if the combination was found */
        if ((_priv->rx[0] == _priv->pair[0][idx])
                && (_priv->rx[1] == _priv->pair[1][idx])
                && (_priv->rx[2] == _priv->pair[2][idx]))
            break;

        /* remember entry having minimum cnt */
        if (_priv->cnt[idx] < _priv->cnt[minidx])
            minidx = idx;
    }
    /* now i points to the correct combination or to the first entry entry */

    /* check if the combination was not found */
    if ((_priv->cnt[idx] == 0) || (idx >= rds_afp_max))
    {
        /* check if there is free space left */
        if (idx < rds_afp_max)
        {
            /* use this entry */
        }
        else
        {
            /* check if other entry is outdated */
            if (_priv->cnt[minidx] < 2)
            {
                /* overwrite old entry */
                idx = minidx;
                _priv->cnt[idx] = 0;
            }
            else
            {
                /* better stop now, there are no entries available */
                return;
            }
        }

        /* set entry */
        _priv->pair[0][idx] = _priv->rx[0];
        _priv->pair[1][idx] = _priv->rx[1];
        _priv->pair[2][idx] = _priv->rx[2];
    }

    /* increase the counter for this combination */
    _priv->cnt[idx]++;

    /* check if counter has reached upper limit */
    if (_priv->cnt[idx] >= 32)
    {
        off = 0;

        for (idx = 0; (idx < rds_afp_max) && (_priv->cnt[idx] > 0); idx++)
        {
            /* copy higher entries in */
            if (off > 0)
            {
                _priv->pair[0][idx] = _priv->pair[0][idx+off];
                _priv->pair[1][idx] = _priv->pair[1][idx+off];
                _priv->pair[2][idx] = _priv->pair[2][idx+off];
                _priv->cnt[idx] = _priv->cnt[idx+off];
            }

            /* divide by 2 */
            _priv->cnt[idx] >>= 1;

            /* jump over this now empty entry */
            if (_priv->cnt[idx] == 0)
            {
                off++;
                idx--;  /* process current index again */
            }
        }

        /* reset freed entries */
        while (off-- > 0)
        {
            _priv->pair[0][idx] = 0;
            _priv->pair[1][idx] = 0;
            _priv->pair[2][idx] = 0;
            _priv->cnt[idx] = 0;
            idx++;
        }
    }

    /* now build up the public list, but first clear it */
    memset(_pub, 0, rds_af_max);

    /* start with combination of af1=224..249 and highest counter */
    maxcnt = 0;
    start = 0;
    for (idx = 0; (idx < rds_afp_max) && (_priv->cnt[idx] > 0); idx++)
    {
        af1 = _priv->pair[0][idx] >> 8;

        if ((af1 >= 224) && (af1 <= 249) &&
                (_priv->cnt[idx] > maxcnt))
        {
            maxcnt = _priv->cnt[idx];
            start = idx;
        }
    }

    /* check if start was found */
    af1 = _priv->pair[0][start] >> 8;
    if ((af1 < 224) || (af1 > 249))
        return;

    /* search for following AF pair combinations and highest counters */
    next = start;
    off = 0;
    do
    {
        /* output AF pair */
        *_pub = _priv->pair[0][next] >> 8;
        _pub++;
        cnt++;
        *_pub = _priv->pair[0][next] & 0xff;
        _pub++;
        cnt++;

        /* search for following AF pair and highest counter */
        maxcnt = 0;
        maxidx = next;
        for (idx = 0; (idx < rds_afp_max) && (_priv->cnt[idx] > 0); idx++)
        {
            if ((_priv->pair[1][next] == _priv->pair[0][idx]) &&
                    (_priv->pair[2][next] == _priv->pair[1][idx]) &&
                    (_priv->cnt[idx] > maxcnt))
            {
                maxidx = idx;
                maxcnt = _priv->cnt[maxidx];
            }
        }

        /* check if following pair was found */
        if ((_priv->pair[1][next] != _priv->pair[0][maxidx]) ||
                (_priv->pair[2][next] != _priv->pair[1][maxidx]) ||
                (next == maxidx))
            break;
        next = maxidx;

    }
    while ((start != next) && (cnt < rds_af_max));
}


/**
 * \brief Handles received Alternative Frequencies codes of 0A messages
 *
 * This function handles received Alternative Frequencies of 0A messages.
 *
 * \param[in] _af1 Alternative Frequency 1
 * \param[in] _af2 Alternative Frequency 2
 */
void rds_af_decode(uint8_t _af1, uint8_t _af2)
{
    /* get accees to private data */
    rds_program_private_t *rds_program_current_private =
        (rds_program_private_t *) rds_program_current;

    rds_af_handle(&rds_program_current->af[0], &rds_program_current_private->af, _af1, _af2);
}
