bin_PROGRAMS = rdsdump
dist_man_MANS = rdsd.8 rdsdump.1 rdsradiocapture.1 rdstimed.8

AM_CPPFLAGS = -I $(top_srcdir)/lib
ACLOCAL_AMFLAGS = -I m4
LDADD = /usr/lib/sqlite3/libtclsqlite3.dylib ../lib/librds.la

CONFIG_CLEAN_FILES = Makefile.in

rdsdump_SOURCES = rdsdump.c filters.c print.c

if ODA_RASANT
bin_PROGRAMS += rdsrtcm
dist_man_MANS += rdsrtcm.1
rdsrtcm_SOURCES = rdsrtcm.c filters.c print.c
endif ODA_RASANT

if ODA_TMC
bin_PROGRAMS += rdstmc
dist_man_MANS += rdstmc.1
rdstmc_SOURCES  = rdstmc.c filters.c print.c
endif ODA_TMC
