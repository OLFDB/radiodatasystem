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
 * \file ta.h
 * \brief Traffic Announcement functions - headers
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains functions for Traffic Announcement handling.
 */

#ifndef TA_H
#define TA_H

#include <stdint.h>


/**
 * \brief Decodes and handles received TA flags
 *
 * This function decodes and handles received TA flags.
 *
 * \param[in] _ta   TA flag
 */
void rds_ta_decode(uint8_t _ta);


#endif /* TA_H */
