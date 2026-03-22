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
 * \file oda_rasant.h
 * \brief Radio Aided Satellite Navigation Technique functions - headers
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains functions for Radio Aided Satellite Navigation Technique handling.
 */

#ifndef ODA_RASANT_H
#define ODA_RASANT_H

#include <stdint.h>
#include "rds.h"


/**
 * \brief Callback function
 *
 * This variable contains the callback function.
 *
 * \param[in,out] _rs   RDS program
 * \param[in]     _data RTCM data
 */
extern void (*rds_oda_rasant_callback)(rds_program_t *_rs, uint8_t _data);


/** \brief Decode and handle RASANT message
 *
 * This function decodes and handles RASANT messages.
 *
 * \param[in] _x    Block 2 data
 * \param[in] _y    Block 3 data
 * \param[in] _z    Block 4 data
 */
void rds_oda_rasant_decode(uint8_t _x, uint16_t _y, uint16_t _z);


#endif /* ODA_RASANT_H */
