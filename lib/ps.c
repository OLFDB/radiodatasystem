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
 * \file ps.c
 * \test All related test cases are defined in \ref ps_test.c
 * \brief Programme Service name functions
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains functions for Programme Service name handling.
 */

#include <stdint.h>
#include <wchar.h>
#include "char_set.h"
#include "ps.h"
#include "rds.h"


/**
 * \brief Decodes and handles received PS characters
 *
 * This function decodes and handles received PS characters.
 *
 * \param[in] _sa PS name Segment Address
 * \param[in] _c1 Text Character
 * \param[in] _c2 Text Character
 */
void rds_ps_decode(uint8_t _sa, uint8_t _c1, uint8_t _c2)
{
    /* process characters */
    rds_program_current->ps[_sa*2+0] = rds_to_wchar[_c1];
    rds_program_current->ps[_sa*2+1] = rds_to_wchar[_c2];
}
