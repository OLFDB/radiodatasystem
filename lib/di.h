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
 * \file di.h
 * \brief Decoder Identification functions - headers
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains functions for Decoder Identification handling.
 */

#ifndef DI_H
#define DI_H

#include <stdint.h>


/**
 * \brief Decodes and handles received DI codes
 *
 * This function decodes and handles received DI flags.
 *
 * \param[in] _c    DI segment address
 * \param[in] _di   DI segment
 */
void rds_di_decode(uint8_t _c, uint8_t _di);


#endif /* DI_H */
