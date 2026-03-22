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
 * \file di.c
 * \test All related test cases are defined in \ref di_test.c
 * \brief Decoder Identification functions
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains functions for Decoder Identification handling.
 */

#include <stdint.h>
#include "di.h"
#include "rds.h"


/**
 * \brief Decodes and handles received DI codes
 *
 * This function decodes and handles received DI flags.
 *
 * \param[in] _c  DI segment address
 * \param[in] _di DI segment
 */
void rds_di_decode(uint8_t _c, uint8_t _di)
{
    switch (_c)
    {
    case 0:
        rds_program_current->di_st = _di;
        break;
    case 1:
        rds_program_current->di_ah = _di;
        break;
    case 2:
        rds_program_current->di_co = _di;
        break;
    case 3:
        rds_program_current->di_dp = _di;
        break;
    }
}
