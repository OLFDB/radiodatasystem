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
 * \file ptyn.h
 * \brief Programme Type Name functions - headers
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains functions for Programme Type Name handling.
 */

#ifndef PTYN_H
#define PTYN_H

#include <stdint.h>


/**
 * \brief Decodes and handles received PTYN characters
 *
 * This function decodes and handles received PTYN characters.
 *
 * \param[in] _ab   A/B flag
 * \param[in] _sa   PTYN Segment Address
 * \param[in] _c1   Text Character
 * \param[in] _c2   Text Character
 * \param[in] _c3   Text Character
 * \param[in] _c4   Text Character
 */
void rds_ptyn_decode(uint8_t _ab, uint8_t _sa, uint8_t _c1, uint8_t _c2, uint8_t _c3, uint8_t _c4);


#endif /* PTYN_H */
