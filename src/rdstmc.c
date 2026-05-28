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
 * \file rdstmc.c
 * \brief Dumps received RDS TMC data.
 * \author Tobias Lorenz <tobias.lorenz@gmx.net>
 *
 * This program dumps all received TMC data from a RDS stream.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "../lib/rds.h"
#include "filters.h"
#include "print.h"
#include "traff.h"

#include <locale.h>

#define BLK "\e[0;30m"
#define RED "\e[0;31m"
#define GRN "\e[0;32m"
#define YEL "\e[0;33m"
#define BLU "\e[0;34m"
#define MAG "\e[0;35m"
#define CYN "\e[0;36m"
#define WHT "\e[0;37m"

static sqlite3 *rds_oda_tmc_db_el;


static void print_time(time_t _t)
{
	static char buf[40];
	struct tm *tm;

	tm = localtime(&_t);
	if (strftime(buf, sizeof(buf), "%F %T %Z", tm) != 0)
		printf("%s\n", buf);
}

static struct isotopos {
    char iso[3];
    int pos;
}isotoarraypos;

static struct isotopos isopos[2] = {{"EN", 0}, {"DE", 1}};

static const char diversion_advice[sizeof isopos/sizeof(struct isotopos)][2][60] = {    /* diversion advice */
    { // EN
        "(no diversion recommended)",
        "end-users are recommended to avoid the area if possible",
    },
    { // DE
        "(keine Umleitung empfohlen",
        "ortskundige sollten den Bereich weiträumig umfahren",
    }
};

static int get_iso_pos(char* iso) { // TODO: make index a cmd line parameter
    int i;
    
    for (i=0; i<(sizeof isopos/sizeof(struct isotopos));i++) {
        if(!strcmp(iso, isopos[i].iso))
            break;
    }
    
    // if country not found use EN as default
    if(i==sizeof isopos/sizeof(struct isotopos))
        return 0;
    
    return isopos[i].pos;
}



/* nature=info (I) duration=dynamic (D) */
/* The situation is expected to continue ... */
static const char duration_str_i_d[sizeof isopos/sizeof(struct isotopos)][8][40] = {	/* dynamic events with an 'information' nature */
    {
        "(no explicit duration to be given)",	/* do not decrement */
        "for at least the next 15 minutes",	    /* do not decrement */
        "for at least the next 30 minutes",	    /* decrement after 15 minutes */
        "for at least the next 1 hour",		    /* decrement after 30 minutes */
        "for at least the next 2 hours",	    /* decrement after 1 hour */
        "for at least the next 3 hours",	    /* decrement after 1 hour */
        "for at least the next 4 hours",	    /* decrement after 1 hour */
        "for the rest of the day",		        /* do not decrement */
    },
    {
        "(keine Dauer angegeben)",
        "für mindestens die nächsten 15 Minuten",
        "für mindestens die nächsten 30 Minuten",
        "für mindestens die nächste Stunde",
        "für mindestens die nächsten 2 Stunden",
        "für mindestens die nächsten 3 Stunden",
        "für mindestens die nächsten 4 Stunden",
        "für den Rest des Tages",
    }
};

/* nature=forecast (F) duration=dynamic (D) */
/* The situation is expected ... */
static const char duration_str_f_d[sizeof isopos/sizeof(struct isotopos)][8][40] = {	/* dynamic events with an 'forecast' nature */
    {
        "(no explicit start-time to be given)",	/* do not decrement */
        "within the next 15 minutes",		    /* do not decrement */
        "within the next 30 minutes",		    /* decrement after 15 minutes */
        "within the next 1 hour",		        /* decrement after 30 minutes */
        "within the next 2 hours",		        /* decrement after 1 hour */
        "within the next 3 hours",		        /* decrement after 1 hour */
        "within the next 4 hours",		        /* decrement after 1 hour */
        "later today",				            /* do not decrement */
    },
    {
        "(keine Startzeit angegeben)",
        "in den nächsten 15 Minuten",
        "in den nächsten 30 Minuten",
        "in der nächsten Stunde",
        "in den nächsten 2 Stunden",
        "in den nächsten 3 Stunden",
        "in den nächsten 4 Stunden",
        "später heute",
    }
};

/* nature=info (I) duration=longer-lasting (L) */
/* The situation is expected to continue... */
static const char duration_str_i_l[sizeof isopos/sizeof(struct isotopos)][8][40] = {	/* long period 'information' events */
    {
        "(no explicit duration to be given)",	/* do not decrement */
        "for the next few hours",		        /* do not decrement */
        "for the rest of the day",		        /* do not decrement */
        "until tomorrow evening",		        /* decrement at midnight */
        "for the rest of the week",		        /* decrement Friday midnight */
        "until the end of next week",		    /* decrement Sunday midnight */
        "until the end of the month",		    /* do not decrement */
        "for a long period",			        /* do not decrement */
    },
    {
        "(keine Dauer angegeben)",
        "für die nächsten Stunden",
        "für den Rest des Tages",
        "bis morgen Abend",
        "für den Rest der Woche",
        "bis Ende nächster Woche",
        "bis zum Ende des Monats",
        "für einen langen Zeitraum",
    }
};

/* nature=forecast (F) duration=long-lasting (L) */
/* The situation is expected ... */
static const char duration_str_f_l[sizeof isopos/sizeof(struct isotopos)][8][40] = {	/* long period 'forecast' events */
    {
        "(no explicit time horizon given)",	        /* do not decrement */
        "within the next few hours",		        /* do not decrement */
        "later today",				                /* do not decrement */
        "tomorrow",				                    /* decrement at midnight */
        "the day after tomorrow",		            /* decrement at midnight */
        "this weekend",				                /* do not decrement */
        "later this week",			                /* do not decrement */
        "next week",				                /* do not decrement */
    },
    {
        "(kein Zeitpunkt angegeben)",
        "in den nächsten Stunden",
        "später heute",
        "morgen",
        "übermorgen",
        "dieses Wochenende",
        "später diese Woche",
        "nächste Woche",
    }
};

/* duration=dynamic (D) */
static const char persistance_str_d[sizeof isopos/sizeof(struct isotopos)][8][60] = {
    {
        "15 minutes (no message to end-user)",
        "15 minutes (with message to end-user)",
        "30 minutes",
        "1 hour",
        "2 hours",
        "3 hours",
        "4 hours",
        "until midnight on the day of message receipt",
    },
    {
        "15 Minuten (keine Nachricht an den Nutzer)",
        "15 Minuten (mit Nachricht an den Nutzer)",
        "30 Minuten",
        "1 Stunde",
        "2 Stunden",
        "3 Stunden",
        "4 Stunden",
        "bis Mitternacht am Tag des Nachrichtenempfangs"
    }
};
/* nature=info (I) or silent (S) */
static const char persistance_str_i[sizeof isopos/sizeof(struct isotopos)][8][62] = {
    {
        "1 hour",
        "2 hours",
        "until midnight on the day of message receipt",
        "until midnight on the day after message receipt",
        "until midnight on the day after message receipt",
        "until midnight on the day after message receipt",
        "until midnight on the day after message receipt",
        "until midnight on the day after message receipt",
    },
    {
        "1 Stunde",
        "2 Stunden",
        "bis Mitternacht am Tag des Nachrichtenempfangs",
        "bis Mitternacht am auf den Nachrichtenempfang folgenden Tages",
        "bis Mitternacht am auf den Nachrichtenempfang folgenden Tages",
        "bis Mitternacht am auf den Nachrichtenempfang folgenden Tages",
        "bis Mitternacht am auf den Nachrichtenempfang folgenden Tages",
        "bis Mitternacht am auf den Nachrichtenempfang folgenden Tages",
    }
};

/* nature=forecast (F) */
static const char persistance_str_f[sizeof isopos/sizeof(struct isotopos)][8][62] = {
    {
        "1 hour",
        "2 hours",
        "until midnight on the day of message receipt",
        "until midnight on the day after message receipt",
        "until midnight on the day after message receipt",
        "until midnight on the day after message receipt",
        "until midnight on the day after message receipt",
        "until midnight on the day after message receipt",
    },
    {
        "1 Stunde",
        "2 Stunden",
        "bis Mitternacht am Tag des Nachrichtenempfangs",
        "bis Mitternacht am auf den Nachrichtenempfang folgenden Tages",
        "bis Mitternacht am auf den Nachrichtenempfang folgenden Tages",
        "bis Mitternacht am auf den Nachrichtenempfang folgenden Tages",
        "bis Mitternacht am auf den Nachrichtenempfang folgenden Tages",
        "bis Mitternacht am auf den Nachrichtenempfang folgenden Tages",
    }
};

static const char duration_pre_str[sizeof isopos/sizeof(struct isotopos)][2][62] = {
    {
        "The situation is expected to continue %s",
        "The situation is expected %s",
    },
    {
        "Die Situation dauert vorraussichtlich an %s",
        "Die Situation wird erwartet %s",
    }
};

/**
 * \brief Decoding of ident information
 *
 * This function returns the duration string.
 *
 * \param n	nature (from event list)
 * \param d	duration (from event list)
 * \param dp	duration and persistance from TMC message
 */
static char *tmc_get_duration_str(uint8_t n, uint8_t d, uint8_t dp)
{
	static char s[256];
	n = toupper(n);
	d = toupper(d);

	memset(&s, 0, sizeof(s));

	if ((n == 'I') && (d == 'D')) {
        snprintf(&s[0], sizeof(s), duration_pre_str[get_iso_pos(rds_program_current->iso)][n == 'I'?0:1], duration_str_i_d[get_iso_pos(rds_program_current->iso)][dp]);
	} else if ((n == 'F') && (d == 'D')) {
		snprintf(&s[0], sizeof(s), duration_pre_str[get_iso_pos(rds_program_current->iso)][n == 'I'?0:1], duration_str_f_d[get_iso_pos(rds_program_current->iso)][dp]);
	} else if ((n == 'I') && (d == 'L')) {
		snprintf(&s[0], sizeof(s), duration_pre_str[get_iso_pos(rds_program_current->iso)][n == 'I'?0:1], duration_str_i_l[get_iso_pos(rds_program_current->iso)][dp]);
	} else if ((n == 'F') && (d == 'L')) {
		snprintf(&s[0], sizeof(s), duration_pre_str[get_iso_pos(rds_program_current->iso)][n == 'I'?0:1], duration_str_f_l[get_iso_pos(rds_program_current->iso)][dp]);
	}

	return &s[0];
}


static void tmc_print_location(uint16_t lcd, rds_oda_tmc_lcl_location_t *l, uint8_t dir, uint8_t dir_ub)
{
	char t[512] = "";

	/* loc = 0: reserved */
	if (lcd == 0) {
		printf("reserved");
		return;
	}

	/* loc = 65533: '(message) for all users' */
	if (lcd == 65533) {
		printf("(message) for all users");
		return;
	}

	/* loc = 65534: 'silent' location code */
	if (lcd == 65534) {
		printf("'silent' location code)");
		return;
	}

	/* loc = 65535: updating or cancelling of messages */
	if (lcd == 65535) {
		printf("updating or cancelling of messages");
		return;
	}

	/* loc = 63488..64511: special use */
	if ((lcd >= 63488) && (lcd <= 64511)) {
		printf("special use");
		return;
	}

	/* loc = 64512 (0xFC00)..65532: INTER-ROAD (foreign location table) */
	if ((lcd >= 64512) && (lcd <= 65532)) {
		printf("INTER-ROAD<ci=%i,ldn=%i> l1=<%i>", (lcd >> 6) & 0xf, lcd & 0x3f, lcd);
		// set ci to ...		4		A		F		1/D
		// set ldn to ...		9..16		1..8		17..32		1..8
		// land is then			Schweiz		Österreich	Frankreich	Deutschland
	}

	/* loc = 1..63487: regular locations */

	/* (sub)type */
	if (l->stcd > 0) { // print only subtypes
		rds_oda_tmc_lcl_get_type_name(&t[0], sizeof(t), l->tclass, l->tcd, l->stcd);
		printf("%s ", &t[0]);
	}

	/* names */
	if (l->roadnumber[0] != '\0')
		printf("%s ", &l->roadnumber[0]);
	if (l->junctionnumber[0] != '\0')
		printf("%s ", &l->junctionnumber[0]);
	if (l->rnid) {
		rds_oda_tmc_lcl_get_name(&t[0], sizeof(t), l->rnid);
		printf("%s ", &t[0]);
	}
	if (l->nid) {
		rds_oda_tmc_lcl_get_name(&t[0], sizeof(t), l->nid);
		printf("%s ", &t[0]);
	}

	if (l->n1id && (dir == 1)) {
		rds_oda_tmc_lcl_get_name(&t[0], sizeof(t), l->n1id);
		printf("%s", &t[0]);
		if (dir_ub == 2)
			return;
		if (l->n2id)
			printf(" heading to ");
	}
	if (l->n2id) {
		rds_oda_tmc_lcl_get_name(&t[0], sizeof(t), l->n2id);
		printf("%s", &t[0]);
		if (dir_ub == 2)
			return;
	}
	if (l->n1id && (dir == 0)) {
		if (l->n2id)
			printf(" heading to ");
		rds_oda_tmc_lcl_get_name(&t[0], sizeof(t), l->n1id);
		printf("%s", &t[0]);
	}
}


static void tmc_print_start_stop_time(uint8_t t)
{
	if (t <= 95) {
		printf("today at %2.2d:%2.2d UTC", (t*15)/60, (t*15)%60);
	} else if (t <= 200) {
		printf("in %d days at %2.2d:00 UTC", (t-96)/24, (t-96)%24);
	} else if (t <= 231) {
		printf("in %d days", (t-200));
	} else {
		printf("%s %d.month", (t-231)%2 ? "mid of" : "end of", (t-230)/2);
	}
}


static char *tmc_get_quantifier(uint8_t q, uint8_t n)
{
	char sql[] = "select L4_Q_ from QC where CODE=__";
	sqlite3_stmt *stmt;
	static char s[256];
	int rc;

	memset(&s, 0, sizeof(s));

	if (q <= 5) {
		/* get q=0..5 from database EL, table QC, columns L4_Q0..L4_Q5 */
		(void) snprintf(sql, sizeof(sql), "select L4_Q%hu from QC where CODE=%hu", q, n);
		(void) sqlite3_prepare(rds_oda_tmc_db_el, sql, (int) sizeof(sql), &stmt, NULL);
		rc = sqlite3_step(stmt);
		if (rc == SQLITE_ROW) {
			(void) strncpy(&s[0], (const char *)sqlite3_column_text(stmt, 0), sizeof(s));
		} else {
		}
		(void) sqlite3_finalize(stmt);
	} else

	switch (q) {
#if 0
	case 0: /* n (small number) */
		if ((n >= 1) && (n <= 28))
			(void) snprintf(&s[0], sizeof(s), "%u", (unsigned int) n);
		else if ((n >= 29) && (n <= 32))
			(void) snprintf(&s[0], sizeof(s), "%u", (unsigned int) (30+(n-29)*2));
		break;
	case 1: /* N (number) */
		if ((n >= 1) && (n <= 4))
			(void) snprintf(&s[0], sizeof(s), "%u", (unsigned int) n);
		else if ((n >= 5) && (n <= 14))
			(void) snprintf(&s[0], sizeof(s), "%u", (unsigned int) (10+(n-5)*10));
		else if ((n >= 15) && (n <= 32))
			(void) snprintf(&s[0], sizeof(s), "%u", (unsigned int) (150+(n-15)*50));
		break;
	case 2: /* less than V metres */
		if ((n >= 1) && (n <= 30))
			(void) snprintf(&s[0], sizeof(s), "less than %u metres", (unsigned int) (n*10));
		break;
	case 3: /* P percent */
		if ((n >= 1) && (n <= 21))
			(void) snprintf(&s[0], sizeof(s), "less than %u metres", (unsigned int) ((n-1)*5));
		break;
	case 4: /* of up to S km/h */
		if ((n >= 1) && (n <= 32))
			(void) snprintf(&s[0], sizeof(s), "of up to %u km/h", (unsigned int) (n*5));
		break;
	case 5: /* of up to M minutes/hours */
		if ((n >= 1) && (n <= 10))
			(void) snprintf(&s[0], sizeof(s), "of up to %u minutes", (unsigned int) (n*5));
		else if ((n >= 11) && (n <= 22))
			(void) snprintf(&s[0], sizeof(s), "of up to %u hours", (unsigned int) (n-10));
		else if ((n >= 23) && (n <= 32))
			(void) snprintf(&s[0], sizeof(s), "of up to %u hours", (unsigned int) (18+(n-23)*6));
		break;
#endif
	case 6: /* T degress Celsius */
		if ((n >= 1) && (n <= 101))
			(void) snprintf(&s[0], sizeof(s), "%u degrees Celsius", (unsigned int) ((n-1)-50));
		break;
	case 7: /* H time */
		if ((n >= 1) && (n <= 144))
			(void) snprintf(&s[0], sizeof(s), "%2.2u:%2.2u", (unsigned int) ((n-1)/6), (unsigned int) (((n-1)%6)*10));
		break;
	case 8: /* W tonnes */
		if ((n >= 1) && (n <= 101))
			(void) snprintf(&s[0], sizeof(s), "%.1f tonnes", (double) (n*0.1));
		else if ((n >= 102) && (n <= 201))
			(void) snprintf(&s[0], sizeof(s), "%.1f tonnes", (double) (10.5+(n-102.0)*0.5));
		break;
	case 9: /* L metres */
		if ((n >= 1) && (n <= 101))
			(void) snprintf(&s[0], sizeof(s), "%.1f metres", (double) ((n-1.0)*0.1));
		else if ((n >= 102) && (n <= 241))
			(void) snprintf(&s[0], sizeof(s), "%.1f metres", (double) (10.5+(n-102.0)*0.5));
		break;
	case 10: /* of up to D millimetres */
		if ((n >= 1) && (n <= 255))
			(void) snprintf(&s[0], sizeof(s), "of up to %u millimetres", (unsigned int) n);
		break;
	case 11: /* M MHz */
		if ((n >= 1) && (n <= 204))
			(void) snprintf(&s[0], sizeof(s), "%u kHz", (unsigned int) (87600+(n-1)*100));
		break;
	case 12: /* k kHz */
		if (rds_program_current->itu == 2) {
			if ((n >= 16) && (n <= 124))
				(void) snprintf(&s[0], sizeof(s), "%u kHz", (unsigned int) (530+(n-16)*10));
		} else { /* ITU regions 1, 3 and 0 (unknown) */
			if ((n >= 1) && (n <= 15)) {
				(void) snprintf(&s[0], sizeof(s), "%u kHz", (unsigned int) (153+(n-1)*9));
			} else if ((n >= 16) && (n <= 135)) {
				(void) snprintf(&s[0], sizeof(s), "%u kHz", (unsigned int) (531+(n-16)*9));
			}
		}
		break;
	}

	return &s[0];
}


/**
 * \brief Prints a TMC message
 */
static void tmc_print_message(rds_oda_tmc_message_t *msg)
{
	int i;
	char *s;
	char t[256] = "";
	rds_oda_tmc_lcl_location_t l1, l2, l3, l4;
	unsigned int diversion_nr;

	/* message receive time */
	printf("Message received at ");
	print_time(msg->receive_time);

	/* urgency */
	switch (msg->urg) {
	case 'N':
//		printf("<Normal urgency>\n");
		break;
	case 'U':
		printf("<Urgent>\n");
		break;
	case 'X':
		printf("<Extremly Urgent>\n");
		break;
	default:
		printf("<unknown Urgency: %c>\n", msg->urg);
	}

	/* nature */
	switch (toupper(msg->nat)) {
	case 'I':
//		printf("<Info>\n");
		break;
	case 'F':
		printf("<Forecast>\n");
		break;
	case 'S':
		printf("<Silent>\n");
		break;
	default:
		printf("<unknown Nature: %c>\n", msg->nat);
	}

	/* loc = 0: reserved */
	/* loc = 1..63487: regular locations */
	/* loc = 63488..64511: special use */
	/* loc = 64512 (0xFC00)..65532: INTER-ROAD (foreign location table) */
	/* loc = 65533: '(message) for all users' */
	/* loc = 65534: 'silent' location code */
	/* loc = 65535: updating or cancelling of messages */

	/* get location 1 */
	if ((msg->loc >= 64512) && (msg->loc <= 65532)) {
		// switch location table in use
		// if unable to load, then don't show the message
		rds_oda_tmc_lcl_get_location(msg->loc_ir, &l1);
	} else {
		rds_oda_tmc_lcl_get_location(msg->loc, &l1);
	}

	/* get location 2 */
	if (msg->ext) {
		memcpy(&l2, &l1, sizeof(rds_oda_tmc_lcl_location_t));
		i = msg->ext;
		while (i-- != 0)
			rds_oda_tmc_lcl_get_location((msg->dir == 0) ? l2.pos_off_lcd : l2.neg_off_lcd, &l2);
	}

	/* get administrative area reference */
	if (l1.pol_lcd) {
		printf("pol_lcd:");
		rds_oda_tmc_lcl_get_location(l1.pol_lcd, &l3);
		tmc_print_location(msg->loc, &l3, msg->dir, 0);
		printf("\n");
	}

	/* get other area reference */
	if (l1.oth_lcd) {
		printf("oth_lcd:");
		rds_oda_tmc_lcl_get_location(l1.oth_lcd, &l3);
		tmc_print_location(msg->loc, &l3, msg->dir, 0);
		printf("\n");
	}

	/* get segment reference */
	if (l1.seg_lcd) {
//		printf("seg_lcd:");
		rds_oda_tmc_lcl_get_location(l1.seg_lcd, &l3);
		tmc_print_location(msg->loc, &l3, msg->dir, 0);
		printf("\n");
	}

	/* get road reference */
	if (l1.roa_lcd) {
//		printf("roa_lcd:");
		rds_oda_tmc_lcl_get_location(l1.roa_lcd, &l3);
		tmc_print_location(msg->loc, &l3, msg->dir, 0);
		printf("\n");
	}

	/* print location */
	if (msg->ext) {
		printf("between ");
		tmc_print_location(msg->loc, &l2, msg->dir, msg->dir_ub);
		printf(" and ");
	}

	/* print location */
	tmc_print_location(msg->loc, &l1, msg->dir, msg->dir_ub);
	printf("\n");

	/* directionality */
	if (msg->dir_ub == 2) {
		printf("both directions");
		printf("\n");
	}

	/* check for explicit stop time */
	/** \todo remove this, if this is correctly implemented in tmc.c */
	int start_time = -1;
	int stop_time = -1;
	int evt_cnt = 1;
	int evt_qnt[4] = {0,0,0,0};
	for (i = 0; i < msg->opt_cnt; i++) {
		unsigned short data = msg->opt[i].data;
		unsigned short dur = msg->dur;
		switch(msg->opt[i].label) {
		case 4: /* Additional quantifiers */
		case 5: /* Additional quantifiers */
			evt_qnt[evt_cnt-1]++;
			break;
		case 7: /* Explicit start time */
			start_time = data;
			break;
		case 8: /* Explicit stop time */
			stop_time = data;
			break;
		case 9: /* Additional event */
			evt_cnt++;
			break;
		}
	}

//	/* Quantifier */
//	printf("qnt=<%i>", msg->qnt);
//	printf("\n");

	/* duration */
	if ((msg->dur > 0) && (msg->stop_time==0)) {
		/* only show this, if we have no explicit stop_time */
		//printf("dur=<%c,%c,%i>", msg->nat, msg->dur_dl, msg->dur);
		printf("%s", tmc_get_duration_str(msg->nat, msg->dur_dl, msg->dur));
		printf("\n");
	}
    
   /* optional message content */
	diversion_nr = 0;
	evt_cnt = 1;
    int qrep=0;
    int dist_hpl_ppl=0;
    int hla=0;
    int hlr=0;
    int hpd=0;
    int sublbl=0;
    int length=0;
    char* quantifier="";
    int optional = 0;
	for (i = 0; i < msg->opt_cnt; i++) {
		unsigned short data = msg->opt[i].data;
		unsigned short dur = msg->dur;
		//printf("ff(%i)=<%i:%i>: ", i, msg->opt[i].label, data);
		switch(msg->opt[i].label) {
		case 0: /* Duration */
			// already covered
			break;
		case 1: /* Control code */
			// already covered
			break;
		case 2:	/* Length of route affected */
                if (data == 0) {
                    printf("$L='Problem extends for more than 100 km'");
                    length=101;
                }
                else if ((data >= 1) && (data <= 10)) {
                    printf("$L='Length of problem is %d km'", data);
                    length=data;
                }
                else if ((data >= 11) && (data <= 15)) {
                    printf("$L='Length of problem is %d km'", 12+(data-11)*2);
                    length=12+(data-11)*2;
                }
                else if ((data >= 16) && (data <= 31)) {
                    printf("$L='Length of problem is %d km'", 25+(data-16)*5);
                    length = 25+(data-16)*5;
                }
			printf("\n");
			break;
		case 3: /* Speed limit */
			if ((data >= 1) && (data <= 26))
				printf("Maximum speed is %d km/h\n", data*5);
			break;
		case 4: /* Additional quantifiers */
		case 5: /* Additional quantifiers */
                /** \todo apply to all preceding additional events
                 Some event descriptions have an additional quantifier, of which the type is specified in the Event list.
                 Label 4 shall precede a 5-bit quantifier field and label 5 an 8-bit quantifier field. Which one is to be used
                 depends on the event; see the Event List. */
            quantifier=tmc_get_quantifier(msg->qnt, data);
			printf("$Q='%s'\n", quantifier);
                
			break;
		case 6: /* Supplementary information */
			rds_oda_tmc_get_phrase(&t[0], sizeof(t), 'Z', data, 0);
			printf("Sub: %s\n", &t[0]);
			break;
		case 7: /* Explicit start time */
			printf("from ");
			//tmc_print_start_stop_time(start_time);
			//printf(" -> ");
			print_time(msg->start_time);
			break;
		case 8: /* Explicit stop time */
			printf("until ");
			//tmc_print_start_stop_time(stop_time);
			//printf(" -> ");
			print_time(msg->stop_time);
			break;
		case 9: /* Additional event */
			evt_cnt++;
			rds_oda_tmc_get_phrase(&t[0], sizeof(t), 0, data, evt_qnt[evt_cnt-1]);
			printf("%s\n", &t[0]);
			break;
		case 10: /* Detailed diversion instructions */
			diversion_nr++;
			if (diversion_nr == 1) {
				printf("Diversion recommended via ");
				rds_oda_tmc_lcl_get_location(data, &l4);
				tmc_print_location(msg->loc, &l4, msg->dir, 0);
			} else {
				printf(" and then via ");
				rds_oda_tmc_lcl_get_location(data, &l4);
				tmc_print_location(msg->loc, &l4, msg->dir, 0);
			}
			printf("\n");
			break;
		case 11: /* Destination */
			if ((i > 0) && (msg->opt[i-1].label != 11))
				printf("for traffic heading towards ");
			else
				printf(" and ");
			rds_oda_tmc_lcl_get_location(data, &l4);
			tmc_print_location(msg->loc, &l4, msg->dir, 0);
			printf("\n");
			break;
        case 12: /* Precise Location Reference */
                dist_hpl_ppl = data & 0x7FF; /* Bit 0-10 */
                /*distance of hazard point location from pre-defined primary location (i.e. distance D1) with
                          100 m resolution */
                hla=(data>>8) & 0x18; /* hazard location accuracy: 0 better than 100m, 1 better than 500m, 2 better than 1km, 3 less than 1km */
                hlr=(data>>8) & 0x20; /* hazard location reliablity 0=reliable, 1=approximate (confirmation desired)*/
                hpd=(data>>8) & 0xC0; /* hazard point dynamics 0=static, 1=dynamic approaching, 2=reciding 3=unknown */
                printf("dist_hpl_ppl=%im hla=%s hlr=%s hpd=%s", dist_hpl_ppl, hla==0?"<=100m":hla==1?"<=500m":hla==2?"<=1km":">1km", hlr==0?"reliable":"approximate", hpd==0?"static":hpd==1?"dynamic/approaching":hpd==2?"dynamic/receding":"dynamic/movement unknown");
                printf("\n");
            break;
        case 13: /* Cross linkage to source of problem, on another route */
			printf("cross_linkage=");
			rds_oda_tmc_lcl_get_location(data, &l4);
			tmc_print_location(msg->loc, &l4, msg->dir, 0);
//			printf("\n");
			break;
		case 14: /* Separator */
			printf(", ");
			diversion_nr = 0;
			break;
        case 15: /* TODO: 15 Other information as defined by sub-labels */
            break;
		default:
			/* Reserved for future use */
			break;
		}
        
        /* text */
        rds_oda_tmc_get_phrase(&t[0], sizeof(t), 0, msg->evt, evt_qnt[0]);
        
        
        
        if(length) {
            char* pos = strstr(&t[0], "(L)");
            if(pos) {
                char* newtext=malloc(strlen(t) + length/10+2);
                if(pos-&t[0]>0)
                    strncpy(newtext, &t[0], pos -&t[0]);
                else
                    newtext[0]='\0';
                char test[100];
                snprintf(test, snprintf(NULL, 0, "%ikm", length)+1, "%ikm", length);
                strcat(newtext, test);
                strcat(&newtext[strlen(newtext)], pos+3);
                printf("%s", newtext);
            }
            length=0;
        }
        
        if(quantifier && strcmp(quantifier, "")) {
            char* pos = strstr(&t[0], "(Q)");
            if(pos) {
                char* newtext=malloc(strlen(t) + strlen(quantifier) + 2);
                memset(newtext, 0, sizeof(strlen(t) + strlen(quantifier) + 2));
                strncpy(newtext, &t[0], pos - &t[0]);
                newtext[pos - &t[0]]='\0';
                char test[100];
                snprintf(test, snprintf(NULL, 0, "%s", quantifier)+1, "%s", quantifier);
                strcat(newtext, test);
                strcat(&newtext[strlen(newtext)], pos+3);
                strncpy(t, newtext, sizeof(t));
            }
            quantifier=0;
        }
        
        printf("%s\n", &t[0]);
        
        printf("Optional %i: %s\n",i, &t[0]);
        optional=1;
	}
    
    if(!optional) {
        rds_oda_tmc_get_phrase(&t[0], sizeof(t), 0, msg->evt, evt_qnt[0]);
        printf("%s -- %i\n", &t[0], msg->evt);
        if(msg->evt == 0) // TODO: remove
            msg->evt=msg->evt;
    } else {
        optional=0;
    }
    
	/* diversion advice */
	if (msg->div) {
		printf("%s", diversion_advice[get_iso_pos(rds_program_current->iso)][msg->div]);
		printf("\n");
	}
    
    traff_create_from_tmc_message(msg);
//    remove_traff_message(msg);
    
}

static void tmc_print(rds_oda_tmc_message_t *_msg, uint8_t _action)
{
	switch(_action) {
	case 0: /* new message */
        printf("\n%s========================================\n", GRN);
        printf("<new message>\n");
		tmc_print_message(_msg);
		break;
	case 1: /* message updated (by timer or by reception) */
        printf("\n%s========================================\n", YEL);
        printf("<message updated>\n");
        tmc_print_message(_msg);
		break;
	default: /* problem cleared (by cancellation message) */
        printf("\n%s========================================\n", RED);
        printf("<message cleared>\n");
		tmc_print_message(_msg);
        remove_traff_message(_msg);
		break;
	}
    printf("%s", WHT);
}

static void callback(rds_program_t *new, rds_program_t *old)
{
	uint8_t separator = 0;
	if (rds_program_current != new)
		return;

	if (new->pi != old->pi) {
		if (separator == 0) {
			printf("========================================\n");
			separator = 1;
		}
		pi_print();
	}
	if (new->ecc != old->ecc) {
		if (separator == 0) {
			printf("========================================\n");
			separator = 1;
		}
		ecc_print();
	}
	if (memcmp(&new->ct, &old->ct, sizeof(new->ct)) != 0) {
		if (separator == 0) {
			printf("========================================\n");
			separator = 1;
		}
		ct_print();
        ecc_print(); // If ECC is not being broadcast the default would never be printed
	}
}


int main(int argc, char *argv[])
{
    
	FILE *fd;
	int opt;
	int filter = 0;
	int usage  = 0;
	int retval = 0;

	while ((opt = getopt(argc, argv, "f:")) != -1) {
		switch (opt) {
		case 'f':
			if (strcmp(optarg, "v4l") == 0) {
				filter = 0;
			} else
			if (strcmp(optarg, "raw") == 0) {
				filter = 1;
			} else
			if (strcmp(optarg, "csv") == 0) {
				filter = 2;
			} else
			if (strcmp(optarg, "smp") == 0) {
				filter = 3;
			} else
            if (strcmp(optarg, "dab") == 0) {
                filter = 4;
            } else
            if (strcmp(optarg, "spy") == 0) {
                filter = 5;
            } else
            if (strcmp(optarg, "fmdx") == 0) {
                filter = 6;
            } else
                filter = -1;
			break;
		default: /* '?' */
			usage = 1;
		}
	}

	/* safety checks */
	if ((filter == -1) || ((filter !=4 && (argc - optind) != 1) && (filter !=6 && (argc - optind) != 1)))
		usage = 1;
	if (usage == 1) {
		printf("Usage: %s [-f filter] filename\n", argv[0]);
		return EXIT_FAILURE;
	}

    if(filter != 4 && filter != 6) {
        /* open file */
        if (strcmp(argv[optind], "-") == 0) {
            fd = stdin;
        } else {
            fd = fopen(argv[optind], "r");
            if (fd <= 0) {
                printf("Unable to open file %s\n", argv[optind]);
                return EXIT_FAILURE;
            }
        }
    }
	/* open database */
    char filename[80];
    
    /* create database filename */
    (void) snprintf(&filename[0], sizeof(filename), "%s/tmc_el_de_DE.db",
                    /*@-unrecog@*/ SHAREDSTATEDIR /*@+unrecog@*/);
    
	if (sqlite3_open(&filename[0], &rds_oda_tmc_db_el) != 0) {
		perror("unable to open EL database");
		(void) sqlite3_close(rds_oda_tmc_db_el);
		return EXIT_SUCCESS;
	}

	/* set callbacks */
	rds_callback = &callback;
	rds_oda_tmc_callback = &tmc_print;

	/* save RDS program on change */
	rds_program_save_config = RDS_PROGRAM_SAVE_CONFIG_ON_CHANGE;

	/* read in stream */
	do {
		switch (filter) {
		case 0:
			retval = rds_decode_v4l(fd);
			break;
		case 1:
			retval = rds_decode_raw(fd);
			break;
		case 2:
			retval = rds_decode_csv(fd);
			break;
		case 3:
			retval = rds_decode_smp(fd);
			break;
        case 4:
            retval = rds_decode_dab();
            break;
        case 5:
            retval = rds_decode_spy(fd);
            break;
        case 6:
            retval = rds_decode_fm_dx_tcp(0, 0);
            break;
        }
	} while (retval == EXIT_SUCCESS);

	/* close file */
    if(filter != 4) {
        (void) fclose(fd);
    }
    
	/* save all RDS programs */
	rds_program_save_all();

	/* close database */
	sqlite3_close(rds_oda_tmc_db_el);

	return EXIT_SUCCESS;
}
