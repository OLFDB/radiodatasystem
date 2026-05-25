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
 * \file filters.c
 * \brief Stream decoding
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This is a library to decode streams in different formats.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../lib/rds.h"

#include <simplecble/simplecble.h>
#include <simplecble/adapter.h>

#include <unistd.h>
#define SLEEP_SEC(x) sleep(x)

/* format: V4L format */
int rds_decode_v4l(FILE *fd)
{
    uint8_t		buf[3];			/* v4l data packet */
    static uint16_t	rds[4];			/* rds block */
    uint8_t		rds_idx;		/* rds block index */
    static uint8_t	next_rds_idx = 0;	/* next rds block index */
    
    /* buf[0]: Least Significant Byte of RDS Block */
    /* buf[1]: Most Significant Byte of RDS Block */
    /* buf[2]: */
    // Bit 7:	Error bit. Indicates that an uncorrectable error occurred during reception of this block.
    // Bit 6:	Corrected bit. Indicates that an error was corrected for this data block.
    // Bits 5-3:	Received Offset. Indicates the offset received by the sync system.
    // Bits 2-0:	Offset Name. Indicates the offset applied to this data.
    /** \todo Make input (incl. rds quality settings) and output somehow configurable */
    
    /* clear RDS buffer */
    memset(&buf, 0, sizeof(buf));
    
    /* read RDS buffer */
    /*@-unrecog@*/
    if (fread(&buf, 1, sizeof(buf), fd) != sizeof(buf)) {
        /*@+unrecog@*/
        next_rds_idx = 0;	// reset index
        return EXIT_SUCCESS;
    }
    
    /* no processing on error or corrected blocks */
    if ((buf[2] >> 6) != 0) {
        next_rds_idx = 0;	// reset index
        return EXIT_SUCCESS;
    }
    
    /* check index */
    rds_idx = buf[2] & 7;			/* offset name */
    /* rds_idx = (buf[2] >> 3) & 7; */	/* received offset */
    if (next_rds_idx == rds_idx) {
        rds[rds_idx] = (buf[1] << 8) | buf[0];
        if (rds_idx == 3) {
            rds_decode(rds[0], rds[1], rds[2], rds[3]);
            next_rds_idx = 0;	// reset index
        } else {
            next_rds_idx++;
        }
    } else {
        next_rds_idx = 0;	// reset index
        return EXIT_SUCCESS;
    }
    
    return EXIT_SUCCESS;
}


/* format: 13-byte pattern (raw unsynchronized RDS bitstream, including CRC bits, with no particular alignment. Each byte is to be read MSB first.) */
int rds_decode_raw(FILE *fd)
{
    uint8_t		buf[13];		/* raw data packet (13 bytes, 104 bits) */
    static uint16_t	rds[4];			/* rds block (4x 16 bits)*/
    static uint16_t crc[4];			/* intermediate crc blocks (4x 10 bits)*/
    
    /* clear RDS buffer */
    memset(&buf, 0, sizeof(buf));
    
    /* read RDS buffer */
    /*@-unrecog@*/
    if (fread(&buf, 1, sizeof(buf), fd) != sizeof(buf)) {
        /*@+unrecog@*/
        return EXIT_FAILURE;
    }
    
    /* extract RDS and CRC blocks */
    rds[0] = (((buf[ 0] >> 0) & 0xff) <<  8) | (((buf[ 1] >> 0) & 0xff) <<  0);
    crc[0] = (((buf[ 2] >> 0) & 0xff) <<  2) | (((buf[ 3] >> 6) & 0x03) <<  0);
    rds[1] = (((buf[ 3] >> 0) & 0x3f) << 10) | (((buf[ 4] >> 0) & 0xff) <<  2) | (((buf[ 5] >> 6) & 0x03) <<  0);
    crc[1] = (((buf[ 5] >> 0) & 0x3f) <<  4) | (((buf[ 6] >> 4) & 0x0f) <<  0);
    rds[2] = (((buf[ 6] >> 0) & 0x0f) << 12) | (((buf[ 7] >> 0) & 0xff) <<  4) | (((buf[ 8] >> 4) & 0x0f) <<  0);
    crc[2] = (((buf[ 8] >> 0) & 0x0f) <<  6) | (((buf[ 9] >> 2) & 0x3f) <<  0);
    rds[3] = (((buf[ 9] >> 0) & 0x03) << 14) | (((buf[10] >> 0) & 0xff) <<  6) | (((buf[11] >> 2) & 0x3f) <<  0);
    crc[3] = (((buf[11] >> 0) & 0x03) <<  8) | (((buf[12] >> 0) & 0xff) <<  0);
    
    /* forward the data to rds_decode */
    rds_decode(rds[0], rds[1], rds[2], rds[3]);
    return EXIT_SUCCESS;
    
    /*
     * syndromes		offset words
     * A = 0x3D8		A = 0x0FC
     * B = 0x3D4		B = 0x198
     * C = 0x25C		C = 0x168
     * C'= 0x3CC		C'= 0x350; // not used any more?
     * D = 0x258		D = 0x1B4
     *			E = 0x000; // not used any more, was for MMBS
     */
    
    /*
     * matrix H
     static int matH[26] = {
     0x31B, 0x38F, 0x2A7, 0x0F7, 0x1EE, 0x3DC, 0x201, 0x1BB, 0x376, 0x355, 0x313, 0x39F, 0x287,
     0x0B7, 0x16E, 0x2DC, 0x001, 0x002, 0x004, 0x008, 0x010, 0x020, 0x040, 0x080, 0x100, 0x200
     };
     */
    
    /** \todo no alignment yet */
    /** \todo no checksum calculation and check yet */
    /** \todo no CRC error correction yet */
}


/* format: csv */
int rds_decode_csv(FILE *fd)
{
    static uint16_t    rds[4];        /* rds block (4x 16 bits) */
    
    while (fscanf(fd, "%hu[^0-9]%hu[^0-9]%hu[^0-9]%hu[^0-9]",
                  &rds[0], &rds[1], &rds[2], &rds[3]) == 4)
        rds_decode(rds[0], rds[1], rds[2], rds[3]);
    
    return EXIT_SUCCESS;
}

/* format: rdsspy */
int rds_decode_spy(FILE *fd)
{
    static unsigned int    rds[4];        /* rds block (4x 16 bits) */
    unsigned int jahr, monat, tag, stunde, minute, sekunde, millisekunde;
    
    int x;
    
    // code for reading rdsspy files
    do {
        x=fscanf(fd, "%X %X %X %X %*[^\n]",
                 (unsigned int *)&rds[0], (unsigned int *)&rds[1], (unsigned int *)&rds[2], (unsigned int *)&rds[3]);
        if(x==0)
            x=fscanf(fd, "%*[^\n]");
        rds_decode(rds[0], rds[1], rds[2], rds[3]);
    } while (x!=EOF);
    
    return EXIT_FAILURE;
}


/* format: smp */
int rds_decode_smp(FILE *fd)
{
    static uint8_t	buf[16];	/* 16-byte data packet */
    static uint16_t	rds[4];		/* rds block (4x 16 bits)*/
    
    /* clear RDS buffer */
    memset(&buf, 0, sizeof(buf));
    
    /* read RDS buffer */
    /*@-unrecog@*/
    if (fread(&buf, 1, sizeof(buf), fd) != sizeof(buf)) {
        /*@+unrecog@*/
        return EXIT_FAILURE;
    }
    
    rds[0] = (buf[ 2] << 8) | buf[ 3];
    rds[1] = (buf[ 6] << 8) | buf[ 7];
    rds[2] = (buf[10] << 8) | buf[11];
    rds[3] = (buf[14] << 8) | buf[15];
    
    rds_decode(rds[0], rds[1], rds[2], rds[3]);
    
    return EXIT_SUCCESS;
}


static void adapter_on_scan_start(simpleble_adapter_t adapter, void* userdata) {
    char* identifier = simpleble_adapter_identifier(adapter);
    
    if (identifier == NULL) {
        return;
    }
    
    printf("Adapter %s started scanning.\n", identifier);
    
    // Let's not forget to clear the allocated memory.
    simpleble_free(identifier);
}

static void adapter_on_scan_stop(simpleble_adapter_t adapter, void* userdata) {
    char* identifier = simpleble_adapter_identifier(adapter);
    
    if (identifier == NULL) {
        return;
    }
    
    printf("Adapter %s stopped scanning.\n", identifier);
    
    // Let's not forget to clear the allocated memory.
    simpleble_free(identifier);
}

static void adapter_on_scan_found(simpleble_adapter_t adapter, simpleble_peripheral_t peripheral, void* userdata) {
    char* adapter_identifier = simpleble_adapter_identifier(adapter);
    char* peripheral_identifier = simpleble_peripheral_identifier(peripheral);
    char* peripheral_address = simpleble_peripheral_address(peripheral);
    
    if (adapter_identifier == NULL || peripheral_identifier == NULL || peripheral_address == NULL) {
        return;
    }
    
    printf("Adapter %s found device: %s [%s]\n", adapter_identifier, peripheral_identifier, peripheral_address);
    
    // Let's not forget to release the associated handles and memory
    simpleble_peripheral_release_handle(peripheral);
    simpleble_free(peripheral_address);
    simpleble_free(peripheral_identifier);
}

static void adapter_on_scan_updated(simpleble_adapter_t adapter, simpleble_peripheral_t peripheral, void* userdata) {
    char* adapter_identifier = simpleble_adapter_identifier(adapter);
    char* peripheral_identifier = simpleble_peripheral_identifier(peripheral);
    char* peripheral_address = simpleble_peripheral_address(peripheral);
    
    if (adapter_identifier == NULL || peripheral_identifier == NULL || peripheral_address == NULL) {
        return;
    }
    
    printf("Adapter %s updated device: %s [%s]\n", adapter_identifier, peripheral_identifier, peripheral_address);
    
    // Let's not forget to release the associated handles and memory
    simpleble_peripheral_release_handle(peripheral);
    simpleble_free(peripheral_address);
    simpleble_free(peripheral_identifier);
}

static void print_buffer_hex(uint8_t* buf, size_t len, bool newline) {
    for (size_t i = 0; i < len; i++) {
        printf("%02X", buf[i]);
        
        if (i < (len - 1)) {
            printf(" ");
        }
    }
    
    if (newline) {
        printf("\n");
    }
}

static uint16_t    rds[4];        /* rds block (4x 16 bits)*/
volatile static bool rec = false;

static void peripheral_on_notify(simpleble_peripheral_t handle, simpleble_uuid_t service,
                                 simpleble_uuid_t characteristic, const uint8_t* data, size_t data_length,
                                 void* userdata) {
    
    /* read RDS buffer */
    /*@-unrecog@*/
    int i=0;
    while(true){
        if (data[i] == 3 && rec==false && data_length>=i+9) {
            rds[0] = (data[i+2] << 8) | data[i+1];
            rds[1] = (data[i+4] << 8) | data[i+3];
            rds[2] = (data[i+6] << 8) | data[i+5];
            rds[3] = (data[i+8] << 8) | data[i+7];
            rec=true;
            rds_decode(rds[0], rds[1], rds[2], rds[3]);
            rds_program_save_all();
            rec=false;
            i+=9;
            if(i==data_length)
                break;
        } else {
            break;
        }
    }
    

    
}

void  INThandler(int sig);
simpleble_peripheral_t peripheral;
char* peripheral_identifier;
char* peripheral_address;
simpleble_adapter_t adapter;
simpleble_uuid_t uuid_tx;

/* OLFsDABRadio */
int rds_decode_dab()
{
    adapter = simpleble_adapter_get_handle(0);
    size_t adapter_count = simpleble_adapter_get_count();
    
    simpleble_err_t err_code = SIMPLEBLE_SUCCESS;
    
    if (adapter_count == 0) {
        printf("No adapter was found.\n");
        return 1;
    }
    
    printf("Scanning for DAB BLE device...\n");
    
    simpleble_adapter_scan_for(adapter, 5000);
    
    // Sleep for an additional second before returning.
    // If there are any detections during this period, it means that the
    // internal peripheral took longer to stop than anticipated.
    SLEEP_SEC(1);
    
    size_t peripheral_count = simpleble_adapter_scan_get_results_count(adapter);
    for (size_t peripheral_index = 0; peripheral_index < peripheral_count; peripheral_index++) {
        peripheral = simpleble_adapter_scan_get_results_handle(adapter, peripheral_index);
        
        peripheral_identifier = simpleble_peripheral_identifier(peripheral);
        peripheral_address = simpleble_peripheral_address(peripheral);
        
        bool peripheral_connectable = false;
        simpleble_peripheral_is_connectable(peripheral, &peripheral_connectable);
        
        int16_t peripheral_rssi = simpleble_peripheral_rssi(peripheral);
        
        printf("[%zu] %s [%s] %d dBm %s\n", peripheral_index, peripheral_identifier, peripheral_address,
               peripheral_rssi, peripheral_connectable ? "Connectable" : "Non-Connectable");
        
        if(!strcmp(peripheral_identifier, "DAB")) {
            err_code = simpleble_peripheral_connect(peripheral);
            if (err_code != SIMPLEBLE_SUCCESS) {
                printf("Failed to connect.\n");
                return 1;
            }
            
            size_t characteristic_count = 0;
            for (size_t i = 0; i < simpleble_peripheral_services_count(peripheral); i++) {
                simpleble_service_t service;
                err_code = simpleble_peripheral_services_get(peripheral, i, &service);
                
                if (err_code != SIMPLEBLE_SUCCESS) {
                    printf("Failed to get service.\n");
                    return 1;
                }
                
                for (size_t j = 0; j < service.characteristic_count; j++) {
                    if(!strcmp(service.uuid.value, "6e400001-b5a3-f393-e0a9-e50e24dcca9e")) {
                        if(!strcmp(service.characteristics[j].uuid.value, "6e400002-b5a3-f393-e0a9-e50e24dcca9e")) {
                            uuid_tx = service.characteristics[j].uuid;
                        }
                    }
                }
                
                for (size_t j = 0; j < service.characteristic_count; j++) {
                    printf("[%zu] %s %s\n", characteristic_count, service.uuid.value, service.characteristics[j].uuid.value);
                    if(!strcmp(service.uuid.value, "6e400001-b5a3-f393-e0a9-e50e24dcca9e")) {
                        if(!strcmp(service.characteristics[j].uuid.value, "6e400003-b5a3-f393-e0a9-e50e24dcca9e")) {
                            
                            signal(SIGINT, INThandler);
                            
                            simpleble_peripheral_notify(peripheral, service.uuid,
                                                        service.characteristics[j].uuid, peripheral_on_notify, NULL);
                            int count=0;
                            bool stnshown = false;
                            while(true) {
                                SLEEP_SEC(1);
                                
                                if(rds_program_current && strcmp((char*)rds_program_current->ps, "") && !stnshown) {
                                    printf("Stationname: %ls\n", rds_program_current->ps);
                                    stnshown=true;
                                }
                                
                                // check if an RDS program is received and seek if not, or seek after 120 sec
                                if(count==5 || count==120) {
                                    if(rds_program_current==0 || (rds_program_current != 0 && rds_program_current->oda_tmc_sid==0) || count==120) {
                                        simpleble_peripheral_write_command(peripheral, service.uuid, uuid_tx, (const uint8_t *)"up", 2);
                                        printf((count==120)?"Seek after 120 seconds...\n":"Seek, no TMC program...\n");
                                        if(rds_program_current) {
                                            rds_program_current->ps[0]=0;
                                            rds_program_current->oda_tmc_sid=0;
                                        }
                                        count=0;
                                        stnshown=false;
                                    }
                                }
                                count++;
                            }
                        }
                    }
                }
            }
        }
    
        // Let's not forget to release the associated handles and memory
        simpleble_peripheral_release_handle(peripheral);
        simpleble_free(peripheral_address);
        simpleble_free(peripheral_identifier);
    }
    
    
    // Let's not forget to release the associated handle.
    simpleble_adapter_release_handle(adapter);
    return 0;
}

void  INThandler(int sig)
{
    printf("\nSignal Handler called to release BLE ressources.\n");
    
    signal(sig, SIG_IGN);
    
    // Let's not forget to release the associated handles and memory
    simpleble_peripheral_release_handle(peripheral);
    simpleble_free(peripheral_address);
    simpleble_free(peripheral_identifier);
    simpleble_adapter_release_handle(adapter);
    
    exit(0);
}


