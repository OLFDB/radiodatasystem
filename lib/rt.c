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
 * \file rt.c
 * \test All related test cases are defined in \ref rt_test.c
 * \brief Radio Text functions
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains functions for Radio Text handling.
 */

#include <stdint.h>
#include <string.h>
#include <wchar.h>
#include "char_set.h"
#include "rds_private.h"
#include "rt.h"


/**
 * \brief Decodes and handles received RT characters
 *
 * This function decodes and handles received RT characters.
 * \param[in] _ab Text A/B flag
 * \param[in] _sa Text segment address code
 * \param[in] _c1 Text character 1
 * \param[in] _c2 Text character 2
 * \param[in] _c3 Text character 3
 * \param[in] _c4 Text character 4
 */
void rds_rt_decode_a(uint8_t _ab, uint8_t _sa, uint8_t _c1, uint8_t _c2, uint8_t _c3, uint8_t _c4)
{
    /* get access to private data */
    rds_program_private_t *rds_program_current_private =
        (rds_program_private_t *) rds_program_current;

    /* clear on A/B flag change */
    if (rds_program_current_private->rt_last_ab != _ab)
    {
        memset(&rds_program_current->rt, 0, sizeof(rds_program_current->rt));
        rds_program_current_private->rt_last_ab = _ab;
    }

    /* process characters */
    rds_program_current->rt[_sa*4+0] = rds_to_wchar[_c1];
    rds_program_current->rt[_sa*4+1] = rds_to_wchar[_c2];
    rds_program_current->rt[_sa*4+2] = rds_to_wchar[_c3];
    rds_program_current->rt[_sa*4+3] = rds_to_wchar[_c4];
}


/**
 * \brief Decodes and handles received RT characters
 *
 * This function decodes and handles received RT characters.
 * \param[in] _ab Text A/B flag
 * \param[in] _sa Text segment address code
 * \param[in] _c1 Text character 1
 * \param[in] _c2 Text character 2
 */
void rds_rt_decode_b(uint8_t _ab, uint8_t _sa, uint8_t _c1, uint8_t _c2)
{
    /* get access to private data */
    rds_program_private_t *rds_program_current_private =
        (rds_program_private_t *) rds_program_current;

    /* clear on A/B flag change */
    if (rds_program_current_private->rt_last_ab != _ab)
    {
        memset(&rds_program_current->rt, 0, sizeof(rds_program_current->rt));
        rds_program_current_private->rt_last_ab = _ab;
    }

    /* process characters */
    rds_program_current->rt[_sa*2+0] = rds_to_wchar[_c1];
    rds_program_current->rt[_sa*2+1] = rds_to_wchar[_c2];
}
