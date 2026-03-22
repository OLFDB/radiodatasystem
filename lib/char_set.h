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
 * \file char_set.h
 * \brief Character set table - headers
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains the character set table.
 */

#ifndef CHAR_SET_H
#define CHAR_SET_H

#include <wchar.h>


/**
 * \brief Conversion table
 *
 * This variable contains the conversion table from RDS to wchar.
 * All unknown RDS characters are mapped to the space character.
 */
extern const wchar_t rds_to_wchar[256];


#endif /* CHAR_SET_H */
