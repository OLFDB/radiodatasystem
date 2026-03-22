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
 * \file ecc.h
 * \brief Extended Country Code functions - headers
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains functions for Extended Country Code handling.
 */

#ifndef ECC_H
#define ECC_H

#include <stdint.h>


/**
 * \brief Set ISO, ITU and CID based on CC and ECC
 *
 * This function sets ISO, ITU and CID based on CC and ECC.
 */
void rds_ecc_get_iso_itu_cid(void);


/**
 * \brief Decodes and handles received EC Codes
 *
 * This function decodes and handles received EC Codes.
 *
 * \param[in] _ecc  EC Code
 */
void rds_ecc_decode(uint8_t _ecc);


#endif /* ECC_H */
