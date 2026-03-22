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
 * \file oda.h
 * \brief Open Data Application functions - headers
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains functions for Open Data Application handling.
 */

#ifndef ODA_H
#define ODA_H

#include <stdint.h>


/**
 * \brief Decode and process ODA assignment message
 *
 * This function decodes and processes ODA assignment messages.
 *
 * \param[in] _agtc Application Group Type Code
 * \param[in] _agtv Application Group Type Version
 * \param[in] _msg  Message bits
 * \param[in] _aid  Application Identification
 */
void rds_oda_decode_assign(uint8_t _agtc, uint8_t _agtv, uint16_t _msg, uint16_t _aid);


/** \brief Process ODA type A message
 *
 * This function processes ODA type A messages.
 *
 * \param[in] _gtc  Group Type Version
 * \param[in] _x    Message bits in blk 2
 * \param[in] _y    Message bits in blk 3
 * \param[in] _z    Message bits in blk 4
 */
void rds_oda_decode_a(uint8_t _gtc, uint8_t _x, uint16_t _y, uint16_t _z);


/** \brief Process ODA type B message
 *
 * This function processes ODA type B messages.
 *
 * \param[in] _gtc  Group Type Version
 * \param[in] _x    Message bits in blk 2
 * \param[in] _z    Message bits in blk 4
 */
void rds_oda_decode_b(uint8_t _gtc, uint8_t _x, uint16_t _z);


#endif /* ODA_H */
