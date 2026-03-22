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
 * \file print.h
 * \brief Prints RDS data in a user friendly way.
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This is a library to print RDS data in a user friendly way.
 */

#include <stdint.h>
#include <wchar.h>

#ifndef PRINT_H
#define PRINT_H


void wchar_print(wchar_t *buf, size_t n);

void af_print(void);
void ct_print(void);
void di_st_print(void);
void di_ah_print(void);
void di_co_print(void);
void di_dp_print(void);
void ecc_print(void);
void eon_print(void);
void eon_tn_mf_print(void);
void ert_print(void);
void ih_print(uint8_t _ab, uint8_t _x, uint16_t _y, uint16_t _z);
void lic_print(void);
void ms_print(void);
void oda_print(uint8_t agtc, uint8_t agtv, uint16_t aid);
void pi_print(void);
void pin_print(void);
void ps_print(void);
void pty_print(void);
void ptyn_print(void);
void rt_print(void);
void rtp_print(uint8_t rtp);
void tdc_print(uint8_t _addr, uint16_t _data);
void tp_ta_print(void);

#endif /* FILTERS_H */
