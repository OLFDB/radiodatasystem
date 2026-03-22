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
 * \file af.h
 * \brief Alternative Frequency functions - headers
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains functions for Alternative Frequency handling.
 */

#ifndef AF_H
#define AF_H

#include <stdint.h>
#include "rds.h"


/**
 * \brief Number of AF pairs
 *
 * This definition defines the number of AF pairs.
 */
#define rds_afp_max ((rds_af_max/2)-1)


/**
 * \brief Structure for Alternative Frequency filters
 *
 * This structure contains the Alternative Frequency processing data.
 * AF pairs are stored as 16-bit values. Single AFs as 2x8-bit values.
 * An AF list consisting of X pairs needs (X-1) correct combinations.
 * The upper bits are for the AF1, the lower bits are for AF2.
 */
typedef struct rds_af_t
{
    uint16_t rx[3];                /**< combination received of last two and current received AF pairs */
    uint16_t pair[3][rds_afp_max]; /**< list of combinations of 3 AF pairs */
    uint8_t  cnt[rds_afp_max];     /**< counter for each combination */
} rds_af_t;


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
void rds_af_handle(uint8_t *_pub, rds_af_t *_priv, uint8_t _af1, uint8_t _af2);


/**
 * \brief Handles received Alternative Frequencies codes of 0A messages
 *
 * This function handles received Alternative Frequencies of 0A messages.
 *
 * \param[in] _af1 Alternative Frequency 1
 * \param[in] _af2 Alternative Frequency 2
 */
void rds_af_decode(uint8_t _af1, uint8_t _af2);


#endif /* AF_H */
