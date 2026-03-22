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
 * \file oda_irds.h
 * \brief In-Receiver Database System functions - headers
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains functions for In-Receiver Database System handling.
 */

#ifndef ODA_IRDS_H
#define ODA_IRDS_H

#include <stdint.h>


/**
 * \brief Decodes and handles received I-RDS information
 *
 * This function decodes and handles received I-RDS information.
 *
 * \param[in] _ac   Address code
 * \param[in] _y    Block 3 data
 * \param[in] _z    Block 4 data
 */
void rds_oda_irds_decode(uint8_t _ac, uint16_t _y, uint16_t _z);


/**
 * \brief Decode and handles received I-RDS assign message
 *
 * This function decodes and handles received I-RDS assign messages.
 *
 * \param[in] _msg  Message bits
 */
void rds_oda_irds_decode_assign(uint16_t _msg);


#endif /* ODA_IRDS_H */
