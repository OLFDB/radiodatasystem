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
 * \file rds.h
 * \brief Radio Data System functions - headers
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 * \deprecated Don't relay on the TMC things. They can change...
 *
 * This file contains functions for Radio Data System handling.
 */

#ifndef RDS_H
#define RDS_H

#include <sqlite3.h>
#include <stdint.h>
#include <time.h>
#include <wchar.h>

/*@-exportlocal@*/


/**
 * \brief Statistics counter
 *
 * This variable contains a statistics counter for received RDS groups and versions.
 */
extern uint32_t rds_group_type_cnt[16][2];


/**
 * \brief Number of AFs
 *
 * This definition defines the number of AFs.
 */
#define rds_af_max (50)


/**
 * \brief Structure for Programme Item Number
 *
 * This structure contains the Programme Item Number.
 */
typedef struct rds_pin_t
{
    uint8_t     day;    /**< day */
    uint8_t     hour;   /**< hour */
    uint8_t     minute; /**< minute */
} rds_pin_t;


/**
 * \brief Number of EONs
 *
 * This definition defines the number of EON LIs.
 */
#define rds_eon_li_max (10)


/**
 * \brief Structure for Enhanced Other Networks (EON) Linkage Information (LI)
 *
 * This structure contains the Linkage Information (LI) of Enhanced Other Networks (EON).
 * The contents of the lsn field depend on ILS:
 * - If ILS=0 Then LSN bits 11..0 are Linkage Set Number
 * - If ILS=1 Then LSN bits 11..8 are Country Identifier (CI) and bits 7..0 are Linkage Identifier (LI)
 */
typedef struct rds_eon_li_t
{
    uint16_t    pi; /**< PI code */
    uint8_t     la; /**< Linkage Actuator (LA) */
    uint8_t     eg; /**< Extended Generic indicator (EG) */
    uint8_t     ils;    /**< International Linkage Set indicator (ILS) */
    uint16_t    lsn;    /**< Linkage Set Number (LSN) or Country Identifier (CI) and Linkage Identifier (LI) */
} rds_eon_li_t;


/**
 * \brief Structure for Enhanced Other Networks (EON) Mapped Frequencies (MF)
 *
 * This structure contains the Mapped Frequencies (MF) of Enhanced Other Networks (EON).
 */
typedef struct rds_eon_mf_t
{
    uint8_t     tn;         /**< Tuning Network frequency (TN) */
    uint8_t     mf;         /**< Mapped FM/AM frequency (MF) of Other Network (ON) */
} rds_eon_mf_t;


/**
 * \brief Structure for RDS program information
 *
 * This structure contains all information corresponding to one RDS program (same PI code).
 */
typedef struct rds_program_t
{
    /* primary key is ecc & pi (including cc) */
    uint16_t    pi;         /**< Programme Identification (PI) code */

    /* Type 0 groups: Basic tuning and switching information */
    wchar_t     ps[8+1];        /**< Programme Service (PS) name */
    uint8_t     pty;            /**< Programme Type (PTY) code */
    uint8_t     tp;         /**< Traffic Programme (TP) identification code */
    uint8_t     af[rds_af_max];     /**< Alternative Frequency (AF) list */
    uint8_t     ta;         /**< Traffic Announcement (TA) code */
    uint8_t     di_st;          /**< Decoder Identification (DI) code 0 (0=mono, 1=stereo) */
    uint8_t     di_ah;          /**< Decoder Identification (DI) code 1 (0=not artificial head, 1=artificial head) */
    uint8_t     di_co;          /**< Decoder Identification (DI) code 2 (0=not compressed, 1=compressed) */
    uint8_t     di_dp;          /**< Decoder Identification (DI) code 3 (0=static PTY, 1=dynamically switched PTY) */
    uint8_t     ms;         /**< Music Speech (MS) code (0=speech, 1=music) */

    /* Type 1 groups: Programme Item Number and slow labelling codes */
    rds_pin_t   pin;            /**< Programme Item Number (PIN) */
    uint8_t     ecc;            /**< Extended Country Code (ECC) */
    char        iso[2+1];       /**< ISO 3166 country code */
    uint8_t     itu;            /**< ITU region */
    uint8_t     lic;            /**< Language Identification Code */
    uint8_t     la;         /**< Linkage Actuator */

    /* Type 2 groups: RadioText */
    wchar_t     rt[64+1];       /**< RadioText (RT) */

    /* Type 3A groups: Application identification for Open Data */
    uint16_t    oda[16][2];     /**< Open Data Application (ODA) Assignment of AIDs to AGT */

    /* Type 4A groups: Clock-time and date */
    time_t      ct;         /**< Clock Time (CT) */
    int8_t      ct_lto;         /**< local time offset, expressed in multiple of half hours */

    /* Type 10A groups: Programme Type Name */
    wchar_t     ptyn[8+1];      /**< Programme Type Name (PTYN) */

    /* Type 14 groups: Enhanced Other Networks information */
    rds_eon_li_t    eon_li[rds_eon_li_max]; /**< Enhanced Other Networks (EON) Linkage Information (LI) */
    rds_eon_mf_t    eon_mf[5];      /**< Enhanced Other Networks (EON) Mapped Frequencies (MF). 0..3: FM, 4: AM */

#ifdef ODA_RASANT
    /* ODA 0x0d45: RDS-TMC: ALERT-C / EN ISO 14819-1 (for testing use, only) */
    /* handled together with 0xcd46 below */
#endif

#ifdef ODA_IRDS
    /* ODA 0x4aa1: RASANT */
#endif

    /* ODA 0x4bd7: RadioText+ / RT+ */
    uint8_t     oda_rtp_cb;     /**< RT+: control bits flag (0=no template available, 1=template available) */
    uint8_t     oda_rtp_scb;        /**< RT+: server control bits */
    uint8_t     oda_rtp_tn;     /**< RT+: template number */
    uint8_t     oda_rtp_itb;        /**< RT+: item toggle bit */
    uint8_t     oda_rtp_irb;        /**< RT+: item running bit */
    uint8_t     oda_rtp_ct[2];      /**< RT+: content types */
    wchar_t     oda_rtp[64][128+1]; /**< RT+: content strings (UTF-16 coded) */

    /* ODA 0x6552: Enhanced RadioText / eRT */
    wchar_t     oda_ert[128+1];     /**< eRT: String */

#ifdef ODA_IRDS
    /* ODA 0xc563: ID Logic */
#endif

#ifdef ODA_TMC
    /* ODA 0xcd46: RDS-TMC: ALERT-C / EN ISO 14819-1 (for service use, only) */
    uint8_t     oda_tmc_cid;        /**< TMC: Country ID */
    uint8_t     oda_tmc_lid;        /**< TMC: Language ID */
    uint8_t     oda_tmc_sid;        /**< TMC: service identifier */
    /* all other information is kept in rds_oda_tmc_dsp_t structure */
#endif
} rds_program_t;


/**
 * \brief Current RDS program
 *
 * This points to the current RDS program.
 */
extern rds_program_t *rds_program_current;


/**
 * \brief Callback function
 *
 * This variable contains the callback function.
 *
 * \param[out] _new New program data
 * \param[out] _old Old program data
 */
extern void (*rds_callback)(rds_program_t *_new, rds_program_t *_old);


/**
 * \brief Decode and handle RDS message
 *
 * This function decodes and handles RDS messages.
 *
 * \param[in] _pi   Block 1 data (contains PI code)
 * \param[in] _blk2 Block 2 data
 * \param[in] _blk3 Block 3 data (contains PI code in type B)
 * \param[in] _blk4 Block 4 data
 */
void rds_decode(uint16_t _pi, uint16_t _blk2, uint16_t _blk3, uint16_t _blk4);


/**
 * \brief RDS decode status type
 */
typedef enum
{
    RDS_DECODE_STATUS_OK            = 0,
    RDS_DECODE_STATUS_NOT_ASSIGNED      = 1, /**< not assigned, spare bits, variant unallocated, reserved for future use */
    RDS_DECODE_STATUS_FOR_BROADCASTERS_USE  = 2, /**< for use by broadcasters, reserved for broadcasters use */
    RDS_DECODE_STATUS_NOT_IMPLEMENTED_YET   = 3, /**< not implemented yet */
    RDS_DECODE_STATUS_UNKNOWN_AID       = 4, /**< unknown ODA AID */
    RDS_DECODE_STATUS_ICONV_ERROR       = 5, /**< iconv error */
} rds_decode_status_t;


/**
 * \brief RDS decode status from the last decoded message.
 *
 * This variable contains RDS decode status from the last rds_decode operation.
 * The status is reset at the beginning of every rds_decode operation.
 */
extern rds_decode_status_t rds_decode_status;


/**
 * \brief RDS program load config type
 */
typedef enum
{
    RDS_PROGRAM_LOAD_CONFIG_NEVER       = 0,    /**< never load, just clear data */
    RDS_PROGRAM_LOAD_CONFIG_ON_NEW_PROGRAM  = 1,    /**< load if new program is received (default) */
} rds_program_load_config_t;


/**
 * \brief Load RDS program configuration
 *
 * This parameter configures when RDS programs will be loaded.
 * Default is \ref RDS_PROGRAM_LOAD_CONFIG_ON_NEW_PROGRAM.
 */
extern rds_program_load_config_t rds_program_load_config;


/**
 * \brief Load RDS program
 *
 * This function loads a RDS program from the persistent storage.
 * Assume that the handle is only valid until the next RDS decode.
 * The function only checks for PI, not for ECC as it may not be received yet.
 *
 * \param[in] _ecc  ECC Code
 * \param[in] _pi   PI Code
 * \return      Pointer to RDS program
 */
rds_program_t *rds_program_load(uint8_t _ecc, uint16_t _pi);


/**
 * \brief RDS program save config type
 */
typedef enum
{
    RDS_PROGRAM_SAVE_CONFIG_NEVER       = 0,    /**< never save */
    RDS_PROGRAM_SAVE_CONFIG_OLDEST      = 1,    /**< save oldest entry if it is needed for new program (default) */
    RDS_PROGRAM_SAVE_CONFIG_ON_CT       = 2,    /**< save on CT reception (usually once per minute) */
    RDS_PROGRAM_SAVE_CONFIG_ON_CHANGE   = 3,    /**< save on change */
    RDS_PROGRAM_SAVE_CONFIG_ON_RECEPTION    = 4,    /**< save on every reception */
} rds_program_save_config_t;


/**
 * \brief Save RDS program configuration
 *
 * This parameter configures when RDS programs will be saved.
 * Default is \ref RDS_PROGRAM_SAVE_CONFIG_OLDEST.
 */
extern rds_program_save_config_t rds_program_save_config;


/**
 * \brief Save RDS program
 *
 * This function saves a RDS program to persistent storage.
 * The data is saved to the tables RDS, EON and ODA_RTP.
 *
 * \param[in] _rs   RDS program
 */
void rds_program_save(rds_program_t *_rs);


/**
 * \brief Save all RDS programs
 *
 * This function saves all RDS programs to persistent storage.
 */
void rds_program_save_all(void);


/**
 * \brief Return country name
 *
 * This function returns the country name.
 *
 * \param[out] _str Return country name
 * \param[in]  _size    String size
 * \param[in]  _ecc Extended country code
 * \param[in]  _cc  Country code
 */
void rds_ecc_get_name(char *_str, size_t _size, uint8_t _ecc, uint8_t _cc);


/**
 * \brief EWS ident callback function
 *
 * This variable contains the EWS ident callback function.
 *
 * \param[in] _id   Identification of EWS channel
 */
extern void (*rds_ews_ident_callback)(uint16_t _id);


/**
 * \brief EWS callback function
 *
 * This variable contains the EWS callback function.
 *
 * \param[in] _x    Message bits in blk 2
 * \param[in] _y    Message bits in blk 3
 * \param[in] _z    Message bits in blk 4
 */
extern void (*rds_ews_callback)(uint8_t _x, uint16_t _y, uint16_t _z);


/**
 * \brief IH callback function
 *
 * This variable contains the IH callback function.
 *
 * \param[in] _ab   A or B message (0=A, 1=B)
 * \param[in] _x    Message bits in blk 2
 * \param[in] _y    Message bits in blk 3
 * \param[in] _z    Message bits in blk 4
 */
extern void (*rds_ih_callback)(uint8_t _ab, uint8_t _x, uint16_t _y, uint16_t _z);


/**
 * \brief Open language database
 *
 * This function opens the database for the given language.
 * If _lang is NULL, it continues using the open database.
 * If there is non open or the new language could not be loaded, it opens the default database.
 *
 * \param[in] _lang Language (e.g. "de_DE", "", NULL)
 * \return      Exit status (EXIT_SUCCESS, EXIT_FAILURE)
 */
int rds_db_lang_open(/*@null@*/ char *_lang);


/**
 * \brief Return LIC string
 *
 * This function returns a LIC string.
 *
 * \param[out] _str Return LIC string
 * \param[in]  _size    String size
 * \param[in]  _lic LI code
 */
void rds_lic_get_str(char *_str, size_t _size, uint8_t _lic);


/**
 * \brief Return PTY string
 *
 * This function returns a PTY string.
 *
 * \param[out] _str     Return PTY string
 * \param[in]  _display_length  Display length (<=8, <=16 or more)
 * \param[in]  _pty     PTY code
 * \param[in]  _rbds        Selector RDS=0 (othen then ITU region 2), RBDS=1 (ITU region 2)
 */
void rds_pty_get_str(char *_str, size_t _display_length, uint8_t _pty, uint8_t _rbds);


/**
 * \brief TDC callback function
 *
 * This variable contains the TDC callback function.
 *
 * \param[in] _addr address-code
 * \param[in] _data data
 */
extern void (*rds_tdc_callback)(uint8_t _addr, uint16_t _data);


/**
 * \brief Return RT+ class string
 *
 * This function returns a RT+ class string.
 *
 * \param[out] _str     Return RT+ class string
 * \param[in]  _size        String size
 * \param[in]  _rtp     RT+ class code
 */
void rds_oda_rtp_get_class(char *_str, size_t _size, uint8_t _rtp);


/**
 * \brief Structure for TMC optional message content
 *
 * This structure contains all content of a TMC optional message.
 */
typedef struct rds_oda_tmc_optional_message_content_t
{
    uint8_t     label;              /**< TMC: message label */
    uint16_t    data;               /**< TMC: message data */
} rds_oda_tmc_optional_message_content_t;


/**
 * \brief Structure for TMC message
 *
 * This structure contains all information of a TMC message.
 */
typedef struct rds_oda_tmc_message_t
{
    /* primary key for TMC provider is ecc|cc|ltn */

    /* receiver information */
    uint8_t     ecc;                /**< 8 bit for extended country code */
    uint8_t     cc;             /**< 4 bit for country code */
    uint8_t     ltn;                /**< 6 bit for location table number */
    uint8_t     sid;                /**< service identifier */
    uint8_t     incomplete;         /**< message incomplete flag */

    /* explicit, broadcast information */
    uint16_t    evt;                /**< 11 bit for event description */
    uint16_t    loc;                /**< 16 bit for primary location */
    uint8_t     dir;                /**< 1 bit for direction */
    uint8_t     ext;                /**< 3 bit for extent */
    uint8_t     dur;                /**< 3 bit for duration */
    uint8_t     div;                /**< 1 bit for diversion advice */

    /* optional message content */
    uint16_t    loc_ir;             /**< 16 bit for interroad primary location */
    rds_oda_tmc_optional_message_content_t opt[28]; /**< list of optional message content */
    uint8_t     opt_cnt;            /**< counter for opt. content */
    /* maximum free format bit size is 4 (MS) * 28 (FF) = 112 */
    /* minimum bit size of opt. msg is 4 (label + no data) */
    /* maximum number of opt. msgs is 112 / 4 = 28 */

    /* implicit information from event list */
    char        nat;                /**< Nature: 'I'=Info 'F'=Forecast 'S'=Silent */
    uint8_t     qnt;                /**< Quantifier: 0..12 */
    char        dur_dl;             /**< Duration Type: 'D/d'=Dynamic 'L/l'=Longer-lasting */
    uint8_t     dir_ub;             /**< Directionality: 1..2 */
    char        urg;                /**< Urgency: 'N'=Normal 'U'=Urgent 'X'=Extremely Urgent */
    uint8_t     upc;                /**< Update Class: 1..31=EL,32..39=FEL */

    /* times (utc for internal presentation) */
    time_t      receive_time;           /**< receive time */
    time_t      expiry_time;            /**< expiry time */
    time_t      start_time;         /**< start time */
    time_t      stop_time;          /**< stop time */
    time_t      decrement_time;         /**< next decrement time of duration */
} rds_oda_tmc_message_t;


/**
 * \brief Structure for TMC data service provider information
 *
 * This structure contains all information corresponding to one TMC data service provider (same SID code).
 */
typedef struct rds_oda_tmc_dsp_t
{
    /* service information */
    uint8_t     ecc;                /**< extended country code */
    uint8_t     cc;             /**< country code */
    uint8_t     sid;                /**< service identifier */
    wchar_t     spn[8];             /**< service provider name */
    uint8_t     ltn;                /**< location table number */
    uint8_t     afi;                /**< alternative frequency indicator */
    uint8_t     mode;               /**< mode: 0=basic 1=enhanced */
    uint8_t     mgs;                /**< message geographical scope: i=international, n=national, r=regional, u=urban */
    /** \todo mgs should be split to mgs_int, _nat, _reg, _urb */

    /* timing information */
    uint8_t     gap;                /**< gap (groups) */
    uint8_t     activity_time;          /**< Ta seconds (mode=enhanced, default 0s) */
    uint8_t     window_time;            /**< Tw seconds (mode=enhanced, default 0s) */
    uint8_t     delay_time;         /**< Td seconds (mode=enhanced, default 1s) */
    /* Time Constraint: 60 (seconds)/(Ta+Tw) = n (where n is an integer > 0) */

    /* alternatives information */
    /** \todo re-add AF handling in TMC */
    /*  rds_af_t    af;*/               /**< Alternative Frequency (AF) code pairs */
    uint8_t     tn[5];              /**< tuned network */
    uint8_t     mf[5];              /**< mapped freq. */
    uint8_t     pi[5];              /**< TN+MF corresponding PI code */
    uint8_t     tnmfpi_cnt;         /**< internal cycle buffer counter (TN+MF+PI) */
    uint8_t     pi_afi[5];          /**< PI (with AFI=1) codes */
    uint8_t     pi_afi_cnt;         /**< internal cycle buffer counter (PI with AFI=1) */

    /* encryption information */
    uint8_t     enc_tabid;          /**< Service Key table ID (0..7) */
    uint8_t     enc_encid;          /**< encryption identifier (5 bits, 0..31) */
    uint8_t     enc_ltnbe;          /**< LTNBE is used instead of LTN, when LTN=0=encrypted */
    uint8_t     enc_test;           /**< 0=not encrypted, 1=use pre-advised key, 2=rfu, 3=full encrypted */
    uint8_t     enc_key_table_pre_adviced[3];   /**< enc_test=1: encryption parameters pre-advised by the service provider (xor value, start bit, rotate left) */
    uint8_t     enc_key_table[8][32][3];    /**< enc_test=3: 8 tables, each with 32 lines (addressed by ENCID), each with 3 decryption parameters (xor value, start bit, rotate left) */

    /* --- internal information --- */

    /* message reception (to build up complete message) */
    rds_oda_tmc_message_t msg;          /**< message that is currently processed */
    uint8_t last_ci;                /**< last received continuity index */

    /* message memory (is expected to store at least 300 msgs) */
    rds_oda_tmc_message_t message_memory[300+10];   /**< message memory for this DSP */
    uint16_t message_memory_cnt;            /**< number of messages in use */

    /* internal information */
    time_t      last_update;            /**< system time of last TMC DSP load */
    uint8_t     decode_last_x;          /**< info from former decode, X part of msg */
    uint16_t    decode_last_y;          /**< info from former decode, Y part of msg */
    uint16_t    decode_last_z;          /**< info from former decode, Z part of msg */
    uint8_t     decode_cnt;         /**< info from former decode, message repetition */
} rds_oda_tmc_dsp_t;


/**
 * \brief Current TMC data service provider
 *
 * This points to the current TMC data service provider.
 */
extern rds_oda_tmc_dsp_t *rds_oda_tmc_dsp_current;


/**
 * \brief Set encryption table ID
 *
 * This function sets the encryption table ID.
 *
 * \param[in] _tabid    Encryption table ID
 */
void rds_oda_tmc_set_enc_tabid(uint8_t _tabid);


/* \todo function to set service key table entries */


/**
 * \brief Get back Event or Phrase Code string
 *
 * This function returns the Phrase (A..Y) or Supplementary Info (Z) string.
 *
 * \param[out] _str Return Event or Phrase Code string
 * \param[in]  _size    Return string maximum length
 * \param[in]  _l   Letter (A..Y=Phrase Code) (Z=Phrase Code)
 * \param[in]  _n   Number in case of _l='Z', Event Code otherwise
 * \param[in]  _q   Number of quantifiers
 */
void rds_oda_tmc_get_phrase(char *_str, size_t _size, char _l, uint16_t _n, uint8_t _q);


/**
 * \brief Structure for TMC Location Code List information
 *
 * This structure contains all relevant information of a TMC Location Code List entry.
 */
typedef struct rds_oda_tmc_lcl_location_t
{
    uint16_t    cid;            /**< Country ID         NUMERIC(3)  fo.key  */
    uint8_t     tabcd;          /**< Table code         NUMERIC(2)  pr.key  */
    uint16_t    lcd;            /**< Location code      NUMERIC(5)  pr.key  */
    uint8_t     allocated;      /**< Allocated          NUMERIC(1)  attr    */
    char        tclass;         /**< Type class         CHAR(1)     fo.key  */
    uint16_t    tcd;            /**< Type code          NUMERIC(3)  fo.key  */
    uint16_t    stcd;           /**< Subtype code       NUMERIC(3)  fo.key  */
    char        roadnumber[11];     /**< Road number        CHAR(10)    opt.    */
    char        junctionnumber[11]; /**< Junction number        CHAR(10)    opt.    */
    uint16_t    rnid;           /**< Road name          NUMERIC     opt.    */
    uint16_t    nid;            /**< Name           NUMERIC     fo.key  */
    uint16_t    n1id;           /**< Name1          NUMERIC     opt.    */
    uint16_t    n2id;           /**< Name2          NUMERIC     opt.    */
    uint16_t    pol_lcd;        /**< Admin area reference   NUMERIC(5)  opt.    */
    uint16_t    oth_lcd;        /**< Other area reference   NUMERIC(5)  opt.    */
    uint16_t    seg_lcd;        /**< Segment reference      NUMERIC(5)  opt.    */
    uint16_t    roa_lcd;        /**< Road reference     NUMERIC(5)  opt.    */
    uint8_t     pes_lev;        /**< Road network level     NUMERIC(1)  fo.key  */
    uint8_t     inpos;          /**< InPos          NUMERIC(1)  opt.    */
    uint8_t     inneg;          /**< InNeg          NUMERIC(1)  opt.    */
    uint8_t     outpos;         /**< OutPos         NUMERIC(1)  opt.    */
    uint8_t     outneg;         /**< OutNeg         NUMERIC(1)  opt.    */
    uint8_t     presentpos;     /**< PresentPos         NUMERIC(1)  opt.    */
    uint8_t     presentneg;     /**< PresentNeg         NUMERIC(1)  opt.    */
    char        diversionpos[11];   /**< DiversionPos       CHAR(10)    opt.    */
    char        diversionneg[11];   /**< DiversionNeg       CHAR(10)    opt.    */
    char        xcoord[10];     /**< Xcoord (Longitude)     CHAR(9)     attr    */
    char        ycoord[9];      /**< Ycoord (Latitude)      CHAR(8)     attr    */
    uint8_t     interruptsroad;     /**< InterruptsRoad     NUMERIC(1)  opt.    */
    uint8_t     urban;          /**< Urban          NUMERIC(1)  attr    */
    uint16_t    neg_off_lcd;        /**< Negative offset        NUMERIC(5)  opt.    */
    uint16_t    pos_off_lcd;        /**< Positive offset        NUMERIC(5)  opt.    */
} rds_oda_tmc_lcl_location_t;


/**
 * \brief Get TMC location information
 *
 * This function returns a TMC location information.
 *
 * \param[in] _lcd  Location Code
 * \param[out] _l   Location Information
 */
void rds_oda_tmc_lcl_get_location(uint16_t _lcd, rds_oda_tmc_lcl_location_t *_l);


/**
 * \brief Get TMC location name
 *
 * This function returns a TMC location name.
 *
 * \param[out] _str Return Name string
 * \param[in]  _size    Return string maximum length
 * \param[in]  _nid Name ID
 */
void rds_oda_tmc_lcl_get_name(char *_str, size_t _size, uint16_t _nid);


/**
 * \brief Get TMC (sub)type name
 *
 * This function returns a TMC (sub)type name.
 *
 * \param[out] _str Return (Sub)Type Name string
 * \param[in]  _size    Return string maximum length
 * \param[in]  _tclass  Type Class
 * \param[in]  _tcd     Type Code
 * \param[in]  _stcd        SubType Code
 */
void rds_oda_tmc_lcl_get_type_name(char *_str, size_t _size, char _tclass, uint8_t _tcd, uint8_t _stcd);


/**
 * \brief TMC callback function
 *
 * This variable contains the TMC callback function.
 *
 * \param[out] _msg TMC message
 * \param[in]  _action  0=new message,
 *          1=message updated (by timer or by reception),
 *          2="problem cleared" (by non-silent cancellation message)
 */
extern void (*rds_oda_tmc_callback)(rds_oda_tmc_message_t *_msg, uint8_t _action);


#endif /* RDS_H */
