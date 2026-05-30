# set these options via -D (e.g. cmake . -DWITH_DE_LCL=ON)
option(WITH_DE_LCL "Create databases for de_DE" OFF)
option(WITH_IT_LCL "Create databases for it_IT" OFF)
option(WITH_SK_LCL "Create databases for dk_cz" OFF)
option(WITH_CZ_LCL "Create databases for cs_CZ" OFF)

set(MY_CUSTOM_DEPS "${CMAKE_CURRENT_BINARY_DIR}/tmc_lcl_00_0_00.db")

if(WITH_DE_LCL)
	list(APPEND MY_CUSTOM_DEPS "${CMAKE_CURRENT_BINARY_DIR}/tmc_lcl_E0_D_01.db")
endif()
if(WITH_IT_LCL)
    	list(APPEND MY_CUSTOM_DEPS "${CMAKE_CURRENT_BINARY_DIR}/tmc_lcl_E0_5_01.db")
endif()
if(WITH_SK_LCL)
    	list(APPEND MY_CUSTOM_DEPS "${CMAKE_CURRENT_BINARY_DIR}/tmc_lcl_E2_5_01.db")
endif()
if(WITH_CZ_LCL)
    	list(APPEND MY_CUSTOM_DEPS "${CMAKE_CURRENT_BINARY_DIR}/tmc_lcl_E2_2_01.db")
endif()


add_custom_command(
  OUTPUT tmc_lcl.sql.gz 
  COMMAND gzip -c tmc_lcl.sql > tmc_lcl.sql.gz )
 
#CEN/GB
add_custom_command(
  OUTPUT tmc_lcl_00_0_00.db
  COMMAND ../../src/rds-db-import tmc_lcl_00_0_00.db tmc_lcl.sql 00_0_00/ )

#DE
add_custom_command(
  OUTPUT tmc_lcl_E0_D_01.db
  COMMAND ../../src/rds-db-import tmc_lcl_E0_D_01.db tmc_lcl.sql E0_D_01/ )

#IT 
add_custom_command(
  OUTPUT tmc_lcl_E0_5_01.db
  COMMAND ../../src/rds-db-import tmc_lcl_E0_5_01.db tmc_lcl.sql E0_5_01/ )

#CZ
add_custom_command(
  OUTPUT tmc_lcl_E2_2_01.db
  COMMAND ../../src/rds-db-import tmc_lcl_E2_2_01.db tmc_lcl.sql E2_2_01/ )

#SK
add_custom_command(
  OUTPUT tmc_lcl_E2_5_01.db
  COMMAND ../../src/rds-db-import tmc_lcl_E2_5_01.db tmc_lcl.sql E2_5_01/ )

add_custom_target(
    create_tmc_lcl_db ALL
    	DEPENDS ${MY_CUSTOM_DEPS}

)

