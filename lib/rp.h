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
 * \file rp.h
 * \brief Radio Paging functions - headers
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains functions for Radio Paging handling.
 */

#ifndef RP_H
#define RP_H

#include <stdint.h>


/** \brief Process RP message
 *
 * This function processes RP messages.
 *
 * \param[in] _ab   Paging A/B
 * \param[in] _sa   Paging segment address code
 * \param[in] _p1   Paging data in blk 3
 * \param[in] _p2   Paging data in blk 4
 */
void rds_rp_decode(uint8_t _ab, uint8_t _sa, uint16_t _p1, uint16_t _p2);


/** \brief Process Paging Identification
 *
 * This function processes Paging Identification.
 *
 * \param[in] _id   Paging Identification
 */
void rds_rp_decode_ident(uint16_t _id);


/** \brief Process Radio Paging Code
 *
 * This function processes Radio Paging Codes.
 *
 * \param[in] _rpc  Ragio Paging Code
 */
void rds_rp_decode_rpc(uint8_t _rpc);


#endif /* RP_H */
