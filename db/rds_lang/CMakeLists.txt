add_custom_command(
  OUTPUT rds_lang.sql.gz 
  COMMAND gzip -c rds_lang.sql > rds_lang.sql.gz )
  
add_custom_command(
  OUTPUT rds_lang_de_DE.db
  COMMAND ../../src/rds-db-import rds_lang_de_DE.db rds_lang.sql de_DE/ )
  
add_custom_command(
  OUTPUT rds_lang_en_CEN.db
  COMMAND ../../src/rds-db-import rds_lang_en_CEN.db rds_lang.sql en_CEN/ )

add_custom_command(
  OUTPUT rds_lang_en_US.db
  COMMAND ../../src/rds-db-import rds_lang_en_US.db rds_lang.sql en_US/ )

add_custom_target(
    create_rds_lang_db_de_DE ALL
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/rds_lang_de_DE.db"
     "${CMAKE_CURRENT_BINARY_DIR}/rds_lang_en_CEN.db"
     "${CMAKE_CURRENT_BINARY_DIR}/rds_lang_en_US.db"
)

