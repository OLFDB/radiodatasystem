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
 * \file oda_rasant.c
 * \test All related test cases are defined in \ref oda_rasant_test.c
 * \brief Radio Aided Satellite Navigation Technique functions
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains functions for Radio Aided Satellite Navigation Technique handling.
 */

/* A suitable output for this module would be RTCM 2.0 stream for navigation equipment. */

/*
 * MZC      Modified Z-Count
 * T        Time Mark                       odd reference minute (GPS time), wechselt xx:xx:49 UTC
 * UDRE     User Differential Range Error (One Sigma Differential Error)
 *          0: <1m
 *          1: >1m and <= 4m
 *          2: >4m and <= 8m
 *          3: >8m
 * SATID    Satellite PRN                   PRN satellite number 0..31
 * PRC      Pseudorange Correction          [mm]     (signed) (s defines scale factor)
 * SF       Scale Factor (for PRC+RRC)
 *          0: range *  20, rangerate *  2
 *          1: range * 320, rangerate * 32
 *          [m] [m from]    [m to]      [m/s]   [m/s from]  [m/s to]    SF
 *          0.02    -655.34     +655.34     0.002   -0.254      +0.254      0
 *          0.32    -10485.44   +10485.44   0.032   -4.064      +4.064      1
 * RRC      Pseudorange Correction Rate     [mm/sec] (signed) (s defines scale factor)
 * IODE     Satellite Ephemeris             Issue Of Data for PRN, new value
                                            Issue of Data for PRN, matching the IOD for the current ephemeris of this satellite, as transmitted by the satellite.
                                            (90sec transistion period)
 * rsc      reference station coordinates [cm] (signed 32 bit number)
 */

#include <stdint.h>
#include <stdio.h>
#include "rds.h"
#include "oda_rasant.h"


static uint16_t mzc;      /**< 13-bit modified z-count */
static uint32_t rsc[3];   /**< 32-bit reference station coordinates [x, y, z], signed */
static uint16_t rsid;     /**< 10-bit reference station ID */
static uint8_t  rsh;      /**<  3-bit health */

/* save all satellites */
static uint8_t  sf[32];   /**<  1-bit scale factor */
static uint8_t  udre[32]; /**<  2-bit user differential range error */
static uint16_t prc[32];  /**< 16-bit pseudorange correction, signed */
static uint8_t  rrc[32];  /**<  8-bit pseudorange correction rate, signed */
static uint8_t  iod[32];  /**<  8-bit issue of date ephemeris */

static uint8_t  prn;      /**<  5-bit satellite number */


/**
 * \brief Callback function
 *
 * This variable contains the callback function.
 *
 * \param[in,out] _rs   RDS Station
 * \param[in]     _data RTCM data
 */
void (*rds_oda_rasant_callback)(rds_program_t *_rs, uint8_t _data);


/**
 * \brief Parity function
 *
 * This functions return the parity information for the given data.
 *
 * \param[in] _th Data
 * \return        Parity information
 */
static uint8_t isgps_parity(uint32_t _th)
{
    uint32_t t;
    uint8_t p;
    const uint8_t parity_array[] =
    {
        0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
        1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
        1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
        0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
        1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
        0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
        0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
        1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
        1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
        0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
        0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
        1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
        0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
        1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
        1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
        0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0
    };
    const uint32_t PARITY_25 = (uint32_t) 0xbb1f3480;
    const uint32_t PARITY_26 = (uint32_t) 0x5d8f9a40;
    const uint32_t PARITY_27 = (uint32_t) 0xaec7cd00;
    const uint32_t PARITY_28 = (uint32_t) 0x5763e680;
    const uint32_t PARITY_29 = (uint32_t) 0x6bb1f340;
    const uint32_t PARITY_30 = (uint32_t) 0x8b7a89c0;

    t = _th & PARITY_25;
    p = parity_array[t & 0xff] ^ parity_array[(t >> 8) & 0xff] ^
        parity_array[(t >> 16) & 0xff] ^ parity_array[(t >> 24) & 0xff];
    t = _th & PARITY_26;
    p = (p << 1) | (parity_array[t & 0xff] ^ parity_array[(t >> 8) & 0xff] ^
                    parity_array[(t >> 16) & 0xff] ^ parity_array[(t >> 24) & 0xff]);
    t = _th & PARITY_27;
    p = (p << 1) | (parity_array[t & 0xff] ^ parity_array[(t >> 8) & 0xff] ^
                    parity_array[(t >> 16) & 0xff] ^ parity_array[(t >> 24) & 0xff]);
    t = _th & PARITY_28;
    p = (p << 1) | (parity_array[t & 0xff] ^ parity_array[(t >> 8) & 0xff] ^
                    parity_array[(t >> 16) & 0xff] ^ parity_array[(t >> 24) & 0xff]);
    t = _th & PARITY_29;
    p = (p << 1) | (parity_array[t & 0xff] ^ parity_array[(t >> 8) & 0xff] ^
                    parity_array[(t >> 16) & 0xff] ^ parity_array[(t >> 24) & 0xff]);
    t = _th & PARITY_30;
    p = (p << 1) | (parity_array[t & 0xff] ^ parity_array[(t >> 8) & 0xff] ^
                    parity_array[(t >> 16) & 0xff] ^ parity_array[(t >> 24) & 0xff]);

    return (p);
}


/**
 * \brief RTCM stream generation function
 *
 * This functions generated the RTCM stream based on RTCM blocks.
 *
 * \param[in] len Length
 * \param[in] ip  Pointer to RTCM block
 */
static void rtcm_stream(uint8_t len, uint32_t *ip)
{
    static uint32_t w;
    const uint8_t reverse_bits[] =
    {
        0, 32, 16, 48, 8, 40, 24, 56, 4, 36, 20, 52, 12, 44, 28, 60,
        2, 34, 18, 50, 10, 42, 26, 58, 6, 38, 22, 54, 14, 46, 30, 62,
        1, 33, 17, 49, 9, 41, 25, 57, 5, 37, 21, 53, 13, 45, 29, 61,
        3, 35, 19, 51, 11, 43, 27, 59, 7, 39, 23, 55, 15, 47, 31, 63
    };
    const uint32_t P_30_MASK = (uint32_t) 0x40000000;
    const uint32_t W_DATA_MASK = (uint32_t) 0x3fffffc0;
    const uint8_t MAG_SHIFT = 6;
    const uint8_t MAG_TAG_DATA = (uint8_t) (1 << MAG_SHIFT);
//  const uint8_t MAG_TAG_MASK = (uint8_t) (3 << MAG_SHIFT);

    if (rds_oda_rasant_callback == NULL)
        return;

    while (len-- > 0)
    {
        w <<= 30;
        w |= *ip++ & W_DATA_MASK;
        w |= isgps_parity(w);

        /* weird-assed inversion */
        if ((w & P_30_MASK) == P_30_MASK)
            w ^= W_DATA_MASK;

        /*
         * Write each 30-bit IS-GPS-200 data word as 5 Magnavox-format bytes
         * with data in the low 6-bits of the byte.  MSB first.
         */
        rds_oda_rasant_callback(rds_program_current, MAG_TAG_DATA | reverse_bits[(w >> 24) & 0x3f]);
        rds_oda_rasant_callback(rds_program_current, MAG_TAG_DATA | reverse_bits[(w >> 18) & 0x3f]);
        rds_oda_rasant_callback(rds_program_current, MAG_TAG_DATA | reverse_bits[(w >> 12) & 0x3f]);
        rds_oda_rasant_callback(rds_program_current, MAG_TAG_DATA | reverse_bits[(w >>  6) & 0x3f]);
        rds_oda_rasant_callback(rds_program_current, MAG_TAG_DATA | reverse_bits[ w        & 0x3f]);
    }
}


/**
 * \brief RTCM message
 *
 * This variable contains the RTCM message content.
 */
static uint32_t rtcm[8];


/**
 * \brief Sequence number
 *
 * This variable contains the RTCM sequence number.
 */
static uint32_t sqnum;


/**
 * \brief Differential GPS Corrections
 *
 * This function generates RTCM msg1 blocks.
 */
/*@unused@*/
static void rtcm2_msg1(void)
{
    /* RTCM Header */
    rtcm[0] = (uint32_t)
              (0x66 << 22) |              /*  8-bit preamble */
              (1 << 16) |                 /*  6-bit message type */
              (rsid << 6);                /* 10-bit station ID */
    rtcm[1] = (uint32_t)
              (mzc << 17) |               /* 13-bit modified z-count */
              (sqnum << 14) |             /*  3-bit sequence no. */
              (7 << 9) |                  /*  5-bit length of frame */
              (rsh << 3);                 /*  3-bit station health */
    sqnum = (sqnum + 1) % 8;

    /* RTCM Content */
    rtcm[2] = (uint32_t)
              (sf[prn] << 29) |           /*  1-bit scale factor */
              (udre[prn] << 27) |         /*  2-bit UDRE */
              (prn << 22) |               /*  5-bit satellite ID */
              (prc[prn] << 6);            /* 16-bit pseudorange correction */
    rtcm[3] = (uint32_t)
              (rrc[prn] << 22) |          /*  8-bit range-rate correction */
              (iod[prn] << 14) |          /*  8-bit issue of data */
              (sf[prn] << 13) |           /*  1-bit scale factor */
              (udre[prn] << 11) |         /*  2-bit UDRE */
              (prn << 6);                 /*  6-bit satellite ID */
    rtcm[4] = (uint32_t)
              (prc[prn] << 14) |          /* 16-bit pseudorange correction */
              (rrc[prn] << 6);            /*  8-bit range-rate correction */
    rtcm[5] = (uint32_t)
              (iod[prn] << 22) |          /*  8-bit issue of date */
              (sf[prn] << 21) |           /*  1-bit scale factor */
              (udre[prn] << 19) |         /*  2-bit UDRE */
              (prn << 14) |               /*  5-bit satellite ID */
              ((prc[prn] >> 8) << 6);     /*  8-bit pseudorange correction (h) */
    rtcm[6] = (uint32_t)
              ((prc[prn] & 0xff) << 22) | /*  8-bit pseudorange correction (l) */
              (rrc[prn] << 14) |          /*  8-bit range-rate correction */
              (iod[prn] << 6);            /*  8-bit issue of date */

    /* complete with all satellites */

    rtcm_stream(7, (uint32_t *) &rtcm);
}


/**
 * \brief Delta Differential GPS Corrections
 *
 * This function generates RTCM msg2 blocks.
 */
/*@unused@*/ static void rtcm2_msg2(void)
{
    /* RTCM Header */
    rtcm[0] = (uint32_t)
              (0x66 << 22) |              /*  8-bit preamble */
              (2 << 16) |                 /*  6-bit message type */
              (rsid << 6);                /* 10-bit station ID */
    rtcm[1] = (uint32_t)
              (mzc << 17) |               /* 13-bit modified z-count */
              (sqnum << 14) |             /*  3-bit sequence no. */
              (7 << 9) |                  /*  5-bit length of frame */
              (rsh << 3);                 /*  3-bit station health */
    sqnum = (sqnum + 1) % 8;

    /* RTCM Content */
    rtcm[2] = (uint32_t)
              (sf[prn] << 29) |           /*  1-bit scale factor */
              (udre[prn] << 27) |         /*  2-bit UDRE */
              (prn << 22) |               /*  5-bit satellite ID */
              (prc[prn] << 6);            /* 16-bit pseudorange correction */
    rtcm[3] = (uint32_t)
              (rrc[prn] << 22) |          /*  8-bit range-rate correction */
              (iod[prn] << 14) |          /*  8-bit issue of data */
              (sf[prn] << 13) |           /*  1-bit scale factor */
              (udre[prn] << 11) |         /*  2-bit UDRE */
              (prn << 6);                 /*  6-bit satellite ID */
    rtcm[4] = (uint32_t)
              (prc[prn] << 14) |          /* 16-bit pseudorange correction */
              (rrc[prn] << 6);            /*  8-bit range-rate correction */
    rtcm[5] = (uint32_t)
              (iod[prn] << 22) |          /*  8-bit issue of date */
              (sf[prn] << 21) |           /*  1-bit scale factor */
              (udre[prn] << 19) |         /*  2-bit UDRE */
              (prn << 14) |               /*  5-bit satellite ID */
              ((prc[prn] >> 8) << 6);     /*  8-bit pseudorange correction (h) */
    rtcm[6] = (uint32_t)
              ((prc[prn] & 0xff) << 22) | /*  8-bit pseudorange correction (l) */
              (rrc[prn] << 14) |          /*  8-bit range-rate correction */
              (iod[prn] << 6);            /*  8-bit issue of date */

    rtcm_stream(7, (uint32_t *) &rtcm);
}


/**
 * \brief Reference Station Parameters
 *
 * This function generates RTCM msg3 blocks.
 */
static void rtcm2_msg3(void)
{
    /* RTCM Header */
    rtcm[0] = (uint32_t)
              (0x66 << 22) |              /*  8-bit preamble */
              (3 << 16) |                 /*  6-bit message type */
              (rsid << 6);                /* 10-bit station ID */
    rtcm[1] = (uint32_t)
              (mzc << 17) |               /* 13-bit modified z-count */
              (sqnum << 14) |             /*  3-bit sequence no. */
              (6 << 9) |                  /*  5-bit length of frame */
              (rsh << 3);                 /*  3-bit station health */
    sqnum = (sqnum + 1) % 8;

    /* RTCM Content */
    rtcm[2] = (uint32_t)
              ((rsc[0] >> 8) << 6);       /* 24-bit x-coordinate (h) */
    rtcm[3] = (uint32_t)
              ((rsc[0] & 0xff) << 22) |   /*  8-bit x-coordinate (l) */
              ((rsc[1] >> 16) << 6);      /* 16-bit y-coordinate (h) */
    rtcm[4] = (uint32_t)
              ((rsc[1] & 0xffff) << 14) | /* 16-bit y-coordinate (l) */
              ((rsc[2] >> 24) << 6);      /*  8-bit z-coordinate (h) */
    rtcm[5] = (uint32_t)
              ((rsc[2] & 0xffffff) << 6); /* 24-bit z-coordinate (l) */

    rtcm_stream(6, (uint32_t *) &rtcm);
}


/**
 * \brief Reference Station Datum
 *
 * This function generates RTCM msg4 blocks.
 */
/** \todo not necessary? */
/*@unused@*/ static void rtcm2_msg4(void)
{
    /* RTCM Header */
    rtcm[0] = (uint32_t)
              (0x66 << 22) |              /*  8-bit preamble */
              (3 << 16) |                 /*  6-bit message type */
              (rsid << 6);                /* 10-bit station ID */
    rtcm[1] = (uint32_t)
              (mzc << 17) |               /* 13-bit modified z-count */
              (sqnum << 14) |             /*  3-bit sequence no. */
              (6 << 9) |                  /*  5-bit length of frame */
              (rsh << 3);                 /*  3-bit station health */
    sqnum = (sqnum + 1) % 8;

    /* RTCM Content */
    rtcm[2] = (uint32_t)                  /** \todo check if this is correct */
              (0 << 27) |                 /*  3-bit dgnss */
              (0 << 26) |                 /*  1-bit dat */
              (0 << 22) |                 /*  4-bit spare */
              (0 << 14) |                 /*  8-bit datum alpha char 1 */
              (0 << 6);                   /*  8-bit datum alpha char 2 */
    rtcm[3] = (uint32_t)                  /** \todo check if this is correct */
              (0 << 22) |                 /*  8-bit datum sub div char 3 */
              (0 << 14) |                 /*  8-bit datum sub div char 1 */
              (0 << 6);                   /*  8-bit datum sub div char 2 */
    rtcm[4] = (uint32_t)                  /** \todo check if this is correct */
              (0 << 14) |                 /* 16-bit dx */
              (0 << 6);                   /*  8-bit dy (h) */
    rtcm[5] = (uint32_t)                  /** \todo check if this is correct */
              (0 << 22) |                 /*  8-bit dy (l) */
              (0 << 6);                   /* 16-bit dz */

    rtcm_stream(6, (uint32_t *) &rtcm);
}


/**
 * \brief Constellation Health
 *
 * This function generates RTCM msg5 blocks.
 */
/** \todo not necessary? */
static void rtcm2_msg5(void)
{
    /* RTCM Header */
    rtcm[0] = (uint32_t)
              (0x66 << 22) |              /*  8-bit preamble */
              (5 << 16) |                 /*  6-bit message type */
              (rsid << 6);                /* 10-bit station ID */
    rtcm[1] = (uint32_t)
              (mzc << 17) |               /* 13-bit modified z-count */
              (sqnum << 14) |             /*  3-bit sequence no. */
              (3 << 9) |                  /*  5-bit length of frame */
              (rsh << 3);                 /*  3-bit station health */
    sqnum = (sqnum + 1) % 8;

    /* RTCM Content */
    rtcm[2] = (uint32_t)                  /** \todo check if this is correct */
              (0 << 29) |                 /*  1-bit reserved */
              (0 << 24) |                 /*  5-bit PRN */
              (0 << 23) |                 /*  1-bit IOD1 */
              (0 << 20) |                 /*  3-bit health */
              (0 << 15) |                 /*  5-bit carrier/noise ratio (+24dB if >0) */
              (0 << 14) |                 /*  1-bit health enable */
              (0 << 13) |                 /*  1-bit new nav data soon */
              (0 << 12) |                 /*  1-bit loss of warning */
              (0 << 8) |                  /*  4-bit time to unhealth */
              (0 << 6);                   /*  2-bit unassigned */
    /** \todo this word can be repeated several times... */

    rtcm_stream(3, (uint32_t *) &rtcm);
}


/**
 * \brief Null Frame
 *
 * This function generates RTCM msg6 blocks.
 */
/** \todo not necessary? */
/*@unused@*/ static void rtcm2_msg6(void)
{
    /* RTCM Header */
    rtcm[0] = (uint32_t)
              (0x66 << 22) |              /*  8-bit preamble */
              (6 << 16) |                 /*  6-bit message type */
              (rsid << 6);                /* 10-bit station ID */
    rtcm[1] = (uint32_t)
              (mzc << 17) |               /* 13-bit modified z-count */
              (sqnum << 14) |             /*  3-bit sequence no. */
              (2 << 9) |                  /*  5-bit length of frame */
              (rsh << 3);                 /*  3-bit station health */
    sqnum = (sqnum + 1) % 8;

    /* RTCM Content */
    /* None. It's a null message */

    rtcm_stream(2, (uint32_t *) &rtcm);
}


/**
 * \brief Radio Beacon Almanac
 *
 * This function generates RTCM msg7 blocks.
 */
/** \todo not necessary? */
/*@unused@*/ static void rtcm2_msg7(void)
{
    /* RTCM Header */
    rtcm[0] = (uint32_t)
              (0x66 << 22) |              /*  8-bit preamble */
              (7 << 16) |                 /*  6-bit message type */
              (rsid << 6);                /* 10-bit station ID */
    rtcm[1] = (uint32_t)
              (mzc << 17) |               /* 13-bit modified z-count */
              (sqnum << 14) |             /*  3-bit sequence no. */
              (5 << 9) |                  /*  5-bit length of frame */
              (rsh << 3);                 /*  3-bit station health */
    sqnum = (sqnum + 1) % 8;

    /* RTCM Content */
    rtcm[2] = (uint32_t)                  /** \todo check if this is correct */
              (0 << 14) |                 /* 16-bit lat */
              (0 << 6);                   /*  8-bit lon (h) */
    rtcm[3] = (uint32_t)                  /** \todo check if this is correct */
              (0 << 22) |                 /*  8-bit lon (l) */
              (0 << 12) |                 /* 10-bit range */
              (0 << 6);                   /*  6-bit freq (h) */
    rtcm[4] = (uint32_t)                  /** \todo check if this is correct */
              (0 << 24) |                 /*  6-bit freq (l) */
              (rsh << 22) |               /*  2-bit health */
              (rsid << 12) |              /* 10-bit station ID */
              (0 << 9) |                  /*  3-bit bit rate */
              (0 << 8) |                  /*  1-bit mod mode */
              (0 << 7) |                  /*  1-bit sync type */
              (0 << 6);                   /*  1-bit encoding */
    /** \todo this word can be repeated several times... */

    rtcm_stream(5, (uint32_t *) &rtcm);
}


/**
 * \brief Partial Satellite Set Differential Corrections
 *
 * This function generates RTCM msg9 blocks.
 */
static void rtcm2_msg9(void)
{
    /* RTCM Header */
    rtcm[0] = (uint32_t)
              (0x66 << 22) |              /*  8-bit preamble */
              (9 << 16) |                 /*  6-bit message type */
              (rsid << 6);                /* 10-bit station ID */
    rtcm[1] = (uint32_t)
              (mzc << 17) |               /* 13-bit modified z-count */
              (sqnum << 14) |             /*  3-bit sequence no. */
              (7 << 9) |                  /*  5-bit length of frame */
              (rsh << 3);                 /*  3-bit station health */
    sqnum = (sqnum + 1) % 8;

    /* RTCM Content */
    rtcm[2] = (uint32_t)
              (sf[prn] << 29) |           /*  1-bit scale factor */
              (udre[prn] << 27) |         /*  2-bit UDRE */
              (prn << 22) |               /*  5-bit satellite ID */
              (prc[prn] << 6);            /* 16-bit pseudorange correction */
    rtcm[3] = (uint32_t)
              (rrc[prn] << 22) |          /*  8-bit range-rate correction */
              (iod[prn] << 14) |          /*  8-bit issue of data */
              (sf[prn] << 13) |           /*  1-bit scale factor */
              (udre[prn] << 11) |         /*  2-bit UDRE */
              (prn << 6);                 /*  6-bit satellite ID */
    rtcm[4] = (uint32_t)
              (prc[prn] << 14) |          /* 16-bit pseudorange correction */
              (rrc[prn] << 6);            /*  8-bit range-rate correction */
    rtcm[5] = (uint32_t)
              (iod[prn] << 22) |          /*  8-bit issue of date */
              (sf[prn] << 21) |           /*  1-bit scale factor */
              (udre[prn] << 19) |         /*  2-bit UDRE */
              (prn << 14) |               /*  5-bit satellite ID */
              ((prc[prn] >> 8) << 6);     /*  8-bit pseudorange correction (h) */
    rtcm[6] = (uint32_t)
              ((prc[prn] & 0xff) << 22) | /*  8-bit pseudorange correction (l) */
              (rrc[prn] << 14) |          /*  8-bit range-rate correction */
              (iod[prn] << 6);            /*  8-bit issue of date */

    rtcm_stream(7, (uint32_t *) &rtcm);
}


/**
 * \brief Special Message
 *
 * This function generates RTCM msg5 blocks.
 */
static void rtcm2_msg16(void)
{
    /* RTCM Header */
    rtcm[0] = (uint32_t)
              (0x66 << 22) |              /*  8-bit preamble */
              (16 << 16) |                /*  6-bit message type */
              (rsid << 6);                /* 10-bit station ID */
    rtcm[1] = (uint32_t)
              (mzc << 17) |               /* 13-bit modified z-count */
              (sqnum << 14) |             /*  3-bit sequence no. */
              (5 << 9) |                  /*  5-bit length of frame */
              (rsh << 3);                 /*  3-bit station health */
    sqnum = (sqnum + 1) % 8;

    /* RTCM Content */
    rtcm[2] = (uint32_t)
              (0x20 << 22) |              /*  8-bit character ' ' */
              (0x52 << 14) |              /*  8-bit character 'R' */
              (0x41 <<  6);               /*  8-bit character 'A' */
    rtcm[3] = (uint32_t)
              (0x53 << 22) |              /*  8-bit character 'S' */
              (0x41 << 14) |              /*  8-bit character 'A' */
              (0x4e <<  6);               /*  8-bit character 'N' */
    rtcm[4] = (uint32_t)
              (0x54 << 22) |              /*  8-bit character 'T' */
              (0x20 << 14) |              /*  8-bit character ' ' */
              (0x00 <<  6);               /*  8-bit character     */

    /** \todo this word can be repeated several times... */

    rtcm_stream(3, (uint32_t *) &rtcm);
}


/**
 * \brief Decode and handle RASANT message
 *
 * This function decodes and handles RASANT messages.
 *
 * \param[in] _x Block 2 data
 * \param[in] _y Block 3 data
 * \param[in] _z Block 4 data
 */
void rds_oda_rasant_decode(uint8_t _x, uint16_t _y, uint16_t _z)
{
    uint8_t msg = (_x >> 2) & 7;

    switch (msg)
    {
    case 0: /* Synchronization: Time synchronisation, ID and Health of reference station */
        /* (_x & 3) */                    /*  2-bit not occupied */
        /* (_y >> 10) */                  /*  8-bit not occupied */
        rsid = _y & 0x3ff;                /* 10-bit station ID */
        rsh = _z >> 13;                   /*  3-bit station health */
        mzc = _z & 0x1fff;                /* 13-bit modified Z-count */
        rtcm2_msg5();
        break;
    case 1: /* Correction data: Satellite correction data */
        prn = _z >> 11;                   /*  5-bit PRN */
        udre[prn] = _x & 3;               /*  2-bit UDRE */
        prc[prn] = _y;                    /* 16-bit PRC [mm] (signed) */
        /* (_z >> 10) & 1 */              /*  1-bit T: Time switch (0=even minute, 1=odd minute) */
        sf[prn] = (_z >> 9) & 1;          /*  1-bit SF: Scaling Factor */
        /* (z >> 8) & 1, */               /*  1-bit IOD switch (changes with each IOD change) */
        rrc[prn] = (_z & 0xff);           /*  8-bit RRC [mm/sec] (signed) */
        rtcm2_msg9();
        break;
    case 2: /* Delta correction: Satellite correction data (when IOD changes) */
        prn = _z >> 11;                   /*  5-bit PRN */
        udre[prn] = _x & 3;               /*  2-bit UDRE */
        prc[prn] = _y;                    /* 16-bit PRC [mm] (signed) */
        /* (z >> 10) & 1 */               /*  1-bit ?? flag */
        /* (z >> 9) & 1 */                /*  1-bit ?? flag */
        sf[prn] = (_z >> 8) & 1;          /*  1-bit SF */
        iod[prn] = _z & 0xff;             /*  8-bit IODE */
        rtcm2_msg9();
        break;
    case 3: /* Reference station data: Reference station coordinates */
        if ((_x&3) >= 3)
            break;
        rsc[_x&3] = ((_y << 8) | _z);     /* 32-bit reference station coordinates [cm] (signed 32 bit number) */
        rtcm2_msg3();
        break;
    case 4: /* Optional data */
        switch (_x & 3)
        {
        case 3:
            /* y */                       /* 16-bit ??? */
            /* z */                       /* 16-bit picode */
            /* z looks like a picode: */
            /* y=e0dc z=d3a3=swr3 bw */   /* -32/-36 */
            /* y=e0fc z=d3a3=swr3 bw */   /* -32/-4 */
            /* y=e0cd z=d81c=charivari neumarkt */
            rtcm2_msg16(); /** \todo check if this is correct */
            break;
        default:
            rds_decode_status = RDS_DECODE_STATUS_NOT_IMPLEMENTED_YET;
            break;
        }
        break;
    case 5: /* IOD: Satellite data IOD */
        /* ((_x & 3) << 3) || (_y >> 13) */ /*  5-bit PRN1    satellite number 0..31 */
        /* (y >> 5) & 0xff */             /*  8-bit IOD1    Issue of Data for PRN1, matching the IOD for the current ephemeris of this satellite, as transmitted by the satellite. */
        /* y & 0x1f */                    /*  5-bit PRN2    satellite number 0..31 */
        /* z >> 8 */                      /*  8-bit IOD2new issue of data for PRN2, new value (90sec transistion period) */
        /* z & 0xff */                    /*  8-bit IOD2old issue of data for PRN2, old value */
        /* rtcm2_msg9(); */ /** \todo check if this is correct */
        break;
    default: /* 6, 7: Unassigned */
        rds_decode_status = RDS_DECODE_STATUS_NOT_ASSIGNED;
        break;
    }
}
