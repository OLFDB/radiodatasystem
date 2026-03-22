CONFIG_CLEAN_FILES = Makefile.in

clean-local:
	$(RM) -f *.sql.gz *.db

data_DATA = tmc_lcl.sql.gz tmc_lcl_00_0_00.db

tmc_lcl.sql.gz: tmc_lcl.sql
	gzip -c tmc_lcl.sql > tmc_lcl.sql.gz

tmc_lcl_00_0_00.db: tmc_lcl.sql 00_0_00/*
	../../src/rds-db-import tmc_lcl_00_0_00.db tmc_lcl.sql 00_0_00/

tmc_lcl_E0_5_02.db: tmc_lcl.sql E0_5_02/*
	../../src/rds-db-import tmc_lcl_E0_5_02.db tmc_lcl.sql E0_5_02/

tmc_lcl_E0_D_01.db: tmc_lcl.sql E0_D_01/*
	../../src/rds-db-import tmc_lcl_E0_D_01.db tmc_lcl.sql E0_D_01/

tmc_lcl_E2_F_01.db: tmc_lcl.sql E2_F_01/*
	../../src/rds-db-import tmc_lcl_E2_F_01.db tmc_lcl.sql E2_F_01/

tmc_lcl_E3_E_01.db: tmc_lcl.sql E3_E_01/*
	../../src/rds-db-import tmc_lcl_E3_E_01.db tmc_lcl.sql E3_E_01/
