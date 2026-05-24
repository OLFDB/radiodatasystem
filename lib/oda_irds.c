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
 * \file oda_irds.c
 * \test All related test cases are defined in \ref oda_irds_test.c
 * \brief In-Receiver Database System functions
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This file contains functions for In-Receiver Database System handling.
 */

#include <stdint.h>
#include <string.h>
#include "rds.h"
#include "oda_irds.h"


/**
 * \brief In-Receiver Database System update data information
 *
 * This structure contains update data information.
 */
static struct irds_update_data
{
    uint8_t     rom_class_number;                  /**< ROM Class number (0..31) */
    uint16_t    update_transmission_serial_number; /**< Update Transmission Serial number (0..1023) */
    uint8_t     scope_flag;                        /**< Scope flag (0=Partial, 1=Total) */

    uint16_t    pointer;                           /**< ROM Addresses */
    uint16_t    update_length;                     /**< Update length */
    uint8_t     eom;                               /**< End Of Message (0xff) */

    uint16_t    grid;                              /**< Grid: center grid of updated region (0..16383) */
    uint8_t     coverage;                          /**< Coverage: 0=1grid(1x1) 1=9 grids(3x3) 2=25grids(5x5) 3=81grids(9x9) */
    uint8_t     channel;                           /**< Channel */
    uint8_t     callsign[4];                       /**< Callsign */
    uint16_t    city_ptr;                          /**< City Pointer */
    uint8_t     format;                            /**< Format */

    uint8_t     receive_flags;                     /**< Received update of channel (Bit 0), Callsign (Bit 1+2), Format (Bit 3) */
    uint16_t    received_length;                   /**< Received length */
} irds_update_data;


/**
 * \brief In-Receiver Database System new station data information
 *
 * This structure contains new station data information.
 */
static struct irds_new_station_data
{
    uint16_t    grid;          /**< Grid number: center grid of updated region (0..16383) */
    uint8_t     channel;       /**< Channel */
    uint8_t     callsign[4];   /**< Callsign */
    uint16_t    city_ptr;      /**< City Pointer */
    uint8_t     format;        /**< Format */

    uint8_t     receive_flags; /**< Received update type 7 (Bit 0), ~ type 8 (bit 1), ~ type 9 (bit 2) */
} irds_new_station_data;


/**
 * \brief In-Receiver Database System RAM
 *
 * This structure contains the 2 kilobyte update RAM.
 *
 * Notes on RAM Header File:
 * - Maximum number of updates: (0x27F-4)/4=158
 * - First 2 bytes in RAM Header File contain Header1, the pointer to the first free entry in RAM Header File.
 * - Second 2 bytes in RAM Header File contain Header2, the pointer to the first free entry in Update Data File.
 *
 * Notes on Update Data File:
 * - Maximum number of entries: 202 with average length or 0x37F/4=127 with maximum length (7 bytes)
 *
 * Notes on New Stations File:
 * - Maximum number of entries: 600/10=60
 */
static struct irds_update_ram
{
    uint8_t ram_header_file[0x200];     /**< 0x000: RAM Header File containing ROM addresses, RAM data pointers */
    uint8_t update_data_file[0x380];    /**< 0x200: Update Data File containing Type, Update Data */
    uint8_t new_stations_file[0x100];   /**< 0x600: New Stations File containing Grid No., Channel, Callsign, City Pointer, Format */
    uint8_t preset_memory_area[0x0d8];  /**< 0x700: Preset Memory area */
    uint8_t gnl_purpose_ram[0x28];      /**< 0x7D8: General Purpose RAM area */
} irds_update_ram;


/**
 * \brief Erase Update Data
 *
 * This function erases Updata Data.
 */
static void rds_oda_irds_update_data_erase(void)
{
    uint16_t ptr = 4;

    do
    {
        /* check for ROM Address */
        if ((irds_update_ram.ram_header_file[ptr + 0] == irds_update_data.pointer >> 8) &&
            (irds_update_ram.ram_header_file[ptr + 1] == (irds_update_data.pointer & 0xff)))
        {
            /* just reset RAM Data Pointers */
            irds_update_ram.ram_header_file[ptr + 2] = 0;
            irds_update_ram.ram_header_file[ptr + 3] = 0;
        }
        ptr += 4;
    }
    while (ptr < sizeof(irds_update_ram.ram_header_file));
}


/**
 * \brief Update Update Data
 *
 * This function updates Updata Data.
 */
static void rds_oda_irds_update_data_update(void)
{
    uint8_t update_type;
    uint8_t update_type_length;
    uint16_t header1 = (irds_update_ram.ram_header_file[0] << 8) | irds_update_ram.ram_header_file[1]; /* point to first free adress of the Header Area */
    uint16_t header2 = (irds_update_ram.ram_header_file[2] << 8) | irds_update_ram.ram_header_file[3]; /* point to first free adress of the Update Area */

    /* Update Type */
    switch (irds_update_data.receive_flags)
    {
    case 1: /* Channel */
        update_type = 0;
        update_type_length = 1;
        break;
    case 2: /* Callsign */
        update_type = 1;
        update_type_length = 4;
        break;
    case 3: /* Channel + Callsign */
        update_type = 3;
        update_type_length = 5;
        break;
    case 4: /* Format */
        update_type = 2;
        update_type_length = 1;
        break;
    case 5: /* Channel + Format */
        update_type = 4;
        update_type_length = 2;
        break;
    case 6: /* Callsign + Format */
        update_type = 5;
        update_type_length = 5;
        break;
    case 7: /* Channel + Callsign + Format */
        update_type = 6;
        update_type_length = 6;
        break;
    default:
        /* do nothing */
        return;
    }

    /* check update length */
    if (irds_update_data.update_length != update_type_length)
        return;

    /* RAM Header File */
    irds_update_ram.ram_header_file[header1 + 0] = irds_update_data.pointer >> 8;   /* ROM Address */
    irds_update_ram.ram_header_file[header1 + 1] = irds_update_data.pointer & 0xff;
    irds_update_ram.ram_header_file[header1 + 2] = header2 >> 8;            /* RAM Data Pointers */
    irds_update_ram.ram_header_file[header1 + 3] = header2 & 0xff;
    header1++;
    irds_update_ram.ram_header_file[0] = header1 >> 8;
    irds_update_ram.ram_header_file[1] = header1 & 0xff;

    /* Update Data File */
    irds_update_ram.update_data_file[header2-0x200] = update_type; /* Type */
    header2++;
    if ((irds_update_data.receive_flags & (1 << 0)) != 0)
    {
        irds_update_ram.update_data_file[header2-0x200] = irds_update_data.channel;
        header2++;
    }
    if ((irds_update_data.receive_flags & (1 << 1)) != 0)
    {
        irds_update_ram.update_data_file[header2-0x200] = irds_update_data.callsign[0];
        header2++;
        irds_update_ram.update_data_file[header2-0x200] = irds_update_data.callsign[1];
        header2++;
        irds_update_ram.update_data_file[header2-0x200] = irds_update_data.callsign[2];
        header2++;
        irds_update_ram.update_data_file[header2-0x200] = irds_update_data.callsign[3];
        header2++;
    }
    if ((irds_update_data.receive_flags & (1 << 2)) != 0)
    {
        irds_update_ram.update_data_file[header2-0x200] = irds_update_data.format;
        header2++;
    }
    irds_update_ram.ram_header_file[2] = header2 >> 8;
    irds_update_ram.ram_header_file[3] = header2 & 0xff;
}


/**
 * \brief Erase New Station
 *
 * This function erases New Station.
 */
static void rds_oda_irds_new_station_data_erase(void)
{
    uint16_t ptr;

    ptr = 0;
    do
    {
        /* check if entry already exists */
        if (memcmp(&irds_update_ram.new_stations_file[ptr], &irds_new_station_data, 10) == 0)
        {
            memset(&irds_update_ram.new_stations_file[ptr], 0, 10);
            return;
        }
        ptr += 10;
    }
    while (ptr < sizeof(irds_update_ram.new_stations_file));
}


/**
 * \brief Update New Station
 *
 * This function updates New Station.
 */
static void rds_oda_irds_new_station_data_update(void)
{
    uint8_t empty[10] = {0,0,0,0,0,0,0,0,0,0};
    uint16_t ptr;
    uint16_t first_free = 0xffff;

    /* search if this entry already exists */
    ptr = 0;
    do
    {
        /* remember first free entry */
        if ((first_free == 0xffff) && (memcmp(&irds_update_ram.new_stations_file[ptr], &empty, 10) == 0))
        {
            first_free = ptr;
        }
        /* check if entry already exists */
        if (memcmp(&irds_update_ram.new_stations_file[ptr], &irds_new_station_data, 10) == 0)
        {
            return;
        }
        ptr += 10;
    }
    while (ptr < sizeof(irds_update_ram.new_stations_file));

    /* entry does not exists */
    if (first_free == 0xffff)
    {
        /* overwrite oldest entry */
        /* the standard gives no clue which the oldest entry is, so just do nothing here... */
    }
    else
    {
        memcpy(&irds_update_ram.new_stations_file[first_free], &irds_new_station_data, 10);
    }
}


/**
 * \brief Decodes and handles received I-RDS information
 *
 * This function decodes and handles received I-RDS information.
 *
 * \param[in] _ac Address code
 * \param[in] _y  Block 3 data
 * \param[in] _z  Block 4 data
 */
void rds_oda_irds_decode(uint8_t _ac, uint16_t _y, uint16_t _z)
{
    switch(_ac)
    {
    case 0:
        /* update identifier */
        irds_update_data.grid = _y >> 2;
        irds_update_data.coverage = _y & 3;
        irds_update_data.update_length = _z;

        irds_update_data.receive_flags = 0;
        irds_update_data.received_length = 0;
        break;
    case 1:
        /* update data (channel) */
        irds_update_data.pointer = _y;
        irds_update_data.channel = _z >> 8;
        if ((_z & 0xff) != 0) /* unused */
            rds_decode_status = RDS_DECODE_STATUS_NOT_ASSIGNED;

        irds_update_data.receive_flags |= (1 << 0);
        irds_update_data.received_length += 1;
        break;
    case 2:
        /* update data (callsign) */
        irds_update_data.pointer = _y;
        irds_update_data.callsign[0] = _z >> 8;
        irds_update_data.callsign[1] = _z & 0xff;

        irds_update_data.receive_flags |= (1 << 1);
        irds_update_data.received_length += 2;
        break;
    case 3:
        /* update data (callsign) */
        irds_update_data.pointer = _y;
        irds_update_data.callsign[2] = _z >> 8;
        irds_update_data.callsign[3] = _z & 0xff;

        irds_update_data.receive_flags |= (1 << 2);
        irds_update_data.received_length += 2;
        break;
    case 4:
        /* update data (format) */
        irds_update_data.pointer = _y;
        irds_update_data.format = _z & 0xff;
        if ((_z >> 8) != 0) /* unused */
            rds_decode_status = RDS_DECODE_STATUS_NOT_ASSIGNED;

        irds_update_data.receive_flags |= (1 << 3);
        irds_update_data.received_length += 1;
        break;
    case 5:
        /* erase rom record */
        irds_update_data.pointer = _y;
        if (((_z >> 8) != 0) || ((_z & 0xff) != 0)) /* unused, unused */
            rds_decode_status = RDS_DECODE_STATUS_NOT_ASSIGNED;

        irds_update_data.receive_flags = 0;
        irds_update_data.received_length = 0;
        rds_oda_irds_update_data_erase();
        break;
    case 6:
        /* erase ram record */
        irds_new_station_data.callsign[0] = _y >> 8;
        irds_new_station_data.callsign[1] = _y & 0xff;
        irds_new_station_data.callsign[2] = _z >> 8;
        irds_new_station_data.callsign[3] = _z & 0xff;

        irds_new_station_data.receive_flags = 0;
        rds_oda_irds_new_station_data_erase();
        break;
    case 7:
        /* new record (channel, format) */
        irds_new_station_data.grid = _y;
        irds_new_station_data.channel = _z >> 8;
        irds_new_station_data.format = _z & 0xff;

        irds_new_station_data.receive_flags = 0; /* transmission likely starts with update type 7 */
        irds_new_station_data.receive_flags |= (1 << 0);
        break;
    case 8:
        /* new record (callsign) */
        irds_new_station_data.callsign[0] = _y >> 8;
        irds_new_station_data.callsign[1] = _y & 0xff;
        irds_new_station_data.callsign[2] = _z >> 8;
        irds_new_station_data.callsign[3] = _z & 0xff;

        irds_new_station_data.receive_flags |= (1 << 1);
        break;
    case 9:
        /* new record (city pointer) */
        irds_new_station_data.city_ptr = _y;
        if (_z != 0) /* unused */
            rds_decode_status = RDS_DECODE_STATUS_NOT_ASSIGNED;

        irds_new_station_data.receive_flags |= (1 << 2);
        if (irds_new_station_data.receive_flags == 7)
            rds_oda_irds_new_station_data_update(); /* transmission likely ends with update type 9 */
        break;
    case 31:
        /* eom */
        irds_update_data.eom = _y >> 8;
        if ((irds_update_data.eom != 0xff) || ((_y & 0xff) != 0) || (_z != 0)) /* EOM, unused, unused */
            rds_decode_status = RDS_DECODE_STATUS_NOT_ASSIGNED;
        if (irds_update_data.eom == 0xff)
        {
            if (irds_update_data.update_length == irds_update_data.received_length)
            {
                /* reception is complete */
                rds_oda_irds_update_data_update();
            }
            else
            {
                /* reception is incomplete, do nothing */
            }
        }

        break;
    default: /* 10..30 */
        /* reserved for future use */
        rds_decode_status = RDS_DECODE_STATUS_NOT_ASSIGNED;
        break;
    }
}


/**
 * \brief Decode and handles received I-RDS assign message
 *
 * This function decodes and handles received I-RDS assign messages.
 *
 * \param[in] _msg Message bits
 */
void rds_oda_irds_decode_assign(uint16_t _msg)
{
    irds_update_data.rom_class_number = (_msg >> 11) & 0x1f;
    irds_update_data.update_transmission_serial_number = (_msg >> 1) & 0x3ff;
    irds_update_data.scope_flag = _msg & 1;
}
