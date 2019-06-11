find_package(Doxygen REQUIRED)
 
# Find all the public headers
get_target_property(yteSources YTE SOURCES)

list(FILTER yteSources INCLUDE REGEX ^.*\.h(pp)?$)

set(DOXYGEN_INPUT_DIR ${PROJECT_SOURCE_DIR}/CatCutifier)
set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/docs/doxygen)
set(DOXYGEN_INDEX_FILE ${DOXYGEN_OUTPUT_DIR}/html/index.html)
set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
 
#Replace variables inside @@ with the current values
configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

 #Doxygen won't create this for us
file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR}) 
add_custom_command(OUTPUT ${DOXYGEN_INDEX_FILE}
                   DEPENDS ${yteSources}
                   COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
                   MAIN_DEPENDENCY ${DOXYFILE_OUT} ${DOXYFILE_IN}
                   COMMENT "Generating docs")
 
add_custom_target(Doxygen ALL DEPENDS ${DOXYGEN_INDEX_FILE})