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
 * \file pi.c
 * \test All related test cases are defined in \ref pi_test.c
 * \brief Programme Identification functions
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains functions for Programme Identification handling.
 */

#include <stdint.h>
#include "ecc.h"
#include "pi.h"
#include "rds.h"


/**
 * \brief Returns country code portion of PI code.
 *
 * This function returns the country code portion of the given PI code.
 *
 * \param[in] _pi PI code
 * \return        Country code
 */
uint8_t rds_pi_cc(uint16_t _pi)
{
    return (_pi >> 12);
}


/**
 * \brief Decodes and handles received PI codes
 *
 * This function decodes and handles received PI codes.
 *
 * \param[in] _pi PI code
 */
void rds_pi_decode(uint16_t _pi)
{
    uint8_t cc_old;
    uint8_t ecc_old;

    /* remember old CC */
    cc_old = rds_pi_cc(rds_program_current->pi);
    ecc_old = rds_program_current->ecc;

    /* set new PI code */
    rds_program_current->pi = _pi;

    /* fill in ISO, ITU and CID, if necessary */
    if ((cc_old != rds_pi_cc(_pi)) || (ecc_old != rds_program_current->ecc))
        rds_ecc_get_iso_itu_cid();
}
