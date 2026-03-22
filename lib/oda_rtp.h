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
 * \file oda_rtp.h
 * \brief Radio Text Plus functions - headers
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains functions for Radio Text Plus handling.
 */

#ifndef ODA_RTP_H
#define ODA_RTP_H

#include <stdint.h>


/**
 * \brief Return RT+ class string
 *
 * This function returns a RT+ class string.
 *
 * \param[out] _str     Return RT+ class string
 * \param[in]  _size        String size
 * \param[in]  _rtp     RT+ class code
 */
void rds_oda_rtp_get_class(char *_str, size_t _size, uint8_t _rtp);


/**
 * \brief Decodes and handles received RT+ information
 *
 * This function decodes and handles received RT+ information.
 *
 * \param[in] _x    Block 2 data
 * \param[in] _y    Block 3 data
 * \param[in] _z    Block 4 data
 */
void rds_oda_rtp_decode(uint8_t _x, uint16_t _y, uint16_t _z);


/**
 * \brief Decode and handles received RT+ assign message
 *
 * This function decodes and handles received RT+ assign messages.
 *
 * \param[in] _msg  Message bits
 */
void rds_oda_rtp_decode_assign(uint16_t _msg);


#endif /* ODA_RTP_H */
