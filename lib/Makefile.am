CONFIG_CLEAN_FILES = Makefile.in

lib_LTLIBRARIES = librds.la

librds_la_SOURCES = \
	rds.c char_set.c \
	af.c ct.c di.c ecc.c eon.c erp.c ews.c ih.c lic.c ms.c oda.c pi.c pin.c ps.c pty.c ptyn.c rp.c rt.c ta.c tdc.c tp.c \
	oda_ert.c oda_rtp.c

include_HEADERS = rds.h

ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DSHAREDSTATEDIR=\"$(sharedstatedir)\"

if ODA_IRDS
librds_la_SOURCES += oda_irds.c
AM_CPPFLAGS += -DODA_IRDS
endif ODA_IRDS
if ODA_RASANT
librds_la_SOURCES += oda_rasant.c
AM_CPPFLAGS += -DODA_RASANT
endif ODA_RASANT
if ODA_TMC
librds_la_SOURCES += oda_tmc.c
AM_CPPFLAGS += -DODA_TMC
endif ODA_TMC
