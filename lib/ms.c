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
 * \file ms.c
 * \test All related test cases are defined in \ref ms_test.c
 * \brief Music/Speech functions
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains function for Music/Speech handling.
 */

#include <stdint.h>
#include "ms.h"
#include "rds.h"


/**
 * \brief Decodes and handles received MS flags
 *
 * This function decodes and handles received MS flags.
 *
 * \param[in] _ms Music/Speech flag
 */
void rds_ms_decode(uint8_t _ms)
{
    rds_program_current->ms = _ms;
}
