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
 * \file tdc.c
 * \test All related test cases are defined in \ref tdc_test.c
 * \brief Transparent Data Channel functions
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains functions for Transparent Data Channel handling.
 */

#include <stdint.h>
#include "rds.h"
#include "tdc.h"


/**
 * \brief TDC callback function
 *
 * This variable contains the TDC callback function.
 *
 * \param[in] _addr address-code
 * \param[in] _data data
 */
void (*rds_tdc_callback)(uint8_t _addr, uint16_t _data);


/**
 * \brief Handles received TDC data
 *
 * This function handles received TDC data.
 *
 * \param[in] _addr address-code
 * \param[in] _data data
 */
void rds_tdc_decode(uint8_t _addr, uint16_t _data)
{
    /* execute callback function */
    if (*rds_tdc_callback != NULL)
        rds_tdc_callback(_addr, _data);
}
