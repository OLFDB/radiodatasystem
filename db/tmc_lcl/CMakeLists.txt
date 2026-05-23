add_custom_command(
  OUTPUT tmc_lcl.sql.gz 
  COMMAND gzip -c tmc_lcl.sql > tmc_lcl.sql.gz )
  
add_custom_command(
  OUTPUT tmc_lcl_00_0_00.db
  COMMAND ../../src/rds-db-import tmc_lcl_00_0_00.db tmc_lcl.sql 00_0_00/ )
  
add_custom_command(
  OUTPUT tmc_lcl_E0_D_01.db
  COMMAND ../../src/rds-db-import tmc_lcl_E0_D_01.db tmc_lcl.sql E0_D_01/ )

add_custom_target(
    create_tmc_lcl_db ALL
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/tmc_lcl_00_0_00.db"
	"${CMAKE_CURRENT_BINARY_DIR}/tmc_lcl_E0_D_01.db"
)

