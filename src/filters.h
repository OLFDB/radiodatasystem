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
 * \file filters.h
 * \brief Stream decoding
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This is a library to decode streams in different formats.
 */

#ifndef FILTERS_H
#define FILTERS_H

#include <stdio.h>


int rds_decode_v4l(FILE *fd);
int rds_decode_raw(FILE *fd);
int rds_decode_csv(FILE *fd);
int rds_decode_smp(FILE *fd);


#endif /* FILTERS_H */
