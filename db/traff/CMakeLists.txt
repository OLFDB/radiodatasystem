add_custom_command(
  OUTPUT "traff.db"
  COMMAND ${CMAKE_CURRENT_BINARY_DIR}/mk_evtlist "TraFF Event List.md"
  COMMAND ${CMAKE_CURRENT_BINARY_DIR}/mk_silist "TraFF SI List.md"
)

set(TO_BE_CLEANED 
	"${CMAKE_CURRENT_BINARY_DIR}/TraFF Event List.md.sql"
	"${CMAKE_CURRENT_BINARY_DIR}/TraFF SI List.md.sql"
)

set_property(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    APPEND PROPERTY ADDITIONAL_CLEAN_FILES "${TO_BE_CLEANED}"
)

add_custom_target(
    create_traff_db ALL
    DEPENDS 
	"${CMAKE_CURRENT_BINARY_DIR}/traff.db"
)

