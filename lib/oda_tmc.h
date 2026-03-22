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
 * \file oda_tmc.h
 * \brief Traffic Message Channel functions - headers
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains functions for Traffic Message Channel handling.
 */

#ifndef ODA_TMC_H
#define ODA_TMC_H

#include <stdint.h>
#include "rds.h"


/**
 * \brief Automatically delete message from message memory
 *
 * This function handles expiry, stop and decrement times.
 */
void rds_oda_tmc_message_memory_autoclean(void);


/**
 * \brief Decoding of TMC message
 *
 * \param[in] _x    Block 2 data
 * \param[in] _y    Block 3 data
 * \param[in] _z    Block 4 data
 */
void rds_oda_tmc_decode(uint8_t _x, uint16_t _y, uint16_t _z);


/**
 * \brief Decoding of ODA assign message information
 *
 * This function decodes the RDS ODA assign message.
 *
 * \param[in] _msg  Message content
 */
void rds_oda_tmc_decode_assign(uint16_t _msg);


#if 0
/**
 * \brief Load TMC data service provider
 *
 * This function loads a TMC DSP from the persistent storage.
 *
 * \param[in] _ecc  ECC Code
 * \param[in] _cc   CC Code
 * \param[in] _sid  SID Code
 * \return      Pointer to TMC DSP structure
 */
rds_oda_tmc_dsp_t *rds_oda_tmc_dsp_load(uint8_t _ecc, uint8_t _cc, uint8_t _sid);


/**
 * \brief Save TMC data service provider
 *
 * This function saves a TMC DSP to persistent storage.
 *
 * \param[in] _dsp  TMC data service provider
 */
void rds_oda_tmc_dsp_save(rds_oda_tmc_dsp_t *_dsp);


/**
 * \brief Save all TMC data service providers
 *
 * This function saves all TMC DSP to persistent storage.
 */
void rds_oda_tmc_dsp_save_all(void);
#endif


#endif /* ODA_TMC_H */
