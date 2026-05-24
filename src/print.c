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
 * \file print.c
 * \brief Prints RDS data in a user friendly way.
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This is a library to print RDS data in a user friendly way.
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <wchar.h>
#include "../lib/rds.h"
#include "../lib/pi.h"


static void af_print_pair(uint8_t af1, uint8_t af2)
{
	static uint8_t tuning_freq;
	static int first;

	/* 0: Not to be used */
	/* 1..204: Carrier frequency */
	if ((af1 >= 1) && (af1 <= 204)) {
		if (af1 == tuning_freq) {
			/* method B */
			if (first == 1)
				printf("AF: %6u kHz (tuning frequency)\n", (unsigned int) (87500+tuning_freq*100));
			printf("AF: %6u kHz (%s)\n", (unsigned int) (87500+af2*100), (af1 < af2) ? "same program" : "regional variant");
		} else
		if (af2 == tuning_freq) {
			/* method B */
			if (first == 1)
				printf("AF: %6u kHz (tuning frequency)\n", (unsigned int) (87500+tuning_freq*100));
			printf("AF: %6u kHz (%s)\n", (unsigned int) (87500+af1*100), (af1 < af2) ? "same program" : "regional variant");
		} else
		{
			if (first == 1)
				printf("AF: %6u kHz\n", (unsigned int) (87500+tuning_freq*100));
			/* method A */
			printf("AF: %6u kHz\n", (unsigned int) (87500+af1*100));
			printf("AF: %6u kHz\n", (unsigned int) (87500+af2*100));
		}
		first = 0;
	} else
	/* 205: Filler code */
	if (af1 == 205) {
	} else
	/* 206..223: Not assigned */
	/* 224..249: 0..25 AFs follow */
	if ((af1 >= 224) && (af1 <= 249)) {
		uint8_t follow = af1-224;
		first = 1;
		tuning_freq = af2;
		printf("AF: (%d AF follow)\n", follow);
		if (follow == 1)
			printf("AF: %6u kHz\n", (unsigned int) (87500+af2*100));
	} else
	/* 250: An LF/MF frequency follows */
	if (af1 == 250) {
		if (first == 1)
			printf("AF: %6u kHz\n", (unsigned int) (87500+tuning_freq*100));
		if (rds_program_current->itu == 2) {
			if ((af2 >= 16) && (af2 <= 124)) {
				/* 530 kHz .. 1610 kHz, spacing 10 kHz */
				printf("AF: %6u kHz\n", (unsigned int) (530+(af2-16)*10));
			}
		} else {
			if ((af2 >= 1) && (af2 <= 15)) {
				/* 153 kHz .. 279 kHz, spacing 9 kHz */
				printf("AF: %6u kHz\n", (unsigned int) (153+(af2-1)*9));
			} else
			if ((af2 >= 16) && (af2 <= 135)) {
				/* 531 kHz .. 1602 kHz, spacing 9 kHz */
				printf("AF: %6u kHz\n", (unsigned int) (531+(af2-16)*9));
			}
		}
		first = 0;
	}
	/* 251..255: Not assigned */
}

void af_print(void)
{
	uint8_t i;

	/* get list */
    for (i = 0; (i < rds_af_max) && (&(rds_program_current->af) != 0); i+=2)
		af_print_pair(
			rds_program_current->af[i+0],
			rds_program_current->af[i+1]);
}

void ct_print(void)
{
	time_t t;
	struct tm *tm;
	char buf[25];

	t = rds_program_current->ct;
	tm = localtime(&t);
	if (strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S%z", tm) != 0)
		printf("CT: %s\n", &buf[0]);
}

void di_st_print(void)
{
	printf("DI: %d (%s)\n", rds_program_current->di_st, (rds_program_current->di_st) ? "Mono" : "Stereo");
}

void di_ah_print(void)
{
	printf("DI: %d (%s)\n", rds_program_current->di_ah, (rds_program_current->di_ah) ? "Not Artificial Head" : "Artificial Head");
}

void di_co_print(void)
{
	printf("DI: %d (%s)\n", rds_program_current->di_co, (rds_program_current->di_co) ? "Not compressed" : "Compressed");
}

void di_dp_print(void)
{
	printf("DI: %d (%s)\n", rds_program_current->di_dp, (rds_program_current->di_dp) ? "Static PTY" : "Dynamically switched PTY");
}

void ecc_print(void)
{
	printf("ECC: ECC=%2.2X PI=%1.1X ISO=%s ITU=%i\n",
		rds_program_current->ecc,
		rds_pi_cc(rds_program_current->pi),
		rds_program_current->iso,
		rds_program_current->itu);
}

void eon_print(void)
{
	uint8_t i;

	printf("EON: cross-referenced programs:");
	for (i = 0; (i < rds_eon_li_max) && (rds_program_current->eon_li[i].pi != 0); i++)
		if (rds_program_current->eon_li[i].pi != 0)
			printf(" (PI:%2.2x LA:%d EG:%d ILS:%d LSN:%d)",
				rds_program_current->eon_li[i].pi,
				rds_program_current->eon_li[i].la,
				rds_program_current->eon_li[i].eg,
				rds_program_current->eon_li[i].ils,
				rds_program_current->eon_li[i].lsn);
	printf("\n");
}

void eon_tn_mf_print(void)
{
	printf("EON: Tuned Networks: %d %d %d %d %d\n",
		rds_program_current->eon_mf[0].tn,
		rds_program_current->eon_mf[1].tn,
		rds_program_current->eon_mf[2].tn,
		rds_program_current->eon_mf[3].tn,
		rds_program_current->eon_mf[4].tn);

	printf("EON: Mapped Frequencies: %d %d %d %d %d\n",
		rds_program_current->eon_mf[0].mf,
		rds_program_current->eon_mf[1].mf,
		rds_program_current->eon_mf[2].mf,
		rds_program_current->eon_mf[3].mf,
		rds_program_current->eon_mf[4].mf);
}

void ert_print(void)
{
    if(wcsstr(&rds_program_current->oda_ert[0], L"\U0000fffd")!=NULL) {
        if(printf("ERT: %ls\n", &rds_program_current->oda_ert[0])<0) {
            perror("printf");
        }
    }
}

void ih_print(uint8_t _ab, uint8_t _x, uint16_t _y, uint16_t _z)
{
	if (_ab == 0) {
		printf("IH: A x=%2.2x y=%4.4x z=%4.4x\n", _x, _y, _z);
	} else {
		printf("IH: B x=%2.2x z=%4.4x\n", _x, _z);
	}
}

void lic_print(void)
{
	char str[60];
	rds_lic_get_str(&str[0], sizeof(str), rds_program_current->lic);
	printf("LIC: %d (%s)\n", rds_program_current->lic, &str[0]);
}

void ms_print(void)
{
	printf("MS: %d (%s)\n", rds_program_current->ms, (rds_program_current->ms == 0) ? "speech" : "music");
}

void oda_print(uint8_t agtc, uint8_t agtv, uint16_t aid)
{
    char* aid_str = "";
    switch (aid) {
#ifdef ODA_TMC
        case 0x0d45: /* RDS-TMC: ALERT-C / EN ISO 14819-1 (for testing use, only) */
            aid_str="RDS-TMC: ALERT-C testing use only";
            break;
#endif
        case 0x4bd7: /* RadioText+ / RT+ */
            aid_str="RadioText+ / RT+";
            break;
        case 0x6552: /* Enhanced RadioText / eRT */
            aid_str="Enhanced RadioText eRT";
            break;
#ifdef ODA_IRDS
        case 0xc563: /* ID Logic */
            aid_str="ID Logic";
            break;
#endif
#ifdef ODA_TMC
        case 0xcd46: /* RDS-TMC: ALERT-C / EN ISO 14819-1 (for service use, only) */
        case 0xcd47: /* RDS-TMC: ALERT-C / EN ISO 14819-1 (for service use, only) */
            aid_str="RDS-TMC: ALERT-C";
            break;
#endif
        default:
            aid_str="UNKNOWN AID";
    }
	if (aid == 0) {
		printf("ODA: deallocation of agt=%i%c to aid=%4.4x -> %s\n",
			agtc, agtv == 0 ? 'A' : 'B', aid, aid_str);
	} else {
		printf("ODA: allocation of agt=%i%c to aid=%4.4x -> %s\n",
			agtc, agtv == 0 ? 'A' : 'B', aid, aid_str);
	}
}

/* country code */
static char *pi_cc_str[16] = {
	"",
	"Deutschland",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"Deutschland",
	"",
	""};

/* programme in terms of area coverage */
static char *pi_ac_str[16] = {
	"local",
	"international",
	"national",
	"supra-regional",
	"regional 1",
	"regional 2",
	"regional 3",
	"regional 4",
	"regional 5",
	"regional 6",
	"regional 7",
	"regional 8",
	"regional 9",
	"regional 10",
	"regional 11",
	"regional 12"};

/* Germany: prn1 is federal land/state */
static char *pi_prn1_str[16] = {
	"Baden-Württemberg",
	"Bayern",
	"Berlin",
	"Brandenburg",
	"Bremen und Bremerhaven",
	"Hamburg",
	"Hessen",
	"Mecklenburg-Vorpommern",
	"Niedersachsen",
	"Nordrhein-Westfalen",
	"Rheinland-Pfalz",
	"Saarland",
	"Sachsen",
	"Sachsen-Anhalt",
	"Schleswig-Holstein",
	"Thüringen"};

void pi_print(void)
{
	unsigned char ecc;
	unsigned char cc;
	unsigned char ac;
	unsigned char prn1;
	unsigned char prn2;
	unsigned char prn;

	ecc  = rds_program_current->ecc;
	cc   = (rds_program_current->pi >> 12) & 0x0f;	/* nibble 1: country code */
	ac   = (rds_program_current->pi >>  8) & 0x0f;	/* nibble 2: programme in terms of area coverage */
	prn1 = (rds_program_current->pi >>  4) & 0x0f;	/* nibble 3: programme reference number - part 1 */
	prn2 = (rds_program_current->pi >>  0) & 0x0f;	/* nibble 4: programme reference number - part 2 */
	prn  = (rds_program_current->pi >>  0) & 0xff;

	if (ecc != 0)
		ecc_print();
	printf("PI: %4.4x (", rds_program_current->pi);

	printf("%s, ", pi_cc_str[cc]);
	printf("%s, ", pi_ac_str[ac]);

	/* Germany specific */
	if ((cc == 0x1) || (cc == 0xd)) {
		if ((cc == 0xd) && (prn2 < 8)) {
			printf("öffentlich-rechtlich, ");
		} else {
			printf("privat, ");
		}

		if ((ac != 1) && (ac != 2)) {
			printf("%s, ", pi_prn1_str[prn1]);
		}
	}

	/* France specific */
	if (cc == 0xF) {
		/* Indicatif zone géographique */
		switch (ac) {
		case 0:
			printf("Radios locales, ");
			break;
		case 1:
			printf("Radios internationales, ");
			break;
		case 2:
			printf("Radios nationales, ");
			break;
		default: /* 4..F */
			printf("Radios à couverture régionales, ");
			break;
		};

		/* Indicatif de la radio */
		if ((prn >= 0x01) && (prn <= 0x0A)) {
			printf("service public");
		} else
		if ((prn >= 0x0B) && (prn <= 0x1E)) {
			printf("autres réseaux nationaux");
		} else
		if ((prn >= 0x1F) && (prn <= 0x32)) {
			printf("réseaux régionaux");
		} else
		if ((prn >= 0x33) && (prn <= 0x50)) {
			printf("autres réseaux régionaux");
		} else
		if ((prn >= 0x51) && (prn <= 0x78)) {
			printf("radios locales");
		} else {
			printf("Unknown");
		}
	}

	printf("");
	printf(")\n");
}

void pin_print(void)
{
	printf("PIN: day %2.2d time %2.2d:%2.2d\n",
		rds_program_current->pin.day,
		rds_program_current->pin.hour,
		rds_program_current->pin.minute);
}

void ps_print(void)
{
    if(wcsstr(&rds_program_current->ps[0], L"\U0000fffd")!=NULL) {
        if(    printf("PS: %ls\n", &rds_program_current->ps[0])<0) {
            perror("PS: printf");
        }
    }

}

void pty_print(void)
{
	char str[60] = "";

	rds_pty_get_str((char*) &str, sizeof(str), rds_program_current->pty, (rds_program_current->itu == 2) ? 1 : 0);
	printf("PTY: %d (%s)\n", rds_program_current->pty, (char*) &str);
}

void ptyn_print(void)
{
    if(wcsstr(&rds_program_current->ptyn[0], L"\U0000fffd")!=NULL) {
        if(printf("PTYN: %ls\n", &rds_program_current->ptyn[0])<0) {
            perror("PTYN: printf");
        }
    }
	
}

void rt_print(void)
{
    if(wcsstr(&rds_program_current->rt[0], L"\U0000fffd")!=NULL) {
        if(printf("RT: %ls\n", &rds_program_current->rt[0])<0) {
            perror("RT: printf");
        }
    }
}

void rtp_print(uint8_t rtp)
{
	char str[60];
	rds_oda_rtp_get_class(&str[0], sizeof(str), rtp);
    if(wcsstr(&rds_program_current->oda_rtp[rtp][0], L"\U0000fffd")!=NULL) {
        if(printf("RTP: %s='%ls'\n", &str[0], &rds_program_current->oda_rtp[rtp][0])<0) {
            perror("RTP: printf");
        }
    }
	
}

void tdc_print(uint8_t _addr, uint16_t _data)
{
	printf("TDC: channel %u: %4.4x\n", _addr, _data);
}

void tp_ta_print(void)
{
	if (rds_program_current->tp == 0) {
		if (rds_program_current->ta == 0) {
			/* This programme does not carry traffic announcements */
			/* nor does it refer, via EON, to a programme that does. */
			printf("TPTA: TP=0 TA=0 (no EON referral to other TP)\n");
		} else {
			/* This programme carries EON information about another programme */
			/* which gives traffic information. */
			printf("TPTA: TP=0 TA=1 (EON refers to traffic info)\n");
		}
	} else {
		if (rds_program_current->ta == 0) {
			/* This programme carries traffic announcements */
			/* but none are being broadcast at present and */
			/* may also carry EON information about */
			/* other traffic announcements. */
			printf("TPTA: TP=1 TA=0 (EON may be available)\n");
		} else {
			/* A traffic announcement is being broadcast */
			/* on this programme at present. */
			printf("TPTA: TP=1 TA=1 (TA is being broadcast)\n");
		}
	}
}
