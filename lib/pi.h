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
 * \file pi.h
 * \brief Programme Identification functions - headers
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains functions for Programme Identification handling.
 */

#ifndef PI_H
#define PI_H

#include <stdint.h>


/**
 * \brief Returns country code portion of PI code.
 *
 * This function returns the country code portion of the given PI code.
 *
 * \param[in] _pi   PI code
 * \return      Country code
 */
uint8_t rds_pi_cc(uint16_t _pi);


/**
 * \brief Decodes and handles received PI codes
 *
 * This function decodes and handles received PI codes.
 *
 * \param[in] _pi   PI code
 */
void rds_pi_decode(uint16_t _pi);


#endif /* PI_H */
