CONFIG_CLEAN_FILES = Makefile.in

AM_CPPFLAGS = -I $(top_srcdir)/lib
ACLOCAL_AMFLAGS = -I m4

TESTS = \
  af_test   ct_test  di_test  ecc_test eon_test \
  erp_test  ews_test ih_test  lic_test ms_test  \
  oda_test  pi_test  pin_test ps_test  pty_test \
  ptyn_test rds_test rp_test  rt_test  ta_test  \
  tdc_test  tp_test \
  oda_ert_test oda_rtp_test

if ODA_IRDS
TESTS += oda_irds_test
AM_CPPFLAGS += -DODA_IRDS
endif ODA_IRDS
if ODA_RASANT
TESTS += oda_rasant_test
AM_CPPFLAGS += -DODA_RASANT
endif ODA_RASANT
if ODA_TMC
TESTS += oda_tmc_test
AM_CPPFLAGS += -DODA_TMC
endif ODA_TMC

check_PROGRAMS = $(TESTS)

TESTS_ENVIRONMENT = $(VALGRIND) -q --leak-check=full --error-exitcode=1

LDADD = /usr/lib/libsqlite3.la ../lib/librds.la @CHECK_LIBS@

#
# RDS standard services
#

af_test_SOURCES = af_test.c
af_test_CFLAGS = @CHECK_CFLAGS@

ct_test_SOURCES = ct_test.c
ct_test_CFLAGS = @CHECK_CFLAGS@

di_test_SOURCES = di_test.c
di_test_CFLAGS = @CHECK_CFLAGS@

ecc_test_SOURCES = ecc_test.c
ecc_test_CFLAGS = @CHECK_CFLAGS@

eon_test_SOURCES = eon_test.c
eon_test_CFLAGS = @CHECK_CFLAGS@

erp_test_SOURCES = erp_test.c
erp_test_CFLAGS = @CHECK_CFLAGS@

ews_test_SOURCES = ews_test.c
ews_test_CFLAGS = @CHECK_CFLAGS@

ih_test_SOURCES = ih_test.c
ih_test_CFLAGS = @CHECK_CFLAGS@

lic_test_SOURCES = lic_test.c
lic_test_CFLAGS = @CHECK_CFLAGS@

ms_test_SOURCES = ms_test.c
ms_test_CFLAGS = @CHECK_CFLAGS@

oda_test_SOURCES = oda_test.c
oda_test_CFLAGS = @CHECK_CFLAGS@

pi_test_SOURCES = pi_test.c
pi_test_CFLAGS = @CHECK_CFLAGS@

pin_test_SOURCES = pin_test.c
pin_test_CFLAGS = @CHECK_CFLAGS@

ps_test_SOURCES = ps_test.c
ps_test_CFLAGS = @CHECK_CFLAGS@

pty_test_SOURCES = pty_test.c
pty_test_CFLAGS = @CHECK_CFLAGS@

ptyn_test_SOURCES = ptyn_test.c
ptyn_test_CFLAGS = @CHECK_CFLAGS@

rds_test_SOURCES = rds_test.c
rds_test_CFLAGS = @CHECK_CFLAGS@

rp_test_SOURCES = rp_test.c
rp_test_CFLAGS = @CHECK_CFLAGS@

rt_test_SOURCES = rt_test.c
rt_test_CFLAGS = @CHECK_CFLAGS@

ta_test_SOURCES = ta_test.c
ta_test_CFLAGS = @CHECK_CFLAGS@

tdc_test_SOURCES = tdc_test.c
tdc_test_CFLAGS = @CHECK_CFLAGS@

tp_test_SOURCES = tp_test.c
tp_test_CFLAGS = @CHECK_CFLAGS@


#
# RDS ODA services
#

oda_ert_test_SOURCES = oda_ert_test.c
oda_ert_test_CFLAGS = @CHECK_CFLAGS@

if ODA_IRDS
oda_irds_test_SOURCES = oda_irds_test.c
oda_irds_test_CFLAGS = @CHECK_CFLAGS@
endif ODA_IRDS

if ODA_RASANT
oda_rasant_test_SOURCES = oda_rasant_test.c
oda_rasant_test_CFLAGS = @CHECK_CFLAGS@
endif ODA_RASANT

oda_rtp_test_SOURCES = oda_rtp_test.c
oda_rtp_test_CFLAGS = @CHECK_CFLAGS@

if ODA_TMC
oda_tmc_test_SOURCES = oda_tmc_test.c
oda_tmc_test_CFLAGS = @CHECK_CFLAGS@
endif ODA_TMC
