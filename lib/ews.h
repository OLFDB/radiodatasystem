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
 * \file ews.h
 * \brief Emergency Warning System functions - headers
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains functions for Emergency Warning System handling.
 */

#ifndef EWS_H
#define EWS_H

#include <stdint.h>


/**
 * \brief Decodes and handles received EWS message
 *
 * This function decodes and handles received EWS messages.
 *
 * \param[in] _id   Identification of EWS channel
 */
void rds_ews_decode_ident(uint16_t _id);


/** \brief Process EWS type A message
 *
 * This function processes EWS type A messages.
 *
 * \param[in] _x    Message bits in blk 2
 * \param[in] _y    Message bits in blk 3
 * \param[in] _z    Message bits in blk 4
 */
void rds_ews_decode(uint8_t _x, uint16_t _y, uint16_t _z);


#endif /* EWS_H */
