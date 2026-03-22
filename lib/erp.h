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
 * \file erp.h
 * \brief Enhanced Radio Paging functions - headers
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains functions for Enhanced Radio Paging handling.
 */

#ifndef ERP_H
#define ERP_H

#include <stdint.h>


/**
 * \brief Decodes and handles received ERP information
 *
 * This function decodes and handles received ERP information.
 *
 * \param[in] _sty  Sub type
 * \param[in] _cs   Cycle Selection
 * \param[in] _blk3 Information field from blk3
 * \param[in] _blk4 Information field from blk4
 */
void rds_erp_decode(uint8_t _sty, uint8_t _cs, uint16_t _blk3, uint16_t _blk4);


/**
 * \brief Process Enhanced Paging Operator Code
 *
 * This function processes Enhanced Paging Operator Code.
 *
 * \param[in] _opc  Operator Code
 */
void rds_erp_decode_opc(uint8_t _opc);


/**
 * \brief Process Enhanced Paging Paging Area Code
 *
 * This function processes Enhanced Paging Paging Area Code.
 *
 * \param[in] _pac  Paging Area Code
 */
void rds_erp_decode_pac(uint8_t _pac);


/**
 * \brief Process Enhanced Paging Current Carrier Frequency
 *
 * This function processes Enhanced Paging Current Carrier Frequency.
 *
 * \param[in] _ccf  Current Carrier Frequency
 */
void rds_erp_decode_ccf(uint8_t _ccf);


#endif /* ERP_H */
