add_custom_command(
  OUTPUT rds_program.db
  COMMAND ../../src/rds-db-import rds_program.db rds_program.sql
)

add_custom_target(
    create_rds_program_db ALL
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/rds_program.db"
)

