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
 * \file eon.h
 * \brief Enhanced Other Networks functions - headers
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains functions for Enhanced Other Networks handling.
 */

#ifndef EON_H
#define EON_H

#include <stdint.h>
#include "rds.h"


/**
 * \brief Decodes and handles PI on other network
 *
 * This function decodes and handles PI on other network.
 *
 * \param[in]     _pi   PI code
 */
void rds_eon_decode_pi(uint16_t _pi);


/**
 * \brief Decodes and handles LA on tuned network.
 *
 * This function decodes and handles LA on tuned network.
 *
 * \param _la   Linkage Actuator
 */
void rds_eon_decode_la(uint8_t _la);


/**
 * \brief Handles TP on other network
 *
 * This function handles TP on other network.
 *
 * \param[in,out] _rs       EON RDS program
 * \param[in]     _tp_on    Traffic Programme on Other Network
 */
void rds_eon_decode_tp_on(rds_program_t *_rs, uint8_t _tp_on);


/**
 * \brief Handles TA on other network
 *
 * This function handles TA on other network.
 *
 * \param[in,out] _rs       EON RDS program
 * \param[in]     _ta_on    Traffic Announcement on Other Network
 */
void rds_eon_decode_ta_on(rds_program_t *_rs, uint8_t _ta_on);


/**
 * \brief Decodes and handles received EON message
 *
 * This function decodes and handles received EON messages.
 *
 * \param[in,out] _rs   EON RDS program
 * \param[in]     _vc   Variant Code
 * \param[in]     _i    Information
 */
void rds_eon_decode(rds_program_t *_rs, uint8_t _vc, uint16_t _i);


#endif /* EON_H */
