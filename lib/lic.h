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
 * \file lic.h
 * \brief Language Identification Code functions - headers
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains functions for Language Identification Code handling.
 */

#ifndef LIC_H
#define LIC_H

#include <stdint.h>


/**
 * \brief Decodes and handles received LI Codes
 *
 * This function decodes and handles received LI Codes.
 *
 * \param[in] _lic  LI Code
 */
void rds_lic_decode(uint8_t _lic);


#endif /* LIC_H */
