CONFIG_CLEAN_FILES = Makefile.in

clean-local:
	$(RM) -f *.sql.gz *.db

data_DATA = tmc_el.sql.gz tmc_el_en_CEN.db tmc_el_en_GB.db

tmc_el.sql.gz: tmc_el.sql
	gzip -c tmc_el.sql > tmc_el.sql.gz

tmc_el_de_DE.db: tmc_el.sql de_DE/*
	../../src/rds-db-import tmc_el_de_DE.db tmc_el.sql de_DE/

tmc_el_en_CEN.db: tmc_el.sql en_CEN/*
	../../src/rds-db-import tmc_el_en_CEN.db tmc_el.sql en_CEN/

tmc_el_en_GB.db: tmc_el.sql en_GB/*
	../../src/rds-db-import tmc_el_en_GB.db tmc_el.sql en_GB/
