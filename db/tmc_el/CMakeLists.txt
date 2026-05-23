add_custom_command(
  OUTPUT tmc_el.sql.gz 
  COMMAND gzip -c tmc_el.sql > tmc_el.sql.gz )

# ================================================== enable for DE
add_custom_command(
  OUTPUT tmc_el2.sql.gz 
  COMMAND gzip -c tmc_el2.sql > tmc_el2.sql.gz )
  
add_custom_command(
  OUTPUT tmc_el_de_DE.db
  COMMAND ../../src/rds-db-import tmc_el_de_DE.db tmc_el2.sql de_DE/ )
# ================================================== enable for DE end

# those are contained in the software already
add_custom_command(
  OUTPUT tmc_el_en_CEN.db
  COMMAND ../../src/rds-db-import tmc_el_en_CEN.db tmc_el.sql en_CEN/ )

add_custom_command(
  OUTPUT tmc_el_en_GB.db
  COMMAND ../../src/rds-db-import tmc_el_en_GB.db tmc_el.sql en_GB/ )

add_custom_target(
    create_tmc_en_db ALL
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/tmc_el_en_CEN.db"
	"${CMAKE_CURRENT_BINARY_DIR}/tmc_el_en_GB.db"
# ================================================== enable for DE
	"${CMAKE_CURRENT_BINARY_DIR}/tmc_el_de_DE.db"
# ================================================== enable for DE end
)

