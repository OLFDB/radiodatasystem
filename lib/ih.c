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
 * \file ih.c
 * \test All related test cases are defined in \ref ih_test.c
 * \brief In-House application functions
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains functions for In-House application handling.
 */

#include <stdint.h>
#include "ih.h"
#include "rds.h"


/**
 * \brief IH callback function
 *
 * This variable contains the IH callback function.
 *
 * \param[in] _ab A or B message (0=A, 1=B)
 * \param[in] _x  Message bits in blk 2
 * \param[in] _y  Message bits in blk 3
 * \param[in] _z  Message bits in blk 4
 */
void (*rds_ih_callback)(uint8_t _ab, uint8_t _x, uint16_t _y, uint16_t _z);


/**
 * \brief Process IH type A message
 *
 * This function processes IH type A messages.
 *
 * \param[in] _x Message bits in blk 2
 * \param[in] _y Message bits in blk 3
 * \param[in] _z Message bits in blk 4
 */
void rds_ih_decode_a(uint8_t _x, uint16_t _y, uint16_t _z)
{
    /* execute callback function */
    if (*rds_ih_callback != NULL)
        rds_ih_callback(0, _x, _y, _z);
}


/**
 * \brief Process IH type B message
 *
 * This function processes IH type B messages.
 *
 * \param[in] _x Message bits in blk 2
 * \param[in] _z Message bits in blk 4
 */
void rds_ih_decode_b(uint8_t _x, uint16_t _z)
{
    /* execute callback function */
    if (*rds_ih_callback != NULL)
        rds_ih_callback(1, _x, 0, _z);
}
