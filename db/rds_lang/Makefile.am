CONFIG_CLEAN_FILES = Makefile.in

clean-local:
	$(RM) -f *.sql.gz *.db

data_DATA = rds_lang.sql.gz rds_lang_de_DE.db rds_lang_en_CEN.db rds_lang_en_US.db

rds_lang.sql.gz: rds_lang.sql
	gzip -c rds_lang.sql > rds_lang.sql.gz

rds_lang_de_DE.db: rds_lang.sql de_DE/*
	../../src/rds-db-import rds_lang_de_DE.db rds_lang.sql de_DE/

rds_lang_en_CEN.db: rds_lang.sql en_CEN/*
	../../src/rds-db-import rds_lang_en_CEN.db rds_lang.sql en_CEN/

rds_lang_en_US.db: rds_lang.sql en_US/*
	../../src/rds-db-import rds_lang_en_US.db rds_lang.sql en_US/
