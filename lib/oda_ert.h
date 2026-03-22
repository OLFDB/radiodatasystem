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
 * \file oda_ert.h
 * \brief Enhanced Radio Text functions - headers
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains functions for Enhanced Radio Text handling.
 */

#ifndef ODA_ERT_H
#define ODA_ERT_H

#include <stdint.h>


/**
 * \brief Decodes and handles received eRT information
 *
 * This function decodes and handles received eRT information.
 *
 * \param[in] _bpac eRT byte pair address code
 * \param[in] _y    eRT byte numbers (block 3)
 * \param[in] _z    eRT byte numbers (block 4)
 */
void rds_oda_ert_decode(uint8_t _bpac, uint16_t _y, uint16_t _z);


/**
 * \brief Decodes and handles received eTR assign message
 *
 * This function decodes and handles received eRT assign messages.
 *
 * \param[in] _msg  eRT byte pair address
 */
void rds_oda_ert_decode_assign(uint8_t _msg);


#endif /* ODA_ERT_H */
