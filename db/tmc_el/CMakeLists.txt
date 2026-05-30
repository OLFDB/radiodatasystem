# set these options via -D (e.g. cmake . -DWITH_DE=ON)
option(WITH_DE_EL "Create databases for de_DE" OFF)
option(WITH_SK_EL "Create databases for sk_CZ" OFF)
option(WITH_CZ_EL "Create databases for cs_CZ" OFF)

set(MY_CUSTOM_DEPS "${CMAKE_CURRENT_BINARY_DIR}/tmc_el_en_CEN.db" "${CMAKE_CURRENT_BINARY_DIR}/tmc_el_en_GB.db")

if(WITH_DE_EL)
	list(APPEND MY_CUSTOM_DEPS "${CMAKE_CURRENT_BINARY_DIR}/tmc_el_de_DE.db")
endif()
if(WITH_CZ_EL)
	list(APPEND MY_CUSTOM_DEPS "${CMAKE_CURRENT_BINARY_DIR}/tmc_el_cs_CZ.db")
endif()
if(WITH_SK_EL)
	list(APPEND MY_CUSTOM_DEPS "${CMAKE_CURRENT_BINARY_DIR}/tmc_el_sk_CZ.db")
endif()

add_custom_command(
  OUTPUT tmc_el.sql.gz 
  COMMAND gzip -c tmc_el.sql > tmc_el.sql.gz )

add_custom_command(
  OUTPUT tmc_el2.sql.gz 
  COMMAND gzip -c tmc_el2.sql > tmc_el2.sql.gz )
  
add_custom_command(
  OUTPUT tmc_el_de_DE.db
  COMMAND ../../src/rds-db-import tmc_el_de_DE.db tmc_el2.sql de_DE/ )

# those are contained in the software already
add_custom_command(
  OUTPUT tmc_el_en_CEN.db
  COMMAND ../../src/rds-db-import tmc_el_en_CEN.db tmc_el.sql en_CEN/ )

add_custom_command(
  OUTPUT tmc_el_en_GB.db
  COMMAND ../../src/rds-db-import tmc_el_en_GB.db tmc_el.sql en_GB/ )

add_custom_target(
    create_tmc_en_db ALL
	DEPENDS ${MY_CUSTOM_DEPS}
)

