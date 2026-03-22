sharedstate_DATA = rds_program.db

CONFIG_CLEAN_FILES = Makefile.in

clean-local:
	$(RM) -f *.db

rds_program.db: rds_program.sql
	../../src/rds-db-import rds_program.db rds_program.sql
