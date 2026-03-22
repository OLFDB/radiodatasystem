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
 * \file ps.h
 * \brief Programme Service name functions - headers
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains functions for Programme Service name handling.
 */

#ifndef PS_H
#define PS_H

#include <stdint.h>


/**
 * \brief Decodes and handles received PS characters
 *
 * This function decodes and handles received PS characters.
 *
 * \param[in] _sa   PS name Segment Address
 * \param[in] _c1   Text Character
 * \param[in] _c2   Text Character
 */
void rds_ps_decode(uint8_t _sa, uint8_t _c1, uint8_t _c2);


#endif /* PS_H */
